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
#include "Lamp.h"

#define MAX_TIME_IN_SEC_SLEEP_EXIT (120)
typedef enum
{
	enSleepIdle = 0,
	enExitSleepEntry,
	enExitSleep,
	enExitSleepExit,
}EN_SLEEP_MODE;

EN_SLEEP_MODE g_SleepModeState = enSleepIdle;
stcTimer g_SleepTimer;
uint32_t g_u32SleepCount = 0;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_PowerSleep , 1, "t0");
static NexButton bSkip = NexButton(en_WinId_PowerSleep , 4, "b0");
static NexText tTextBuffer = NexText(en_WinId_PowerSleep , 5 , "t1");

static NexProgressBar bProgressBar = NexProgressBar(en_WinId_PowerOffMenu, 2 , "j0");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bSkip,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbSkip(void *ptr);
static void SleepModeTask(void);
static void ExitSleepMode(void);

enWindowStatus ShowMainPowerSleepScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bSkip.attachPush(HandlerbSkip, &bSkip);

	bProgressBar.setVisible(false);
	bProgressBar.setValue(0);

	bSkip.setVisible(false);
	tTextBuffer.setVisible(false);
//	bBack.setText("Sleep mode");
	EnableDisableLamp(en_LampDisable);
	return WinStatus;
}

void HandlerMainPowerSleepScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
	SleepModeTask();
	Reset_AutoSleepMinsCounterTimer();
}

void HandlerbBack(void *ptr)
{
	if(g_SleepModeState == enSleepIdle)
	{
		ExitSleepMode();
		BeepBuzzer();
	}
}

void HandlerbSkip(void *ptr)
{
		g_u32SleepCount = 0;
		enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
		enWindowID NextWindow = stcScreenNavigation.NextWindowId;
		ChangeWindowPage(stcScreenNavigation.PrevSleepWindowId , (enWindowID)NULL);
		stcScreenNavigation.PrevWindowId = PrevWindow;
		stcScreenNavigation.NextWindowId = NextWindow;

		g_SleepModeState = enSleepIdle;
		BeepBuzzer();
}

void ExitSleepMode(void)
{
	g_SleepModeState = enExitSleepEntry;
}
void SleepModeTask(void)
{
	switch(g_SleepModeState)
	{
		case enExitSleepEntry:
		{
			InitilizeTimer(&(g_SleepTimer));
			StartTimer(&(g_SleepTimer), 1000);
			g_u32SleepCount = 0;
			g_SleepModeState = enExitSleep;
			EnableDisableLamp(en_LampEnable);
//			bBack.setText("Exiting sleep mode");
			bProgressBar.setVisible(true);
			bSkip.setVisible(true);
			tTextBuffer.setVisible(true);

		}break;

		case enExitSleep:
		{
			if(true == Timer_IsTimeout(&(g_SleepTimer) , enTimerNormalStop))
			{
				char arrBuff[32] = {0};
				StartTimer(&(g_SleepTimer), 1000);
				g_u32SleepCount++;
				snprintf(arrBuff , 32 , "Wake in %d Sec" , 120-(int)g_u32SleepCount);
				tTextBuffer.setText(arrBuff);

				uint32_t u32Percent = mapInt(g_u32SleepCount , 0 , MAX_TIME_IN_SEC_SLEEP_EXIT , 0 , 100);
				bProgressBar.setValue(u32Percent);
				if(100 <= u32Percent)
				{
					g_SleepModeState = enExitSleepExit;
					g_u32SleepCount = 0;
				}
			}
		}break;

		case enExitSleepExit:
		{
			enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
			enWindowID NextWindow = stcScreenNavigation.NextWindowId;
			ChangeWindowPage(stcScreenNavigation.PrevSleepWindowId , (enWindowID)NULL);
			stcScreenNavigation.PrevWindowId = PrevWindow;
			stcScreenNavigation.NextWindowId = NextWindow;

			g_SleepModeState = enSleepIdle;
		}break;

		default:
		case enSleepIdle:
		{
			g_SleepModeState = enSleepIdle;
		}break;
	}
}
