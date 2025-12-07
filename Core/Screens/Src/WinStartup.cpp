/*
 * WindowStartup.c
 *
 *  Created on: Aug 1, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestResultsHistory.hpp"
#include "../APPL/Inc/NVM_QCSetup.hpp"
#include <HandlerPheripherals.hpp>
#include "DcMotor.hpp"
#include "Appl_Timer.hpp"
#include "Printer.h"
#include "Lamp.h"
#include "Preamp.h"

typedef enum
{
	en_DarkADC_Idle = 0,
	en_DarkADC_State__Homming,
	en_DarkADC_State__Homming_Busy,
	en_DarkADC_State__FilterBlack,
	en_DarkADC_State__FilterBlack_Busy,
	en_DarkADC_State__Home_Error,
	en_DarkADC_State_SelectingGain,
	en_DarkADC_State_SelectingGainDelay,
	en_DarkADC_Reading_DarkAdc,
	enDarkADC_Task_Completed,
}enDarkADCState;

const enPreampGain GainSelectionArray[en_PreampGain_Max] = {en_PreampGain_OFF,//0
															en_PreampGain_15,//1
															en_PreampGain_7,//2
															en_PreampGain_11,//3
															en_PreampGain_3,//4
															en_PreampGain_13,//5
															en_PreampGain_5,//6
															en_PreampGain_9,//7
															en_PreampGain_1,//8
															en_PreampGain_14,//9
															en_PreampGain_6,//10
															en_PreampGain_10,//11
															en_PreampGain_2,//12
															en_PreampGain_12,//13
															en_PreampGain_4,//14
															en_PreampGain_8};//15

static enDarkADCState g_u8DarkADCState = en_DarkADC_Idle;
static uint16_t AdcDataFetchCount = 0;
uint8_t Index = 0;

static NexProgressBar bProgressBar = NexProgressBar(en_WinId_StartupScreen, 1 , "j0");
static NexPicture ElscionLogo = NexPicture(en_WinId_StartupScreen , 2, "p0");

static NexPicture ElscoinTitle = NexPicture(en_WinId_StartupScreen , 3, "p1");
static NexPicture LabnovaTitle = NexPicture(en_WinId_StartupScreen , 4, "p2");
static NexPicture RivoChemTitle = NexPicture(en_WinId_StartupScreen , 5, "p3");

stcTimer g_StartUpTimer;
stcTimer g_DarkAdcTimer;
uint32_t g_u32StartUpCount = 0;
#define MAX_TIME_IN_SEC_STARTUP (150)
static void DarkADC_Task(void);
static bool StartUpMillsDelay(uint32_t Delay);

enWindowStatus ShowStartUpScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

	if(objstcSettings.Elscion_Model == NVM_OEM_ENABLE_FLAG)
	{
		ElscoinTitle.setVisible(true);
		osDelay(5000);
		ElscoinTitle.setVisible(false);
	}
	else if(objstcSettings.OEM1_Model == NVM_OEM_ENABLE_FLAG)
	{
		LabnovaTitle.setVisible(true);
		osDelay(5000);
		LabnovaTitle.setVisible(false);
	}
	else if(objstcSettings.OEM2_Model  == NVM_OEM_ENABLE_FLAG)
	{
		RivoChemTitle.setVisible(true);
		osDelay(5000);
		RivoChemTitle.setVisible(false);
	}
	else if(objstcSettings.OEM3_Model == NVM_OEM_ENABLE_FLAG)
	{

	}

	bProgressBar.setVisible(true);
	ElscionLogo.setVisible(true);

	StartTimer(&(g_StartUpTimer), 100);
	EnableDisableLamp(en_LampEnable);
	Index = en_PreampGain_OFF ;

	for(uint16_t NoOfGain = en_PreampGain_OFF; NoOfGain < en_PreampGain_Max; ++NoOfGain)
	{
		stcTestData.fPreampDarkADCVal[NoOfGain] = 0;
	}
	g_u8DarkADCState = en_DarkADC_State__Homming;
	return WinStatus;
}

void HandlerStartUpScreen (NexPage *ptr_obJCurrPage)
{
	if(true == Timer_IsTimeout(&(g_StartUpTimer) , enTimerNormalStop))
	{
		StartTimer(&(g_StartUpTimer), 100);
		g_u32StartUpCount++;
		uint32_t u32Percent = mapInt(g_u32StartUpCount , 0 , MAX_TIME_IN_SEC_STARTUP , 0 , 100);
		bProgressBar.setValue(u32Percent);
		if(100 <= u32Percent && g_u8DarkADCState == en_DarkADC_Idle)
		{
			DisableDispResponses();
			g_u32StartUpCount = 0;
			ReadTestParameterBuffer();
			ReadAllTestResultHistory();
			NVM_ReadQcSetup();
			EnableDisableLamp(en_LampDisable);
			Startup_Print();
			enPopUpKeys Key = ShowMainPopUp("System Diagnostics","Do you want to run system diagnostics?" , false);
			if(enKeyYes == Key)
			{
				ChangeWindowPage(en_WinId_SelfTestScreen , (enWindowID)en_WinId_SelfTestScreen);
				EnableDisableLamp(en_LampEnable);
			}
			else
			{
				ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)en_WinID_MainMenuScreen);
				EnableDisableLamp(en_LampEnable);
			}
		}
	}
	DarkADC_Task();
	Pump_RunTask();
}

enDarkADCState Get_ReadDarkADC_TaskState(void)
{
	return g_u8DarkADCState;
}

void DarkADC_Task(void)
{
	switch(g_u8DarkADCState)
	{
		case en_DarkADC_Idle:
		break;
		case en_DarkADC_State__Homming:
		{
			 Aspirate_Pump(500);//Ul
			 SelectFilter(en_FilterHome);
			 StartTimer(&g_DarkAdcTimer , 3 * 1000);/*Homming max delay*/
			 g_u8DarkADCState = en_DarkADC_State__Homming_Busy;
		}break;
		case en_DarkADC_State__Homming_Busy:
		{
			if(true == Timer_IsTimeout(&g_DarkAdcTimer , enTimerNormalStop))
			{
				if(en_FilterMotorBusy != ReadFilterMotorStatus())
				{
					if(en_FilterMotorHomeError == ReadFilterMotorStatus())
					{
						g_u8DarkADCState = en_DarkADC_State__Home_Error;
					}
					else
					{
						g_u8DarkADCState = en_DarkADC_State__FilterBlack;
					}
				}
				else
				{
					 StartTimer(&g_DarkAdcTimer , 1 * 1000);/*Homming max delay*/
				}
			}
		}
		break;
		case en_DarkADC_State__FilterBlack:
		{
			 SelectFilter(en_FilterBlack);
			 StartTimer(&g_DarkAdcTimer , 2 * 1000);/*Homming max delay*/
			 g_u8DarkADCState = en_DarkADC_State__FilterBlack_Busy;
		}break;
		case en_DarkADC_State__FilterBlack_Busy:
		{
			if(true == Timer_IsTimeout(&g_DarkAdcTimer , enTimerNormalStop))
			{
				if(en_FilterMotorBusy != ReadFilterMotorStatus())
				{
					if(en_FilterMotorHomeError == ReadFilterMotorStatus())
					{
						g_u8DarkADCState = en_DarkADC_State__Home_Error;
					}
					else
					{
						g_u8DarkADCState = en_DarkADC_State_SelectingGain;
					}
				}
			}
		}
		break;

		case en_DarkADC_State__Home_Error:
		{
			EnableDisableLamp(en_LampDisable);
			g_u8DarkADCState = en_DarkADC_State_SelectingGain;/*Stop state machine*/
		}
		break;
		case en_DarkADC_State_SelectingGain:
		{
			if(en_PreampGain_15 > Index)
			{
				Index++;
				uint8_t GainArrayIndex = 0;
				 uint32_t DelayTimeMs = 100;
				for(;GainArrayIndex < en_PreampGain_Max ; GainArrayIndex++)//Finding filter gain position in array location
				{
					if(Index == GainSelectionArray[GainArrayIndex])
					{
						break;
					}
				}
				if(Index == 8)
				{
					DelayTimeMs = 5000;
				}
				else if(Index == 4 ||Index == 12)
				{
					DelayTimeMs = 500;
				}
				SetPreampGain(GainSelectionArray[GainArrayIndex]);
				StartTimer(&(g_DarkAdcTimer), DelayTimeMs);
				g_u8DarkADCState = en_DarkADC_State_SelectingGainDelay;
			}
			else
			{
				g_u8DarkADCState = enDarkADC_Task_Completed;/*Stop state machine*/
			}
		}
		break;

		case en_DarkADC_State_SelectingGainDelay:
		{
			if(true == Timer_IsTimeout(&(g_DarkAdcTimer) , enTimerNormalStop))
			{
				g_u8DarkADCState = en_DarkADC_Reading_DarkAdc;
			}
		}
		break;
		case en_DarkADC_Reading_DarkAdc:
		{
			uint32_t BalanceTime = 5 - AdcDataFetchCount;/*Read Dark ADC 10 times for 1 second*/
			if(StartUpMillsDelay(50/*100 millis*/))
			{
				stcTestData.fPreampDarkADCVal[Index] += ReadPhotometryMeasurement((enPreampGain)Index , false);
				AdcDataFetchCount++;
				if(1 >= (BalanceTime))
				{
					stcTestData.fPreampDarkADCVal[Index] /= 5;
					g_u8DarkADCState = en_DarkADC_State_SelectingGain;
					AdcDataFetchCount = 0;
				}
			}
		}
		break;
		case enDarkADC_Task_Completed:
		{
			g_u8DarkADCState = en_DarkADC_Idle;
		}
		break;
	}

}

bool StartUpMillsDelay(uint32_t Delay)
{
  unsigned long currentMillis = HAL_GetTick();
  static unsigned long PrevMills = 0;/*Initilized one time only*/
  int elapsedTime = (int)(currentMillis - PrevMills);
  if(abs((int)elapsedTime) >= Delay)
  {
   PrevMills = currentMillis;
   return true;
  }
  return false;
}

