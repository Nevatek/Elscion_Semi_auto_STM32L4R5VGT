/*
 * CommonDisplayFunctions.h
 *
 *  Created on: May 20, 2023
 *      Author: Alvin
 */

#ifndef SCREENS_INC_COMMONDISPLAYFUNCTIONS_HPP_
#define SCREENS_INC_COMMONDISPLAYFUNCTIONS_HPP_

#include "../libraries/Nextion/Nextion.h"
#include "../../Screens/Inc/Screens.h"
#include "../APPL/Inc/NVM_QCSetup.hpp"

//#define MAX_LEN_UNIT_BUFFER_VAL (8)
/*All unit strings are defined here*/
extern char arr_cUnitBuffer[en_MaxUnits][8];
extern char g_arrTestmethodnameBuffer[en_TestProdecureMax][20];
extern char g_arrFilterNames[en_FilterMax][8];
extern char g_arrFilterNames_OpticsDiag[en_FilterMax][32];
//extern float e_TemperatureTargetList[3];
extern char g_CalibrationTypeNameBUffer[en_CalibrationMax][24];
extern char g_arrCuvveteNameBuffer[en_MaxCuvetteType][24];


typedef enum
{
	enTestPopUp_Null,
	enTestPopUp_Abort,
	enTestPopUp_Error,
	enTestPopUp_DarkADCError,
	enTestPopUp_fitlerwheelError,
	enTestPopUp_limitsoutofrange,
	enTestPopUp_Max,
}en_TestPopUp;

typedef enum
{
	enNouser = 0,
	enAdminuser,
	enServiceUser,
	enNormaluser
}enUserType;

typedef enum
{
	enTestPrograms = 0,
	enTestResults,
}enMemoryEraiseType;



void ShowHideTestScreenPopup(en_TestPopUp Popup , std::string Msg);

//void UpdateTestCreenDateTime(NexText *ptrTxt);
void UpdateHomeSreenDateTimeTemp(NexText *ptrDateTimeTxt, NexText *pTxtFlowTemp, NexText *pTxtIncuTemp, NexText *pTxtUser);

void UpdateStatusBar(NexPicture *pPicTestType , NexText *pTestName , NexText *pTestWavelength ,
		NexText *pTxtDate, NexText *pTxtTime, NexText *pTxtPelTemp ,NexText *pTxtIncbTemp, NexText *pTxtUser);

void ClearGraph(enWindowID ID_Win , float X1 , float Y1 ,float X2 , float Y2);
void DrawGraphRectangle(enWindowID ID_Win , float X1 , float Y1 ,float X2 , float Y2);
void UpdateGraph(enWindowID ID_Win , float X1 , float Y1 ,float X2 , float Y2 ,
				float XvalMin , float XvalMax , float YvalMin , float YvalMax ,
				uint16_t GraphX , uint16_t GraphY , uint16_t GraphWidth , uint16_t GraphHeight ,
				uint32_t Colour);
void DrawGraphFilledCircle(enWindowID ID_Win , float X1 , float Y1,
		float XvalMin , float XvalMax,float YvalMin , float YvalMax ,
		uint16_t GraphX , uint16_t GraphY , uint16_t GraphWidth , uint16_t GraphHeight ,
		uint16_t radius, uint32_t Colour);

void UpdateGraphAxisData(NexText *pXMax , NexText *pYMax , float XvalMax , float YvalMax,
		NexText *pXMin , NexText *pYMin , float XvalMin , float YvalMin);
void DrawVerticalLineOnGraph(enWindowID ID_Win , float X , float XvalMin , float XvalMax);
void DrawLineOnGraph(enWindowID ID_Win , uint16_t X1 ,
		uint16_t Y1 , uint16_t X2 , uint16_t Y2 , uint16_t u16Col);
float mapFloat(float value, float fromLow, float fromHigh, float toLow, float toHigh) ;
long mapInt(long x, long in_min, long in_max, long out_min, long out_max) ;
uint16_t FindMaxADCValueFromList(uint16_t *ptrArray , uint16_t Start, uint16_t Length);
float FindMaxValueFromList(float *ptrArray , uint16_t Length);
float FindMinValueFromList(float *ptrArray , uint16_t Length);
bool CompareBuffer(float *ptrBuff1 , float *ptrBuff2 , size_t Length);
bool StatusBarUpdateMillsDelay(void);
uint8_t MaxNumOfStandardsForSelectedMethod(enCalibrationType CalibType);
uint8_t MinNumOfStandardsForSelectedMethod(enCalibrationType CalibType);
void DrawRescaledGraph(void);
void UpdateRealTimeBuffers(enMeasurementType CurrentMeasType);
void UpdateTestScreenMenu(void);
void HideButtonBackground(bool visible);
void ShowHideInitializePopup(bool visible, std::string Title , std::string Msg);
void ClearTestSavedValues(void);
float Calculate_Percentage(float val , float TotalVal);
void PrintTestParams(uint16_t u16TestId);
void PrintQcLJPlot(float *pData , QCTEST *pQcTest , uint32_t u32NumofData);
void PrintQcHistResults(float *pData , QCTEST *pQcTest , uint32_t u32NumofData);
#endif /* SCREENS_INC_COMMONDISPLAYFUNCTIONS_HPP_ */
