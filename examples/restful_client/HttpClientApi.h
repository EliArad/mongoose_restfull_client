#ifndef HTTP_CLIENT_API_H
#define HTTP_CLIENT_API_H


#define HC_STR_LENGTH 		256
#define HC_HTTP_TIMEOUT  	5
#define HC_BUFFER_SIZE   	1024
#define MD5_DIGEST_LENGTH   16
#define HC_FILE_SIZE        50000
#define HTTP_CLIENT_SUCCESS      1


void HTTP_CLIENT_Close();
void HTTP_CLIENT_SetServerUrl(char *url);
int  HTTP_CLIENT_SendHTTP(char* api, void *pData,
						  char* pDataOut,
						  unsigned int Timeout,
						  int isJson,
						  char *access_token);


#endif 
