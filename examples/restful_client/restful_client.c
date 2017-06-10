/*
 * Copyright (c) 2014 Cesanta Software Limited
 * All rights reserved
 */

#include "mongoose.h"

struct mg_mgr mgr;
struct mg_connection *nc;
#include "AcuteHttpClientApi.h"
#include "HttpClientApi.h"
#include <pthread.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "fifo.h"
#include "restful_client.h"


#define SIM_HOST 1

/* RESTful server host and request URI */
//static const char *s_url =
    //"https://acute-device.herokuapp.com/api/v1/acute/setup";
uint8_t fifoBuffer[1000];
static uint8_t host_result = 0;


typedef struct Header
{
	uint16_t opcode;
	uint16_t size;
} Header;

uint8_t m_running = 1;
uint8_t (*acuteFunctions[100])(uint8_t *buffer);

uint8_t fifoHandler_acute_MaintainenceMessage(uint8_t *buffer)
{
	buffer = buffer;
	for (int i = 0 ; i < 3; i++)
	{
		if (Acute_MaintainenceMessage() == 1)
			return 1;
	}
	return 0;
}


uint8_t fifoHandler_acute_RegimeExchange(uint8_t *buffer)
{
	ACUTE_Regime *regime = (ACUTE_Regime *)buffer;
	for (int i = 0 ; i < 3; i++)
	{
		if (Acute_RegimeExchange(*regime) == 1)
			return 1;
	}
	return 0;
}


uint8_t fifoHandler_acute_setup(uint8_t *buffer)
{

	printf("fifoHandler_acute_setup\n");

	ACUTE_Setup *setup = (ACUTE_Setup *)buffer;

	for (int i = 0 ; i < 3; i++)
	{
		if (Acute_Setup(*setup) == 1)
		{
			return 1;
		}
	}

	return 0;
}

#if SIM_HOST
void *host_loop(void *arg)
{

	arg = arg;

	while (m_running)
	{
		Fifo_Read (1,(uint8_t *)&host_result, 0, sizeof(host_result));
		printf("host_result = %d\n", host_result);
	}


	return NULL;
}
#endif


void* main_loop(void *arg)
{
	arg = arg;

	Header h;



	while (m_running == 1)
	{
		if (Fifo_Read (0,(uint8_t *)&h, 0, sizeof(Header)) == 0)
		{
			printf("error reading from fifo 0\n");
			exit(1);
		}
		printf("h opcode %d\n" , h.opcode);
		printf("h size %d\n" , h.size);
		if (h.size > 0)
		{
			if (Fifo_Read(0,fifoBuffer, 0 , h.size) == 0)
			{
				printf("error reading from fifo 0\n");
				exit(1);
			}
		}
		uint8_t res = acuteFunctions[h.opcode](fifoBuffer);
		printf("acute function endded\n");
		if (Fifo_Write(1,&res , 0 , sizeof(uint8_t)) == 0)
		{
			printf("error writing to fifo 1\n");
			exit(1);
		}
	}



    return NULL;
}

void Initialize()
{
	acuteFunctions[ACUTE_SETUP] = &fifoHandler_acute_setup;
	acuteFunctions[ACUTE_MAINTAINENCE] = &fifoHandler_acute_MaintainenceMessage;
	acuteFunctions[ACUTE_REGIME_EXCHANGE] = &fifoHandler_acute_RegimeExchange;
}

int main(void)
{

	Header h;
    mg_mgr_init(&mgr, NULL);
    Initialize();
	CFifo();

    if (Fifo_Open(0, "/mnt/appfs/webclientfifo", 3) == 0)
	{
		printf("error creating fifo webclientfifo\n");
		return 1;
	}

	if (Fifo_Open(1 ,"/mnt/appfs/webclientfifo2", 3) == 0)
	{
		printf("error creating fifo webclientfifo\n");
		return 1;
	}

	HTTP_CLIENT_SetServerUrl("https://acute-device.herokuapp.com");
	//HTTP_CLIENT_SetServerUrl("http://localhost");

#if SIM_HOST
	pthread_t host_tid = -1;
	pthread_create(&host_tid, NULL, &host_loop, NULL);
#endif

	pthread_t main_tid = -1;
	if (pthread_create(&main_tid, NULL, &main_loop, NULL) == 0)
	{

#if SIM_HOST
		ACUTE_Setup setup;
		strcpy(setup.dev.serialNumber, "abcdef-12354654");
		strcpy(setup.dev.version, "0.5.0");
		strcpy(setup.dev.timeZone, "Europe / Prague");

		strcpy(setup.patient_identifier, "123");

		strcpy(setup.site.name, "Hospital NY");
		strcpy(setup.site.ward, "Left wing");

		setup.regime.DrugGroupIdentify = 0x10;
		strcpy(setup.regime.DragName, "Oxygesic");
		setup.regime.Dosage = 10;
		setup.regime.Pillsperfullblister = 10;
		setup.regime.PatientLockout.hour = 4;
		setup.regime.PatientLockout.minutes = 0;
		setup.regime.PatientMax8h = 3;
		setup.regime.NurseLockout.hour = 2;
		setup.regime.NurseLockout.minutes = 0;
		setup.regime.NurseMax8h = 4;
		strcpy(setup.state, "active");

		h.opcode = ACUTE_SETUP;
		h.size = sizeof(setup);

		host_result = 2;
		if (Fifo_Write(0,(uint8_t *)&h ,0, sizeof(Header)) == 0)
		{
			printf("error fifo write 1\n");
			exit(1);
		}
		if (Fifo_Write(0,(uint8_t *)&setup,0, sizeof(setup)) == 0)
		{
			printf("error fifo write 1\n");
			exit(1);
		}
		while (host_result == 2)
		{
			usleep(1000);
		}

		h.opcode = ACUTE_MAINTAINENCE;
		h.size = 0;
		host_result = 2;
		Fifo_Write(0,(uint8_t *)&h ,0, sizeof(Header));
		while (host_result == 2)
		{
			usleep(1000);
		}


		ACUTE_Regime  ar;
		ar.Dosage = 10;
		strcpy(ar.DragName ,"Eli");
		ar.DrugGroupIdentify = 0x10;
		ar.NurseLockout.hour = 2;
		ar.NurseLockout.minutes = 1;
		ar.NurseMax8h = 10;
		ar.PatientLockout.hour = 3;
		ar.PatientLockout.minutes = 0;
		ar.PatientMax8h = 2;
		ar.Pillsperfullblister = 10;
		h.opcode = ACUTE_MAINTAINENCE;
		h.size = sizeof(ACUTE_Regime);;
		host_result = 2;
		Fifo_Write(0,(uint8_t *)&h ,0, sizeof(Header));
		Fifo_Write(0,(uint8_t *)&ar ,0, sizeof(ACUTE_Regime));
		h.size = sizeof(ACUTE_Regime);
		while (host_result == 2)
		{
			usleep(1000);
		}
		printf("finished test\n");

#endif

		pthread_join(main_tid, NULL);
	}else {
		printf("error creating thread\n");
		exit(1);
	}

	Acute_Close();
    mg_mgr_free(&mgr);

  return 0;
}
