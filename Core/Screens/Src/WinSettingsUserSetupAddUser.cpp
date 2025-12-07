/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"

static char g_arrEnteredPassword[1 + MAX_PASSWORD_LENGTH];
static char g_arrEnteredUsername[1 + MAX_USERNAME_LENGTH];
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_SettingsUserAddUser , 2, "b5");
static NexButton bSave = NexButton(en_WinId_SettingsUserAddUser , 11 , "b0");
static NexButton bUsers = NexButton(en_WinId_SettingsUserAddUser , 6 , "b3");
static NexText tUserName = NexText(en_WinId_SettingsUserAddUser, 7 , "t3");
static NexText tPassword = NexText(en_WinId_SettingsUserAddUser, 8 , "t4");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									&bSave,
									&bUsers,
									&tUserName,
									&tPassword,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlertUserName(void *ptr);
static void HandlertPassword(void *ptr);
static void HandlerbSave(void *ptr);
static void HandlerbUsers(void *ptr);
enWindowStatus ShowSettingsUserSetupAddUserScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bSave.attachPush(HandlerbSave, &bSave);
	bUsers.attachPush(HandlerbUsers, &bUsers);
	tUserName.attachPush(HandlertUserName, &tUserName);
	tPassword.attachPush(HandlertPassword, &tPassword);

	/*Read settings buffer*/
	ReadSettingsBuffer();

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_AlphaKeypad == PrevWindow || en_WinId_MainPopup == PrevWindow)
	{
		tUserName.setText(g_arrEnteredUsername);
		tPassword.setText(g_arrEnteredPassword);
	}
	else
	{
		memset(g_arrEnteredUsername , 0 , sizeof(g_arrEnteredUsername));
		memset(g_arrEnteredPassword , 0 , sizeof(g_arrEnteredPassword));
	}

	return WinStatus;
}

void HandlerSettingsUserSetupAddUserScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlertUserName(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrEnteredUsername , sizeof(g_arrEnteredUsername) , false ,
			"Enter User Name" , g_arrEnteredUsername , MAX_USERNAME_LENGTH);
	BeepBuzzer();
}
void HandlertPassword(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrEnteredPassword , sizeof(g_arrEnteredPassword) , false ,
			"Enter Password" , g_arrEnteredPassword , MAX_PASSWORD_LENGTH);
	BeepBuzzer();
}
void HandlerbSave(void *ptr)
{
	BeepBuzzer();
	for(uint8_t u8Idx = 0; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
	{
		if(NVM_INIT_OK_FLAG != objstcSettings.NormalUser[u8Idx].Enable)
		{
	        if (g_arrEnteredUsername[0] == 0)
	        {
	            if(enkeyOk == ShowMainPopUp("User Setup","User Name Should Not Be Empty", true))
	            {
	            	ChangeWindowPage(en_WinId_SettingsUserAddUser , (enWindowID)NULL);
	            	stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
	            }
	        }
	        else if(g_arrEnteredPassword[0] == 0)
	        {
	            if(enkeyOk == ShowMainPopUp("User Setup","Password Should Not Be Empty", true))
	            {
	            	ChangeWindowPage(en_WinId_SettingsUserAddUser , (enWindowID)NULL);
	            	stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
	            }
	        }
	        else
	        {
	        	strncpy(objstcSettings.NormalUser[u8Idx].arrUsername , " " , (MAX_USERNAME_LENGTH + 1));
	        	strncpy(objstcSettings.NormalUser[u8Idx].arrPassword , " " , (MAX_PASSWORD_LENGTH + 1));
				strncpy(objstcSettings.NormalUser[u8Idx].arrUsername , g_arrEnteredUsername , MAX_USERNAME_LENGTH);
				strncpy(objstcSettings.NormalUser[u8Idx].arrPassword , g_arrEnteredPassword , MAX_PASSWORD_LENGTH);
				objstcSettings.NormalUser[u8Idx].Enable = NVM_INIT_OK_FLAG;
				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
				if(enkeyOk == ShowMainPopUp("User Setup","User Name And Password Saved", true))
				{
					/*save settings*/
					WriteSettingsBuffer();
//					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
					ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
				}
	        }
			return;
		}
	}
	/*if no slot available*/
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enkeyOk == ShowMainPopUp("User Setup","Maximum Users Added. Delete To Add New User", true))
	{
		stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
		ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
	}
}
void HandlerbUsers(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsUserSetupList , (enWindowID)NULL);
	BeepBuzzer();
}
