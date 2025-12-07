/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "HandlerPheripherals.hpp"
#include "Autowash.hpp"

#define DEFAULT_PUMP_AUTOWASH_ASP_VOL_UL (1000)
typedef enum
{
	enDiagAutoWashPump_Idle = 0,
	enDiagAutoWashPump_Asp1000Ul,
	enDiagAutoWashPump_Asp1000Ul_Busy,
	enDiagAutoWashPump_Asp1000Ul_Completed,
}en_DiagAutoWashPumpState;

static en_DiagAutoWashPumpState g_DiagAutoWashPumpState = enDiagAutoWashPump_Idle;

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_Diag_AutoWash_B , 2 , "b5");
static NexButton bProbeCheck = NexButton(en_WinId_Diag_AutoWash_B , 5 , "b3");
static NexButton bFludicCheck = NexButton(en_WinId_Diag_AutoWash_B , 6 , "b4");
static NexButton bAspirate = NexButton(en_WinId_Diag_AutoWash_B , 7 , "b1");
static NexText tWashStatus = NexText(en_WinId_Diag_AutoWash_A, 8 , "t0");
static NexText tFluidicSensorStatus = NexText(en_WinId_Diag_AutoWash_A, 11 , "t6");
static NexText tValveStatus = NexText(en_WinId_Diag_AutoWash_A, 12 , "t7");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bProbeCheck,
									 &bFludicCheck,
									 &bAspirate,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbProbeCheck(void *ptr);
static void HandlerbFludicCheck(void *ptr);
static void HandlerbAspirate(void *ptr);
static en_DiagAutoWashPumpState Get_DiagAutoWashPumpStatus(void);
static bool NonBlockMillsDelay(uint32_t u32DelayMs);
static void ShowValveStatus(void);
static void ShowFluidicSensorStatus(void);
static void Pump_DiagAutoWashTask(void);
static void Start_AutoWashPumpAsp(void);


enWindowStatus ShowDiagScreen_AutoWash_B (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bProbeCheck.attachPush(HandlerbProbeCheck, &bProbeCheck);
	bFludicCheck.attachPush(HandlerbFludicCheck, &bFludicCheck);
	bAspirate.attachPush(HandlerbAspirate, &bAspirate);

	if(enDiagAutoWashPump_Idle != Get_DiagAutoWashPumpStatus())
	{
		tWashStatus.setText("Washing");
		bAspirate.Set_background_image_pic(355);
	}
	else
	{
		tWashStatus.setText("Start Wash");
		bAspirate.Set_background_image_pic(354);
	}
	ShowValveStatus();
	ShowFluidicSensorStatus();

	return WinStatus;
}

void HandlerDiagScreen_AutoWash_B (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
	Pump_DiagAutoWashTask();
	if(true == NonBlockMillsDelay(1000))/*For each second*/
	{
		if(enDiagAutoWashPump_Idle != Get_DiagAutoWashPumpStatus())
		{
			tWashStatus.setText("Washing");
			bAspirate.Set_background_image_pic(355);
		}
		else
		{
			tWashStatus.setText("Start Wash");
			bAspirate.Set_background_image_pic(354);
		}
		ShowValveStatus();
		ShowFluidicSensorStatus();
	}
}

void ShowValveStatus(void)
{
	if(en_ValvePosProbe == Get_ValveState())
	{
		tValveStatus.setText("Probe");
	}
	else if(en_ValvePosWaterTank == Get_ValveState())
	{
		tValveStatus.setText("Water Tank");
	}
}
void ShowFluidicSensorStatus(void)
{
	if(true == Read_FluidicSensorPin())
	{
		tFluidicSensorStatus.setText("ON");
	}
	else
	{
		tFluidicSensorStatus.setText("OFF");
	}
}
void HandlerbBack(void *ptr)
{
	if(enDiagAutoWashPump_Idle == Get_DiagAutoWashPumpStatus())
	{
		ChangeWindowPage(en_WinId_DiagnosticsScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
}

void HandlerbProbeCheck(void *ptr)
{
	if(enDiagAutoWashPump_Idle == Get_DiagAutoWashPumpStatus())
	{
		ChangeWindowPage(en_WinId_Diag_AutoWash_A , (enWindowID)NULL);
		BeepBuzzer();
	}
}

void HandlerbFludicCheck(void *ptr)
{

}
void HandlerbAspirate(void *ptr)
{
	if(enDiagAutoWashPump_Idle != Get_DiagAutoWashPumpStatus())
	{
		return;
	}
	BeepBuzzer();
	bAspirate.Set_background_image_pic(355);
	Start_AutoWashPumpAsp();
	tWashStatus.setText("Washing");
}

en_DiagAutoWashPumpState Get_DiagAutoWashPumpStatus(void)
{
	return g_DiagAutoWashPumpState;
}

void Start_AutoWashPumpAsp(void)
{
	g_DiagAutoWashPumpState = enDiagAutoWashPump_Asp1000Ul;
}

void Pump_DiagAutoWashTask(void)
{
	switch(g_DiagAutoWashPumpState)
	{
		case enDiagAutoWashPump_Idle:
		{

		}break;
		case enDiagAutoWashPump_Asp1000Ul:
		{
			Aspirate_Pump(DEFAULT_PUMP_AUTOWASH_ASP_VOL_UL);
			g_DiagAutoWashPumpState = enDiagAutoWashPump_Asp1000Ul_Busy;
		}break;
		case enDiagAutoWashPump_Asp1000Ul_Busy:
		{
			if(enPump_AspCompleted == Pump_RunTask())
			{
				g_DiagAutoWashPumpState = enDiagAutoWashPump_Asp1000Ul_Completed;
			}
		}break;
		case enDiagAutoWashPump_Asp1000Ul_Completed:
		{
			g_DiagAutoWashPumpState = enDiagAutoWashPump_Idle;
			bAspirate.Set_background_image_pic(354);
		}break;
	}
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
