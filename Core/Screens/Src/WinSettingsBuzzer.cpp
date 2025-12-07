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
static bool Buzzer = true;
static NexButton bBack = NexButton(en_WinId_SettingsBuzzer , 1, "b5");
static NexButton bSave = NexButton(en_WinId_SettingsBuzzer , 6, "b4");
static NexButton bBuzzSwitch = NexButton(en_WinId_SettingsBuzzer , 4 , "b0");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bBuzzSwitch,
									 &bSave,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbSave(void *ptr);
static void HandlerbBuzzSwitch(void *ptr);

enWindowStatus ShowSettingsBuzzerScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bSave.attachPush(HandlerbSave, &bSave);
	bBuzzSwitch.attachPush(HandlerbBuzzSwitch, &bBuzzSwitch);

	/*Read settings buffer*/
//	ReadSettingsBuffer();
	Buzzer = objstcSettings.u32BuzzerEnable;

	if(Buzzer)
	{
		bBuzzSwitch.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	else
	{
		bBuzzSwitch.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	return WinStatus;
}

void HandlerSettingsBuzzerScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbBuzzSwitch(void *ptr)
{
	if(Buzzer)
	{
		Buzzer = false;
		bBuzzSwitch.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else
	{
		Buzzer = true;
		bBuzzSwitch.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	BeepBuzzer();
}

void HandlerbSave(void *ptr)
{
	BeepBuzzer();
	objstcSettings.u32BuzzerEnable = Buzzer;
	WriteSettingsBuffer();
    if(enkeyOk == ShowMainPopUp("Buzzer Settings","Buzzer settings saved", true))
    {
    	ChangeWindowPage(en_WinId_SettingsBuzzer , (enWindowID)NULL);
    	stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
    }
}

