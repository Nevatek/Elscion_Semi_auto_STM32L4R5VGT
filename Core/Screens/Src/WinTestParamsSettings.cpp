/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "./../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"

#define MAX_PREDEFINED_TEMP (4U)
float g_TemperatureTargetList[MAX_PREDEFINED_TEMP] = {ROOM_TEMP_VAL/*Room Temp*/ , float(25) , float(30) , float(37)};
bool g_BichromaticFlag = false;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
static NexButton bBackButton = NexButton(en_WinId_TestParamSettings , 7 , "b5");
static NexButton bHomeButton = NexButton(en_WinId_TestParamSettings , 8 , "b7");

static NexButton bButtonGeneral = NexButton(en_WinId_TestParamSettings , 6 , "b0");
static NexButton bButtonSettings = NexButton(en_WinId_TestParamSettings , 5 , "b1");
static NexButton bButtonLimits = NexButton(en_WinId_TestParamSettings , 4 , "b2");
static NexButton bButtonCalib = NexButton(en_WinId_TestParamSettings , 3 , "b3");
static NexButton bButtonStandards = NexButton(en_WinId_TestParamSettings , 2 , "b4");

static NexButton bCbButtonTestType = NexButton(en_WinId_TestParamSettings , 9 , "cb2");
static NexButton bCbButtonPirmFilter = NexButton(en_WinId_TestParamSettings , 12 , "cb0");
static NexButton bCbButtonSecFilter = NexButton(en_WinId_TestParamSettings , 22 , "cb1");
static NexButton bCbButtonTemp = NexButton(en_WinId_TestParamSettings , 13 , "cb3");
static NexText   bCbButtonAspVol = NexText(en_WinId_TestParamSettings , 26 , "t10");
static NexButton bCbButtonCuvetteType = NexButton(en_WinId_TestParamSettings , 14 , "cb5");
static NexButton bCbButtonbBichromatic = NexButton(en_WinId_TestParamSettings , 24 , "b8");
static NexText   bBichromaticDisable = NexText(en_WinId_TestParamSettings , 34 , "t17");
static NexText 	 tCbSecFilDisable = NexText(en_WinId_TestParamSettings , 33 , "t16");
static NexText 	 tTextBichromatic = NexText(en_WinId_TestParamSettings , 23 , "t11");
static NexText   tTextSecWavelength = NexText(en_WinId_TestParamSettings , 25 , "t2");
//static NexPicture pArrowSecWavelength = NexPicture(en_WinId_TestParamSettings , 15 , "p1");

static NexText tLagTime = NexText(en_WinId_TestParamSettings , 18 , "t8");
static NexText tReadTime = NexText(en_WinId_TestParamSettings , 19 , "t9");

static NexText tCorrelationA = NexText(en_WinId_TestParamSettings , 31 , "t14");
static NexText tCorrelationB = NexText(en_WinId_TestParamSettings , 32 , "t15");


/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bButtonStandards,
										&bBackButton,
										&bHomeButton,
										&bButtonGeneral,
										&bButtonSettings,
										&bButtonLimits,
										&bButtonCalib,
										&bCbButtonPirmFilter,
										&bCbButtonTestType,
										&bCbButtonSecFilter,
										&bCbButtonTemp,
										&bCbButtonAspVol,
										&bCbButtonCuvetteType,
										&tLagTime,
										&tReadTime,
										&tCorrelationA,
										&tCorrelationB,
										&bCbButtonbBichromatic,
										NULL};

uint8_t g_u8TimeCounter = 0;
static void HandlerButtonback(void *ptr);
static void HandlerButtonHome(void *ptr);

static void HandlerbButtonGeneral(void *ptr);
static void HandlerbButtonSettings(void *ptr);
static void HandlerbButtonLimits(void *ptr);
static void HandlerbButtonCalib(void *ptr);
static void HandlerbButtonStandards(void *ptr);

static void HandlerComboBoxTestType(void *ptr);
static void HandlerComboBoxPirmFilter(void *ptr);
static void HandlerComboBoxSecFilter(void *ptr);
static void HandlerComboBoxTemp(void *ptr);
static void HandlerComboBoxAspVol(void *ptr);
static void HandlerComboBoxCuvetteType(void *ptr);
static void HandlertextLagTime(void *ptr);
static void HandlertextReadTime(void *ptr);
static void HandlerbButtonbBichromatic(void *ptr);
void SetBichromaticStatus(bool Currstatus);
static bool TestParamsMillsDelay(uint32_t Delay);
static void HandlertextCorr_A(void *ptr);
static void HandlertextCorr_B(void *ptr);

enWindowStatus ShowTestParamsSettings(NexPage *ptr_obJCurrPage)
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

	bCbButtonTestType.attachPush(HandlerComboBoxTestType, &bCbButtonTestType);
	bCbButtonPirmFilter.attachPush(HandlerComboBoxPirmFilter, &bCbButtonPirmFilter);
	bCbButtonSecFilter.attachPush(HandlerComboBoxSecFilter, &bCbButtonSecFilter);
	bCbButtonTemp.attachPush(HandlerComboBoxTemp, &bCbButtonTemp);
	bCbButtonAspVol.attachPush(HandlerComboBoxAspVol, &bCbButtonAspVol);
	bCbButtonCuvetteType.attachPush(HandlerComboBoxCuvetteType, &bCbButtonCuvetteType);
	tLagTime.attachPush(HandlertextLagTime, &tLagTime);
	tReadTime.attachPush(HandlertextReadTime, &tReadTime);
	tCorrelationA.attachPush(HandlertextCorr_A, &tCorrelationA);
	tCorrelationB.attachPush(HandlertextCorr_B, &tCorrelationB);
	bCbButtonbBichromatic.attachPush(HandlerbButtonbBichromatic, &bCbButtonbBichromatic);

	char arr_Buffer[64] = {0};
	snprintf(arr_Buffer , 61 , "%d" , (int)GetInstance_TestParams()[stcTestData.TestId].AspVoleume);
	bCbButtonAspVol.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%d" , (int)GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
	tReadTime.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%d" , (int)GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec);
	tLagTime.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%.01f" , GetInstance_TestParams()[stcTestData.TestId].Corr_A);
	tCorrelationA.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%.01f" , GetInstance_TestParams()[stcTestData.TestId].Corr_B);
	tCorrelationB.setText(arr_Buffer);

	bCbButtonTestType.setValue(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType);
	bCbButtonPirmFilter.setValue(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength - 1);/*Because in UI home is not there*/
	bCbButtonSecFilter.setValue(GetInstance_TestParams()[stcTestData.TestId].SecWavelength - 1);/*Because in UI home is not there*/
	bCbButtonCuvetteType.setValue(GetInstance_TestParams()[stcTestData.TestId].CuvetteType);

	tCbSecFilDisable.setVisible(false);
	bCbButtonSecFilter.setVisible(false);
	bCbButtonbBichromatic.setVisible(false);
	bBichromaticDisable.setVisible(false);

	for(uint8_t Idx = 0 ; Idx < MAX_PREDEFINED_TEMP ; ++Idx)
	{
		if(g_TemperatureTargetList[Idx] == GetInstance_TestParams()[stcTestData.TestId].Temperature)
		{
			bCbButtonTemp.setValue(Idx);
			break;
		}
	}

	enTestProcedureType TestType = GetInstance_TestParams()[stcTestData.TestId].TestProcedureType;
	if(en_Endpoint == TestType || en_Absorbance == TestType)
	{
		bCbButtonbBichromatic.setVisible(true);
		bBichromaticDisable.setVisible(false);

		if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
		{
			g_BichromaticFlag = true;
		}
		else
		{
			g_BichromaticFlag = false;
		}
	}
	else
	{
		bCbButtonbBichromatic.setVisible(false);
		bBichromaticDisable.setVisible(true);
		g_BichromaticFlag = false;
	}
	SetBichromaticStatus(g_BichromaticFlag);
	return WinStatus;
}

void HandlerTestParamsSettings (NexPage *ptr_obJCurrPage)
{

	if(en_WinID_NumericKeypad == stcScreenNavigation.PrevWindowId)
	{
		char arr_Buffer[64] = {0};
			/* 1. Aspiration volume limit conditions starts here*/
			if(GetInstance_TestParams()[stcTestData.TestId].AspVoleume < MIN_ASP_VOL || GetInstance_TestParams()[stcTestData.TestId].AspVoleume > MAX_ASP_VOL)
			{
				int	MinAspVol = MIN_ASP_VOL;
				int	MaxAspVol = MAX_ASP_VOL;
				snprintf(arr_Buffer , 61 , "Please enter Aspiration vol ( %d - %d ) uL" , MinAspVol , MaxAspVol);
				if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
				{
					GetInstance_TestParams()[stcTestData.TestId].AspVoleume = DEFAULT_ASP_VOL;
					ChangeWindowPage(en_WinId_TestParamSettings , (enWindowID)NULL);
				}
			}

			if(GetInstance_TestParams()[stcTestData.TestId].AspVoleume < 300)
			{
				snprintf(arr_Buffer , 128 , "If the aspiration volume is below 300uL, \\rThe system automatically enables an inter-sample wash to prevent carryover");
				if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
				{
					ChangeWindowPage(en_WinId_TestParamSettings , (enWindowID)NULL);
				}
			}
			/* 1. Aspiration volume limit conditions ends here*/

			/* 2. Incubating time limit conditions starts here*/

			if(GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec < MIN_LAG_TIME || GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec > MAX_LAG_TIME)
			{
				int	MinLagTime = MIN_LAG_TIME;
				int	MaxLagTime = MAX_LAG_TIME;
				snprintf(arr_Buffer , 61 , "Please enter Lag time ( %d - %d ) sec" , MinLagTime , MaxLagTime);

				if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
				{
					GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec = INCUBATION_TIME_FOR_WATER_BLANK_SEC;
					ChangeWindowPage(en_WinId_TestParamSettings , (enWindowID)NULL);
				}
			}

			/* 2. Incubating time limit conditions ends here*/


			/* 3. Measuring time limit conditions starts here*/

			if(GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec < MIN_READ_TIME || GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec > MAX_READ_TIME)
			{
				int	MinReadTime = MIN_READ_TIME;
				int	MaxReadTime = MAX_READ_TIME;
				snprintf(arr_Buffer , 61 , "Please enter Read time ( %d - %d ) sec" , MinReadTime , MaxReadTime);

				if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
				{
					GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec = MEASUREMENT_TIME_FOR_WATER_BLANK_SEC;
					ChangeWindowPage(en_WinId_TestParamSettings , (enWindowID)NULL);
				}
			}

			if(GetInstance_TestParams()[stcTestData.TestId].Corr_A == 0)
			{
				if(enkeyOk == ShowMainPopUp("Warning","Correlation factor (A) should not be zero", true))
				{
					GetInstance_TestParams()[stcTestData.TestId].Corr_A = 1;
					ChangeWindowPage(en_WinId_TestParamSettings , (enWindowID)NULL);
				}
			}
			/* 3. Measuring time limit conditions ends here*/
		stcScreenNavigation.PrevWindowId = en_WinId_TestParamSettings;
	}

	nexLoop(nex_Listen_List);
		uint32_t val = 0;

	if(TestParamsMillsDelay(300))
	{
		if(1 == g_u8TimeCounter)
		{
			if(true == bCbButtonSecFilter.getValue((uint32_t*)&val))
			{
				if((en_WavelengthFilter)(val + 1) != GetInstance_TestParams()[stcTestData.TestId].SecWavelength &&
						en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength &&
						en_Filter340 <= (val+1) && en_Filter630 >= (val+1))
				{
					ClearTestSavedValues();
					GetInstance_TestParams()[stcTestData.TestId].SecWavelength = (en_WavelengthFilter)(val + 1);
					BeepBuzzer();
				}
			}
		}
		else if(2 == g_u8TimeCounter)
		{
			if(true == bCbButtonTemp.getValue((uint32_t*)&val))
			{
				if(0 <= (val+1) && MAX_PREDEFINED_TEMP >= (val+1))
				{
					if((float)g_TemperatureTargetList[val] != GetInstance_TestParams()[stcTestData.TestId].Temperature)
					{
						ClearTestSavedValues();
						GetInstance_TestParams()[stcTestData.TestId].Temperature = (float)g_TemperatureTargetList[val];
						BeepBuzzer();
					}
				}
			}
		}
		else if(3 == g_u8TimeCounter)
		{
			if(true == bCbButtonPirmFilter.getValue((uint32_t*)&val))
			{
				if((en_WavelengthFilter)(val + 1) != GetInstance_TestParams()[stcTestData.TestId].PrimWavelength &&
						 en_Filter340 <= (val+1) && en_Filter630 >=(val+1))
				{
					ClearTestSavedValues();
					GetInstance_TestParams()[stcTestData.TestId].PrimWavelength = (en_WavelengthFilter)(val + 1);
					BeepBuzzer();
				}
			}
		}
		else if(4 == g_u8TimeCounter)
		{
			if(true == bCbButtonCuvetteType.getValue((uint32_t*)&val))
			{
				if((enCuvetteType)val != GetInstance_TestParams()[stcTestData.TestId].CuvetteType &&
						en_Flowcell <= val && en_Coagulationtube > val)
				{
					GetInstance_TestParams()[stcTestData.TestId].CuvetteType = (enCuvetteType)val;
					BeepBuzzer();
				}
			}
		}
		else if(4 < g_u8TimeCounter)
		{
			g_u8TimeCounter = 0;

			if(true == bCbButtonTestType.getValue((uint32_t*)&val))
			{
				if((enTestProcedureType)val != GetInstance_TestParams()[stcTestData.TestId].TestProcedureType  &&
						en_Endpoint <= val && en_CoagulationTest > val)
				{
					ClearTestSavedValues();
					GetInstance_TestParams()[stcTestData.TestId].TestProcedureType = (enTestProcedureType)val;
					if(en_Endpoint == GetInstance_TestParams()[stcTestData.TestId].TestProcedureType
							|| en_Absorbance == GetInstance_TestParams()[stcTestData.TestId].TestProcedureType)
					{
						bCbButtonbBichromatic.setVisible(true);
						bBichromaticDisable.setVisible(false);
						if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
						{
							g_BichromaticFlag = true;
						}
						else
						{
							g_BichromaticFlag = false;
						}
					}
					else
					{
						bCbButtonbBichromatic.setVisible(false);
						bBichromaticDisable.setVisible(true);
						g_BichromaticFlag = false;
					}
					SetBichromaticStatus(g_BichromaticFlag);
					BeepBuzzer();
				}
			}
		}
		g_u8TimeCounter++;
	}
}

void HandlerbButtonbBichromatic(void *ptr)
{
	enTestProcedureType TestType = GetInstance_TestParams()[stcTestData.TestId].TestProcedureType;
	if((en_Endpoint == TestType || en_Absorbance == TestType) && false == g_BichromaticFlag)
	{
		g_BichromaticFlag = true;
	}
	else
	{
		g_BichromaticFlag = false;
	}
	GetInstance_TestParams()[stcTestData.TestId].SecWavelength = en_Filter340;
	SetBichromaticStatus(g_BichromaticFlag);
	BeepBuzzer();
}
void HandlerComboBoxTestType(void *ptr)
{
//	enTestProcedureType TestType = en_Endpoint;
//	if(false == bCbButtonTestType.getValue((uint32_t*)&val))
//	{
//		bCbButtonTestType.setValue(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType);
//		return;
//	}
//	uint32_t val = 0;
//	if(true == bCbButtonTestType.getValue((uint32_t*)&val))
//	{
//		if((enTestProcedureType)val != GetInstance_TestParams()[stcTestData.TestId].TestProcedureType)
//		{
//			ClearTestSavedValues();
//			TestType = GetInstance_TestParams()[stcTestData.TestId].TestProcedureType = (enTestProcedureType)val;
//			BeepBuzzer();
//		}
//	}
//
//	if(en_Endpoint == TestType || en_Absorbance == TestType)
//	{
//		bCbButtonbBichromatic.setVisible(true);
//		tTextBichromatic.setVisible(true);
//		if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
//		{
//			g_BichromaticFlag = true;
//		}
//		else
//		{
//			g_BichromaticFlag = false;
//		}
//	}
//	else
//	{
//		bCbButtonbBichromatic.setVisible(false);
//		tTextBichromatic.setVisible(false);
//		g_BichromaticFlag = false;
//	}
//	SetBichromaticStatus(g_BichromaticFlag);
}
void HandlerComboBoxPirmFilter(void *ptr)
{
//	uint32_t val = 0;
//	if(false == bCbButtonPirmFilter.getValue((uint32_t*)&val))
//	{
//		bCbButtonPirmFilter.setValue(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength - 1);
//		return;
//	}
//	uint32_t val = 0;
//	if(true == bCbButtonPirmFilter.getValue((uint32_t*)&val))
//	{
//		if((en_WavelengthFilter)(val + 1) != GetInstance_TestParams()[stcTestData.TestId].PrimWavelength)
//		{
//			ClearTestSavedValues();
//			GetInstance_TestParams()[stcTestData.TestId].PrimWavelength = (en_WavelengthFilter)(val + 1);
//			BeepBuzzer();
//		}
//	}
}
static void HandlerComboBoxSecFilter(void *ptr)
{
//	uint32_t val = 0;
//	if(false == bCbButtonSecFilter.getValue((uint32_t*)&val))
//	{
//		bCbButtonSecFilter.setValue(GetInstance_TestParams()[stcTestData.TestId].SecWavelength - 1);
//		return;
//	}
//	uint32_t val = 0;
//	if(true == bCbButtonSecFilter.getValue((uint32_t*)&val))
//	{
//		if((en_WavelengthFilter)(val + 1) != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
//		{
//			ClearTestSavedValues();
//			GetInstance_TestParams()[stcTestData.TestId].SecWavelength = (en_WavelengthFilter)(val + 1);
//			BeepBuzzer();
//		}
//	}

}
static void HandlerComboBoxTemp(void *ptr)
{
//	uint32_t val = 0;
//	if(false == bCbButtonTemp.getValue((uint32_t*)&val))
//	{
//		for(uint8_t Idx = 0 ; Idx < MAX_PREDEFINED_TEMP ; ++Idx)
//		{
//			if(g_TemperatureTargetList[Idx] == GetInstance_TestParams()[stcTestData.TestId].Temperature)
//			{
//				bCbButtonTemp.setValue(Idx);
//				break;
//			}
//		}
//		return;
//	}
//	if(0 > val || MAX_PREDEFINED_TEMP < val)
//	{
//		return;/*Return if error values obtained*/
//	}


//	uint32_t val = 0;
//	if(true == bCbButtonTemp.getValue((uint32_t*)&val))
//	{
//		if(0 < val && MAX_PREDEFINED_TEMP > val)
//		{
//			if((float)g_TemperatureTargetList[val] != GetInstance_TestParams()[stcTestData.TestId].Temperature)
//			{
//				ClearTestSavedValues();
//				GetInstance_TestParams()[stcTestData.TestId].Temperature = (float)g_TemperatureTargetList[val];
//			}
//		}
//	}
//	BeepBuzzer();
}
static void HandlerComboBoxAspVol(void *ptr)
{
		char arr_Buffer[64] = {0};
		uint32_t	MinAspVol = MIN_ASP_VOL;
		uint32_t	MaxAspVol = MAX_ASP_VOL;
		snprintf(arr_Buffer , 61 , "Aspiration volume ( %d - %d ) uL" ,(unsigned int)MinAspVol , (unsigned int)MaxAspVol);

	openKeyBoard(en_NumericKeyboard , (uint16_t*)&GetInstance_TestParams()[stcTestData.TestId].AspVoleume , sizeof(GetInstance_TestParams()[stcTestData.TestId].AspVoleume) , false ,
			arr_Buffer , "NO Test" , 5);
	BeepBuzzer();
}
static void HandlerComboBoxCuvetteType(void *ptr)
{
//	uint32_t val = 0;
//	if(true == bCbButtonCuvetteType.getValue((uint32_t*)&val))
//	{
//		GetInstance_TestParams()[stcTestData.TestId].CuvetteType = (enCuvetteType)val;
//		BeepBuzzer();
//	}
}
static void HandlertextLagTime(void *ptr)
{
		char arr_Buffer[64] = {0};
		uint32_t	MinLagTime = MIN_LAG_TIME;
		uint32_t	MaxLagTime = MAX_LAG_TIME;
		snprintf(arr_Buffer , 61 , "Lag time ( %d - %d ) sec" , (unsigned int)MinLagTime , (unsigned int)MaxLagTime);

	openKeyBoard(en_NumericKeyboard , (uint32_t*)&GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec , sizeof(GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec) , false ,
			arr_Buffer , "NO Test" , 5);
	BeepBuzzer();
}
static void HandlertextReadTime(void *ptr)
{
		char arr_Buffer[64] = {0};
		uint32_t	MinReadTime = MIN_READ_TIME;
		uint32_t	MaxReadTime = MAX_READ_TIME;
		snprintf(arr_Buffer , 61 , "Read time ( %d - %d ) sec" , (unsigned int)MinReadTime , (unsigned int)MaxReadTime);

	openKeyBoard(en_NumericKeyboard , (uint32_t*)&GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec , sizeof(GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec) , false ,
			arr_Buffer , "NO Test" , 5);
	BeepBuzzer();
}
static void HandlertextCorr_A(void *ptr)
{
		openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].Corr_A ,
				sizeof(GetInstance_TestParams()[stcTestData.TestId].Corr_A) , true ,"Enter Correlation factor A" , " " , 7);
		BeepBuzzer();
}
static void HandlertextCorr_B(void *ptr)
{
		openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].Corr_B ,
				sizeof(GetInstance_TestParams()[stcTestData.TestId].Corr_B) , true ,"Enter Correlation factor B" , " " , 7);
		BeepBuzzer();
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
	return;
}
static void HandlerbButtonLimits(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
	BeepBuzzer();
}
static void HandlerbButtonCalib(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamCalib , (enWindowID)NULL);
	BeepBuzzer();
}
static void HandlerbButtonStandards(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
	BeepBuzzer();
}

void SetBichromaticStatus(bool Currstatus)
{
	bCbButtonSecFilter.setVisible(Currstatus);
	if(false == Currstatus)
	{
		tCbSecFilDisable.setVisible(true);
		GetInstance_TestParams()[stcTestData.TestId].SecWavelength = en_FilterHome;/*Disabling secondary wavelength using home*/
		bCbButtonSecFilter.setValue(GetInstance_TestParams()[stcTestData.TestId].SecWavelength);
		bCbButtonbBichromatic.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else
	{
		tCbSecFilDisable.setVisible(false);
		bCbButtonbBichromatic.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
}
bool TestParamsMillsDelay(uint32_t Delay)
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
