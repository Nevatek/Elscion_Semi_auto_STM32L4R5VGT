/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"

static char g_arrServicePassword[MAX_PASSWORD_LENGTH];
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_SettingsReset , 1, "b5");
static NexButton bReset = NexButton(en_WinId_SettingsReset , 7, "b1");
static NexText tPassword = NexText(en_WinId_SettingsReset, 4 , "t1");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bReset,
									 &tPassword,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbResetFactory(void *ptr);
static void HandlertPassword(void *ptr);

enWindowStatus ShowSettingsResetScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bReset.attachPush(HandlerbResetFactory, &bReset);
	tPassword.attachPush(HandlertPassword, &tPassword);

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_AlphaKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
	{
		/*Reset buffer*/
		memset(g_arrServicePassword , 0 , sizeof(g_arrServicePassword));
	}
	tPassword.setText(g_arrServicePassword);
	return WinStatus;
}

void HandlerSettingsResetScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlertPassword(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrServicePassword , sizeof(g_arrServicePassword) , false ,
			"Enter Service Password" , g_arrServicePassword , MAX_PASSWORD_LENGTH);
	BeepBuzzer();
}
void HandlerbResetFactory(void *ptr)
{
	BeepBuzzer();
	/*if old password is same as entered old password*/
	if(	0 == strncmp(objstcSettings.arr_cServicePassword , g_arrServicePassword , MAX_PASSWORD_LENGTH))
	{
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enKeyYes == ShowMainPopUp("Reset","Do you want to factory re-set?", false))
		{
			ShowPleaseWaitPopup("Reset",PLEASE_WAIT);
			ResetNVMFlash();/*Reset settings*/
			LongBeepBuzzer();
			ChangeWindowPage(en_WinId_StartupScreen , (enWindowID)en_WinID_MainMenuScreen);
		}
		else
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
	}
	else
	{
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enkeyOk == ShowMainPopUp("Reset","Invalid credentials", true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
	}

}
