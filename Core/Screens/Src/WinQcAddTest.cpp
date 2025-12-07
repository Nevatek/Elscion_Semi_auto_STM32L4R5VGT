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

static uint8_t g_u8ControlLostIdx[enQcCntrl_Max] = {NVM_INIT_NOT_OK_FLAG};
static uint8_t g_u8SeletedControlIdx = NVM_INIT_NOT_OK_FLAG;
static uint8_t g_u8SeletedControlbox = NVM_INIT_NOT_OK_FLAG;
static float fPSD = 0;
static float fMean = 0;
static float fMSD = 0;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinID_QcAddTest , 2, "b5");
static NexButton bSave = NexButton(en_WinID_QcAddTest , 5, "b7");

static NexButton bSelectControl[enQcCntrl_Max] = {
		NexButton(en_WinID_QcAddTest , 9,  "b8"),//0
		NexButton(en_WinID_QcAddTest , 6,  "b1"),//1
		NexButton(en_WinID_QcAddTest , 7,  "b4"),//2
		NexButton(en_WinID_QcAddTest , 8,  "b6"),//3
		NexButton(en_WinID_QcAddTest , 17, "b10"),//4

		NexButton(en_WinID_QcAddTest , 22, "b16"),//5
		NexButton(en_WinID_QcAddTest , 21, "b15"),//6
		NexButton(en_WinID_QcAddTest , 20, "b14"),//7
		NexButton(en_WinID_QcAddTest , 19, "b13"),//8
		NexButton(en_WinID_QcAddTest , 18, "b12"),//9

		NexButton(en_WinID_QcAddTest , 27, "b21"),//10
		NexButton(en_WinID_QcAddTest , 26, "b20"),//11
		NexButton(en_WinID_QcAddTest , 25, "b19"),//12
		NexButton(en_WinID_QcAddTest , 24, "b18"),//13
		NexButton(en_WinID_QcAddTest , 23, "b17"),//14
};

static NexButton strPSD = NexButton(en_WinID_QcAddTest , 11 , "b0");
static NexButton strMean = NexButton(en_WinID_QcAddTest , 12 , "b9");
static NexButton strMSD = NexButton(en_WinID_QcAddTest , 13 , "b11");
static NexText constPSD = NexText(en_WinID_QcAddTest , 14 , "t2");
static NexText constMean = NexText(en_WinID_QcAddTest , 15 , "t3");
static NexText constMSD = NexText(en_WinID_QcAddTest , 16 , "t4");
//static NexText TestName = NexText(en_WinID_QcAddTest , 28 , "t5");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bSave,
									 &strPSD,
									 &strMean,
									 &strMSD,
									 &bSelectControl[0],&bSelectControl[1],&bSelectControl[2],
									 &bSelectControl[3],&bSelectControl[4],&bSelectControl[5],
									 &bSelectControl[6],&bSelectControl[7],&bSelectControl[8],
									 &bSelectControl[9],&bSelectControl[10],&bSelectControl[11],
									 &bSelectControl[12],&bSelectControl[13],&bSelectControl[14],
									 NULL};


static void HandlerbBack(void *ptr);
static void HandlerbSave(void *ptr);

static void HandlerbPSD(void *ptr);
static void HandlerbMean(void *ptr);
static void HandlerbMSD(void *ptr);

static void HandlerSelect0(void *ptr);
static void HandlerSelect1(void *ptr);
static void HandlerSelect2(void *ptr);
static void HandlerSelect3(void *ptr);
static void HandlerSelect4(void *ptr);
static void HandlerSelect5(void *ptr);
static void HandlerSelect6(void *ptr);
static void HandlerSelect7(void *ptr);
static void HandlerSelect8(void *ptr);
static void HandlerSelect9(void *ptr);
static void HandlerSelect10(void *ptr);
static void HandlerSelect11(void *ptr);
static void HandlerSelect12(void *ptr);
static void HandlerSelect13(void *ptr);
static void HandlerSelect14(void *ptr);

static void ShowControlList(void);
static void UpdateUserInputWindow(void);


enWindowStatus ShowQcAddTestScreen (NexPage *ptr_obJCurrPage)
{
	char g_arrPSD[32] = {0};
	char g_arrMean[32] = {0};
	char g_arrMSD[32] = {0};
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bSave.attachPush(HandlerbSave, &bSave);
	strPSD.attachPush(HandlerbPSD, &strPSD);
	strMean.attachPush(HandlerbMean, &strMean);
	strMSD.attachPush(HandlerbMSD, &strMSD);

	bSelectControl[0].attachPush(HandlerSelect0, &bSelectControl[0]);//0
	bSelectControl[1].attachPush(HandlerSelect1, &bSelectControl[1]);//1
	bSelectControl[2].attachPush(HandlerSelect2, &bSelectControl[2]);//2
	bSelectControl[3].attachPush(HandlerSelect3, &bSelectControl[3]);//3
	bSelectControl[4].attachPush(HandlerSelect4, &bSelectControl[4]);//4
	bSelectControl[5].attachPush(HandlerSelect5, &bSelectControl[5]);//5
	bSelectControl[6].attachPush(HandlerSelect6, &bSelectControl[6]);//6
	bSelectControl[7].attachPush(HandlerSelect7, &bSelectControl[7]);//7
	bSelectControl[8].attachPush(HandlerSelect8, &bSelectControl[8]);//8
	bSelectControl[9].attachPush(HandlerSelect9, &bSelectControl[9]);//9
	bSelectControl[10].attachPush(HandlerSelect10, &bSelectControl[10]);//10
	bSelectControl[11].attachPush(HandlerSelect11, &bSelectControl[11]);//11
	bSelectControl[12].attachPush(HandlerSelect12, &bSelectControl[12]);//12
	bSelectControl[13].attachPush(HandlerSelect13, &bSelectControl[13]);//13
	bSelectControl[14].attachPush(HandlerSelect14, &bSelectControl[14]);//14

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_NumericKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
	{
		fPSD = 0;
		fMean = 0;
		fMSD = 0;
		g_arrPSD[0] = 0;
		g_arrMean[0] = 0;
		g_arrMSD[0] = 0;
		g_u8SeletedControlIdx = NVM_INIT_NOT_OK_FLAG;
		g_u8SeletedControlbox = NVM_INIT_NOT_OK_FLAG;

	}
	else
	{
		snprintf(g_arrPSD , 31 , "%.02f" , fPSD);
		snprintf(g_arrMean , 31 , "%.02f" , fMean);
		snprintf(g_arrMSD , 31 , "%.02f" , fMSD);
	}

	strPSD.setText(g_arrPSD);
	strMean.setText(g_arrMean);
	strMSD.setText(g_arrMSD);

	ShowControlList();
	UpdateUserInputWindow();
	return WinStatus;
}

void HandlerQcAddTesScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbSave(void *ptr)
{
	BeepBuzzer();
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	uint8_t u8FreeSlot = GetQcTestSetupFreeSlotIndex_QcTestSetup();
	if(NVM_INIT_NOT_OK_FLAG != u8FreeSlot)
	{
		float QcLvlConc[3];
		QcLvlConc[0] = fPSD;
		QcLvlConc[1] = fMean;
		QcLvlConc[2] = fMSD;

		if( fPSD == 0 || fMean == 0)
		{
			if(enkeyOk == ShowMainPopUp("QC Limits","Mean and SD Should Not Be Zero", true))
			{
				ChangeWindowPage(en_WinID_QcAddTest , (enWindowID)NULL);
				stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
			}
		}
		else if(isInIncreasingOrder(QcLvlConc, 2) == false)
		{
			if(enkeyOk == ShowMainPopUp("QC Limits","Mean should be greater than SD", true))
			{
				ChangeWindowPage(en_WinID_QcAddTest , (enWindowID)NULL);
				stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
			}
		}
		else
		{
			m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8FreeSlot].fHighVal = fPSD; // This is +1SD value Without changing QC setup we are saving the SD Val
			m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8FreeSlot].fMeanVal = fMean;
			m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8FreeSlot].fLowVal = fMSD;   // This is -1SD Value
			m_QcSetup->m_QcTestSetup[stcTestData.TestId][u8FreeSlot].u8QcIdx = g_u8SeletedControlIdx;
			NVM_WriteQcSetup();
			InitQcData(stcTestData.TestId);
			ChangeWindowPage(en_WinID_QcTestScreen , (enWindowID)NULL);
		}
	}
}

void HandlerbPSD(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &fPSD ,
			sizeof(fPSD) , true ,"Enter SD Value" , " " , 7);
	BeepBuzzer();
}
void HandlerbMean(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &fMean ,
			sizeof(fMean) , true ,"Enter Mean Value" , " " , 7);
	BeepBuzzer();
}
void HandlerbMSD(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &fMSD ,
			sizeof(fMSD) , true ,"Enter -1SD Value" , " " , 7);
	BeepBuzzer();
}
void HandlerSelect0(void *ptr)
{
	uint8_t u8Count = 0;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect1(void *ptr)
{
	uint8_t u8Count = 1;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect2(void *ptr)
{
	uint8_t u8Count = 2;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect3(void *ptr)
{
	uint8_t u8Count = 3;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect4(void *ptr)
{
	uint8_t u8Count = 4;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect5(void *ptr)
{
	uint8_t u8Count = 5;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect6(void *ptr)
{
	uint8_t u8Count = 6;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect7(void *ptr)
{
	uint8_t u8Count = 7;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect8(void *ptr)
{
	uint8_t u8Count = 8;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect9(void *ptr)
{
	uint8_t u8Count = 9;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect10(void *ptr)
{
	uint8_t u8Count = 10;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect11(void *ptr)
{
	uint8_t u8Count = 11;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect12(void *ptr)
{
	uint8_t u8Count = 12;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect13(void *ptr)
{
	uint8_t u8Count = 13;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}
void HandlerSelect14(void *ptr)
{
	uint8_t u8Count = 14;
	g_u8SeletedControlbox = u8Count;
	g_u8SeletedControlIdx = g_u8ControlLostIdx[u8Count];
	UpdateUserInputWindow();
	BeepBuzzer();
}

void ShowControlList(void)
{
	char arrBuff[32] = {0};
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	for(uint8_t u8Idx = 0 , u8Count = 0; u8Idx < enQcCntrl_Max ; ++u8Idx)
	{
		if(NVM_INIT_OK_FLAG == m_QcSetup->m_QcControls[u8Idx].u8ValidFlag &&
				u8Idx != m_QcSetup->m_QcTestSetup[stcTestData.TestId][0].u8QcIdx &&
				u8Idx != m_QcSetup->m_QcTestSetup[stcTestData.TestId][1].u8QcIdx &&
				u8Idx != m_QcSetup->m_QcTestSetup[stcTestData.TestId][2].u8QcIdx &&
				u8Idx != m_QcSetup->m_QcTestSetup[stcTestData.TestId][3].u8QcIdx)
		{
			g_u8ControlLostIdx[u8Count] = u8Idx;
			bSelectControl[u8Count].setVisible(true);

			snprintf(arrBuff , 31 , "%s-%s", m_QcSetup->m_QcControls[u8Idx].arrControlName,m_QcSetup->m_QcControls[u8Idx].arRLotNum);
			bSelectControl[u8Count].setText(arrBuff);
			u8Count++;
		}
	}
//	TestName.setText(GetInstance_TestParams()[stcTestData.TestId].arrTestName);
}

void UpdateUserInputWindow(void)
{
	for(uint8_t u8Idx = 0; u8Idx < enQcCntrl_Max ; ++u8Idx)
	{
		bSelectControl[u8Idx].Set_font_color_pco(COLOUR_BLACK);
		bSelectControl[u8Idx].Set_background_image_pic(474);
	}
	if(NVM_INIT_NOT_OK_FLAG != 	g_u8SeletedControlbox)
	{
		bSave.setVisible(true);
		strPSD.setVisible(true);
		strMean.setVisible(true);
		strMSD.setVisible(true);
		constPSD.setVisible(true);
		constMean.setVisible(true);
		constMSD.setVisible(true);
		bSelectControl[g_u8SeletedControlbox].Set_font_color_pco(COLOUR_WHITE);
		bSelectControl[g_u8SeletedControlbox].Set_background_image_pic(561);
	}
}
