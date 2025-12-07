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
#define SETT_DATE_TIME_MAX_LENGTH_BUFFER (2)

RTC_DateTypeDef g_sDate;
RTC_TimeTypeDef g_sTime;
static char arrBuff[64];
static char arrTimeBuff[64];
static uint16_t Hour = 0;
static uint16_t Minute = 0;

static NexButton bBack = NexButton(en_WinId_SettingsDateTime , 1, "b5");
static NexButton bSave = NexButton(en_WinId_SettingsDateTime , 12, "b4");

static NexText tDate = NexText(en_WinId_SettingsDateTime, 6 , "t5");
//static NexText tMonth = NexText(en_WinId_SettingsDateTime, 8 , "t4");
//static NexText tYear = NexText(en_WinId_SettingsDateTime, 10 , "t6");
static NexText tHour = NexText(en_WinId_SettingsDateTime, 10 , "t14");
static NexText tMinute = NexText(en_WinId_SettingsDateTime, 9 , "t13");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									&tDate,
//									&tMonth,
//									&tYear,
									&tHour,
									&bSave,
									&tMinute,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbSave(void *ptr);
static void HandlerbDate(void *ptr);
//static void HandlerbMonth(void *ptr);
//static void Handlerbyear(void *ptr);
static void HandlerbHour(void *ptr);
static void HandlerbMinute(void *ptr);
static bool NonblockingSettingMillsDelay(void);

enWindowStatus ShowSettingsDateTimeScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

	bBack.attachPush(HandlerbBack, &bBack);
	tDate.attachPush(HandlerbDate, &tDate);
	tHour.attachPush(HandlerbHour, &tHour);
	tMinute.attachPush(HandlerbMinute, &tMinute);
	bSave.attachPush(HandlerbSave, &bSave);

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_NumericKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)/*Resets variables only if we are selecting screen not from numeric keyboard screen*/
	{
		GetCurrentDate(&g_sDate);
		GetCurrentTime(&g_sTime);
		snprintf(arrBuff , 63 , "%02d/%02d/%02d" , g_sDate.Date, g_sDate.Month, g_sDate.Year);

		Hour = g_sTime.Hours;
		Minute = g_sTime.Minutes;
	}
	tDate.setText(arrBuff);
	snprintf(arrTimeBuff , 63 , "%02d" ,Hour);
	tHour.setText(arrTimeBuff);
	snprintf(arrTimeBuff , 63 ,  "%02d" ,Minute);
	tMinute.setText(arrTimeBuff);

	return WinStatus;
}

void HandlerSettingsDateTimeScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
//	if(NonblockingSettingMillsDelay())
//	{
//		GetCurrentDate(&g_sDate);
//		GetCurrentTime(&g_sTime);
//
//		snprintf(arrBuff , 63 , "%02d/%02d/%02d" , g_sDate.Date, g_sDate.Month, g_sDate.Year);
//		tDate.setText(arrBuff);
//
//		tHour.setText(std::to_string(g_sTime.Hours).c_str());
//		tMinute.setText(std::to_string(g_sTime.Minutes).c_str());
//	}
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbDate(void *ptr)
{
	DateEntry = true;
	openKeyBoard(en_NumericKeyboard , arrBuff, sizeof(arrBuff) , false ,
			"Date (DD/MM/YY)" , arrBuff , strlen("DD/MM/YY"));
	BeepBuzzer();

}
void HandlerbHour(void *ptr)
{
	openKeyBoard(en_NumericKeyboard ,(uint16_t*)&Hour , sizeof(Hour) ,false ,
			"Hour" , "00", SETT_DATE_TIME_MAX_LENGTH_BUFFER);
	BeepBuzzer();
}
void HandlerbMinute(void *ptr)
{
	openKeyBoard(en_NumericKeyboard ,(uint16_t*)&Minute , sizeof(Minute) ,false ,
			"Minute" , "00" , SETT_DATE_TIME_MAX_LENGTH_BUFFER);
	BeepBuzzer();
}
void HandlerbSave(void *ptr)
{
	BeepBuzzer();
    std::string strDate(&arrBuff[0], 2);
    std::string strMonth(&arrBuff[3], 2);
    std::string strYear(&arrBuff[6], 2);

    g_sDate.Date =  atoi(strDate.c_str());
    g_sDate.Month =  atoi(strMonth.c_str());
    g_sDate.Year =  atoi(strYear.c_str());
    g_sTime.Hours = Hour;
    g_sTime.Minutes = Minute;

    if( g_sDate.Date == 0 || g_sDate.Month == 0 ||  g_sDate.Year == 0)
    {
		if(enkeyOk == ShowMainPopUp("Date","Fields should not be zero", true))
		{
			ChangeWindowPage(en_WinId_SettingsDateTime , (enWindowID)NULL);
			stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
		}
    }
    else if(g_sDate.Date > 31 || g_sDate.Month > 12 ||  g_sDate.Year > 99)
    {
		if(enkeyOk == ShowMainPopUp("Date","Invalid Date", true))
		{
			ChangeWindowPage(en_WinId_SettingsDateTime , (enWindowID)NULL);
			stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
		}
    }
    else if( g_sTime.Hours > 23 || g_sTime.Minutes > 59)
    {
		if(enkeyOk == ShowMainPopUp("Time","Invalid Time", true))
		{
			ChangeWindowPage(en_WinId_SettingsDateTime , (enWindowID)NULL);
			stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
		}
    }
    else
    {
    	SetCurrentDate(g_sDate.Year , g_sDate.Month , g_sDate.Date);
    	SetCurrentTime(g_sTime.Hours , g_sTime.Minutes);
		if(enkeyOk == ShowMainPopUp("Date & Time","Date and Time saved", true))
		{
			ChangeWindowPage(en_WinId_SettingsDateTime , (enWindowID)NULL);
			stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
		}
    }
}
bool NonblockingSettingMillsDelay(void)
{
  #define DELAY_MS (500)
  unsigned long currentMillis = HAL_GetTick();
  static unsigned long PrevMills = 0;/*Initilized one time only*/
  int elapsedTime = (int)(currentMillis - PrevMills);
  if(abs((int)elapsedTime) >= DELAY_MS)
  {
   PrevMills = currentMillis;
   return true;
  }
  return false;
}
