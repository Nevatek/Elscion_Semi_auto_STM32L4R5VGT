/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "Autowash.hpp"

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_SettingsAutowash , 1, "b5");
static NexButton bAutowashEnable = NexButton(en_WinId_SettingsAutowash , 4, "b0");
static NexText bAutowashIndication = NexText(en_WinId_SettingsAutowash , 7, "t2");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bAutowashEnable,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbAutoWashEnable(void *ptr);
static bool NonblockingSettingMillsDelay(void);
enWindowStatus ShowSettingsAutoWashScreen (NexPage *ptr_obJCurrPage)
{
	bool bStatus = false;
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bAutowashEnable.attachPush(HandlerbAutoWashEnable, &bAutowashEnable);

	bStatus = Read_AutowashPin();
	if(true == bStatus)
	{
		bAutowashIndication.setText("ON");
	}
	else
	{
		bAutowashIndication.setText("OFF");
	}

	if(true == objstcSettings.u32AutoWashEnable)
	{
		bAutowashEnable.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	else
	{
		bAutowashEnable.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	return WinStatus;
}

void HandlerSettingsAutoWashScreen (NexPage *ptr_obJCurrPage)
{
	bool bStatus = false;
	if(true == NonblockingSettingMillsDelay())
	{
		bStatus = Read_AutowashPin();
		if(true == bStatus)
		{
			bAutowashIndication.setText("ON");
		}
		else
		{
			bAutowashIndication.setText("OFF");
		}
	}
	nexLoop(nex_Listen_List);

}
void HandlerbAutoWashEnable(void *ptr)
{
	if(true == objstcSettings.u32AutoWashEnable)
	{
		objstcSettings.u32AutoWashEnable = false;
		bAutowashEnable.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else
	{
		objstcSettings.u32AutoWashEnable = true;
		bAutowashEnable.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	/*save settings*/
	WriteSettingsBuffer();
}
void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}
bool NonblockingSettingMillsDelay(void)
{
  #define DELAY_MS (500)
  unsigned long currentMillis = HAL_GetTick();
  static unsigned long PrevMills = 0;/*Initilized one time only*/
  int elapsedTime = (int)(currentMillis - PrevMills);
  if(abs((int)elapsedTime) >= DELAY_MS)
  {
   PrevMills = currentMillis;
   return true;
  }
  return false;
}
