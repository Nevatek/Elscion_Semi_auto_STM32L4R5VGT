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

#define MAX_QC_RES_VIEW_ENTRY_PER_PAGE (5)

static int8_t  g_n8CurrPage = 0;
static uint8_t g_u8TotalPage = 0;
static uint8_t g_u8LastSelectedRow = 0;
static bool g_bMasterSelectionButton = false;
static bool g_bSelectionButtons[MAX_QC_RES_VIEW_ENTRY_PER_PAGE] = {false};
static bool g_u8QcValidEntry[MAX_QC_RES_VIEW_ENTRY_PER_PAGE] = {false};
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bQCMenuBack = NexButton(en_WinID_QcResHistory , 1, "b5");
static NexButton bUp = NexButton(en_WinID_QcResHistory , 7 , "b0");
static NexButton bDown = NexButton(en_WinID_QcResHistory , 6 , "b1");
static NexButton bMasterSelection = NexButton(en_WinID_QcResHistory , 15 , "b8");
static NexButton bDelete = NexButton(en_WinID_QcResHistory , 4, "b7");

static NexText tTestName = NexText(en_WinID_QcResHistory , 8, "t0");
static NexText tCntrlName = NexText(en_WinID_QcResHistory , 11, "t1");
static NexText tLotNum = NexText(en_WinID_QcResHistory , 13, "t2");
static NexText tExpDate = NexText(en_WinID_QcResHistory , 14, "t3");

static NexText tMeanText = NexText(en_WinID_QcResHistory , 47, "t28");
static NexText tSDText = NexText(en_WinID_QcResHistory , 48, "t29");


static NexButton bSelect[MAX_QC_RES_VIEW_ENTRY_PER_PAGE] = {
		NexButton(en_WinID_QcResHistory , 17, "b9"),
		NexButton(en_WinID_QcResHistory , 19, "b10"),
		NexButton(en_WinID_QcResHistory , 21, "b11"),
		NexButton(en_WinID_QcResHistory , 23, "b12"),
		NexButton(en_WinID_QcResHistory , 25, "b13"),
};

static NexText SelectBar[MAX_QC_RES_VIEW_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcResHistory , 49, "t30"),
		NexText(en_WinID_QcResHistory , 50, "t31"),
		NexText(en_WinID_QcResHistory , 51, "t32"),
		NexText(en_WinID_QcResHistory , 52, "t33"),
		NexText(en_WinID_QcResHistory , 53, "t34"),
};


static NexButton bSlNo[MAX_QC_RES_VIEW_ENTRY_PER_PAGE] = {
		NexButton(en_WinID_QcResHistory , 40, "t23"),
		NexButton(en_WinID_QcResHistory , 41, "t24"),
		NexButton(en_WinID_QcResHistory , 42, "t25"),
		NexButton(en_WinID_QcResHistory , 43, "t26"),
		NexButton(en_WinID_QcResHistory , 44, "t27"),
};

static NexButton bConc[MAX_QC_RES_VIEW_ENTRY_PER_PAGE] = {
		NexButton(en_WinID_QcResHistory , 18, "t5"),
		NexButton(en_WinID_QcResHistory , 20, "t6"),
		NexButton(en_WinID_QcResHistory , 22, "t7"),
		NexButton(en_WinID_QcResHistory , 24, "t8"),
		NexButton(en_WinID_QcResHistory , 26, "t9"),
};

static NexButton bDate[MAX_QC_RES_VIEW_ENTRY_PER_PAGE] = {
		NexButton(en_WinID_QcResHistory , 31, "t14"),
		NexButton(en_WinID_QcResHistory , 30, "t10"),
		NexButton(en_WinID_QcResHistory , 29, "t11"),
		NexButton(en_WinID_QcResHistory , 28, "t12"),
		NexButton(en_WinID_QcResHistory , 27, "t13"),
};

static NexButton bTime[MAX_QC_RES_VIEW_ENTRY_PER_PAGE] = {
		NexButton(en_WinID_QcResHistory , 34, "t17"),
		NexButton(en_WinID_QcResHistory , 35, "t18"),
		NexButton(en_WinID_QcResHistory , 36, "t19"),
		NexButton(en_WinID_QcResHistory , 37, "t20"),
		NexButton(en_WinID_QcResHistory , 38, "t21"),
};
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bQCMenuBack,
									 &bSelect[0],
									 &bSelect[1],
									 &bSelect[2],
									 &bSelect[3],
									 &bSelect[4],
									 &SelectBar[0],
									 &SelectBar[1],
									 &SelectBar[2],
									 &SelectBar[3],
									 &SelectBar[4],
									 &bUp,
									 &bDown,
									 &bMasterSelection,
									 &bDelete,
									 NULL};

static void HandlerbQCMenuBack(void *ptr);
static void HandlerbUp(void *ptr);
static void HandlerbDown(void *ptr);
static void HandlerbDelete(void *ptr);
static void HandlerbMasterSelection(void *ptr);
static void HandlerbSelect0(void *ptr);
static void HandlerbSelect1(void *ptr);
static void HandlerbSelect2(void *ptr);
static void HandlerbSelect3(void *ptr);
static void HandlerbSelect4(void *ptr);
static void ShowQcResultEntries(void);

static void ShowHideButtons(void);

enWindowStatus ShowQCResHistoryScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bQCMenuBack.attachPush(HandlerbQCMenuBack, &bQCMenuBack);
	bMasterSelection.attachPush(HandlerbMasterSelection, &bMasterSelection);
	bSelect[0].attachPush(HandlerbSelect0, &bSelect[0]);
	bSelect[1].attachPush(HandlerbSelect1, &bSelect[1]);
	bSelect[2].attachPush(HandlerbSelect2, &bSelect[2]);
	bSelect[3].attachPush(HandlerbSelect3, &bSelect[3]);
	bSelect[4].attachPush(HandlerbSelect4, &bSelect[4]);

	SelectBar[0].attachPush(HandlerbSelect0, &SelectBar[0]);
	SelectBar[1].attachPush(HandlerbSelect1, &SelectBar[1]);
	SelectBar[2].attachPush(HandlerbSelect2, &SelectBar[2]);
	SelectBar[3].attachPush(HandlerbSelect3, &SelectBar[3]);
	SelectBar[4].attachPush(HandlerbSelect4, &SelectBar[4]);
	bDelete.attachPush(HandlerbDelete, &bDelete);
	bUp.attachPush(HandlerbUp, &bUp);
	bDown.attachPush(HandlerbDown, &bDown);
	bMasterSelection.attachPush(HandlerbMasterSelection, &bMasterSelection);

	g_n8CurrPage = 0;

	QCDATA *pQcData = Get_InstaneQcResult();
	ReadQcResData(stcTestData.TestId , pQcData);

	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);
	uint8_t u8ControlIdx = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8QcIdx;
	tTestName.setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);
	tCntrlName.setText(m_QcSetup->m_QcControls[u8ControlIdx].arrControlName);
	tLotNum.setText(m_QcSetup->m_QcControls[u8ControlIdx].arRLotNum);

	char arrBuff[64] = {0};
	snprintf(arrBuff , 63 , "%02d/%02d/%02d" , m_QcSetup->m_QcControls[u8ControlIdx].m_ExpiryDate.Date,
			m_QcSetup->m_QcControls[u8ControlIdx].m_ExpiryDate.Month,
			m_QcSetup->m_QcControls[u8ControlIdx].m_ExpiryDate.Year);
	tExpDate.setText(arrBuff);

	snprintf(arrBuff , 63 , "MEAN : %.02f" , m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fMeanVal);
	tMeanText.setText(arrBuff);

	snprintf(arrBuff , 63 , "SD : %.02f" , m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].fHighVal);
	tSDText.setText(arrBuff);


	ShowQcResultEntries();
	ShowHideButtons();
	return WinStatus;
}

void HandlerQCResHistoryScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);

	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);
	uint8_t u8NumOfQcResAvailable = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved;
	if(0 >= u8NumOfQcResAvailable)
	{
		/*No QC found*/
		if(enkeyOk == ShowMainPopUp("QC Results","No Results Found", true))
		{
			ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
		}
	}
}

void HandlerbQCMenuBack(void *ptr)
{
	g_bMasterSelectionButton = false;
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		g_bSelectionButtons[u8Idx] = false;
	}
	ShowHideButtons();
	ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbUp(void *ptr)
{
	g_n8CurrPage--;
	if(0 > g_n8CurrPage)
	{
		g_n8CurrPage = g_u8TotalPage - 1;
	}
	g_bMasterSelectionButton = false;
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		g_bSelectionButtons[u8Idx] = false;
	}
	ShowHideButtons();
	ShowQcResultEntries();
	BeepBuzzer();
}

void HandlerbDown(void *ptr)
{
	g_n8CurrPage++;
	if(g_u8TotalPage <= g_n8CurrPage)
	{
		g_n8CurrPage = 0;
	}

	g_bMasterSelectionButton = false;
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		g_bSelectionButtons[u8Idx] = false;
	}
	ShowHideButtons();
	ShowQcResultEntries();
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
		for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
		{
			g_bSelectionButtons[u8Idx] = false;
		}
	}
	ShowHideButtons();
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
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
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
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
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
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
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
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		g_bSelectionButtons[u8Idx] = false;
	}
	g_bSelectionButtons[u8ButtonId] = true;
	ShowHideButtons();
	BeepBuzzer();
}
void HandlerbSelect4(void *ptr)
{
	uint8_t u8ButtonId = 4;
//	if(false == g_bSelectionButtons[u8ButtonId])
//	{
//		g_bSelectionButtons[u8ButtonId] = true;
//	}
//	else
//	{
//		g_bSelectionButtons[u8ButtonId] = false;
//	}
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		g_bSelectionButtons[u8Idx] = false;
	}
	g_bSelectionButtons[u8ButtonId] = true;
	ShowHideButtons();
	BeepBuzzer();
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

	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		if(true == g_bMasterSelectionButton ||
				true == g_bSelectionButtons[u8Idx])
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
	}
	else
	{
		bDelete.setVisible(false);
	}
}
void HandlerbDelete(void *ptr)
{
	BeepBuzzer();
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	QCDATA *pQcData = Get_InstaneQcResult();
	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enKeyYes == ShowMainPopUp("QC Results","Do you want to delete the results(s)?", false))
	{
		for(uint8_t u8Idx = 0; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
		{
			if(true == g_u8QcValidEntry[u8Idx] && (true == g_bMasterSelectionButton
					|| true == g_bSelectionButtons[u8Idx]))
			{
				/*Re arrange all qc result datas*/
				uint8_t u8DataIdx = u8Idx + (g_n8CurrPage * MAX_QC_RES_VIEW_ENTRY_PER_PAGE);
				for(; u8DataIdx < MAX_NUM_OF_QC_PER_TEST ; ++u8DataIdx)
				{
					pQcData->Res[u8QcTestEntryIdx][u8DataIdx] =
							pQcData->Res[u8QcTestEntryIdx][u8DataIdx + 1];

					memcpy(&pQcData->m_DateTime[u8QcTestEntryIdx][u8DataIdx] ,
							&pQcData->m_DateTime[u8QcTestEntryIdx][u8DataIdx + 1] , sizeof(DATETIME));
				}
				m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved--;
				g_bSelectionButtons[u8Idx] = false;
				g_u8QcValidEntry[u8Idx] = false;
			}
			g_bMasterSelectionButton = false;
		}
		WriteQcResData(stcTestData.TestId , pQcData);
		NVM_WriteQcSetup();
	}
	stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
	ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
}

void ShowQcResultEntries(void)
{
	uint8_t u8NumOfQcResAvailable = 0;
	uint8_t u8LastSelRow = GetQcTestLastSelectedRow();
	uint8_t u8QcTestEntryIdx = Get_QcTestEntryId(u8LastSelRow);
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	QCDATA *pQcData = Get_InstaneQcResult();
	char arrBuff[64] = {0};

	u8NumOfQcResAvailable = m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8QcTestEntryIdx].u8NumOfResSaved;
	if(MAX_NUM_OF_QC_PER_TEST <= u8NumOfQcResAvailable)
	{
		u8NumOfQcResAvailable = MAX_NUM_OF_QC_PER_TEST;
	}

	bMasterSelection.setVisible(false);
	for(uint8_t u8Idx = 0 ; u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
	{
		bSelect[u8Idx].setVisible(false);
		SelectBar[u8Idx].setVisible(false);
		bSlNo[u8Idx].setText(" ");
		bConc[u8Idx].setText(" ");
		bDate[u8Idx].setText(" ");
		bTime[u8Idx].setText(" ");
		g_u8QcValidEntry[u8Idx] = false;
	}

	if(0 < u8NumOfQcResAvailable)
	{
		g_u8TotalPage = (uint16_t)abs(u8NumOfQcResAvailable / MAX_QC_RES_VIEW_ENTRY_PER_PAGE);
		g_u8TotalPage++;
		/*Added to avoid displaying empty pages*/
		if (u8NumOfQcResAvailable % MAX_QC_RES_VIEW_ENTRY_PER_PAGE == 0)
		{
			g_u8TotalPage--;
		}

		uint8_t u8DataStartIdx = (g_n8CurrPage * MAX_QC_RES_VIEW_ENTRY_PER_PAGE);
		for(uint8_t u8Idx = 0 ; (u8Idx + u8DataStartIdx) < u8NumOfQcResAvailable &&
										u8Idx < MAX_QC_RES_VIEW_ENTRY_PER_PAGE ; ++u8Idx)
		{
			g_u8QcValidEntry[u8Idx] = true;
			snprintf(arrBuff , 63 , "%d" , u8Idx + u8DataStartIdx + 1);
			bSlNo[u8Idx].setText(arrBuff);

			snprintf(arrBuff , 63 , "%.02f" , pQcData->Res[u8QcTestEntryIdx][u8Idx + u8DataStartIdx]);
			bConc[u8Idx].setText(arrBuff);

			snprintf(arrBuff , 63 , "%02d/%02d/%02d" ,
					pQcData->m_DateTime[u8QcTestEntryIdx][u8Idx + u8DataStartIdx].u16Date_day,
					pQcData->m_DateTime[u8QcTestEntryIdx][u8Idx + u8DataStartIdx].u16Date_Month,
					pQcData->m_DateTime[u8QcTestEntryIdx][u8Idx + u8DataStartIdx].u16Date_Year);
			bDate[u8Idx].setText(arrBuff);

			snprintf(arrBuff , 63 , "%02d:%02d" ,
					pQcData->m_DateTime[u8QcTestEntryIdx][u8Idx + u8DataStartIdx].u16Date_Hour,
					pQcData->m_DateTime[u8QcTestEntryIdx][u8Idx + u8DataStartIdx].u16Date_min);
			bTime[u8Idx].setText(arrBuff);

			bMasterSelection.setVisible(true);
			bSelect[u8Idx].setVisible(true);
			SelectBar[u8Idx].setVisible(true);
		}

	}

}
