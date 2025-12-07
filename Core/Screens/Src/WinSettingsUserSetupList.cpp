/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"

uint8_t g_u8SelectedSlot = 0;
uint8_t g_u8SlotUserPos[MAX_NUM_OF_USER] = {0xFF};
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_SettingsUserSetupList , 2, "b5");
static NexButton bAddUser = NexButton(en_WinId_SettingsUserSetupList , 5, "b2");
static NexButton bDelete = NexButton(en_WinId_SettingsUserSetupList , 15 , "b0");

static NexText tUser[MAX_NUM_OF_USER] = {NexText(en_WinId_SettingsUserSetupList, 7 , "t3"),
											NexText(en_WinId_SettingsUserSetupList, 8 , "t4"),
											NexText(en_WinId_SettingsUserSetupList, 13 , "t2"),
											NexText(en_WinId_SettingsUserSetupList, 14 , "t7")};

static NexText tUserConst[MAX_NUM_OF_USER] = {NexText(en_WinId_SettingsUserSetupList, 9 , "t5"),
											NexText(en_WinId_SettingsUserSetupList, 10 , "t6"),
											NexText(en_WinId_SettingsUserSetupList, 11 , "t0"),
											NexText(en_WinId_SettingsUserSetupList, 12 , "t1")};
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &tUser[0],
									 &tUser[1],
									 &tUser[2],
									 &tUser[3],
									 &bAddUser,
									 &bDelete,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlertUser1(void *ptr);
static void HandlertUser2(void *ptr);
static void HandlertUser3(void *ptr);
static void HandlertUser4(void *ptr);
static void HandlertUserDelete(void *ptr);
static void HandlertUserAdd(void *ptr);

enWindowStatus ShowSettingsUserSetupListScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bDelete.attachPush(HandlertUserDelete, &bDelete);
	bAddUser.attachPush(HandlertUserAdd, &bAddUser);
	tUser[0].attachPush(HandlertUser1 , &tUser[0]);
	tUser[1].attachPush(HandlertUser2 , &tUser[1]);
	tUser[2].attachPush(HandlertUser3 , &tUser[2]);
	tUser[3].attachPush(HandlertUser4 , &tUser[3]);

	/*Read settings buffer*/
	ReadSettingsBuffer();

	/*Default selected user is 0*/
	g_u8SelectedSlot = 0;
	for(uint8_t u8Idx = 0 ; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
	{
		if(NVM_INIT_OK_FLAG == objstcSettings.NormalUser[u8Idx].Enable)
		{
			g_u8SlotUserPos[u8Idx] = u8Idx;
			tUser[u8Idx].setText(objstcSettings.NormalUser[u8Idx].arrUsername);
			tUser[u8Idx].setVisible(true);
			tUserConst[u8Idx].setVisible(true);
			bDelete.setVisible(true);
		}
	}

//	for(uint8_t u8Idx = 0; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
//	{
//		if(u8Idx == g_u8SelectedSlot)
//		{
//			tUser[u8Idx].Set_font_color_pco(COLOUR_WHITE);
//			tUser[u8Idx].Set_background_image_pic(547);
//			tUserConst[u8Idx].Set_font_color_pco(COLOUR_DARKBLUE);
//		}
//		else
//		{
//			tUser[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
//			tUser[u8Idx].Set_background_image_pic(318);
//			tUserConst[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
//		}
//	}
	return WinStatus;
}

void HandlerSettingsUserSetupListScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlertUserAdd(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsUserAddUser , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlertUserDelete(void *ptr)
{
	BeepBuzzer();
	/*Save upcomming window id before show popup page*/
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enKeyYes == ShowMainPopUp("User Setup","Do you want to delete the user account?", false))
	{
		objstcSettings.NormalUser[g_u8SlotUserPos[g_u8SelectedSlot]].Enable = NVM_INIT_NOT_OK_FLAG;/*Delete user*/

		memset(objstcSettings.NormalUser[g_u8SlotUserPos[g_u8SelectedSlot]].arrUsername , 0
				,sizeof(objstcSettings.NormalUser[g_u8SlotUserPos[g_u8SelectedSlot]].arrUsername));

		memset(objstcSettings.NormalUser[g_u8SlotUserPos[g_u8SelectedSlot]].arrPassword , 0
				,sizeof(objstcSettings.NormalUser[g_u8SlotUserPos[g_u8SelectedSlot]].arrPassword));

		/*hide all entries*/
		for(uint8_t u8Idx = 0 ; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
		{
			tUser[u8Idx].setVisible(false);
			tUserConst[u8Idx].setVisible(false);
			bDelete.setVisible(false);
		}

		for(uint8_t u8Idx = g_u8SelectedSlot ; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
		{
			if(NVM_INIT_OK_FLAG == objstcSettings.NormalUser[u8Idx].Enable)
			{
				objstcSettings.NormalUser[u8Idx - 1].Enable = objstcSettings.NormalUser[u8Idx].Enable;

				memcpy(objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx - 1]].arrUsername , objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx]].arrUsername
								,sizeof(objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx]].arrUsername));

				memcpy(objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx - 1]].arrPassword , objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx]].arrPassword
								,sizeof(objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx]].arrPassword));

				objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx]].Enable = NVM_INIT_NOT_OK_FLAG;/*Delete user*/

				memset(objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx]].arrUsername , 0
						,sizeof(objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx]].arrUsername));

				memset(objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx]].arrPassword , 0
						,sizeof(objstcSettings.NormalUser[g_u8SlotUserPos[u8Idx]].arrPassword));

//				g_u8SlotUserPos[u8Idx] = u8Idx;
//				tUser[u8Jdx].setText(objstcSettings.NormalUser[u8Idx].arrUsername);
//				tUser[u8Jdx].setVisible(true);
//				tUserConst[u8Jdx].setVisible(true);
//				bDelete.setVisible(true);
			}
		}

//		for(uint8_t u8Idx = 0; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
//		{
//			if(u8Idx == g_u8SelectedSlot)
//			{
//				tUser[u8Idx].Set_font_color_pco(COLOUR_WHITE);
//				tUser[u8Idx].Set_background_image_pic(547);
//				tUserConst[u8Idx].Set_font_color_pco(COLOUR_DARKBLUE);
//			}
//			else
//			{
//				tUser[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
//				tUser[u8Idx].Set_background_image_pic(318);
//				tUserConst[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
//			}
//		}
		memset(stcTestData.arrUserName,0,sizeof(stcTestData.arrUserName));
		WriteSettingsBuffer();
	}
	stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
	ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
}

void HandlertUser1(void *ptr)
{
	g_u8SelectedSlot = 0;
	for(uint8_t u8Idx = 0; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
	{
		if(u8Idx == g_u8SelectedSlot)
		{
			tUser[u8Idx].Set_font_color_pco(COLOUR_WHITE);
			tUser[u8Idx].Set_background_image_pic(547);
			tUserConst[u8Idx].Set_font_color_pco(COLOUR_DARKBLUE);
		}
		else
		{
			tUser[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
			tUser[u8Idx].Set_background_image_pic(318);
			tUserConst[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
		}
	}
	BeepBuzzer();
}
void HandlertUser2(void *ptr)
{
	g_u8SelectedSlot = 1;
	for(uint8_t u8Idx = 0; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
	{
		if(u8Idx == g_u8SelectedSlot)
		{
			tUser[u8Idx].Set_font_color_pco(COLOUR_WHITE);
			tUser[u8Idx].Set_background_image_pic(547);
			tUserConst[u8Idx].Set_font_color_pco(COLOUR_DARKBLUE);
		}
		else
		{
			tUser[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
			tUser[u8Idx].Set_background_image_pic(318);
			tUserConst[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
		}
	}
	BeepBuzzer();
}
void HandlertUser3(void *ptr)
{
	g_u8SelectedSlot = 2;
	for(uint8_t u8Idx = 0; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
	{
		if(u8Idx == g_u8SelectedSlot)
		{
			tUser[u8Idx].Set_font_color_pco(COLOUR_WHITE);
			tUser[u8Idx].Set_background_image_pic(547);
			tUserConst[u8Idx].Set_font_color_pco(COLOUR_DARKBLUE);
		}
		else
		{
			tUser[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
			tUser[u8Idx].Set_background_image_pic(318);
			tUserConst[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
		}
	}
	BeepBuzzer();
}
void HandlertUser4(void *ptr)
{
	g_u8SelectedSlot = 3;
	for(uint8_t u8Idx = 0; u8Idx < MAX_NUM_OF_USER ; ++u8Idx)
	{
		if(u8Idx == g_u8SelectedSlot)
		{
			tUser[u8Idx].Set_font_color_pco(COLOUR_WHITE);
			tUser[u8Idx].Set_background_image_pic(547);
			tUserConst[u8Idx].Set_font_color_pco(COLOUR_DARKBLUE);
		}
		else
		{
			tUser[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
			tUser[u8Idx].Set_background_image_pic(318);
			tUserConst[u8Idx].Set_font_color_pco(NOT_SELECTED_FONT_COLOUR);
		}
	}
	BeepBuzzer();
}
