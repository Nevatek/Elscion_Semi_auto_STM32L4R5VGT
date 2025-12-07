/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/NVM_QCSetup.hpp"

#define MAX_QC_ENTRY_PER_PAGE (5)

static int8_t  g_n8CurrPage = 0;
static uint8_t g_u8TotalPage = 0;
static uint8_t g_u8QcEntryIndex[enQcCntrl_Max] = {0};
static uint8_t g_u8NumOfQcFound = 0;
static bool g_bSelectionButtons[MAX_QC_ENTRY_PER_PAGE] = {false};
static bool g_bMasterSelectionButton = false;
static uint8_t g_u8LastSelectedRow = 0;
static uint8_t g_u8EditDataIndex = NVM_INIT_NOT_OK_FLAG;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinID_QcControl , 2, "b5");
static NexButton bUp = NexButton(en_WinID_QcControl , 8 , "b0");
static NexButton bDown = NexButton(en_WinID_QcControl , 9 , "b1");
static NexButton bTestControl = NexButton(en_WinID_QcControl , 6, "b3");
static NexButton bAdd = NexButton(en_WinID_QcControl , 10, "b4");
static NexButton bDelete = NexButton(en_WinID_QcControl , 11, "b6");
static NexButton bEdit = NexButton(en_WinID_QcControl , 12, "b7");
static NexButton bMasterSelection = NexButton(en_WinID_QcControl , 14 , "b8");

static NexButton bSelect[MAX_QC_ENTRY_PER_PAGE] = {
		NexButton(en_WinID_QcControl , 32, "b9"),
		NexButton(en_WinID_QcControl , 33, "b10"),
		NexButton(en_WinID_QcControl , 34, "b11"),
		NexButton(en_WinID_QcControl , 35, "b12"),
		NexButton(en_WinID_QcControl , 36, "b13"),
};

static NexText SelectBar[MAX_QC_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcControl , 43, "t24"),
		NexText(en_WinID_QcControl , 44, "t25"),
		NexText(en_WinID_QcControl , 45, "t26"),
		NexText(en_WinID_QcControl , 46, "t27"),
		NexText(en_WinID_QcControl , 47, "t28"),
};

static NexText SrNo[MAX_QC_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcControl , 38, "t14"),
		NexText(en_WinID_QcControl , 39, "t20"),
		NexText(en_WinID_QcControl , 40, "t21"),
		NexText(en_WinID_QcControl , 41, "t22"),
		NexText(en_WinID_QcControl , 42, "t23"),
};

static NexText ControlName[MAX_QC_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcControl , 17, "t3"),
		NexText(en_WinID_QcControl , 18, "t4"),
		NexText(en_WinID_QcControl , 19, "t5"),
		NexText(en_WinID_QcControl , 20, "t6"),
		NexText(en_WinID_QcControl , 21, "t7"),
};

static NexText LotNumber[MAX_QC_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcControl , 22, "t9"),
		NexText(en_WinID_QcControl , 23, "t10"),
		NexText(en_WinID_QcControl , 24, "t11"),
		NexText(en_WinID_QcControl , 25, "t12"),
		NexText(en_WinID_QcControl , 26, "t13"),
};

static NexText ExpDate[MAX_QC_ENTRY_PER_PAGE] = {
		NexText(en_WinID_QcControl , 27, "t15"),
		NexText(en_WinID_QcControl , 28, "t16"),
		NexText(en_WinID_QcControl , 29, "t17"),
		NexText(en_WinID_QcControl , 30, "t18"),
		NexText(en_WinID_QcControl , 31, "t19"),
};
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bUp,
									 &bDown,
									 &bTestControl,
									 &bAdd,
									 &bDelete,
									 &bEdit,
									 &bSelect[0],&bSelect[1],
									 &bSelect[2],&bSelect[3],
									 &bSelect[4],
									 &SelectBar[0],&SelectBar[1],
									 &SelectBar[2],&SelectBar[3],
									 &SelectBar[4],
									 &bMasterSelection,
									 NULL};


static void HandlerbBack(void *ptr);
static void HandlerbUp(void *ptr);
static void HandlerbDown(void *ptr);
static void HandlerbTestControl(void *ptr);
static void HandlerbAdd(void *ptr);
static void HandlerbDelete(void *ptr);
static void HandlerbEdit(void *ptr);
static void HandlerbSelect0(void *ptr);
static void HandlerbSelect1(void *ptr);
static void HandlerbSelect2(void *ptr);
static void HandlerbSelect3(void *ptr);
static void HandlerbSelect4(void *ptr);
static void HandlerbMasterSelection(void *ptr);

static uint8_t FindNumOfControlSaved(void);
void ShowQcEntries(void);
static uint8_t Get_EntryId(uint8_t u8Row);
static void ShowHideButtons(void);
static uint8_t GetQcSetup_FreeIndexPos(void);

enWindowStatus ShowQcControlViewScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bUp.attachPush(HandlerbUp, &bUp);
	bDown.attachPush(HandlerbDown, &bDown);
	bTestControl.attachPush(HandlerbTestControl, &bTestControl);
	bAdd.attachPush(HandlerbAdd, &bAdd);
	bDelete.attachPush(HandlerbDelete, &bDelete);
	bEdit.attachPush(HandlerbEdit, &bEdit);
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

	NVM_ReadQcSetup();
	g_n8CurrPage = 0;
	g_bMasterSelectionButton = false;
	ShowQcEntries();
	memset(g_bSelectionButtons , false , sizeof(g_bSelectionButtons));
	ShowHideButtons();
	return WinStatus;
}

void HandlerQcControlViewScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
	BeepBuzzer();
}

static void HandlerbUp(void *ptr)
{
	g_n8CurrPage--;
	if(0 > g_n8CurrPage)
	{
		g_n8CurrPage = g_u8TotalPage - 1;
	}
	ShowQcEntries();
	BeepBuzzer();
}

static void HandlerbDown(void *ptr)
{
	g_n8CurrPage++;
	if(g_u8TotalPage <= g_n8CurrPage)
	{
		g_n8CurrPage = 0;
	}
	ShowQcEntries();
	BeepBuzzer();
}
void HandlerbTestControl(void *ptr)
{
	ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbAdd(void *ptr)
{
	BeepBuzzer();
	g_u8EditDataIndex = GetQcSetup_FreeIndexPos();
	if (g_u8EditDataIndex == NVM_INIT_NOT_OK_FLAG)
	{
		if(enkeyOk == ShowMainPopUp("Quality Control","Maximum Controls Added", true))
		{
			ChangeWindowPage(en_WinID_QcControl , (enWindowID)NULL);
		}
	}
	else
	{
		ChangeWindowPage(en_WinID_AddQcControl , (enWindowID)NULL);
	}

}
void HandlerbDelete(void *ptr)
{
	BeepBuzzer();
	uint8_t u8SlotIndex = 0;
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enKeyYes == ShowMainPopUp("Quality Control","Do you want to delete?", false))
	{
		for(uint8_t u8Idx = 0; u8Idx < MAX_QC_ENTRY_PER_PAGE ; ++u8Idx)
		{
			if(true == g_bMasterSelectionButton
					|| true == g_bSelectionButtons[u8Idx])
			{
				u8SlotIndex = Get_EntryId(u8Idx);
				m_QcSetup->m_QcControls[u8SlotIndex].arRLotNum[0] = '\0';
				m_QcSetup->m_QcControls[u8SlotIndex].arrControlName[0] = '\0';
				m_QcSetup->m_QcControls[u8SlotIndex].m_ExpiryDate.Date = 0;
				m_QcSetup->m_QcControls[u8SlotIndex].m_ExpiryDate.Month = 0;
				m_QcSetup->m_QcControls[u8SlotIndex].m_ExpiryDate.Year = 0;
				m_QcSetup->m_QcControls[u8SlotIndex].u8ValidFlag = NVM_INIT_NOT_OK_FLAG;

				for(uint8_t nI = 0 ; nI < MAX_NUM_OF_TEST_PARAMS_SAVED ; ++nI)
				{
					for(uint8_t nIdx = 0 ; nIdx < MAX_NUM_OF_QC_CONTROL_PER_TEST ; ++nIdx)
					{
						if(u8SlotIndex == m_QcSetup->m_QcTestSetup[nI][nIdx].u8QcIdx)
						{
							m_QcSetup->m_QcTestSetup[nI][nIdx].u8QcIdx = NVM_INIT_NOT_OK_FLAG;
							m_QcSetup->m_QcTestSetup[nI][nIdx].fHighVal = 0;
							m_QcSetup->m_QcTestSetup[nI][nIdx].fLowVal = 0;
							m_QcSetup->m_QcTestSetup[nI][nIdx].fMeanVal = 0;
							m_QcSetup->m_QcTestSetup[nI][nIdx].u8NumOfResSaved = 0;
						}
					}
				}
			}
		}
		NVM_WriteQcSetup();
	}
	stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
	ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
}
void HandlerbEdit(void *ptr)
{
	g_u8EditDataIndex = Get_EntryId(g_u8LastSelectedRow);
	ChangeWindowPage(en_WinID_AddQcControl , (enWindowID)NULL);
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
		for(uint8_t u8Idx = 0; u8Idx < MAX_QC_ENTRY_PER_PAGE ; ++u8Idx)
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
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_ENTRY_PER_PAGE ; ++u8Idx)
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
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_ENTRY_PER_PAGE ; ++u8Idx)
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
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_ENTRY_PER_PAGE ; ++u8Idx)
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
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_ENTRY_PER_PAGE ; ++u8Idx)
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
	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_ENTRY_PER_PAGE ; ++u8Idx)
	{
		g_bSelectionButtons[u8Idx] = false;
	}
	g_bSelectionButtons[u8ButtonId] = true;
	ShowHideButtons();
	BeepBuzzer();
}

void ShowQcEntries(void)
{
	char arr_cTimeStamp[32] = {0};
	char arr_cSerialNo[32] = {0};
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();

	g_u8NumOfQcFound = FindNumOfControlSaved();
	if(0 < g_u8NumOfQcFound)
	{
		g_u8TotalPage = (uint16_t)abs(g_u8NumOfQcFound / MAX_QC_ENTRY_PER_PAGE);
		g_u8TotalPage++;
		/*Added to avoid displaying empty pages*/
		if (g_u8NumOfQcFound % MAX_QC_ENTRY_PER_PAGE == 0)
		{
			g_u8TotalPage--;
		}

		for(uint8_t SlNo = 0 ; SlNo < MAX_QC_ENTRY_PER_PAGE; ++SlNo)
		{
			ControlName[SlNo].setText(" ");
			LotNumber[SlNo].setText(" ");
			ExpDate[SlNo].setText(" ");
			SrNo[SlNo].setText(" ");
			bSelect[SlNo].setVisible(false);
			SelectBar[SlNo].setVisible(false);
		}
		bMasterSelection.setVisible(false);

		for(uint8_t SlNo = 0 , CntrlIdx = MAX_QC_ENTRY_PER_PAGE * g_n8CurrPage
								; SlNo < MAX_QC_ENTRY_PER_PAGE && CntrlIdx < g_u8NumOfQcFound; )
		{
			uint8_t u8DataIdx = g_u8QcEntryIndex[CntrlIdx];
			if(NVM_INIT_OK_FLAG == m_QcSetup->m_QcControls[u8DataIdx].u8ValidFlag)
			{
				bSelect[SlNo].setVisible(true);
				bMasterSelection.setVisible(true);
				SelectBar[SlNo].setVisible(true);
				snprintf(arr_cTimeStamp , 31 , "%02u/%02u/%02u" ,
						(unsigned int)m_QcSetup->m_QcControls[u8DataIdx].m_ExpiryDate.Date ,
						(unsigned int)m_QcSetup->m_QcControls[u8DataIdx].m_ExpiryDate.Month ,
						(unsigned int)m_QcSetup->m_QcControls[u8DataIdx].m_ExpiryDate.Year);

				ControlName[SlNo].setText(m_QcSetup->m_QcControls[u8DataIdx].arrControlName);
				LotNumber[SlNo].setText(m_QcSetup->m_QcControls[u8DataIdx].arRLotNum);
				ExpDate[SlNo].setText(arr_cTimeStamp);

				snprintf(arr_cSerialNo , sizeof(arr_cSerialNo) , "%u" , 1 + (SlNo + (g_n8CurrPage * MAX_QC_ENTRY_PER_PAGE)));
				SrNo[SlNo].setText(arr_cSerialNo);

				++SlNo;
			}
			CntrlIdx++;
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

	for(uint8_t u8Idx = 0; u8Idx < MAX_QC_ENTRY_PER_PAGE ; ++u8Idx)
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
		if(1 == u8NumOfSelection)
		{
			bEdit.setVisible(true);
		}
		else
		{
			bEdit.setVisible(false);
		}
	}
	else
	{
		bDelete.setVisible(false);
		bEdit.setVisible(false);
	}
}
uint8_t Get_EntryId(uint8_t u8Row)
{
	uint8_t u8CntrlIdx = 0;
	u8CntrlIdx = u8Row + (MAX_QC_ENTRY_PER_PAGE * g_n8CurrPage);
	return g_u8QcEntryIndex[u8CntrlIdx];
}
uint8_t FindNumOfControlSaved(void)
{
	uint8_t u8Count = 0;
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	for(uint8_t nI = enQcCntrl_0 ; nI < enQcCntrl_Max ; ++nI)
	{
		if(NVM_INIT_OK_FLAG == m_QcSetup->m_QcControls[nI].u8ValidFlag)
		{
			g_u8QcEntryIndex[u8Count] = nI;
			u8Count++;
		}
	}
	return u8Count;
}

uint8_t GetQcSetup_FreeIndexPos(void)
{
	uint8_t u8SlotIndex = NVM_INIT_NOT_OK_FLAG;
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	for(uint8_t nI = enQcCntrl_0 ; nI < enQcCntrl_Max ; ++nI)
	{
		if(NVM_INIT_NOT_OK_FLAG == m_QcSetup->m_QcControls[nI].u8ValidFlag)
		{
			u8SlotIndex = nI;
			break;
		}
	}
	return u8SlotIndex;
}
uint8_t GetQcSetupMemorySlotIndex_QcControl(void)
{
	uint8_t u8SlotIndex = g_u8EditDataIndex;
	return u8SlotIndex;
}
