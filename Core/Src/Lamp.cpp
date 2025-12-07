/*
 * Lamp.c
 *
 *  Created on: Jul 14, 2022
 *      Author: Alvin
 */

#include "main.h"
#include "cmsis_os.h"
#include "Lamp.h"
#include "../APPL/Inc/NonVolatileMemory.h"
#include "../Screens/Inc/Screens.h"
#include "../Screens/Inc/CommonDisplayFunctions.h"

static bool LampMillsDelay(uint32_t Delay);
static enLamp g_LampState = en_LampDisable;
static uint8_t LampStsSavecount = 0;
static bool LampsettingsSaved = false;
#define LAMP_SAVE_INTERVAL_MIN  15

void EnableDisableLamp(enLamp enStatus)
{
	g_LampState = enStatus;
	HAL_GPIO_WritePin(LampEnable_GPIO_Port, LampEnable_Pin, GPIO_PinState(g_LampState));
}
void Reset_LampWorkHours(void)
{
	objstcSecondarySettings.u32LampWorkMinutes = objstcSecondarySettings.u32LampWorkHours = 0;
}
enLamp Get_LampWorkState(void)
{
	return g_LampState;
}
uint32_t Get_LampWorkHours(void)
{
	float totalHours = objstcSecondarySettings.u32LampWorkHours + (objstcSecondarySettings.u32LampWorkMinutes / 60.0f);
	return totalHours;
}
//void Lamp_Task(void)
//{
//	if(en_LampEnable == Get_LampWorkState() &&
//			true == LampMillsDelay(1000 * 60))/*for each minute*/
//	{
//		objstcSecondarySettings.u32LampWorkMinutes++;
//
//		if(60 <= objstcSecondarySettings.u32LampWorkMinutes)
//		{
//			objstcSecondarySettings.u32LampWorkMinutes = 0;
//			objstcSecondarySettings.u32LampWorkHours++;
//
//			if(objstcSecondarySettings.u32LampWorkHours > 1800 && enTestStatus_Idle == GetInstance_TestData()->CurrentTestStatus && stcScreenNavigation.CurrentWindowId != en_WinId_PowerSleep)
//			{
//				enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
//				enWindowID NextWindow = stcScreenNavigation.NextWindowId;
//				enWindowID CurrWindowID = stcScreenNavigation.CurrentWindowId;
//
//				if(enkeyOk == ShowMainPopUp("Lamp","Lamp Life is Less than 10 percentage\\rPlease replace as soon as possible", true))
//				{
//					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//					ChangeWindowPage((enWindowID)CurrWindowID , (enWindowID)NextWindow);
//					stcScreenNavigation.PrevWindowId = PrevWindow;
//					stcScreenNavigation.NextWindowId = NextWindow;
//				}
//			}
//		}
//		/*save settings*/
//		WriteSecondarySettingsBuffer();
//	}
//}


void Lamp_Task(void)
{
//	if(en_LampEnable == Get_LampWorkState() &&	true == LampMillsDelay(1000 * 60))/*for each minute*/

	if(true == LampMillsDelay(1000 * 60))/*for each minute*/
	{
		if(en_LampEnable == Get_LampWorkState())
		{
			objstcSecondarySettings.u32LampWorkMinutes++;
			LampStsSavecount++;
			LampsettingsSaved = false;
			if(60 <= objstcSecondarySettings.u32LampWorkMinutes)
			{
				objstcSecondarySettings.u32LampWorkMinutes = 0;
				objstcSecondarySettings.u32LampWorkHours++;

				if(objstcSecondarySettings.u32LampWorkHours > 1800 && enTestStatus_Idle == GetInstance_TestData()->CurrentTestStatus && stcScreenNavigation.CurrentWindowId != en_WinId_PowerSleep)
				{
					enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
					enWindowID NextWindow = stcScreenNavigation.NextWindowId;
					enWindowID CurrWindowID = stcScreenNavigation.CurrentWindowId;

					if(enkeyOk == ShowMainPopUp("Lamp","Lamp Life is Less than 10 percentage\\rPlease replace as soon as possible", true))
					{
						stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
						ChangeWindowPage((enWindowID)CurrWindowID , (enWindowID)NextWindow);
						stcScreenNavigation.PrevWindowId = PrevWindow;
						stcScreenNavigation.NextWindowId = NextWindow;
					}
				}
			}

			if(LampStsSavecount >= LAMP_SAVE_INTERVAL_MIN)
			{
				WriteSecondarySettingsBuffer();
				LampStsSavecount = 0;
			}
		}
		else
		{
			if(LampsettingsSaved == false)
			{
				WriteSecondarySettingsBuffer();
				LampStsSavecount = 0;
				LampsettingsSaved = true;
			}
		}

		/*save settings*/
	}

}


bool LampMillsDelay(uint32_t Delay)
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
