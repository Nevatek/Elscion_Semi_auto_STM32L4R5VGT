/*
 * Screens.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */


#include "../../Screens/Inc/Screens.h"
#include "Appl_Timer.hpp"

ScreenNavigation stcScreenNavigation = {/*Starting screen*/en_WinID_MainMenuScreen, en_WinID_Max , en_WinID_Max};

/*Initializing all pages (screens) (Format : page id = 0, component id = 1, component name = "b0")*/
WindowMapping stcWindowMapping[en_WinID_Max] =
{/*ShowWindowFunction , HandlerFunction*/
	{ NexPage(en_WinID_TestSelectionScreen, 0, "TestSelection"), ShowTestSelectionScreen , HandlerTestSelectionScreen}, // 0
	{ NexPage(en_WinID_NumericKeypad, 0, "NumericKeypad"), ShowWNumericKeypadScreen , HandlerNumericKeypadScreen}, // 1
	{ NexPage(en_WinID_AlphaKeypad, 0, "AlphaNKeypad"), ShowWAlphaNumKeypadScreen , HandlerAlphaNumKeypadScreen}, // 2
	{ NexPage(en_WinID_MainMenuScreen, 0, "Mainscreen"), ShowWMainMenuScreen , HandlerMainMenuScreen}, // 3
	{ NexPage(en_WinId_TestParamsGeneral, 0, "TestParamsGen"), ShowTestParamsGeneral , HandlerTestParamsGeneral}, // 4
	{ NexPage(en_WinId_TestRun, 0, "TestRun"), ShowTestRunScreen , HandlerTestRunScreen}, // 5
	{ NexPage(en_WinId_HistoryMMenu, 0, "HistoryMMenu"), ShowPResultsScreen , HandlerPResultsScreen}, // 6
	{ NexPage(en_WinId_TestParamsView, 0, "en_WinId_TestParamsView"), ShowTestParamsViewScreen , HandlerTestParamsViewcreen}, // 7
	{ NexPage(en_WinId_StartupScreen, 0, "StartupScreen"), ShowStartUpScreen , HandlerStartUpScreen}, // 8
	{ NexPage(en_WinId_SelfTestScreen, 0, "SelfTestScreen"), ShowStartupDiagScreen , HandlerStartupDiagScreen}, // 9
	{ NexPage(en_WinId_TestParamSettings, 0, "TestParamSett"), ShowTestParamsSettings , HandlerTestParamsSettings}, // 10
	{ NexPage(en_WinId_TestParamsLimit, 0, "TestParamLim"), ShowTestParamsLimits , HandlerTestParamsLimits}, // 11
	{ NexPage(en_WinId_TestParamCalib, 0, "TestParamCalib"), ShowTestParamsCalib , HandlerTestParamsCalib}, // 12
	{ NexPage(en_WinId_TestParamStandards, 0, "TestParamStd"), ShowTestParamsStandards , HandlerTestParamsStandards}, // 13
	{ NexPage(en_WinId_MainPopup, 0, "MainPopUp"), NULL , NULL}, // 14
	{ NexPage(en_WinId_TestCalibScreen, 0, "TestCalib"), ShowTestCalibScreen , HandlerTestCalibScreen}, // 15
	{ NexPage(en_WinId_TestResultScreen, 0, "TestResult"), ShowTestResultScreen , HandlerTestResultScreen}, // 16
	{ NexPage(en_WinId_TestTimerScreen, 0, "TestTimer"), ShowTestTimerScreen , HandlerTestTimerScreen}, // 17
	{ NexPage(en_WinId_TestQCSummaryScreen, 0, "TestQCSummary"), ShowTestQCSummaryScreen , HandlerTestQCSummaryScreen}, // 18
	{ NexPage(en_WinId_HomeFavouriteScreen, 0, "HomeFavScreen"), ShowHomeFavScreen , HandlerHomeFavScreen}, // 19
	{ NexPage(en_WinId_SystemsettingsScreen, 0, "SystemSettings"), ShowSystemsettingsScreen , HandlerSystemsettingsScreen}, // 20
	{ NexPage(en_WinId_MaintananceScreen, 0, "Maintenance"), ShowMaintananceScreen , HandlerMaintananceScreen}, // 21
	{ NexPage(en_WinId_DiagnosticsScreen, 0, "Diagnostics"), ShowDiagnosticsScreen , HandlerDiagnosticsScreen}, // 22
	{ NexPage(en_WinId_TestParamStdGraph, 0, "StdGraph"), ShowTestParamstdGraphScreen , HandlerTestParamstdGraphScreen}, // 23
	{ NexPage(en_WinId_TestParamCalTrace, 0, "CalTrace"), ShowTestParamsCalTraceScreen , HandlerTestParamCalTraceScreen}, // 24
	{ NexPage(en_WinId_SettingsAbout, 0, "SettAbout"), ShowSettingsAboutScreen , HandlerSettingsAboutScreen}, // 25
	{ NexPage(en_WinId_SettingsDateTime, 0, "SettDateTime"), ShowSettingsDateTimeScreen , HandlerSettingsDateTimeScreen}, // 26
	{ NexPage(en_WinId_SettingsAdmin, 0, "SettAdmin"), ShowSettingsAdminScreen , HandlerSettingsAdminScreen}, // 27
	{ NexPage(en_WinId_SettingsUserSetup, 0, "SettUser"), ShowSettingsUserSetupScreen , HandlerSettingsUserSetupScreen}, // 28
	{ NexPage(en_WinId_SettingsPump, 0, "SettPump"), ShowSettingsPumpScreen , HandlerSettingsPumpScreen}, // 29
	{ NexPage(en_WinId_SettingsPrinter, 0, "SettPrinter"), ShowSettingsPrinterScreen , HandlerSettingsPrinterScreen}, // 30
	{ NexPage(en_WinId_SettingsLamp, 0, "SettLamp"), ShowSettingsLampScreen , HandlerSettingsLampScreen}, // 31
	{ NexPage(en_WinId_SettingsBuzzer, 0, "SettBuzz"), ShowSettingsBuzzerScreen , HandlerSettingsBuzzerScreen}, // 32
	{ NexPage(en_WinId_SettingsSysP_Lamp, 0, "SysP_Lamp"), ShowSettingsSysP_Lamp , HandlerSettingsSysP_Lamp}, // 33
	{ NexPage(en_WinId_SettingsReset, 0, "SettReset"), ShowSettingsResetScreen , HandlerSettingsResetScreen}, // 34
	{ NexPage(en_WinId_SettingsAutowash, 0, "SettWash"), ShowSettingsAutoWashScreen , HandlerSettingsAutoWashScreen}, // 35
	{ NexPage(en_WinId_SettingsMemory, 0, "SettMemory"), ShowSettingsMemoryScreen , HandlerSettingsMemoryScreen}, // 36
	{ NexPage(en_WinId_SettingsUserSetupList, 0, "SettUserList"), ShowSettingsUserSetupListScreen , HandlerSettingsUserSetupListScreen}, // 37
	{ NexPage(en_WinId_SettingsUserAddUser, 0, "SettAddUser"), ShowSettingsUserSetupAddUserScreen , HandlerSettingsUserSetupAddUserScreen}, // 38
	{ NexPage(en_WinId_SettingsSysP_Login, 0, "SysP_Login"), ShowSettingsSysP_Login , HandlerSettingsSysP_Login}, // 39
	{ NexPage(en_WinId_SettingsSysP_Pump, 0, "SysP_Pump"), ShowSettingsSysP_Pump , HandlerSettingsSysP_Pump}, // 40
	{ NexPage(en_WinId_SettingsSysP_Barcode, 0, "SysP_Barcode"), ShowSettingsSysP_Barcode , HandlerSettingsSysP_Barcode}, // 41
	{ NexPage(en_WinId_SettingsSysP_Printer, 0, "SysP_Printer"), ShowSettingsSysP_Printer , HandlerSettingsSysP_Printer}, // 42
	{ NexPage(en_WinId_SettingsSysP_Temp_B, 0, "SysP_Temp_B"), ShowSettingsSysP_Temp_B , HandlerSettingsSysP_Temp_B}, // 43
	{ NexPage(en_WinId_SettingsSysP_Temp_A, 0, "SysP_Temp_A"), ShowSettingsSysP_Temp_A , HandlerSettingsSysP_Temp_A}, // 44
	{ NexPage(en_WinId_Diag_Optical, 0, "Diag optical"), ShowDiagScreen_Optics , HandlerDiagScreen_Optics}, // 45
	{ NexPage(en_WinId_Diag_Pump, 0, "Diag pump"), ShowDiagScreen_Pump , HandlerDiagScreen_Pump}, // 46
	{ NexPage(en_WinId_Diag_Lamp, 0, "Diag lamp"), ShowDiagScreen_Lamp , HandlerDiagScreen_Lamp}, // 47
	{ NexPage(en_WinId_Diag_Temp, 0, "Diag temp"), ShowDiagScreen_Temp , HandlerDiagScreen_Temp}, // 48
	{ NexPage(en_WinId_Diag_AutoWash_A, 0, "Diag autowash a"), ShowDiagScreen_AutoWash_A , HandlerDiagScreen_AutoWash_A}, // 49
	{ NexPage(en_WinId_Diag_Filter, 0, "Diag filter"), ShowDiagScreen_Filter , HandlerDiagScreen_Filter}, // 50
	{ NexPage(en_WinId_Diag_AutoWash_B, 0, "Diag autowash b"), ShowDiagScreen_AutoWash_B , HandlerDiagScreen_AutoWash_B}, // 51
	{ NexPage(en_WinID_HistSearch, 0, "HistorySearch"), ShowSearchResHist , HandlerSearchResHist}, // 52
	{ NexPage(en_WinID_HistView, 0, "History result view"), ShowResHistViewScreen , HandlerResHistViewScreen}, // 53
	{ NexPage(en_WinID_QcControl, 0, "Qc Control view"), ShowQcControlViewScreen , HandlerQcControlViewScreen}, // 54
	{ NexPage(en_WinID_QcTestScreen, 0, "Qc Test view"), ShowQcTestViewScreen , HandlerQcTestViewScreen}, // 55
	{ NexPage(en_WinID_QcAddTest, 0, "Qc Add Test view"), ShowQcAddTestScreen , HandlerQcAddTesScreen}, // 56
	{ NexPage(en_WinID_AddQcControl, 0, "Qc Add Control view"), ShowQcAddControlScreen , HandlerQcAddControlScreen}, // 57
	{ NexPage(en_WinID_LJ_Plot, 0, "Qc Res LJ Plot"), ShowQCResLJPlotScreen , HandlerQCResLJPlotScreen}, // 58
	{ NexPage(en_WinID_Qc_Res_Summary, 0, "Qc Res Summary"), ShowQCResSummaryScreen , HandlerQCResSummaryScreen}, // 59
	{ NexPage(en_WinID_QcResHistory, 0, "Qc Res history"), ShowQCResHistoryScreen , HandlerQCResHistoryScreen}, // 60
	{ NexPage(en_WinId_SelectQcLot, 0, "Select Test run QC"), ShowTestQCSelectMenuScreen , HandlerTestQCSelectScreen}, // 61
	{ NexPage(en_WinId_PowerOffMenu, 0, "Power off menu"), ShowMainPowerOffScreen , HandlerMainPowerOffScreen}, // 62
	{ NexPage(en_WinId_PowerSleep, 0, "Power sleep"), ShowMainPowerSleepScreen , HandlerMainPowerSleepScreen}, // 63
	{ NexPage(en_WinId_UserLogin, 0, "User login"), ShowUserLoginScreen , HandlerUserLoginScreen}, // 64
	{ NexPage(en_WinId_ClearMemoryLogin, 0, "SettMemLogin"), ShowSettingsClearMemoryloginScreen , HandlerSettingsClearMemoryloginScreen}, // 65
	{ NexPage(en_WinId_LampReplaceLogin, 0, "DiagLampReplaceLogin"), ShowDiagLampReplaceloginScreen , HandlerDiagLampReplaceloginScreen}, // 66
	{ NexPage(en_WinId_UpdateLogin, 0, "en_WinId_UpdateLogin"), ShowSettingsUpdate_Login , HandlerSettingsUpdate_Login}, // 67
	{ NexPage(en_WinId_OEMLogin, 0, "en_WinId_OEMLogin"), ShowOEMSettings_Login , HandlerOEMSettings_Login}, // 68
	{ NexPage(en_WinId_OEMSettings, 0, "en_WinId_OEMSettings"), ShowOEMSettingsScreen , HandlerOEMSettingsScreen}, // 69

   };
/*******************************************Page or screen initilization END**************************************************/

enWindowStatus g_WinStatus = en_WindowNotShown;
stcTimer g_autoSleepTimer;
uint16_t g_u16AutoSleepMinCounter = 0U;
bool DateEntry = false;


static void AutoSleep_Task(void);

void ProcessScreens(void)
{
	uint16_t CurrWindowId = stcScreenNavigation.CurrentWindowId;
	if(en_WindowNotShown == g_WinStatus || en_WindowChange == g_WinStatus)
	{
		g_WinStatus = stcWindowMapping[CurrWindowId].
				ShowWindowFunction(&stcWindowMapping[CurrWindowId].obJCurrPage);//do return type handler
	}
	stcWindowMapping[CurrWindowId].HandlerWindowFunction(&stcWindowMapping[CurrWindowId].obJCurrPage);
	TestTimer_Task();
	AutoSleep_Task();
}

void ChangeWindowPage(enWindowID NextWindow , enWindowID UpcommingWindow)
{
	stcScreenNavigation.PrevWindowId = stcScreenNavigation.CurrentWindowId;
	stcScreenNavigation.CurrentWindowId = NextWindow;
	stcScreenNavigation.NextWindowId = UpcommingWindow;
	g_WinStatus = en_WindowChange;

	Reset_AutoSleepMinsCounterTimer();/*Reset auto sleep mins counter when user presses any button*/
}

void openKeyBoard (enkeyboardType Keyboard , void *ptr_KeyBoardBuffer , uint8_t SizeOfBuffer,  bool NumberFloatEnable,
		std::string KeyboardName , std::string Default_value , uint8_t MaxCharLength)
{
	stcScreenNavigation.MaxKeyboardCharLength = MaxCharLength;
	stcScreenNavigation.strKeyboardDefaultvalue = Default_value;
	stcScreenNavigation.strKeyboardname = KeyboardName;
	stcScreenNavigation.ptrInterscreenDataBuffer = 0;
	stcScreenNavigation.ptrInterscreenDataFloatBuffer = 0;
	stcScreenNavigation.SizeOfBuffer = 0;
	if(en_AlphaKeyboard == Keyboard)
	{
		stcScreenNavigation.ptr_KeyBoardBuffer = (char*)ptr_KeyBoardBuffer;
		ChangeWindowPage(en_WinID_AlphaKeypad , stcScreenNavigation.NextWindowId);
	}
	else
	{
		stcScreenNavigation.SizeOfBuffer = SizeOfBuffer;
		if(DateEntry)
		{
			stcScreenNavigation.NumberFloatEnable = false;
			stcScreenNavigation.ptr_KeyBoardBuffer = (char*)ptr_KeyBoardBuffer;
		}
		 else if(NumberFloatEnable == true && DateEntry == false)
		{
			stcScreenNavigation.NumberFloatEnable = true;
			stcScreenNavigation.ptrInterscreenDataFloatBuffer = (float*)ptr_KeyBoardBuffer;
		}
		else if(NumberFloatEnable == false && DateEntry == false)
		{
			stcScreenNavigation.NumberFloatEnable = false;
			stcScreenNavigation.ptrInterscreenDataBuffer = (uint32_t*)ptr_KeyBoardBuffer;
		}
		ChangeWindowPage(en_WinID_NumericKeypad , stcScreenNavigation.NextWindowId);
	}
}
void ConfigAutoSleepTimer(uint16_t u32Delay)
{
	StartTimer(&(g_autoSleepTimer) , 60 * 1000/*1 Min timer*/);
}
void Reset_AutoSleepMinsCounterTimer(void)
{
	g_u16AutoSleepMinCounter = 0U;
}
void AutoSleep_Task(void)
{
	if(true == Timer_IsTimeout(&(g_autoSleepTimer) , enTimerNormalStop))
	{
		if(enTestStatus_Idle == GetInstance_TestData()->CurrentTestStatus)
		{
			g_u16AutoSleepMinCounter++;
		}
		else
		{
			Reset_AutoSleepMinsCounterTimer();/*Reset auto sleep mins counter if user runs any test*/
		}
		if(g_u16AutoSleepMinCounter >= GetInstance_SettingsData()->u16LampDelayTime)
		{
			/*Enter Sleep mode when lamp delay expired*/
			/*Enter into sleep mode*/
			enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
			enWindowID NextWindow = stcScreenNavigation.NextWindowId;
			stcScreenNavigation.PrevSleepWindowId = stcScreenNavigation.CurrentWindowId;

			ChangeWindowPage(en_WinId_PowerSleep , (enWindowID)stcScreenNavigation.CurrentWindowId);
			stcScreenNavigation.PrevWindowId = PrevWindow;
			stcScreenNavigation.NextWindowId = NextWindow;
		}
//		else
//		{
//			/*If time out expired after delay and not any test is runnning*/
//			if(enTestStatus_Idle == GetInstance_TestData()->CurrentTestStatus)
//			{
//				g_u16AutoSleepMinCounter++;
//			}
//			else
//			{
//				Reset_AutoSleepMinsCounterTimer();/*Reset auto sleep mins counter if user runs any test*/
//			}
//		}
		StartTimer(&(g_autoSleepTimer) , 60 * 1000/*1 Min timer*/);
	}
}
