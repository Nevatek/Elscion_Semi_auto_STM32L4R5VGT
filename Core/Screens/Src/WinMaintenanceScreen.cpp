/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "Appl_Timer.hpp"
#include <HandlerPheripherals.hpp>

#define MAINTANANCE_WASH_HOLDING_TIME_MSEC (2000)

typedef enum
{
	enHalfDay = 0,
	enEndDay,
}enDay;

typedef enum
{
	enMaintanaceStateIdle = 0,
	enMaintanaceStateAspEthanol,
	enMaintanaceStateAspDetergent,
	enMaintanaceStateAspDiWater,
	enMaintanaceStateWashingInprogress,
	enMaintanaceStateWashHold,
	enMaintanaceStateFlushOut,
	enMaintanaceStateWashComplete,
}MAINTANACE_STATES;

MAINTANACE_STATES g_CurrMaintState = enMaintanaceStateIdle;
bool g_WashSequenceEnable = false;
enDay m_enDay = enHalfDay;
stcTimer g_StcTimer;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bMaintenanceBack = NexButton(en_WinId_MaintananceScreen , 2, "b5");
static NexButton bAspButton = NexButton(en_WinId_MaintananceScreen , 8, "b1");
static NexButton bHalfDay = NexButton(en_WinId_MaintananceScreen , 5, "b2");
static NexButton bEndDay = NexButton(en_WinId_MaintananceScreen , 6, "b3");
static NexText tTextBuffer = NexText(en_WinId_MaintananceScreen , 7 , "t0");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bMaintenanceBack,
									 &bAspButton,
									 &bHalfDay,
									 &bEndDay,
									 NULL};

static void HandlerbMaintenanceBack(void *ptr);
static void HandlerbAspButton(void *ptr);
static void HandlerbHalfDay(void *ptr);
static void HandlerbEndDay(void *ptr);

static void StartDayMaintanance(enDay m_Day);
static void Maintanace_Task(void);
static void EnableWashSequence(bool State);

enWindowStatus ShowMaintananceScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bMaintenanceBack.attachPush(HandlerbMaintenanceBack, &bMaintenanceBack);
	bAspButton.attachPush(HandlerbAspButton, &bAspButton);
	bHalfDay.attachPush(HandlerbHalfDay, &bHalfDay);
	bEndDay.attachPush(HandlerbEndDay, &bEndDay);

	m_enDay = enHalfDay;
	g_CurrMaintState = enMaintanaceStateIdle;
	tTextBuffer.setText("Aspirate 5mL of 70% Ethanol");
	bAspButton.Set_background_image_pic(63);
	EnableWashSequence(false);
	return WinStatus;
}

void HandlerMaintananceScreen (NexPage *ptr_obJCurrPage)
{
	Maintanace_Task();
	nexLoop(nex_Listen_List);
}

void HandlerbMaintenanceBack(void *ptr)
{
	if(enMaintanaceStateIdle == g_CurrMaintState && g_WashSequenceEnable == false)
	{
		ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}

void HandlerMaintenanceAspButton(void)
{
	if(enMaintanaceStateIdle == g_CurrMaintState)
	{
		AspSwitchLed_Red(en_AspLedON);
		AspSwitchLed_White(en_AspLedOFF);

		StartDayMaintanance(m_enDay);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}

void HandlerbAspButton(void *ptr)
{
	HandlerMaintenanceAspButton();
}

void HandlerbHalfDay(void *ptr)
{
	if(enMaintanaceStateIdle == g_CurrMaintState && g_WashSequenceEnable == false)
	{
		m_enDay = enHalfDay;
		bHalfDay.Set_background_image_pic(428);
		bEndDay.Set_background_image_pic(429);
		tTextBuffer.setText("Aspirate 5mL of 70% Ethanol");
		EnableWashSequence(false);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}

void HandlerbEndDay(void *ptr)
{
	if(enMaintanaceStateIdle == g_CurrMaintState && g_WashSequenceEnable == false)
	{
		m_enDay = enEndDay;
		bHalfDay.Set_background_image_pic(427);
		bEndDay.Set_background_image_pic(433);
		tTextBuffer.setText("Aspirate 5mL of Detergent");
		EnableWashSequence(false);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void EnableWashSequence(bool State)
{
	g_WashSequenceEnable = State;
}
void StartDayMaintanance(enDay m_Day)
{
	if(false == g_WashSequenceEnable)
	{
		if(enHalfDay == m_Day)
		{
			g_CurrMaintState = enMaintanaceStateAspEthanol;
		}
		else if(enEndDay == m_Day)
		{
			g_CurrMaintState = enMaintanaceStateAspDetergent;
		}
	}
	else/*Wash sequence enabled*/
	{
		g_CurrMaintState = enMaintanaceStateAspDiWater;
	}

	InitilizeTimer(&g_StcTimer);
	/*Show wash in progress message*/
	tTextBuffer.setText("Aspirating");/*Show wash completed message*/
	bAspButton.Set_background_image_pic(76);
}

void Maintanace_Task(void)
{
	switch(g_CurrMaintState)
	{
		case enMaintanaceStateIdle:
		break;
		case enMaintanaceStateAspEthanol:
		{
			Aspirate_Pump(5 * 1000);/*5 ML*/
			g_CurrMaintState = enMaintanaceStateWashingInprogress;
		}
		break;
		case enMaintanaceStateAspDetergent:
		{
			Aspirate_Pump(5 * 1000);/*5 ML*/
			g_CurrMaintState = enMaintanaceStateWashingInprogress;
		}
		break;
		case enMaintanaceStateAspDiWater:
		{
			Aspirate_Pump(5 * 1000);/*5 ML*/
			g_CurrMaintState = enMaintanaceStateWashingInprogress;
		}
		break;
		case enMaintanaceStateWashingInprogress:
		{
			if(enPump_AspCompleted == Pump_RunTask())
			{
				g_CurrMaintState = enMaintanaceStateWashHold;
				LongBeepBuzzer();
				tTextBuffer.setText("Washing in Progress");/*Show wash completed message*/
				StartTimer(&g_StcTimer , MAINTANANCE_WASH_HOLDING_TIME_MSEC);/*Holding time*/
			}
		}
		break;
		case enMaintanaceStateWashHold:
		{
			if(true == Timer_IsTimeout(&g_StcTimer , enTimerNormalStop))
			{
				g_CurrMaintState = enMaintanaceStateFlushOut;
				Aspirate_Pump(1000);/*5 ML*/
				tTextBuffer.setText("Flushing out");/*Show wash completed message*/
			}
		}
		break;
		case enMaintanaceStateFlushOut:
		{
			if(enPump_AspCompleted == Pump_RunTask())
			{
				g_CurrMaintState = enMaintanaceStateWashComplete;
				tTextBuffer.setText("Wash Completed");/*Show wash completed message*/
			}
		}
		break;
		case enMaintanaceStateWashComplete:
		{
			if(false == g_WashSequenceEnable)
			{
				EnableWashSequence(true);
				tTextBuffer.setText("Aspirate 5mL of DI Water");/*Show aspirate DI water message*/
			}
			else/*If wash sequence is enabled*/
			{
				EnableWashSequence(false);
				if(enHalfDay == m_enDay)
				{
					tTextBuffer.setText("Aspirate 5mL of 70% Ethanol");
				}
				else if(enEndDay == m_enDay)
				{
					tTextBuffer.setText("Aspirate 5mL of Detergent");
				}
			}
			bAspButton.Set_background_image_pic(63);
			g_CurrMaintState = enMaintanaceStateIdle;
			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);
		}
		break;
	}
}
