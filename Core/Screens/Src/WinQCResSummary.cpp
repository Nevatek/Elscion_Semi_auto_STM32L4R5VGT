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
#include "math.h"

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bQCMenuBack = NexButton(en_WinID_Qc_Res_Summary , 1, "b5");
static NexButton bQCRunMenuBack = NexButton(en_WinID_Qc_Res_Summary , 43, "b4");
static NexButton bQCLJ = NexButton(en_WinID_Qc_Res_Summary , 14, "b1");

static NexButton bQCAccept = NexButton(en_WinID_Qc_Res_Summary , 35, "b2");
static NexButton bQCDecline= NexButton(en_WinID_Qc_Res_Summary , 36, "b3");
//static NexButton bQCPrint= NexButton(en_WinID_Qc_Res_Summary , 4, "b7");

static NexText tLastQCResText = NexText(en_WinID_Qc_Res_Summary , 41, "t32");
static NexText tLastQCRes = NexText(en_WinID_Qc_Res_Summary , 42, "t33");

static NexText tLabMean = NexText(en_WinID_Qc_Res_Summary , 26, "t13");
static NexText tLabMin = NexText(en_WinID_Qc_Res_Summary , 27, "t14");
static NexText tLabMax = NexText(en_WinID_Qc_Res_Summary , 29, "t16");
static NexButton bPrintQcLjPlot = NexButton(en_WinID_Qc_Res_Summary , 4, "b7");

static NexText tCV = NexText(en_WinID_Qc_Res_Summary , 28, "t15");
static NexText tSD = NexText(en_WinID_Qc_Res_Summary , 31, "t18");
static NexText tN = NexText(en_WinID_Qc_Res_Summary , 30, "t17");

static NexText tMean = NexText(en_WinID_Qc_Res_Summary , 32, "t19");
static NexText tMin = NexText(en_WinID_Qc_Res_Summary , 33, "t20");
static NexText tMax = NexText(en_WinID_Qc_Res_Summary , 34, "t21");

static NexText tMeanText = NexText(en_WinID_Qc_Res_Summary , 39, "t22");
static NexText tSDText = NexText(en_WinID_Qc_Res_Summary , 40, "t23");

static NexText tTestName = NexText(en_WinID_Qc_Res_Summary , 5, "t1");
static NexText tCntrlName = NexText(en_WinID_Qc_Res_Summary , 7, "t0");
static NexText tLotNum = NexText(en_WinID_Qc_Res_Summary , 9, "t2");
static NexText tExpDate = NexText(en_WinID_Qc_Res_Summary , 11, "t3");
/*This list condains all the local buttons to be monitored in realtime*/
extern bool e_QCTestDoneflag;

static NexTouch *nex_Listen_List[] ={&bQCMenuBack,
	 	 	 	 	 	 	 	 	 &bQCLJ,
	 	 	 	 	 	 	 	 	 &bPrintQcLjPlot,
									 &bQCAccept,
									 &bQCDecline,
									 &bQCRunMenuBack,
									 NULL};

static void HandlerbQCMenuBack(void *ptr);
static void HandlerbQCRunMenuBack(void *ptr);
static void HandlerShowLJPlotBack(void *ptr);
static void HandlerPrintLjPlot(void *ptr);
static void ShowHideQCSummaryAcceptRejectButton(void);
static void HandlerbQCAccept(void *ptr);
static void HandlerbQCDecline(void *ptr);

enWindowStatus ShowQCResSummaryScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bQCMenuBack.attachPush(HandlerbQCMenuBack, &bQCMenuBack);
	bQCRunMenuBack.attachPush(HandlerbQCRunMenuBack, &bQCRunMenuBack);
	bQCLJ.attachPush(HandlerShowLJPlotBack, &bQCLJ);
	bPrintQcLjPlot.attachPush(HandlerPrintLjPlot, &bQCLJ);
	bQCAccept.attachPush(HandlerbQCAccept, &bQCAccept);
	bQCDecline.attachPush(HandlerbQCDecline, &bQCDecline);

	QCDATA *pQcData = Get_InstaneQcResult();
	ReadQcResData(stcTestData.TestId , pQcData);


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

	uint8_t u8ControlIdx = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8QcIdx;
	tTestName.setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);
	tCntrlName.setText(m_QcSetup->m_QcControls[u8ControlIdx].arrControlName);
	tLotNum.setText(m_QcSetup->m_QcControls[u8ControlIdx].arRLotNum);

	char arrBuff[64] = {0};
	snprintf(arrBuff , 63 , "%02d/%02d/%02d" , m_QcSetup->m_QcControls[u8ControlIdx].m_ExpiryDate.Date,
			m_QcSetup->m_QcControls[u8ControlIdx].m_ExpiryDate.Month,
			m_QcSetup->m_QcControls[u8ControlIdx].m_ExpiryDate.Year);
	tExpDate.setText(arrBuff);

	float fLabMean = 0;
	float fLabMin = 0;
	float fLabMax = 0;
	float fLabSd = 0;
	float fLabCv = 0;
	float fSdSum = 0;
	uint8_t fNumOfAnalysis = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved;


	/* Displaying last run result to accept or decline*/
	if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
	{
		snprintf(arrBuff , 63 , "%.02f %s" , pQcData->Res[u8QcTestEntryIdx][fNumOfAnalysis-1],GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
	}
	else
	{
		snprintf(arrBuff , 63 , "%.02f %s" , pQcData->Res[u8QcTestEntryIdx][fNumOfAnalysis-1], &arr_cUnitBuffer[GetInstance_TestParams()[stcTestData.TestId].Unit][0]);
	}

//	snprintf(arrBuff , 63 , "%.02f" , pQcData->Res[u8QcTestEntryIdx][u8N-1]);
	tLastQCRes.setText(arrBuff);


	/*Calculating mean values for number of test results*/

	for(uint8_t u8Count = 0; u8Count < fNumOfAnalysis; ++u8Count)
	{
		fLabMean += pQcData->Res[u8QcTestEntryIdx][u8Count];
	}
	fLabMean /= fNumOfAnalysis;

	if(fNumOfAnalysis > 1)
	{
		for(uint8_t u8Count = 0; u8Count < fNumOfAnalysis; ++u8Count)
		{
			fSdSum += (pQcData->Res[u8QcTestEntryIdx][u8Count] - fLabMean) *
					(pQcData->Res[u8QcTestEntryIdx][u8Count] - fLabMean);
		}

		fLabSd = sqrt(fSdSum / (fNumOfAnalysis-1));
		fLabCv = (fLabSd / fLabMean)*100;

		fLabMin = (fLabMean - (fLabSd * 2));
		fLabMax = (fLabMean + (fLabSd * 2));
	}

	snprintf(arrBuff , 63 , "%.02f" , fLabMean);
	tLabMean.setText(arrBuff);

	snprintf(arrBuff , 63 , "%.02f" , fLabSd);
	tSD.setText(arrBuff);

	snprintf(arrBuff , 63 , "%.02f" , fLabCv);
	tCV.setText(arrBuff);

	snprintf(arrBuff , 63 , "%.02f" , fLabMax);
	tLabMax.setText(arrBuff);

	snprintf(arrBuff , 63 , "%.02f" , fLabMin);
	tLabMin.setText(arrBuff);

	snprintf(arrBuff , 63 , "%d" , fNumOfAnalysis);
	tN.setText(arrBuff);

	snprintf(arrBuff , 63 , "%.02f" , m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fMeanVal);
	tMean.setText(arrBuff);

	snprintf(arrBuff , 63 , "MEAN : %.02f" , m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fMeanVal);
	tMeanText.setText(arrBuff);

//	snprintf(arrBuff , 63 , "%.02f" , m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fLowVal);
//	tMin.setText(arrBuff);
//
//	snprintf(arrBuff , 63 , "%.02f" , m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fHighVal);
//	tMax.setText(arrBuff);

	snprintf(arrBuff , 63 , "SD : %.02f" , m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fHighVal);
	tSDText.setText(arrBuff);

	ShowHideQCSummaryAcceptRejectButton();
	return WinStatus;
}

void HandlerQCResSummaryScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerShowLJPlotBack(void *ptr)
{

	if(en_WinId_TestRun == stcScreenNavigation.PrevWindowId)
	{
		ChangeWindowPage(en_WinID_LJ_Plot , en_WinId_TestRun);
		stcScreenNavigation.PrevWindowId = en_WinId_TestRun;
	}
	else
	{
		ChangeWindowPage(en_WinID_LJ_Plot , en_WinID_QcTestScreen);
		stcScreenNavigation.PrevWindowId = en_WinID_QcTestScreen;
	}

	BeepBuzzer();

}

void HandlerbQCMenuBack(void *ptr)
{
	ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
	BeepBuzzer();
}
static void HandlerbQCRunMenuBack(void *ptr)
{
	ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerPrintLjPlot(void *ptr)
{
	BeepBuzzer();
	bPrintQcLjPlot.setVisible(false);

	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	QCDATA *pQcData = Get_InstaneQcResult();

	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);

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
void ShowHideQCSummaryAcceptRejectButton(void)
{
	if(true == e_QCTestDoneflag)
	{
		bQCAccept.setVisible(true);
		bQCDecline.setVisible(true);
		tLastQCResText.setVisible(false);
		tLastQCRes.setVisible(false);
		bQCRunMenuBack.setVisible(false);
	}
	else
	{
		bQCAccept.setVisible(false);
		bQCDecline.setVisible(false);
		tLastQCResText.setVisible(false);
		tLastQCRes.setVisible(false);
	}
}
void HandlerbQCAccept(void *ptr)
{
	e_QCTestDoneflag = false;
	ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbQCDecline(void *ptr)
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
	ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
	BeepBuzzer();
}


