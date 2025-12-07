/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestResultsHistory.hpp"
#include "Printer.h"

#define MAX_PAGE_RESULT_HISTORY (TOTAL_NUM_OF_RESULTS_SAVED / MAX_RESULT_PER_WINDOW)

static bool g_ErrNoResFoundFlag = false;
static bool g_masterSelect = false;
static uint16_t g_u16CurrPageNum = 0;
static uint8_t g_u8Slno = 0;
uint16_t u16ResultsFOund = 0;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bPResultsBack = NexButton(en_WinId_HistoryMMenu , 1, "b5");
static NexButton bButtonDown = NexButton(en_WinId_HistoryMMenu , 5 , "b0");
static NexButton bButtonUp = NexButton(en_WinId_HistoryMMenu , 6 , "b1");
static NexButton bButtonPrint = NexButton(en_WinId_HistoryMMenu , 7 , "b2");
static NexButton bButtonView = NexButton(en_WinId_HistoryMMenu , 8 , "b3");
static NexButton bButtonDelete = NexButton(en_WinId_HistoryMMenu , 9 , "b4");
static NexButton bRbMasterSelect = NexButton(en_WinId_HistoryMMenu , 42 , "b6");

static NexText tSearchtypeNameText = NexText(en_WinId_HistoryMMenu , 15 , "t5");
static NexText tPatientIDText = NexText(en_WinId_HistoryMMenu , 50 , "t34");
static NexText tSearchtypeText = NexText(en_WinId_HistoryMMenu , 16 , "t6");
static NexText tDatetoText = NexText(en_WinId_HistoryMMenu , 48 , "t32");
static NexText tEndDateText = NexText(en_WinId_HistoryMMenu , 49 , "t33");

static NexButton bRadioButtons[MAX_RESULT_PER_WINDOW] = {NexButton(en_WinId_HistoryMMenu , 43 , "b7"),
														NexButton(en_WinId_HistoryMMenu , 44 , "b8"),
														NexButton(en_WinId_HistoryMMenu , 45 , "b9"),
														NexButton(en_WinId_HistoryMMenu , 46 , "b10"),
														NexButton(en_WinId_HistoryMMenu , 47 , "b11")};

static NexText SelectBar[MAX_RESULT_PER_WINDOW] = {NexText(en_WinId_HistoryMMenu , 57, "t35"),
													NexText(en_WinId_HistoryMMenu , 58, "t36"),
													NexText(en_WinId_HistoryMMenu , 59, "t37"),
													NexText(en_WinId_HistoryMMenu , 60, "t38"),
													NexText(en_WinId_HistoryMMenu , 61, "t39")};

static NexText tTextSlNo[MAX_RESULT_PER_WINDOW] = {NexText(en_WinId_HistoryMMenu , 17, "t7"),
													NexText(en_WinId_HistoryMMenu , 18, "t8"),
													NexText(en_WinId_HistoryMMenu , 19, "t9"),
													NexText(en_WinId_HistoryMMenu , 20, "t10"),
													NexText(en_WinId_HistoryMMenu , 21, "t11")};

static NexText tTextPatientId[MAX_RESULT_PER_WINDOW] = {NexText(en_WinId_HistoryMMenu , 22, "t12"),
													NexText(en_WinId_HistoryMMenu , 23, "t13"),
													NexText(en_WinId_HistoryMMenu , 24, "t14"),
													NexText(en_WinId_HistoryMMenu , 25, "t15"),
													NexText(en_WinId_HistoryMMenu , 26, "t16")};

static NexText tTextDate[MAX_RESULT_PER_WINDOW] = {NexText(en_WinId_HistoryMMenu , 27, "t17"),
													NexText(en_WinId_HistoryMMenu , 28, "t18"),
													NexText(en_WinId_HistoryMMenu , 29, "t19"),
													NexText(en_WinId_HistoryMMenu , 30, "t20"),
													NexText(en_WinId_HistoryMMenu , 31, "t21")};

static NexText tTextTime[MAX_RESULT_PER_WINDOW] = {NexText(en_WinId_HistoryMMenu , 52, "t41"),
													NexText(en_WinId_HistoryMMenu , 53, "t42"),
													NexText(en_WinId_HistoryMMenu , 54, "t43"),
													NexText(en_WinId_HistoryMMenu , 55, "t44"),
													NexText(en_WinId_HistoryMMenu , 56, "t45")};

static NexText tTextTestName[MAX_RESULT_PER_WINDOW] = {NexText(en_WinId_HistoryMMenu , 32, "t22"),
													NexText(en_WinId_HistoryMMenu , 33, "t23"),
													NexText(en_WinId_HistoryMMenu , 34, "t24"),
													NexText(en_WinId_HistoryMMenu , 35, "t25"),
													NexText(en_WinId_HistoryMMenu , 36, "t26")};

static NexText tTextResult[MAX_RESULT_PER_WINDOW] = {NexText(en_WinId_HistoryMMenu , 37, "t27"),
													NexText(en_WinId_HistoryMMenu , 38, "t28"),
													NexText(en_WinId_HistoryMMenu , 39, "t29"),
													NexText(en_WinId_HistoryMMenu , 40, "t30"),
													NexText(en_WinId_HistoryMMenu , 41, "t31")};
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bPResultsBack,
									 &bButtonUp,
									 &bButtonDown,
									 &bButtonPrint,
									 &bButtonView,
									 &bButtonDelete,
									 &bRbMasterSelect,
									 &bRadioButtons[0],
									 &bRadioButtons[1],
									 &bRadioButtons[2],
									 &bRadioButtons[3],
									 &bRadioButtons[4],
									 &SelectBar[0],
									 &SelectBar[1],
									 &SelectBar[2],
									 &SelectBar[3],
									 &SelectBar[4],
									 NULL};

static void HandlerbPResultsBack(void *ptr);
static void HandlerbButtonUp(void *ptr);
static void HandlerbButtonDown(void *ptr);
static void HandlerbButtonPrint(void *ptr);
static void HandlerbButtonView(void *ptr);
static void HandlerbButtonDelete(void *ptr);
static void HandlerRbMasterSelect(void *ptr);
static void HandlerRbRadioButtons0(void *ptr);
static void HandlerRbRadioButtons1(void *ptr);
static void HandlerRbRadioButtons2(void *ptr);
static void HandlerRbRadioButtons3(void *ptr);
static void HandlerRbRadioButtons4(void *ptr);
static void ShowUpdateHistoryList(void);
static void UpdateSelectionradioButton(void);

enWindowStatus ShowPResultsScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bPResultsBack.attachPush(HandlerbPResultsBack, &bPResultsBack);
	bButtonUp.attachPush(HandlerbButtonUp, &bButtonUp);
	bButtonDown.attachPush(HandlerbButtonDown, &bButtonDown);
	bButtonPrint.attachPush(HandlerbButtonPrint, &bButtonPrint);
	bButtonView.attachPush(HandlerbButtonView, &bButtonView);
	bButtonDelete.attachPush(HandlerbButtonDelete, &bButtonDelete);
	bRbMasterSelect.attachPush(HandlerRbMasterSelect, &bRbMasterSelect);
	bRadioButtons[0].attachPush(HandlerRbRadioButtons0, &bRadioButtons[0]);
	bRadioButtons[1].attachPush(HandlerRbRadioButtons1, &bRadioButtons[1]);
	bRadioButtons[2].attachPush(HandlerRbRadioButtons2, &bRadioButtons[2]);
	bRadioButtons[3].attachPush(HandlerRbRadioButtons3, &bRadioButtons[3]);
	bRadioButtons[4].attachPush(HandlerRbRadioButtons4, &bRadioButtons[4]);

	SelectBar[0].attachPush(HandlerRbRadioButtons0, &SelectBar[0]);
	SelectBar[1].attachPush(HandlerRbRadioButtons1, &SelectBar[1]);
	SelectBar[2].attachPush(HandlerRbRadioButtons2, &SelectBar[2]);
	SelectBar[3].attachPush(HandlerRbRadioButtons3, &SelectBar[3]);
	SelectBar[4].attachPush(HandlerRbRadioButtons4, &SelectBar[4]);


	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_AlphaKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow &&
			en_WinID_HistView != PrevWindow)
	{
		g_u16CurrPageNum = 0;
	}

	SearchResultHistDatabase();
	u16ResultsFOund = GetInstance_SearchResultHistory()->u16ItemsFound;
	if(0 < u16ResultsFOund)
	{
		g_ErrNoResFoundFlag = false;
		ShowUpdateHistoryList();
	}
	else
	{
		g_ErrNoResFoundFlag = true;
	}

	UpdateSelectionradioButton();
	return WinStatus;
}

void HandlerPResultsScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
	if(true == g_ErrNoResFoundFlag)
	{
		/*Show no results popup*/
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enkeyOk == ShowMainPopUp("Results","No Results Found", true))
		{
			stcScreenNavigation.CurrentWindowId = NextWindow;
		}
		ChangeWindowPage(en_WinID_HistSearch , (enWindowID)NULL);
	}
}

void HandlerbPResultsBack(void *ptr)
{
	/*Added to disable selection box if press back*/
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
	{
		p_Search->bItemSelectionStatus[u8Idx] = false;
	}
	g_masterSelect = false;
	ChangeWindowPage(en_WinID_HistSearch , (enWindowID)NULL);
	BeepBuzzer();
}

void ShowUpdateHistoryList(void)
{
	en_RESULT_HIST_SEARCH m_SearchType = GetCurrSearchType();
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	ResHistoryDatabase *p_HistDatabase = GetInstance_ResultHistoryDatabase();
	uint16_t u16Block = 0;
	uint16_t u16TarIndexPos = 0;
	g_u8Slno = 0;
	char	g_arrBuff[16] = {0};

	if(enSearchDate == m_SearchType)
	{
		tSearchtypeNameText.setVisible(true);
		tSearchtypeNameText.setText("Date");

		snprintf(g_arrBuff, 15 , "%02d/%02d/%02d" ,p_Search->m_DateStart.Date , p_Search->m_DateStart.Month , p_Search->m_DateStart.Year);
		tSearchtypeText.setVisible(true);
		tSearchtypeText.setText(g_arrBuff);

		if(p_Search->m_DateEnd.Date != 0 && p_Search->m_DateEnd.Month != 0 && p_Search->m_DateEnd.Year !=0 )
		{
		tDatetoText.setVisible(true);
		snprintf(g_arrBuff, 15 , "%02d/%02d/%02d" ,p_Search->m_DateEnd.Date , p_Search->m_DateEnd.Month , p_Search->m_DateEnd.Year);
		tEndDateText.setVisible(true);
		tEndDateText.setText(g_arrBuff);
		}
	}
	else if(enSearchMethod == m_SearchType)
	{
		tSearchtypeNameText.setVisible(true);
		tSearchtypeNameText.setText("Method");

		switch(p_Search->m_TestType)
			{
				case en_Endpoint:
					tSearchtypeText.setVisible(true);
					tSearchtypeText.setText("End Point");
					break;
				case en_FixedTime:
					tSearchtypeText.setVisible(true);
					tSearchtypeText.setText("Fixed Time");
					break;
				case en_Kinetics:
					tSearchtypeText.setVisible(true);
					tSearchtypeText.setText("Kinetics");
					break;
				case en_Absorbance:
					tSearchtypeText.setVisible(true);
					tSearchtypeText.setText("Absorbance");
					break;
//				case en_CoagulationTest:
//					tSearchtypeText.setVisible(true);
//					tSearchtypeText.setText("Coagulation");
//					break;
				case en_Turbidimetry:
					tSearchtypeText.setVisible(true);
					tSearchtypeText.setText("Turbi - Fix Time");
					break;
//				case en_Differential:
//					tSearchtypeText.setVisible(false);
//					break;
				default:
					tSearchtypeText.setVisible(false);
					break;
			}
	}
	else if(enSearchTestname == m_SearchType)
	{
		tSearchtypeNameText.setVisible(true);
		tSearchtypeNameText.setText("Test");

		tSearchtypeText.setVisible(true);
		tSearchtypeText.setText(p_Search->arrBuff);
	}
	else if(enSearchPatientId == m_SearchType)
	{
		tSearchtypeNameText.setVisible(true);
		tSearchtypeNameText.setText("Patient ID");

		tPatientIDText.setVisible(true);
		tPatientIDText.setText(p_Search->arrBuff);
	}
	else if(enSearchUser == m_SearchType)
	{
		tSearchtypeNameText.setVisible(true);
		tSearchtypeNameText.setText("User");

		tSearchtypeText.setVisible(true);
		tSearchtypeText.setText(p_Search->arrBuff);
	}


	for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
	{
		tTextSlNo[u8Idx].setText(" ");
		tTextPatientId[u8Idx].setText(" ");
		tTextDate[u8Idx].setText(" ");
		tTextTime[u8Idx].setText(" ");
		tTextTestName[u8Idx].setText(" ");
		tTextResult[u8Idx].setText(" ");
	}

	for(uint16_t u16Idx = g_u16CurrPageNum * MAX_RESULT_PER_WINDOW ; u16Idx < p_Search->u16ItemsFound && u16Idx < TOTAL_NUM_OF_RESULTS_SAVED ; ++u16Idx)
	{
		if((-1) != p_Search->n16SearchItemIndex[u16Idx])
		{
			char arr_cDateStamp[32] = {0};
			char arr_cTimeStamp[32] = {0};
			char arr_cResult[32] = {0};
			char arr_cSerialNo[32] = {0};

			u16Block = uint32_t(p_Search->n16SearchItemIndex[u16Idx] / MAX_RESULTS_IN_EACH_BLOCK);
			u16TarIndexPos = p_Search->n16SearchItemIndex[u16Idx] - (u16Block * MAX_RESULTS_IN_EACH_BLOCK);

			snprintf(arr_cSerialNo , sizeof(arr_cSerialNo) , "%u" , 1 + (g_u8Slno + (g_u16CurrPageNum * MAX_RESULT_PER_WINDOW)));
			tTextSlNo[g_u8Slno].setText(arr_cSerialNo);

			tTextPatientId[g_u8Slno].setText(p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].arrPatientId);
			tTextTestName[g_u8Slno].setText(p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].arrTestName);

			snprintf(arr_cResult , 31 , "%.02f %s" ,p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].fResult ,
					p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].Unit);
			tTextResult[g_u8Slno].setText(arr_cResult);

			snprintf(arr_cDateStamp , 31 , "%02u/%02u/%02u" , (unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_day ,
					(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_Month ,
					(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_Year);
			tTextDate[g_u8Slno].setText(arr_cDateStamp);
			snprintf(arr_cTimeStamp , 31 , "%02u:%02u" , (unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Hour ,
					(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Minute /*,
					(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Second*/);
			tTextTime[g_u8Slno].setText(arr_cTimeStamp);
			g_u8Slno++;
			if(g_u8Slno >= MAX_RESULT_PER_WINDOW)
			{
				return;
			}
		}
	}
}
void UpdateSelectionradioButton(void)
{
	bool bSelectionEnable = false;
	uint16_t u8NumOfSelectedItems = 0;
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();

	for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
//	for(uint8_t u8Idx = 0 ; u8Idx < u16ResultsFOund ; ++u8Idx)
	{
		if(u8Idx < g_u8Slno)
		{
			bRadioButtons[u8Idx].setVisible(true);
			SelectBar[u8Idx].setVisible(true);
		}
		else
		{
			bRadioButtons[u8Idx].setVisible(false);
			SelectBar[u8Idx].setVisible(false);
		}
	}

	if(true == g_masterSelect)
	{
		bRbMasterSelect.Set_background_image_pic(RADIO_BUTTON_ON);
	}
	else
	{
		bRbMasterSelect.Set_background_image_pic(RADIO_BUTTON_OFF);
	}

	for(uint8_t u8Idx = 0 ; u8Idx < g_u8Slno && u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
	{
		if(true == g_masterSelect || true == p_Search->bItemSelectionStatus[u8Idx])
		{
			u8NumOfSelectedItems++;
			bRadioButtons[u8Idx].Set_background_image_pic(RADIO_BUTTON_ON);
			bSelectionEnable = true;
			p_Search->bItemSelectionStatus[u8Idx] = true;
		}
		else
		{
			bRadioButtons[u8Idx].Set_background_image_pic(RADIO_BUTTON_OFF);
		}
	}

	if(true == bSelectionEnable)
	{
		bButtonPrint.setVisible(true);
		bButtonDelete.setVisible(true);
		if(1 == u8NumOfSelectedItems)
		{
			bButtonView.setVisible(true);
		}
		else
		{
			bButtonView.setVisible(false);
		}
	}
	else
	{
		bButtonPrint.setVisible(false);
		bButtonDelete.setVisible(false);
		bButtonView.setVisible(false);
	}
}
void HandlerbButtonUp(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	uint16_t u16TotalNumPages = (uint16_t)(p_Search->u16ItemsFound / MAX_RESULT_PER_WINDOW);

	/*Added to avoid displaying empty pages*/
	if (p_Search->u16ItemsFound % MAX_RESULT_PER_WINDOW == 0) {
	    u16TotalNumPages--;
	}

	if(0 >= g_u16CurrPageNum)
	{
		g_u16CurrPageNum = u16TotalNumPages;
	}
	else
	{
		g_u16CurrPageNum--;
	}

	for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
	{
		p_Search->bItemSelectionStatus[u8Idx] = false;
	}
//	g_masterSelect = false;

	ShowUpdateHistoryList();
	UpdateSelectionradioButton();
	BeepBuzzer();
}
void HandlerbButtonDown(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	uint16_t u16TotalNumPages = (uint16_t)(p_Search->u16ItemsFound / MAX_RESULT_PER_WINDOW);

	/*Added to avoid displaying empty pages*/
	if (p_Search->u16ItemsFound % MAX_RESULT_PER_WINDOW == 0) {
	    u16TotalNumPages--;
	}

	if(u16TotalNumPages <= g_u16CurrPageNum)
	{
		g_u16CurrPageNum = 0;
	}
	else
	{
		g_u16CurrPageNum++;
	}

	for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
	{
		p_Search->bItemSelectionStatus[u8Idx] = false;
	}
//	g_masterSelect = false;

	ShowUpdateHistoryList();
	UpdateSelectionradioButton();
	BeepBuzzer();
}
void HandlerRbMasterSelect(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	if(true == g_masterSelect)
	{
		for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
		{
			p_Search->bItemSelectionStatus[u8Idx] = false;
		}
		g_masterSelect = false;
	}
	else
	{
//		for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
//		{
//			p_Search->bItemSelectionStatus[u8Idx] = true;
//		}
		g_masterSelect = true;
	}
	BeepBuzzer();
	UpdateSelectionradioButton();
}
void HandlerRbRadioButtons0(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	uint8_t u8RadioIndex = 0;
//	if(true == p_Search->bItemSelectionStatus[u8RadioIndex])
//	{
//		p_Search->bItemSelectionStatus[u8RadioIndex] = false;
//	}
//	else
//	{
//		p_Search->bItemSelectionStatus[u8RadioIndex] = true;
//	}
	for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
	{
		p_Search->bItemSelectionStatus[u8Idx] = false;
	}
	p_Search->bItemSelectionStatus[u8RadioIndex] = true;
	UpdateSelectionradioButton();
	BeepBuzzer();
}
void HandlerRbRadioButtons1(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	uint8_t u8RadioIndex = 1;
//	if(true == p_Search->bItemSelectionStatus[u8RadioIndex])
//	{
//		p_Search->bItemSelectionStatus[u8RadioIndex] = false;
//	}
//	else
//	{
//		p_Search->bItemSelectionStatus[u8RadioIndex] = true;
//	}
	for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
	{
		p_Search->bItemSelectionStatus[u8Idx] = false;
	}
	p_Search->bItemSelectionStatus[u8RadioIndex] = true;
	UpdateSelectionradioButton();
	BeepBuzzer();
}
void HandlerRbRadioButtons2(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	uint8_t u8RadioIndex = 2;
//	if(true == p_Search->bItemSelectionStatus[u8RadioIndex])
//	{
//		p_Search->bItemSelectionStatus[u8RadioIndex] = false;
//	}
//	else
//	{
//		p_Search->bItemSelectionStatus[u8RadioIndex] = true;
//	}
	for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
	{
		p_Search->bItemSelectionStatus[u8Idx] = false;
	}
	p_Search->bItemSelectionStatus[u8RadioIndex] = true;
	UpdateSelectionradioButton();
	BeepBuzzer();
}
void HandlerRbRadioButtons3(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	uint8_t u8RadioIndex = 3;
//	if(true == p_Search->bItemSelectionStatus[u8RadioIndex])
//	{
//		p_Search->bItemSelectionStatus[u8RadioIndex] = false;
//	}
//	else
//	{
//		p_Search->bItemSelectionStatus[u8RadioIndex] = true;
//	}
	for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
	{
		p_Search->bItemSelectionStatus[u8Idx] = false;
	}
	p_Search->bItemSelectionStatus[u8RadioIndex] = true;
	UpdateSelectionradioButton();
	BeepBuzzer();
}
void HandlerRbRadioButtons4(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	uint8_t u8RadioIndex = 4;
//	if(true == p_Search->bItemSelectionStatus[u8RadioIndex])
//	{
//		p_Search->bItemSelectionStatus[u8RadioIndex] = false;
//	}
//	else
//	{
//		p_Search->bItemSelectionStatus[u8RadioIndex] = true;
//	}
	for(uint8_t u8Idx = 0 ; u8Idx < MAX_RESULT_PER_WINDOW ; ++u8Idx)
	{
		p_Search->bItemSelectionStatus[u8Idx] = false;
	}
	p_Search->bItemSelectionStatus[u8RadioIndex] = true;
	UpdateSelectionradioButton();
	BeepBuzzer();
}
void HandlerbButtonPrint(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	ResHistoryDatabase *p_HistDatabase = GetInstance_ResultHistoryDatabase();
	uint16_t u16Block = 0;
	uint16_t u16TarIndexPos = 0;
	char arr_PrintBuffer[128] = {0};

	Print_Header();
	snprintf(arr_PrintBuffer , 63 , "%s" , "********************************");
	PrintStringOnPrinter(&arr_PrintBuffer[0]);
	PrinterFeedLine(1);

	/*Showing print name*/
	SendBoldOnOffCMD(true);
	snprintf(arr_PrintBuffer , 63 , "%s" , "Results");
	PrintStringOnPrinter(&arr_PrintBuffer[0]);
	PrinterFeedLine(1);
	SendBoldOnOffCMD(false);

//	uint16_t u16MaxData = MAX_RESULT_PER_WINDOW;
	uint16_t u16MaxData = u16ResultsFOund;
	int16_t n16SearchIndex = 0;
	uint16_t SelectResNo = 0;
	if(g_masterSelect != true)
	{
		SelectResNo = (g_u16CurrPageNum * MAX_RESULT_PER_WINDOW);
	}
	for(uint16_t u16Idx = 0 ; u16Idx < u16MaxData ; ++u16Idx)
	{
		if(true == g_masterSelect ||true == p_Search->bItemSelectionStatus[u16Idx])
		{
			if(g_masterSelect != true)
			{
				 n16SearchIndex = p_Search->n16SearchItemIndex[u16Idx + (g_u16CurrPageNum * MAX_RESULT_PER_WINDOW)];
			}
			else
			{
				 n16SearchIndex = p_Search->n16SearchItemIndex[u16Idx];
			}

			if((-1) != n16SearchIndex)
			{
				u16Block = uint32_t(n16SearchIndex / MAX_RESULTS_IN_EACH_BLOCK);
				u16TarIndexPos = n16SearchIndex - (u16Block * MAX_RESULTS_IN_EACH_BLOCK);

				snprintf(arr_PrintBuffer , 127 , "%d) Date       : %02u/%02u/%02u" , (1 + u16Idx + SelectResNo) ,
						(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_day ,
						(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_Month ,
						(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_Year);
				PrintStringOnPrinter(&arr_PrintBuffer[0]);

				snprintf(arr_PrintBuffer , 127 , "   Time       : %02u:%02u",
						(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Hour,
						(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Minute/*,
						(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Second*/);
				PrintStringOnPrinter(&arr_PrintBuffer[0]);

				snprintf(arr_PrintBuffer , 63 , "   Patient ID : %s" ,
						p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].arrPatientId);
				PrintStringOnPrinter(&arr_PrintBuffer[0]);

				snprintf(arr_PrintBuffer , 63 , "   Test name  : %s" ,
						p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].arrTestName);
				PrintStringOnPrinter(&arr_PrintBuffer[0]);

				snprintf(arr_PrintBuffer , 63 , "   Result     : %.0004f %s" ,p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].fResult ,
							p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].Unit);
				PrintStringOnPrinter(&arr_PrintBuffer[0]);

				PrinterFeedLine(1);
			}
		}
	}

	/*Feed*/
	strncpy(arr_PrintBuffer , "********************************" , 63);
	PrintStringOnPrinter(&arr_PrintBuffer[0]);
	Print_Footer();
	PrinterFeedLine(4);
}
void HandlerbButtonView(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	for(uint16_t u16Idx = 0 ; u16Idx < MAX_RESULT_PER_WINDOW ; ++u16Idx)
	{
		if(true == p_Search->bItemSelectionStatus[u16Idx])
		{
			SetResultViewId(p_Search->n16SearchItemIndex[u16Idx + (g_u16CurrPageNum * MAX_RESULT_PER_WINDOW)]);
			break;
		}
	}
	ChangeWindowPage(en_WinID_HistView , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbButtonDelete(void *ptr)
{
	BeepBuzzer();
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	/*Save upcomming window id before show popup page*/
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enKeyYes == ShowMainPopUp("Results","Do You Want To Delete The Result(s)?", false))
	{
		ShowPleaseWaitPopup("Results",PLEASE_WAIT);
		uint16_t u16MaxData = MAX_RESULT_PER_WINDOW;
//		uint16_t u16MaxData = u16ResultsFOund;
		for(uint16_t u16Idx = 0 ; u16Idx < u16MaxData ; ++u16Idx)
		{
			if(true == g_masterSelect)
			{
				DeleteTestResultHistory(p_Search->n16SearchItemIndex[u16Idx]);
				p_Search->n16SearchItemIndex[u16Idx] = (-1);
//				memset(&p_Search->bItemSelectionStatus[0] , false , MAX_RESULT_PER_WINDOW);
				p_Search->bItemSelectionStatus[u16Idx] = false;
			}
			else if(true == p_Search->bItemSelectionStatus[u16Idx])
			{
				DeleteTestResultHistory(p_Search->n16SearchItemIndex[u16Idx + (g_u16CurrPageNum * MAX_RESULT_PER_WINDOW)]);
				p_Search->n16SearchItemIndex[u16Idx + (g_u16CurrPageNum * MAX_RESULT_PER_WINDOW)] = (-1);
//				memset(&p_Search->bItemSelectionStatus[0] , false , MAX_RESULT_PER_WINDOW);
				p_Search->bItemSelectionStatus[u16Idx] = false;
			}
		}
		SaveTestResults();
		g_masterSelect = false;
	}
	stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
	ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
}

