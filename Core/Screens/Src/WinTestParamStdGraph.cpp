/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/calibration.h"
#include "../APPL/Inc/NVM_TestParameters.hpp"
#include <math.h>


#define SETT_GRAPH_PIXEL_MIN_X (85)
#define SETT_GRAPH_PIXEL_MIN_Y (100)
#define SETT_GRAPH_PIXEL_X_WIDTH (385)
#define SETT_GRAPH_PIXEL_Y_HEIGHT (248)
#define SETT_GRAPH_PIXEL_MAX_X (SETT_GRAPH_PIXEL_MIN_X + SETT_GRAPH_PIXEL_X_WIDTH)
#define SETT_GRAPH_PIXEL_MAX_Y (SETT_GRAPH_PIXEL_MIN_Y + SETT_GRAPH_PIXEL_Y_HEIGHT)

extern char g_CalibrationTypeNameBUffer[en_CalibrationMax][24];
#define PIC_GRAPH_PARAMS (65)
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBackButton = NexButton(en_WinId_TestParamStdGraph , 7 , "b5");
static NexButton bCaltraceButton = NexButton(en_WinId_TestParamStdGraph , 8 , "b2");
static NexText textCalibGraphXMax = NexText(en_WinId_TestParamStdGraph , 1 , "t4");
static NexText textCalibGraphYMax = NexText(en_WinId_TestParamStdGraph , 2 , "t0");
static NexText textCalibGraphXMin = NexText(en_WinId_TestParamStdGraph , 4 , "t2");
static NexText textCalibGraphYMin = NexText(en_WinId_TestParamStdGraph , 3 , "t1");

static NexPicture pGraph_StdParams = NexPicture(en_WinId_TestCalibScreen , 16 , "p3");

static NexPicture PlsWaitMsg_CalibScreen = NexPicture(en_WinId_TestCalibScreen , 17 , "p4");

static NexPicture CalibTextBox = NexPicture(en_WinId_TestCalibScreen , 9 , "p0");
static NexText textCaltypeStatic = NexText(en_WinId_TestParamStdGraph , 10 , "t5");
static NexText textCaltyp = NexText(en_WinId_TestParamStdGraph , 11 , "t7");
static NexText textEqStatic = NexText(en_WinId_TestParamStdGraph , 12 , "t8");
static NexText textEq = NexText(en_WinId_TestParamStdGraph , 13 , "t9");



/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBackButton,
									&bCaltraceButton,
									 NULL};
extern double e_Runtime_R2value_linear;
extern double e_Runtime_R2value_cubic;
extern double e_Runtime_R2value_4pl;
extern double e_Runtime_R2value_3pl;

static void HandlerBackButton(void *ptr);
static void HandlerCalTraceButton(void *ptr);
static void ShowSettingsCalibGraphData(void);
static void showHideCalTextDetails(bool status);

enWindowStatus ShowTestParamstdGraphScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	showHideCalTextDetails(false);
	bBackButton.attachPush(HandlerBackButton, &bBackButton);
	bCaltraceButton.attachPush(HandlerCalTraceButton , &bCaltraceButton);
	textEq.setVisible(false);
	textEqStatic.setVisible(false);
	pGraph_StdParams.setVisible(false);

	enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;
	char ArrCalibTypeTxt[64] = {0};
	snprintf(ArrCalibTypeTxt , 63 , "%s" , &g_CalibrationTypeNameBUffer[CalibType][0]);
	textCaltyp.setText(ArrCalibTypeTxt);

	ShowSettingsCalibGraphData();
	showHideCalTextDetails(true);
	return WinStatus;
}
void HandlerTestParamstdGraphScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerCalTraceButton(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamCalTrace , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerBackButton(void *ptr)
{
	ChangeWindowPage(en_WinId_TestParamStandards , (enWindowID)NULL);
	BeepBuzzer();
}

void showHideCalTextDetails(bool status)
{
	if(status)
	{
		PlsWaitMsg_CalibScreen.setVisible(false);

		CalibTextBox.setVisible(true);
		textCaltypeStatic.setVisible(true);
		textCaltyp.setVisible(true);
		textEqStatic.setVisible(true);
		textEq.setVisible(true);
	}
	else
	{
		CalibTextBox.setVisible(false);
		textCaltypeStatic.setVisible(false);
		textCaltyp.setVisible(false);
		textEqStatic.setVisible(false);
		textEq.setVisible(false);

		PlsWaitMsg_CalibScreen.setVisible(true);
	}

}

void ShowSettingsCalibGraphData(void)
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

		/*Clear and show standard graph*/
		ClearGraph(en_WinId_TestParamStdGraph ,
								SETT_GRAPH_PIXEL_MIN_X ,
								SETT_GRAPH_PIXEL_MIN_Y,
								SETT_GRAPH_PIXEL_X_WIDTH,
								SETT_GRAPH_PIXEL_Y_HEIGHT);

//		pGraph_StdParams.setPic(PIC_GRAPH_PARAMS);

		DrawGraphRectangle(en_WinId_TestParamStdGraph ,
								SETT_GRAPH_PIXEL_MIN_X ,
								SETT_GRAPH_PIXEL_MIN_Y,
								SETT_GRAPH_PIXEL_MAX_X,
								SETT_GRAPH_PIXEL_MAX_Y);

		enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;

		if(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable == true)
		{
			R_Blank_OD = GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
		}
		else
		{
			R_Blank_OD = 0;
		}
		/*Draw standard graph*/
		if( en_Factor == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

			if(0 <= GetInstance_TestParams()[stcTestData.TestId].KFactor)
			{
				X1 = MinXVal = 0;
				X2 = MaxXVal = GetInstance_TestParams()[stcTestData.TestId].KFactor * (3.0 - R_Blank_OD) ;
				Y1 = MinYVal = R_Blank_OD;
				Y2 = MaxYVal = 3.0 - R_Blank_OD;
			}
			else
			{
				X1 = MinXVal = 0;
				X2 = MaxXVal = GetInstance_TestParams()[stcTestData.TestId].KFactor * (3.0 - R_Blank_OD) ;
				Y1 = MaxYVal = R_Blank_OD;
				Y2 = MinYVal = 3.0 - R_Blank_OD;
			}
			textEqStatic.setVisible(true);
			char arr_Buffer[64] = {0};
			snprintf(arr_Buffer , 63 , "Factor : %.01f" , GetInstance_TestParams()[stcTestData.TestId].KFactor);
			textEqStatic.setText(arr_Buffer);

			UpdateGraph(en_WinId_TestParamStdGraph , X1/*X1*/,
					Y1 /*Y1*/,
					X2/*X2*/,
					Y2 /*Y2*/,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
					SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					COLOUR_BLUE);

			// updating points

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
					Y1 = (GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI - 1] );
				}

				if (isnan(Y1))
				{
				   Y1 = SETT_GRAPH_PIXEL_MAX_Y;
				}
				if (isnan(X1))
				{
					X1 = SETT_GRAPH_PIXEL_MIN_X;
				}
				DrawGraphFilledCircle(en_WinId_TestParamStdGraph , X1, Y1,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal /*X max value*/,
						SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
						SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						radius,	COLOUR_RED);
			}

			// updating axis data
				UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
				MaxXVal/*X max value*/,
				MaxYVal /*Y max value*/,
				&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
				MinXVal/*X max value*/,
				MinYVal/*Y max value*/);

		}

		else if( en_1PointLinear == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
			GetInstance_TestParams()[stcTestData.TestId].KFactor = CalculateFactor_Graph();

			if(0 <= GetInstance_TestParams()[stcTestData.TestId].KFactor )
			{
				X1 = MinXVal = 0;
				X2 = MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
				Y1 = MinYVal = R_Blank_OD;
				Y2 = MaxYVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved);
			}
			else
			{
				X1 = MinXVal = 0;
				X2 = MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
				Y1 = MaxYVal = R_Blank_OD;
				Y2 = MinYVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved);
			}

			char arr_Buffer[64] = {0};
			textEqStatic.setVisible(true);
			snprintf(arr_Buffer , 63 , "Factor : %.01f" , GetInstance_TestParams()[stcTestData.TestId].KFactor);
			textEqStatic.setText(arr_Buffer);

			UpdateGraph(en_WinId_TestParamStdGraph , X1/*X1*/,
					Y1 /*Y1*/,
					X2/*X2*/,
					Y2 /*Y2*/,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
					SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					COLOUR_BLUE);

			// updating points

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
					Y1 = (GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI - 1] );
				}

				if (isnan(Y1))
				{
				   Y1 = SETT_GRAPH_PIXEL_MAX_Y;
				}
				if (isnan(X1))
				{
					X1 = SETT_GRAPH_PIXEL_MIN_X;
				}
				DrawGraphFilledCircle(en_WinId_TestParamStdGraph , X1, Y1,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal /*X max value*/,
						SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
						SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						radius,	COLOUR_RED);
			}

			// updating axis data
				UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
				MaxXVal/*X max value*/,
				MaxYVal /*Y max value*/,
				&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
				MinXVal/*X max value*/,
				MinYVal/*Y max value*/);

		}

		else if( en_2PointLinear == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

			MaxYVal = (FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved));
			MinYVal = R_Blank_OD;
			pixel_Y = (MaxYVal - ((FindMinValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved)))) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = 0;

			for( float nI = 0 ; nI < MaxYVal; nI= nI + pixel_Y)
			{
				X1 = CalibrateToResult(stcTestData.TestId , (nI- pixel_Y));
				X2 = CalibrateToResult(stcTestData.TestId , (nI));

				UpdateGraph(en_WinId_TestParamStdGraph , X1/*X1*/,
					Y1 = (nI-pixel_Y) /*Y1*/,
					X2 /*X2*/,
					Y2 = nI /*Y2*/,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
					SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					COLOUR_BLUE);
			}

			// updating points

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
					Y1 = (GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI - 1] );
				}

				if (isnan(Y1))
				{
				   Y1 = SETT_GRAPH_PIXEL_MAX_Y;
				}
				if (isnan(X1))
				{
					X1 = SETT_GRAPH_PIXEL_MIN_X;
				}
				DrawGraphFilledCircle(en_WinId_TestParamStdGraph , X1, Y1,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal /*X max value*/,
						SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
						SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						radius,	COLOUR_RED);
			}

			// updating axis data
				UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
				MaxXVal/*X max value*/,
				MaxYVal /*Y max value*/,
				&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
				MinXVal/*X max value*/,
				MinYVal/*Y max value*/);

		}
		else if( en_LinearRegression == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
			MaxYVal = (FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved));
			MinYVal = R_Blank_OD;
			pixel_Y = (MaxYVal - ((FindMinValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved)))) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = 0;

			for( float nI = 0 ; nI < MaxYVal; nI= nI + pixel_Y)
			{
				X1 = CalibrateToResult(stcTestData.TestId , (nI- pixel_Y));
				X2 = CalibrateToResult(stcTestData.TestId , (nI));

				UpdateGraph(en_WinId_TestParamStdGraph , X1/*X1*/,
					Y1 = (nI-pixel_Y) /*Y1*/,
					X2 /*X2*/,
					Y2 = nI /*Y2*/,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
					SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					COLOUR_BLUE);
			}

			if( en_LinearRegression == CalibType)
			{
				if(e_Runtime_R2value_linear * 100 < 90)
				{
					e_Runtime_R2value_linear = e_Runtime_R2value_linear * 100;
					textEq.setVisible(true);
					textEqStatic.setVisible(true);

					char ArrCalibTypeTxt[64] = {0};
					snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.2f " ,e_Runtime_R2value_linear);
					textEqStatic.setText(ArrCalibTypeTxt);

					ArrCalibTypeTxt[64] = {0};
					snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is lesser than 90 percentage");
					textEq.setText(ArrCalibTypeTxt);
				}
			}
			// updating points

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
					Y1 = (GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI - 1] );
				}

				if (isnan(Y1))
				{
				   Y1 = SETT_GRAPH_PIXEL_MAX_Y;
				}
				if (isnan(X1))
				{
					X1 = SETT_GRAPH_PIXEL_MIN_X;
				}
				DrawGraphFilledCircle(en_WinId_TestParamStdGraph , X1, Y1,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal /*X max value*/,
						SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
						SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						radius,	COLOUR_RED);
			}

			// updating axis data
				UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
				MaxXVal/*X max value*/,
				MaxYVal /*Y max value*/,
				&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
				MinXVal/*X max value*/,
				MinYVal/*Y max value*/);

		}
		else if( en_PointToPoint == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
			MaxYVal = (FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved));
			MinYVal = R_Blank_OD;
			pixel_Y = (MaxYVal - ((FindMinValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved)))) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = 0;

			for( float nI = 0 ; nI < MaxYVal; nI= nI + pixel_Y)
			{
				X1 = CalibrateToResult(stcTestData.TestId , (nI- pixel_Y));
				X2 = CalibrateToResult(stcTestData.TestId , (nI));

				UpdateGraph(en_WinId_TestParamStdGraph , X1/*X1*/,
					Y1 = (nI-pixel_Y) /*Y1*/,
					X2 /*X2*/,
					Y2 = nI /*Y2*/,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
					SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					COLOUR_BLUE);
			}

			// updating points

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
					Y1 = (GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI - 1] );
				}

				if (isnan(Y1))
				{
				   Y1 = SETT_GRAPH_PIXEL_MAX_Y;
				}
				if (isnan(X1))
				{
					X1 = SETT_GRAPH_PIXEL_MIN_X;
				}
				DrawGraphFilledCircle(en_WinId_TestParamStdGraph , X1, Y1,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal /*X max value*/,
						SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
						SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						radius,	COLOUR_RED);
			}

			// updating axis data
				UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
				MaxXVal/*X max value*/,
				MaxYVal /*Y max value*/,
				&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
				MinXVal/*X max value*/,
				MinYVal/*Y max value*/);

		}
		else if(en_3PL == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

			MaxYVal = (FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved));
			MinYVal =  R_Blank_OD;
			pixel_Y = (MaxYVal - ((FindMinValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved)))) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = 0;

			for( float nI = MinYVal ; nI <= MaxYVal ; nI= nI + pixel_Y)
			{
				X1 = CalibrateToResult(stcTestData.TestId, (nI- pixel_Y));
				X2 = CalibrateToResult(stcTestData.TestId, (nI));

				if (X1 < 0 || X2 < 0)
				{
					continue;
				}

				UpdateGraph(en_WinId_TestParamStdGraph , X1/*X1*/,
						Y1 = (nI-pixel_Y) /*Y1*/,
						X2 /*X2*/,
						Y2 = nI /*Y2*/,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal/*X max value*/,
						SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
						SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						COLOUR_BLUE);
	    	}
			if(e_Runtime_R2value_3pl * 100 < 90)
			{
				e_Runtime_R2value_3pl = e_Runtime_R2value_3pl * 100;
				textEq.setVisible(true);
				textEqStatic.setVisible(true);

				char ArrCalibTypeTxt[64] = {0};
				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.2f " ,e_Runtime_R2value_3pl);
				textEqStatic.setText(ArrCalibTypeTxt);

				ArrCalibTypeTxt[64] = {0};
				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is lesser than 90 percentage");
//				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is out of range");
				textEq.setText(ArrCalibTypeTxt);
			}

			// updating points

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
					Y1 = (GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI - 1] );
				}

				if (isnan(Y1))
				{
				   Y1 = SETT_GRAPH_PIXEL_MAX_Y;
				}
				if (isnan(X1))
				{
					X1 = SETT_GRAPH_PIXEL_MIN_X;
				}
				DrawGraphFilledCircle(en_WinId_TestParamStdGraph , X1, Y1,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal /*X max value*/,
						SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
						SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						radius,	COLOUR_RED);
			}

			// updating axis data
				UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
				MaxXVal/*X max value*/,
				MaxYVal /*Y max value*/,
				&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
				MinXVal/*X max value*/,
				MinYVal/*Y max value*/);
		}
		else if(en_4PL == CalibType)
		{
			Calibrate_4PL_initial_params(stcTestData.TestId);
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

			MaxYVal = (FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved)) - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
			MinYVal = (FindMinValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved)) - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
			pixel_Y = (MaxYVal - MinYVal) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = FindMinValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);

			for( float nI = MinYVal ; nI <= MaxYVal + 0.2 ; nI= nI + pixel_Y)
			{
				X1 = CalibrateToResult(stcTestData.TestId, (nI- pixel_Y));
				X2 = CalibrateToResult(stcTestData.TestId, (nI));

				if (X1 < 0 || X2 < 0)
				{
					continue;
				}

				UpdateGraph(en_WinId_TestParamStdGraph , X1/*X1*/,
						Y1 = (nI-pixel_Y) /*Y1*/,
						X2 /*X2*/,
						Y2 = nI /*Y2*/,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal/*X max value*/,
						SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
						SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						COLOUR_BLUE);
	    	}
			if(e_Runtime_R2value_4pl * 100 < 90)
			{
				e_Runtime_R2value_4pl = e_Runtime_R2value_4pl * 100;
				textEq.setVisible(true);
				textEqStatic.setVisible(true);

				char ArrCalibTypeTxt[64] = {0};
				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.2f " ,e_Runtime_R2value_4pl);
				textEqStatic.setText(ArrCalibTypeTxt);

				ArrCalibTypeTxt[64] = {0};
				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is lesser than 90 percentage");
//				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is out of range");
				textEq.setText(ArrCalibTypeTxt);
			}

			// updating points

			for(uint8_t nI = 0 ; nI < NumOfStdSaved ; nI++)
			{
//				if(nI == 0)
//				{
//					X1 = 0;
//					Y1 = R_Blank_OD;
//				}
//				else
//				{
					X1 = (GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI]);
					Y1 = (GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI]) - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
//				}

				if (isnan(Y1))
				{
				   Y1 = SETT_GRAPH_PIXEL_MAX_Y;
				}
				if (isnan(X1))
				{
					X1 = SETT_GRAPH_PIXEL_MIN_X;
				}
				DrawGraphFilledCircle(en_WinId_TestParamStdGraph , X1, Y1,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal /*X max value*/,
						SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
						SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						radius,	COLOUR_RED);
			}

			// updating axis data
				UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
				MaxXVal/*X max value*/,
				MaxYVal /*Y max value*/,
				&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
				MinXVal/*X max value*/,
				MinYVal/*Y max value*/);

		}

		else if(en_CubicSpline == CalibType)
		{
			NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

			MaxYVal = (FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved));
			MinYVal = R_Blank_OD;
			pixel_Y = (MaxYVal - ((FindMinValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardOD , NumOfStdSaved)))) / 31;
			MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc , NumOfStdSaved);
			MinXVal = 0;

			for( float nI = 0 ; nI < MaxYVal; nI= nI + pixel_Y)
			{
				X1 = CalibrateToResult(stcTestData.TestId , (nI- pixel_Y));
				X2 = CalibrateToResult(stcTestData.TestId , (nI));

				if(en_CubicSpline == CalibType)
				{
					if (X1 < 0 || X2 < 0)
					{
						continue;
					}
				}
				UpdateGraph(en_WinId_TestParamStdGraph , X1/*X1*/,
					Y1 = (nI-pixel_Y) /*Y1*/,
					X2 /*X2*/,
					Y2 = nI /*Y2*/,
					MinXVal/*X min value*/,
					MaxXVal/*X max value*/,
					MinYVal/*Y min value*/,
					MaxYVal/*X max value*/,
					SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
					SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
					SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
					COLOUR_BLUE);
			}
			if( en_CubicSpline == CalibType){
				if(e_Runtime_R2value_cubic * 100 < 90)
				{
					e_Runtime_R2value_cubic = e_Runtime_R2value_cubic * 100;
					textEq.setVisible(true);
					textEqStatic.setVisible(true);

					char ArrCalibTypeTxt[64] = {0};
					snprintf(ArrCalibTypeTxt , 63 , "R-Sqr : %0.2f " ,e_Runtime_R2value_cubic);
					textEqStatic.setText(ArrCalibTypeTxt);

					ArrCalibTypeTxt[64] = {0};
					snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is lesser than 90 percentage");
	//				snprintf(ArrCalibTypeTxt , 63 , "R-Sqr is out of range");
					textEq.setText(ArrCalibTypeTxt);
				}
		}
			// updating points

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
					Y1 = (GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI - 1] );
				}

				if (isnan(Y1))
				{
				   Y1 = SETT_GRAPH_PIXEL_MAX_Y;
				}
				if (isnan(X1))
				{
					X1 = SETT_GRAPH_PIXEL_MIN_X;
				}
				DrawGraphFilledCircle(en_WinId_TestParamStdGraph , X1, Y1,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal /*X max value*/,
						SETT_GRAPH_PIXEL_MIN_X/*Graph X*/,
						SETT_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						SETT_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						SETT_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						radius,	COLOUR_RED);
			}

			// updating axis data
				UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
				MaxXVal/*X max value*/,
				MaxYVal /*Y max value*/,
				&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
				MinXVal/*X max value*/,
				MinYVal/*Y max value*/);
		}
}
