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

#define MAX_QC_TEST_VIEW_ENTRY_PER_PAGE (4)

static uint8_t g_u8QcTestList[MAX_QC_TEST_VIEW_ENTRY_PER_PAGE] = {NVM_INIT_NOT_OK_FLAG};
static bool g_bSelectionButtons[MAX_QC_TEST_VIEW_ENTRY_PER_PAGE] = {false};
static bool g_bMasterSelectionButton = false;
static uint8_t g_u8LastSelectedRow = 0;
static uint8_t g_u8QcTestSetupFreeIndex = NVM_INIT_NOT_OK_FLAG;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinID_QcTestScreen , 2, "b5");
static NexButton bAdd = NexButton(en_WinID_QcTestScreen , 35 , "b4");
static NexButton bDelete = NexButton(en_WinID_QcTestScreen , 8 , "b6");
static NexButton bLjPlot = NexButton(en_WinID_QcTestScreen , 9 , "b7");
static NexButton bQcControl = NexButton(en_WinID_QcTestScreen , 5 , "b2");
static NexButton bQcResHistory = NexButton(en_WinID_QcTestScreen , 38 , "b14");
static NexButton bSelectTest = NexButton(en_WinID_QcTestScreen , 36 , "b12");
static NexText tTestName = NexText(en_WinID_QcTestScreen , 37 , "t35");

static NexButton bMasterSelection = NexButton(en_WinID_QcTestScreen , 30 , "b13");

static NexButton bSelect[MAX_QC_TEST_VIEW_ENTRY_PER_PAGE] = {
		NexButton(en_WinID_QcTestScreen , 31, "b8"),
		NexButton(en_WinID_QcTestScreen , 32, "b9"),
		NexButton(en_WinID_QcTestScreen , 33, "b10"),
		NexButton(en_WinID_QcTestScreen , 34, "b11"),
};

static NexText SelectBar[MAX_QC_TEST_VIEW_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcTestScreen , 39, "t1"),
		NexText(en_WinID_QcTestScreen , 40, "t2"),
		NexText(en_WinID_QcTestScreen , 41, "t7"),
		NexText(en_WinID_QcTestScreen , 42, "t8"),
};

//static NexText TestName[MAX_QC_TEST_VIEW_ENTRY_PER_PAGE] = {
//		NexText(en_WinID_QcTestScreen , 20, "t2"),
//		NexText(en_WinID_QcTestScreen , 21, "t9"),
//		NexText(en_WinID_QcTestScreen , 22, "t10"),
//		NexText(en_WinID_QcTestScreen , 23, "t11"),
//};

static NexText ControlName[MAX_QC_TEST_VIEW_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcTestScreen , 11, "t3"),
		NexText(en_WinID_QcTestScreen , 12, "t4"),
		NexText(en_WinID_QcTestScreen , 13, "t5"),
		NexText(en_WinID_QcTestScreen , 14, "t6"),
};

static NexText strHigh[MAX_QC_TEST_VIEW_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcTestScreen , 16, "t15"),
		NexText(en_WinID_QcTestScreen , 17, "t16"),
		NexText(en_WinID_QcTestScreen , 18, "t17"),
		NexText(en_WinID_QcTestScreen , 19, "t18"),
};

static NexText strMean[MAX_QC_TEST_VIEW_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcTestScreen , 21, "t22"),
		NexText(en_WinID_QcTestScreen , 22, "t23"),
		NexText(en_WinID_QcTestScreen , 23, "t24"),
		NexText(en_WinID_QcTestScreen , 24, "t25"),
};

static NexText strLow[MAX_QC_TEST_VIEW_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcTestScreen , 26, "t29"),
		NexText(en_WinID_QcTestScreen , 27, "t30"),
		NexText(en_WinID_QcTestScreen , 28, "t31"),
		NexText(en_WinID_QcTestScreen , 29, "t32"),
};

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bAdd,
									 &bDelete,
									 &bLjPlot,
									 &bQcControl,
									 &bQcResHistory,
									 &bSelect[0],&bSelect[1],
									 &bSelect[2],&bSelect[3],
									 &SelectBar[0],&SelectBar[1],
									 &SelectBar[2],&SelectBar[3],
									 &bMasterSelection,
									 &bSelectTest,
									 NULL};


static void HandlerbBack(void *ptr);
static void HandlerbAdd(void *ptr);
static void HandlerbDelete(void *ptr);
static void HandlerbLJplot(void *ptr);
static void HandlerbQcControl(void *ptr);
static void HandlerbSelect0(void *ptr);
static void HandlerbSelect1(void *ptr);
static void HandlerbSelect2(void *ptr);
static void HandlerbSelect3(void *ptr);
static void HandlerbMasterSelection(void *ptr);
static void HandlerbSelectTest(void *ptr);
static void HandlerbQcResultHistory(void *ptr);

static void ShowQcTestData(void);
static uint8_t Get_NumOfQcFound(void);
static void ShowHideButtons(void);
static uint8_t Get_FreeSlot(void);

enWindowStatus ShowQcTestViewScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bAdd.attachPush(HandlerbAdd, &bAdd);
	bDelete.attachPush(HandlerbDelete, &bDelete);
	bLjPlot.attachPush(HandlerbLJplot, &bLjPlot);
	bQcControl.attachPush(HandlerbQcControl, &bQcControl);
	bMasterSelection.attachPush(HandlerbMasterSelection, &bMasterSelection);
	bSelectTest.attachPush(HandlerbSelectTest, &bSelectTest);
	bSelect[0].attachPush(HandlerbSelect0, &bSelect[0]);
	bSelect[1].attachPush(HandlerbSelect1, &bSelect[1]);
	bSelect[2].attachPush(HandlerbSelect2, &bSelect[2]);
	bSelect[3].attachPush(HandlerbSelect3, &bSelect[3]);

	SelectBar[0].attachPush(HandlerbSelect0, &SelectBar[0]);
	SelectBar[1].attachPush(HandlerbSelect1, &SelectBar[1]);
	SelectBar[2].attachPush(HandlerbSelect2, &SelectBar[2]);
	SelectBar[3].attachPush(HandlerbSelect3, &SelectBar[3]);

	bQcResHistory.attachPush(HandlerbQcResultHistory, &bQcResHistory);

	NVM_ReadQcSetup();
	ShowQcTestData();

	g_bMasterSelectionButton = false;
	memset(g_bSelectionButtons , false , sizeof(g_bSelectionButtons));
	ShowHideButtons();

	if((GetInstance_TestParams()[stcTestData.TestId].ValidTestParamSavedFlag) == VALID_TEST_PARAM_FLAG)
	{
		tTestName.setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);
	}
	else
	{
		tTestName.setText("---");
	}
	return WinStatus;
}

void HandlerQcTestViewScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbSelectTest(void *ptr)
{
	ChangeWindowPage(en_WinID_TestSelectionScreen , en_WinID_QcTestScreen);
	BeepBuzzer();
}

void HandlerbAdd(void *ptr)
{
	BeepBuzzer();
	g_u8QcTestSetupFreeIndex = Get_FreeSlot();
	if(NVM_INIT_NOT_OK_FLAG == stcTestData.TestId)
	{
		if(enkeyOk == ShowMainPopUp("Quality Control","Please Select A Valid Test", true))
		{
			ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
		}
	}
	else if(NVM_INIT_NOT_OK_FLAG == g_u8QcTestSetupFreeIndex)
	{
		if(enkeyOk == ShowMainPopUp("Quality Control","Maximum Controls Linked", true))
		{
			ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
		}
	}
	else
	{
		ChangeWindowPage(en_WinID_QcAddTest , (enWindowID)NULL);
	}
}
void HandlerbDelete(void *ptr)
{
	BeepBuzzer();
	uint8_t u8SlotIndex = 0;
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enKeyYes == ShowMainPopUp("Quality Control","Do You Want To Unlink QC?", false))
	{
		for(uint8_t u8Idx = 0; u8Idx < MAX_NUM_OF_QC_CONTROL_PER_TEST ; ++u8Idx)
		{
			if(NVM_INIT_NOT_OK_FLAG != g_u8QcTestList[u8Idx] && (true == g_bMasterSelectionButton ||
																true == g_bSelectionButtons[u8Idx]))
			{
				u8SlotIndex = Get_QcTestEntryId(u8Idx);
				if(0 <= u8SlotIndex &&
						u8SlotIndex < MAX_NUM_OF_QC_CONTROL_PER_TEST)
				{
					m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].u8QcIdx = NVM_INIT_NOT_OK_FLAG;
					m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].u8NumOfResSaved = 0;
					m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].fHighVal = 0;
					m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].fLowVal = 0;
					m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8SlotIndex].fMeanVal = 0;
				}
			}
		}
		NVM_WriteQcSetup();
	}
	stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
	ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
}
void HandlerbLJplot(void *ptr)
{
	ChangeWindowPage(en_WinID_LJ_Plot , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbQcResultHistory(void *ptr)
{
	ChangeWindowPage(en_WinID_QcResHistory , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbQcControl(void *ptr)
{
	ChangeWindowPage(en_WinID_QcControl , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbSelect0(void *ptr)
{
	uint8_t u8ButtonId = 0;
//	if(false == g_bSelectionButtons[u8ButtonId])
//	{
//		g_bSelectionButtons[u8ButtonId] = true;
//	}
//	else
//	{
//		g_bSelectionButtons[u8ButtonId] = false;
//	}
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_TEST_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		g_bSelectionButtons[u8Idx] = false;
	}
	g_bSelectionButtons[u8ButtonId] = true;
	ShowHideButtons();
	BeepBuzzer();
}
void HandlerbSelect1(void *ptr)
{
	uint8_t u8ButtonId = 1;
//	if(false == g_bSelectionButtons[u8ButtonId])
//	{
//		g_bSelectionButtons[u8ButtonId] = true;
//	}
//	else
//	{
//		g_bSelectionButtons[u8ButtonId] = false;
//	}
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_TEST_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		g_bSelectionButtons[u8Idx] = false;
	}
	g_bSelectionButtons[u8ButtonId] = true;
	ShowHideButtons();
	BeepBuzzer();
}
void HandlerbSelect2(void *ptr)
{
	uint8_t u8ButtonId = 2;
//	if(false == g_bSelectionButtons[u8ButtonId])
//	{
//		g_bSelectionButtons[u8ButtonId] = true;
//	}
//	else
//	{
//		g_bSelectionButtons[u8ButtonId] = false;
//	}
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_TEST_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		g_bSelectionButtons[u8Idx] = false;
	}
	g_bSelectionButtons[u8ButtonId] = true;
	ShowHideButtons();
	BeepBuzzer();
}
void HandlerbSelect3(void *ptr)
{
	uint8_t u8ButtonId = 3;
//	if(false == g_bSelectionButtons[u8ButtonId])
//	{
//		g_bSelectionButtons[u8ButtonId] = true;
//	}
//	else
//	{
//		g_bSelectionButtons[u8ButtonId] = false;
//	}
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_TEST_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		g_bSelectionButtons[u8Idx] = false;
	}
	g_bSelectionButtons[u8ButtonId] = true;
	ShowHideButtons();
	BeepBuzzer();
}

void HandlerbMasterSelection(void *ptr)
{
	if(false == g_bMasterSelectionButton)
	{
		g_bMasterSelectionButton = true;
	}
	else
	{
		g_bMasterSelectionButton = false;
		for(uint8_t u8Idx = 0; u8Idx < MAX_QC_TEST_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
		{
			g_bSelectionButtons[u8Idx] = false;
		}
	}
	ShowHideButtons();
	BeepBuzzer();
}

uint8_t Get_NumOfQcFound(void)
{
	uint8_t u8Count = 0;
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	for(uint8_t u8Idx = 0 ; u8Idx < MAX_NUM_OF_QC_CONTROL_PER_TEST ; ++u8Idx)
	{
		if(enQcCntrl_Max > m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8Idx].u8QcIdx &&
				NVM_INIT_NOT_OK_FLAG != m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8Idx].u8QcIdx)
		{
			u8Count++;
		}
	}
	return u8Count;
}

uint8_t Get_QcTestEntryId(uint8_t u8Row)
{
	uint8_t u8CntrlIdx = 0;
	uint8_t u8CurrPage = 0;
	u8CntrlIdx = u8Row + (MAX_QC_TEST_VIEW_ENTRY_PER_PAGE * u8CurrPage);
	return g_u8QcTestList[u8CntrlIdx];
}

uint8_t GetQcTestLastSelectedRow(void)
{
	return g_u8LastSelectedRow;
}

uint8_t Get_FreeSlot(void)
{
	uint8_t u8Count = NVM_INIT_NOT_OK_FLAG;
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();

	for(uint8_t u8Idx = 0 ; u8Idx < MAX_NUM_OF_QC_CONTROL_PER_TEST ; ++u8Idx)
	{
		if(NVM_INIT_NOT_OK_FLAG == m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8Idx].u8QcIdx)
		{
			u8Count = u8Idx;
			break;
		}
	}
	return u8Count;
}

uint8_t GetQcTestSetupFreeSlotIndex_QcTestSetup(void)
{
	uint8_t u8QcTestSlotIndex = g_u8QcTestSetupFreeIndex;
	return u8QcTestSlotIndex;
}

void ShowQcTestData(void)
{
	char arrBuff[32] = {0};
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();

	for(uint8_t SlNo = 0 ; SlNo < MAX_QC_TEST_VIEW_ENTRY_PER_PAGE; ++SlNo)
	{
		ControlName[SlNo].setText(" ");
		strHigh[SlNo].setText(" ");
		strMean[SlNo].setText(" ");
		strLow[SlNo].setText(" ");
		bSelect[SlNo].setVisible(false);
		SelectBar[SlNo].setVisible(false);
		bMasterSelection.setVisible(false);
		g_u8QcTestList[SlNo] = NVM_INIT_NOT_OK_FLAG;
	}

	for(uint8_t u8Idx = 0 , u8SlNo = 0;
			u8Idx < MAX_QC_TEST_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		uint8_t u8ControlIdx = NVM_INIT_NOT_OK_FLAG;

		if(stcTestData.TestId != INVALID_TEST_PARAM_FLAG)
		{
			u8ControlIdx = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8Idx].u8QcIdx;
		}

		if(enQcCntrl_Max > u8ControlIdx &&
				NVM_INIT_NOT_OK_FLAG != u8ControlIdx)
		{
			g_u8QcTestList[u8SlNo] = u8Idx;
//			TestName[u8SlNo].setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);

			snprintf(arrBuff , 31 , "%s-%s", m_QcSetup->m_QcControls[u8ControlIdx].arrControlName,m_QcSetup->m_QcControls[u8ControlIdx].arRLotNum);
			ControlName[u8SlNo].setText(arrBuff);
			snprintf(arrBuff , 31 , "%.02f", m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8Idx].fHighVal);
			strHigh[u8SlNo].setText(arrBuff);
			snprintf(arrBuff , 31 , "%.02f", m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8Idx].fLowVal);
			strLow[u8SlNo].setText(arrBuff);
			snprintf(arrBuff , 31 , "%.02f", m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8Idx].fMeanVal);
			strMean[u8SlNo].setText(arrBuff);
			bSelect[u8SlNo].setVisible(true);
			bMasterSelection.setVisible(true);
			SelectBar[u8SlNo].setVisible(true);
			u8SlNo++;
		}
	}
}

void ShowHideButtons(void)
{
	uint8_t u8NumOfSelection = 0;
	if(true == g_bMasterSelectionButton)
	{
		bMasterSelection.Set_background_image_pic(RADIO_BUTTON_ON);
	}
	else
	{
		bMasterSelection.Set_background_image_pic(RADIO_BUTTON_OFF);
	}

	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_TEST_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		if(NVM_INIT_NOT_OK_FLAG != g_u8QcTestList[u8Idx] && (true == g_bMasterSelectionButton ||
															true == g_bSelectionButtons[u8Idx] ))
		{
			g_bSelectionButtons[u8Idx] = true;
			bSelect[u8Idx].Set_background_image_pic(RADIO_BUTTON_ON);
			g_u8LastSelectedRow = u8Idx;
			u8NumOfSelection++;
		}
		else
		{
			bSelect[u8Idx].Set_background_image_pic(RADIO_BUTTON_OFF);
		}
	}
	if(0 < u8NumOfSelection)
	{
		bDelete.setVisible(true);
		if(1 == u8NumOfSelection)
		{
			bLjPlot.setVisible(true);
			bQcResHistory.setVisible(true);
		}
		else
		{
			bLjPlot.setVisible(false);
			bQcResHistory.setVisible(false);
		}
	}
	else
	{
		bDelete.setVisible(false);
		bLjPlot.setVisible(false);
		bQcResHistory.setVisible(false);
	}
}
