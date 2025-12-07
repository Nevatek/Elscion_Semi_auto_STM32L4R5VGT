/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/NVM_QCSetup.hpp"

uint8_t g_u8SelectedTestQcIdx = 0;
uint8_t g_arrQcIdx[MAX_NUM_OF_QC_CONTROL_PER_TEST] = {NVM_INIT_NOT_OK_FLAG};
uint8_t g_u8SlNo = 0;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bQCMenuBack = NexButton(en_WinId_SelectQcLot , 3, "b5");
static NexText tQcSelectMainBoard = NexText(en_WinId_SelectQcLot , 4 , "t0");
static NexButton bQCLotList[MAX_NUM_OF_QC_CONTROL_PER_TEST] =
{
		NexButton(en_WinId_SelectQcLot , 6, "t11"),
		NexButton(en_WinId_SelectQcLot , 7, "t2"),
		NexButton(en_WinId_SelectQcLot , 8, "t4"),
		NexButton(en_WinId_SelectQcLot , 9, "t1"),
};

//static NexText bQCControlNameList[MAX_NUM_OF_QC_CONTROL_PER_TEST] =
//{
//		NexText(en_WinId_SelectQcLot , 9, "t3"),
//		NexText(en_WinId_SelectQcLot , 10, "t5"),
//		NexText(en_WinId_SelectQcLot , 11, "t6"),
//		NexText(en_WinId_SelectQcLot , 12, "t7"),
//};
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bQCMenuBack,
									 &bQCLotList[0],
									 &bQCLotList[1],
									 &bQCLotList[2],
									 &bQCLotList[3],
									 NULL};

static void HandlerbQCMenuBack(void *ptr);
static void HandlerbQc0(void *ptr);
static void HandlerbQc1(void *ptr);
static void HandlerbQc2(void *ptr);
static void HandlerbQc3(void *ptr);

enWindowStatus ShowTestQCSelectMenuScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bQCMenuBack.attachPush(HandlerbQCMenuBack, &bQCMenuBack);
	bQCLotList[0].attachPush(HandlerbQc0, &bQCLotList[0]);
	bQCLotList[1].attachPush(HandlerbQc1, &bQCLotList[1]);
	bQCLotList[2].attachPush(HandlerbQc2, &bQCLotList[2]);
	bQCLotList[3].attachPush(HandlerbQc3, &bQCLotList[3]);

	g_u8SelectedTestQcIdx = 0;
	g_u8SlNo = 0;

	ResetSelectedQcIdx();
	char arrBuff[32] = {0};

	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	for(uint8_t u8Idx = 0;
			u8Idx < MAX_NUM_OF_QC_CONTROL_PER_TEST ; ++u8Idx)
	{
		uint8_t u8ControlIdx = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8Idx].u8QcIdx;
		if(enQcCntrl_Max > u8ControlIdx &&
				NVM_INIT_NOT_OK_FLAG != u8ControlIdx)
		{
			/*Valid test*/
			tQcSelectMainBoard.setVisible(true);
			bQCLotList[g_u8SlNo].setVisible(true);
			snprintf(arrBuff , 31 , "%s-%s", m_QcSetup->m_QcControls[u8ControlIdx].arrControlName,m_QcSetup->m_QcControls[u8ControlIdx].arRLotNum);
			bQCLotList[g_u8SlNo].setText(arrBuff);
			g_arrQcIdx[g_u8SlNo] = u8ControlIdx;
			g_u8SlNo++;
		}
	}
	return WinStatus;
}

void HandlerTestQCSelectScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
	if(0 == g_u8SlNo)
	{
		/*No QC found*/
		if(enkeyOk == ShowMainPopUp("Quality Control","No QC Found", true))
		{
			ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
		}
	}
}

void HandlerbQCMenuBack(void *ptr)
{
	ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbQc0(void *ptr)
{
	g_u8SelectedTestQcIdx = 0;
	ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbQc1(void *ptr)
{
	g_u8SelectedTestQcIdx = 1;
	ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbQc2(void *ptr)
{
	g_u8SelectedTestQcIdx = 2;
	ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbQc3(void *ptr)
{
	g_u8SelectedTestQcIdx = 3;
	ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
	BeepBuzzer();
}
void ResetSelectedQcIdx(void)
{
	for(uint8_t u8Idx = 0 ;
			u8Idx < MAX_NUM_OF_QC_CONTROL_PER_TEST ; ++u8Idx)
	{
		g_arrQcIdx[u8Idx] = NVM_INIT_NOT_OK_FLAG;
	}
}
uint8_t GetSelectedTestQcIdx(void)
{
	return g_arrQcIdx[g_u8SelectedTestQcIdx];
}

uint8_t GetSelectedTestQcRow(void)
{
	return g_u8SelectedTestQcIdx;
}

