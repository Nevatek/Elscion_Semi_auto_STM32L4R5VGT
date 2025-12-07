/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../APPL/Inc/NonVolatileMemory.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
//static NexButton bDeleteButton = NexButton(en_WinId_TestParamsGeneral , 7 , "b6");
static NexButton bBackButton = NexButton(en_WinId_TestParamsGeneral , 7 , "b5");
static NexButton bButtonParams = NexButton(en_WinId_TestParamsGeneral , 8 , "b7");

static NexButton bButtonGeneral = NexButton(en_WinId_TestParamsGeneral , 6 , "b0");
static NexButton bButtonSettings = NexButton(en_WinId_TestParamsGeneral , 5 , "b1");
static NexButton bButtonLimits = NexButton(en_WinId_TestParamsGeneral , 4 , "b2");
static NexButton bButtonCalib = NexButton(en_WinId_TestParamsGeneral , 3 , "b3");
static NexButton bButtonStandards = NexButton(en_WinId_TestParamsGeneral , 2 , "b4");

static NexButton bButtonReagnetBlank = NexButton(en_WinId_TestParamsGeneral , 15 , "b8");
static NexButton bButtonSampleBlank = NexButton(en_WinId_TestParamsGeneral , 16 , "b9");
static NexButton bTxtButtonTestName = NexButton(en_WinId_TestParamsGeneral , 12 , "t3");
static NexButton bTxtButtonFullTestName = NexButton(en_WinId_TestParamsGeneral , 25 , "t10");
static NexButton bCbButtonUnit = NexButton(en_WinId_TestParamsGeneral , 17 , "cb0");

static NexText tTestId = NexText(en_WinId_TestParamsGeneral , 18 , "t6");

static NexText tUnitConstant = NexText(en_WinId_TestParamsGeneral , 11 , "t2");
static NexText tCustomUnitBox = NexText(en_WinId_TestParamsGeneral , 19 , "t7");
static NexButton bCustomUnitEnable = NexButton(en_WinId_TestParamsGeneral , 21 , "b6");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={/*&bDeleteButton,*/
										&bBackButton,
										&bButtonParams,
										&bButtonGeneral,
										&bButtonSettings,
										&bButtonLimits,
										&bButtonCalib,
										&bButtonStandards,
										&bButtonReagnetBlank,
										&bButtonSampleBlank,
										&bTxtButtonTestName,
										&bTxtButtonFullTestName,
										&bCbButtonUnit,
										&tCustomUnitBox,
										&bCustomUnitEnable,
										NULL};


//static void HandlerButtonDelete(void *ptr);
static void HandlerButtonback(void *ptr);
static void HandlerButtonParams(void *ptr);

static void HandlerbButtonGeneral(void *ptr);
static void HandlerbButtonSettings(void *ptr);
static void HandlerbButtonLimits(void *ptr);
static void HandlerbButtonCalib(void *ptr);
static void HandlerbButtonStandards(void *ptr);

static void HandlerbButtonReagnetBlank(void *ptr);
static void HandlerbButtonSampleBlank(void *ptr);
static void HandlerbTxtButtonTestName(void *ptr);
static void HandlerbTxtButtonFullTestName(void *ptr);
static void HandlerbCbButtonUnit(void *ptr);
static void HandlerbCustomUnitBox(void *ptr);
static void HandlerbCustomUnitEnable(void *ptr);
static bool TestParamsgeneralUpdateDelay(uint32_t Delay);


enWindowStatus ShowTestParamsGeneral(NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBackButton.attachPush(HandlerButtonback, &bBackButton);
	bButtonParams.attachPush(HandlerButtonParams, &bButtonParams);

	bButtonGeneral.attachPush(HandlerbButtonGeneral, &bButtonGeneral);
	bButtonSettings.attachPush(HandlerbButtonSettings, &bButtonSettings);
	bButtonLimits.attachPush(HandlerbButtonLimits, &bButtonLimits);
	bButtonCalib.attachPush(HandlerbButtonCalib, &bButtonCalib);
	bButtonStandards.attachPush(HandlerbButtonStandards, &bButtonStandards);

	bButtonReagnetBlank.attachPush(HandlerbButtonReagnetBlank, &bButtonReagnetBlank);
	bButtonSampleBlank.attachPush(HandlerbButtonSampleBlank, &bButtonSampleBlank);
	bTxtButtonTestName.attachPush(HandlerbTxtButtonTestName, &bTxtButtonTestName);
	bTxtButtonFullTestName.attachPush(HandlerbTxtButtonFullTestName, &bTxtButtonFullTestName);
	bCbButtonUnit.attachPush(HandlerbCbButtonUnit, &bCbButtonUnit);

	tCustomUnitBox.attachPush(HandlerbCustomUnitBox, &tCustomUnitBox);
	bCustomUnitEnable.attachPush(HandlerbCustomUnitEnable, &bCustomUnitEnable);

	tTestId.setText(std::to_string(stcTestData.TestId).c_str());
	bTxtButtonTestName.setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);
	bTxtButtonFullTestName.setText(GetInstance_TestParams()[stcTestData.TestId].arrFullTestName);
	bCbButtonUnit.setValue(GetInstance_TestParams()[stcTestData.TestId].Unit);
	tCustomUnitBox.setText(GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);

	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		bButtonSampleBlank.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	else
	{
		bButtonSampleBlank.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}

	if(true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
	{
		bButtonReagnetBlank.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	else
	{
		bButtonReagnetBlank.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}

	if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
	{
		tUnitConstant.setText("Custom unit");
		tCustomUnitBox.setVisible(true);
		bCbButtonUnit.setVisible(false);
		bCustomUnitEnable.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	else
	{
		tUnitConstant.setText("Unit");
		tCustomUnitBox.setVisible(false);
		bCbButtonUnit.setVisible(true);
		bCustomUnitEnable.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}

	if(stcScreenNavigation.PrevWindowId == en_WinID_TestSelectionScreen)
	{
		memcpy(&LastSavedTestParams , &stcTestparams[stcTestData.TestId] , sizeof(stcTestparams[0]));
	}

	return WinStatus;
}

void HandlerTestParamsGeneral (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
	if(TestParamsgeneralUpdateDelay(700))
	{
		uint32_t val = 0;
		if(true == bCbButtonUnit.getValue((uint32_t*)&val))
		{
			if(val != GetInstance_TestParams()[stcTestData.TestId].Unit)
			{
				GetInstance_TestParams()[stcTestData.TestId].Unit = (enUnits)val;
				BeepBuzzer();
			}
		}
	}
}

bool TestParamsgeneralUpdateDelay(uint32_t Delay)
{
//  #define DELAY_MS (500)
//  unsigned long currentMillis = HAL_GetTick();
//  static unsigned long PrevMills = 0;/*Initilized one time only*/
//  int elapsedTime = (int)(currentMillis - PrevMills);
//  if(abs((int)elapsedTime) >= DELAY_MS)
//  {
//   PrevMills = currentMillis;
//   return true;
//  }
//  return false;
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

void HandlerbCustomUnitBox(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit , sizeof(char*) ,false ,
			"Custom unit" , " " , (MAX_CUSTOM_UNITS_LENGTH - 1));
	BeepBuzzer();
}
void HandlerbCustomUnitEnable(void *ptr)
{
	if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
	{
		tUnitConstant.setText("Unit");
		GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable = false;
		tCustomUnitBox.setVisible(false);
		bCbButtonUnit.setVisible(true);
		bCustomUnitEnable.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else
	{
		tUnitConstant.setText("Custom Unit");
		GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable = true;
		tCustomUnitBox.setVisible(true);
		bCbButtonUnit.setVisible(false);
		bCustomUnitEnable.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
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

void HandlerButtonParams(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamsView , en_WinId_TestParamsGeneral);
	BeepBuzzer();
}
static void HandlerbButtonGeneral(void *ptr)
{
	return;
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
	ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
	BeepBuzzer();
}

static void HandlerbButtonReagnetBlank(void *ptr)
{
	if(true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
	{
		GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable = false;
		GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs = 0;
		GetInstance_TestParams()[stcTestData.TestId].ReagentAbs = 0;
		bButtonReagnetBlank.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else//en_ReagentBlank
	{
		GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable = true;
		bButtonReagnetBlank.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	BeepBuzzer();
}
static void HandlerbButtonSampleBlank(void *ptr)
{
	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
	{
		GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable = false;
		bButtonSampleBlank.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else//en_ReagentBlank
	{
		GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable = true;
		bButtonSampleBlank.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	BeepBuzzer();
}
static void HandlerbTxtButtonTestName(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , GetInstance_TestParams()[stcTestData.TestId].arrTestName , sizeof(char*) ,false ,
			"Test Name" , "NoTest" , MAX_TEST_NAME_CHAR_LENGTH);
	BeepBuzzer();
}
static void HandlerbTxtButtonFullTestName(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , GetInstance_TestParams()[stcTestData.TestId].arrFullTestName , sizeof(char*) ,false ,
			"Full Test Name" , "" , MAX_FULL_TEST_NAME_CHAR_LENGTH);
	BeepBuzzer();
}
static void HandlerbCbButtonUnit(void *ptr)
{
//	uint32_t val = 0;
//	bCbButtonUnit.getValue((uint32_t*)&val);
////	if(val != GetInstance_TestParams()[stcTestData.TestId].Unit)
//	{
//		GetInstance_TestParams()[stcTestData.TestId].Unit = (enUnits)val;
//		BeepBuzzer();
//	}
}
