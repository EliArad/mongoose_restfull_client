
#include "mongoose.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define _CRT_SECURE_NO_WARNINGS			// stop complaining about deprecated functions
#include "jWrite.h"
#ifdef WIN32
#include <windows.h>
#endif 
#include "AcuteHttpClientApi.h"
#include "HttpClientApi.h"
#include "nxjson.h"

char buffer[2048];
unsigned int buflen = 2048;
char access_token[512];


int Acute_Setup(ACUTE_Setup setup)
{
	
	char response[256] = "";
	char api[256] = "/api/v1/acute/setup";
	int ret_code = HTTP_CLIENT_SUCCESS;
	char temp[256];
	
	//int err;
	memset(access_token, 0, sizeof(access_token));
	
	jwOpen(buffer, buflen, JW_OBJECT, JW_PRETTY);		 

		jwObj_object("device");					 
			jwObj_string("version", setup.dev.version);
			jwObj_string("serial_number", setup.dev.serialNumber);
			jwObj_string("timezone", setup.dev.timeZone);
		jwEnd();
		jwObj_string("patient_identifier", setup.patient_identifier);				 

		jwObj_object("site");				 
			jwObj_string("name", setup.site.name);
			jwObj_string("ward", setup.site.ward);
		jwEnd();

		jwObj_object("regime");
			jwObj_int("Drug Group Identify", setup.regime.DrugGroupIdentify);
			jwObj_string("Drag Name", setup.regime.DragName);
			jwObj_int("Dosage", setup.regime.Dosage);
			sprintf(temp, "%d pills", setup.regime.Pillsperfullblister);
			jwObj_string("Pills per full blister", temp);
			sprintf(temp, "%d:%d", setup.regime.PatientLockout.hour, setup.regime.PatientLockout.minutes);
			jwObj_string("Patient Lockout", temp);
			sprintf(temp, "%d", setup.regime.PatientMax8h);
			jwObj_string("Patient Max 8h", temp);
			sprintf(temp, "%d:%d", setup.regime.NurseLockout.hour, setup.regime.NurseLockout.minutes);
			jwObj_string("Nurse Lockout", temp);
			sprintf(temp, "%d", setup.regime.NurseMax8h);
			jwObj_string("Nurse Max 8h", temp);
		jwEnd(); 
		jwObj_string("state", setup.state);
	 
	jwEnd();
	jwClose( );

	ret_code = HTTP_CLIENT_SendHTTP(api, buffer, response, HC_HTTP_TIMEOUT, 1, NULL);
	if (ret_code != HTTP_CLIENT_SUCCESS)
	{
		printf("http action retruned with %d\n", ret_code);
		return ret_code;
	}


	int res = 0;
	if (strcmp(response , "Successful connection") != 0)
	{
		res = 0;
	}
	const nx_json* nxjsonParser =nx_json_parse(response, 0);
	if (nxjsonParser) {
	  //printf("some-int=%ld\n", nx_json_get(json, "some-int")->int_value);
	  printf("access_token =%s\n", nx_json_get(nxjsonParser, "access_token")->text_value);
	  strcpy(access_token , nx_json_get(nxjsonParser, "access_token")->text_value);
	  nx_json_free(nxjsonParser);
	  res = 1;
	}



	return res;
  
}


int Acute_MaintainenceMessage()
{

	char response[256] = "";
	char api[256] = "/api/v1/acute/data";
	int ret_code = HTTP_CLIENT_SUCCESS;

	jwOpen(buffer, buflen, JW_OBJECT, JW_PRETTY);
		jwObj_string("source", "device");
		jwObj_string("destination", "cloud");
	
		jwObj_array("messages");					// array with elements
			jwArr_object(); 
				jwObj_string("generated_at", "2016-09-17T21:39:11.255Z");
				jwObj_int("msg_number", 1);
				jwObj_string("msg_type", "MAINTANANCE");
				jwObj_object("data");
					jwObj_string("battery" , "good");
					jwObj_int("voltage_in", 9);
					jwObj_string("malfunction", "door");
					jwObj_string("event_time", "2016 - 09 - 17T21:39 : 11.255Z");	
				jwEnd();
			jwEnd();
		jwEnd();
	jwEnd();
	jwClose( );


	ret_code = HTTP_CLIENT_SendHTTP(api, buffer, response, HC_HTTP_TIMEOUT, 1, access_token);
	if (ret_code != HTTP_CLIENT_SUCCESS)
	{
		printf("http action retruned with %d\n", ret_code);
		return ret_code;
	}

	return 1;
}

int Acute_RegimeExchange(ACUTE_Regime regime)
{
	char api[256] = "/api/v1/acute/data";
	char response[256] = "";
	int ret_code = HTTP_CLIENT_SUCCESS;
	char temp[256];
	jwOpen(buffer, buflen, JW_OBJECT, JW_PRETTY);
		jwObj_object("regime");
			jwObj_int("Drug Group Identify", regime.DrugGroupIdentify);
			jwObj_string("Drag Name", regime.DragName);
			jwObj_int("Dosage", regime.Dosage);
			sprintf(temp, "%d pills", regime.Pillsperfullblister);
			jwObj_string("Pills per full blister", temp);
			sprintf(temp, "%d:%d", regime.PatientLockout.hour, regime.PatientLockout.minutes);
			jwObj_string("Patient Lockout", temp);
			sprintf(temp, "%d", regime.PatientMax8h);
			jwObj_string("Patient Max 8h", temp);
			sprintf(temp, "%d:%d", regime.NurseLockout.hour, regime.NurseLockout.minutes);
			jwObj_string("Nurse Lockout", temp);
			sprintf(temp, "%d", regime.NurseMax8h);
			jwObj_string("Nurse Max 8h", temp);
		jwEnd();
	jwClose( );

	ret_code = HTTP_CLIENT_SendHTTP(api, buffer, response, HC_HTTP_TIMEOUT, 1, access_token);
	if (ret_code != HTTP_CLIENT_SUCCESS)
	{
		printf("http action retruned with %d\n", ret_code);
		return ret_code;
	}

	return 1;
}

void Acute_GetEventTime(char *jsTimeLike)
{
	time_t mytime = time(0);
	struct tm* tm_ptr = localtime(&mytime);



	if (tm_ptr)
	{
		sprintf(jsTimeLike, "%d-%d-%dT%d:%d:%d.%dZ" , tm_ptr->tm_year, tm_ptr->tm_mon, tm_ptr->tm_mday, tm_ptr->tm_hour,tm_ptr->tm_min, tm_ptr->tm_sec, 0);
	}


}

int Acute_PaintRating(int painRating)
{
	char time[150];
	Acute_GetEventTime(time);
	int ret_code = HTTP_CLIENT_SUCCESS;
	char response[256] = "";
	char api[256] = "/api/v1/acute/data";

	jwOpen(buffer, buflen, JW_OBJECT, JW_PRETTY);
		jwObj_string("source", "device");
		jwObj_string("destination", "cloud");

		jwObj_array("messages");					// array with elements
			jwArr_object();
				jwObj_string("generated_at", time);
				jwObj_int("msg_number", 1);
				jwObj_string("msg_type", "PAIN_RATING");
				jwObj_object("data");
					jwObj_string("event_time" , time);
					jwObj_int("rating", painRating);
				jwEnd();
			jwEnd();
		jwEnd();
	jwEnd();
	jwClose( );

	ret_code = HTTP_CLIENT_SendHTTP(api, buffer, response, HC_HTTP_TIMEOUT, 1, access_token);
	if (ret_code != HTTP_CLIENT_SUCCESS)
	{
		printf("http action retruned with %d\n", ret_code);
		return ret_code;
	}


	return 1;

}
int Acute_TherapyStarted()
{
	char api[256] = "/api/v1/acute/data";
	char response[256] = "";
	int ret_code = HTTP_CLIENT_SUCCESS;
	ret_code = HTTP_CLIENT_SendHTTP(api, buffer, response, HC_HTTP_TIMEOUT, 1, access_token);
	if (ret_code != HTTP_CLIENT_SUCCESS)
	{
		printf("http action retruned with %d\n", ret_code);
		return ret_code;
	}

	return 1;
}
int Acute_PillIntake()
{

	return 1;
}
int Acute_ExtraPillIntake()
{
	char api[256] = "/api/v1/acute/data";
	char response[256] = "";
	int ret_code = HTTP_CLIENT_SUCCESS;
	ret_code = HTTP_CLIENT_SendHTTP(api, buffer, response, HC_HTTP_TIMEOUT, 1, access_token);
	if (ret_code != HTTP_CLIENT_SUCCESS)
	{
		printf("http action retruned with %d\n", ret_code);
		return ret_code;
	}


	return 1;

}
int Acute_PillRequest()
{
	char api[256] = "/api/v1/acute/data";
	char response[256] = "";
	int ret_code = HTTP_CLIENT_SUCCESS;
	ret_code = HTTP_CLIENT_SendHTTP(api, buffer, response, HC_HTTP_TIMEOUT, 1, access_token);
	if (ret_code != HTTP_CLIENT_SUCCESS)
	{
		printf("http action retruned with %d\n", ret_code);
		return ret_code;
	}


	return 1;
}
int Acute_WrisbandReplace()
{
	char api[256] = "/api/v1/acute/data";
	char response[256] = "";
	char time[150];
	Acute_GetEventTime(time);

	jwOpen(buffer, buflen, JW_OBJECT, JW_PRETTY);
		jwObj_string("source", "device");
		jwObj_string("destination", "cloud");

		jwObj_array("messages");					// array with elements
			jwArr_object();
				jwObj_string("generated_at", time);
				jwObj_int("msg_number", 1);
				jwObj_string("msg_type", "WRISTBAND_REPLACE");
				jwObj_object("data");
					jwObj_string("event_time" , time);
				jwEnd();
			jwEnd();
		jwEnd();
	jwEnd();
	jwClose( );



	int ret_code = HTTP_CLIENT_SUCCESS;
	ret_code = HTTP_CLIENT_SendHTTP(api, buffer, response, HC_HTTP_TIMEOUT, 1, access_token);
	if (ret_code != HTTP_CLIENT_SUCCESS)
	{
		printf("http action retruned with %d\n", ret_code);
		return ret_code;
	}

	return 1;
}
int Acute_BlisterExchange()
{
	char api[256] = "/api/v1/acute/data";
	char response[256] = "";
	char time[150];
	Acute_GetEventTime(time);

	jwOpen(buffer, buflen, JW_OBJECT, JW_PRETTY);
		jwObj_string("source", "device");
		jwObj_string("destination", "cloud");

		jwObj_array("messages");					// array with elements
			jwArr_object();
				jwObj_string("generated_at", time);
				jwObj_int("msg_number", 1);
				jwObj_string("msg_type", "BLISTER_EXCHANGE");
				jwObj_object("data");
					jwObj_string("event_time" , time);
				jwEnd();
			jwEnd();
		jwEnd();
	jwEnd();
	jwClose( );


	int ret_code = HTTP_CLIENT_SUCCESS;
	ret_code = HTTP_CLIENT_SendHTTP(api, buffer, response, HC_HTTP_TIMEOUT, 1, access_token);
	if (ret_code != HTTP_CLIENT_SUCCESS)
	{
		printf("http action retruned with %d\n", ret_code);
		return ret_code;
	}

	return 1;
}
int Acute_TherapyEnd()
{
	char api[256] = "/api/v1/acute/data";
	char response[256] = "";
	char time[150];
	Acute_GetEventTime(time);

	jwOpen(buffer, buflen, JW_OBJECT, JW_PRETTY);
		jwObj_string("source", "device");
		jwObj_string("destination", "cloud");

		jwObj_array("messages");					// array with elements
			jwArr_object();
				jwObj_string("generated_at", time);
				jwObj_int("msg_number", 1);
				jwObj_string("msg_type", "THERAPY_END");
				jwObj_object("data");
					jwObj_string("event_time" , time);
				jwEnd();
			jwEnd();
		jwEnd();
	jwEnd();
	jwClose( );


	int ret_code = HTTP_CLIENT_SUCCESS;
	ret_code = HTTP_CLIENT_SendHTTP(api, buffer, response, HC_HTTP_TIMEOUT, 1, access_token);
	if (ret_code != HTTP_CLIENT_SUCCESS)
	{
		printf("http action retruned with %d\n", ret_code);
		return ret_code;
	}

	return 1;
}



void Acute_Close()
{
	HTTP_CLIENT_Close();
}
