/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

#define SOFTWARE_VERSION_MAJOR 1
#define SOFTWARE_VERSION_MINOR 1
#define SPACE_CHAR ' '

/*Definition of device name*/
#define DEVICE_NAME "Semi-Automatic Chemistry Analyzer"
/*Definition of model name*/
#define MODEL_NAME "ARIS 100"
/*Definition of model id*/
#define MODEL_ID "AS100"
#define SOFTWARE_VERSION  "EB_1.0_AS100_V_1.1.5"
/*definition of software version length in bytes*/
#define SOFT_VER_LENGTH (16)
char e_arr_cSoftwareVersionBuff[SOFT_VER_LENGTH];
static uint8_t Presscount = 0;

static NexButton bBack = NexButton(en_WinId_SettingsAbout , 2 , "b5");

static NexPicture OEMEditMode = NexPicture(en_WinId_SettingsAbout , 4, "p4");

static NexText tDeviceName = NexText(en_WinId_SettingsAbout, 11 , "t6");
static NexText tModelName = NexText(en_WinId_SettingsAbout, 10 , "t5");
static NexText tModelNum = NexText(en_WinId_SettingsAbout, 9 , "t4");
static NexText tSoftVerion = NexText(en_WinId_SettingsAbout, 12 , "t7");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &OEMEditMode,
									 NULL};
static void LoadBuildVersion(char caVersionString[]);
void AssignSoftwareVersion(void);
static void HandlerbBack(void *ptr);
static void HandlerOEMEditMode(void *ptr);

enWindowStatus ShowSettingsAboutScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

	bBack.attachPush(HandlerbBack, &bBack);
	OEMEditMode.attachPush(HandlerOEMEditMode, &OEMEditMode);
	Presscount = 0;
	/*Assign software version*/
	AssignSoftwareVersion();

	tDeviceName.setText(DEVICE_NAME);
	tSoftVerion.setText(SOFTWARE_VERSION);

	if(objstcSettings.Elscion_Model == NVM_OEM_ENABLE_FLAG)
	{
		tModelName.setText("ARIS 100");
		tModelNum.setText("AS100");
	}
	else if(objstcSettings.OEM1_Model == NVM_OEM_ENABLE_FLAG) // Labnova
	{
		tModelName.setText("LABCHEM 7 PRO");
		tModelNum.setText("LC-7PRO");
	}
	else if(objstcSettings.OEM2_Model == NVM_OEM_ENABLE_FLAG) //
	{
		tModelName.setText("RIVO CHEM 100");
		tModelNum.setText("RC100");
	}
	else if(objstcSettings.OEM3_Model == NVM_OEM_ENABLE_FLAG) //
	{
		tModelName.setText(" ");
		tModelNum.setText(" ");
	}
//	tSoftVerion.setText(e_arr_cSoftwareVersionBuff);
	return WinStatus;
}

void HandlerSettingsAboutScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
	Presscount = 0;
}
void HandlerOEMEditMode(void *ptr)
{
	Presscount = Presscount + 1;

	if(Presscount > 10)
	{
		ChangeWindowPage(en_WinId_OEMLogin , (enWindowID)NULL);
		Presscount = 0;
	}
}
void LoadBuildVersion(char caVersionString[])
{
   volatile char caDateString[30]=__DATE__;
   volatile char caTimeString[30]=__TIME__;
   uint8_t u_n8BuildMinute;   //To store Minutes value of Build time
   uint8_t u_n8BuildHour;     //To store Hour value of Build time
   uint8_t u_n8BuildDay;      //To store Day value of Build Date
   uint8_t u_n8BuildMonth;    //To store Months value of Build Date
   uint8_t u_n8BuildYear;     //To store Year value of Build Year
   u_n8BuildMinute  =   ((caTimeString[3]-'0')*10)+(caTimeString[4]-'0');
   u_n8BuildHour    =   ((caTimeString[0]-'0')*10)+(caTimeString[1]-'0');


   if(SPACE_CHAR == caDateString[4])
   {
         u_n8BuildDay= caDateString[5]-'0';
   }
   else
   {
         u_n8BuildDay=((caDateString[4]-'0')*10)+(caDateString[5]-'0');
   }

    u_n8BuildYear    =   ((caDateString[9]-'0')*10)+(caDateString[10]-'0');
   if(caDateString[0]=='J' && caDateString[1]=='a' && caDateString[2]=='n')
   {
        u_n8BuildMonth=1;
   }
   else if(caDateString[0]=='F' && caDateString[1]=='e' && caDateString[2]=='b')
   {
        u_n8BuildMonth=2;
   }
   else if(caDateString[0]=='M' && caDateString[1]=='a' && caDateString[2]=='r')
   {
        u_n8BuildMonth=3;
   }
   else if(caDateString[0]=='A' && caDateString[1]=='p' && caDateString[2]=='r')
   {
        u_n8BuildMonth=4;
   }
   else if(caDateString[0]=='M' && caDateString[1]=='a' && caDateString[2]=='y')
   {
        u_n8BuildMonth=5;
   }
   else if(caDateString[0]=='J' && caDateString[1]=='u' && caDateString[2]=='n')
   {
        u_n8BuildMonth=6;
   }
   else if(caDateString[0]=='J' && caDateString[1]=='u' && caDateString[2]=='l')
   {
        u_n8BuildMonth=7;
   }
   else if(caDateString[0]=='A' && caDateString[1]=='u' && caDateString[2]=='g')
   {
        u_n8BuildMonth=8;
   }
   else if(caDateString[0]=='S' && caDateString[1]=='e' && caDateString[2]=='p')
   {
        u_n8BuildMonth=9;
   }
   else if(caDateString[0]=='O' && caDateString[1]=='c' && caDateString[2]=='t')
   {
        u_n8BuildMonth=10;
   }
   else if(caDateString[0]=='N' && caDateString[1]=='o' && caDateString[2]=='v')
   {
        u_n8BuildMonth=11;
   }
   else if(caDateString[0]=='D' && caDateString[1]=='e' && caDateString[2]=='c')
   {
        u_n8BuildMonth=12;
   }
   else
   {
       u_n8BuildMonth=0;
   }//u_n8BuildMonth is generated by switching
   sprintf(caVersionString,"%02d%02d%02d%02d%02d",u_n8BuildYear,u_n8BuildMonth,u_n8BuildDay,u_n8BuildHour,u_n8BuildMinute);

}
void AssignSoftwareVersion(void)
{
    sprintf(&e_arr_cSoftwareVersionBuff[0],"%d.%d.",SOFTWARE_VERSION_MAJOR,SOFTWARE_VERSION_MINOR);
    LoadBuildVersion(&e_arr_cSoftwareVersionBuff[0]+strlen(&e_arr_cSoftwareVersionBuff[0]));
}
