/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "HandlerPheripherals.hpp"
#include "../APPL/Inc/Testrun.h"
#include "../../APPL/Inc/NVM_TestResultsHistory.hpp"
#include "../APPL/Inc/NVM_QCSetup.hpp"
#include "Printer.h"
#include "DcMotor.hpp"
#include "Appl_Timer.hpp"

typedef enum
{
	en_HomeWashProcess_Idle = 0,
	en_HomeWashProcess_1,
	en_HomeWashProcess_1_Busy,
	en_HomeWashProcess_2,
	en_HomeWashProcess_2_Busy,
	en_HomeWashProcess_3,
	en_HomeWashProcess_3_Busy,
	en_HomeWashProcess_AspPreDelay,
	en_HomeWashProcess_AspAirGap,
	en_HomeWashProcess_AspAirGapdelay,
	en_HomeWashProcess_BetweenSampleCompleted,
	en_HomeWashProcess_Completed,
}en_HomeWashProcess;

static en_HomeWashProcess g_HomeWashProcessState = en_HomeWashProcess_Idle;
static stcTimer g_airGapTimer;

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
static NexButton bTestParams = NexButton(en_WinID_MainMenuScreen , 4, "b5");
static NexButton bTestRun = NexButton(en_WinID_MainMenuScreen , 1 , "b0");
static NexButton bTestHistory = NexButton(en_WinID_MainMenuScreen , 3, "b2");
static NexButton bQC = NexButton(en_WinID_MainMenuScreen , 2, "b1");
static NexButton bMaintanance = NexButton(en_WinID_MainMenuScreen , 9, "b3");
static NexButton bSettings = NexButton(en_WinID_MainMenuScreen , 10, "b4");
static NexButton bFavScreen = NexButton(en_WinID_MainMenuScreen , 12, "b10");
static NexButton bDiagnostics = NexButton(en_WinID_MainMenuScreen , 8, "b7");
static NexButton bWash = NexButton(en_WinID_MainMenuScreen , 7, "b8");
static NexButton bPowerOff = NexButton(en_WinID_MainMenuScreen , 11, "b6");
static NexButton bPrinterFeed = NexButton(en_WinID_MainMenuScreen , 6, "b9");

static NexText textUserName = NexText(en_WinID_MainMenuScreen , 21 , "t3");

static NexText textDateTime = NexText(en_WinID_MainMenuScreen , 15 , "t6");
static NexText textPeltTemp = NexText(en_WinID_MainMenuScreen , 14 , "t7");

static NexText textIncuTemp = NexText(en_WinID_MainMenuScreen , 18 , "t1");
static NexText textIncuTempC = NexText(en_WinID_MainMenuScreen , 19 , "t2");
static NexPicture pIncubator = NexPicture(en_WinID_MainMenuScreen , 17 , "p2");
static NexPicture pUserlogin = NexPicture(en_WinID_MainMenuScreen , 20 , "p3");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bTestParams,
									 &bTestRun,
									 &bTestHistory,
									 &bQC,
									 &bMaintanance,
									 &bSettings,
									 &bFavScreen,
									 &bDiagnostics,
									 &bWash,
									 &bPowerOff,
									 &bPrinterFeed,
									 &pUserlogin,
									 NULL};

static void HandlerButtonTestParams(void *ptr);
static void HandlerButtonbTestRun(void *ptr);
static void HandlerButtonbTestHistory(void *ptr);
static void HandlerButtonbQC(void *ptr);
static void HandlerButtonbMaintanance(void *ptr);
static void HandlerButtonbSettings(void *ptr);
static void HandlerbFavScreen(void *ptr);
static void HandlerbDiagnostics(void *ptr);
static void HandlerbWash(void *ptr);
static void HandlerbPowerOff(void *ptr);
static void HandlerbPrintFeed(void *ptr);
static void HandlerUserlogin(void *ptr);
void WashProcedure_Home(void);

enWindowStatus ShowWMainMenuScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	UpdateHomeSreenDateTimeTemp(&textDateTime, &textPeltTemp,  &textIncuTemp, &textUserName);

	bTestParams.attachPush(HandlerButtonTestParams, &bTestParams);
	bTestRun.attachPush(HandlerButtonbTestRun, &bTestRun);
	bTestHistory.attachPush(HandlerButtonbTestHistory, &bTestHistory);
	bQC.attachPush(HandlerButtonbQC, &bQC);
	bMaintanance.attachPush(HandlerButtonbMaintanance, &bMaintanance);
	bSettings.attachPush(HandlerButtonbSettings, &bSettings);
	bFavScreen.attachPush(HandlerbFavScreen, &bFavScreen);
	bDiagnostics.attachPush(HandlerbDiagnostics, &bDiagnostics);
	bWash.attachPush(HandlerbWash, &bWash);
	bPowerOff.attachPush(HandlerbPowerOff, &bPowerOff);
	bPrinterFeed.attachPush(HandlerbPrintFeed, &bPrinterFeed);
	pUserlogin.attachPush(HandlerUserlogin, &pUserlogin);

	if(objstcSettings.fTempControlOnOff[en_ChamperTemp] == true)
	{
		textIncuTemp.setVisible(true);
		textIncuTempC.setVisible(true);
		pIncubator.setVisible(true);
	}
	ConfigAutoSleepTimer(objstcSettings.u16LampDelayTime);
	return WinStatus;
}

void HandlerMainMenuScreen (NexPage *ptr_obJCurrPage)
{
	if(en_WinId_StartupScreen == stcScreenNavigation.PrevWindowId || en_WinId_SelfTestScreen == stcScreenNavigation.PrevWindowId)
	{
		if(stcMemoryPeripherals.InitMemoryfailed == true || stcMemoryPeripherals.SettingsMemCorrpt == true ||
				stcMemoryPeripherals.QCResultBackUpMemCorrpt == true || stcMemoryPeripherals.QCSetupBackUpMemCorrpt == true ||
				stcMemoryPeripherals.SettingsBackUpMemCorrpt == true || stcMemoryPeripherals.TestParamsBackUpMemCorrpt == true ||
				stcMemoryPeripherals.TestResultBackUpMemCorrpt == true || stcMemoryPeripherals.SecondarySettingsMemCorrpt == true )
		{
			MemoryCorruptionBuzzer();
			if(enkeyOk == ShowMainPopUp("Warning","Memory issues found\\rPlease contact service", true))
			{
				ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
				stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
				InitializesystemPeripherals();
			}
		}
		if(en_FilterMotorBusy != ReadFilterMotorStatus())
		{
			if(en_FilterMotorHomeError == ReadFilterMotorStatus())
			{
				if(enkeyOk == ShowMainPopUp("Warning","Filter Wheel sensor error\\rPlease contact service", true))
				{
					ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
					stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
				}
			}
			else
			{
				stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
			}
		}
		if(en_FilterMotorHomeError != ReadFilterMotorStatus())
		{
			for(int nI = 0; nI < en_PreampGain_Max; ++nI)
			{
				if(stcTestData.fPreampDarkADCVal[nI] > 300)
				{
					if(enkeyOk == ShowMainPopUp("Warning","Optical light path issue found\\rPlease restart the device", true))
					{
						ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
						stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
						break;
					}
				}
			}
				stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
		}
	}

	Pump_RunTask();
	if(StatusBarUpdateMillsDelay())
	{
		UpdateHomeSreenDateTimeTemp(&textDateTime, &textPeltTemp,  &textIncuTemp, &textUserName);

		if(	stcMemoryPeripherals.QCResultBackUpMemCorrpt == true || stcMemoryPeripherals.QCSetupBackUpMemCorrpt == true ||
			stcMemoryPeripherals.SettingsBackUpMemCorrpt == true || stcMemoryPeripherals.TestParamsBackUpMemCorrpt == true ||
			stcMemoryPeripherals.TestResultBackUpMemCorrpt == true || stcMemoryPeripherals.SecondarySettingsBackUpMemCorrpt == true)
		{
			MemoryCorruptionBuzzer();

			if(stcMemoryPeripherals.SettingsBackUpMemCorrpt == true)
			{
				if(enkeyOk == ShowMainPopUp("Warning","Settings memory issue found\\rPlease contact service", true))
				{
					ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
					stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
					stcMemoryPeripherals.SettingsBackUpMemCorrpt = false;
				}
			}
			if(stcMemoryPeripherals.SecondarySettingsBackUpMemCorrpt == true)
			{
				if(enkeyOk == ShowMainPopUp("Warning","Sec. Settings memory issue found\\rPlease contact service", true))
				{
					ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
					stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
					stcMemoryPeripherals.SecondarySettingsBackUpMemCorrpt = false;
				}
			}
			if(stcMemoryPeripherals.TestParamsBackUpMemCorrpt == true)
			{
				if(enkeyOk == ShowMainPopUp("Warning","Test setup memory issue found\\rPlease contact service", true))
				{
					ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
					stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
					stcMemoryPeripherals.TestParamsBackUpMemCorrpt = false;
				}
			}
			if(stcMemoryPeripherals.TestResultBackUpMemCorrpt == true)
			{
				if(enkeyOk == ShowMainPopUp("Warning","Test result memory issue found\\rPlease contact service", true))
				{
					ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
					stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
					stcMemoryPeripherals.TestResultBackUpMemCorrpt = false;
				}
			}
			if(stcMemoryPeripherals.QCSetupBackUpMemCorrpt == true)
			{
				if(enkeyOk == ShowMainPopUp("Warning","QC setup memory issue found\\rPlease contact service", true))
				{
					ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
					stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
					stcMemoryPeripherals.QCSetupBackUpMemCorrpt = false;
				}
			}
			if(stcMemoryPeripherals.QCResultBackUpMemCorrpt == true)
			{
				if(enkeyOk == ShowMainPopUp("Warning","QC result memory issue found\\rPlease contact service", true))
				{
					ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
					stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
					stcMemoryPeripherals.QCResultBackUpMemCorrpt = false;
				}
			}
		}
	}
	nexLoop(nex_Listen_List);
	WashProcedure_Home();
}

void WashProcedure_Home(void)
{
	switch (g_HomeWashProcessState)
	{
		case en_HomeWashProcess_Idle:
		{

		}break;

		case en_HomeWashProcess_1:
		{
			AspSwitchLed_Red(en_AspLedON);
			AspSwitchLed_White(en_AspLedOFF);
			Aspirate_Pump(objstcSettings.fWashVoleume_uL);// Manual wash
			g_HomeWashProcessState = en_HomeWashProcess_1_Busy;
		}break;

		case en_HomeWashProcess_1_Busy:
		{
			 if(enPump_AspCompleted == Pump_RunTask())
			 {
				LongBeepBuzzer();
				StartTimer(&g_airGapTimer , 2 * 1000);/*Homming max delay*/
				g_HomeWashProcessState = en_HomeWashProcess_AspPreDelay;
			 }
		}break;

//		case en_HomeWashProcess_2:
//		{
//			 ThreeWayValeCntrl(en_ValvePosWaterTank);
//			 if(stcTestData.CurrentTestStatus == enTestStatus_Wash_Busy)
//			 {
//				 Aspirate_Pump(objstcSettings.fWashVoleume_uL / 2);//auto empty
//			 }
//			 else
//			 {
//
//			 }
//			 g_HomeWashProcessState = en_HomeWashProcess_2_Busy;
//		}break;
//		case en_HomeWashProcess_2_Busy:
//		{
//			 if(enPump_AspCompleted == Pump_RunTask())
//			 {
//
//				 if(WashBtwSample)
//				 {
//					 g_HomeWashProcessState = en_HomeWashProcess_BetweenSampleCompleted;
//					 WashBtwSample = false;
//				 }
//				 else
//				 {
//						if(Manual_Wash)
//						{
//
//						}
//						else
//						{
//							 g_HomeWashProcessState = en_HomeWashProcess_Completed;
//						}
//				 }
//			 }
//		}break;
//		case en_HomeWashProcess_3:
//		{
//			 ThreeWayValeCntrl(en_ValvePosProbe);
//			 Aspirate_Pump(250);//Ul
//			 g_HomeWashProcessState = en_HomeWashProcess_3_Busy;
//		}break;
//		case en_HomeWashProcess_3_Busy:
//		{
//			 if(enPump_AspCompleted == Pump_RunTask())
//			 {
//				 g_HomeWashProcessState = en_HomeWashProcess_Completed;
//			 }
//		}break;
		case en_HomeWashProcess_AspPreDelay:
		{
			if(true == Timer_IsTimeout(&g_airGapTimer , enTimerNormalStop))
			{
				g_HomeWashProcessState = en_HomeWashProcess_AspAirGap;
			}
		}break;

		case en_HomeWashProcess_AspAirGap:
		{
			Aspirate_Pump(objstcSettings.fWashVoleume_uL / 2);//Aspiration of Air gap //added
			g_HomeWashProcessState = en_HomeWashProcess_AspAirGapdelay;
		}break;

		case en_HomeWashProcess_AspAirGapdelay:
		{
			if(enPump_AspCompleted == Pump_RunTask())
			{
				g_HomeWashProcessState = en_HomeWashProcess_Completed;
			}
		}break;

		case en_HomeWashProcess_Completed:
		{
			g_HomeWashProcessState = en_HomeWashProcess_Idle;
			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);
		}break;
	}
}


void HandlerbPowerOff(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		ChangeWindowPage(en_WinId_PowerOffMenu , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerButtonTestParams(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		ChangeWindowPage(en_WinID_TestSelectionScreen , en_WinId_TestParamsGeneral);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerButtonbTestRun(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		stcTestData.CurrentMeasType = en_DIWaterAdcBlank;/*Initialize menu with DI water blank test procedure*/
		stcTestData.SelectedStandard = 0;/*Initialize menu with S0*/
	//	ReadAllTestResultHistory();
		ChangeWindowPage(en_WinID_TestSelectionScreen , en_WinId_TestRun);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerButtonbTestHistory(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		ChangeWindowPage(en_WinID_HistSearch , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}

}
void HandlerButtonbQC(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		if((GetInstance_TestParams()[stcTestData.TestId].ValidTestParamSavedFlag) != VALID_TEST_PARAM_FLAG)
		{
			stcTestData.TestId = INVALID_TEST_PARAM_FLAG;
		}
		GetTestParamsTotalItemSaved();
		ChangeWindowPage(en_WinID_QcControl , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerButtonbMaintanance(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		ChangeWindowPage(en_WinId_MaintananceScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerButtonbSettings(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		BeepBuzzer();
		ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerbFavScreen(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		ChangeWindowPage(en_WinId_HomeFavouriteScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerbDiagnostics(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		ChangeWindowPage(en_WinId_DiagnosticsScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerbWash(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		BeepBuzzer();
		g_HomeWashProcessState = en_HomeWashProcess_1;
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerbPrintFeed(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		BeepBuzzer();
		PrinterFeedLine(1);
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
static void HandlerUserlogin(void *ptr)
{
	if(g_HomeWashProcessState == en_HomeWashProcess_Idle)
	{
		ChangeWindowPage(en_WinId_UserLogin ,  en_WinID_MainMenuScreen);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}

}
