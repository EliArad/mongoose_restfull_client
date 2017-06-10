#include "mongoose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HttpClientApi.h"
#include "nxjson.h"
#include <stdint.h>


static char serverUrl[250];
extern struct mg_mgr mgr;
extern struct mg_connection *nc;
static char *m_response;

static int s_exit_flag = 0;

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;
  int connect_status;

  switch (ev) {
    case MG_EV_CONNECT:
      printf("connect\n");
      connect_status = *(int *) ev_data;
      printf("Connect Status = %d\n", connect_status);
       if (connect_status != 0) {
        printf("Error connecting to %s: %s\n", serverUrl, strerror(connect_status));
        s_exit_flag = 1;
      }
      break;
    case MG_EV_HTTP_REPLY:
      printf("Got reply:\n%d , %s\n", (int) hm->body.len, hm->body.p);
      strcpy(m_response , hm->body.p);
      nc->flags |= MG_F_SEND_AND_CLOSE;
      s_exit_flag = 1;
      break;
    case MG_EV_CLOSE:
      if (s_exit_flag == 0) {
        printf("Server closed connection\n");
        s_exit_flag = 1;
      };
      break;
    default:
      break;
  }
}


void HTTP_CLIENT_SetServerUrl(char *url)
{
	strcpy(serverUrl, url);
	m_response = (char *)malloc (2000);
}
void HTTP_CLIENT_Close()
{
	free(m_response);
	m_response = NULL;
}

int HTTP_CLIENT_SendHTTP(char* api,
						 void *pData, 
						 char* pDataOut, 
						 unsigned int Timeout, 
						 int isJson,
						 char *access_token)
{

	//int32_t                   nRetCode = 0;
	char                      s_url[HC_STR_LENGTH];
	//const struct json_token *tok = NULL;

   (void)isJson;

	char headers [256];

	if (access_token != NULL)
	{
		sprintf(headers , "Content-Type:application/json\r\naccess_token: %s\r\n" , access_token);
	} else {
		 strcpy(headers , "Content-Type:application/json\r\n");
	}
	printf("headers %s\n" , headers);

	s_exit_flag  = 0;
	  //printf("json to send %s\n" , (char *)pData);
	strcpy((char *)&s_url, serverUrl);
	strncpy(&s_url[strlen(serverUrl)], api, (HC_STR_LENGTH - strlen(serverUrl)));


	nc =  mg_connect_http(&mgr, ev_handler, s_url, headers , pData);


	mg_set_protocol_http_websocket(nc);


	printf("Starting RESTful client against %s\n", s_url);

	//for (i = 0 ; i < 5; i++)
	int to = Timeout;
	while (s_exit_flag == 0)
	{
	  mg_mgr_poll(&mgr, 1000);
	  if (s_exit_flag == 1)
		  break;
	  usleep(100000);
	  if (to == 0)
		  break;
	  to--;
	  printf("timeout %d\n", to);

	}
	if (to == 0)
	{
	  printf("time out happend on url %s\n" , s_url);
	  return 0;
	}
	strcpy(pDataOut ,m_response);

	return 1;

	 
};

