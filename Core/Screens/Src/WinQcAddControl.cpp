/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/NVM_QCSetup.hpp"

static char g_arrControlName[MAX_QC_CONTROL_NAME_LEN];
static char g_arrLotNumber[MAX_QC_LOT_NUM_LEN];
static char g_arrExpDate[9];
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinID_AddQcControl , 1, "b5");
static NexButton bSave = NexButton(en_WinID_AddQcControl , 4, "b7");
static NexText ControlName = NexText(en_WinID_AddQcControl , 5 , "t11");
static NexText LotNumber = NexText(en_WinID_AddQcControl , 8 , "t2");
static NexText ExpirtyDate = NexText(en_WinID_AddQcControl , 10, "t4");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bSave,
									 &ControlName,
									 &LotNumber,
									 &ExpirtyDate,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbSave(void *ptr);
static void HandlerControlName(void *ptr);
static void HandlerLotNumber(void *ptr);
static void HandlerbExpiryDate(void *ptr);

enWindowStatus ShowQcAddControlScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bSave.attachPush(HandlerbSave, &bSave);
	ControlName.attachPush(HandlerControlName, &ControlName);
	LotNumber.attachPush(HandlerLotNumber, &LotNumber);
	ExpirtyDate.attachPush(HandlerbExpiryDate, &ExpirtyDate);

	uint8_t u8SlotIndex = GetQcSetupMemorySlotIndex_QcControl();
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_NumericKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow && en_WinID_AlphaKeypad != PrevWindow)
	{
		g_arrControlName[0] = 0;
		g_arrLotNumber[0] = 0;
		g_arrExpDate[0] = 0;

		if(NVM_INIT_NOT_OK_FLAG != m_QcSetup->m_QcControls[u8SlotIndex].u8ValidFlag)
		{
			snprintf(g_arrExpDate , 9 , "%02u/%02u/%02u" ,
					(unsigned int)m_QcSetup->m_QcControls[u8SlotIndex].m_ExpiryDate.Date ,
					(unsigned int)m_QcSetup->m_QcControls[u8SlotIndex].m_ExpiryDate.Month ,
					(unsigned int)m_QcSetup->m_QcControls[u8SlotIndex].m_ExpiryDate.Year);

			strncpy(g_arrControlName ,
					m_QcSetup->m_QcControls[u8SlotIndex].arrControlName , MAX_QC_CONTROL_NAME_LEN);
			strncpy(g_arrLotNumber ,
					m_QcSetup->m_QcControls[u8SlotIndex].arRLotNum , MAX_QC_LOT_NUM_LEN);
		}
	}

	ControlName.setText(g_arrControlName);
	LotNumber.setText(g_arrLotNumber);
	ExpirtyDate.setText(g_arrExpDate);
	return WinStatus;
}

void HandlerQcAddControlScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinID_QcControl , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbSave(void *ptr)
{
	BeepBuzzer();
	uint8_t u8FreeSlotIndex = 0;
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	u8FreeSlotIndex = GetQcSetupMemorySlotIndex_QcControl();
	if(NVM_INIT_NOT_OK_FLAG != u8FreeSlotIndex &&
			u8FreeSlotIndex < enQcCntrl_Max)
	{

		for(uint8_t u8Idx = 0 ; u8Idx < 9 ; ++u8Idx)
		{
			if('-' == g_arrExpDate[u8Idx] || '.' == g_arrExpDate[u8Idx] || ' ' == g_arrExpDate[u8Idx])
			{
				g_arrExpDate[u8Idx] = 0;
			}
		}
	    if(g_arrExpDate[0] == 0 || g_arrExpDate[3] == 0 || g_arrExpDate[6] == 0)
	    {
	        for(uint8_t u8Idx = 0 ; u8Idx < 9 ; ++u8Idx)
	        {
	            g_arrExpDate[u8Idx] = 0;
	        }
	    }
		std::string strDate = std::string(&g_arrExpDate[0],2);
		std::string strMonth = std::string(&g_arrExpDate[3],2);
		std::string strYear = std::string(&g_arrExpDate[6],2);


        if (g_arrControlName[0] == 0)
        {
            if(enkeyOk == ShowMainPopUp("Quality Control","Control Name Should Not Be Empty", true))
            {
            	ChangeWindowPage(en_WinID_AddQcControl , (enWindowID)NULL);
            	stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
            }
        }
        else if(g_arrLotNumber[0] == 0)
        {
            if(enkeyOk == ShowMainPopUp("Quality Control","Lot Number Should Not Be Empty", true))
            {
            	ChangeWindowPage(en_WinID_AddQcControl , (enWindowID)NULL);
            	stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
            }
        }
        else if(g_arrExpDate[0] == 0)
        {
            if(enkeyOk == ShowMainPopUp("Quality Control","Expiry Date Should Not Be Empty", true))
            {
            	ChangeWindowPage(en_WinID_AddQcControl , (enWindowID)NULL);
            	stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
            }
        }
        else
		{
    		m_QcSetup->m_QcControls[u8FreeSlotIndex].m_ExpiryDate.Date = atoi(strDate.c_str());
    		m_QcSetup->m_QcControls[u8FreeSlotIndex].m_ExpiryDate.Month = atoi(strMonth.c_str());
    		m_QcSetup->m_QcControls[u8FreeSlotIndex].m_ExpiryDate.Year = atoi(strYear.c_str());

    		strncpy(m_QcSetup->m_QcControls[u8FreeSlotIndex].arrControlName ,
    				g_arrControlName , MAX_QC_CONTROL_NAME_LEN);

    		strncpy(m_QcSetup->m_QcControls[u8FreeSlotIndex].arRLotNum ,
    				g_arrLotNumber , MAX_QC_LOT_NUM_LEN);

    		m_QcSetup->m_QcControls[u8FreeSlotIndex].u8ValidFlag = NVM_INIT_OK_FLAG;
    		NVM_WriteQcSetup();
    		ChangeWindowPage(en_WinID_QcControl , (enWindowID)NULL);
		}
	}
//	else
//	{
//		/*Show memory full message popup*/
//		/*Show no results popup*/
//		/*Save upcomming window id before show popup page*/
//		enWindowID CurrWindow = stcScreenNavigation.CurrentWindowId;
//		if(enkeyOk == ShowMainPopUp("Quality Control","Memory Full", true))
//		{
//			ChangeWindowPage(en_WinID_QcControl , (enWindowID)NULL);
//			BeepBuzzer();
//		}
//	}

}
void HandlerControlName(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrControlName , sizeof(g_arrControlName) , false ,
			"Control Name" , g_arrControlName , sizeof(g_arrControlName) - 1);
	BeepBuzzer();
}
void HandlerLotNumber(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrLotNumber , sizeof(g_arrLotNumber) , false ,
			"Lot Number" , g_arrLotNumber , sizeof(g_arrLotNumber) - 1);
	BeepBuzzer();
}
void HandlerbExpiryDate(void *ptr)
{
	DateEntry = true;
	openKeyBoard(en_NumericKeyboard , g_arrExpDate , sizeof(g_arrExpDate) , false ,
			"Expiry Date (DD/MM/YY)" , g_arrExpDate , sizeof(g_arrExpDate) - 1);
	BeepBuzzer();
}
