/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "./../Screens/Inc/CommonDisplayFunctions.h"

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
//static NexButton bDeleteButton = NexButton(en_WinId_TestParamsLimit , 7 , "b6");
static NexButton bBackButton = NexButton(en_WinId_TestParamsLimit , 7 , "b5");
static NexButton bHomeButton = NexButton(en_WinId_TestParamsLimit , 8 , "b7");

static NexButton bButtonGeneral = NexButton(en_WinId_TestParamsLimit , 6 , "b0");
static NexButton bButtonSettings = NexButton(en_WinId_TestParamsLimit , 5 , "b1");
static NexButton bButtonLimits = NexButton(en_WinId_TestParamsLimit , 4 , "b2");
static NexButton bButtonCalib = NexButton(en_WinId_TestParamsLimit , 3 , "b3");
static NexButton bButtonStandards = NexButton(en_WinId_TestParamsLimit , 2 , "b4");

static NexText tReagentBlankLimitLow = NexText(en_WinId_TestParamsLimit , 10 , "t9");
static NexText tReagentBlankLimitHigh = NexText(en_WinId_TestParamsLimit , 12 , "t1");
static NexText tAbsLimitLow = NexText(en_WinId_TestParamsLimit , 14 , "t3");
static NexText tAbsLimitHigh = NexText(en_WinId_TestParamsLimit , 16 , "t5");
static NexText tRefRangeLow = NexText(en_WinId_TestParamsLimit , 18 , "t8");
static NexText tRefRangeHigh = NexText(en_WinId_TestParamsLimit , 20 , "t11");
static NexText tLinearityRangeLow = NexText(en_WinId_TestParamsLimit , 22 , "t13");
static NexText tLinearityRangeHigh = NexText(en_WinId_TestParamsLimit , 24 , "t15");
static NexText tMaxDeltaAbsPerMin = NexText(en_WinId_TestParamsLimit , 26 , "t17");
static NexText tMaxDeltaAbsPerMintxt = NexText(en_WinId_TestParamsLimit , 25 , "t16");

static NexText tTextTurbiAbs = NexText(en_WinId_TestParamsLimit, 29, "t32");
static NexText tTurbiAbs = NexText(en_WinId_TestParamsLimit, 30, "t33");


/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={/*&bDeleteButton,*/
										&bBackButton,
										&bHomeButton,
										&bButtonGeneral,
										&bButtonSettings,
										&bButtonLimits,
										&bButtonCalib,
										&bButtonStandards,
										&tReagentBlankLimitLow,
										&tReagentBlankLimitHigh,
										&tAbsLimitLow,
										&tAbsLimitHigh,
										&tRefRangeLow,
										&tRefRangeHigh,
										&tLinearityRangeLow,
										&tLinearityRangeHigh,
										&tMaxDeltaAbsPerMin,
										&tTurbiAbs,
										NULL};


//static void HandlerButtonDelete(void *ptr);
static void HandlerButtonback(void *ptr);
static void HandlerButtonHome(void *ptr);

static void HandlerbButtonGeneral(void *ptr);
static void HandlerbButtonSettings(void *ptr);
static void HandlerbButtonLimits(void *ptr);
static void HandlerbButtonCalib(void *ptr);
static void HandlerbButtonStandards(void *ptr);

static void HandlerbButtontReagentBlankLimitLow(void *ptr);
static void HandlerbButtontReagentBlankLimitHigh(void *ptr);
static void HandlerbButtontAbsLimitLow(void *ptr);
static void HandlerbButtontAbsLimitHigh(void *ptr);
static void HandlerbButtontRefRangeLow(void *ptr);
static void HandlerbButtontRefRangeHigh(void *ptr);
static void HandlerbButtontLinearityRangeLow(void *ptr);
static void HandlerbButtontLinearityRangeHigh(void *ptr);
static void HandlerbButtontMaxDeltaAbsPerMin(void *ptr);
static void HandlertTurbiAbs(void *ptr);
bool VerifyLimitOrder(void);



enWindowStatus ShowTestParamsLimits(NexPage *ptr_obJCurrPage)
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

	tReagentBlankLimitLow.attachPush(HandlerbButtontReagentBlankLimitLow, &tReagentBlankLimitLow);
	tReagentBlankLimitHigh.attachPush(HandlerbButtontReagentBlankLimitHigh, &tReagentBlankLimitHigh);
	tAbsLimitLow.attachPush(HandlerbButtontAbsLimitLow, &tAbsLimitLow);
	tAbsLimitHigh.attachPush(HandlerbButtontAbsLimitHigh, &tAbsLimitHigh);
	tRefRangeLow.attachPush(HandlerbButtontRefRangeLow, &tRefRangeLow);
	tRefRangeHigh.attachPush(HandlerbButtontRefRangeHigh, &bButtonStandards);
	tLinearityRangeLow.attachPush(HandlerbButtontLinearityRangeLow, &tLinearityRangeLow);
	tLinearityRangeHigh.attachPush(HandlerbButtontLinearityRangeHigh, &tLinearityRangeHigh);
	tMaxDeltaAbsPerMin.attachPush(HandlerbButtontMaxDeltaAbsPerMin, &tMaxDeltaAbsPerMin);
	tTurbiAbs.attachPush(HandlertTurbiAbs, &tTurbiAbs);

//	tReagentBlankLimitLow.setText(std::to_string(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeLow]).c_str());
//	tReagentBlankLimitHigh.setText(std::to_string(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeHigh]).c_str());
//	tAbsLimitLow.setText(std::to_string(GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeLow]).c_str());
//	tAbsLimitHigh.setText(std::to_string(GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeHigh]).c_str());
//	tRefRangeLow.setText(std::to_string(GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[en_RangeLow]).c_str());
//	tRefRangeHigh.setText(std::to_string(GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[en_RangeHigh]).c_str());
//	tLinearityRangeLow.setText(std::to_string(GetInstance_TestParams()[stcTestData.TestId].LinearityRange[en_RangeLow]).c_str());
//	tLinearityRangeHigh.setText(std::to_string(GetInstance_TestParams()[stcTestData.TestId].LinearityRange[en_RangeHigh]).c_str());
//	tMaxDeltaAbsPerMin.setText(std::to_string(GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin).c_str());

	char arr_Buffer[64] = {0};
	snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeLow]);
	tReagentBlankLimitLow.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeHigh]);
	tReagentBlankLimitHigh.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeLow]);
	tAbsLimitLow.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeHigh]);
	tAbsLimitHigh.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[en_RangeLow]);
	tRefRangeLow.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[en_RangeHigh]);
	tRefRangeHigh.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].LinearityRange[en_RangeLow]);
	tLinearityRangeLow.setText(arr_Buffer);

	snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].LinearityRange[en_RangeHigh]);
	tLinearityRangeHigh.setText(arr_Buffer);


	enTestProcedureType TestType = GetInstance_TestParams()[stcTestData.TestId].TestProcedureType;
	if(en_Kinetics == TestType)
	{
		tMaxDeltaAbsPerMintxt.setVisible(true);
		tMaxDeltaAbsPerMin.setVisible(true);

		snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin);
		tMaxDeltaAbsPerMin.setText(arr_Buffer);
	}
	else if(en_Turbidimetry == TestType)
	{
		tTextTurbiAbs.setVisible(true);
		tTurbiAbs.setVisible(true);

		snprintf(arr_Buffer , 61 , "%.4f" , GetInstance_TestParams()[stcTestData.TestId].TurbiAbsLimit);
		tTurbiAbs.setText(arr_Buffer);
	}
	else
	{
		tTextTurbiAbs.setVisible(false);
		tTurbiAbs.setVisible(false);
		GetInstance_TestParams()[stcTestData.TestId].TurbiAbsLimit = float(DEFAULT_TURBIABSLIMIT);

		tMaxDeltaAbsPerMintxt.setVisible(false);
		tMaxDeltaAbsPerMin.setVisible(false);
		GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin = float(0);
	}

	return WinStatus;
}

void HandlerTestParamsLimits (NexPage *ptr_obJCurrPage)
{

	if(en_WinID_NumericKeypad == stcScreenNavigation.PrevWindowId)
	{
		char arr_Buffer[64] = {0};
			/* 1. Reagent blank absorbance limit conditions starts here*/
			if(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeLow] < DEFAULT_ABS_LOW || GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeLow] > DEFAULT_ABS_HIGH)
			{
				float	MinAbsValue = DEFAULT_ABS_LOW;
				float	MaxAbsValue = DEFAULT_ABS_HIGH;
				snprintf(arr_Buffer , 61 , "Please enter R.Blank limit low ( %0.1f - %0.1f ) Abs" , MinAbsValue , MaxAbsValue);

				if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
				{
					GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeLow] = DEFAULT_REG_BLANK_LOW;
					ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
				}
			}

			if(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeHigh] < DEFAULT_ABS_LOW || GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeHigh] > DEFAULT_ABS_HIGH)
			{
				float MinAbsValue = DEFAULT_ABS_LOW;
				float MaxAbsValue = DEFAULT_ABS_HIGH;
				snprintf(arr_Buffer , 61 , "Please enter R.Blank limit high ( %0.1f - %0.1f ) Abs" , MinAbsValue , MaxAbsValue);

				if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
				{
						GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeHigh] = DEFAULT_REG_BLANK_HIGH;
						ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
				}
			}


			/* 1. Reagent blank absorbance limit conditions ends here*/


			/* 2. Measuring range absorbance limit conditions starts here*/

			if(GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeLow] < DEFAULT_ABS_LOW || GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeLow] > DEFAULT_ABS_HIGH)
			{
				float	MinAbsValue = DEFAULT_ABS_LOW;
				float	MaxAbsValue = DEFAULT_ABS_HIGH;
				snprintf(arr_Buffer , 61 , "Please enter Abs. limit low ( %0.1f - %0.1f ) Abs" , MinAbsValue , MaxAbsValue);

				if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
				{
					GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeLow] = DEFAULT_ABS_LOW;
					ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
				}
			}

			if(GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeHigh] < DEFAULT_ABS_LOW || GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeHigh] > DEFAULT_ABS_HIGH)
			{
				float MinAbsValue = DEFAULT_ABS_LOW;
				float MaxAbsValue = DEFAULT_ABS_HIGH;
				snprintf(arr_Buffer , 61 , "Please enter Absorbance limit high ( %0.1f - %0.1f ) Abs" , MinAbsValue , MaxAbsValue);

				if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
				{
					GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeHigh] = DEFAULT_ABS_HIGH;
					ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
				}
			}

			/* 2. Measuring range absorbance limit conditions ends here*/


			/* 3. DeltaA range absorbance limit conditions starts here*/
			if(GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin < DEFAULT_ABS_LOW || GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin  > DEFAULT_ABS_HIGH)
			{
				float MinAbsValue = DEFAULT_ABS_LOW;
				float MaxAbsValue = DEFAULT_ABS_HIGH;
				snprintf(arr_Buffer , 61 , "Please enter dAbs./min ( %0.1f - %0.1f ) Abs" , MinAbsValue , MaxAbsValue);

				if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
				{
					GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin = DEFAULT_ABS_LOW;
					ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
				}
			}
			/* 3. DeltaA range absorbance limit conditions ends here*/

			/* 4. Turbi absorbance limit conditions starts here*/
			if(GetInstance_TestParams()[stcTestData.TestId].TurbiAbsLimit < DEFAULT_ABS_LOW || GetInstance_TestParams()[stcTestData.TestId].TurbiAbsLimit  > DEFAULT_ABS_HIGH)
			{
				float MinAbsValue = DEFAULT_ABS_LOW;
				float MaxAbsValue = DEFAULT_ABS_HIGH;
				snprintf(arr_Buffer , 61 , "Please enter Turbi Abs ( %0.1f - %0.1f )" , MinAbsValue , MaxAbsValue);

				if(enkeyOk == ShowMainPopUp("Warning",arr_Buffer, true))
				{
					GetInstance_TestParams()[stcTestData.TestId].TurbiAbsLimit = float(DEFAULT_TURBIABSLIMIT);
					ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
				}
			}
			/*4. Turbi absorbance limit conditions ends here*/


		stcScreenNavigation.PrevWindowId = en_WinId_TestParamCalib;
	}

	nexLoop(nex_Listen_List);
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

static void HandlerbButtontReagentBlankLimitLow(void *ptr)
{

	char arr_Buffer[64] = {0};
	float	MinAbsValue = DEFAULT_ABS_LOW;
	float 	MaxAbsValue = DEFAULT_ABS_HIGH;
	snprintf(arr_Buffer , 61 , "Reagent blank limit low ( %0.1f - %0.1f ) Abs" , MinAbsValue , MaxAbsValue);


	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeLow] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeLow]) , true ,arr_Buffer , " " , 7);
	BeepBuzzer();
}
static void HandlerbButtontReagentBlankLimitHigh(void *ptr)
{
	char arr_Buffer[64] = {0};
	float	MinAbsValue = DEFAULT_ABS_LOW;
	float 	MaxAbsValue = DEFAULT_ABS_HIGH;
	snprintf(arr_Buffer , 61 , "Reagent blank limit high ( %0.1f - %0.1f ) Abs" , MinAbsValue , MaxAbsValue);

	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeHigh] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[en_RangeHigh]) , true ,arr_Buffer , " " , 7);
	BeepBuzzer();
}
static void HandlerbButtontAbsLimitLow(void *ptr)
{
	char arr_Buffer[64] = {0};
	float	MinAbsValue = DEFAULT_ABS_LOW;
	float 	MaxAbsValue = DEFAULT_ABS_HIGH;
	snprintf(arr_Buffer , 61 , "Absorbance limit low ( %0.1f - %0.1f ) Abs" , MinAbsValue , MaxAbsValue);

	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeLow] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeLow]) , true ,arr_Buffer , " " , 7);
	BeepBuzzer();
}
static void HandlerbButtontAbsLimitHigh(void *ptr)
{
	char arr_Buffer[64] = {0};
	float	MinAbsValue = DEFAULT_ABS_LOW;
	float 	MaxAbsValue = DEFAULT_ABS_HIGH;
	snprintf(arr_Buffer , 61 , "Absorbance limit high ( %0.1f - %0.1f ) Abs" , MinAbsValue , MaxAbsValue);

	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeHigh] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeHigh]) , true ,arr_Buffer , " " , 7);
	BeepBuzzer();
}
static void HandlerbButtontRefRangeLow(void *ptr)
{
	char arr_Buffer[64] = {0};
	if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
	{
		snprintf(arr_Buffer , 61 , "Refrence range low ( %s )" , GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
	}
	else
	{
		enUnits ResultUnit = GetInstance_TestParams()[stcTestData.TestId].Unit;
		snprintf(arr_Buffer , 61 ,"Refrence range low ( %s )" ,&arr_cUnitBuffer[ResultUnit][0]);
	}
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[en_RangeLow] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[en_RangeLow]) , true ,arr_Buffer , " " , 7);
	BeepBuzzer();
}
static void HandlerbButtontRefRangeHigh(void *ptr)
{
	char arr_Buffer[64] = {0};
	if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
	{
		snprintf(arr_Buffer , 61 , "Refrence range high ( %s )" , GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
	}
	else
	{
		enUnits ResultUnit = GetInstance_TestParams()[stcTestData.TestId].Unit;
		snprintf(arr_Buffer , 61 ,"Refrence range high ( %s )" ,&arr_cUnitBuffer[ResultUnit][0]);
	}
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[en_RangeHigh] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[en_RangeHigh]) , true ,arr_Buffer , " " , 7);
	BeepBuzzer();
}
static void HandlerbButtontLinearityRangeLow(void *ptr)
{
	char arr_Buffer[64] = {0};
	if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
	{
		snprintf(arr_Buffer , 61 , "Linearity range low ( %s )" , GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
	}
	else
	{
		enUnits ResultUnit = GetInstance_TestParams()[stcTestData.TestId].Unit;
		snprintf(arr_Buffer , 61 ,"Linearity range low ( %s )" ,&arr_cUnitBuffer[ResultUnit][0]);
	}
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].LinearityRange[en_RangeLow] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].LinearityRange[en_RangeLow]) , true ,arr_Buffer , " " , 7);
	BeepBuzzer();
}
static void HandlerbButtontLinearityRangeHigh(void *ptr)
{
	char arr_Buffer[64] = {0};
	if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
	{
		snprintf(arr_Buffer , 61 , "Linearity range high ( %s )" , GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
	}
	else
	{
		enUnits ResultUnit = GetInstance_TestParams()[stcTestData.TestId].Unit;
		snprintf(arr_Buffer , 61 ,"Linearity range high ( %s )" ,&arr_cUnitBuffer[ResultUnit][0]);
	}
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].LinearityRange[en_RangeHigh] ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].LinearityRange[en_RangeHigh]) , true ,arr_Buffer , " " , 7);
	BeepBuzzer();
}
static void HandlerbButtontMaxDeltaAbsPerMin(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin) , true ,"Max dAbs. per min" , " " , 7);
	BeepBuzzer();
}

void HandlertTurbiAbs(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &GetInstance_TestParams()[stcTestData.TestId].TurbiAbsLimit ,
			sizeof(GetInstance_TestParams()[stcTestData.TestId].TurbiAbsLimit) ,true , "Turbi Abs Limit (0 - 3.0)" , " " , 7);
	BeepBuzzer();
}

static void HandlerbButtonGeneral(void *ptr)
{
	bool LimitLowtoHighflag = VerifyLimitOrder();
	if(LimitLowtoHighflag == true)
	{
		ChangeWindowPage(en_WinId_TestParamsGeneral , (enWindowID)NULL);
	}
	BeepBuzzer();
}
static void HandlerbButtonSettings(void *ptr)
{
	bool LimitLowtoHighflag = VerifyLimitOrder();
	if(LimitLowtoHighflag == true)
	{
		ChangeWindowPage(en_WinId_TestParamSettings , (enWindowID)NULL);
	}
	BeepBuzzer();
}
static void HandlerbButtonLimits(void *ptr)
{
	return;
}
static void HandlerbButtonCalib(void *ptr)
{
	bool LimitLowtoHighflag = VerifyLimitOrder();
	if(LimitLowtoHighflag == true)
	{
		ChangeWindowPage(en_WinId_TestParamCalib , (enWindowID)NULL);
	}
	BeepBuzzer();
}
static void HandlerbButtonStandards(void *ptr)
{
	bool LimitLowtoHighflag = VerifyLimitOrder();
	if(LimitLowtoHighflag == true)
	{
		ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
	}
	BeepBuzzer();
}

bool VerifyLimitOrder(void)
{
	bool LimitOrderflag = false;

	   if (isInIncreasingOrder(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit, en_RangeMax) || ((GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[0] != 0 && GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[1] == 0)))
	    {
		   if(isInIncreasingOrder(GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit, en_RangeMax))
		   {
			   if(isInIncreasingOrder(GetInstance_TestParams()[stcTestData.TestId].RefrenceRange, en_RangeMax))
			  	 {
				   if(isInIncreasingOrder(GetInstance_TestParams()[stcTestData.TestId].LinearityRange, en_RangeMax))
				  	 {
						LimitOrderflag = true;
				  	 }
				   else
				   	  {
						if(enkeyOk == ShowMainPopUp("Warning","Please enter Linearity range from low to high", true))
							{
								ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
							}
							LimitOrderflag = false;
				   	  	  }
			  	  	  }
			   else
			   {
					if(enkeyOk == ShowMainPopUp("Warning","Please enter Ref range from low to high", true))
					{
						ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
					}
					LimitOrderflag = false;
			   }

		   }
		   else{
				if(enkeyOk == ShowMainPopUp("Warning","Please enter Abs limit from low to high", true))
				{
					ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
				}
				LimitOrderflag = false;
		   }
	    }
	  else
	    {
			if(enkeyOk == ShowMainPopUp("Warning","Please enter R.Blank limit from low to high", true))
			{
				ChangeWindowPage(en_WinId_TestParamsLimit , (enWindowID)NULL);
			}
			LimitOrderflag = false;
	    }
	   return LimitOrderflag;
	}


