/*
 * Screens.h
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#ifndef SCREENS_INC_SCREENS_H_
#define SCREENS_INC_SCREENS_H_

#include "main.h"
#include "cmsis_os.h"
#include "Buzzer.h"
#include "lcd.h"
#include <string>
#include <string.h>
#include "../../APPL/Inc/ApplLcd.h"
#include "../libraries/Nextion/Nextion.h"
#include "../APPL/Inc/NonVolatileMemory.h"
#include "../APPL/Inc/Testrun.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"

#define PICTURE_ID_BUTTON_ON (169)
#define PICTURE_ID_BUTTON_OFF (168)

#define RADIO_BUTTON_ON (426)
#define RADIO_BUTTON_OFF (425)

#define DELAULT_NO_TEST_IMAGE_ID (214)
#define IMAGE_ID_ENDPOINT (28)
#define IMAGE_ID_FIXEDPOINT (31)
#define IMAGE_ID_KINETICS (29)
#define IMAGE_ID_DIFFRENTIAL (30)
#define IMAGE_ID_TURBIDIMETRY (32)
#define IMAGE_ID_COAGULATION (33)

#define SELECTED_FONT_COLOUR (1032)
#define NOT_SELECTED_FONT_COLOUR (0)

#define COLOUR_RED (63488)
#define COLOUR_WHITE (65535)
#define COLOUR_BLUE (31)
#define COLOUR_BLACK (0)
#define COLOUR_GRAY (33840)
#define COLOUR_POPUP_GRAY (19017)
#define COLOUR_GREEN (13867)
#define COLOUR_YELLOW (65120)
#define COLOUR_DARKBLUE (459)
#define COLOUR_LIGHTBLACK (10597)

#define COLOUR_MEAS_INCU (COLOUR_BLUE)
#define COLOUR_MEAS_PRI (COLOUR_BLUE)
#define COLOUR_MEAS_SEC (COLOUR_GREEN)
#define COLOUR_VERTICAL_LINE (COLOUR_RED)

#define ADC_RANGE_340NM (1000)
#define ADC_RANGE_405NM (1800)
#define ADC_RANGE_450NM (4095)
#define ADC_RANGE_505NM (2800)
#define ADC_RANGE_546NM (2800)
#define ADC_RANGE_578NM (2200)
#define ADC_RANGE_630NM (2900)
#define ADC_RANGE_670NM (4095)

#define ADC_MINMAX_RANGE (300)

const uint16_t g_Default_ADC_Ranges_min[en_FilterMax] = {(0),// home
														(1200),//340nm
														(1600),//405nm
														(2000),//505nm
														(1600),//546nm
														(1700),//578nm
														(2000),//630nm
														(0)//Black
};
const uint16_t g_Default_ADC_Ranges_max[en_FilterMax] = {(0),// home
														(2000),//340nm
														(3200),//405nm
														(3800),//505nm
														(3800),//546nm
														(3900),//578nm
														(4000),//630nm
														(0)//Black
};

typedef enum
{
	enKeyNull = 0,
	enKeyYes,
	enKeyNo,
	enkeyOk,
	enKeyMax
}enPopUpKeys;

typedef enum
{
	en_WinID_TestSelectionScreen = 0,
	en_WinID_NumericKeypad,//1
	en_WinID_AlphaKeypad,//2
	en_WinID_MainMenuScreen,//3
	en_WinId_TestParamsGeneral,//4
	en_WinId_TestRun,//5
	en_WinId_HistoryMMenu,//6
	en_WinId_TestParamsView,//7
	en_WinId_StartupScreen,//8
	en_WinId_SelfTestScreen,//9
	en_WinId_TestParamSettings,//10
	en_WinId_TestParamsLimit,//11
	en_WinId_TestParamCalib,//12
	en_WinId_TestParamStandards,//13
	en_WinId_MainPopup,//14
	en_WinId_TestCalibScreen,//15
	en_WinId_TestResultScreen,//16
	en_WinId_TestTimerScreen,//17
	en_WinId_TestQCSummaryScreen,//18
	en_WinId_HomeFavouriteScreen,//19
	en_WinId_SystemsettingsScreen,//20
	en_WinId_MaintananceScreen,//21
	en_WinId_DiagnosticsScreen,//22
	en_WinId_TestParamStdGraph,//23
	en_WinId_TestParamCalTrace,//24
	en_WinId_SettingsAbout,//25
	en_WinId_SettingsDateTime,//26
	en_WinId_SettingsAdmin,//27
	en_WinId_SettingsUserSetup,//28
	en_WinId_SettingsPump,//29
	en_WinId_SettingsPrinter,//30
	en_WinId_SettingsLamp,//31
	en_WinId_SettingsBuzzer,//32
	en_WinId_SettingsSysP_Lamp,//33
	en_WinId_SettingsReset,//34
	en_WinId_SettingsAutowash,//35
	en_WinId_SettingsMemory,//36
	en_WinId_SettingsUserSetupList,//37
	en_WinId_SettingsUserAddUser,//38
	en_WinId_SettingsSysP_Login,//39
	en_WinId_SettingsSysP_Pump,//40
	en_WinId_SettingsSysP_Barcode,//41
	en_WinId_SettingsSysP_Printer,//42
	en_WinId_SettingsSysP_Temp_B,//43
	en_WinId_SettingsSysP_Temp_A,//44
	en_WinId_Diag_Optical,//45
	en_WinId_Diag_Pump,//46
	en_WinId_Diag_Lamp,//47
	en_WinId_Diag_Temp,//48
	en_WinId_Diag_AutoWash_A,//49
	en_WinId_Diag_Filter,//50
	en_WinId_Diag_AutoWash_B,//51
	en_WinID_HistSearch,//52
	en_WinID_HistView,//53
	en_WinID_QcControl,//54
	en_WinID_QcTestScreen,//55
	en_WinID_QcAddTest,//56
	en_WinID_AddQcControl,//57
	en_WinID_LJ_Plot,//58
	en_WinID_Qc_Res_Summary,//59
	en_WinID_QcResHistory,//60
	en_WinId_SelectQcLot,//61
	en_WinId_PowerOffMenu,//62
	en_WinId_PowerSleep,//63
	en_WinId_UserLogin,//64
	en_WinId_ClearMemoryLogin,//65
	en_WinId_LampReplaceLogin,//66
	en_WinId_UpdateLogin,//67
	en_WinId_OEMLogin,//68
	en_WinId_OEMSettings,//69
	en_WinID_Max//17
}enWindowID;


typedef enum
{
	en_WindowShown,
	en_WindowNotShown,
	en_WindowChange,
}enWindowStatus;

typedef enum
{
	en_AlphaKeyboard = 0,
	en_NumericKeyboard,
}enkeyboardType;

typedef struct
{
	enWindowID CurrentWindowId;
	enWindowID PrevWindowId;
	enWindowID NextWindowId;
	enWindowID PrevSleepWindowId;
	bool NumberFloatEnable; /*Used to denote type of number*/
	uint8_t SizeOfBuffer;
	uint32_t *ptrInterscreenDataBuffer;
	float *ptrInterscreenDataFloatBuffer;
	char* ptr_KeyBoardBuffer;
	uint8_t MaxKeyboardCharLength;
	std::string strKeyboardname;
	std::string strKeyboardDefaultvalue;
}ScreenNavigation;

extern ScreenNavigation stcScreenNavigation;
extern bool DateEntry;
extern Testparams stcTestparams[MAX_NUM_OF_TEST_PARAMS_SAVED];
extern Testparams LastSavedTestParams;

typedef struct
{
	NexPage obJCurrPage;
	enWindowStatus (*ShowWindowFunction)(NexPage *ptr_obJCurrPage);
	void (*HandlerWindowFunction)(NexPage *ptr_obJCurrPage);

}WindowMapping;



#define PLEASE_WAIT					"\\rPlease Wait"

void ProcessScreens(void);
void ChangeWindowPage(enWindowID NextWindow , enWindowID UpcommingWindow);
bool UpdateTestSelectionGridDetails(void);
void openKeyBoard (enkeyboardType Keyboard , void *ptr_KeyBoardBuffer , uint8_t SizeOfBuffer,  bool NumberFloatEnable,
		std::string KeyboardName , std::string Default_value , uint8_t MaxCharLength);
enPopUpKeys ShowMainPopUp(std::string ptrTitle, std::string ptrMsg , bool OK_Flag);
void ShowPleaseWaitPopup(std::string ptrTitle, std::string ptrMsg);
void HidePleaseWaitPopup(enWindowID CurrWindow);
void HandlerSaveTestParams(void);
void ShowResultScreen(void);
void ShowTestCalibScreen(void);
void ShowTestParamsViewScreen(void);
void ShowQCResLJPlotScreen(void);
bool VerifyForBlanksPerformed(void);
void HandlerSaveEditedTestParams(void);
void ShowTestQCSummaryScreen(void);
void ShowTestTimerScreen(void);
int isInIncreasingOrder(float arr[], int size);
void TestTimer_Task(void);
void SetResultViewId(int16_t n16ResId);
uint8_t GetQcSetupMemorySlotIndex_QcControl(void);
uint8_t GetQcTestSetupFreeSlotIndex_QcTestSetup(void);
uint8_t Get_QcTestEntryId(uint8_t u8Row);
uint8_t GetQcTestLastSelectedRow(void);
uint8_t GetSelectedTestQcIdx(void);
uint8_t GetSelectedTestQcRow(void);
void ResetSelectedQcIdx(void);
void ConfigAutoSleepTimer(uint16_t u32Delay);
void Reset_AutoSleepMinsCounterTimer(void);

enWindowStatus ShowWMainMenuScreen (NexPage *ptr_obJCurrPage);
void HandlerMainMenuScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowWAlphaNumKeypadScreen (NexPage *ptr_obJCurrPage);
void HandlerAlphaNumKeypadScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestSelectionScreen (NexPage *ptr_obJCurrPage);
void HandlerTestSelectionScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowWNumericKeypadScreen (NexPage *ptr_obJCurrPage);
void HandlerNumericKeypadScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestRunScreen (NexPage *ptr_obJCurrPage);
void HandlerTestRunScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowPopUpTestAdd (NexPage *ptr_obJCurrPage);
void HandlerPopUpTestAdd (NexPage *ptr_obJCurrPage);
enWindowStatus ShowPopUpTestAddBarcode (NexPage *ptr_obJCurrPage);
void HandlerPopUpTestAddBarcode (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestParamsGeneral(NexPage *ptr_obJCurrPage);
void HandlerTestParamsGeneral (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestParamsSettings(NexPage *ptr_obJCurrPage);
void HandlerTestParamsSettings (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestParamsLimits(NexPage *ptr_obJCurrPage);
void HandlerTestParamsLimits (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestParamsCalib(NexPage *ptr_obJCurrPage);
void HandlerTestParamsCalib (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestParamsStandards(NexPage *ptr_obJCurrPage);
void HandlerTestParamsStandards (NexPage *ptr_obJCurrPage);
enWindowStatus ShowMainPopUpScreen (NexPage *ptr_obJCurrPage);
void HandlerMainPopUpScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestCalibScreen (NexPage *ptr_obJCurrPage);
void HandlerTestCalibScreen(NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestResultScreen (NexPage *ptr_obJCurrPage);
void HandlerTestResultScreen(NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestTimerScreen (NexPage *ptr_obJCurrPage);
void HandlerTestTimerScreen(NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestQCSummaryScreen (NexPage *ptr_obJCurrPage);
void HandlerTestQCSummaryScreen(NexPage *ptr_obJCurrPage);
enWindowStatus ShowHomeFavScreen (NexPage *ptr_obJCurrPage);
void HandlerHomeFavScreen(NexPage *ptr_obJCurrPage);
enWindowStatus ShowQCMenuScreen (NexPage *ptr_obJCurrPage);
void HandlerQCMenuScreen(NexPage *ptr_obJCurrPage);
enWindowStatus ShowPResultsScreen (NexPage *ptr_obJCurrPage);
void HandlerPResultsScreen(NexPage *ptr_obJCurrPage);
enWindowStatus ShowSystemsettingsScreen (NexPage *ptr_obJCurrPage);
void HandlerSystemsettingsScreen(NexPage *ptr_obJCurrPage);
enWindowStatus ShowMaintananceScreen (NexPage *ptr_obJCurrPage);
void HandlerMaintananceScreen(NexPage *ptr_obJCurrPage);
enWindowStatus ShowDiagnosticsScreen (NexPage *ptr_obJCurrPage);
void HandlerDiagnosticsScreen(NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestParamstdGraphScreen (NexPage *ptr_obJCurrPage);
void HandlerTestParamstdGraphScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestParamsCalTraceScreen (NexPage *ptr_obJCurrPage);
void HandlerTestParamCalTraceScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsAboutScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsAboutScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsDateTimeScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsDateTimeScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsAdminScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsAdminScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsUserSetupScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsUserSetupScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsPumpScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsPumpScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsPrinterScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsPrinterScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsLampScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsLampScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsBuzzerScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsBuzzerScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsSysP_Lamp (NexPage *ptr_obJCurrPage);
void HandlerSettingsSysP_Lamp (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsResetScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsResetScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsAutoWashScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsAutoWashScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsMemoryScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsMemoryScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsUserSetupListScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsUserSetupListScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsUserSetupAddUserScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsUserSetupAddUserScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsSysP_Login (NexPage *ptr_obJCurrPage);
void HandlerSettingsSysP_Login (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsSysP_Pump (NexPage *ptr_obJCurrPage);
void HandlerSettingsSysP_Pump (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsSysP_Temp_A(NexPage *ptr_obJCurrPage);
void HandlerSettingsSysP_Temp_A (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsSysP_Temp_B(NexPage *ptr_obJCurrPage);
void HandlerSettingsSysP_Temp_B (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsSysP_Barcode(NexPage *ptr_obJCurrPage);
void HandlerSettingsSysP_Barcode (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsSysP_Printer(NexPage *ptr_obJCurrPage);
void HandlerSettingsSysP_Printer (NexPage *ptr_obJCurrPage);
enWindowStatus ShowDiagScreen_Optics (NexPage *ptr_obJCurrPage);
void HandlerDiagScreen_Optics (NexPage *ptr_obJCurrPage);
enWindowStatus ShowDiagScreen_AutoWash_A (NexPage *ptr_obJCurrPage);
void HandlerDiagScreen_AutoWash_A (NexPage *ptr_obJCurrPage);
enWindowStatus ShowDiagScreen_AutoWash_B (NexPage *ptr_obJCurrPage);
void HandlerDiagScreen_AutoWash_B (NexPage *ptr_obJCurrPage);
enWindowStatus ShowDiagScreen_Filter (NexPage *ptr_obJCurrPage);
void HandlerDiagScreen_Filter (NexPage *ptr_obJCurrPage);
enWindowStatus ShowDiagScreen_Lamp (NexPage *ptr_obJCurrPage);
void HandlerDiagScreen_Lamp (NexPage *ptr_obJCurrPage);
enWindowStatus ShowDiagScreen_Pump (NexPage *ptr_obJCurrPage);
void HandlerDiagScreen_Pump (NexPage *ptr_obJCurrPage);
enWindowStatus ShowDiagScreen_Temp (NexPage *ptr_obJCurrPage);
void HandlerDiagScreen_Temp (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSearchResHist (NexPage *ptr_obJCurrPage);
void HandlerSearchResHist (NexPage *ptr_obJCurrPage);
enWindowStatus ShowResHistViewScreen (NexPage *ptr_obJCurrPage);
void HandlerResHistViewScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowQcControlViewScreen (NexPage *ptr_obJCurrPage);
void HandlerQcControlViewScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowQcAddControlScreen (NexPage *ptr_obJCurrPage);
void HandlerQcAddControlScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowQcTestViewScreen (NexPage *ptr_obJCurrPage);
void HandlerQcTestViewScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowQcAddTestScreen (NexPage *ptr_obJCurrPage);
void HandlerQcAddTesScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowQCResHistoryScreen (NexPage *ptr_obJCurrPage);
void HandlerQCResHistoryScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestQCSelectMenuScreen (NexPage *ptr_obJCurrPage);
void HandlerTestQCSelectScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowQCResSummaryScreen (NexPage *ptr_obJCurrPage);
void HandlerQCResSummaryScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowQCResLJPlotScreen (NexPage *ptr_obJCurrPage);
void HandlerQCResLJPlotScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowMainPowerOffScreen (NexPage *ptr_obJCurrPage);
void HandlerMainPowerOffScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowMainPowerSleepScreen (NexPage *ptr_obJCurrPage);
void HandlerMainPowerSleepScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowUserLoginScreen (NexPage *ptr_obJCurrPage);
void HandlerUserLoginScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowTestParamsViewScreen (NexPage *ptr_obJCurrPage);
void HandlerTestParamsViewcreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsClearMemoryloginScreen (NexPage *ptr_obJCurrPage);
void HandlerSettingsClearMemoryloginScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowDiagLampReplaceloginScreen (NexPage *ptr_obJCurrPage);
void HandlerDiagLampReplaceloginScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowStartUpScreen (NexPage *ptr_obJCurrPage);
void HandlerStartUpScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowStartupDiagScreen (NexPage *ptr_obJCurrPage);
void HandlerStartupDiagScreen (NexPage *ptr_obJCurrPage);
enWindowStatus ShowSettingsUpdate_Login (NexPage *ptr_obJCurrPage);
void HandlerSettingsUpdate_Login (NexPage *ptr_obJCurrPage);
enWindowStatus ShowOEMSettings_Login (NexPage *ptr_obJCurrPage);
void HandlerOEMSettings_Login (NexPage *ptr_obJCurrPage);
enWindowStatus ShowOEMSettingsScreen (NexPage *ptr_obJCurrPage);
void HandlerOEMSettingsScreen (NexPage *ptr_obJCurrPage);

#endif /* SCREENS_INC_SCREENS_H_ */
