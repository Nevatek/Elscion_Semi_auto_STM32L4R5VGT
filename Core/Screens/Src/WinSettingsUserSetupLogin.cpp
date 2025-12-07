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

static NexButton bBack = NexButton(en_WinId_SettingsUserSetup , 1, "b5");
static NexButton bEnter = NexButton(en_WinId_SettingsUserSetup , 7, "b1");

static NexText tEnteredPassword = NexText(en_WinId_SettingsUserSetup, 4 , "t1");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bEnter,
									 &tEnteredPassword,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbEnter(void *ptr);
static void HandlertPasswordBox(void *ptr);

enWindowStatus ShowSettingsUserSetupScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bEnter.attachPush(HandlerbEnter, &bEnter);
	tEnteredPassword.attachPush(HandlertPasswordBox, &tEnteredPassword);

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_AlphaKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
	{
		/*Reset buffer*/
		memset(g_arrEnteredPassword , 0 , sizeof(g_arrEnteredPassword));
	}

	tEnteredPassword.setText(g_arrEnteredPassword);
	return WinStatus;
}

void HandlerSettingsUserSetupScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbEnter(void *ptr)
{
	BeepBuzzer();
	if(0 == strncmp(objstcSettings.arr_cAdminPassword , g_arrEnteredPassword , MAX_PASSWORD_LENGTH))
	{
		ChangeWindowPage(en_WinId_SettingsUserSetupList , (enWindowID)NULL);
	}
	else
	{
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enkeyOk == ShowMainPopUp("User Setup","Invalid credentials", true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
	}
}

void HandlertPasswordBox(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrEnteredPassword , sizeof(g_arrEnteredPassword) , false ,
			"Enter Admin Password" , g_arrEnteredPassword , MAX_PASSWORD_LENGTH);
	BeepBuzzer();
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}
