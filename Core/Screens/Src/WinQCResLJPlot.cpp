/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/NVM_QCSetup.hpp"
#include "../../APPL/Inc/NVM_TestParameters.hpp"

#define LJ_GRAPH_PIXEL_MIN_X (65)
#define LJ_GRAPH_PIXEL_MIN_Y (180)
#define LJ_GRAPH_PIXEL_X_WIDTH (400)
#define LJ_GRAPH_PIXEL_Y_HEIGHT (210)
#define LJ_GRAPH_PIXEL_MAX_X (LJ_GRAPH_PIXEL_MIN_X + LJ_GRAPH_PIXEL_X_WIDTH)
#define LJ_GRAPH_PIXEL_MAX_Y (LJ_GRAPH_PIXEL_MIN_Y + LJ_GRAPH_PIXEL_Y_HEIGHT)

#define MAX_SD_AVAILABLE (3)

typedef enum
{
	en1sd = 0,
	en2sd = 1,
	en3sd = 2,
}SD;

float g_fPSdVal[MAX_SD_AVAILABLE] = {0};
float g_fMSdVal[MAX_SD_AVAILABLE] = {0};
float g_fPStandardDeviation;
float g_fMStandardDeviation;
extern bool e_QCTestDoneflag;

uint16_t u16MainCentralYAxis = (LJ_GRAPH_PIXEL_MIN_Y + LJ_GRAPH_PIXEL_MAX_Y)/2;
uint16_t u16YAxisP3s;
uint16_t u16YAxisP2s;
uint16_t u16YAxisP1s;
uint16_t u16YAxisMean;
uint16_t u16YAxisM1s;
uint16_t u16YAxisM2s;
uint16_t u16YAxisM3s;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bQCMenuBack = NexButton(en_WinID_LJ_Plot , 1, "b5");
static NexButton bQCRunMenuBack = NexButton(en_WinID_LJ_Plot , 52, "b4");

static NexButton bQCSummary = NexButton(en_WinID_LJ_Plot , 13, "b0");
static NexButton bPrintQcLjPlot = NexButton(en_WinID_LJ_Plot , 4, "b7");
static NexButton bQCLJAccept = NexButton(en_WinID_LJ_Plot , 42, "b2");
static NexButton bQCLJDecline= NexButton(en_WinID_LJ_Plot , 43, "b3");
//static NexButton bQCLJPrint= NexButton(en_WinID_LJ_Plot , 4, "b7");

static NexText tLastQCResText = NexText(en_WinID_LJ_Plot , 48, "t32");
static NexText tLastQCRes = NexText(en_WinID_LJ_Plot , 49, "t33");
static NexText tQCResUnit = NexText(en_WinID_LJ_Plot , 50, "t34");

static NexText tTestName = NexText(en_WinID_LJ_Plot , 5, "t1");
static NexText tCntrlName = NexText(en_WinID_LJ_Plot , 7, "t0");
static NexText tLotNum = NexText(en_WinID_LJ_Plot , 9, "t2");
static NexText tExpDate = NexText(en_WinID_LJ_Plot , 11, "t3");
static NexText tNumofResults = NexText(en_WinID_LJ_Plot , 23, "t11");

static NexText tMeanText = NexText(en_WinID_LJ_Plot , 46, "t30");
static NexText tSDText = NexText(en_WinID_LJ_Plot , 47, "t31");

static NexText tRule1_2S = NexText(en_WinID_LJ_Plot , 34, "t22");
static NexText tRule1_3S = NexText(en_WinID_LJ_Plot , 35, "t23");
static NexText tRule2_2S = NexText(en_WinID_LJ_Plot , 36, "t24");
static NexText tRuleR_4S = NexText(en_WinID_LJ_Plot , 37, "t25");
static NexText tRule4_1S = NexText(en_WinID_LJ_Plot , 38, "t26");
static NexText tRule8X = NexText(en_WinID_LJ_Plot , 39, "t27");
static NexText tRule10X = NexText(en_WinID_LJ_Plot , 40, "t28");
static NexText tRule12X = NexText(en_WinID_LJ_Plot , 41, "t29");

static NexText tTitleRule1_2S = NexText(en_WinID_LJ_Plot , 24, "t12");
static NexText tTitleRule1_3S = NexText(en_WinID_LJ_Plot , 25, "t13");
static NexText tTitleRule2_2S = NexText(en_WinID_LJ_Plot , 26, "t14");
static NexText tTitleRuleR_4S = NexText(en_WinID_LJ_Plot , 27, "t15");
static NexText tTitleRule4_1S = NexText(en_WinID_LJ_Plot , 28, "t16");
static NexText tTitleRule8X = NexText(en_WinID_LJ_Plot , 29, "t17");
static NexText tTitleRule10X = NexText(en_WinID_LJ_Plot , 30, "t18");
static NexText tTitleRule12X = NexText(en_WinID_LJ_Plot , 31, "t19");


static NexPicture pLJplot = NexPicture(en_WinID_LJ_Plot , 51 , "p9");


/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bQCMenuBack,
								 	 &bQCSummary,
									 &bPrintQcLjPlot,
									 &bQCLJAccept,
									 &bQCLJDecline,
									 &bQCRunMenuBack,
									 NULL};

static void HandlerbQCMenuBack(void *ptr);
static void HandlerbQCRunMenuBack(void *ptr);
static void HandlerShowQcSummary(void *ptr);
static void HandlerPrintLjPlot(void *ptr);
static void ShowHideQCLJPlotAcceptRejectButton(void);
static void HandlerbQCLJAccept(void *ptr);
static void HandlerbQCLJDecline(void *ptr);
static void DispalyLJPlot(uint8_t u8N);
static void westgards_rules(QCDATA *pQcData, uint8_t u8NumOfAnalysis);

int check_violations_2s(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd);
int check_violations_3s(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd);
int check_violations_2_2s(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd);
int check_violations_R4s(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd);
int check_violations_4_1s(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd);
int check_violations_8x(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd);
int check_violations_10x(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd);
int check_violations_12x(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd);

enWindowStatus ShowQCResLJPlotScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bQCMenuBack.attachPush(HandlerbQCMenuBack, &bQCMenuBack);
	bQCRunMenuBack.attachPush(HandlerbQCRunMenuBack, &bQCRunMenuBack);
	bQCSummary.attachPush(HandlerShowQcSummary, &bQCSummary);
	bPrintQcLjPlot.attachPush(HandlerPrintLjPlot, &bPrintQcLjPlot);
	bQCLJAccept.attachPush(HandlerbQCLJAccept, &bQCLJAccept);
	bQCLJDecline.attachPush(HandlerbQCLJDecline, &bQCLJDecline);


	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);

	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
	{
		u8QcTestEntryIdx = GetSelectedTestQcRow();
		bQCRunMenuBack.setVisible(true);
	}
	else
	{
		bQCMenuBack.setVisible(true);
	}

	if(NVM_INIT_NOT_OK_FLAG != u8QcTestEntryIdx)
	{
		uint8_t u8ControlIdx = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8QcIdx;
		tTestName.setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);
		tCntrlName.setText(m_QcSetup->m_QcControls[u8ControlIdx].arrControlName);
		tLotNum.setText(m_QcSetup->m_QcControls[u8ControlIdx].arRLotNum);

		char arrBuff[64] = {0};

		/*Calculate SDs*/
		float fMean = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fMeanVal;
		g_fPStandardDeviation =	(m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fHighVal);  // This is +1SD Value
		g_fMStandardDeviation =	(m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fHighVal);  // This is -1SD Value

		for(uint8_t u8Idx = 0 ; u8Idx < MAX_SD_AVAILABLE ; ++u8Idx)
		{
			g_fPSdVal[u8Idx] = fMean + ((u8Idx + 1) * g_fPStandardDeviation);
			g_fMSdVal[u8Idx] = fMean - ((u8Idx + 1) * g_fMStandardDeviation);
		}

		snprintf(arrBuff , 63 , "%02d/%02d/%02d" , m_QcSetup->m_QcControls[u8ControlIdx].m_ExpiryDate.Date,
				m_QcSetup->m_QcControls[u8ControlIdx].m_ExpiryDate.Month,
				m_QcSetup->m_QcControls[u8ControlIdx].m_ExpiryDate.Year);
		tExpDate.setText(arrBuff);

		snprintf(arrBuff , 63 , "MEAN : %.02f" , m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fMeanVal);
		tMeanText.setText(arrBuff);

		snprintf(arrBuff , 63 , "SD : %.02f" , m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fHighVal);
		tSDText.setText(arrBuff);

		snprintf(arrBuff , 63 ,"N = %d",
				m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved);
		tNumofResults.setText(arrBuff);
		/*Main central axis*/
		u16MainCentralYAxis = (LJ_GRAPH_PIXEL_MIN_Y + LJ_GRAPH_PIXEL_MAX_Y)/2;

		u16YAxisP3s = (LJ_GRAPH_PIXEL_MIN_Y +
								(LJ_GRAPH_PIXEL_MIN_Y + u16MainCentralYAxis)/2)/2;

		u16YAxisP2s = (LJ_GRAPH_PIXEL_MIN_Y + u16MainCentralYAxis)/2;

		u16YAxisP1s = (u16MainCentralYAxis +
								(LJ_GRAPH_PIXEL_MIN_Y + u16MainCentralYAxis)/2)/2;
		u16YAxisMean = u16MainCentralYAxis;

		u16YAxisM1s = (u16MainCentralYAxis +
								(LJ_GRAPH_PIXEL_MAX_Y + u16MainCentralYAxis)/2)/2;

		u16YAxisM2s = (LJ_GRAPH_PIXEL_MAX_Y + u16MainCentralYAxis)/2;

		u16YAxisM3s = (LJ_GRAPH_PIXEL_MAX_Y +
								(LJ_GRAPH_PIXEL_MAX_Y + u16MainCentralYAxis)/2)/2;

		DrawLineOnGraph(en_WinID_LJ_Plot,LJ_GRAPH_PIXEL_MIN_X,
				u16YAxisMean,
				LJ_GRAPH_PIXEL_MAX_X,
				u16YAxisMean,
				COLOUR_BLACK);

		DrawLineOnGraph(en_WinID_LJ_Plot,LJ_GRAPH_PIXEL_MIN_X,
				u16YAxisP1s,
				LJ_GRAPH_PIXEL_MAX_X,
				u16YAxisP1s,
				COLOUR_GREEN);

		DrawLineOnGraph(en_WinID_LJ_Plot,LJ_GRAPH_PIXEL_MIN_X,
				u16YAxisP2s,
				LJ_GRAPH_PIXEL_MAX_X,
				u16YAxisP2s,
				COLOUR_YELLOW);

		DrawLineOnGraph(en_WinID_LJ_Plot,LJ_GRAPH_PIXEL_MIN_X,
				u16YAxisP3s,
				LJ_GRAPH_PIXEL_MAX_X,
				u16YAxisP3s,
				COLOUR_RED);

		DrawLineOnGraph(en_WinID_LJ_Plot,LJ_GRAPH_PIXEL_MIN_X,
				u16YAxisM1s,
				LJ_GRAPH_PIXEL_MAX_X,
				u16YAxisM1s,
				COLOUR_GREEN);

		DrawLineOnGraph(en_WinID_LJ_Plot,LJ_GRAPH_PIXEL_MIN_X,
				u16YAxisM2s,
				LJ_GRAPH_PIXEL_MAX_X,
				u16YAxisM2s,
				COLOUR_YELLOW);

		DrawLineOnGraph(en_WinID_LJ_Plot,LJ_GRAPH_PIXEL_MIN_X,
				u16YAxisM3s,
				LJ_GRAPH_PIXEL_MAX_X,
				u16YAxisM3s,
				COLOUR_RED);

//		DrawGraphRectangle(en_WinID_LJ_Plot , LJ_GRAPH_PIXEL_MIN_X , LJ_GRAPH_PIXEL_MIN_Y,
//												LJ_GRAPH_PIXEL_MAX_X , LJ_GRAPH_PIXEL_MAX_Y);

		DispalyLJPlot(m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved);
	}

	ShowHideQCLJPlotAcceptRejectButton();

	return WinStatus;
}

void HandlerPrintLjPlot(void *ptr)
{
	BeepBuzzer();
	bPrintQcLjPlot.setVisible(false);

	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	QCDATA *pQcData = Get_InstaneQcResult();

	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);
	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
	{
		u8QcTestEntryIdx = GetSelectedTestQcRow();
	}

	ReadQcResData(stcTestData.TestId , pQcData);
	uint8_t u8NumOfQcResAvailable =
			m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved;

	PrintQcHistResults(&pQcData->Res[u8QcTestEntryIdx][0] ,
			&m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx] ,
			u8NumOfQcResAvailable);

	PrintQcLJPlot(&pQcData->Res[u8QcTestEntryIdx][0] ,
			&m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx] ,
			u8NumOfQcResAvailable);

	bPrintQcLjPlot.setVisible(true);
}

void HandlerQCResLJPlotScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);

	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	uint8_t u8LastSelRow = 0;
	uint8_t u8QcTestEntryIdx = 0;
	uint8_t u8NumOfQcResAvailable = 0;
	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId ||
			en_WinID_TestSelectionScreen == stcScreenNavigation.PrevWindowId)
	{
		u8QcTestEntryIdx = GetSelectedTestQcRow();
		if(NVM_INIT_NOT_OK_FLAG == u8QcTestEntryIdx)
		{
			u8NumOfQcResAvailable = 0;
		}
		else
		{
			u8NumOfQcResAvailable = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved;
		}
	}
	else if(en_WinID_QcTestScreen == stcScreenNavigation.PrevWindowId)
	{
		u8LastSelRow = GetQcTestLastSelectedRow();
		u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);
		u8NumOfQcResAvailable = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved;
	}

	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId ||
			en_WinID_TestSelectionScreen == stcScreenNavigation.PrevWindowId ||
			en_WinID_QcTestScreen == stcScreenNavigation.PrevWindowId)
	{
		if(0 >= u8NumOfQcResAvailable)
		{
		/*No QC found*/
			if(enkeyOk == ShowMainPopUp("QC Results","No Results Found", true))
			{
				if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
				{
					ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
				}
				else
				{
					ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
				}
			}
		}
	}
}

void HandlerShowQcSummary(void *ptr)
{
	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
	{
		ChangeWindowPage(en_WinID_Qc_Res_Summary , en_WinId_TestRun);
		stcScreenNavigation.PrevWindowId = en_WinId_TestRun;
	}
	else
	{
		ChangeWindowPage(en_WinID_Qc_Res_Summary , en_WinID_QcTestScreen);
		stcScreenNavigation.PrevWindowId = en_WinID_QcTestScreen;
	}
	BeepBuzzer();
}

void HandlerbQCMenuBack(void *ptr)
{
//	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
//	{
//		ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
////		stcScreenNavigation.PrevWindowId = en_WinID_TestSelectionScreen;
//	}
//	else
//	{
//		ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
//	}
	ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
	BeepBuzzer();
}
static void HandlerbQCRunMenuBack(void *ptr)
{
	ChangeWindowPage(en_WinId_TestRun ,  (enWindowID)NULL);
	BeepBuzzer();
}

void DispalyLJPlot(uint8_t u8N)
{
	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);
	uint32_t Colour = 0;
	char arrBuff[64] = {0};

	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
	{
		u8QcTestEntryIdx = GetSelectedTestQcRow();
	}

	QCDATA *pQcData = Get_InstaneQcResult();
	ReadQcResData(stcTestData.TestId , pQcData);

	/* Displaying last run result to accept or decline*/
	snprintf(arrBuff , 63 , "%0.01f" ,pQcData->Res[u8QcTestEntryIdx][u8N-1]);

	tLastQCRes.setText(arrBuff);

	if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
	{
		snprintf(arrBuff , 63 , "%s" ,GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
	}
	else
	{
		snprintf(arrBuff , 63 , "%s" ,&arr_cUnitBuffer[GetInstance_TestParams()[stcTestData.TestId].Unit][0]);
	}
	tQCResUnit.setText(arrBuff);
	uint8_t radius = 3;
	if(u8N > 1)
	{
		westgards_rules(pQcData , u8N);
	}
	for(uint8_t u8Idx = 0 ; u8Idx < u8N ; ++u8Idx)
	{
		if(pQcData->Res[u8QcTestEntryIdx][u8Idx] < g_fPSdVal[en1sd] && pQcData->Res[u8QcTestEntryIdx][u8Idx] > g_fMSdVal[en1sd])
		{
			Colour = COLOUR_BLACK ;
		}
		else if((pQcData->Res[u8QcTestEntryIdx][u8Idx] < g_fPSdVal[en2sd] && pQcData->Res[u8QcTestEntryIdx][u8Idx] > g_fPSdVal[en1sd]) ||
				(pQcData->Res[u8QcTestEntryIdx][u8Idx] > g_fMSdVal[en2sd] && pQcData->Res[u8QcTestEntryIdx][u8Idx] < g_fMSdVal[en1sd]))
		{
			Colour = COLOUR_GREEN ;
		}
		else if((pQcData->Res[u8QcTestEntryIdx][u8Idx] < g_fPSdVal[en3sd] && pQcData->Res[u8QcTestEntryIdx][u8Idx] > g_fPSdVal[en2sd]) ||
				(pQcData->Res[u8QcTestEntryIdx][u8Idx] > g_fMSdVal[en3sd] && pQcData->Res[u8QcTestEntryIdx][u8Idx] < g_fMSdVal[en2sd]))
		{
			Colour = COLOUR_YELLOW ;
		}
		else
		{
			Colour = COLOUR_RED ;
		}
		if(0 == u8Idx)
		{
			DrawGraphFilledCircle(en_WinID_LJ_Plot ,
					(u8Idx),
					pQcData->Res[u8QcTestEntryIdx][u8Idx],
					0/*X min value*/,
					(u8N - 1)/*X max value*/,
					g_fMSdVal[en3sd]/*Y min value*/,
					g_fPSdVal[en3sd]/*Y max value*/,
					LJ_GRAPH_PIXEL_MIN_X/*Graph X*/,
					u16YAxisP3s/*Graph Y*/,
					LJ_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					u16YAxisM3s/*Graph Max Y*/,
					radius, Colour);
		}
		else
		{
			UpdateGraph(en_WinID_LJ_Plot /*Window ID*/,
					(u8Idx - 1)/*X1*/,
					pQcData->Res[u8QcTestEntryIdx][u8Idx - 1] /*Y1*/,
					(u8Idx)/*X2*/,
					pQcData->Res[u8QcTestEntryIdx][u8Idx] /*Y2*/,
					0/*X min value*/,
					(u8N - 1)/*X max value*/,
					g_fMSdVal[en3sd]/*Y min value*/,
					g_fPSdVal[en3sd]/*Y max value*/,
					LJ_GRAPH_PIXEL_MIN_X/*Graph X*/,
					u16YAxisP3s/*Graph Y*/,
					LJ_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					u16YAxisM3s/*Graph Max Y*/,
					COLOUR_BLUE);


			DrawGraphFilledCircle(en_WinID_LJ_Plot ,
					(u8Idx),
					pQcData->Res[u8QcTestEntryIdx][u8Idx],
					0/*X min value*/,
					(u8N - 1)/*X max value*/,
					g_fMSdVal[en3sd]/*Y min value*/,
					g_fPSdVal[en3sd]/*X max value*/,
					LJ_GRAPH_PIXEL_MIN_X/*Graph X*/,
					u16YAxisP3s/*Graph Y*/,
					LJ_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
					u16YAxisM3s/*Graph Max Y*/,
					radius, Colour);
		}
	}
}


void westgards_rules(QCDATA *pQcData, uint8_t u8NumOfAnalysis)
{
    // Initialize variables to count violations
	uint8_t violations = 0;

	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);

	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
	{
		u8QcTestEntryIdx = GetSelectedTestQcRow();
	}

	float mean = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fMeanVal;

	float Pstd =	(m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fHighVal);
	float Mstd =	(m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fHighVal);

    // Iterate through measurements and check for violations
	float measurements[MAX_NUM_OF_QC_PER_TEST] = {0};
	memcpy(measurements , &pQcData->Res[u8QcTestEntryIdx][0] , sizeof(measurements));

	violations = check_violations_2s(measurements, u8NumOfAnalysis, mean, Pstd, Mstd);
	    if (violations > 0) {

	        // Print violations for Rule 1: 1 3s
	    	char arrBuff[64] = {0};
	    	tTitleRule1_2S.setVisible(true);
	        snprintf(arrBuff , 63 , "%d" , violations);
	        tRule1_2S.setText(arrBuff);

	        violations = check_violations_3s(measurements, u8NumOfAnalysis, mean, Pstd, Mstd);
	        if (violations > 0) {
	            // Print violations for Rule 2: 1 2s
		    	tTitleRule1_3S.setVisible(true);
	            snprintf(arrBuff , 63 , "%d" , violations);
	            tRule1_3S.setText(arrBuff);
//	            return;
	        }

	        violations = check_violations_2_2s(measurements, u8NumOfAnalysis, mean, Pstd, Mstd);
	        if (violations > 0) {
	            // Print violations for Rule 3: 2 2s
		    	tTitleRule2_2S.setVisible(true);
	            snprintf(arrBuff , 63 , "%d" , violations);
	            tRule2_2S.setText(arrBuff);
//	            return;
	        }

	        violations = check_violations_R4s(measurements, u8NumOfAnalysis, mean, Pstd, Mstd);
	        if (violations > 0) {
	            // Print violations for Rule 4: R 4s
		    	tTitleRuleR_4S.setVisible(true);
	            snprintf(arrBuff , 63 , "%d" , violations);
	            tRuleR_4S.setText(arrBuff);
//	            return;
	        }

	        violations = check_violations_4_1s(measurements, u8NumOfAnalysis, mean, Pstd, Mstd);
	        if (violations > 0) {
	            // Print violations for Rule 5: 4 1s
		    	tTitleRule4_1S.setVisible(true);
	            snprintf(arrBuff , 63 , "%d" , violations);
	            tRule4_1S.setText(arrBuff);
//	            return;
	        }

	        violations = check_violations_8x(measurements, u8NumOfAnalysis, mean, Pstd, Mstd);
	        if (violations > 0) {
	            // Print violations for Rule 7: 8x
		    	tTitleRule8X.setVisible(true);
	            snprintf(arrBuff , 63 , "%d" , violations);
	            tRule8X.setText(arrBuff);
//	            return;
	        }

	        violations = check_violations_10x(measurements, u8NumOfAnalysis, mean, Pstd, Mstd);
	        if (violations > 0) {
	            // Print violations for Rule 6: 10x
		    	tTitleRule10X.setVisible(true);
	            snprintf(arrBuff , 63 , "%d" , violations);
	            tRule10X.setText(arrBuff);
//	            return;
	        }

	        violations = check_violations_12x(measurements, u8NumOfAnalysis, mean, Pstd, Mstd);
	        if (violations > 0) {
	            // Print violations for Rule 8: 12x
		    	tTitleRule12X.setVisible(true);
	            snprintf(arrBuff , 63 , "%d" , violations);
	            tRule12X.setText(arrBuff);
//	            return;
	        }
	    }
}



int check_violations_2s(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd) {
    int violations = 0;
    for (int i = 0; i < NumOfRes; i++) {
        if (measurements[i] > mean + 2 * Pstd || measurements[i] < mean - 2 * Mstd) {
            violations++;
        }
    }
    return violations;
}

int check_violations_3s(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd) {
    int violations = 0;
    for (int i = 0; i < NumOfRes; i++) {
        if (measurements[i] > mean + 3 * Pstd || measurements[i] < mean - 3 * Mstd) {
            violations++;
        }
    }
    return violations;
}

int check_violations_2_2s(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd) {
    int violations = 0;
    for (int i = 1; i < NumOfRes; i++) {
        if ((measurements[i-1] > mean + 2 * Pstd && measurements[i] > mean + 2 * Pstd) ||
            (measurements[i-1] < mean - 2 * Mstd && measurements[i] < mean - 2 * Mstd)) {
            violations++;
        }
    }
    return violations;
}

int check_violations_R4s(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd) {
    int violations = 0;
    for (int i = 1; i < NumOfRes; i++) {
        if ((measurements[i-1] > mean + 2 * Pstd && measurements[i] < mean - 2 * Mstd) ||
            (measurements[i-1] < mean - 2 * Mstd && measurements[i] > mean + 2 * Pstd)) {
            violations++;
        }
    }
    return violations;
}

int check_violations_4_1s(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd) {
    int violations = 0;
    int consecutive_up = 0;
    int consecutive_down = 0;
    for (int i = 0; i < NumOfRes; i++)
    {
        if (measurements[i] > mean + Pstd) {
            consecutive_up++;
            consecutive_down = 0;
        } else if (measurements[i] < mean - Mstd) {
            consecutive_down++;
            consecutive_up = 0;
        } else {
            consecutive_up = 0;
            consecutive_down = 0;
        }
    }
        if (consecutive_up >= 4 || consecutive_down >= 4)
        {
            violations++;
            consecutive_up = 0;
            consecutive_down = 0;
        }

    return violations;
}

int check_violations_8x(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd) {
    int violations = 0;
    int consecutive_up = 0;
    int consecutive_down = 0;
    for (int i = 0; i < NumOfRes; i++)
    {
        if (measurements[i] > mean) {
            consecutive_up++;
            consecutive_down = 0;
        } else if (measurements[i] < mean) {
            consecutive_down++;
            consecutive_up = 0;
        } else {
            consecutive_up = 0;
            consecutive_down = 0;
        }
    }
        if (consecutive_up >= 8 || consecutive_down >= 8)
        {
            violations++;
            consecutive_up = 0;
            consecutive_down = 0;
        }

    return violations;
}

int check_violations_10x(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd) {
    int violations = 0;
    int consecutive_up = 0;
    int consecutive_down = 0;
    for (int i = 0; i < NumOfRes; i++)
    {
        if (measurements[i] > mean) {
            consecutive_up++;
            consecutive_down = 0;
        } else if (measurements[i] < mean) {
            consecutive_down++;
            consecutive_up = 0;
        } else {
            consecutive_up = 0;
            consecutive_down = 0;
        }
    }
        if (consecutive_up >= 10 || consecutive_down >= 10)
        {
            violations++;
            consecutive_up = 0;
            consecutive_down = 0;
        }

    return violations;
}

int check_violations_12x(float* measurements, uint8_t NumOfRes,  float mean, float Pstd, float Mstd) {
    int violations = 0;
    int consecutive_up = 0;
    int consecutive_down = 0;
    for (int i = 0; i < NumOfRes; i++)
    {
        if (measurements[i] > mean) {
            consecutive_up++;
            consecutive_down = 0;
        } else if (measurements[i] < mean) {
            consecutive_down++;
            consecutive_up = 0;
        } else {
            consecutive_up = 0;
            consecutive_down = 0;
        }
    }
        if (consecutive_up >= 12 || consecutive_down >= 12)
        {
            violations++;
            consecutive_up = 0;
            consecutive_down = 0;
        }

    return violations;
}

void ShowHideQCLJPlotAcceptRejectButton(void)
{
	if(true == e_QCTestDoneflag)
	{
		bQCLJAccept.setVisible(true);
		bQCLJDecline.setVisible(true);
		tLastQCResText.setVisible(true);
		tLastQCRes.setVisible(true);
		tQCResUnit.setVisible(true);
		bQCRunMenuBack.setVisible(false);

	}
	else
	{
		bQCLJAccept.setVisible(false);
		bQCLJDecline.setVisible(false);
		tLastQCResText.setVisible(false);
		tLastQCRes.setVisible(false);
		tQCResUnit.setVisible(false);
	}
}

void HandlerbQCLJAccept(void *ptr)
{
	e_QCTestDoneflag = false;
//	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	ChangeWindowPage(en_WinId_TestRun ,  (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbQCLJDecline(void *ptr)
{
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);

	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
	{
		u8QcTestEntryIdx = GetSelectedTestQcRow();
	}
	e_QCTestDoneflag = false;
	m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved--;
	NVM_WriteQcSetup();
//	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	ChangeWindowPage(en_WinId_TestRun ,  (enWindowID)NULL);
	BeepBuzzer();
}



