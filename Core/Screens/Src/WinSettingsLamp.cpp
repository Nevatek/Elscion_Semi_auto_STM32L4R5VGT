/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"

#define MAX_LAMP_DELAY_TIME (15)/*Minute*/
#define MIN_LAMP_DELAY_TIME (5)/*Minute*/
#define MAX_LAMP_DELAY_TIME_TXT_SIZE (32)
char g_arrLampDelayTxt[MAX_LAMP_DELAY_TIME_TXT_SIZE];
static uint16_t LampDelayTime = 0;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_SettingsLamp , 1, "b5");
static NexButton bSave = NexButton(en_WinId_SettingsLamp , 8, "b4");
static NexText tLampDelay = NexText(en_WinId_SettingsLamp, 5 , "t1");
static NexButton bUp = NexButton(en_WinId_SettingsLamp , 7, "1");
static NexButton bDown = NexButton(en_WinId_SettingsLamp , 6, "b0");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bSave,
									 &bUp,
									 &bDown,
									 NULL};


static void HandlerbBack(void *ptr);
static void HandlerbSave(void *ptr);
static void HandlerbUp(void *ptr);
static void HandlerbDown(void *ptr);

enWindowStatus ShowSettingsLampScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bSave.attachPush(HandlerbSave, &bSave);
	bUp.attachPush(HandlerbUp, &bUp);
	bDown.attachPush(HandlerbDown, &bDown);

	/*Read settings buffer*/
//	ReadSettingsBuffer();
	LampDelayTime = objstcSettings.u16LampDelayTime;
	snprintf(g_arrLampDelayTxt , MAX_LAMP_DELAY_TIME_TXT_SIZE - 1 , "%d Min",LampDelayTime);
	tLampDelay.setText(g_arrLampDelayTxt);
	return WinStatus;
}

void HandlerSettingsLampScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

static void HandlerbUp(void *ptr)
{
	LampDelayTime += uint8_t(5);
	if(MAX_LAMP_DELAY_TIME < LampDelayTime)
	{
		LampDelayTime = MAX_LAMP_DELAY_TIME;
		InstrumentBusyBuzz();
	}
	else
	{
		BeepBuzzer();
	}
	snprintf(g_arrLampDelayTxt , MAX_LAMP_DELAY_TIME_TXT_SIZE - 1 , "%d Min",LampDelayTime);
	tLampDelay.setText(g_arrLampDelayTxt);
}

static void HandlerbDown(void *ptr)
{
	LampDelayTime -= uint8_t(5);
	if(MIN_LAMP_DELAY_TIME > LampDelayTime)
	{
		LampDelayTime = MIN_LAMP_DELAY_TIME;
		InstrumentBusyBuzz();
	}
	else
	{
		BeepBuzzer();
	}
	snprintf(g_arrLampDelayTxt , MAX_LAMP_DELAY_TIME_TXT_SIZE - 1 , "%d Min",LampDelayTime);
	tLampDelay.setText(g_arrLampDelayTxt);
}
void HandlerbSave(void *ptr)
{
	objstcSettings.u16LampDelayTime = LampDelayTime;
	WriteSettingsBuffer();
	ConfigAutoSleepTimer(objstcSettings.u16LampDelayTime);
    if(enkeyOk == ShowMainPopUp("Lamp Settings","Lamp settings saved", true))
    {
    	ChangeWindowPage(en_WinId_SettingsLamp , (enWindowID)NULL);
    	stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
    }
}
