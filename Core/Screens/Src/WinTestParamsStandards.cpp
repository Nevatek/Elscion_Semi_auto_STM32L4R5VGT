/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include <stdio.h>
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "../Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/NVM_QCSetup.hpp"
#include "../APPL/Inc/calibration.h"
#include <cstring>
#include <string.h>
#include <string>

extern uint16_t g_NumOfTPAvailable;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
static NexButton bDeleteButton = NexButton(en_WinId_TestParamStandards , 8 , "b6");
static NexButton bBackButton = NexButton(en_WinId_TestParamStandards , 7 , "b5");
static NexButton bGraphButton = NexButton(en_WinId_TestParamStandards , 9 , "b7");
static NexButton bButtonSave = NexButton(en_WinId_TestParamStandards , 10 , "b8");
static NexButton bButtonPrint = NexButton(en_WinId_TestParamStandards , 11 , "b9");

static NexButton bButtonGeneral = NexButton(en_WinId_TestParamStandards , 6 , "b0");
static NexButton bButtonSettings = NexButton(en_WinId_TestParamStandards , 5 , "b1");
static NexButton bButtonLimits = NexButton(en_WinId_TestParamStandards , 4 , "b2");
static NexButton bButtonCalib = NexButton(en_WinId_TestParamStandards , 3 , "b3");
static NexButton bButtonStandards = NexButton(en_WinId_TestParamStandards , 2 , "b4");

#if 0
static NexText textCalibGraphXMin = NexText(en_WinId_TestCalibScreen , 23 , "t10");
static NexText textCalibGraphYMin = NexText(en_WinId_TestCalibScreen , 22 , "t8");
static NexText textCalibGraphXMax = NexText(en_WinId_TestCalibScreen , 21 , "t66");
static NexText textCalibGraphYMax = NexText(en_WinId_TestCalibScreen , 20 , "t4");
#endif

static NexText arr_tConc[MAX_NUM_OF_STANDARD_CONC] = {NexText(en_WinId_TestParamStandards , 23 , "t13"),
												NexText(en_WinId_TestParamStandards , 13 , "t9"),
												NexText(en_WinId_TestParamStandards , 14 , "t0"),
												NexText(en_WinId_TestParamStandards , 19 , "t5"),
												NexText(en_WinId_TestParamStandards , 21 , "t8"),
												NexText(en_WinId_TestParamStandards , 17 , "t3")};

static NexText arr_tODS[MAX_NUM_OF_STANDARD_OD] = {	NexText(en_WinId_TestParamStandards , 29 , "t20"),
													NexText(en_WinId_TestParamStandards , 24 , "t14"),
													NexText(en_WinId_TestParamStandards , 25 , "t15"),
													NexText(en_WinId_TestParamStandards , 27 , "t17"),
													NexText(en_WinId_TestParamStandards , 28 , "t18"),
													NexText(en_WinId_TestParamStandards , 26 , "t16")};

static NexText arr_tSlNo[MAX_NUM_OF_STANDARDS] = {	NexText(en_WinId_TestParamStandards , 22 , "t12"),
													NexText(en_WinId_TestParamStandards , 12 , "t7"),
													NexText(en_WinId_TestParamStandards , 15 , "t1"),
													NexText(en_WinId_TestParamStandards , 18 , "t4"),
													NexText(en_WinId_TestParamStandards , 20 , "t6"),
													NexText(en_WinId_TestParamStandards , 16 , "t2")};

static NexText textStdconc1 = NexText(en_WinId_TestParamStandards , 30 , "t10");
static NexText textStdabs1 = NexText(en_WinId_TestParamStandards , 31 , "t11");
static NexText textStdconc2 = NexText(en_WinId_TestParamStandards , 33 , "t21");
static NexText textStdabs2 = NexText(en_WinId_TestParamStandards , 32 , "t19");

static NexText textRBlank_Water = NexText(en_WinId_TestParamStandards , 37 , "t23");
static NexText textBlankOD = NexText(en_WinId_TestParamStandards , 38 , "t24");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bDeleteButton,
										&bBackButton,
										&bGraphButton,
										&bButtonSave,
										&bButtonPrint,
										&bButtonGeneral,
										&bButtonSettings,
										&bButtonLimits,
										&bButtonCalib,
										&bButtonStandards,
										&arr_tConc[0],
										&arr_tConc[1],
										&arr_tConc[2],
										&arr_tConc[3],
										&arr_tConc[4],
										&arr_tConc[5],
										&arr_tODS[0],
										&arr_tODS[1],
										&arr_tODS[2],
										&arr_tODS[3],
										&arr_tODS[4],
										&arr_tODS[5],
										&textStdconc1,
										&textStdconc2,
										&textStdabs1,
										&textStdabs2,
										&textBlankOD,
										NULL};
static void ShowStandardBoxes(uint8_t NumofStd);

static void HandlerButtonDelete(void *ptr);
static void HandlerButtonback(void *ptr);
static void HandlerGraphButton(void *ptr);
static void HandlerButtonPrint(void *ptr);

static void HandlerbButtonGeneral(void *ptr);
static void HandlerbButtonSettings(void *ptr);
static void HandlerbButtonLimits(void *ptr);
static void HandlerbButtonCalib(void *ptr);
static void HandlerbButtonStandards(void *ptr);
static void HandlerButtonSaveTestParams(void *ptr);

static void HandlerTextConcS0(void *ptr);
static void HandlerTextConcS1(void *ptr);
static void HandlerTextConcS2(void *ptr);
static void HandlerTextConcS3(void *ptr);
static void HandlerTextConcS4(void *ptr);
static void HandlerTextConcS5(void *ptr);
static void HandlerTextODS0(void *ptr);
static void HandlerTextODS1(void *ptr);
static void HandlerTextODS2(void *ptr);
static void HandlerTextODS3(void *ptr);
static void HandlerTextODS4(void *ptr);
static void HandlerTextODS5(void *ptr);
static void HandlerTextODRBlank(void *ptr);

enWindowStatus ShowTestParamsStandards(NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bDeleteButton.attachPush(HandlerButtonDelete, &bDeleteButton);
	bBackButton.attachPush(HandlerButtonback, &bBackButton);
	bGraphButton.attachPush(HandlerGraphButton, &bGraphButton);
	bButtonSave.attachPush(HandlerButtonSaveTestParams, &bButtonSave);

	bButtonGeneral.attachPush(HandlerbButtonGeneral, &bButtonGeneral);
	bButtonSettings.attachPush(HandlerbButtonSettings, &bButtonSettings);
	bButtonLimits.attachPush(HandlerbButtonLimits, &bButtonLimits);
	bButtonCalib.attachPush(HandlerbButtonCalib, &bButtonCalib);
	bButtonStandards.attachPush(HandlerbButtonStandards, &bButtonStandards);
	bButtonPrint.attachPush(HandlerButtonPrint, &bButtonPrint);

	arr_tConc[0].attachPush(HandlerTextConcS0, &arr_tConc[0]);
	arr_tConc[1].attachPush(HandlerTextConcS1, &arr_tConc[1]);
	arr_tConc[2].attachPush(HandlerTextConcS2, &arr_tConc[2]);
	arr_tConc[3].attachPush(HandlerTextConcS3, &arr_tConc[3]);
	arr_tConc[4].attachPush(HandlerTextConcS4, &arr_tConc[4]);
	arr_tConc[5].attachPush(HandlerTextConcS5, &arr_tConc[5]);
	arr_tODS[0].attachPush(HandlerTextODS0, &arr_tODS[0]);
	arr_tODS[1].attachPush(HandlerTextODS1, &arr_tODS[1]);
	arr_tODS[2].attachPush(HandlerTextODS2, &arr_tODS[2]);
	arr_tODS[3].attachPush(HandlerTextODS3, &arr_tODS[3]);
	arr_tODS[4].attachPush(HandlerTextODS4, &arr_tODS[4]);
	arr_tODS[5].attachPush(HandlerTextODS5, &arr_tODS[5]);
	textBlankOD.attachPush(HandlerTextODRBlank, &textBlankOD);
	ShowStandardBoxes(GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);

	return WinStatus;
}

void HandlerTestParamsStandards (NexPage *ptr_obJCurrPage)
{
	if(en_WinID_NumericKeypad == stcScreenNavigation.PrevWindowId)
	{
		char arr_Buffer[64] = {0};
			/* 1. Standard absorbance limit conditions starts here*/
			uint8_t NumofStd = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
			for(uint8_t nI = 0 ; nI < NumofStd ; nI++)
			{
				if(GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI] < DEFAULT_ABS_LOW_NEG || GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI] > DEFAULT_ABS_HIGH)
				{
					float	MinAbsValue = DEFAULT_ABS_LOW;
					float	MaxAbsValue = DEFAULT_ABS_HIGH;
					snprintf(arr_Buffer , 61 , "Please enter Standard Abs ( %0.1f - %0.1f ) Abs" , MinAbsValue , MaxAbsValue);

					if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
					{
						GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI] = DEFAULT_ABS_LOW;
						ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
						break;
					}
				}
			}
			/* 1. Standard absorbance limit conditions starts here*/
		stcScreenNavigation.PrevWindowId = en_WinId_TestParamStandards;
	}
	nexLoop(nex_Listen_List);
}
static void HandlerButtonPrint(void *ptr)
{
	BeepBuzzer();
	PrintTestParams(stcTestData.TestId);
}
void HandlerTextConcS0(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardConc[0] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardConc[0]) , true ,"Concentration S1" , " " , 7);
	BeepBuzzer();
}
void HandlerTextConcS1(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardConc[1] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardConc[1]) , true ,"Concentration S2" , " " , 7);
	BeepBuzzer();
}
void HandlerTextConcS2(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardConc[2] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardConc[2]) , true ,"Concentration S3" , " " , 7);
	BeepBuzzer();
}
void HandlerTextConcS3(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardConc[3] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardConc[3]) , true ,"Concentration S4" , " " , 7);
	BeepBuzzer();
}
void HandlerTextConcS4(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardConc[4] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardConc[4]) , true ,"Concentration S5" , " " , 7);
	BeepBuzzer();
}
void HandlerTextConcS5(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardConc[5] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardConc[5]) , true ,"Concentration S6" , " " , 7);
	BeepBuzzer();
}
void HandlerTextODS0(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardOD[0] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardOD[0]) , true ,"OD S1" , " " , 7);
	BeepBuzzer();
}
void HandlerTextODS1(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardOD[1] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardOD[1]) , true ,"OD S2" , " " , 7);
	BeepBuzzer();
}
void HandlerTextODS2(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardOD[2] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardOD[2]) , true ,"OD S3" , " " , 7);
	BeepBuzzer();
}
void HandlerTextODS3(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardOD[3] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardOD[3]) , true ,"OD S4" , " " , 7);
	BeepBuzzer();
}
void HandlerTextODS4(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardOD[4] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardOD[4]) , true ,"OD S5" , " " , 7);
	BeepBuzzer();
}
void HandlerTextODS5(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].StandardOD[5] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].StandardOD[5]) , true ,"OD S6" , " " , 7);
	BeepBuzzer();
}
void HandlerTextODRBlank(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs) , true ,"Reagent Blank OD" , " " , 7);
	BeepBuzzer();
}

void HandlerSaveEditedTestParams(void)
{
	uint8_t NumofStd = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
	bool allConcentrationsNonZero = true;
	bool allAbsNonZero = true;
	bool RepeatTestnameflag = false;
	bool StdConcInIncreasing = false;
	bool StdAbsInIncreasing = false;
	char currentTestName[MAX_TEST_NAME_CHAR_LENGTH + 1];
	char comparedTestName[MAX_TEST_NAME_CHAR_LENGTH + 1];

	/*Confirming the Std conc values not to be zero*/
	for(uint8_t nI = 0 ; nI < NumofStd ; nI++)
	{
		if(GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI] == 0)
		{
			if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_4PL || GetInstance_TestParams()[stcTestData.TestId].Calib == en_CubicSpline)
			{
				if(nI != 0)
				{
					if(enkeyOk == ShowMainPopUp("Warning","Concentration should not be zero", true))
					{
						ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
						allConcentrationsNonZero = false;
						break;
					}
				}
			}
			else
			{
				if(enkeyOk == ShowMainPopUp("Warning","Concentration should not be zero", true))
				{
					ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
					allConcentrationsNonZero = false;
					break;
				}
			}
		}
	}
	/*Confirming the same Test name is present or not */
	strcpy(currentTestName, GetInstance_TestParams()[stcTestData.TestId].arrTestName);
	for(int nI = 0 ; nI < g_NumOfTPAvailable ; ++nI)
	{
		strcpy(comparedTestName, GetInstance_TestParams()[nI].arrTestName);
	    if(strcmp(currentTestName, comparedTestName) == 0)
		{
			if(nI != stcTestData.TestId)
			{
				if(enkeyOk == ShowMainPopUp("Warning","Test Name should not be same", true))
				{
					ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
					RepeatTestnameflag = true;
					break;
				}
			}
		}
	}

	if (allConcentrationsNonZero == true && RepeatTestnameflag == false)
	{
		StdConcInIncreasing = isInIncreasingOrder(GetInstance_TestParams()[stcTestData.TestId].StandardConc, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
		StdAbsInIncreasing  = isInIncreasingOrder(GetInstance_TestParams()[stcTestData.TestId].StandardOD, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);

	   if (!StdConcInIncreasing)
		{
			if(enKeyYes == ShowMainPopUp("Warning","Std Concentration not in increasing order \\rDo you want to continue?", false))
			{
				StdConcInIncreasing = true;
			}
		}

	   if(!StdAbsInIncreasing)
	   {
			if(enKeyYes == ShowMainPopUp("Warning","Std Abs. not in increasing order \\rDo you want to continue?", false))
			{
				StdAbsInIncreasing = true;
			}
	   }

	   if(StdConcInIncreasing == true && StdAbsInIncreasing == true)
	   {
			ShowPleaseWaitPopup("Test Setup",PLEASE_WAIT);
			GetInstance_TestParams()[stcTestData.TestId].ValidTestParamSavedFlag = VALID_TEST_PARAM_FLAG;
			enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;

			if( en_1PointLinear == CalibType)
			{
				if(GetInstance_TestParams()[stcTestData.TestId].StandardOD[0] != 0 && GetInstance_TestParams()[stcTestData.TestId].KFactor == DEFAULT_K_FACTOR)
				{
					GetInstance_TestParams()[stcTestData.TestId].KFactor = CalculateFactor_Graph();
				}
			}
			else if(CalibType == en_4PL )
			{
				for(int i = 0; i < NumofStd; i++)
				{
					if(GetInstance_TestParams()[stcTestData.TestId].StandardOD[i] == 0)
					{
						allAbsNonZero = false;
					}
				}
				if((memcmp(LastSavedTestParams.StandardOD, GetInstance_TestParams()[stcTestData.TestId].StandardOD, sizeof(LastSavedTestParams.StandardOD)) != 0) ||
						(memcmp(LastSavedTestParams.StandardConc, GetInstance_TestParams()[stcTestData.TestId].StandardConc, sizeof(LastSavedTestParams.StandardConc)) != 0))
				{
					if(allAbsNonZero)
					{
						Calibrate_4PL_initial_params(stcTestData.TestId);
					}
				}
			}
			WriteTestparameterBuffer();/*Writing tets params to memory*/

			if(enkeyOk == ShowMainPopUp("Test Setup","Test Parameters saved", true))
			{
				ChangeWindowPage(en_WinID_TestSelectionScreen , en_WinId_TestParamsGeneral);
				stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
			}
	   }
	   else
	   {
			ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
	   }
	}
}

void HandlerButtonSaveTestParams(void *ptr)/*show popups*/
{
	BeepBuzzer();
	HandlerSaveEditedTestParams();
}

void HandlerButtonDelete(void *ptr)
{
	BeepBuzzer();
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	enPopUpKeys Key = ShowMainPopUp("Test Setup","Do you want to delete?" , false);
	if(enKeyYes == Key)
	{
		ShowPleaseWaitPopup("Test Setup",PLEASE_WAIT);
		GetInstance_TestParams()[stcTestData.TestId].ValidTestParamSavedFlag = INVALID_TEST_PARAM_FLAG;
		/*Deleting all 4 QCs data s*/
		for(uint8_t u8SlotIndex = 0 ; u8SlotIndex < MAX_NUM_OF_QC_CONTROL_PER_TEST ; ++u8SlotIndex)
		{
			m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].u8QcIdx = NVM_INIT_NOT_OK_FLAG;
			m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].u8NumOfResSaved = 0;
			m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].fHighVal = 0;
			m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].fLowVal = 0;
			m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].fMeanVal = 0;
		}

		WriteTestparameterBuffer();/*Writing tets params to memory*/
		NVM_WriteQcSetup();
		ChangeWindowPage(en_WinID_TestSelectionScreen , en_WinId_TestParamsGeneral);
		stcScreenNavigation.PrevWindowId = en_WinID_MainMenuScreen;
	}
	else
	{
		ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
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

void HandlerGraphButton(void *ptr)
{
	BeepBuzzer();
	ChangeWindowPage(en_WinId_TestParamStdGraph , (enWindowID)NULL);
	uint8_t NumofStd = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
	bool allConcentrationsNonZero = true;

	for(uint8_t nI = 0 ; nI < NumofStd ; nI++)
	{
		if(GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI] == 0)
		{
			if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_4PL || GetInstance_TestParams()[stcTestData.TestId].Calib == en_CubicSpline)
			{
				if(nI != 0)
				{
					if(enkeyOk == ShowMainPopUp("Warning","Concentration should not be zero", true))
					{
						ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
						allConcentrationsNonZero = false;
						break;
					}
				}
			}
			else
			{
				if(enkeyOk == ShowMainPopUp("Warning","Concentration should not be zero", true))
				{
					ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
					allConcentrationsNonZero = false;
					break;
				}
			}
		}
	}

	if (allConcentrationsNonZero)
	{
	   if (isInIncreasingOrder(GetInstance_TestParams()[stcTestData.TestId].StandardConc, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved))
	   {
		   if((isInIncreasingOrder(GetInstance_TestParams()[stcTestData.TestId].StandardOD, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)))
		   {
			ChangeWindowPage(en_WinId_TestParamStdGraph , (enWindowID)NULL);
		   }
		   else
		   {
				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
				if(enkeyOk == ShowMainPopUp("Warning","Please enter Abs value from low to high", true))
				{
					ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NextWindow);
				}
		   }
	   }
	   else
	   {
			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
			if(enkeyOk == ShowMainPopUp("Warning","Please enter Conc. value from low to high", true))
			{
				ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NextWindow);
			}
		}
	}
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
	ChangeWindowPage(en_WinId_TestParamCalib , (enWindowID)NULL);
	BeepBuzzer();
}
static void HandlerbButtonStandards(void *ptr)
{
	return;
}
void ShowStandardBoxes(uint8_t NumofStd)
{
	char arr_Buffer[64] = {0};
	if(NumofStd>0){
		textStdconc1.setVisible(true);
		textStdabs1.setVisible(true);
	}
	if(NumofStd>3){
		textStdconc2.setVisible(true);
		textStdabs2.setVisible(true);
	}
	if(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable == true)
	{
		textRBlank_Water.setText("R Blank");
		textRBlank_Water.setVisible(true);
		textBlankOD.setVisible(true);
		textStdabs1.setVisible(true);
		snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs);
		textBlankOD.setText(arr_Buffer);
	}
	else
	{
		textRBlank_Water.setText("DI Water");
		textRBlank_Water.setVisible(true);
		textBlankOD.setVisible(false);
	}
	for(uint8_t nI = 0 ;nI < NumofStd ; ++nI)
	{
		arr_tConc[nI].setVisible(true);
		arr_tODS[nI].setVisible(true);
		arr_tSlNo[nI].setVisible(true);

		snprintf(arr_Buffer , 61 , "%.2f" , GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI]);
		arr_tConc[nI].setText(arr_Buffer);

		snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI]);
		arr_tODS[nI].setText(arr_Buffer);
	}
}

int isInIncreasingOrder(float arr[], int size)
{
	if(size >= 2)
	{
	    for (int i = 1; i < size; i++)
	    {
	        if (( arr[i] <= arr[i - 1] ) && (( arr[i] || arr[i-1]) !=0 ))
	//        if (( arr[i] <= arr[i - 1] ))
	        {
	            return false; // Not in increasing order
	        }
	    }
	    return true; // In increasing order
	}
	else
	{
		  return true; // In if only one data
	}

}

