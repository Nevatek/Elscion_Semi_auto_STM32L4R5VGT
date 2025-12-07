/*
 * WindowStartup.c
 *
 *  Created on: Aug 1, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"

static uint8_t g_u8SelectedUsrIdx = 0;
static char arrUserLoginPassword[MAX_PASSWORD_LENGTH];

typedef enum
{
	Normal_user = 0,
	Admin_user,
}enUserType;

enUserType g_UserType = Normal_user;

static NexButton bBack = NexButton(en_WinId_UserLogin , 10, "b5");
static NexButton bGuestUser = NexButton(en_WinId_UserLogin , 4, "b0");
static NexButton bUserLogin = NexButton(en_WinId_UserLogin , 14, "b6");
static NexButton bUserLoginClose = NexButton(en_WinId_UserLogin , 16, "b7");

static NexPicture bUserLoginPopup = NexPicture(en_WinId_UserLogin , 11, "p1");
static NexText bUserLoginUser = NexText(en_WinId_UserLogin , 15, "t3");
static NexText bUserLoginPass = NexText(en_WinId_UserLogin , 13, "t2");
static NexText bUserLoginPassText = NexText(en_WinId_UserLogin , 12, "t1");

static NexButton bUser[MAX_NUM_OF_USER] =
		{
				NexButton(en_WinId_UserLogin , 5 , "b1"),
				NexButton(en_WinId_UserLogin , 6 , "b2"),
				NexButton(en_WinId_UserLogin , 7 , "b3"),
				NexButton(en_WinId_UserLogin , 8 , "b4"),
		};

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									&bGuestUser,
									&bUser[0],
									&bUser[1],
									&bUser[2],
									&bUser[3],
									&bUserLogin,
									&bUserLoginPassText,
									&bUserLoginClose,
									NULL};
static void HandlerbBack(void *ptr);
static void HandlerbGuestLogin(void *ptr);
static void HandlerbUser0(void *ptr);
static void HandlerbUser1(void *ptr);
static void HandlerbUser2(void *ptr);
static void HandlerbUser3(void *ptr);
static void HandlerbUserLogin(void *ptr);
static void HandlerbUserLoginPassowrd(void *ptr);
static void HandlerbUserLoginClose(void *ptr);

static void ShowPasswordLogin(uint8_t u8Status);

enWindowStatus ShowUserLoginScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

	bBack.attachPush(HandlerbBack, &bBack);
	bGuestUser.attachPush(HandlerbGuestLogin, &bGuestUser);
	bUser[0].attachPush(HandlerbUser0, &bUser[0]);
	bUser[1].attachPush(HandlerbUser1, &bUser[1]);
	bUser[2].attachPush(HandlerbUser2, &bUser[2]);
	bUser[3].attachPush(HandlerbUser3, &bUser[3]);
	bUserLogin.attachPush(HandlerbUserLogin, &bUserLogin);
	bUserLoginPassText.attachPush(HandlerbUserLoginPassowrd, &bUserLoginPassText);
	bUserLoginClose.attachPush(HandlerbUserLoginClose, &bUserLoginClose);
	bGuestUser.setText("ADMIN");

	for(uint8_t u8Idx = 0  , u8BoxEnable = 0; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
	{
		if(NVM_INIT_OK_FLAG == objstcSettings.NormalUser[u8Idx].Enable)
		{
			bUser[u8BoxEnable].setVisible(true);
			bUser[u8BoxEnable].setText(objstcSettings.NormalUser[u8Idx].arrUsername);
			u8BoxEnable++;
		}
	}

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_AlphaKeypad == PrevWindow)
	{
		ShowPasswordLogin(true);
		bUserLoginPassText.setText(arrUserLoginPassword);
	}
	else
	{
		g_u8SelectedUsrIdx = 0;
		ShowPasswordLogin(false);
	}
	return WinStatus;
}

void HandlerUserLoginScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}
void HandlerbBack(void *ptr)
{
	ChangeWindowPage(stcScreenNavigation.NextWindowId , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbUserLoginClose(void *ptr)
{
	ShowPasswordLogin(false);
	BeepBuzzer();
}
void HandlerbGuestLogin(void *ptr)
{
	/*For admin account login*/
	g_UserType = Admin_user;
	ShowPasswordLogin(true);
	BeepBuzzer();
}
void HandlerbUser0(void *ptr)
{
	g_UserType = Normal_user;
	g_u8SelectedUsrIdx = 0;
	ShowPasswordLogin(true);
	BeepBuzzer();
}
void HandlerbUser1(void *ptr)
{
	g_UserType = Normal_user;
	g_u8SelectedUsrIdx = 1;
	ShowPasswordLogin(true);
	BeepBuzzer();
}
void HandlerbUser2(void *ptr)
{
	g_UserType = Normal_user;
	g_u8SelectedUsrIdx = 2;
	ShowPasswordLogin(true);
	BeepBuzzer();
}
void HandlerbUser3(void *ptr)
{
	g_UserType = Normal_user;
	g_u8SelectedUsrIdx = 3;
	ShowPasswordLogin(true);
	BeepBuzzer();
}
void HandlerbUserLoginPassowrd(void *ptr)
{
	memset(arrUserLoginPassword , 0 , sizeof(arrUserLoginPassword));
	openKeyBoard(en_AlphaKeyboard , arrUserLoginPassword , sizeof(arrUserLoginPassword) , false ,
			"Enter Password" , arrUserLoginPassword , MAX_PASSWORD_LENGTH);
	BeepBuzzer();
}
void HandlerbUserLogin(void *ptr)
{
	BeepBuzzer();
	if(g_UserType == Admin_user)
	{
		if(0 == strncmp(arrUserLoginPassword ,objstcSettings.arr_cAdminPassword , MAX_PASSWORD_LENGTH))
		{
			strncpy(stcTestData.arrUserName , "ADMIN" , MAX_USERNAME_LENGTH);
			ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
		}
		else
		{
			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
			if(enkeyOk == ShowMainPopUp("Admin Login","Password Incorrect", true))
			{
				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
				ChangeWindowPage((enWindowID)NextWindow , stcScreenNavigation.NextWindowId);
			}
		}
	}
	else if(g_UserType == Normal_user)
	{
		if(0 == strncmp(arrUserLoginPassword ,
				objstcSettings.NormalUser[g_u8SelectedUsrIdx].arrPassword , MAX_PASSWORD_LENGTH))
		{
			strncpy(stcTestData.arrUserName ,
					objstcSettings.NormalUser[g_u8SelectedUsrIdx].arrUsername , MAX_USERNAME_LENGTH);
			ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
		}
		else
		{
			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
			if(enkeyOk == ShowMainPopUp("User Login","Password Incorrect", true))
			{
				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
				ChangeWindowPage((enWindowID)NextWindow , stcScreenNavigation.NextWindowId);
			}
		}
	}

}
void ShowPasswordLogin(uint8_t u8Status)
{
	if(	g_UserType == Admin_user)
	{
		bUserLoginUser.setText("ADMIN");
	}
	else
	{
		bUserLoginUser.setText(objstcSettings.NormalUser[g_u8SelectedUsrIdx].arrUsername);
	}
	bUserLogin.setVisible(u8Status);
	bUserLoginPopup.setVisible(u8Status);
	bUserLoginUser.setVisible(u8Status);
	bUserLoginPass.setVisible(u8Status);
	bUserLoginPassText.setVisible(u8Status);
	bUserLoginClose.setVisible(u8Status);
}
