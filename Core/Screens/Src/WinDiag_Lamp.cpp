/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "Lamp.h"
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
bool lamp_replace = false;

static NexButton bBack = NexButton(en_WinId_Diag_Lamp , 1, "b5");
static NexButton bReplaced = NexButton(en_WinId_Diag_Lamp , 11, "b0");
static NexPicture pBatteryBorder = NexPicture(en_WinId_Diag_Lamp , 4, "p0");
static NexPicture Bat0 = NexPicture(en_WinId_Diag_Lamp , 5, "p1");
static NexPicture Bat1 = NexPicture(en_WinId_Diag_Lamp , 6, "p2");
static NexPicture Bat2 = NexPicture(en_WinId_Diag_Lamp , 7, "p5");
static NexPicture Bat3 = NexPicture(en_WinId_Diag_Lamp , 8, "p6");
static NexPicture pLamp = NexPicture(en_WinId_Diag_Lamp , 12, "p7");
static NexPicture pReplaceLamp = NexPicture(en_WinId_Diag_Lamp , 10 , "p8");
static NexPicture pReplacePopup = NexPicture(en_WinId_Diag_Lamp , 13 , "p9");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bReplaced,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbReplaced(void *ptr);
static void ShowBatteryPercentage(float Percent);

enWindowStatus ShowDiagScreen_Lamp (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bReplaced.attachPush(HandlerbReplaced, &bReplaced);

	/*Read settings buffer*/
	ReadSecondarySettingsBuffer();
	if(lamp_replace)
	{
		pReplacePopup.setVisible(true);
		pReplaceLamp.setVisible(false);
		bBack.setVisible(false);
	}
	else
	{
		pReplacePopup.setVisible(false);
		bBack.setVisible(true);
	}
	uint32_t u32LampHours = 0;
	float fLampLifePercent = 0;
	u32LampHours = Get_LampWorkHours();
	fLampLifePercent = Calculate_Percentage((float)u32LampHours , (float)MAX_LAMP_HOURS);
	ShowBatteryPercentage(fLampLifePercent);

	return WinStatus;
}

void HandlerDiagScreen_Lamp (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}
void ShowBatteryPercentage(float Percent)
{
	if(100 >= Percent && 75 < Percent)
	{
		Bat0.setVisible(true);
		Bat1.setVisible(false);
		Bat2.setVisible(false);
		Bat3.setVisible(false);
		pReplaceLamp.setVisible(true);
		bReplaced.setVisible(true);
		pBatteryBorder.setVisible(true);
		Bat0.Set_background_image_pic(376);
	}
	else if(75 >= Percent && 50 < Percent)
	{
		Bat0.setVisible(true);
		Bat1.setVisible(true);
		Bat2.setVisible(false);
		Bat3.setVisible(false);
		pReplaceLamp.setVisible(false);
		bReplaced.setVisible(false);
		pBatteryBorder.setVisible(true);
		Bat0.Set_background_image_pic(372);
		Bat1.Set_background_image_pic(372);
	}
	else if(50 >= Percent && 25 < Percent)
	{
		Bat0.setVisible(true);
		Bat1.setVisible(true);
		Bat2.setVisible(true);
		Bat3.setVisible(false);
		pReplaceLamp.setVisible(false);
		bReplaced.setVisible(false);
		pBatteryBorder.setVisible(true);
		Bat0.Set_background_image_pic(372);
		Bat1.Set_background_image_pic(372);
		Bat2.Set_background_image_pic(372);
	}
	else if(25 >= Percent && 10 < Percent)
	{
		Bat0.setVisible(true);
		Bat1.setVisible(true);
		Bat2.setVisible(true);
		Bat3.setVisible(true);
		pReplaceLamp.setVisible(false);
		bReplaced.setVisible(false);
		pBatteryBorder.setVisible(true);
		Bat0.Set_background_image_pic(372);
		Bat1.Set_background_image_pic(372);
		Bat2.Set_background_image_pic(372);
		Bat3.Set_background_image_pic(372);
	}
	else if(10 >= Percent)
	{
		Bat0.setVisible(true);
		Bat1.setVisible(true);
		Bat2.setVisible(true);
		Bat3.setVisible(true);
		pReplaceLamp.setVisible(false);
		bReplaced.setVisible(false);
		pBatteryBorder.setVisible(true);
		Bat0.Set_background_image_pic(372);
		Bat1.Set_background_image_pic(372);
		Bat2.Set_background_image_pic(372);
		Bat3.Set_background_image_pic(372);
	}
}
void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_DiagnosticsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbReplaced(void *ptr)
{
	BeepBuzzer();
	/*Save upcomming window id before show popup page*/
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enKeyYes == ShowMainPopUp("Lamp", "Do You Want To Replace?" , false))
	{
		Reset_LampWorkHours();
		WriteSecondarySettingsBuffer();
		lamp_replace = true;
	}
	stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
	ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
}
