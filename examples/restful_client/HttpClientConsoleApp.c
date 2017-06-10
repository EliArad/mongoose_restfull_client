// HttpClientConsoleApp.cpp : Defines the entry point for the console application.
//
#include "HTTPClientCommon.h"
#include "HttpClientApi.h"
#include <stdlib.h>
#include <stdio.h>
#define _CRT_SECURE_NO_WARNINGS			// stop complaining about deprecated functions
#include "jWrite.h"
#ifdef WIN32
#include <windows.h>
#endif 

#include "AcuteHttpClientApi.h"

int main(int argc, char* argv[])
{
	 
 

 
#ifdef WIN32
	// Windows Specific - Sockets initialization 
	unsigned short      wVersionRequested;
	WSADATA             wsaData;
	UINT32              nErr = 0;
	// We want to use Winsock v 1.2 (can be higher)
	wVersionRequested = MAKEWORD(1, 2);
	nErr = WSAStartup(wVersionRequested, &wsaData);
	if (nErr != 0) {
		printf("WSAStartup failed with error: %d\n", nErr);
		return 1;
	}
#endif 

 
	HTTP_CLIENT_SetServerUrl("http://acute-device.herokuapp.com");
	//HTTP_CLIENT_SetServerUrl("http://localhost");
	ACUTE_Setup setup;
	strcpy(setup.dev.serialNumber, "abcdef-12354654");
	strcpy(setup.dev.version, "0.5.0");
	strcpy(setup.dev.timeZone, "Europe / Prague");
	
	strcpy(setup.patient_identifier, "123");
	
	strcpy(setup.site.name, "Hospital NY");
	strcpy(setup.site.ward, "Left wing");

	strcpy(setup.regime.DrugGroupIdentify, "Ox10");
	strcpy(setup.regime.DragName, "Oxygesic");
	strcpy(setup.regime.Dosage, "10mg");
	setup.regime.Pillsperfullblister = 10;
	setup.regime.PatientLockout.hour = 4;
	setup.regime.PatientLockout.minutes = 0;	
	setup.regime.PatientMax8h = 3;
	setup.regime.NurseLockout.hour = 2;
	setup.regime.NurseLockout.minutes = 0;
	setup.regime.NurseMax8h = 4;	
	strcpy(setup.state, "active");
	

	Acute_Setup(setup);
	Acute_Message();
	 
	WSACleanup();
	return 1;
}

