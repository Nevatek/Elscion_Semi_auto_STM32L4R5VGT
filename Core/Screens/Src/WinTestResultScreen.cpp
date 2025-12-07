/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "./../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "../../APPL/Inc/NVM_TestResultsHistory.hpp"
#include "Printer.h"
#include <math.h>

extern uint8_t g_u8IsNewResult;

extern bool HyperActiveSampleflag;
extern bool e_linearity_limit_flag;
static char arr_DateTime[64] = {0};
/*Below is IDs for test abort POP UP */
static NexButton bTestResultPrint = NexButton(en_WinId_TestResultScreen , 4 , "b1");
static NexButton bTestResultNextSample = NexButton(en_WinId_TestResultScreen , 3 , "b0");
static NexButton bTestResultHome = NexButton(en_WinId_TestResultScreen , 15 , "b2");

static NexText textABSTitle = NexText(en_WinId_TestResultScreen , 11 , "t13");
static NexText textAbsorbance = NexText(en_WinId_TestResultScreen , 14 , "t15");

static NexPicture pTestTypePic = NexPicture(en_WinId_TestResultScreen , 5 , "p4");
static NexText textTestName = NexText(en_WinId_TestResultScreen , 6 , "t1");
static NexText textResult = NexText(en_WinId_TestResultScreen , 7 , "t2");
static NexText textResult2 = NexText(en_WinId_TestResultScreen , 23 , "t5");
static NexText textUnit = NexText(en_WinId_TestResultScreen , 24 , "t3");
static NexText textNormalRange = NexText(en_WinId_TestResultScreen , 8 , "t4");
static NexText textRangeStatus = NexText(en_WinId_TestResultScreen , 9 , "t10");
static NexText textPatientId = NexText(en_WinId_TestResultScreen , 13 , "t12");
static NexText textHyperActSamp = NexText(en_WinId_TestResultScreen , 25 , "t6");
static NexPicture pInRangeOutRange = NexPicture(en_WinId_TestResultScreen , 1 , "p8");

/*Below is IDs for test abort POP UP  - END*/

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bTestResultPrint,
									&bTestResultNextSample,
									&bTestResultHome,
									NULL};

static void HandlerbTestResultPrint(void *ptr);
static void HandlerbTestResultNextSample(void *ptr);
static void HandlerbTestResultHome(void *ptr);
void TestResutlInstantPrint(void);

enWindowStatus ShowTestResultScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

	{
		char arr_CBuffer[64] = {0};
		float TestResult = stcTestData.Result;
		float RefRangeLow = GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[0];
		float RefRangeHigh = GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[1];
		/*Showing test range status*/
		if(DEFAULT_REF_LOW != RefRangeLow || DEFAULT_REF_HIGH != RefRangeHigh)
		{
			if(RefRangeLow <= TestResult && RefRangeHigh >= TestResult)
			{
				pInRangeOutRange.Set_background_image_pic(524);
				strncpy(arr_CBuffer , "WITHIN REFERENCE RANGE" , 31);
				textRangeStatus.Set_font_color_pco(COLOUR_GREEN);
			}
			else
			{
				pInRangeOutRange.Set_background_image_pic(523);
				strncpy(arr_CBuffer , "OUT OF REFERENCE RANGE" , 31);
				textRangeStatus.Set_font_color_pco(COLOUR_RED);
			}
			textRangeStatus.setText(arr_CBuffer);
		}
		else
		{
			pInRangeOutRange.Set_background_image_pic(530);
			strncpy(arr_CBuffer , " " , 31);
			textRangeStatus.Set_font_color_pco(COLOUR_BLACK);
			textRangeStatus.setText(arr_CBuffer);
		}

		float LinRangeLow = GetInstance_TestParams()[stcTestData.TestId].LinearityRange[0];
		float LinRangeHigh = GetInstance_TestParams()[stcTestData.TestId].LinearityRange[1];

		if(HyperActiveSampleflag == true || e_linearity_limit_flag ==  true)
		{
			if(DEFAULT_LINEARITY_LOW != LinRangeLow || DEFAULT_LINEARITY_HIGH != LinRangeHigh)
			{
				float LinRangeavg = (LinRangeLow + LinRangeHigh) / 2;
				uint8_t DilutionFactor = (TestResult / LinRangeavg) ;
				uint8_t roundedDilutionFactor = 0;

				if(DilutionFactor < 2)
				{
					snprintf(arr_CBuffer , 63 , "Hyper active sample. Dilute and re-run");
				}
				else
				{
				    // Map to the closest valid dilution factor 2,5,10,20,50
				    if (DilutionFactor >= 38)
				    {
				        roundedDilutionFactor = 50;
				    }
				    else if (DilutionFactor >= 15)
				    {
				        roundedDilutionFactor = 20;
				    }
				    else if (DilutionFactor >= 7)
				    {
				        roundedDilutionFactor = 10;
				    }
				    else if (DilutionFactor >= 3)
				    {
				        roundedDilutionFactor = 5;
				    }
				    else if (DilutionFactor >= 2)
				    {
				        roundedDilutionFactor = 2;
				    }
					snprintf(arr_CBuffer , 63 , "Hyper active sample. Dilute 1 : %d and re-run" , roundedDilutionFactor);
				}
				textHyperActSamp.setVisible(true);
				textHyperActSamp.Set_font_color_pco(COLOUR_RED);
				textHyperActSamp.setText(arr_CBuffer);
			}
			else
			{
				textHyperActSamp.setVisible(true);
				textHyperActSamp.Set_font_color_pco(COLOUR_RED);
				textHyperActSamp.setText("Hyper active sample. Dilute and re-run");
			}
		}
		/*Showing test result*/
		int wholeNumber = (int)TestResult;
		float fractionalPart = TestResult - wholeNumber;
		int fractionalDigits = (int)(fractionalPart * 100);
		if (fractionalDigits < 0)
		{
		    fractionalDigits = (-1)*fractionalDigits;
		}
		if(TestResult < 0 && TestResult > (-1))
		{
			snprintf(arr_CBuffer , 31 , "-%d" , wholeNumber);
			textResult.setText(arr_CBuffer);
		}
		else
		{
			snprintf(arr_CBuffer , 31 , "%d" , wholeNumber);
			textResult.setText(arr_CBuffer);
		}
		snprintf(arr_CBuffer , 31 , "%02d" , fractionalDigits);
		textResult2.setText(arr_CBuffer);
		/*Showing test normal range*/
		snprintf(arr_CBuffer , 31 , "%.01f to %.01f" , RefRangeLow , RefRangeHigh);
		textNormalRange.setText(arr_CBuffer);
		/*Showing test unit*/
		if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable){
			snprintf(arr_CBuffer , MAX_CUSTOM_UNITS_LENGTH , "%s" ,GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
		}
		else
		{
			enUnits ResultUnit = GetInstance_TestParams()[stcTestData.TestId].Unit;
			snprintf(arr_CBuffer , 31 , "%s" ,&arr_cUnitBuffer[ResultUnit][0]);
		}
		textUnit.setText(arr_CBuffer);

		/*Showing patient ID*/
		strncpy(arr_CBuffer , stcTestData.arrPatientIDBuffer, 31);
		textPatientId.setText(arr_CBuffer);

		/*Showing result absorbance (OD)*/

		if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics || GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_FixedTime)
		{
			float ResultAbs = stcTestData.SampleOpticalDensity;
			snprintf(arr_CBuffer , 31 , "dAbs.      : %.03f" , ResultAbs);
			textABSTitle.setText(arr_CBuffer);
			snprintf(arr_CBuffer , 31 , "Init. Abs. : %.03f" , stcTestData.PrimaryAbsBuffer[(GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec)]);
			textAbsorbance.setText(arr_CBuffer);
		}
		else
		{
			textABSTitle.setText("Abs.");
			float ResultAbs = stcTestData.SampleOpticalDensity;
			snprintf(arr_CBuffer , 31 , "%.03f" , ResultAbs);
			textAbsorbance.setText(arr_CBuffer);
		}
		/*Showing test Name*/
		textTestName.setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);
		/*Showing test Type Icon*/
		switch(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType)
		{
			case en_Endpoint:
				pTestTypePic.Set_background_image_pic(IMAGE_ID_ENDPOINT);
				break;
			case en_FixedTime:
				pTestTypePic.Set_background_image_pic(IMAGE_ID_FIXEDPOINT);
				break;
			case en_Kinetics:
				pTestTypePic.Set_background_image_pic(IMAGE_ID_KINETICS);
				break;
			case en_Turbidimetry:
				pTestTypePic.Set_background_image_pic(IMAGE_ID_TURBIDIMETRY);
				break;
			case en_CoagulationTest:
				pTestTypePic.Set_background_image_pic(IMAGE_ID_COAGULATION);
				break;
			case en_Absorbance:
				pTestTypePic.Set_background_image_pic(IMAGE_ID_ENDPOINT);
				break;
			default:
				pTestTypePic.Set_background_image_pic(DELAULT_NO_TEST_IMAGE_ID);
				break;
		}

	}
	/*Outputting result - ENDS*/

	bTestResultPrint.attachPush(HandlerbTestResultPrint, &bTestResultPrint);
	bTestResultNextSample.attachPush(HandlerbTestResultNextSample, &bTestResultNextSample);
	bTestResultHome.attachPush(HandlerbTestResultHome, &bTestResultHome);
	if(g_u8IsNewResult == true)
	{
		RTC_DateTypeDef m_Date;
		RTC_TimeTypeDef m_Time;
		GetCurrentDate(&m_Date);
		GetCurrentTime(&m_Time);

		snprintf(arr_DateTime , 63 , "Date & Time  : %02d/%02d/%02d %02d:%02d:%02d",
				m_Date.Date, m_Date.Month, m_Date.Year,	m_Time.Hours, m_Time.Minutes, m_Time.Seconds);

		TestResutlInstantPrint();
		g_u8IsNewResult = false;
	}
	return WinStatus;
}

void HandlerTestResultScreen(NexPage *ptr_obJCurrPage)
{
//	if(true == g_u8IsNewResult)
//	{
//		/*Save result*/
////		for(uint32_t ResCount = 0; ResCount < 500; ResCount++)
////		{
////			AddNewResultHistoryData(stcTestData.TestId , &stcTestData , stcTestData.arrUserName);
////		}
////		SaveTestResults();/*Save results to flash*/
//
//		AddNewResultHistoryData(stcTestData.TestId , &stcTestData , stcTestData.arrUserName);
//		g_u8IsNewResult = false;
//	}
	nexLoop(nex_Listen_List);
}

void HandlerbTestResultPrint(void *ptr)
{
	BeepBuzzer();
	TestResutlInstantPrint();
}
void HandlerbTestResultNextSample(void *ptr)
{
	HyperActiveSampleflag = false;
	e_linearity_limit_flag = false;
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
void HandlerbTestResultHome(void *ptr)
{
	BeepBuzzer();
	HyperActiveSampleflag = false;
	e_linearity_limit_flag = false;
	SelectFilter(en_FilterBlack);
	ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
}

void TestResutlInstantPrint()
{
	char arr_CBuffer[64] = {0};
		RTC_DateTypeDef m_Date;
		RTC_TimeTypeDef m_Time;
		GetCurrentDate(&m_Date);
		GetCurrentTime(&m_Time);

		Print_Header();
		strncpy(arr_CBuffer , "================================" , 63);
		PrintStringOnPrinter(&arr_CBuffer[0]);
		PrinterFeedLine(1);
		/*Showing patient ID*/
		snprintf(arr_CBuffer , 63 , "%s   : %s" , "Patient ID" , stcTestData.arrPatientIDBuffer);
		PrintStringOnPrinter(&arr_CBuffer[0]);

		/*Date & Time*/
		PrintStringOnPrinter(&arr_DateTime[0]);

		/*Showing Test name*/
		snprintf(arr_CBuffer , 63 , "%s    : %s" , "Test name" ,
				GetInstance_TestParams()[stcTestData.TestId].arrTestName);
		PrintStringOnPrinter(&arr_CBuffer[0]);

		/*Result abs*/

		if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics || GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_FixedTime)
		{
			snprintf(arr_CBuffer , 63 , "%s        : %.0004f" , "dAbs." , stcTestData.SampleOpticalDensity + stcTestData.SampleBlankAbs);
			PrintStringOnPrinter(&arr_CBuffer[0]);

			snprintf(arr_CBuffer , 63 , "%s : %.0004f" , "Initial Abs." , stcTestData.PrimaryAbsBuffer[(GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec)]);
			PrintStringOnPrinter(&arr_CBuffer[0]);
		}
		else
		{
			snprintf(arr_CBuffer , 63 , "%s  : %.0004f" , "Sample Abs." , stcTestData.SampleOpticalDensity + stcTestData.SampleBlankAbs);
			PrintStringOnPrinter(&arr_CBuffer[0]);
		}

		if(GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
		{
			snprintf(arr_CBuffer , 63 , "%s : %.0004f" , "S.Blank Abs." , stcTestData.SampleBlankAbs);
			PrintStringOnPrinter(&arr_CBuffer[0]);
		}

		/*Result*/
		snprintf(arr_CBuffer , 63 , "%s       : %.0004f " , "Result" , stcTestData.Result);
		if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
		{
			strncat(arr_CBuffer,
					GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit ,
					MAX_CUSTOM_UNITS_LENGTH);
		}
		else
		{
			enUnits ResultUnit = GetInstance_TestParams()[stcTestData.TestId].Unit;
			strncat(arr_CBuffer ,
					&arr_cUnitBuffer[ResultUnit][0] , 63);
		}
		PrintStringOnPrinter(&arr_CBuffer[0]);

		/*Showing test normal range*/
		float RefRangeLow = GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[0];
		float RefRangeHigh = GetInstance_TestParams()[stcTestData.TestId].RefrenceRange[1];

		if(DEFAULT_REF_LOW != RefRangeLow || DEFAULT_REF_HIGH != RefRangeHigh)
		{
			snprintf(arr_CBuffer , 63 , "Ref. Range   : %0.2f - %0.2f " , RefRangeLow , RefRangeHigh);
			PrintStringOnPrinter(&arr_CBuffer[0]);

			/*Showing test range status*/
			if(RefRangeLow <= stcTestData.Result &&
					RefRangeHigh >= stcTestData.Result)
			{
				snprintf(arr_CBuffer , 63 , "Status       : %s" , "Within Ref. range");
			}
			else
			{
				snprintf(arr_CBuffer , 63 , "Status       : %s" , "Out of Ref. range");
			}
			PrintStringOnPrinter(&arr_CBuffer[0]);
		}
		/*Feed*/
		strncpy(arr_CBuffer , "================================" , 63);
		PrintStringOnPrinter(&arr_CBuffer[0]);
		Print_Footer();
		PrinterFeedLine(4);
}

