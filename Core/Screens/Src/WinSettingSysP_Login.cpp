/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"

static char g_arrEnteredPassword[MAX_PASSWORD_LENGTH + 1];

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_SettingsSysP_Login , 1, "b5");
static NexButton bProceed = NexButton(en_WinId_SettingsSysP_Login, 7 , "b1");
static NexText tPassword = NexText(en_WinId_SettingsSysP_Login, 4 , "t1");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bProceed,
									 &tPassword,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbProceed(void *ptr);
static void HandlerbPassword(void *ptr);

enWindowStatus ShowSettingsSysP_Login (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bProceed.attachPush(HandlerbProceed, &bProceed);
	tPassword.attachPush(HandlerbPassword, &tPassword);

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_AlphaKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
	{
		/*Reset buffer*/
		memset(g_arrEnteredPassword , 0 , sizeof(g_arrEnteredPassword));
	}

	tPassword.setText(g_arrEnteredPassword);

	return WinStatus;
}

void HandlerSettingsSysP_Login (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbProceed(void *ptr)
{
	BeepBuzzer();
	if(0 == strncmp(objstcSettings.arr_cServicePassword , g_arrEnteredPassword , MAX_PASSWORD_LENGTH))
	{
		ChangeWindowPage(en_WinId_SettingsSysP_Lamp , (enWindowID)NULL);
	}
	else
	{
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enkeyOk == ShowMainPopUp("SERVICE","Invalid Credentials", true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
	}
}
void HandlerbPassword(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrEnteredPassword , sizeof(g_arrEnteredPassword) , false ,
			"Enter Service Password" , g_arrEnteredPassword , MAX_PASSWORD_LENGTH);
	BeepBuzzer();
}
