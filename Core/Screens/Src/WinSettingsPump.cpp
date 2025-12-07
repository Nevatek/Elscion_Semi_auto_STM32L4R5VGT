/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"

#define MAX_WASH_VOLUME (5)/*ML - Milliliter*/
#define MIN_WASH_VOLUME (0.5)/*ML - Milliliter*/
#define MAX_AIR_GAP_VOLUME (200)/*uL - Micro liter*/
#define MIN_AIR_GAP_VOLUME (10)/*uL - Micro liter*/
#define MAX_WASH_VOL_TXT_SIZE (32)
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
static float Wash_Volume = 0;
static float AirGap_Volume = 0;
static NexButton bBack = NexButton(en_WinId_SettingsPump , 1, "b5");
static NexButton bSave = NexButton(en_WinId_SettingsPump , 12, "b4");
static NexButton bUp = NexButton(en_WinId_SettingsPump , 7, "b1");
static NexButton bDown = NexButton(en_WinId_SettingsPump , 6, "b0");
static NexText tWashVol = NexText(en_WinId_SettingsPump, 5 , "t1");
static NexButton bAirGapUp = NexButton(en_WinId_SettingsPump , 11, "b3");
static NexButton bAirGapDown = NexButton(en_WinId_SettingsPump , 10, "b2");
static NexText tAirGapVol = NexText(en_WinId_SettingsPump, 9 , "t3");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bSave,
									 &bUp,
									 &bDown,
									 &bAirGapUp,
									 &bAirGapDown,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbSave(void *ptr);
static void HandlerbUp(void *ptr);
static void HandlerbDown(void *ptr);
static void HandlerbAirGapUp(void *ptr);
static void HandlerbAirGapDown(void *ptr);

enWindowStatus ShowSettingsPumpScreen (NexPage *ptr_obJCurrPage)
{
	char g_arrBuff[MAX_WASH_VOL_TXT_SIZE] = {0};
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bSave.attachPush(HandlerbSave, &bSave);
	bUp.attachPush(HandlerbUp, &bUp);
	bDown.attachPush(HandlerbDown, &bDown);
	bAirGapUp.attachPush(HandlerbAirGapUp, &bAirGapUp);
	bAirGapDown.attachPush(HandlerbAirGapDown, &bAirGapDown);
	/*Read settings buffer*/
//	ReadSettingsBuffer();
	Wash_Volume = objstcSettings.fWashVoleume_uL;
	AirGap_Volume = objstcSettings.fAirGapVol_uL;
	snprintf(g_arrBuff , MAX_WASH_VOL_TXT_SIZE - 1 , "%.1f",(Wash_Volume / 1000));
	tWashVol.setText(g_arrBuff);
	snprintf(g_arrBuff , MAX_WASH_VOL_TXT_SIZE - 1 , "%0.f",AirGap_Volume);
	tAirGapVol.setText(g_arrBuff);
	return WinStatus;
}

void HandlerSettingsPumpScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbAirGapUp(void *ptr)
{
	char g_arrBuff[MAX_WASH_VOL_TXT_SIZE] = {0};
	AirGap_Volume += float(10);
	if(MAX_AIR_GAP_VOLUME < AirGap_Volume)
	{
		AirGap_Volume = MAX_AIR_GAP_VOLUME;
		InstrumentBusyBuzz();
	}
	else
	{
		BeepBuzzer();
	}
	snprintf(g_arrBuff , MAX_WASH_VOL_TXT_SIZE - 1 , "%0.f",AirGap_Volume);
	tAirGapVol.setText(g_arrBuff);
}
void HandlerbAirGapDown(void *ptr)
{
	char g_arrBuff[MAX_WASH_VOL_TXT_SIZE] = {0};
	AirGap_Volume -= float(10);
	if(MIN_AIR_GAP_VOLUME > AirGap_Volume)
	{
		AirGap_Volume = MIN_AIR_GAP_VOLUME;
		InstrumentBusyBuzz();
	}
	else
	{
		BeepBuzzer();
	}
	snprintf(g_arrBuff , MAX_WASH_VOL_TXT_SIZE - 1 , "%0.f",AirGap_Volume);
	tAirGapVol.setText(g_arrBuff);

}
void HandlerbUp(void *ptr)
{
	char g_arrBuff[MAX_WASH_VOL_TXT_SIZE] = {0};
	Wash_Volume += float(0.5 * 1000);
	if((MAX_WASH_VOLUME * 1000) < Wash_Volume)
	{
		Wash_Volume = (MAX_WASH_VOLUME * 1000);
		InstrumentBusyBuzz();
	}
	else
	{
		BeepBuzzer();
	}
	snprintf(g_arrBuff , MAX_WASH_VOL_TXT_SIZE - 1 , "%.1f",(Wash_Volume / 1000));
	tWashVol.setText(g_arrBuff);

}
void HandlerbDown(void *ptr)
{
	char g_arrBuff[MAX_WASH_VOL_TXT_SIZE] = {0};
	Wash_Volume -= float(0.5 * 1000);
	if((MIN_WASH_VOLUME * 1000) > Wash_Volume)
	{
		Wash_Volume = (MIN_WASH_VOLUME * 1000);
		InstrumentBusyBuzz();
	}
	else
	{
		BeepBuzzer();
	}
	snprintf(g_arrBuff , MAX_WASH_VOL_TXT_SIZE - 1 , "%.1f",(Wash_Volume / 1000));
	tWashVol.setText(g_arrBuff);
}

void HandlerbSave(void *ptr)
{
	objstcSettings.fWashVoleume_uL = Wash_Volume;
	objstcSettings.fAirGapVol_uL = AirGap_Volume;
	WriteSettingsBuffer();
    if(enkeyOk == ShowMainPopUp("Pump Settings","Pump settings saved", true))
    {
    	ChangeWindowPage(en_WinId_SettingsPump , (enWindowID)NULL);
    	stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
    }
}
