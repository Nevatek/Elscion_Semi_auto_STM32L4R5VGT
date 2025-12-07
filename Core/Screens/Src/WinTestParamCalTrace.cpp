/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"


//extern char g_CalibrationTypeNameBUffer[en_CalibrationMax][24];

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static uint8_t g_SelectedEntry = 0;

static NexButton bBackButton = NexButton(en_WinId_TestParamCalTrace , 1 , "b5");
static NexButton bSelectBar[MAX_NUM_OF_CAL_TRACE] = {   NexButton(en_WinId_TestParamCalTrace , 55 , "b0"),
														NexButton(en_WinId_TestParamCalTrace , 56 , "b1"),
														NexButton(en_WinId_TestParamCalTrace , 57 , "b2"),
														NexButton(en_WinId_TestParamCalTrace , 58 , "b3")};

static NexButton bRadiobutton[MAX_NUM_OF_CAL_TRACE] = {  NexButton(en_WinId_TestParamCalTrace , 51 , "b9"),
													NexButton(en_WinId_TestParamCalTrace , 52 , "b4"),
													NexButton(en_WinId_TestParamCalTrace , 53 , "b8"),
													NexButton(en_WinId_TestParamCalTrace , 54 , "b10")};

//static NexButton bButton4 = NexButton(en_WinId_TestParamCalTrace , 50 , "b4");
static NexButton bButtonDelete = NexButton(en_WinId_TestParamCalTrace , 43 , "b7");
static NexButton bButtonUse = NexButton(en_WinId_TestParamCalTrace , 42 , "b6");

static NexText tDate[MAX_NUM_OF_CAL_TRACE] = {	NexText(en_WinId_TestParamCalTrace , 11 , "t7"),
												NexText(en_WinId_TestParamCalTrace , 12 , "t9"),
												NexText(en_WinId_TestParamCalTrace , 14 , "t11"),
												NexText(en_WinId_TestParamCalTrace , 16 , "t13")};

static NexText tTime[MAX_NUM_OF_CAL_TRACE] = {	NexText(en_WinId_TestParamCalTrace , 10 , "t8"),
												NexText(en_WinId_TestParamCalTrace , 13 , "t10"),
												NexText(en_WinId_TestParamCalTrace , 15 , "t12"),
												NexText(en_WinId_TestParamCalTrace , 17 , "t14")};

static NexText tRBlank[MAX_NUM_OF_CAL_TRACE] = {NexText(en_WinId_TestParamCalTrace , 47 , "t48"),
												NexText(en_WinId_TestParamCalTrace , 48 , "t49"),
												NexText(en_WinId_TestParamCalTrace , 49 , "t50"),
												NexText(en_WinId_TestParamCalTrace , 50 , "t51")};

static NexText tS0[MAX_NUM_OF_CAL_TRACE] = {	NexText(en_WinId_TestParamCalTrace , 18 , "t17"),
												NexText(en_WinId_TestParamCalTrace , 19 , "t18"),
												NexText(en_WinId_TestParamCalTrace , 21 , "t20"),
												NexText(en_WinId_TestParamCalTrace , 20 , "t19")};

static NexText tS1[MAX_NUM_OF_CAL_TRACE] = {	NexText(en_WinId_TestParamCalTrace , 22 , "t22"),
												NexText(en_WinId_TestParamCalTrace , 23 , "t23"),
												NexText(en_WinId_TestParamCalTrace , 24 , "t24"),
												NexText(en_WinId_TestParamCalTrace , 25 , "t25")};

static NexText tS2[MAX_NUM_OF_CAL_TRACE] = {	NexText(en_WinId_TestParamCalTrace , 30 , "t32"),
												NexText(en_WinId_TestParamCalTrace , 31 , "t33"),
												NexText(en_WinId_TestParamCalTrace , 32 , "t34"),
												NexText(en_WinId_TestParamCalTrace , 33 , "t35")};

static NexText tS3[MAX_NUM_OF_CAL_TRACE] = {	NexText(en_WinId_TestParamCalTrace , 26 , "t27"),
												NexText(en_WinId_TestParamCalTrace , 27 , "t28"),
												NexText(en_WinId_TestParamCalTrace , 28 , "t29"),
												NexText(en_WinId_TestParamCalTrace , 29 , "t30")};

static NexText tS4[MAX_NUM_OF_CAL_TRACE] = {	NexText(en_WinId_TestParamCalTrace , 38 , "t42"),
												NexText(en_WinId_TestParamCalTrace , 39 , "t43"),
												NexText(en_WinId_TestParamCalTrace , 40 , "t44"),
												NexText(en_WinId_TestParamCalTrace , 41 , "t45")};

static NexText tS5[MAX_NUM_OF_CAL_TRACE] = {	NexText(en_WinId_TestParamCalTrace , 34 , "t37"),
												NexText(en_WinId_TestParamCalTrace , 35 , "t38"),
												NexText(en_WinId_TestParamCalTrace , 36 , "t39"),
												NexText(en_WinId_TestParamCalTrace , 37 , "t40")};

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBackButton,
									 &bSelectBar[0],
									 &bSelectBar[1],
									 &bSelectBar[2],
									 &bSelectBar[3],
									 &bRadiobutton[0],
									 &bRadiobutton[1],
									 &bRadiobutton[2],
									 &bRadiobutton[3],
									 &bButtonUse,
									 &bButtonDelete,
									 NULL};

static void HandlerBackButton(void *ptr);
static void HandlerButton0(void *ptr);
static void HandlerButton1(void *ptr);
static void HandlerButton2(void *ptr);
static void HandlerButton3(void *ptr);
//static void HandlerButton4(void *ptr);
static void HandlerButtonDelete(void *ptr);
static void HandlerButtonUse(void *ptr);
static void ShowCalTrace(void);
static void SelectButton(uint8_t Idx);

enWindowStatus ShowTestParamsCalTraceScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBackButton.attachPush(HandlerBackButton, &bBackButton);
	bSelectBar[0].attachPush(HandlerButton0, &bSelectBar[0]);
	bSelectBar[1].attachPush(HandlerButton1, &bSelectBar[1]);
	bSelectBar[2].attachPush(HandlerButton2, &bSelectBar[2]);
	bSelectBar[3].attachPush(HandlerButton3, &bSelectBar[3]);

	bButtonUse.attachPush(HandlerButtonUse, &bButtonUse);
	bButtonDelete.attachPush(HandlerButtonDelete, &bButtonDelete);
	ShowCalTrace();
	SelectButton(g_SelectedEntry = 0xFF);
	return WinStatus;
}
void HandlerTestParamCalTraceScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

static void HandlerButtonDelete(void *ptr)
{
	if(0xFF == g_SelectedEntry)
	{
		return;
	}
	BeepBuzzer();
	GetInstance_TestParams()[stcTestData.TestId].NumOfCalTraceSaved--;/*Reduce num of cal trace saved by one*/
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[g_SelectedEntry].Datetime.u8Day = 0;
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[g_SelectedEntry].Datetime.u8Month = 0;
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[g_SelectedEntry].Datetime.u8Year = 0;
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[g_SelectedEntry].Datetime.u8Hour = 0;
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[g_SelectedEntry].Datetime.u8Min = 0;
	for(uint8_t nI = 0 ; nI < MAX_NUM_OF_STANDARD_OD + 1 ; ++nI)
	{
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[g_SelectedEntry].OD[nI] = 0;
	}
	/*Rearrange entries*/
	for(uint8_t CalTrace = g_SelectedEntry ; CalTrace < (MAX_NUM_OF_CAL_TRACE - 1); ++CalTrace)
	{
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace].Datetime.u8Day = GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace+1].Datetime.u8Day;
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace].Datetime.u8Month = GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace+1].Datetime.u8Month;
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace].Datetime.u8Year = GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace+1].Datetime.u8Year;
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace].Datetime.u8Hour = GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace+1].Datetime.u8Hour;
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace].Datetime.u8Min = GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace+1].Datetime.u8Min;

		for(uint8_t nI = 0 ; nI < (MAX_NUM_OF_STANDARD_OD + 1) ; ++nI)
		{
			GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace].OD[nI] = GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[CalTrace + 1].OD[nI] ;
		}
	}
	ShowPleaseWaitPopup("Test Setup",PLEASE_WAIT);
	HandlerSaveTestParams();
	ShowCalTrace();
	SelectButton(g_SelectedEntry = 0xFF);/*Reset entry to staring*/
	ChangeWindowPage(en_WinId_TestParamCalTrace , (enWindowID)NULL);
}
static void HandlerButtonUse(void *ptr)
{
	if(0xFF == g_SelectedEntry)
	{
		return;
	}
	BeepBuzzer();
	uint8_t u8NumOfStd = GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[g_SelectedEntry].NumOfOdSaved;
	for(uint8_t nI = 0 ; nI < u8NumOfStd ; ++nI)
	{
		GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI] =
				GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[g_SelectedEntry].OD[nI];

	}
	GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved = u8NumOfStd;
	GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs = GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[g_SelectedEntry].OD[RBLK_WATER_OD];
	ShowPleaseWaitPopup("Test Setup",PLEASE_WAIT);
	HandlerSaveTestParams();
	ChangeWindowPage(en_WinId_TestParamStdGraph , (enWindowID)NULL);
}

void HandlerButton0(void *ptr)
{
	SelectButton(g_SelectedEntry = 0);
	BeepBuzzer();
}

void HandlerButton1(void *ptr)
{
	SelectButton(g_SelectedEntry = 1);
	BeepBuzzer();
}

void HandlerButton2(void *ptr)
{
	SelectButton(g_SelectedEntry = 2);
	BeepBuzzer();
}
void HandlerButton3(void *ptr)
{
	SelectButton(g_SelectedEntry = 3);
	BeepBuzzer();
}
void HandlerBackButton(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamStdGraph , (enWindowID)NULL);
	BeepBuzzer();
}
void ShowCalTrace(void)
{
	#define MAX_DATE_TIME_STRING_PRINTF 32
	#define MAX_OD_STRING_PRINTF 16
	uint8_t NoOfCalTrace = 0;
	if((MAX_NUM_OF_CAL_TRACE) <= GetInstance_TestParams()[stcTestData.TestId].NumOfCalTraceSaved)
	{
		NoOfCalTrace = MAX_NUM_OF_CAL_TRACE;
	}
	else
	{
		NoOfCalTrace = GetInstance_TestParams()[stcTestData.TestId].NumOfCalTraceSaved;
	}

	/*clear all data first*/

	for(uint8_t u8Idx = 0; u8Idx < MAX_NUM_OF_CAL_TRACE ; ++u8Idx)
	{
		tDate[u8Idx].setText(" ");
		tTime[u8Idx].setText(" ");
		tRBlank[u8Idx].setText(" ");/*Reagent Blank OD*/
		 bSelectBar[u8Idx].setVisible(false);
		 bRadiobutton[u8Idx].setVisible(false);

		for(uint8_t nI = 0 ; nI < GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[u8Idx].NumOfOdSaved ; ++nI)
		{
			switch(nI)
			{
				case 0 : tS0[u8Idx].setText(" ");break;/*std OD 1*/
				case 1 : tS1[u8Idx].setText(" ");break;/*std OD 2*/
				case 2 : tS2[u8Idx].setText(" ");break;/*std OD 3*/
				case 3 : tS3[u8Idx].setText(" ");break;/*std OD 4*/
				case 4 : tS4[u8Idx].setText(" ");break;/*std OD 5*/
				case 5 : tS5[u8Idx].setText(" ");break;/*std OD 6*/
			}
		}
	}

	for(uint8_t u8Idx = 0; u8Idx < NoOfCalTrace ; ++u8Idx)
	{
		char strDate[MAX_DATE_TIME_STRING_PRINTF] = {0};
		char strTime[MAX_DATE_TIME_STRING_PRINTF] = {0};
		char strStdOd[MAX_OD_STRING_PRINTF] = {0};
		snprintf(strDate,MAX_DATE_TIME_STRING_PRINTF,"%02d/%02d/%02d",
				GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[u8Idx].Datetime.u8Day,
				GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[u8Idx].Datetime.u8Month,
				GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[u8Idx].Datetime.u8Year);

		snprintf(strTime,MAX_DATE_TIME_STRING_PRINTF,"%02d:%02d",
				GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[u8Idx].Datetime.u8Hour,
				GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[u8Idx].Datetime.u8Min);

		tDate[u8Idx].setText(strDate);
		tTime[u8Idx].setText(strTime);
		bSelectBar[u8Idx].setVisible(true);
		bRadiobutton[u8Idx].setVisible(true);

			/*Displaying R Blank or Water blank OD*/
			snprintf(strStdOd , MAX_OD_STRING_PRINTF , "%.03f" ,
					GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[u8Idx].OD[RBLK_WATER_OD]);
			tRBlank[u8Idx].setText(strStdOd);/*R Blank OD*/

		for(uint8_t nI = 0 ; nI < GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[u8Idx].NumOfOdSaved ; ++nI)
		{
			snprintf(strStdOd , MAX_OD_STRING_PRINTF , "%.03f" ,
					GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[u8Idx].OD[nI]);
			switch(nI)
			{
				case 0 : tS0[u8Idx].setText(strStdOd);break;/*std OD 1*/
				case 1 : tS1[u8Idx].setText(strStdOd);break;/*std OD 2*/
				case 2 : tS2[u8Idx].setText(strStdOd);break;/*std OD 3*/
				case 3 : tS3[u8Idx].setText(strStdOd);break;/*std OD 4*/
				case 4 : tS4[u8Idx].setText(strStdOd);break;/*std OD 5*/
				case 5 : tS5[u8Idx].setText(strStdOd);break;/*std OD 6*/
			}

		}
	}
}
void SelectButton(uint8_t Idx)
{
	bRadiobutton[0].Set_background_image_pic(RADIO_BUTTON_OFF);
	bRadiobutton[1].Set_background_image_pic(RADIO_BUTTON_OFF);
	bRadiobutton[2].Set_background_image_pic(RADIO_BUTTON_OFF);
	bRadiobutton[3].Set_background_image_pic(RADIO_BUTTON_OFF);

	if(0 <= Idx && 3 >= Idx)
	{
		bButtonUse.setVisible(true);
		bButtonDelete.setVisible(true);
	}
	else
	{
		bButtonUse.setVisible(false);
		bButtonDelete.setVisible(false);
	}

	switch(Idx)
	{
		case 0:bRadiobutton[0].Set_background_image_pic(RADIO_BUTTON_ON);
		break;
		case 1:bRadiobutton[1].Set_background_image_pic(RADIO_BUTTON_ON);
		break;
		case 2:bRadiobutton[2].Set_background_image_pic(RADIO_BUTTON_ON);
		break;
		case 3:bRadiobutton[3].Set_background_image_pic(RADIO_BUTTON_ON);
		break;
		default:break;
	}

}
