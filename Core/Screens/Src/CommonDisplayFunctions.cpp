/*
 * StatusBar.cpp
 *
 *  Created on: May 20, 2023
 *      Author: Alvin
 */
#include "../../APPL/Inc/NonVolatileMemory.h"
#include "RTC.h"
#include "Printer.h"
#include "../APPL/Inc/Testrun.h"
#include "../../APPL/Inc/NVM_QCSetup.hpp"
#include "../Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "math.h"

#include "RTC.h"
#define MAX_PRINT_LINELENGTH	33


#define SEPERATION_STRING 				"********************************"
#define SEPERATION_STRING_DOUBLE_LINE	"================================"
#define MULTILINE_SPERATOR				"\n\n\n"
#define DOUBLELINE_SPERATOR				"\n\n"
#define SINGLELINE_SPERATOR				"\n"


extern WindowMapping stcWindowMapping[en_WinID_Max];

/*All unit strings are defined here*/
char arr_cUnitBuffer[en_MaxUnits][8] = {"mg/dL",/*0*/
									"U/L",/*1*/
									"g/dL",/*2*/
									"mmol/L",/*3*/
									"%",/*4*/
									"umol/L",/*5*/
									"g/L",/*6*/
									"mg/L",/*7*/
									"ug/L",/*8*/
									"U/mL",/*9*/
									"ug/mL",/*10*/
									"ng/mL",/*11*/
									"ABS"/*12*/
									" "/*13*/
};

char g_arrTestmethodnameBuffer[en_TestProdecureMax][20] =
									{
										"Endpoint",/*0*/
										"Kinetics",/*1*/
										"Fixed Time",/*2*/
										"Turbi - Fix Time",/*3*/
										"Absorbance",/*5*/

//										"Coagulation",/*4*/

									};

char g_arrFilterNames[en_FilterMax][8] = {
			"NILL",/*0*/
			"340",/*1*/
			"405",/*2*/
//			"450",/*3*/
			"505",/*4*/
			"546",/*5*/
			"578",/*6*/
			"630",/*7*/
//			"670",/*8*/
};

char g_arrFilterNames_OpticsDiag[en_FilterMax][32] = {
			"NILL",/*0*/
			"340  (1200 - 2000)",/*1*/
			"405  (1600 - 3200)",/*2*/
//			"450",/*3*/
			"505  (2000 - 3800)",/*4*/
			"546  (1600 - 3800)",/*5*/
			"578  (1700 - 3900)",/*6*/
			"630  (2000 - 4000)",/*7*/
//			"670",/*8*/
};


char g_arrCuvveteNameBuffer[en_MaxCuvetteType][24] =
									{
										"Flowcell",
										"Cuvette",
										"Coagulationtube",
									};
char g_CalibrationTypeNameBUffer[en_CalibrationMax][24] =
									{
											"Factor",/*0*/
											"1 Point Linear",/*1*/
											"2 Point Linear",/*2*/
											"Point To Point",/*3*/
											"Linear Regression",/*4*/
											"Cubic Spline",/*5*/
											"3PL",/*6*/
											"4PL",/*7*/
									};

void UpdateHomeSreenDateTimeTemp(NexText *ptrDateTimeTxt, NexText *pTxtPelTemp, NexText *pTxtIncuTemp, NexText *pTxtUser)
{
	char arr_CurrTimeStamp[32] = {0};
	RTC_DateTypeDef sDate;
	GetCurrentDate(&sDate);
	RTC_TimeTypeDef sTime;
	GetCurrentTime(&sTime);

	snprintf(arr_CurrTimeStamp , 31 , "%02u/%02u/%02u   %02u:%02u:%02u" , (unsigned int)sDate.Date ,
			(unsigned int)sDate.Month , (unsigned int)sDate.Year ,
			(unsigned int)sTime.Hours , (unsigned int)sTime.Minutes,(unsigned int)sTime.Seconds);

	ptrDateTimeTxt->setText(arr_CurrTimeStamp);
	pTxtUser->setText(stcTestData.arrUserName);
	char arr_strTemp[13] = {0};

	if(objstcTempControl[en_PhotometerTemp].CurrentTemp > 99 || objstcTempControl[en_PhotometerTemp].CurrentTemp < 1)
	{
		pTxtPelTemp->setText("--.-");
	}
	else
	{
		snprintf(arr_strTemp , 12 , "%.1f" , objstcTempControl[en_PhotometerTemp].CurrentTemp);
		pTxtPelTemp->setText(arr_strTemp);
	}
	if(objstcTempControl[en_ChamperTemp].CurrentTemp > 99 || objstcTempControl[en_ChamperTemp].CurrentTemp < 1)
	{
		pTxtIncuTemp->setText("--.-");
	}
	else
	{
		snprintf(arr_strTemp , 12 , "%.1f" , objstcTempControl[en_ChamperTemp].CurrentTemp);
		pTxtIncuTemp->setText(arr_strTemp);
	}
}

void UpdateStatusBar(NexPicture *pPicTestType , NexText *pTestName , NexText *pTestWavelength ,
		NexText *pTxtDate, NexText *pTxtTime, NexText *pTxtPelTemp ,NexText *pTxtIncbTemp, NexText *pTxtUser)

{
	char arr_CurrTimeStamp[32] = {0};
	RTC_DateTypeDef sDate;
	GetCurrentDate(&sDate);
	RTC_TimeTypeDef sTime;
	GetCurrentTime(&sTime);
	char arrBuffer[32] = {0};

	snprintf(arr_CurrTimeStamp , 31 , "%02u/%02u/%02u" , (unsigned int)sDate.Date ,
			(unsigned int)sDate.Month , (unsigned int)sDate.Year);

	pTxtDate->setText(arr_CurrTimeStamp);

	snprintf(arr_CurrTimeStamp , 31 , "%02u:%02u" , (unsigned int)sTime.Hours , (unsigned int)sTime.Minutes);
	pTxtUser->setText(stcTestData.arrUserName);
	pTxtTime->setText(arr_CurrTimeStamp);

	char arr_strTemp[13] = {0};
	if(objstcTempControl[en_PhotometerTemp].CurrentTemp > 99 || objstcTempControl[en_PhotometerTemp].CurrentTemp < 1)
	{
		pTxtPelTemp->setText("--.-");
	}
	else
	{
		snprintf(arr_strTemp , 12 , "%.1f" , objstcTempControl[en_PhotometerTemp].CurrentTemp);
		pTxtPelTemp->setText(arr_strTemp);
	}

	if(objstcTempControl[en_ChamperTemp].CurrentTemp > 99 || objstcTempControl[en_ChamperTemp].CurrentTemp < 1)
	{
		pTxtIncbTemp->setText("--.-");
	}
	else
	{
		snprintf(arr_strTemp , 12 , "%.1f" , objstcTempControl[en_ChamperTemp].CurrentTemp);
		pTxtIncbTemp->setText(arr_strTemp);
	}

	pTestName->setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);


	if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
	{
		snprintf(arrBuffer , 31 , "%snm\\r%snm",&g_arrFilterNames[GetInstance_TestParams()[stcTestData.TestId].PrimWavelength][0],
													&g_arrFilterNames[GetInstance_TestParams()[stcTestData.TestId].SecWavelength][0]);
	}
	else
	{
		snprintf(arrBuffer , 31 , "%snm",&g_arrFilterNames[GetInstance_TestParams()[stcTestData.TestId].PrimWavelength][0]);
	}
	pTestWavelength->setText(arrBuffer);

	switch(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType)
	{
		case en_Endpoint:
			pPicTestType->Set_background_image_pic(IMAGE_ID_ENDPOINT);
//			pTestType->setText("ENDPOINT");
			break;
		case en_FixedTime:
			pPicTestType->Set_background_image_pic(IMAGE_ID_FIXEDPOINT);
//			pTestType->setText("FIXED TIME");
			break;
		case en_Kinetics:
			pPicTestType->Set_background_image_pic(IMAGE_ID_KINETICS);
//			pTestType->setText("KINETICS");
			break;
		case en_Turbidimetry:
			pPicTestType->Set_background_image_pic(IMAGE_ID_TURBIDIMETRY);
//			pTestType->setText("TURBIDIMETRY");
			break;
//		case en_Differential:
//			pPicTestType->Set_background_image_pic(IMAGE_ID_DIFFRENTIAL);
////			pTestType->setText("DIFFERENTIAL");
//			break;
		case en_CoagulationTest:
			pPicTestType->Set_background_image_pic(IMAGE_ID_COAGULATION);
//			pTestType->setText("COAGULATION");
			break;
		case en_Absorbance:
			pPicTestType->Set_background_image_pic(IMAGE_ID_ENDPOINT);
			break;
		default:
			pPicTestType->Set_background_image_pic(DELAULT_NO_TEST_IMAGE_ID);
//			pTestType->setText("NO TEST");
			break;
	}
}
uint8_t MaxNumOfStandardsForSelectedMethod(enCalibrationType CalibType)
{
	uint8_t MaxNumOfStd = 0;
	switch(CalibType)
	{
	 case en_Factor:
		 MaxNumOfStd = 0;
		 break;
	 case en_1PointLinear:
		 MaxNumOfStd = 1;
		 break;
	 case en_2PointLinear:
		 MaxNumOfStd = 2;
		 break;
	 case en_PointToPoint:
		 MaxNumOfStd = 6;
		 break;
	 case en_LinearRegression:
		 MaxNumOfStd = 6;
		 break;
	 case en_3PL:
		 MaxNumOfStd = 2;
		 break;
	 case en_4PL:
	 case en_CubicSpline:
		 MaxNumOfStd = 6;
		 break;
	 default:
		 MaxNumOfStd = 0;
		 break;
	}
	return MaxNumOfStd;
}
uint8_t MinNumOfStandardsForSelectedMethod(enCalibrationType CalibType)
{
	uint8_t MinNumOfStd = 0;
	switch(CalibType)
	{
	 case en_Factor:
		 MinNumOfStd = 0;
		 break;
	 case en_1PointLinear:
		 MinNumOfStd = 1;
		 break;
	 case en_2PointLinear:
		 MinNumOfStd = 2;
		 break;
	 case en_PointToPoint:
		 MinNumOfStd = 3;
		 break;
	 case en_LinearRegression:
		 MinNumOfStd = 2;
		 break;
	 case en_3PL:
		 MinNumOfStd = 2;
		 break;
	 case en_4PL:
		 MinNumOfStd = 4;
		 break;
	 case en_CubicSpline:
		 MinNumOfStd = 4;
		 break;
	 default:
		 MinNumOfStd = 0;
		 break;
	}
	return MinNumOfStd;
}
float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh)
{
  float val = (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
  return val;
}
long mapInt(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

uint16_t FindMaxADCValueFromList(uint16_t *ptrArray , uint16_t Start, uint16_t Length)
{
	uint16_t MaxVal = 0;

	for(uint16_t nI = Start ; nI < Length ; ++nI)
	{
		if(MaxVal < ptrArray[nI])
		{
			MaxVal = ptrArray[nI];
		}
	}
	return MaxVal;
}


float FindMaxValueFromList(float *ptrArray , uint16_t Length)
{
	float MaxVal = 0x00000000;
	for(uint16_t nI = 0 ; nI < Length ; ++nI)
	{
		if(MaxVal < ptrArray[nI])
		{
			MaxVal = ptrArray[nI];
		}
	}
	return MaxVal;
}

float FindMinValueFromList(float *ptrArray , uint16_t Length)
{
	float MinVal = 0xFFFFFFFF;
	for(uint16_t nI = 0 ; nI < Length ; ++nI)
	{
		if(MinVal > ptrArray[nI])
		{
			MinVal = ptrArray[nI];
		}
	}
	return MinVal;
}

void ClearGraph(enWindowID ID_Win , float X1 , float Y1 ,float X2 , float Y2)
{
	stcWindowMapping[ID_Win].obJCurrPage.FillRectangle(X1 - 2,
			Y1 - 2, X2 + 2, Y2 + 2, COLOUR_WHITE);
}
void DrawGraphRectangle(enWindowID ID_Win , float X1 , float Y1 ,float X2 , float Y2)
{
	stcWindowMapping[ID_Win].obJCurrPage.DrawReactange(X1, Y1, X2,
			Y2, COLOUR_LIGHTBLACK);
}

void DrawGraphFilledCircle(enWindowID ID_Win , float X1 , float Y1,
		float XvalMin , float XvalMax,float YvalMin , float YvalMax ,
		uint16_t GraphX , uint16_t GraphY , uint16_t GraphWidth , uint16_t GraphHeight ,
		uint16_t radius, uint32_t Colour)
{
	if(XvalMin > X1)
	{
		X1 = XvalMin;
	}
	if(YvalMin > Y1)
	{
		Y1 = YvalMin;
	}

	if(XvalMax < X1)
	{
		X1 = XvalMax;
	}
	if(YvalMax < Y1)
	{
		Y1 = YvalMax;
	}


	uint16_t nX1 = (uint16_t)mapFloat(X1 , (float)XvalMin , (float)XvalMax ,
			(float)GraphX , (float)GraphWidth);

	uint16_t nY1 = (uint16_t)mapFloat(Y1 , (float)YvalMin , (float)YvalMax ,
				(float)GraphHeight , (float)GraphY);

	if(GraphX > nX1)
	{
		nX1 = GraphX;
	}

	if(nY1 == 0)
	{
		nY1 = GraphHeight;
	}

	if(GraphY > nY1)
	{
		nY1 = GraphY;
	}

//
	if(GraphWidth < nX1)
	{
		nX1 = GraphWidth;
	}

	if(GraphHeight < nY1)
	{
		nY1 = GraphHeight;
	}

	stcWindowMapping[ID_Win].obJCurrPage.FilledCircle(nX1, nY1, radius , Colour);
}

void UpdateGraph(enWindowID ID_Win , float X1 , float Y1 ,float X2 , float Y2 ,
				float XvalMin , float XvalMax , float YvalMin , float YvalMax ,
				uint16_t GraphX , uint16_t GraphY , uint16_t GraphWidth , uint16_t GraphHeight ,
				uint32_t Colour)
{

	if(XvalMin > X1)
	{
		X1 = XvalMin;
	}
	if(XvalMin > X2)
	{
		X2 = XvalMin;
	}

	if(YvalMin > Y1)
	{
		Y1 = YvalMin;
	}
	if(YvalMin > Y2)
	{
		Y2 = YvalMin;
	}

//
	if(XvalMax < X1)
	{
		X1 = XvalMax;
	}
	if(XvalMax < X2)
	{
		X2 = XvalMax;
	}

	if(YvalMax < Y1)
	{
		Y1 = YvalMax;
	}
	if(YvalMax < Y2)
	{
		Y2 = YvalMax;
	}

	uint16_t nX1 = (uint16_t)mapFloat(X1 , (float)XvalMin , (float)XvalMax ,
			(float)GraphX , (float)GraphWidth);

	uint16_t nY1 = (uint16_t)mapFloat(Y1 , (float)YvalMin , (float)YvalMax ,
				(float)GraphHeight , (float)GraphY);

	uint16_t nX2 = (uint16_t)mapFloat(X2 , (float)XvalMin , (float)XvalMax ,
			(float)GraphX , (float)GraphWidth);

	uint16_t nY2 = (uint16_t)mapFloat(Y2 , (float)YvalMin , (float)YvalMax ,
				(float)GraphHeight , (float)GraphY);

	if(GraphX > nX1)
	{
		nX1 = GraphX;
	}
	if(GraphX > nX2)
	{
		nX2 = GraphX;
	}
	if(nY1 == 0)
	{
		nY1 = GraphHeight;
	}
	if(nY2 == 0)
	{
		nY2 = GraphHeight;
	}
	if(GraphY > nY1)
	{
		nY1 = GraphY;
	}
	if(GraphY > nY2)
	{
		nY2 = GraphY;
	}
//
	if(GraphWidth < nX1)
	{
		nX1 = GraphWidth;
	}
	if(GraphWidth < nX2)
	{
		nX2 = GraphWidth;
	}

	if(GraphHeight < nY1)
	{
		nY1 = GraphHeight;
	}
	if(GraphHeight < nY2)
	{
		nY2 = GraphHeight;
	}

	stcWindowMapping[ID_Win].obJCurrPage.DrawLine(nX1, nY1+1, nX2, nY2+1, Colour);
	stcWindowMapping[ID_Win].obJCurrPage.DrawLine(nX1, nY1, nX2, nY2, Colour);
	stcWindowMapping[ID_Win].obJCurrPage.DrawLine(nX1, nY1-1, nX2, nY2-1, Colour);
}
void UpdateGraphAxisData(NexText *pXMax , NexText *pYMax , float XvalMax , float YvalMax,
		NexText *pXMin , NexText *pYMin , float XvalMin , float YvalMin)
{
	char arrBuffer[64] = {0};
	snprintf(arrBuffer , 63 , "%d",(int)XvalMax);
	pXMax->setText(arrBuffer);
	snprintf(arrBuffer , 63 , "%.1f",YvalMax);
	pYMax->setText(arrBuffer);

	snprintf(arrBuffer , 63 , "%d",(int)XvalMin);
	pXMin->setText(arrBuffer);
	snprintf(arrBuffer , 63 , "%.1f",YvalMin);
	pYMin->setText(arrBuffer);
}
void DrawVerticalLineOnGraph(enWindowID ID_Win , float X , float XvalMin , float XvalMax)
{
	uint16_t nX = (uint16_t)mapFloat(X , (float)XvalMin , (float)XvalMax ,
			(float)TEST_GRAPH_PIXEL_MIN_X , (float)TEST_GRAPH_PIXEL_MAX_X);
	stcWindowMapping[ID_Win].obJCurrPage.DrawLine(nX, TEST_GRAPH_PIXEL_MIN_Y, nX, TEST_GRAPH_PIXEL_MAX_Y, COLOUR_VERTICAL_LINE);
}

void DrawLineOnGraph(enWindowID ID_Win , uint16_t X1 ,
		uint16_t Y1 , uint16_t X2 , uint16_t Y2 , uint16_t u16Col)
{
	stcWindowMapping[ID_Win].obJCurrPage.DrawLine(X1, Y1, X2, Y2, u16Col);
}

bool CompareBuffer(float *ptrBuff1 , float *ptrBuff2 , size_t Length)
{
	bool status = true;
	for(size_t nI = 0 ; nI < Length ; ++nI)
	{
		if(ptrBuff1[nI] != ptrBuff2[nI])
		{
			status = false;
			break;
		}
	}
	return status;
}

void ClearTestSavedValues(void)
{
	uint8_t nI = stcTestData.TestId;
	GetInstance_TestParams()[nI].DiwaterDoneFlag = false;
	GetInstance_TestParams()[nI].DiwaterPriADC = float(0);
	GetInstance_TestParams()[nI].DiwaterSecADC = float(0);
	GetInstance_TestParams()[nI].ReagentDoneFlag = false;
	GetInstance_TestParams()[nI].ReagentBlankDoneFlag = false;
	GetInstance_TestParams()[nI].ReagentBlankAbs = float(0);
	GetInstance_TestParams()[nI].ReagentAbs = float(0);
	GetInstance_TestParams()[nI].KFactor = float(DEFAULT_K_FACTOR);
	GetInstance_TestParams()[nI].TurbiAbsLimit = float(DEFAULT_TURBIABSLIMIT);
	for(uint8_t nJ = 0 ; nJ < MAX_NUM_OF_STANDARDS ; ++nJ)
	{
		GetInstance_TestParams()[nI].StandardConc[nJ] = float(0);
		GetInstance_TestParams()[nI].StandardOD[nJ] = float(0);
		GetInstance_TestParams()[nI].StandardODBlank[nJ] = float(0);
		GetInstance_TestParams()[nI].StandardDoneFlag[nJ] = false;
		GetInstance_TestParams()[nI].StandardBlankDoneFlag[nJ] = false;
	}
}

float Calculate_Percentage(float val , float TotalVal)
{
 float percentage = (float)((val / TotalVal) * 100.0);
 return percentage;
}
void PrintTestParams(uint16_t u16TestId)
{
	char arr_CBuffer[64] = {0};
	RTC_DateTypeDef m_Date;
	RTC_TimeTypeDef m_Time;
	GetCurrentDate(&m_Date);
	GetCurrentTime(&m_Time);

	/*Printing headers*/
	Print_Header();
	/*Showing print name*/

	snprintf(arr_CBuffer , 63 , "%s" , "********************************");
	PrintStringOnPrinter(&arr_CBuffer[0]);
	PrinterFeedLine(1);

	SendBoldOnOffCMD(true);
	snprintf(arr_CBuffer , 63 , "%s" , "Test Parameter");
	PrintStringOnPrinter(&arr_CBuffer[0]);
	PrinterFeedLine(1);
	SendBoldOnOffCMD(false);

	/*Showing Test name*/
	snprintf(arr_CBuffer , 63 , "%s    : %s" , "Test name" ,
			GetInstance_TestParams()[u16TestId].arrTestName);
	PrintStringOnPrinter(&arr_CBuffer[0]);

	snprintf(arr_CBuffer , 63 , "%s    : %s" , "Full name" ,
			GetInstance_TestParams()[u16TestId].arrFullTestName);
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing Test Unit*/
	if(true == GetInstance_TestParams()[u16TestId].bCustomUNitEnable)
	{
		snprintf(arr_CBuffer,63,  "%s         : %s" , "Unit" ,
				GetInstance_TestParams()[u16TestId].arrCustomUnit);
	}
	else
	{
		enUnits ResultUnit = GetInstance_TestParams()[u16TestId].Unit;
		snprintf(arr_CBuffer,63,  "%s         : %s" , "Unit" ,
													&arr_cUnitBuffer[ResultUnit][0]);
	}
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing Status of reagent blank*/
	snprintf(arr_CBuffer , 63 , "%s      : %s" , "R.Blank" ,
			(GetInstance_TestParams()[u16TestId].ReagentBlankEnable)?"Yes":"No");
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing Status of sample blank*/
	snprintf(arr_CBuffer , 63 , "%s : %s" , "Sample blank" ,
			(GetInstance_TestParams()[u16TestId].SampleBlankEnable)?"Yes":"No");
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing Test procedure type*/
	enTestProcedureType TestMethod = GetInstance_TestParams()[u16TestId].TestProcedureType;
	snprintf(arr_CBuffer , 63 , "%s       : %s" , "Method" ,
								(&g_arrTestmethodnameBuffer[TestMethod][0]));
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing Primary filter*/
	en_WavelengthFilter enPFIlter = GetInstance_TestParams()[u16TestId].PrimWavelength;
	snprintf(arr_CBuffer , 63 , "%s     : %s" , "Filter 1" ,
								(&g_arrFilterNames[enPFIlter][0]));
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing Secondary filter*/
	if(	en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
	{
		en_WavelengthFilter enSFIlter = GetInstance_TestParams()[u16TestId].SecWavelength;
		snprintf(arr_CBuffer , 63 , "%s     : %s" , "Filter 2" ,
									(&g_arrFilterNames[enSFIlter][0]));
		PrintStringOnPrinter(&arr_CBuffer[0]);
	}

	/*Showing Temperature*/
	float Temp = GetInstance_TestParams()[u16TestId].Temperature;
	snprintf(arr_CBuffer , 63 , "%s  : %.02f" , "Temperature" ,
								(Temp));
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing Aspiration voleume*/
	snprintf(arr_CBuffer , 63 , "%s     : %u" , "Asp Vol." ,
								((unsigned int)GetInstance_TestParams()[u16TestId].AspVoleume));
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing Lag time(S)*/
	snprintf(arr_CBuffer , 63 , "%s  : %u" , "Lag Time(s)" ,
								(unsigned int)(GetInstance_TestParams()[u16TestId].IncubationTimeSec));
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing Read time(S)*/
	snprintf(arr_CBuffer , 63 , "%s : %u" , "Read Time(s)" ,
								(unsigned int)(GetInstance_TestParams()[u16TestId].MeasurementTimeSec));
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing Cuvette type*/
	enCuvetteType m_Cvtype = GetInstance_TestParams()[u16TestId].CuvetteType;
	snprintf(arr_CBuffer , 63 , "%s : %s" , "Cuvette Type" ,
								(&g_arrCuvveteNameBuffer[m_Cvtype][0]));
	PrintStringOnPrinter(&arr_CBuffer[0]);



	/*Showing test R blank limit*/
	float RBlankLow = GetInstance_TestParams()[u16TestId].ReagentBlankLimit[0];
	float RBlankHigh = GetInstance_TestParams()[u16TestId].ReagentBlankLimit[1];
	snprintf(arr_CBuffer , 63 , "RB Abs Range : %.0004f - %.0004f " , RBlankLow , RBlankHigh);
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing test normal range*/
	float RefRangeLow = GetInstance_TestParams()[u16TestId].RefrenceRange[0];
	float RefRangeHigh = GetInstance_TestParams()[u16TestId].RefrenceRange[1];
	snprintf(arr_CBuffer , 63 , "Ref. Range   : %.0004f - %.0004f " , RefRangeLow , RefRangeHigh);
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing test Linearity range*/
	float LinRangeLow = GetInstance_TestParams()[u16TestId].LinearityRange[0];
	float LinRangeHigh = GetInstance_TestParams()[u16TestId].LinearityRange[1];
	snprintf(arr_CBuffer , 63 , "Lin. Range   : %.0004f - %.0004f " , LinRangeLow , LinRangeHigh);
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing test Abs limit*/
	float AbsLow = GetInstance_TestParams()[u16TestId].AbsorbanceLimit[0];
	float AbsHigh = GetInstance_TestParams()[u16TestId].AbsorbanceLimit[1];
	snprintf(arr_CBuffer , 63 , "Abs. Range   : %.0004f - %.0004f " , AbsLow , AbsHigh);
	PrintStringOnPrinter(&arr_CBuffer[0]);

	/*Showing test Abs limit*/
	if(GetInstance_TestParams()[u16TestId].TestProcedureType == en_Kinetics)
	{
		float Delta = GetInstance_TestParams()[u16TestId].MaxDeltaAbsPerMin;
		snprintf(arr_CBuffer , 63 , "Max dAbs./min   : %.0004f" , Delta);
		PrintStringOnPrinter(&arr_CBuffer[0]);
	}
	PrinterFeedLine(1);

	/*Showing Cal type*/
	snprintf(arr_CBuffer , 63 , "%s    : %s" , "Cal. Type" ,
								(&g_CalibrationTypeNameBUffer[GetInstance_TestParams()[u16TestId].Calib][0]));
	PrintStringOnPrinter(&arr_CBuffer[0]);

	if (GetInstance_TestParams()[u16TestId].Calib == en_Factor)
	{
	/*Showing Factor value*/
	snprintf(arr_CBuffer , 63 , "%s       : %.0004f" , "Factor" ,
								(GetInstance_TestParams()[u16TestId].KFactor));
	PrintStringOnPrinter(&arr_CBuffer[0]);
	}

	if (GetInstance_TestParams()[u16TestId].Calib != en_Factor)
	{
	/*Showing Num of standards*/
	snprintf(arr_CBuffer , 63 , "%s   : %u" , "No of Std." ,
								(unsigned int)(GetInstance_TestParams()[u16TestId].NumOfStandardsSaved));
	PrintStringOnPrinter(&arr_CBuffer[0]);

		if (GetInstance_TestParams()[u16TestId].ReagentBlankEnable)
		{
			PrintStringOnPrinter((char*)"Blank Type   : Reagent Blank");
		}
		else
		{
			PrintStringOnPrinter((char*)"Blank Type   : Water Blank");
		}
		uint8_t NumofStd = GetInstance_TestParams()[u16TestId].NumOfStandardsSaved;

		for(uint8_t nI = 0 ;nI < NumofStd ; ++nI)
		{
			snprintf(arr_CBuffer , 63 , "S%d Conc.     : %0.0004f" , nI+1 ,
										(GetInstance_TestParams()[u16TestId].StandardConc[nI]));
			PrintStringOnPrinter(&arr_CBuffer[0]);
		}
	}

	/*Feed*/
	strncpy(arr_CBuffer , "********************************" , 63);
	PrintStringOnPrinter(&arr_CBuffer[0]);
	Print_Footer();
	PrinterFeedLine(4);
}
void PrintQcLJPlot(float *pData , QCTEST *pQcTest , uint32_t u32NumofData)
{
	uint8_t u8NumOfQcResAvailable = 0;
	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	QCDATA *pQcData = Get_InstaneQcResult();

	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);
	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
	{
		u8QcTestEntryIdx = GetSelectedTestQcRow();
	}
	u8NumOfQcResAvailable = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved;
	if(MAX_NUM_OF_QC_PER_TEST <= u8NumOfQcResAvailable)
	{
		u8NumOfQcResAvailable = MAX_NUM_OF_QC_PER_TEST;
	}

	uint8_t buffer_array[195] = {0};
	double diff=0;
	uint8_t  xloc=0;
	uint8_t  yloc=0;
	uint8_t feed[]={0x0A};
	uint8_t x=0;
	uint8_t y=0;

	uint8_t line_height[]={0x1B, 0x33, 0x00};
	uint8_t default_line_height[]={0x1B, 0x32};

	static const uint8_t graph_array[12][195] = {
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92, 0x92, 0x92, 0x6c, 0x00, 0x64, 0x92, 0x92, 0x4c, 0x00, 0xfe, 0x82, 0x44, 0x38, 0x00, 0x00, 0xff, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x08, 0x08, 0x09, 0x06, 0x00, 0x06, 0x09, 0x09, 0x04, 0x00, 0x0f, 0x08, 0x04, 0x03, 0x00, 0x00, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x60, 0xa0, 0x20, 0x20, 0x00, 0x40, 0x20, 0x20, 0xc0, 0x00, 0xe0, 0x20, 0x40, 0x80, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0xfe, 0x02, 0x00, 0x64, 0x92, 0x92, 0x4c, 0x00, 0xfe, 0x82, 0x44, 0x38, 0x00, 0x00, 0xff, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x80, 0x00, 0x80, 0x40, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x42, 0xfe, 0x02, 0x00, 0x64, 0x92, 0x92, 0x4c, 0x00, 0xfe, 0x82, 0x44, 0x38, 0x00, 0x00, 0xff, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x04, 0x08, 0x08, 0x09, 0x06, 0x00, 0x06, 0x09, 0x09, 0x04, 0x00, 0x0f, 0x08, 0x04, 0x03, 0x00, 0x00, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x60, 0xa0, 0x20, 0x20, 0x00, 0x40, 0x20, 0x20, 0xc0, 0x00, 0xe0, 0x20, 0x40, 0x80, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x92, 0x92, 0x92, 0x6c, 0x00, 0x64, 0x92, 0x92, 0x4c, 0x00, 0xfe, 0x82, 0x44, 0x38, 0x00, 0x00, 0xff, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0xff},
	{0x1B, 0x2A, 0x00, 0xBE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xff}};

	uint8_t print_array[12][195];

	uint8_t LJ_plot[MAX_PRINT_LINELENGTH] =  {"             LJ - PLOT         "};
	uint8_t  number[MAX_PRINT_LINELENGTH]  = {"                 N             "};
	uint16_t total_result_count = u8NumOfQcResAvailable;
//	float result[MAX_NUM_OF_QC_PER_TEST] = {0};
//	memcpy(result , pQcData->Res[u8QcTestEntryIdx] , MAX_NUM_OF_QC_PER_TEST);

   for (y = 0; y < 12; y++)
   {
		for (x = 0; x < 195; x++)
		{
			print_array[y][x] = graph_array[y][x];
		}
   }

   float lj_mean = pQcTest->fMeanVal;
   float lj_sd =	(pQcTest->fHighVal);

	double val_per_dot = ((3*lj_sd) / 36);

	for(uint16_t result_count = 0; result_count < total_result_count; result_count++)
	{
		diff = pQcData->Res[u8QcTestEntryIdx][result_count] - lj_mean;

	   double pixel = diff/val_per_dot;

	   if(pixel < -50){
		   pixel = -50;
	   }

	   if(pixel > 50)
	   {
		   pixel = 50;
	   }

		if(((int)(pixel * 100) % 100) > 50)
		{
			pixel = (int8_t)((int)pixel + 1);
		}
		else{
			pixel = (int8_t)((int)pixel);
		}

		  /*Line - mean */

	   if (pixel == 0 || pixel == 1 ){

		 xloc= (result_count*5) + 35;
		 yloc= 5;
		 print_array[yloc][xloc] = 0x03;
		 print_array[yloc][xloc+1] = 0x03;
	   }

	   if (pixel == 2 || pixel == 3 ){

		 xloc= (result_count*5) + 35;
		 yloc= 5;
		 print_array[yloc][xloc] = 0x0D;
		 print_array[yloc][xloc+1] = 0x0D;
	   }

	   if (pixel == 4 || pixel == 5 ){

		 xloc= (result_count*5) + 35;
		 yloc= 5;
		 print_array[yloc][xloc] = 0x31;
		 print_array[yloc][xloc+1] = 0x31;
	   }

	   if (pixel == 6 || pixel == 7 ){

		 xloc= (result_count*5) + 35;
		 yloc= 5;
		 print_array[yloc][xloc] = 0xC1;
		 print_array[yloc][xloc+1] = 0xC1;
	   }

	   if (pixel == 8 || pixel == 9 ){

		 xloc= (result_count*5) + 35;
		 yloc= 4;
		 print_array[yloc][xloc] = 0x13;
		 print_array[yloc][xloc+1] = 0x13;
	   }

	   if (pixel == 10 || pixel == 11 ){

		 xloc= (result_count*5) + 35;
		 yloc= 4;
		 print_array[yloc][xloc] = 0x1C;
		 print_array[yloc][xloc+1] = 0x1C;
	   }
		  /*Line - mean + 1sd */

	   if (pixel == 12 || pixel == 13 ){

		 xloc= (result_count*5) + 35;
		 yloc= 4;
		 print_array[yloc][xloc] = 0x38;
		 print_array[yloc][xloc+1] = 0x38;
	   }

	   if (pixel == 14 || pixel == 15 ){

		 xloc= (result_count*5) + 35;
		 yloc= 4;
		 print_array[yloc][xloc] = 0xD0;
		 print_array[yloc][xloc+1] = 0xD0;
	   }

	   if (pixel == 16 || pixel == 17 ){

		 xloc= (result_count*5) + 35;
		 yloc= 3;
		 print_array[yloc][xloc] = 0x03;
		 print_array[yloc][xloc+1] = 0x03;
	   }

	   if (pixel == 18 || pixel == 19 ){

		 xloc= (result_count*5) + 35;
		 yloc= 3;
		 print_array[yloc][xloc] = 0x0C;
		 print_array[yloc][xloc+1] = 0x0C;
	   }

	   if (pixel == 20 || pixel == 21 ){

		 xloc= (result_count*5) + 35;
		 yloc= 3;
		 print_array[yloc][xloc] = 0x30;
		 print_array[yloc][xloc+1] = 0x30;
	   }

	   if (pixel == 22 || pixel == 23 ){

		 xloc= (result_count*5) + 35;
		 yloc= 3;
		 print_array[yloc][xloc] = 0xC0;
		 print_array[yloc][xloc+1] = 0xC0;
	   }

		  /*Line - mean + 2sd */

	if (pixel == 24 || pixel == 25 ){

		 xloc= (result_count*5) + 35;
		 yloc= 2;
		 print_array[yloc][xloc] = 0x03;
		 print_array[yloc][xloc+1] = 0x03;
	}

	if (pixel == 26 || pixel == 27 ){

		 xloc= (result_count*5) + 35;
		 yloc= 2;
		 print_array[yloc][xloc] = 0x0D;
		 print_array[yloc][xloc+1] = 0x0D;
	}

	if (pixel == 28 || pixel == 29 ){

		 xloc= (result_count*5) + 35;
		 yloc= 2;
		 print_array[yloc][xloc] = 0x31;
		 print_array[yloc][xloc+1] = 0x31;
	}

	if (pixel == 30 || pixel == 31 ){

		 xloc= (result_count*5) + 35;
		 yloc= 2;
		 print_array[yloc][xloc] = 0xC1;
		 print_array[yloc][xloc+1] = 0xC1;
	}


	if (pixel == 32 || pixel == 33 ){

		 xloc= (result_count*5) + 35;
		 yloc= 1;
		 print_array[yloc][xloc] = 0x13;
		 print_array[yloc][xloc+1] = 0x13;
	}

	if (pixel == 34 || pixel == 35 ){

		 xloc= (result_count*5) + 35;
		 yloc= 1;
		 print_array[yloc][xloc] = 0x1C;
		 print_array[yloc][xloc+1] = 0x1C;
	}


	if (pixel == 36 || pixel == 37 ){

		 xloc= (result_count*5) + 35;
		 yloc= 1;
		 print_array[yloc][xloc] = 0x38;
		 print_array[yloc][xloc+1] = 0x38;
	}

	if (pixel == 38 || pixel == 39 ){

		 xloc= (result_count*5) + 35;
		 yloc= 1;
		 print_array[yloc][xloc] = 0xD0;
		 print_array[yloc][xloc+1] = 0xD0;
	}

	if (pixel > 39 ){

		 xloc= (result_count*5) + 35;
		 yloc= 0;
		 print_array[yloc][xloc] = 0xB0;
		 print_array[yloc][xloc+1] = 0xB0;
	}



   /*mean - Negative value 0 line */

   if (pixel == 0)
   {
	 xloc= (result_count*5) + 35;
	 yloc= 6;
	 print_array[yloc][xloc] = 0x80;
	 print_array[yloc][xloc+1] = 0x80;
   }



   if (pixel == -1 || pixel == -2 ){

	 xloc= (result_count*5) + 35;
	 yloc= 6;
	 print_array[yloc][xloc] = 0xC0;
	 print_array[yloc][xloc+1] = 0xC0;
   }
   if (pixel == -3 || pixel == -4 ){

	 xloc= (result_count*5) + 35;
	 yloc= 6;
	 print_array[yloc][xloc] = 0x30;
	 print_array[yloc][xloc+1] = 0x30;
   }
   if (pixel == -5 || pixel == -6 ){

	 xloc= (result_count*5) + 35;
	 yloc= 6;
	 print_array[yloc][xloc] = 0x0C;
	 print_array[yloc][xloc+1] = 0x0C;
   }
   if (pixel == -7 || pixel == -8 ){

	 xloc= (result_count*5) + 35;
	 yloc= 6;
	 print_array[yloc][xloc] = 0x03;
	 print_array[yloc][xloc+1] = 0x03;
   }

   if (pixel == -9 || pixel == -10 ){

	 xloc= (result_count*5) + 35;
	 yloc= 7;
	 print_array[yloc][xloc] = 0xD0;
	 print_array[yloc][xloc+1] = 0xD0;
   }
   if (pixel == -11 || pixel == -12 ){

	 xloc= (result_count*5) + 35;
	 yloc= 7;
	 print_array[yloc][xloc] = 0x38;
	 print_array[yloc][xloc+1] = 0x38;
   }


   /*Line mean - -1SD*/

   if (pixel == -13 || pixel == -14 ){

	 xloc= (result_count*5) + 35;
	 yloc= 7;
	 print_array[yloc][xloc] = 0x1C;
	 print_array[yloc][xloc+1] = 0x1C;
   }
   if (pixel == -15 || pixel == -16 ){

	 xloc= (result_count*5) + 35;
	 yloc= 7;
	 print_array[yloc][xloc] = 0x13;
	 print_array[yloc][xloc+1] = 0x13;
   }

   if (pixel == -17 || pixel == -18 ){

	 xloc= (result_count*5) + 35;
	 yloc= 8;
	 print_array[yloc][xloc] = 0xC1;
	 print_array[yloc][xloc+1] = 0xC1;
   }
   if (pixel == -19 || pixel == -20 ){

	 xloc= (result_count*5) + 35;
	 yloc= 8;
	 print_array[yloc][xloc] = 0x31;
	 print_array[yloc][xloc+1] = 0x31;
   }
   if (pixel == -21 || pixel == -22 ){

	 xloc= (result_count*5) + 35;
	 yloc= 8;
	 print_array[yloc][xloc] = 0x0D;
	 print_array[yloc][xloc+1] = 0x0D;
   }
   if (pixel == -23 || pixel == -24 ){

	 xloc= (result_count*5) + 35;
	 yloc= 8;
	 print_array[yloc][xloc] = 0x03;
	 print_array[yloc][xloc+1] = 0x03;
   }


   /*Line mean -2sd  1*/

   if (pixel == -25 || pixel == -26 ){

	 xloc= (result_count*5) + 35;
	 yloc= 9;
	 print_array[yloc][xloc] = 0xC0;
	 print_array[yloc][xloc+1] = 0xC0;
   }
   if (pixel == -27 || pixel == -28 ){

	 xloc= (result_count*5) + 35;
	 yloc= 9;
	 print_array[yloc][xloc] = 0x30;
	 print_array[yloc][xloc+1] = 0x30;
   }
   if (pixel == -29 || pixel == -30 ){

	 xloc= (result_count*5) + 35;
	 yloc= 9;
	 print_array[yloc][xloc] = 0x0C;
	 print_array[yloc][xloc+1] = 0x0C;
   }
   if (pixel == -31 || pixel == -32 ){

	 xloc= (result_count*5) + 35;
	 yloc= 9;
	 print_array[yloc][xloc] = 0x03;
	 print_array[yloc][xloc+1] = 0x03;
   }

   if (pixel == -33 || pixel == -34 ){

	 xloc= (result_count*5) + 35;
	 yloc= 10;
	 print_array[yloc][xloc] = 0xD0;
	 print_array[yloc][xloc+1] = 0xD0;
   }

   if (pixel == -35 || pixel == -36 ){

	 xloc= (result_count*5) + 35;
	 yloc= 10;
	 print_array[yloc][xloc] = 0x38;
	 print_array[yloc][xloc+1] = 0x38;
   }

   /* Above -3sd*/

   if (pixel == -37 || pixel == -38 ){

	 xloc= (result_count*5) + 35;
	 yloc= 10;
	 print_array[yloc][xloc] = 0x1C;
	 print_array[yloc][xloc+1] = 0x1C;
   }

   if (pixel == -39 || pixel == -40 ){

	 xloc= (result_count*5) + 35;
	 yloc= 10;
	 print_array[yloc][xloc] = 0x13;
	 print_array[yloc][xloc+1] = 0x13;
   }

   if (pixel < -40 ){
	 xloc= (result_count*5) + 35;
	 yloc= 11;
	 print_array[yloc][xloc] = 0x0D;
	 print_array[yloc][xloc+1] = 0x0D;
   }

}

	SendBoldOnOffCMD(true);
	PrintDataOnPrinter((uint8_t*)LJ_plot , sizeof(LJ_plot) , 1000/*Wait delay*/);
	SendBoldOnOffCMD(false);
	PrintDataOnPrinter((uint8_t*)line_height , sizeof(line_height) , 1000/*Wait delay*/);
	PrintDataOnPrinter((uint8_t*)feed , sizeof(feed) , 1000/*Wait delay*/);
    for (y = 0; y < 12; y++)
    {
		for (x = 0; x < 195; x++)
		{
			buffer_array[x]= print_array[y][x];
		}
		PrintDataOnPrinter((uint8_t*)buffer_array , sizeof(buffer_array) , 1000/*Wait delay*/);
		PrintDataOnPrinter((uint8_t*)feed , sizeof(feed) , 1000/*Wait delay*/);
  	}
    PrintDataOnPrinter((uint8_t*)default_line_height , sizeof(default_line_height) , 1000/*Wait delay*/);
    PrintDataOnPrinter((uint8_t*)number , sizeof(number) , 1000/*Wait delay*/);
    PrintDataOnPrinter((uint8_t*)SINGLELINE_SPERATOR , sizeof(SINGLELINE_SPERATOR) , 1000/*Wait delay*/);
    PrintDataOnPrinter((uint8_t*)SEPERATION_STRING , sizeof(SEPERATION_STRING) , 1000/*Wait delay*/);
	Print_Footer();
    PrintDataOnPrinter((uint8_t*)MULTILINE_SPERATOR , sizeof(MULTILINE_SPERATOR) , 1000/*Wait delay*/);
    PrintDataOnPrinter((uint8_t*)SINGLELINE_SPERATOR , sizeof(SINGLELINE_SPERATOR) , 1000/*Wait delay*/);
}

void PrintQcHistResults(float *pData , QCTEST *pQcTest , uint32_t u32NumofData)
{
	uint8_t u8NumOfQcResAvailable = 0;
	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	QCDATA *pQcData = Get_InstaneQcResult();
	char arrTimeBuff[64] = {0};
	char arrPrintBuff[128] = {0};

	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);
	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
	{
		u8QcTestEntryIdx = GetSelectedTestQcRow();
	}
	uint8_t u8ControlIdx = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8QcIdx;

	u8NumOfQcResAvailable = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved;
	if(MAX_NUM_OF_QC_PER_TEST <= u8NumOfQcResAvailable)
	{
		u8NumOfQcResAvailable = MAX_NUM_OF_QC_PER_TEST;
	}
	/* Print Headers */
	Print_Header();
	snprintf(arrPrintBuff , 63 , "%s" , "********************************");
	PrintStringOnPrinter(&arrPrintBuff[0]);
	PrinterFeedLine(1);

	SendBoldOnOffCMD(true);
	snprintf(arrPrintBuff , 63 , "%s" , "Quality Control");
	PrintStringOnPrinter(&arrPrintBuff[0]);
	PrinterFeedLine(1);
	SendBoldOnOffCMD(false);

	snprintf(arrPrintBuff , 63 , "%s : %s" , "   Control Name" , m_QcSetup->m_QcControls[u8ControlIdx].arrControlName);
	PrintStringOnPrinter(&arrPrintBuff[0]);

	snprintf(arrPrintBuff , 63 , "%s   : %s" , "   Lot Number", m_QcSetup->m_QcControls[u8ControlIdx].arRLotNum);
	PrintStringOnPrinter(&arrPrintBuff[0]);

	snprintf(arrPrintBuff , 63 , "%s         : %0.0004f" , "   Mean", m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fMeanVal);
	PrintStringOnPrinter(&arrPrintBuff[0]);

	snprintf(arrPrintBuff , 63 , "%s           : %0.0004f" , "   SD", m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fHighVal);
	PrintStringOnPrinter(&arrPrintBuff[0]);

	snprintf(arrPrintBuff , 63 , "%s" , "--------------------------------");
	PrintStringOnPrinter(&arrPrintBuff[0]);
	PrinterFeedLine(1);

	SendBoldOnOffCMD(true);
	snprintf(arrPrintBuff , 63 , "%s" , "QC Results");
	PrintStringOnPrinter(&arrPrintBuff[0]);
	PrinterFeedLine(1);
	SendBoldOnOffCMD(false);

	if(0 < u8NumOfQcResAvailable)
	{
		for(uint8_t u8Idx = 0 ; (u8Idx) < u8NumOfQcResAvailable ; ++u8Idx)
		{
			snprintf(arrTimeBuff , 63 , "Time:%02d:%02d" ,
					pQcData->m_DateTime[u8QcTestEntryIdx][u8Idx].u16Date_Hour,
					pQcData->m_DateTime[u8QcTestEntryIdx][u8Idx].u16Date_min);

			snprintf(arrPrintBuff , 127 , "%d) Date   :%02d/%02d/%02d %s" , 1 + u8Idx ,
					pQcData->m_DateTime[u8QcTestEntryIdx][u8Idx].u16Date_day,
					pQcData->m_DateTime[u8QcTestEntryIdx][u8Idx].u16Date_Month,
					pQcData->m_DateTime[u8QcTestEntryIdx][u8Idx].u16Date_Year,
					arrTimeBuff);
			PrintStringOnPrinter((char*)arrPrintBuff);

			/*Showing Test Unit*/
			if(u8Idx >= 9)
			{
				if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
				{
					snprintf(arrPrintBuff , 63 , "    Result :%.0004f %s" ,
							pQcData->Res[u8QcTestEntryIdx][u8Idx] ,
							GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
				}
				else
				{
					enUnits ResultUnit = GetInstance_TestParams()[stcTestData.TestId].Unit;
					snprintf(arrPrintBuff , 63 , "    Result :%.0004f %s" ,
							pQcData->Res[u8QcTestEntryIdx][u8Idx] ,
							&arr_cUnitBuffer[ResultUnit][0]);
				}
			}
			else
			{
				if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
				{
					snprintf(arrPrintBuff , 63 , "   Result :%.0004f %s" ,
							pQcData->Res[u8QcTestEntryIdx][u8Idx] ,
							GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
				}
				else
				{
					enUnits ResultUnit = GetInstance_TestParams()[stcTestData.TestId].Unit;
					snprintf(arrPrintBuff , 63 , "   Result :%.0004f %s" ,
							pQcData->Res[u8QcTestEntryIdx][u8Idx] ,
							&arr_cUnitBuffer[ResultUnit][0]);
				}
			}
			PrintStringOnPrinter((char*)arrPrintBuff);
		}

	}
	snprintf(arrPrintBuff , 63 , "%s" , "--------------------------------");
	PrintStringOnPrinter(&arrPrintBuff[0]);
	PrinterFeedLine(1);

	/*Showing print name*/
	SendBoldOnOffCMD(true);
	PrintStringOnPrinter((char*)"LAB SUMMARY");
	SendBoldOnOffCMD(false);
	PrinterFeedLine(1);

	/*Calculating mean values for number of test results*/
	float fLabMean = 0;
	float fLabSd = 0;
	float fLabCv = 0;
	float fSdSum = 0;
	for(uint8_t u8Count = 0; u8Count < u8NumOfQcResAvailable; ++u8Count)
	{
		fLabMean += pQcData->Res[u8QcTestEntryIdx][u8Count];
	}
	fLabMean /= u8NumOfQcResAvailable;

	for(uint8_t u8Count = 0; u8Count < u8NumOfQcResAvailable; ++u8Count)
	{
		fSdSum += (pQcData->Res[u8QcTestEntryIdx][u8Count] - fLabMean) *
				(pQcData->Res[u8QcTestEntryIdx][u8Count] - fLabMean);
	}

	fLabSd = sqrt(fSdSum / (u8NumOfQcResAvailable-1));
	fLabCv = (fLabSd / fLabMean)*100;

	/*Showing L.Mean*/
	snprintf(arrPrintBuff , 63 , "   L.Mean : %.0004f" , fLabMean);
	PrintStringOnPrinter(&arrPrintBuff[0]);

	/*Showing L.SD*/
	snprintf(arrPrintBuff , 63 , "   L.SD   : %.0004f" , fLabSd);
	PrintStringOnPrinter(&arrPrintBuff[0]);

	/*Showing CV*/
	snprintf(arrPrintBuff , 63 , "   CV     : %.0004f" , fLabCv);
	PrintStringOnPrinter(&arrPrintBuff[0]);

	/*Showing N*/
	snprintf(arrPrintBuff , 63 , "   N      : %u" , u8NumOfQcResAvailable);
	PrintStringOnPrinter(&arrPrintBuff[0]);
	PrinterFeedLine(1);
}
bool StatusBarUpdateMillsDelay(void)
{
  #define DELAY_MS (1000)
  unsigned long currentMillis = HAL_GetTick();
  static unsigned long PrevMills = 0;/*Initilized one time only*/
  int elapsedTime = (int)(currentMillis - PrevMills);
  if(abs((int)elapsedTime) >= DELAY_MS)
  {
   PrevMills = currentMillis;
   return true;
  }
  return false;
}
