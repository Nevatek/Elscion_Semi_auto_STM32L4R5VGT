/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
//#include "../../Screens/Inc/Screens.h"
//#include "../APPL/Inc/Testrun.h"
//#include "../../APPL/Inc/NonVolatileMemory.h"
//#include "./../Screens/Inc/CommonDisplayFunctions.h"

#include "HandlerPheripherals.hpp"
#include "../../Screens/Inc/Screens.h"
#include "../APPL/Inc/Testrun.h"
#include "../APPL/Inc/ApplUsbDevice.h"
#include "../APPL/Inc/calibration.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/NVM_TestParameters.hpp"

#define PIC_FACTOR_1POINT_GRAPH (239)
#define PIC_2POINT_GRAPH (239)
#define PIC_4PLL_GRAPH (241)
#define PIC_3PLL_GRAPH (241)
#define PIC_CUBIC_GRAPH (242)
#define PIC_LINEAR_GRAPH (239)
#define PIC_POINTTOPOINT_GRAPH (244)


//#define CALIB_WAVEFORM_CHANNEL_NUMBER (0)
//
//char arrTestTypeNames[en_TestProdecureMax][24] = {{"NoTestSelected"},{"End point"},{"Kinetics"},
//								{"Fixed time"},{"Differential"},
//								{"Turbidimetry"},{"Coagulation"},{"Absorbance"}};
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
//static NexButton bDeleteButton = NexButton(en_WinId_TestParamCalib , 7 , "b6");
static NexPicture pParamsCalcurve = NexPicture(en_WinId_TestParamCalib , 17 , "p0");

static NexButton bBackButton = NexButton(en_WinId_TestParamCalib , 7 , "b5");
static NexButton bHomeButton = NexButton(en_WinId_TestParamCalib , 8 , "b7");

static NexButton bButtonGeneral = NexButton(en_WinId_TestParamCalib , 6 , "b0");
static NexButton bButtonSettings = NexButton(en_WinId_TestParamCalib , 5 , "b1");
static NexButton bButtonLimits = NexButton(en_WinId_TestParamCalib , 4 , "b2");
static NexButton bButtonCalib = NexButton(en_WinId_TestParamCalib , 3 , "b3");
static NexButton bButtonStandards = NexButton(en_WinId_TestParamCalib , 2 , "b4");

static NexText bTxtButtonTestName = NexText(en_WinId_TestParamCalib , 10 , "t9");
static NexButton CbTxtButtonTestMethod = NexButton(en_WinId_TestParamCalib , 15 , "cb0");

static NexText tKFactor = NexText(en_WinId_TestParamCalib , 13 , "t2");
static NexButton bNumberOfStandards = NexButton(en_WinId_TestParamCalib , 16 , "t11");

//static NexWaveform wCalibGraph = NexWaveform(en_WinId_TestParamCalib , 16 , "s0");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={/*&bDeleteButton,*/
										&bBackButton,
										&bHomeButton,
										&bButtonGeneral,
										&bButtonSettings,
										&bButtonLimits,
										&bButtonCalib,
										&bButtonStandards,
										&tKFactor,
										&bNumberOfStandards,
										&CbTxtButtonTestMethod,
										NULL};


static void ShowSettingsCalibGraphPicture(void);
static void HandlerButtonback(void *ptr);
static void HandlerButtonHome(void *ptr);

static void HandlerbButtonGeneral(void *ptr);
static void HandlerbButtonSettings(void *ptr);
static void HandlerbButtonLimits(void *ptr);
static void HandlerbButtonCalib(void *ptr);
static void HandlerbButtonStandards(void *ptr);

static void HandlerbButtonKFactor(void *ptr);
static void HandlerbButtonNumberOfStandards(void *ptr);
static void HandlerCbTxtButtonTestMethod(void *ptr);

static void ValidateNumOfStd(void);
static bool TestCalibMillsDelay(uint32_t Delay);

enWindowStatus ShowTestParamsCalib(NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

	bBackButton.attachPush(HandlerButtonback, &bBackButton);
	bHomeButton.attachPush(HandlerButtonHome, &bHomeButton);

	bButtonGeneral.attachPush(HandlerbButtonGeneral, &bButtonGeneral);
	bButtonSettings.attachPush(HandlerbButtonSettings, &bButtonSettings);
	bButtonLimits.attachPush(HandlerbButtonLimits, &bButtonLimits);
	bButtonCalib.attachPush(HandlerbButtonCalib, &bButtonCalib);
	bButtonStandards.attachPush(HandlerbButtonStandards, &bButtonStandards);

	tKFactor.attachPush(HandlerbButtonKFactor, &tKFactor);
	bNumberOfStandards.attachPush(HandlerbButtonNumberOfStandards, &bNumberOfStandards);
	CbTxtButtonTestMethod.attachPush(HandlerCbTxtButtonTestMethod, &CbTxtButtonTestMethod);

	bTxtButtonTestName.setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);
	CbTxtButtonTestMethod.setValue(GetInstance_TestParams()[stcTestData.TestId].Calib);

	char arr_Buffer[64] = {0};
	snprintf(arr_Buffer , 61 , "%d" , (int)GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
	bNumberOfStandards.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].KFactor);
	tKFactor.setText(arr_Buffer);

	if (GetInstance_TestParams()[stcTestData.TestId].Calib == en_Factor || GetInstance_TestParams()[stcTestData.TestId].Calib == en_1PointLinear)
	{
		tKFactor.Set_background_image_pic(171);
		bNumberOfStandards.Set_background_image_pic(503);
	}
	else if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_2PointLinear || GetInstance_TestParams()[stcTestData.TestId].Calib == en_3PL)
	{
		tKFactor.Set_background_image_pic(503);
		bNumberOfStandards.Set_background_image_pic(503);
	}
	else
	{
		tKFactor.Set_background_image_pic(503);
		bNumberOfStandards.Set_background_image_pic(171);
	}
	ShowSettingsCalibGraphPicture();

	return WinStatus;
}

void HandlerTestParamsCalib (NexPage *ptr_obJCurrPage)
{
	if(en_WinID_NumericKeypad == stcScreenNavigation.PrevWindowId)
	{
		ValidateNumOfStd();
		stcScreenNavigation.PrevWindowId = en_WinId_TestParamCalib;
		CbTxtButtonTestMethod.setValue(GetInstance_TestParams()[stcTestData.TestId].Calib);
	}
	if(TestCalibMillsDelay(700))
	{
		uint32_t val = 0;
		if(true == CbTxtButtonTestMethod.getValue((uint32_t*)&val))
		{
			/*If new calibration is selected*/


//			if((enCalibrationType)val != GetInstance_TestParams()[stcTestData.TestId].Calib &&
//					en_1PointLinear <= val && en_CalibrationMax > val)
			if((enCalibrationType)val != GetInstance_TestParams()[stcTestData.TestId].Calib && en_CalibrationMax > val)
			{
				if(GetInstance_TestParams()[stcTestData.TestId].ValidTestParamSavedFlag == VALID_TEST_PARAM_FLAG)
				{
				/*Save upcomming window id before show popup page*/
				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
				if(enKeyYes == ShowMainPopUp("Calibration","It will reset previous cal. data? \\rDo you want to continue?", false))
				{
					/*Clear data of calibrations - std and od*/
					ClearTestSavedValues();
					GetInstance_TestParams()[stcTestData.TestId].Calib = (enCalibrationType)val;
				}
				ChangeWindowPage(en_WinId_TestParamCalib , (enWindowID)NextWindow);
				uint8_t MinNumOfStd = MinNumOfStandardsForSelectedMethod(GetInstance_TestParams()[stcTestData.TestId].Calib);
				GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved = MinNumOfStd;/*Reset number of standard saved
																	 to minimum valuefor each calibration method*/
				char arr_Buffer[64] = {0};
				snprintf(arr_Buffer , 61 , "%d" , (int)GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
				bNumberOfStandards.setText(arr_Buffer);
				}
				else
				{
					char arr_Buffer[64] = {0};
					GetInstance_TestParams()[stcTestData.TestId].Calib = (enCalibrationType)val;
					uint8_t MinNumOfStd = MinNumOfStandardsForSelectedMethod(GetInstance_TestParams()[stcTestData.TestId].Calib);
					GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved = MinNumOfStd;
					snprintf(arr_Buffer , 61 , "%d" , (int)GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
					bNumberOfStandards.setText(arr_Buffer);
					BeepBuzzer();
				}
				if (GetInstance_TestParams()[stcTestData.TestId].Calib == en_Factor || GetInstance_TestParams()[stcTestData.TestId].Calib == en_1PointLinear)
				{
					tKFactor.Set_background_image_pic(171);
					bNumberOfStandards.Set_background_image_pic(503);
				}
				else if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_2PointLinear || GetInstance_TestParams()[stcTestData.TestId].Calib == en_3PL)
				{
					tKFactor.Set_background_image_pic(503);
					bNumberOfStandards.Set_background_image_pic(503);
				}
				else
				{
					tKFactor.Set_background_image_pic(503);
					bNumberOfStandards.Set_background_image_pic(171);
				}
			}
		}
	}
	nexLoop(nex_Listen_List);
}
void HandlerCbTxtButtonTestMethod(void *ptr)
{
//	uint32_t val = 0;
//	CbTxtButtonTestMethod.getValue((uint32_t*)&val);
//	BeepBuzzer();
//
//	/*If new calibration is selected*/
//	if((enCalibrationType)val != GetInstance_TestParams()[stcTestData.TestId].Calib)
//	{
//		/*Save upcomming window id before show popup page*/
//		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//		if(enKeyYes == ShowMainPopUp("Reset cal data? do you want to continue?", false))
//		{
//			/*Clear data of calibrations - std and od*/
//			GetInstance_TestParams()[stcTestData.TestId].Calib = (enCalibrationType)val;
//			ClearTestSavedValues();
//			ChangeWindowPage(en_WinId_TestParamCalib , (enWindowID)NextWindow);
//		}
//	}
//
//	uint8_t MinNumOfStd = MinNumOfStandardsForSelectedMethod(GetInstance_TestParams()[stcTestData.TestId].Calib);
//	GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved = MinNumOfStd;/*Reset number of standard saved
//																		 to minimum value
//																		for each calibration method*/
//	char arr_Buffer[64] = {0};
//	snprintf(arr_Buffer , 61 , "%d" , (int)GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
//	bNumberOfStandards.setText(arr_Buffer);
//
//
//	ShowSettingsCalibGraphData();/*Display graph in calibration screen*/
}

void HandlerbButtonKFactor(void *ptr)
{
	enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;
	switch(CalibType)
	{
	 case en_Factor:
	 case en_1PointLinear:
			openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].KFactor ,
					sizeof(GetInstance_TestParams()[stcTestData.TestId].KFactor) , true ,"K Factor" , " " , 7);
			BeepBuzzer();
			break;
	 case en_2PointLinear:
	 case en_PointToPoint:
	 case en_LinearRegression:
	 case en_4PL:
	 case en_CubicSpline:
		 break;
	 default:
		 return;
	}
}
void HandlerbButtonNumberOfStandards(void *ptr)
{
	char arr_Buffer[64] = {0};
	enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;
	uint8_t MaxNumOfStd = MaxNumOfStandardsForSelectedMethod(CalibType);
	uint8_t MinNumOfStd = MinNumOfStandardsForSelectedMethod(CalibType);
	snprintf(arr_Buffer , 61 , "No of Std (%d - %d)" , MinNumOfStd , MaxNumOfStd);

	switch(CalibType)
	{
	 case en_Factor:
	 case en_1PointLinear:
	 case en_2PointLinear:
	 case en_3PL:
		 break;/*No need to edit num of standards*/
	 case en_PointToPoint:
	 case en_LinearRegression:
	 case en_4PL:
	 case en_CubicSpline:
			BeepBuzzer();
			openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved ,
					sizeof(GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved) , false ,arr_Buffer , " " , 7);
		 break;
	 default:
		 return;
	}
}

void HandlerButtonback(void *ptr)
{
	BeepBuzzer();
	if(memcmp(&LastSavedTestParams, &stcTestparams[stcTestData.TestId], sizeof(Testparams)) == 0)
	{
		ChangeWindowPage(en_WinID_TestSelectionScreen , en_WinId_TestParamsGeneral);
		stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
	}
	else
	{
		enPopUpKeys Key = ShowMainPopUp("Test Setup","Test Parameters changed. Do you want to save?", false);
		if(enKeyYes == Key)
		{
			HandlerSaveEditedTestParams();
		}
		else
		{
			ChangeWindowPage(en_WinID_TestSelectionScreen , en_WinId_TestParamsGeneral);
			stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
		}
	}
}

void HandlerButtonHome(void *ptr)
{
	ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
	BeepBuzzer();
}

static void HandlerbButtonGeneral(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamsGeneral , (enWindowID)NULL);
	BeepBuzzer();
}
static void HandlerbButtonSettings(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamSettings , (enWindowID)NULL);
	BeepBuzzer();
}
static void HandlerbButtonLimits(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
	BeepBuzzer();
}
static void HandlerbButtonCalib(void *ptr)
{
	return;
}
static void HandlerbButtonStandards(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
	BeepBuzzer();
}

void ShowSettingsCalibGraphPicture(void){

	if(en_Factor == GetInstance_TestParams()[stcTestData.TestId].Calib || en_1PointLinear == GetInstance_TestParams()[stcTestData.TestId].Calib)
	{
		pParamsCalcurve.Set_background_image_pic(PIC_FACTOR_1POINT_GRAPH);
	}
	else if(en_2PointLinear == GetInstance_TestParams()[stcTestData.TestId].Calib)
	{
		pParamsCalcurve.Set_background_image_pic(PIC_2POINT_GRAPH);
	}
	else if(en_PointToPoint == GetInstance_TestParams()[stcTestData.TestId].Calib)
	{
		pParamsCalcurve.Set_background_image_pic(PIC_POINTTOPOINT_GRAPH);
	}
	else if(en_LinearRegression == GetInstance_TestParams()[stcTestData.TestId].Calib)
	{
		pParamsCalcurve.Set_background_image_pic(PIC_LINEAR_GRAPH);
	}
	else if(en_CubicSpline == GetInstance_TestParams()[stcTestData.TestId].Calib)
	{
		pParamsCalcurve.Set_background_image_pic(PIC_CUBIC_GRAPH);
	}
	else
	{
		pParamsCalcurve.Set_background_image_pic(PIC_4PLL_GRAPH); /*4PL*/
	}
}

void ValidateNumOfStd(void)
{
	enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;
	uint8_t MaxNumOfStd = MaxNumOfStandardsForSelectedMethod(CalibType);
	uint8_t MinNumOfStd = MinNumOfStandardsForSelectedMethod(CalibType);
	uint8_t NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
	if(MinNumOfStd > NumOfStdSaved || MaxNumOfStd < NumOfStdSaved)
	{
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enkeyOk == ShowMainPopUp("Standards","No of standard is out of range", true))
		{
			GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved = MinNumOfStd;
			ChangeWindowPage(en_WinId_TestParamCalib , (enWindowID)NextWindow);
		}
	}
}
bool TestCalibMillsDelay(uint32_t Delay)
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

