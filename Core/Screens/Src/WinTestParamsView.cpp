/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bTestParamsViewBack = NexButton(en_WinId_TestParamsView , 3, "b5");
static NexButton bTestParamsViewPrint = NexButton(en_WinId_TestParamsView , 4 , "b0");

/*Test param data objects ID - START*/
static NexText textParamsName = NexText(en_WinId_TestParamsView , 14 , "t49");
static NexText textParamsFullName = NexText(en_WinId_TestParamsView , 42 , "t1");
static NexText textParamsUnits = NexText(en_WinId_TestParamsView , 15 , "t50");
static NexText textParamsReagentBlank = NexText(en_WinId_TestParamsView , 16 , "t51");
static NexText textParamsSampleBlank = NexText(en_WinId_TestParamsView , 17 , "t52");
static NexText textParamsMethod = NexText(en_WinId_TestParamsView , 18 , "t53");
static NexText textParamsPrimaryFilter = NexText(en_WinId_TestParamsView , 19 , "t54");
static NexText textParamsSecondaryFilter = NexText(en_WinId_TestParamsView , 20 , "t55");
static NexText textParamsTemperature = NexText(en_WinId_TestParamsView , 21 , "t56");
static NexText textParamsAx_B = NexText(en_WinId_TestParamsView , 22 , "t83");
static NexText textParamsAspVol = NexText(en_WinId_TestParamsView , 40 , "t64");
static NexText textParamsLagTimeS = NexText(en_WinId_TestParamsView , 39 , "t63");
static NexText textParamsReadTimeS = NexText(en_WinId_TestParamsView , 38 , "t62");
static NexText textParamsCuvetteType = NexText(en_WinId_TestParamsView , 37 , "t61");
static NexText textParamsRefRange = NexText(en_WinId_TestParamsView , 36 , "t60");
static NexText textParamsLinearityRange = NexText(en_WinId_TestParamsView , 35 , "t59");
static NexText textParamsFactor = NexText(en_WinId_TestParamsView , 34 , "t58");
static NexText textParamsCalibrartion = NexText(en_WinId_TestParamsView , 33 , "t57");
static NexText textParamsStandards = NexText(en_WinId_TestParamsView , 32 , "t65");


/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bTestParamsViewBack,
									 &bTestParamsViewPrint,
									 NULL};

static void HandlerbTestParamsViewBack(void *ptr);
static void HandlerbTestParamsViewPrint(void *ptr);

enWindowStatus ShowTestParamsViewScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bTestParamsViewBack.attachPush(HandlerbTestParamsViewBack, &bTestParamsViewBack);
	bTestParamsViewPrint.attachPush(HandlerbTestParamsViewPrint, &bTestParamsViewPrint);

	textParamsName.setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);
	textParamsFullName.setText(GetInstance_TestParams()[stcTestData.TestId].arrFullTestName);
	if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
	{
		textParamsUnits.setText(GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
	}
	else
	{
		textParamsUnits.setText(&arr_cUnitBuffer[GetInstance_TestParams()[stcTestData.TestId].Unit][0]);
	}

	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		textParamsSampleBlank.setText("Yes");
	}
	else//en_BlankTypeReagentBlank :  if(en_ReagentBlank == GetInstance_TestParams()[stcTestData.TestId].BlankType)
	{
		textParamsSampleBlank.setText("No");
	}

	if(true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
	{
		textParamsReagentBlank.setText("Yes");
	}
	else//en_BlankTypeReagentBlank :  if(en_ReagentBlank == GetInstance_TestParams()[stcTestData.TestId].BlankType)
	{
		textParamsReagentBlank.setText("No");
	}

	textParamsMethod.setText(&g_arrTestmethodnameBuffer[GetInstance_TestParams()[stcTestData.TestId].TestProcedureType][0]);
	textParamsPrimaryFilter.setText(&g_arrFilterNames[GetInstance_TestParams()[stcTestData.TestId].PrimWavelength][0]);
	textParamsSecondaryFilter.setText(&g_arrFilterNames[GetInstance_TestParams()[stcTestData.TestId].SecWavelength][0]);
	char arrBuffer[24] = {0};

	if(ROOM_TEMP_VAL == GetInstance_TestParams()[stcTestData.TestId].Temperature)
	{
		snprintf(arrBuffer , 23 , "%s","Disabled");
	}
	else
	{
		snprintf(arrBuffer , 23 , "%.02f",GetInstance_TestParams()[stcTestData.TestId].Temperature);
	}
	textParamsTemperature.setText(arrBuffer);

	snprintf(arrBuffer , 23 , "%u",(unsigned int)GetInstance_TestParams()[stcTestData.TestId].AspVoleume);
	textParamsAspVol.setText(arrBuffer);
	snprintf(arrBuffer , 23 , "%u",(unsigned int)GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec);
	textParamsLagTimeS.setText(arrBuffer);
	snprintf(arrBuffer , 23 , "%u",(unsigned int)GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
	textParamsReadTimeS.setText(arrBuffer);
	textParamsCuvetteType.setText(&g_arrCuvveteNameBuffer[GetInstance_TestParams()[stcTestData.TestId].CuvetteType][0]);
	snprintf(arrBuffer , 23 , "%.02f - %.02f",GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[0],
			GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[1]);
	textParamsRefRange.setText(arrBuffer);
	snprintf(arrBuffer , 23 , "%.02f - %.02f",GetInstance_TestParams()[stcTestData.TestId].LinearityRange[0],
			GetInstance_TestParams()[stcTestData.TestId].LinearityRange[1]);
	textParamsLinearityRange.setText(arrBuffer);
	snprintf(arrBuffer , 23 , "%.02f",GetInstance_TestParams()[stcTestData.TestId].KFactor);
	textParamsFactor.setText(arrBuffer);
	snprintf(arrBuffer , 23 , "%u",(unsigned int)GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
	textParamsStandards.setText(arrBuffer);
	textParamsCalibrartion.setText(&g_CalibrationTypeNameBUffer[GetInstance_TestParams()[stcTestData.TestId].Calib][0]);
	snprintf(arrBuffer , 23 , "%.01f X + %.01f",GetInstance_TestParams()[stcTestData.TestId].Corr_A, GetInstance_TestParams()[stcTestData.TestId].Corr_B);
	textParamsAx_B.setText(arrBuffer);

	return WinStatus;
}

void HandlerTestParamsViewcreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbTestParamsViewBack(void *ptr)
{
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
void HandlerbTestParamsViewPrint(void *ptr)
{
	BeepBuzzer();
	PrintTestParams(stcTestData.TestId);
}


