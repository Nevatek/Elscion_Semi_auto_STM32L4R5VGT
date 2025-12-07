/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"

static char g_arrOldPassword[MAX_PASSWORD_LENGTH + 1];
static char g_arrNewPassword[MAX_PASSWORD_LENGTH + 1];
static char g_arrRenterPassword[MAX_PASSWORD_LENGTH + 1];

static enUserType g_User = enAdminuser;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_SettingsAdmin , 2, "b5");
static NexButton bAdmin = NexButton(en_WinId_SettingsAdmin , 5, "b0");
static NexButton bService = NexButton(en_WinId_SettingsAdmin , 6, "b1");
static NexButton bReset = NexButton(en_WinId_SettingsAdmin , 13, "b2");

static NexText tOldPassword = NexText(en_WinId_SettingsAdmin, 7 , "t0");
static NexText tPassword = NexText(en_WinId_SettingsAdmin, 8 , "t1");
static NexText tRePassword = NexText(en_WinId_SettingsAdmin, 9 , "t2");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									&tOldPassword,
									&tPassword,
									&tRePassword,
									&bAdmin,
									&bService,
									&bReset,
									NULL};

static void HandlerbBack(void *ptr);
static void HandlerbOldPassword(void *ptr);
static void HandlerbNewPassword(void *ptr);
static void HandlerbRerenterPassword(void *ptr);

static void HandlerbAdmin(void *ptr);
static void HandlerbService(void *ptr);
static void HandlerbReset(void *ptr);

enWindowStatus ShowSettingsAdminScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	tOldPassword.attachPush(HandlerbOldPassword, &tOldPassword);
	tPassword.attachPush(HandlerbNewPassword, &tPassword);
	tRePassword.attachPush(HandlerbRerenterPassword, &tRePassword);

	bAdmin.attachPush(HandlerbAdmin, &bAdmin);
	bService.attachPush(HandlerbService, &bService);
	bReset.attachPush(HandlerbReset, &bReset);

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_AlphaKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
	{
		/*Reset buffer*/
		memset(g_arrOldPassword , 0 , sizeof(g_arrOldPassword));
		memset(g_arrNewPassword , 0 , sizeof(g_arrNewPassword));
		memset(g_arrRenterPassword , 0 , sizeof(g_arrRenterPassword));
		g_User = enAdminuser;
	}

	if(enServiceUser == g_User)
	{
		bAdmin.Set_background_image_pic(299);
		bService.Set_background_image_pic(297);
	}
	else if(enAdminuser == g_User)
	{
		bAdmin.Set_background_image_pic(298);
		bService.Set_background_image_pic(296);
	}
	tOldPassword.setText(g_arrOldPassword);
	tPassword.setText(g_arrNewPassword);
	tRePassword.setText(g_arrRenterPassword);
	return WinStatus;
}

void HandlerSettingsAdminScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbAdmin(void *ptr)
{
	if(enServiceUser == g_User)
	{
		g_User = enAdminuser;
		bAdmin.Set_background_image_pic(298);
		bService.Set_background_image_pic(296);
		/*Reset buffer*/
		memset(g_arrOldPassword , 0 , sizeof(g_arrOldPassword));
		memset(g_arrNewPassword , 0 , sizeof(g_arrNewPassword));
		memset(g_arrRenterPassword , 0 , sizeof(g_arrRenterPassword));
		tOldPassword.setText(g_arrOldPassword);
		tPassword.setText(g_arrNewPassword);
		tRePassword.setText(g_arrRenterPassword);
		BeepBuzzer();
	}
}
void HandlerbService(void *ptr)
{
	if(enAdminuser == g_User)
	{
		g_User = enServiceUser;
		bAdmin.Set_background_image_pic(299);
		bService.Set_background_image_pic(297);
		/*Reset buffer*/
		memset(g_arrOldPassword , 0 , sizeof(g_arrOldPassword));
		memset(g_arrNewPassword , 0 , sizeof(g_arrNewPassword));
		memset(g_arrRenterPassword , 0 , sizeof(g_arrRenterPassword));
		tOldPassword.setText(g_arrOldPassword);
		tPassword.setText(g_arrNewPassword);
		tRePassword.setText(g_arrRenterPassword);
		BeepBuzzer();
	}
}
void HandlerbReset(void *ptr)
{
	bool bSaveSettings = false;
	BeepBuzzer();
	if(enAdminuser == g_User)
	{
		/*if super user password entered to reset admin password*/
		if(0 == strncmp(DEFAULT_SUPER_USER_ADMIN_PASSWORD , g_arrOldPassword , MAX_PASSWORD_LENGTH))
		{
			/*Save upcomming window id before show popup page*/
			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
			if(enkeyOk == ShowMainPopUp("Admin","Password Saved", true))
			{
				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
				ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
			}
			/*save new password*/
			strncpy(objstcSettings.arr_cAdminPassword , g_arrNewPassword , MAX_PASSWORD_LENGTH);
			bSaveSettings = true;
		}
		/*if old password is same as entered old password*/
		else if('\0' == g_arrOldPassword[0] || 0xFF == g_arrOldPassword[0] ||
				0 != strncmp(objstcSettings.arr_cAdminPassword , g_arrOldPassword , MAX_PASSWORD_LENGTH))
		{
			/*Save upcomming window id before show popup page*/
			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
			if(enkeyOk == ShowMainPopUp("Admin","Invalid Credentials", true))
			{
				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
				ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
			}
		}
		else
		{
			if('\0' == g_arrNewPassword [0] || 0xFF == g_arrNewPassword[0] ||
					0 != strncmp(g_arrNewPassword , g_arrRenterPassword , MAX_PASSWORD_LENGTH))
			{
				/*Save upcomming window id before show popup page*/
				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
				if(enkeyOk == ShowMainPopUp("Admin","New Password Mismatch", true))
				{
					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
					ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
				}
			}
			else/*if new passwords entered are same*/
			{
				/*Save upcomming window id before show popup page*/
				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
				if(enkeyOk == ShowMainPopUp("Admin","Password Changed", true))
				{
					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
					ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
				}
				/*save new password*/
				strncpy(objstcSettings.arr_cAdminPassword , g_arrNewPassword , MAX_PASSWORD_LENGTH);
				bSaveSettings = true;
			}
		}
	}
	else if(enServiceUser == g_User)
	{
		/*if super user password entered to reset serive password*/
		if(0 == strncmp(DEFAULT_SUPER_USER_SERVICE_PASSWORD , g_arrOldPassword , MAX_PASSWORD_LENGTH))
		{
			/*Save upcomming window id before show popup page*/
			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
			if(enkeyOk == ShowMainPopUp("Service","Password Saved", true))
			{
				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
				ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
			}
			/*save new password*/
			strncpy(objstcSettings.arr_cServicePassword , g_arrNewPassword , MAX_PASSWORD_LENGTH);
			bSaveSettings = true;
		}
		/*if old password is not same as entered old password*/
		else if('\0' == g_arrOldPassword[0] || 0xFF == g_arrOldPassword[0] ||
				0 != strncmp(objstcSettings.arr_cServicePassword , g_arrOldPassword , MAX_PASSWORD_LENGTH))
		{
			/*Save upcomming window id before show popup page*/
			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
			if(enkeyOk == ShowMainPopUp("Service","Invalid Credentials", true))
			{
				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
				ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
			}
		}
		else
		{
			if('\0' == g_arrNewPassword [0] || 0xFF == g_arrNewPassword[0] ||
					0 != strncmp(g_arrNewPassword , g_arrRenterPassword , MAX_PASSWORD_LENGTH))
			{
				/*Save upcomming window id before show popup page*/
				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
				if(enkeyOk == ShowMainPopUp("Service","New Password Mismatch", true))
				{
					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
					ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
				}
			}
			else/*if new passwords entered are same*/
			{
				/*Save upcomming window id before show popup page*/
				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
				if(enkeyOk == ShowMainPopUp("Service","Password Changed", true))
				{
					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
					ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
				}
				/*save new password*/
				strncpy(objstcSettings.arr_cServicePassword , g_arrNewPassword , MAX_PASSWORD_LENGTH);
				bSaveSettings = true;
			}
		}
	}

	if(bSaveSettings)
	{
		/*save settings*/
		WriteSettingsBuffer();
	}
}
//
//void HandlerbSave(void *ptr)
//{
//	bool bSaveSettings = false;
//	BeepBuzzer();
//	if(enAdminuser == g_User)
//	{
//		/*if super user password entered*/
//		if(0 == strncmp(DEFAULT_SUPER_USER_ADMIN_PASSWORD , g_arrOldPassword , MAX_PASSWORD_LENGTH))
//		{
//			/*Save upcomming window id before show popup page*/
//			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//			if(enkeyOk == ShowMainPopUp("Password Saved", true))
//			{
//				BeepBuzzer();
//				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//				ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
//			}
//			/*save new password*/
//			strncpy(objstcSettings.arr_cAdminPassword , g_arrNewPassword , MAX_PASSWORD_LENGTH);
//			bSaveSettings = true;
//		}
//		/*if old password is same as entered old password*/
//		else if('\0' == g_arrOldPassword[0] || 0xFF == g_arrOldPassword[0] ||
//				0 != strncmp(objstcSettings.arr_cAdminPassword , g_arrOldPassword , MAX_PASSWORD_LENGTH))
//		{
//			/*Save upcomming window id before show popup page*/
//			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//			if(enkeyOk == ShowMainPopUp("Invalid Credentials", true))
//			{
//				BeepBuzzer();
//				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//				ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
//			}
//		}
//		else
//		{
//			if('\0' == g_arrNewPassword [0] || 0xFF == g_arrNewPassword[0] ||
//					0 != strncmp(g_arrNewPassword , g_arrRenterPassword , MAX_PASSWORD_LENGTH))
//			{
//				/*Save upcomming window id before show popup page*/
//				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//				if(enkeyOk == ShowMainPopUp("New Password Mismatch", true))
//				{
//					BeepBuzzer();
//					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//					ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
//				}
//			}
//			else/*if new passwords entered are same*/
//			{
//				/*Save upcomming window id before show popup page*/
//				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//				if(enkeyOk == ShowMainPopUp("Admin Password Changed", true))
//				{
//					BeepBuzzer();
//					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//					ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
//				}
//				/*save new password*/
//				strncpy(objstcSettings.arr_cAdminPassword , g_arrNewPassword , MAX_PASSWORD_LENGTH);
//				bSaveSettings = true;
//			}
//		}
//	}
//	else if(enServiceUser == g_User)
//	{
//		/*if super user password entered*/
//		if(0 == strncmp(DEFAULT_SUPER_USER_SERVICE_PASSWORD , g_arrOldPassword , MAX_PASSWORD_LENGTH))
//		{
//			/*Save upcomming window id before show popup page*/
//			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//			if(enkeyOk == ShowMainPopUp("Password Saved", true))
//			{
//				BeepBuzzer();
//				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//				ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
//			}
//			/*save new password*/
//			strncpy(objstcSettings.arr_cServicePassword , g_arrNewPassword , MAX_PASSWORD_LENGTH);
//			bSaveSettings = true;
//		}
//		/*if old password is not same as entered old password*/
//		else if('\0' == g_arrOldPassword[0] || 0xFF == g_arrOldPassword[0] ||
//				0 != strncmp(objstcSettings.arr_cServicePassword , g_arrOldPassword , MAX_PASSWORD_LENGTH))
//		{
//			/*Save upcomming window id before show popup page*/
//			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//			if(enkeyOk == ShowMainPopUp("Invalid Credentials", true))
//			{
//				BeepBuzzer();
//				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//				ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
//			}
//		}
//		else
//		{
//			if('\0' == g_arrNewPassword [0] || 0xFF == g_arrNewPassword[0] ||
//					0 != strncmp(g_arrNewPassword , g_arrRenterPassword , MAX_PASSWORD_LENGTH))
//			{
//				/*Save upcomming window id before show popup page*/
//				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//				if(enkeyOk == ShowMainPopUp("New Password Mismatch", true))
//				{
//					BeepBuzzer();
//					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//					ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
//				}
//			}
//			else/*if new passwords entered are same*/
//			{
//				/*Save upcomming window id before show popup page*/
//				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//				if(enkeyOk == ShowMainPopUp("Service Password Changed", true))
//				{
//					BeepBuzzer();
//					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//					ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
//				}
//				/*save new password*/
//				strncpy(objstcSettings.arr_cServicePassword , g_arrNewPassword , MAX_PASSWORD_LENGTH);
//				bSaveSettings = true;
//			}
//		}
//	}
//
//	if(bSaveSettings)
//	{
//		/*save settings*/
//		WriteSettingsBuffer();
//	}
//}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbOldPassword(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrOldPassword , sizeof(g_arrOldPassword) , false ,
			"Enter Old Password" , g_arrOldPassword , MAX_PASSWORD_LENGTH);
	BeepBuzzer();
}
void HandlerbNewPassword(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrNewPassword , sizeof(g_arrNewPassword) , false ,
			"Enter New Password" , g_arrNewPassword , MAX_PASSWORD_LENGTH);
	BeepBuzzer();
}
void HandlerbRerenterPassword(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrRenterPassword , sizeof(g_arrRenterPassword) , false ,
			"Re-enter New Password" , g_arrRenterPassword , MAX_PASSWORD_LENGTH);
	BeepBuzzer();
}
