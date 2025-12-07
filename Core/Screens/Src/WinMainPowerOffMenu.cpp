/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "Appl_Timer.hpp"
#include <HandlerPheripherals.hpp>
#include "Lamp.h"

typedef enum
{
	enStateIdle = 0,
	enStateAspirate,
	enStateAspBusy,
	enStateAspEmpty,
	enStateAspEmptyBusy,
	enStateAspCompleted,
	enStatePowerOffmode,
}enPowerOffState;

typedef enum
{
	enPowerOff = 0,
	enStandby,
}enPowerOfMethod;

enPowerOfMethod g_pwrOffMethod = enPowerOff;
stcTimer g_powerOffTimer;

static enPowerOffState g_PoffState = enStateIdle;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_PowerOffMenu , 2, "b5");
static NexButton bStandby = NexButton(en_WinId_PowerOffMenu , 6, "b4");
static NexButton bPowerOff = NexButton(en_WinId_PowerOffMenu, 5 , "b3");
static NexButton bAspirate = NexButton(en_WinId_PowerOffMenu, 7 , "b1");

static NexText tTextBuffer = NexText(en_WinId_PowerOffMenu , 8 , "t0");

static NexPicture pPoweroffpic = NexPicture(en_WinId_PowerOffMenu , 9 , "p1");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bAspirate,
									 &bStandby,
									 &bPowerOff,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbAspirate(void *ptr);
static void HandlerbPowerOff(void *ptr);
static void HandlerbStandby(void *ptr);
static void PowerOffProcessTask(void);
static void StartPowerOffTask(void);

enWindowStatus ShowMainPowerOffScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bAspirate.attachPush(HandlerbAspirate, &bAspirate);
	bStandby.attachPush(HandlerbStandby, &bStandby);
	bPowerOff.attachPush(HandlerbPowerOff, &bPowerOff);
	pPoweroffpic.setVisible(false);
	tTextBuffer.setText("Aspirate 5mL of DI Water");
	g_pwrOffMethod = enPowerOff;

	bPowerOff.Set_background_image_pic(487);
	bStandby.Set_background_image_pic(488);

	return WinStatus;
}

void HandlerMainPowerOffScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
	if(enPowerOff == g_pwrOffMethod)
	{
		PowerOffProcessTask();
	}
}

void HandlerbBack(void *ptr)
{
	if(	g_PoffState == enStateIdle)
	{
		ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerPowerOffAspButton(void)
{
	if(enPowerOff == g_pwrOffMethod)
	{
		if(	g_PoffState == enStateIdle)
		{
			AspSwitchLed_Red(en_AspLedON);
			AspSwitchLed_White(en_AspLedOFF);

			StartPowerOffTask();
			tTextBuffer.setText("Aspirating");
			BeepBuzzer();
		}
		else
		{
			InstrumentBusyBuzz();
		}
	}

	else if(enStandby == g_pwrOffMethod)
	{
		/*Enter into sleep mode*/
		ChangeWindowPage(en_WinId_PowerSleep , (enWindowID)NULL);
		stcScreenNavigation.PrevSleepWindowId = en_WinId_PowerOffMenu;
		BeepBuzzer();
	}
}

void HandlerbAspirate(void *ptr)
{
	HandlerPowerOffAspButton();
}

void HandlerbPowerOff(void *ptr)
{
	if(	g_PoffState == enStateIdle)
	{
		tTextBuffer.setText("Aspirate 5mL of DI Water");
		bPowerOff.Set_background_image_pic(487);
		bStandby.Set_background_image_pic(488);
		g_pwrOffMethod = enPowerOff;
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerbStandby(void *ptr)
{
	if(	g_PoffState == enStateIdle)
	{
		tTextBuffer.setText("Sleep Mode");
		bPowerOff.Set_background_image_pic(491);
		bStandby.Set_background_image_pic(486);
		g_pwrOffMethod = enStandby;
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void StartPowerOffTask(void)
{
	g_PoffState = enStateAspirate;
}

void PowerOffProcessTask(void)
{
	switch(g_PoffState)
	{
		case enStateIdle:
		{
			/*Idle task*/
		}break;
		case enStateAspirate:
		{
			Aspirate_Pump(5 * 1000);//5ML
			g_PoffState = enStateAspBusy;
		}break;
		case enStateAspBusy:
		{
			if(enPump_AspCompleted == Pump_RunTask())
			{
				g_PoffState = enStateAspEmpty;
				LongBeepBuzzer();
				InitilizeTimer(&(g_powerOffTimer));
				StartTimer(&(g_powerOffTimer), 2000);
			}
		}break;
		case enStateAspEmpty:
		{
			if(true == Timer_IsTimeout(&(g_powerOffTimer) , enTimerNormalStop))
			{
				InitilizeTimer(&(g_powerOffTimer));
				Aspirate_Pump(1000);//1ML
				g_PoffState = enStateAspEmptyBusy;
				tTextBuffer.setText("Emptying");
			}
		}break;
		case enStateAspEmptyBusy:
		{
			if(enPump_AspCompleted == Pump_RunTask())
			{
				g_PoffState = enStateAspCompleted;
			}
		}break;
		case enStateAspCompleted:
		{
			tTextBuffer.setText("Instrument Is Safe To Turn Off");
			pPoweroffpic.setVisible(true);

			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);
			EnableDisableLamp(en_LampDisable);

			g_PoffState = enStatePowerOffmode;

		}break;
		case enStatePowerOffmode:
		{
			Reset_AutoSleepMinsCounterTimer();
		}break;


		default:
		{
			g_PoffState = enStateIdle;
		}break;
	}
}
