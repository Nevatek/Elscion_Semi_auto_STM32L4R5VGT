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

static NexButton bSettingsBack = NexButton(en_WinId_SystemsettingsScreen , 1, "b5");
static NexButton bSettReset = NexButton(en_WinId_SystemsettingsScreen , 28, "b2");

static NexButton bAbout = NexButton(en_WinId_SystemsettingsScreen , 16 , "t0");
static NexButton bDateTime = NexButton(en_WinId_SystemsettingsScreen , 17, "t1");
static NexButton bAdmin = NexButton(en_WinId_SystemsettingsScreen , 18, "t2");
static NexButton bUserSetup = NexButton(en_WinId_SystemsettingsScreen , 19, "t3");

static NexButton bPump = NexButton(en_WinId_SystemsettingsScreen , 21 , "t5");
static NexButton bPrinter = NexButton(en_WinId_SystemsettingsScreen , 22, "t6");
static NexButton bLamp = NexButton(en_WinId_SystemsettingsScreen , 23, "t7");
static NexButton bBuzzer = NexButton(en_WinId_SystemsettingsScreen , 20, "t4");
static NexButton bSysParams = NexButton(en_WinId_SystemsettingsScreen , 25, "t9");
static NexButton bReset = NexButton(en_WinId_SystemsettingsScreen , 26, "t10");
static NexButton bAutoWash = NexButton(en_WinId_SystemsettingsScreen , 27, "t11");
static NexButton bMemory = NexButton(en_WinId_SystemsettingsScreen , 24 , "t8");
static NexButton bUpdate = NexButton(en_WinId_SystemsettingsScreen , 30, "t12");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bSettingsBack,
									 &bAbout,
									 &bDateTime,
									 &bAdmin,
									 &bUserSetup,
									 &bPump,
									 &bPrinter,
									 &bLamp,
									 &bBuzzer,
									 &bSysParams,
									 &bReset,
									 &bAutoWash,
									 &bMemory,
									 &bSettReset,
									 &bUpdate,
									 NULL};

static void HandlerbSettingsBack(void *ptr);
static void HandlerbResetSettings(void *ptr);

static void HandlerbAbout(void *ptr);
static void HandlerbDateTime(void *ptr);
static void HandlerbAdmin(void *ptr);
static void HandlerbUserSetup(void *ptr);
static void HandlerbPump(void *ptr);
static void HandlerbPrinter(void *ptr);
static void HandlerbLamp(void *ptr);
static void HandlerbBuzzer(void *ptr);
static void HandlerbSysParams(void *ptr);
static void HandlerbReset(void *ptr);
static void HandlerbAutoWash(void *ptr);
static void HandlerbMemory(void *ptr);
static void HandlerbUpdate(void *ptr);

enWindowStatus ShowSystemsettingsScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

	bSettingsBack.attachPush(HandlerbSettingsBack, &bSettingsBack);
	bSettReset.attachPush(HandlerbResetSettings, &bSettReset);

	bAbout.attachPush(HandlerbAbout, &bAbout);
	bDateTime.attachPush(HandlerbDateTime, &bDateTime);
	bAdmin.attachPush(HandlerbAdmin, &bAdmin);
	bUserSetup.attachPush(HandlerbUserSetup, &bUserSetup);
	bPump.attachPush(HandlerbPump, &bPump);
	bPrinter.attachPush(HandlerbPrinter, &bPrinter);
	bLamp.attachPush(HandlerbLamp, &bLamp);
	bBuzzer.attachPush(HandlerbBuzzer, &bBuzzer);
	bSysParams.attachPush(HandlerbSysParams, &bSysParams);
	bReset.attachPush(HandlerbReset, &bReset);
	bAutoWash.attachPush(HandlerbAutoWash, &bAutoWash);
	bMemory.attachPush(HandlerbMemory, &bMemory);
	bUpdate.attachPush(HandlerbUpdate, &bUpdate);

	/*Read settings buffer*/
	ReadSettingsBuffer();
	if(objstcSettings.InitFlag1 != NVM_INIT_OK_FLAG || objstcSettings.InitFlag2 != NVM_INIT_OK_FLAG)
	{
		bSettReset.setVisible(true);
	}
	return WinStatus;
}

void HandlerSystemsettingsScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbSettingsBack(void *ptr)
{
	ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbAbout(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsAbout , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbDateTime(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsDateTime , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbAdmin(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsAdmin , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbUserSetup(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsUserSetup , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbPump(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsPump , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbPrinter(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsPrinter , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbLamp(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsLamp , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbBuzzer(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsBuzzer , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbSysParams(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Login , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbReset(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsReset , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbAutoWash(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsAutowash , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbMemory(void *ptr)
{
	ChangeWindowPage(en_WinId_ClearMemoryLogin , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbResetSettings(void *ptr)
{
	BeepBuzzer();
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enKeyYes == ShowMainPopUp("Settings","Do you want to re-set the Settings?", false))
	{
		ShowPleaseWaitPopup("Reset",PLEASE_WAIT);
		InitSettingsBuffer();
		WriteSettingsBuffer();
		LongBeepBuzzer();
		ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
	}
	else
	{
		ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
	}
}
static void HandlerbUpdate(void *ptr)
{
	ChangeWindowPage(en_WinId_UpdateLogin , (enWindowID)NULL);
	BeepBuzzer();
}
