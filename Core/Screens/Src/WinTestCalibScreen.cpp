/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "./../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/calibration.h"
#include "../APPL/Inc/NVM_TestParameters.hpp"
#include <math.h>
#include "Printer.h"

#define CALIB_GRAPH_PIXEL_MIN_X (85)
#define CALIB_GRAPH_PIXEL_MIN_Y (100)
#define CALIB_GRAPH_PIXEL_X_WIDTH (385)
#define CALIB_GRAPH_PIXEL_Y_HEIGHT (248)
#define CALIB_GRAPH_PIXEL_MAX_X (CALIB_GRAPH_PIXEL_MIN_X + CALIB_GRAPH_PIXEL_X_WIDTH)
#define CALIB_GRAPH_PIXEL_MAX_Y (CALIB_GRAPH_PIXEL_MIN_Y + CALIB_GRAPH_PIXEL_Y_HEIGHT)

#define PIC_GRAPH_CALIB (65)

extern float e_RunTimeStdOd[MAX_NUM_OF_STANDARDS];/*Buffer to store run time OD for each standard*/
extern float e_RunTimeStdBlankOd[MAX_NUM_OF_STANDARDS];/*Buffer to store run time OD for each standard blank*/
extern float e_Runtime4plA;
extern float e_Runtime4plB;
extern float e_Runtime4plC;
extern float e_Runtime4plD;
extern double e_Runtime_R2value_linear;
extern double e_Runtime_R2value_cubic;
extern double e_Runtime_R2value_4pl;
extern double e_Runtime_R2value_3pl;
//extern bool e_StdDoneflag;

/*Title Bar txt and Pictures Starts*/
static NexPicture pTestTypePic = NexPicture(en_WinId_TestCalibScreen , 4 , "p0");
static NexText textTestName = NexText(en_WinId_TestCalibScreen , 5 , "t0");
static NexText textTestWavelength = NexText(en_WinId_TestRun , 32 , "t13");
static NexText textDate = NexText(en_WinId_TestCalibScreen , 7 , "t6");
static NexText textTime = NexText(en_WinId_TestCalibScreen , 6 , "t5");
static NexText textTemperature = NexText(en_WinId_TestCalibScreen , 8 , "t7");
static NexText textUserName = NexText(en_WinId_TestCalibScreen , 10 , "t8");
static NexPicture pUserlogin = NexPicture(en_WinId_TestCalibScreen , 11 , "p6");
/*Incubator temp and image*/
static NexText textIncuTemp = NexText(en_WinId_TestCalibScreen , 29 , "t10");
static NexText textIncuTempC = NexText(en_WinId_TestCalibScreen , 30 , "t11");
static NexPicture pIncubator = NexPicture(en_WinId_TestCalibScreen , 28 , "p2");
/*Title Bar txt and Pictures Ends*/

/*Below is IDs for test abort POP UP */
static NexButton bCalibPrint = NexButton(en_WinId_TestCalibScreen , 26 , "b0");
static NexButton bCalibClose = NexButton(en_WinId_TestCalibScreen , 3 , "b5");
static NexButton bCalibAccept = NexButton(en_WinId_TestCalibScreen , 13 , "b23");
static NexButton bCalibReject = NexButton(en_WinId_TestCalibScreen , 14 , "b24");


static NexText textCalibGraphXMax = NexText(en_WinId_TestCalibScreen , 12 , "t66");
static NexText textCalibGraphYMax = NexText(en_WinId_TestCalibScreen , 17 , "t1");
static NexText textCalibGraphXMin = NexText(en_WinId_TestCalibScreen , 23 , "t3");
static NexText textCalibGraphYMin = NexText(en_WinId_TestCalibScreen , 22 , "t2");
static NexPicture pGraph_CalibScreen = NexPicture(en_WinId_TestCalibScreen , 25 , "p15");

/* show hide the calibration type detail and popup*/
static NexPicture PlsWaitMsg_CalibScreen = NexPicture(en_WinId_TestCalibScreen , 33 , "p4");

static NexPicture CalibTextBox = NexPicture(en_WinId_TestCalibScreen , 2 , "p3");
static NexText tCalibTypeTxt = NexText(en_WinId_TestCalibScreen , 24 , "t4");
static NexText textCalTypeHeader = NexText(en_WinId_TestCalibScreen , 31 , "t12");
static NexText tCalibFactorValueStatic = NexText(en_WinId_TestCalibScreen , 20 , "t68");
static NexText tCalibFactorValue = NexText(en_WinId_TestCalibScreen , 21 , "t69");
static NexText tCalibLinearEqStatic = NexText(en_WinId_TestCalibScreen , 18 , "t70");
static NexText tCalibLinearEqR2 = NexText(en_WinId_TestCalibScreen , 19 , "t71");

/*Below is IDs for test abort POP UP  - END*/



float g_CalculatedKFactor = float(0);

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bCalibClose,
									&bCalibAccept,
									&bCalibReject,
									&bCalibPrint,
									&pUserlogin,
									NULL};

static void ShowHideAcceptRejectButton(void);
static void HandlerbCalibPrint(void *ptr);
static void HandlerbCalibClose(void *ptr);
static void HandlerbCalibAccept(void *ptr);
static void HandlerbCalibReject(void *ptr);
static void ShowHideCalibGraphData(void);
static void HandlerUserlogin(void *ptr);
static void CalDataInstantPrint(void);
static void showHideCalTextDetails(bool status);

enWindowStatus ShowTestCalibScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

	if(objstcSettings.fTempControlOnOff[en_ChamperTemp] == true)
	{
		textIncuTemp.setVisible(true);
		textIncuTempC.setVisible(true);
		pIncubator.setVisible(true);
	}
	showHideCalTextDetails(false);
	UpdateStatusBar(&pTestTypePic , &textTestName , &textTestWavelength ,
			&textDate, &textTime, &textTemperature , &textIncuTemp, &textUserName);

	char ArrCalibTypeTxt[64] = {0};
	enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;
	snprintf(ArrCalibTypeTxt , 63 , "%s" , &g_CalibrationTypeNameBUffer[CalibType][0]);
	tCalibTypeTxt.setText(ArrCalibTypeTxt);

	tCalibFactorValueStatic.setVisible(false);
	tCalibFactorValue.setVisible(false);
	tCalibLinearEqStatic.setVisible(false);
	tCalibLinearEqR2.setVisible(false);
	pGraph_CalibScreen.setVisible(false);

	g_CalculatedKFactor = float(0);
	if(en_1PointLinear == CalibType)
	{
		/*Displaying new factor and old factor*/
		bool ComparisonStatus = CompareBuffer((float*)e_RunTimeStdOd , (float*)GetInstance_TestParams()[stcTestData.TestId].StandardOD ,
				GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
		if(false == ComparisonStatus)/*Show accept or reject buttons if data is different*/
		{
			g_CalculatedKFactor = CalculateFactor();/*Calculating factor*/
			tCalibLinearEqStatic.setVisible(true);
			tCalibLinearEqR2.setVisible(true);

			char arr_Buffer[64] = {0};
			snprintf(arr_Buffer , 63 , "Old Factor : %.01f" , GetInstance_TestParams()[stcTestData.TestId].KFactor );
			tCalibLinearEqR2.setText(arr_Buffer);

			arr_Buffer[64] = {0};
			snprintf(arr_Buffer , 63 , "New Factor : %.01f" , g_CalculatedKFactor);
			tCalibLinearEqStatic.setText(arr_Buffer);
		}
		else
		{
			tCalibLinearEqStatic.setVisible(true);
			char arr_Buffer[64] = {0};
			g_CalculatedKFactor = GetInstance_TestParams()[stcTestData.TestId].KFactor;
			snprintf(arr_Buffer , 63 , "Factor : %.01f" , GetInstance_TestParams()[stcTestData.TestId].KFactor );
			tCalibLinearEqStatic.setText(arr_Buffer);
		}
	}
	else if(en_Factor == CalibType)
	{
		g_CalculatedKFactor = GetInstance_TestParams()[stcTestData.TestId].KFactor ;/*Calculating factor*/
		tCalibLinearEqStatic.setVisible(true);

		char arr_Buffer[64] = {0};
		snprintf(arr_Buffer , 63 , "Factor : %.01f" , g_CalculatedKFactor);
		tCalibLinearEqStatic.setText(arr_Buffer);
	}
	else if(en_2PointLinear == CalibType)
	{

	}
	else if(en_LinearRegression == CalibType)
	{
//		if(e_Runtime_R2value_linear * 100 < 90)
//		{
//		e_Runtime_R2value_linear = e_Runtime_R2value_linear * 100;
//		tCalibLinearEqStatic.setVisible(true);
//		tCalibLinearEqR2.setVisible(true);
//
//		char ArrCalibTypeTxt[64] = {0};
//		snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.2f " ,e_Runtime_R2value_linear);
//		tCalibLinearEqStatic.setText(ArrCalibTypeTxt);
//		snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is out of range");
//		tCalibLinearEqR2.setText(ArrCalibTypeTxt);
//		}
	}
	else if(en_PointToPoint == CalibType)
	{

	}
	else if(en_4PL == CalibType)
	{
//		if(e_Runtime_R2value_4pl * 100 < 90)
//		{
//			e_Runtime_R2value_4pl = e_Runtime_R2value_4pl * 100;
//			tCalibLinearEqStatic.setVisible(true);
//			tCalibLinearEqR2.setVisible(true);
//
//			char ArrCalibTypeTxt[64] = {0};
//			snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.2f " ,e_Runtime_R2value_4pl);
//			tCalibLinearEqStatic.setText(ArrCalibTypeTxt);
//
//			snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is out of range");
//			tCalibLinearEqR2.setText(ArrCalibTypeTxt);
//		}
	}
	else if(en_CubicSpline == CalibType)
	{
//		if(e_Runtime_R2value_cubic * 100 < 90)
//		{
//
//		e_Runtime_R2value_cubic = e_Runtime_R2value_cubic * 100;
//		tCalibLinearEqStatic.setVisible(true);
//		tCalibLinearEqR2.setVisible(true);
//
//		char ArrCalibTypeTxt[64] = {0};
//		snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.2f " ,e_Runtime_R2value_cubic);
//		tCalibLinearEqStatic.setText(ArrCalibTypeTxt);
//		snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is out of range");
//		tCalibLinearEqR2.setText(ArrCalibTypeTxt);
//		}
	}
	bCalibClose.attachPush(HandlerbCalibClose, &bCalibClose);
	bCalibAccept.attachPush(HandlerbCalibAccept, &bCalibAccept);
	bCalibReject.attachPush(HandlerbCalibReject, &bCalibReject);
	bCalibPrint.attachPush(HandlerbCalibPrint, &bCalibPrint);
	pUserlogin.attachPush(HandlerUserlogin, &pUserlogin);

	ShowHideCalibGraphData();
	ShowHideAcceptRejectButton();
	PlsWaitMsg_CalibScreen.setVisible(false);
	showHideCalTextDetails(true);
	return WinStatus;
}

void HandlerbCalibPrint(void *ptr)
{
	CalDataInstantPrint();
	BeepBuzzer();
}

void HandlerTestCalibScreen(NexPage *ptr_obJCurrPage)
{
	if(StatusBarUpdateMillsDelay())
	{
		UpdateStatusBar(&pTestTypePic , &textTestName , &textTestWavelength ,
				&textDate, &textTime, &textTemperature , &textIncuTemp, &textUserName);
	}
	nexLoop(nex_Listen_List);
}

static void HandlerbCalibClose(void *ptr)
{
	ChangeWindowPage(en_WinId_TestRun , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}
static void HandlerbCalibAccept(void *ptr)
{
	BeepBuzzer();
	uint8_t NoOfCalTrace = 0;
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	if(MAX_NUM_OF_CAL_TRACE <= GetInstance_TestParams()[stcTestData.TestId].NumOfCalTraceSaved)
	{
		for(uint8_t CalTrace = 0 ; CalTrace < (MAX_NUM_OF_CAL_TRACE - 1); ++CalTrace)
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
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[(MAX_NUM_OF_CAL_TRACE - 1)].Datetime.u8Day = 0;
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[(MAX_NUM_OF_CAL_TRACE - 1)].Datetime.u8Month = 0;
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[(MAX_NUM_OF_CAL_TRACE - 1)].Datetime.u8Year = 0;
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[(MAX_NUM_OF_CAL_TRACE - 1)].Datetime.u8Hour = 0;
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[(MAX_NUM_OF_CAL_TRACE - 1)].Datetime.u8Min = 0;

		for(uint8_t nI = 0 ; nI < (MAX_NUM_OF_STANDARD_OD + 1) ; ++nI)
		{
			GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[(MAX_NUM_OF_CAL_TRACE - 1)].OD[nI] = 0;
		}
		NoOfCalTrace = MAX_NUM_OF_CAL_TRACE - 1;
	}
	else
	{
		NoOfCalTrace = GetInstance_TestParams()[stcTestData.TestId].NumOfCalTraceSaved;
	}

	/*If reagent blk is enabled we are saving R.Blank OD or OD = 0 (Water blank);*/

	if (GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable == true)
	{
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[NoOfCalTrace].OD[RBLK_WATER_OD] = GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
	}
	else
	{
		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[NoOfCalTrace].OD[RBLK_WATER_OD] = 0;
	}

	/*When accept of New OD - Copying current std OD saved to RUN time buffer of std OD*/
	for(uint8_t nI = 0 ; nI < GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved ; ++nI)
	{

		GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[NoOfCalTrace].OD[nI] =
				GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI] = e_RunTimeStdOd[nI];

		GetInstance_TestParams()[stcTestData.TestId].StandardODBlank[nI] = e_RunTimeStdBlankOd[nI];
	}
	GetCurrentDate(&sDate);
	GetCurrentTime(&sTime);
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[NoOfCalTrace].Datetime.u8Day = sDate.Date;
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[NoOfCalTrace].Datetime.u8Month = sDate.Month;
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[NoOfCalTrace].Datetime.u8Year = sDate.Year;
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[NoOfCalTrace].Datetime.u8Hour = sTime.Hours;
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[NoOfCalTrace].Datetime.u8Min = sTime.Minutes;
	GetInstance_TestParams()[stcTestData.TestId].stcCalTrace[NoOfCalTrace].NumOfOdSaved =
			GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

	if((MAX_NUM_OF_CAL_TRACE) <= GetInstance_TestParams()[stcTestData.TestId].NumOfCalTraceSaved)
	{
		GetInstance_TestParams()[stcTestData.TestId].NumOfCalTraceSaved = MAX_NUM_OF_CAL_TRACE;
	}
	else
	{
		GetInstance_TestParams()[stcTestData.TestId].NumOfCalTraceSaved++;
	}
		GetInstance_TestParams()[stcTestData.TestId].FourplA = e_Runtime4plA;
		GetInstance_TestParams()[stcTestData.TestId].FourplD = e_Runtime4plD;
		GetInstance_TestParams()[stcTestData.TestId].FourplC = e_Runtime4plC;
		GetInstance_TestParams()[stcTestData.TestId].FourplB = e_Runtime4plB;

	/*Save standard abs value*/
	GetInstance_TestParams()[stcTestData.TestId].KFactor = g_CalculatedKFactor;
	ShowHideAcceptRejectButton();
	ShowPleaseWaitPopup("Calibration",PLEASE_WAIT);
	HandlerSaveTestParams();

		enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Sample?", false);
		if(enKeyYes == Key)
		{
			if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
			{
				stcTestData.CurrentMeasType = en_SampleBlank;
			}
			else
			{
				stcTestData.CurrentMeasType = en_Sample;
			}
			ChangeWindowPage(en_WinId_TestRun , stcScreenNavigation.PrevWindowId);
		}
		else
		{
			if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
			{
				stcTestData.CurrentMeasType = en_StandardBlank;
				stcTestData.SelectedStandard = 0;/*S0*/
			}
			else
			{
				stcTestData.CurrentMeasType = en_Standard;
				stcTestData.SelectedStandard = 0;/*S0*/
			}
			ChangeWindowPage(en_WinId_TestRun , stcScreenNavigation.PrevWindowId);
		}
}
static void HandlerbCalibReject(void *ptr)
{
	/*When rejection - Copying current std OD saved to RUN time buffer of std OD*/
	for(uint8_t nI = 0 ; nI < GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved ; ++nI)
	{
		e_RunTimeStdOd[nI] = GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI];
		e_RunTimeStdBlankOd[nI] = GetInstance_TestParams()[stcTestData.TestId].StandardODBlank[nI];
	}
	g_CalculatedKFactor = GetInstance_TestParams()[stcTestData.TestId].KFactor;
	ShowHideAcceptRejectButton();
	ChangeWindowPage(en_WinId_TestRun , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}

void ShowHideCalibGraphData(void)
{
	uint8_t NumOfStdSaved = 0;
	float MaxXVal = 0;
	float MaxYVal = 0;
	float MinXVal = 0;
	float MinYVal = 0;

	float X1 = 0;
	float X2 = 0;
	float Y1 = 0;
	float Y2 = 0;
	float pixel_X = 0;
	float pixel_Y = 0;
	float R_Blank_OD = 0;
	uint16_t radius = 5;

	e_Runtime_R2value_linear = 0.0;
	e_Runtime_R2value_cubic = 0.0;
	e_Runtime_R2value_4pl = 0.0;
	e_Runtime_R2value_3pl = 0.0;

		/*Clear and show standard graph*/
		ClearGraph(en_WinId_TestCalibScreen ,
								CALIB_GRAPH_PIXEL_MIN_X ,
								CALIB_GRAPH_PIXEL_MIN_Y,
								CALIB_GRAPH_PIXEL_X_WIDTH,
								CALIB_GRAPH_PIXEL_Y_HEIGHT);
		DrawGraphRectangle(en_WinId_TestCalibScreen ,
									CALIB_GRAPH_PIXEL_MIN_X ,
									CALIB_GRAPH_PIXEL_MIN_Y,
									CALIB_GRAPH_PIXEL_MAX_X,
									CALIB_GRAPH_PIXEL_MAX_Y);
//		pGraph_CalibScreen.setPic(PIC_GRAPH_CALIB);
		/*Draw standard graph*/
		if(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable == true)
		{
			R_Blank_OD = GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
		}
		else
		{
			R_Blank_OD = 0;
		}

		enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;

		if(en_Factor == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
			if(0 <= g_CalculatedKFactor )
			{
				X1 = MinXVal = 0;
				X2 = MaxXVal = g_CalculatedKFactor * (3.0 - R_Blank_OD) ;

				Y1 = MinYVal = R_Blank_OD;
				Y2 = MaxYVal = 3.0 - R_Blank_OD;
			}
			else
			{
				X1 = MinXVal = 0;
				X2 = MaxXVal = g_CalculatedKFactor * (3.0 - R_Blank_OD) ;
				Y1 = MaxYVal = R_Blank_OD;
				Y2 = MinYVal = 3.0 - R_Blank_OD;
			}


			UpdateGraph(en_WinId_TestCalibScreen , X1/*X1*/,
					Y1 /*Y1*/,
					X2/*X2*/,
					Y2 /*Y2*/,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
					CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					COLOUR_BLUE);

			for(uint8_t nI = 0 ; nI < NumOfStdSaved ; nI++)
			{
				if(nI == 0)
				{
					X1 = 0;
					Y1 = R_Blank_OD;
				}
				else
				{
					X1 = (GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI - 1]);
					Y1 = (e_RunTimeStdOd[nI - 1]);
				}

		    if (isnan(Y1))
		    {
		        Y1 = CALIB_GRAPH_PIXEL_MAX_Y;
		    }
		    if (isnan(X1))
		    {
		    	X1 = CALIB_GRAPH_PIXEL_MIN_X;
		    }
			DrawGraphFilledCircle(en_WinId_TestCalibScreen , X1, Y1,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
					CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					radius, COLOUR_RED);
			}
			UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
					MaxXVal/*X max value*/,
					MaxYVal/*Y max value*/,
					&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
					MinXVal/*X max value*/,
					MinYVal/*Y max value*/);

		}

		else if( en_1PointLinear == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

			if(0 <= g_CalculatedKFactor)
			{
				X1 = MinXVal = 0;
				X2 = MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
				Y1 = MinYVal = R_Blank_OD;
				Y2 = MaxYVal = FindMaxValueFromList(e_RunTimeStdOd , NumOfStdSaved);
			}
			else
			{
				X1 = MinXVal = 0;
				X2 = MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
				Y1 = MaxYVal = R_Blank_OD;
				Y2 = MinYVal = FindMaxValueFromList(e_RunTimeStdOd , NumOfStdSaved);
			}

			UpdateGraph(en_WinId_TestCalibScreen , X1/*X1*/,
					Y1 /*Y1*/,
					X2/*X2*/,
					Y2 /*Y2*/,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
					CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					COLOUR_BLUE);

			for(uint8_t nI = 0 ; nI < NumOfStdSaved ; nI++)
			{
				if(nI == 0)
				{
					X1 = 0;
					Y1 = R_Blank_OD;
				}
				else
				{
					X1 = (GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI - 1]);
					Y1 = (e_RunTimeStdOd[nI - 1]);
				}

		    if (isnan(Y1))
		    {
		        Y1 = CALIB_GRAPH_PIXEL_MAX_Y;
		    }
		    if (isnan(X1))
		    {
		    	X1 = CALIB_GRAPH_PIXEL_MIN_X;
		    }
			DrawGraphFilledCircle(en_WinId_TestCalibScreen , X1, Y1,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
					CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					radius, COLOUR_RED);
			}
			UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
					MaxXVal/*X max value*/,
					MaxYVal/*Y max value*/,
					&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
					MinXVal/*X max value*/,
					MinYVal/*Y max value*/);

		}
		else if( en_2PointLinear == CalibType)
		{

			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

			MaxYVal = (FindMaxValueFromList(e_RunTimeStdOd , NumOfStdSaved));
			MinYVal = R_Blank_OD;
			pixel_Y = (MaxYVal - ((FindMinValueFromList(e_RunTimeStdOd , NumOfStdSaved)))) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = 0;

			for( float nI = MinYVal ; nI < MaxYVal ; nI= nI + pixel_Y)
			{
				X1 = CalibrateToRuntimeStdConc(stcTestData.TestId, (nI- pixel_Y));
				X2 = CalibrateToRuntimeStdConc(stcTestData.TestId, (nI));

				if (X1 < 0 || X2 < 0)
				{
					continue;
				}

				UpdateGraph(en_WinId_TestCalibScreen , X1/*X1*/,
										Y1 = (nI-pixel_Y) /*Y1*/,
										X2/*X2*/,
										Y2 = nI/*Y2*/,
										MinXVal/*X min value*/,
										MaxXVal/*X max value*/,
										MinYVal/*Y min value*/,
										MaxYVal/*X max value*/,
										CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
										CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
										CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
										CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
										COLOUR_BLUE);
	    	}

			for(uint8_t nI = 0 ; nI < NumOfStdSaved + 1 ; nI++)
			{
				if(nI == 0)
				{
					X1 = 0;
					Y1 = R_Blank_OD;
				}
				else
				{
					X1 = (GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI - 1]);
					Y1 = (e_RunTimeStdOd[nI - 1]);
				}

		    if (isnan(Y1))
		    {
		        Y1 = CALIB_GRAPH_PIXEL_MAX_Y;
		    }
		    if (isnan(X1))
		    {
		    	X1 = CALIB_GRAPH_PIXEL_MIN_X;
		    }
			DrawGraphFilledCircle(en_WinId_TestCalibScreen , X1, Y1,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
					CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					radius, COLOUR_RED);
			}
			UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
					MaxXVal/*X max value*/,
					MaxYVal/*Y max value*/,
					&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
					MinXVal/*X max value*/,
					MinYVal/*Y max value*/);

		}
		else if( en_LinearRegression == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
			MaxYVal = (FindMaxValueFromList(e_RunTimeStdOd , NumOfStdSaved));
			MinYVal = R_Blank_OD;
			pixel_Y = (MaxYVal - ((FindMinValueFromList(e_RunTimeStdOd , NumOfStdSaved)))) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = 0;

			for( float nI = MinYVal ; nI <= MaxYVal ; nI= nI + pixel_Y)
			{
				X1 = CalibrateToRuntimeStdConc(stcTestData.TestId, (nI- pixel_Y));
				X2 = CalibrateToRuntimeStdConc(stcTestData.TestId, (nI));

				if (X1 < 0 || X2 < 0)
				{
					continue;
				}

				UpdateGraph(en_WinId_TestCalibScreen , X1/*X1*/,
										Y1 = (nI-pixel_Y) /*Y1*/,
										X2/*X2*/,
										Y2 = nI/*Y2*/,
										MinXVal/*X min value*/,
										MaxXVal/*X max value*/,
										MinYVal/*Y min value*/,
										MaxYVal/*X max value*/,
										CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
										CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
										CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
										CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
										COLOUR_BLUE);
	    	}

			if( en_LinearRegression == CalibType)
			{
				if(e_Runtime_R2value_linear * 100 < 90)
				{
				e_Runtime_R2value_linear = e_Runtime_R2value_linear * 100;
				tCalibLinearEqStatic.setVisible(true);
				tCalibLinearEqR2.setVisible(true);

				char ArrCalibTypeTxt[64] = {0};
				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.01f " ,e_Runtime_R2value_linear);
				tCalibLinearEqStatic.setText(ArrCalibTypeTxt);

				ArrCalibTypeTxt[64] = {0};
				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is Out Of Range");
				tCalibLinearEqR2.setText(ArrCalibTypeTxt);
				}
			}

			for(uint8_t nI = 0 ; nI < NumOfStdSaved + 1 ; nI++)
			{
				if(nI == 0)
				{
					X1 = 0;
					Y1 = R_Blank_OD;
				}
				else
				{
					X1 = (GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI - 1]);
					Y1 = (e_RunTimeStdOd[nI - 1]);
				}

		    if (isnan(Y1))
		    {
		        Y1 = CALIB_GRAPH_PIXEL_MAX_Y;
		    }
		    if (isnan(X1))
		    {
		    	X1 = CALIB_GRAPH_PIXEL_MIN_X;
		    }
			DrawGraphFilledCircle(en_WinId_TestCalibScreen , X1, Y1,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
					CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					radius, COLOUR_RED);
			}
			UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
					MaxXVal/*X max value*/,
					MaxYVal/*Y max value*/,
					&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
					MinXVal/*X max value*/,
					MinYVal/*Y max value*/);

		}
		else if( en_PointToPoint == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
			MaxYVal = (FindMaxValueFromList(e_RunTimeStdOd , NumOfStdSaved));
			MinYVal = R_Blank_OD;
			pixel_Y = (MaxYVal - ((FindMinValueFromList(e_RunTimeStdOd , NumOfStdSaved)))) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = 0;

			for( float nI = MinYVal ; nI <= MaxYVal ; nI= nI + pixel_Y)
			{
				X1 = CalibrateToRuntimeStdConc(stcTestData.TestId, (nI- pixel_Y));
				X2 = CalibrateToRuntimeStdConc(stcTestData.TestId, (nI));

				if (X1 < 0 || X2 < 0)
				{
					continue;
				}

				UpdateGraph(en_WinId_TestCalibScreen , X1/*X1*/,
										Y1 = (nI-pixel_Y) /*Y1*/,
										X2/*X2*/,
										Y2 = nI/*Y2*/,
										MinXVal/*X min value*/,
										MaxXVal/*X max value*/,
										MinYVal/*Y min value*/,
										MaxYVal/*X max value*/,
										CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
										CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
										CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
										CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
										COLOUR_BLUE);
	    	}

			for(uint8_t nI = 0 ; nI < NumOfStdSaved + 1; nI++)
			{
				if(nI == 0)
				{
					X1 = 0;
					Y1 = R_Blank_OD;
				}
				else
				{
					X1 = (GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI - 1]);
					Y1 = (e_RunTimeStdOd[nI - 1]);
				}

		    if (isnan(Y1))
		    {
		        Y1 = CALIB_GRAPH_PIXEL_MAX_Y;
		    }
		    if (isnan(X1))
		    {
		    	X1 = CALIB_GRAPH_PIXEL_MIN_X;
		    }
			DrawGraphFilledCircle(en_WinId_TestCalibScreen , X1, Y1,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
					CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					radius, COLOUR_RED);
			}
			UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
					MaxXVal/*X max value*/,
					MaxYVal/*Y max value*/,
					&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
					MinXVal/*X max value*/,
					MinYVal/*Y max value*/);

		}
		else if( en_3PL == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
			MaxYVal = (FindMaxValueFromList(e_RunTimeStdOd , NumOfStdSaved));
			MinYVal = R_Blank_OD;
			pixel_Y = (MaxYVal - ((FindMinValueFromList(e_RunTimeStdOd , NumOfStdSaved)))) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = 0;

			for( float nI = MinYVal ; nI <= MaxYVal ; nI= nI + pixel_Y)
			{
				X1 = CalibrateToRuntimeStdConc(stcTestData.TestId, (nI- pixel_Y));
				X2 = CalibrateToRuntimeStdConc(stcTestData.TestId, (nI));

				if (X1 < 0 || X2 < 0)
				{
					continue;
				}

				UpdateGraph(en_WinId_TestCalibScreen , X1/*X1*/,
										Y1 = (nI-pixel_Y) /*Y1*/,
										X2/*X2*/,
										Y2 = nI/*Y2*/,
										MinXVal/*X min value*/,
										MaxXVal/*X max value*/,
										MinYVal/*Y min value*/,
										MaxYVal/*X max value*/,
										CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
										CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
										CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
										CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
										COLOUR_BLUE);
	    	}

			if( en_3PL == CalibType)
			{
				if(e_Runtime_R2value_3pl * 100 < 90)
				{
					e_Runtime_R2value_3pl = e_Runtime_R2value_3pl * 100;
					tCalibLinearEqStatic.setVisible(true);
					tCalibLinearEqR2.setVisible(true);

					char ArrCalibTypeTxt[64] = {0};
					snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.2f " ,e_Runtime_R2value_3pl);
					tCalibLinearEqStatic.setText(ArrCalibTypeTxt);

					ArrCalibTypeTxt[64] = {0};
					snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is lesser than 90 percentage");
					tCalibLinearEqR2.setText(ArrCalibTypeTxt);
				}
			}
			for(uint8_t nI = 0 ; nI < NumOfStdSaved + 1; nI++)
			{
				if(nI == 0)
				{
					X1 = 0;
					Y1 = R_Blank_OD;
				}
				else
				{
					X1 = (GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI - 1]);
					Y1 = (e_RunTimeStdOd[nI - 1]);
				}

		    if (isnan(Y1))
		    {
		        Y1 = CALIB_GRAPH_PIXEL_MAX_Y;
		    }
		    if (isnan(X1))
		    {
		    	X1 = CALIB_GRAPH_PIXEL_MIN_X;
		    }
			DrawGraphFilledCircle(en_WinId_TestCalibScreen , X1, Y1,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
					CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					radius, COLOUR_RED);
			}
			UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
					MaxXVal/*X max value*/,
					MaxYVal/*Y max value*/,
					&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
					MinXVal/*X max value*/,
					MinYVal/*Y max value*/);

		}
		else if(en_4PL == CalibType)
		{
			bool ComparisonStatus = CompareBuffer((float*)e_RunTimeStdOd , (float*)GetInstance_TestParams()[stcTestData.TestId].StandardOD ,
					GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);

			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

			if(false == ComparisonStatus)/*Calculate Initial params for 4PL*/
			{
				CalibrateRuntime_4PL_initial_params(stcTestData.TestId);

				if(e_Runtime_R2value_4pl * 100 < 90)
				{
					e_Runtime_R2value_4pl = e_Runtime_R2value_4pl * 100;
					tCalibLinearEqStatic.setVisible(true);
					tCalibLinearEqR2.setVisible(true);

					char ArrCalibTypeTxt[64] = {0};
					snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.01f " ,e_Runtime_R2value_4pl);
					tCalibLinearEqStatic.setText(ArrCalibTypeTxt);

					ArrCalibTypeTxt[64] = {0};
					snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is Out Of Range");
					tCalibLinearEqR2.setText(ArrCalibTypeTxt);
				}
			}

			MaxYVal = (FindMaxValueFromList(e_RunTimeStdOd , NumOfStdSaved)) - R_Blank_OD;
			MinYVal = (FindMinValueFromList(e_RunTimeStdOd , NumOfStdSaved)) - R_Blank_OD;;
			pixel_Y = (MaxYVal - MinYVal) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = FindMinValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);;


			for( float nI = MinYVal ; nI <= MaxYVal + 0.2 ; nI= nI + pixel_Y)
			{
				X1 = CalibrateToRuntimeStdConc(stcTestData.TestId, (nI- pixel_Y));
				X2 = CalibrateToRuntimeStdConc(stcTestData.TestId, (nI));

				if (X1 < 0 || X2 < 0)
				{
					continue;
				}

				UpdateGraph(en_WinId_TestCalibScreen , X1/*X1*/,
										Y1 = (nI-pixel_Y) /*Y1*/,
										X2/*X2*/,
										Y2 = nI/*Y2*/,
										MinXVal/*X min value*/,
										MaxXVal/*X max value*/,
										MinYVal/*Y min value*/,
										MaxYVal/*X max value*/,
										CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
										CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
										CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
										CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
										COLOUR_BLUE);
	    	}
			for(uint8_t nI = 0 ; nI < NumOfStdSaved; nI++)
			{
//				if(nI == 0)
//				{
//					X1 = 0;
//					Y1 = R_Blank_OD;
//				}
//				else
//				{
					X1 = (GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI]);
					Y1 = (e_RunTimeStdOd[nI] - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs);
//				}

		    if (isnan(Y1))
		    {
		        Y1 = CALIB_GRAPH_PIXEL_MAX_Y;
		    }
		    if (isnan(X1))
		    {
		    	X1 = CALIB_GRAPH_PIXEL_MIN_X;
		    }
			DrawGraphFilledCircle(en_WinId_TestCalibScreen , X1, Y1,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
					CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					radius, COLOUR_RED);
			}
			UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
					MaxXVal/*X max value*/,
					MaxYVal/*Y max value*/,
					&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
					MinXVal/*X max value*/,
					MinYVal/*Y max value*/);
		}

		else if(en_CubicSpline == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

			MaxYVal = (FindMaxValueFromList(e_RunTimeStdOd , NumOfStdSaved));
			MinYVal =  R_Blank_OD;
			pixel_Y = (MaxYVal - ((FindMinValueFromList(e_RunTimeStdOd , NumOfStdSaved)))) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = 0;

			for( float nI = pixel_Y ; nI < MaxYVal ; nI= nI + pixel_Y)
			{
				X1 = CalibrateToRuntimeStdConc(stcTestData.TestId , (nI- pixel_Y));
				X2 = CalibrateToRuntimeStdConc(stcTestData.TestId , (nI));

				if(en_CubicSpline == CalibType)
				{
					if (X1 < 0 || X2 < 0)
					{
						continue;
					}
				}

				UpdateGraph(en_WinId_TestCalibScreen , X1/*X1*/,
						Y1 = (nI-pixel_Y) /*Y1*/,
						X2/*X2*/,
						Y2 = nI/*Y2*/,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal/*X max value*/,
						CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
						CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						COLOUR_BLUE);
			}

			if( en_CubicSpline == CalibType)
			{
				if(e_Runtime_R2value_cubic * 100 < 90)
				{

				e_Runtime_R2value_cubic = e_Runtime_R2value_cubic * 100;
				tCalibLinearEqStatic.setVisible(true);
				tCalibLinearEqR2.setVisible(true);

				char ArrCalibTypeTxt[64] = {0};
				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.01f " ,e_Runtime_R2value_cubic);
				tCalibLinearEqStatic.setText(ArrCalibTypeTxt);

				ArrCalibTypeTxt[64] = {0};
				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is Out Of Range");
				tCalibLinearEqR2.setText(ArrCalibTypeTxt);
				}
			}
			for(uint8_t nI = 0 ; nI < NumOfStdSaved + 1; nI++)
			{
				if(nI == 0)
				{
					X1 = 0;
					Y1 = R_Blank_OD;
				}
				else
				{
					X1 = (GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI - 1]);
					Y1 = (e_RunTimeStdOd[nI - 1]);
				}

		    if (isnan(Y1))
		    {
		        Y1 = CALIB_GRAPH_PIXEL_MAX_Y;
		    }
		    if (isnan(X1))
		    {
		    	X1 = CALIB_GRAPH_PIXEL_MIN_X;
		    }
			DrawGraphFilledCircle(en_WinId_TestCalibScreen , X1, Y1,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					CALIB_GRAPH_PIXEL_MIN_X/*Graph X*/,
					CALIB_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					CALIB_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					CALIB_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					radius, COLOUR_RED);
			}
			UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
					MaxXVal/*X max value*/,
					MaxYVal/*Y max value*/,
					&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
					MinXVal/*X max value*/,
					MinYVal/*Y max value*/);

	}
}

 void ShowHideAcceptRejectButton(void)
 {
	bool ComparisonStatus = CompareBuffer((float*)e_RunTimeStdOd , (float*)GetInstance_TestParams()[stcTestData.TestId].StandardOD ,
			GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
	if(false == ComparisonStatus)/*Show accept or reject buttons if data is different*/
	{
		bCalibClose.setVisible(false);
		bCalibAccept.setVisible(true);
		bCalibReject.setVisible(true);
		CalDataInstantPrint();
	}
	else
	{
		bCalibClose.setVisible(true);
		bCalibAccept.setVisible(false);
		bCalibReject.setVisible(false);
	}
 }

 void showHideCalTextDetails(bool status)
 {
 	 if(status)
 	 {
 		 PlsWaitMsg_CalibScreen.setVisible(false);
 		 CalibTextBox.setVisible(true);
 		 tCalibTypeTxt.setVisible(true);
 		 textCalTypeHeader.setVisible(true);
 		 tCalibFactorValueStatic.setVisible(true);
 		 tCalibFactorValue.setVisible(true);
 		 tCalibLinearEqStatic.setVisible(true);
 		 tCalibLinearEqR2.setVisible(true);
 	 }
 	 else
 	 {
 		 CalibTextBox.setVisible(false);
 		 tCalibTypeTxt.setVisible(false);
 		 textCalTypeHeader.setVisible(false);
 		 tCalibFactorValueStatic.setVisible(false);
 		 tCalibFactorValue.setVisible(false);
 		 tCalibLinearEqStatic.setVisible(false);
 		 tCalibLinearEqR2.setVisible(false);
 		 PlsWaitMsg_CalibScreen.setVisible(true);
 	 }

 }

 static void HandlerUserlogin(void *ptr)
 {
 	ChangeWindowPage(en_WinId_UserLogin ,  en_WinId_TestCalibScreen);
 	BeepBuzzer();
 }
 void CalDataInstantPrint(void)
 {
	 	 char arr_CBuffer[64] = {0};
		/*Showing Test name*/
		PrinterFeedLine(1);
		snprintf(arr_CBuffer , 63 , "%s    : %s" , "Test name" ,
				GetInstance_TestParams()[stcTestData.TestId].arrTestName);
		PrintStringOnPrinter(&arr_CBuffer[0]);

		/*Showing Cal type*/
		snprintf(arr_CBuffer , 63 , "%s    : %s" , "Cal. Type" ,
									(&g_CalibrationTypeNameBUffer[GetInstance_TestParams()[stcTestData.TestId].Calib][0]));
		PrintStringOnPrinter(&arr_CBuffer[0]);

		if (GetInstance_TestParams()[stcTestData.TestId].Calib == en_Factor)
		{
		/*Showing Factor value*/
		snprintf(arr_CBuffer , 63 , "%s       : %.0004f" , "Factor" ,
									(GetInstance_TestParams()[stcTestData.TestId].KFactor));
		PrintStringOnPrinter(&arr_CBuffer[0]);
		}
		else
		{
		/*Showing Num of standards*/
		snprintf(arr_CBuffer , 63 , "%s   : %u" , "No of Std." , (unsigned int)(GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved));
		PrintStringOnPrinter(&arr_CBuffer[0]);

			if (GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
			{
				PrintStringOnPrinter((char*)"Blank Type   : Reagent Blank");
			}
			else
			{
				PrintStringOnPrinter((char*)"Blank Type   : DI Water");
			}
			uint8_t NumofStd = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

			for(uint8_t nI = 0 ;nI < NumofStd ; ++nI)
			{
				snprintf(arr_CBuffer , 63 , "S%d Conc.     : %0.0004f" , nI+1, (GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI]));
				PrintStringOnPrinter(&arr_CBuffer[0]);
				if(GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
				{
					snprintf(arr_CBuffer , 63 , "S%d Blank Abs : %0.0004f" , nI+1, (e_RunTimeStdBlankOd[nI]));
					PrintStringOnPrinter(&arr_CBuffer[0]);
				}
				snprintf(arr_CBuffer , 63 , "S%d Abs.      : %0.0004f" , nI+1, (e_RunTimeStdOd[nI]) + (e_RunTimeStdBlankOd[nI]));
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}

			if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_1PointLinear)
			{
				snprintf(arr_CBuffer , 63 ,     "Factor       : %0.0004f" , g_CalculatedKFactor);
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}
		}
		/*Feed*/
		strncpy(arr_CBuffer , "********************************" , 63);
		PrintStringOnPrinter(&arr_CBuffer[0]);
		PrinterFeedLine(4);
 }
