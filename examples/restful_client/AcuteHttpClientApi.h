#ifndef ACUTE_HTTP_CLIENT_API_H
#define ACUTE_HTTP_CLIENT_API_H

typedef struct _ACUTE_Device
{
	char version[50];
	char serialNumber[100];
	char timeZone[100];
} ACUTE_Device;

typedef struct _ACUTE_Site
{
	char name[100];
	char ward[100];
} ACUTE_Site;

typedef struct _ACUTE_Time
{
	int hour;
	int minutes;
} ACUTE_Time;

typedef struct _ACUTE_Regime
{
	int DrugGroupIdentify;
	char DragName[100];
	int Dosage;
	int Pillsperfullblister;
	ACUTE_Time PatientLockout;
	int PatientMax8h;
	ACUTE_Time NurseLockout;
	int NurseMax8h;
} ACUTE_Regime;

typedef struct _ACUTE_Setup
{
	ACUTE_Device dev;
	char patient_identifier[100];
	ACUTE_Site site;
	ACUTE_Regime regime;
	char state[20];
} ACUTE_Setup;



#define MSG_TYPE_MAINTANANCE  "MAINTANANCE"
#define MSG_TYPE_THERAPY_STARTED	 "THERAPY_STARTED"	
#define MSG_TYPE_PILL_INTAKE		 "PILL_INTAKE"		
#define MSG_TYPE_EXTRA_PILL_INTAKE	 "EXTRA_PILL_INTAKE"
#define MSG_TYPE_PILL_REQUEST		 "PILL_REQUEST"	
#define MSG_TYPE_PAIN_RATING		"PAIN_RATING"			
#define MSG_TYPE_WRISTBAND_REPLACE   "WRISTBAND_REPLACE"		
#define MSG_TYPE_BLISTER_EXCHANGE	 "BLISTER_EXCHANGE"
#define MSG_TYPE_THERAPY_END		 "THERAPY_END"

int Acute_PaintRating(int painRating);
int Acute_TherapyStarted();
int Acute_PillIntake();
int Acute_ExtraPillIntake();
int Acute_PillRequest();
int Acute_WrisbandReplace();
int Acute_BlisterExchange();
int Acute_TherapyEnd();
void Acute_GetEventTime(char *jsTimeLike);
int Acute_MaintainenceMessage();
int Acute_Setup(ACUTE_Setup setup);
int Acute_RegimeExchange(ACUTE_Regime regime);

void Acute_Close();

#endif 
