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

static NexButton bBack = NexButton(en_WinId_Diag_AutoWash_A , 2, "b5");
static NexButton bProbeCheck = NexButton(en_WinId_Diag_AutoWash_A , 5 , "b3");
static NexButton bFludicCheck = NexButton(en_WinId_Diag_AutoWash_A , 6 , "b4");
static NexPicture pAutoWashDoorIcon = NexPicture(en_WinId_Diag_AutoWash_A , 9, "p0");
static NexText tAutoWashStatus = NexText(en_WinId_Diag_AutoWash_A, 8 , "t4");

static bool g_bPrevAutowashDoorStatus = false;
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bProbeCheck,
									 &bFludicCheck,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbProbeCheck(void *ptr);
static void HandlerbFludicCheck(void *ptr);
static bool NonBlockMillsDelay(uint32_t u32DelayMs);
static void ShowDoorStatus(en_AutoWashDoorStatus Status);

enWindowStatus ShowDiagScreen_AutoWash_A (NexPage *ptr_obJCurrPage)
{
	bool bDoorStatus = false;
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bProbeCheck.attachPush(HandlerbProbeCheck, &bProbeCheck);
	bFludicCheck.attachPush(HandlerbFludicCheck, &bFludicCheck);

	bDoorStatus = Read_AutowashPin();
	if(true == bDoorStatus)
	{
		ShowDoorStatus(enAutoWashDoorOpen);
	}
	else if(false == bDoorStatus)
	{
		ShowDoorStatus(enAutoWashDoorClosed);
	}

	return WinStatus;
}

void HandlerDiagScreen_AutoWash_A (NexPage *ptr_obJCurrPage)
{
	bool bDoorStatus = false;
	nexLoop(nex_Listen_List);
	if(true == NonBlockMillsDelay(1000))/*For each second*/
	{
		bDoorStatus = Read_AutowashPin();
		if(true == bDoorStatus && false == g_bPrevAutowashDoorStatus)
		{
			ShowDoorStatus(enAutoWashDoorOpen);
		}
		else if(false == bDoorStatus && true == g_bPrevAutowashDoorStatus)
		{
			ShowDoorStatus(enAutoWashDoorClosed);
		}
		g_bPrevAutowashDoorStatus = bDoorStatus;
	}
}
void ShowDoorStatus(en_AutoWashDoorStatus Status)
{
	if(enAutoWashDoorOpen == Status)
	{
		pAutoWashDoorIcon.Set_background_image_pic(383);
		tAutoWashStatus.setText("Open");
	}
	else
	{
		pAutoWashDoorIcon.Set_background_image_pic(384);
		tAutoWashStatus.setText("Closed");
	}
}
void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_DiagnosticsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbProbeCheck(void *ptr)
{

}
void HandlerbFludicCheck(void *ptr)
{
	ChangeWindowPage(en_WinId_Diag_AutoWash_B , (enWindowID)NULL);
	BeepBuzzer();
}

bool NonBlockMillsDelay(uint32_t u32DelayMs)
{
  unsigned long currentMillis = HAL_GetTick();
  static unsigned long PrevMills = 0;/*Initilized one time only*/
  int elapsedTime = (int)(currentMillis - PrevMills);
  if(abs((int)elapsedTime) >= u32DelayMs)
  {
   PrevMills = currentMillis;
   return true;
  }
  return false;
}
