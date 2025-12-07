/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestResultsHistory.hpp"

static char g_arrBuff[3][16];
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bPResultsBack = NexButton(en_WinID_HistSearch , 1, "b5");
static NexButton bDate = NexButton(en_WinID_HistSearch , 5, "b0");
static NexButton bMethod = NexButton(en_WinID_HistSearch , 6, "b1");
static NexButton bPatientId = NexButton(en_WinID_HistSearch , 7, "b2");
static NexButton bUser = NexButton(en_WinID_HistSearch , 8, "b3");
static NexButton bTest = NexButton(en_WinID_HistSearch , 9, "b4");
static NexText bTextStartDate = NexText(en_WinID_HistSearch , 16, "t2");
static NexText bTextEnddate = NexText(en_WinID_HistSearch , 17, "t3");
static NexText bTextCommon = NexText(en_WinID_HistSearch , 19, "t5");
static NexButton bReset = NexButton(en_WinID_HistSearch , 21 , "b11");
static NexButton bSearch = NexButton(en_WinID_HistSearch , 20 , "b10");

static NexText bDateStart = NexText(en_WinID_HistSearch , 14, "t0");
static NexText bDateEnd = NexText(en_WinID_HistSearch , 15, "t1");
static NexText tText0 = NexText(en_WinID_HistSearch , 18, "t4");
static NexButton bButton0n[5] = {NexButton(en_WinID_HistSearch , 10, "b6"),
								 NexButton(en_WinID_HistSearch , 11, "b7"),
								 NexButton(en_WinID_HistSearch , 12, "b8"),
								 NexButton(en_WinID_HistSearch , 13, "b9"),
								 NexButton(en_WinID_HistSearch , 22, "b12")
};
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bPResultsBack,
									 &bDate,
									 &bMethod,
									 &bPatientId,
									 &bUser,
									 &bTest,
									 &bDateStart,
									 &bDateEnd,
									 &tText0,
									 &bButton0n[0],
									 &bButton0n[1],
									 &bButton0n[2],
									 &bButton0n[3],
									 &bButton0n[4],
									 &bReset,
									 &bSearch,
									 NULL};

static void HandlerbPResultsBack(void *ptr);
static void HandlerbPResultsDate(void *ptr);
static void HandlerbPResultsMethod(void *ptr);
static void HandlerbPResultsPatientId(void *ptr);
static void HandlerbPResultsUser(void *ptr);
static void HandlerbPResultsTest(void *ptr);
static void HandlerbPResultsDateStart(void *ptr);
static void HandlerbPResultsDateEnd(void *ptr);
static void HandlerbPResultsText0(void *ptr);
static void HandlerbPResultsButton0(void *ptr);
static void HandlerbPResultsButton1(void *ptr);
static void HandlerbPResultsButton2(void *ptr);
static void HandlerbPResultsButton3(void *ptr);
static void HandlerbPResultsButton4(void *ptr);
static void HandlerbPReset(void *ptr);
static void HandlerbPSearch(void *ptr);
static void UpdateSearchScreen(void);

enWindowStatus ShowSearchResHist (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bPResultsBack.attachPush(HandlerbPResultsBack, &bPResultsBack);
	bDate.attachPush(HandlerbPResultsDate, &bDate);
	bMethod.attachPush(HandlerbPResultsMethod, &bMethod);
	bPatientId.attachPush(HandlerbPResultsPatientId, &bPatientId);
	bUser.attachPush(HandlerbPResultsUser, &bUser);
	bTest.attachPush(HandlerbPResultsTest, &bTest);
	bDateStart.attachPush(HandlerbPResultsDateStart, &bDateStart);
	bDateEnd.attachPush(HandlerbPResultsDateEnd, &bDateEnd);
	tText0.attachPush(HandlerbPResultsText0, &tText0);
	bButton0n[0].attachPush(HandlerbPResultsButton0, &bButton0n[0]);
	bButton0n[1].attachPush(HandlerbPResultsButton1, &bButton0n[1]);
	bButton0n[2].attachPush(HandlerbPResultsButton2, &bButton0n[2]);
	bButton0n[3].attachPush(HandlerbPResultsButton3, &bButton0n[3]);
	bButton0n[4].attachPush(HandlerbPResultsButton4, &bButton0n[4]);
	bReset.attachPush(HandlerbPReset, &bReset);
	bSearch.attachPush(HandlerbPSearch, &bSearch);


	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();

	if(en_WinID_NumericKeypad != PrevWindow &&
			en_WinId_MainPopup != PrevWindow &&
			en_WinId_HistoryMMenu != PrevWindow &&
			en_WinID_AlphaKeypad != PrevWindow)
	{
		strncpy(&g_arrBuff[0][0] , "" , 15);
		strncpy(&g_arrBuff[1][0] , "" , 15);
		strncpy(&g_arrBuff[2][0] , "" , 15);
		SetCurrSearchType(enSearchDate);
	}
	else if(en_WinId_HistoryMMenu == PrevWindow)
	{
		strncpy(&g_arrBuff[0][0] , "" , 15);
		snprintf(&g_arrBuff[0][0] , sizeof(g_arrBuff) , "%02d/%02d/%02d" ,
				p_Search->m_DateStart.Date ,
				p_Search->m_DateStart.Month,
				p_Search->m_DateStart.Year);
		snprintf(&g_arrBuff[1][0] , sizeof(g_arrBuff) , "%02d/%02d/%02d" ,
				p_Search->m_DateEnd.Date ,
				p_Search->m_DateEnd.Month,
				p_Search->m_DateEnd.Year);
	}
	UpdateSearchScreen();
	return WinStatus;
}

void HandlerSearchResHist (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void UpdateSearchScreen(void)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	en_RESULT_HIST_SEARCH m_Search = GetCurrSearchType();
	switch(m_Search)
	{
		case enSearchDate:
		{
			bDate.Set_background_image_pic(399);
			bMethod.Set_background_image_pic(400);
			bPatientId.Set_background_image_pic(401);
			bUser.Set_background_image_pic(402);
			bTest.Set_background_image_pic(403);
			bDateEnd.setVisible(true);
			tText0.setVisible(false);
			bButton0n[0].setVisible(true);
			bButton0n[1].setVisible(false);
			bButton0n[2].setVisible(false);
			bButton0n[3].setVisible(false);
			bButton0n[4].setVisible(false);
			bDateStart.setVisible(true);
			bTextStartDate.setVisible(true);
			bTextEnddate.setVisible(true);
			bTextStartDate.setVisible(true);
			bTextEnddate.setVisible(true);
			bTextCommon.setVisible(false);
			bReset.setVisible(true);
			bSearch.setVisible(true);
			bButton0n[0].setText("Today");
			bDateStart.setText(&g_arrBuff[0][0]);
			bDateEnd.setText(&g_arrBuff[1][0]);
		}break;
		case enSearchMethod:
		{
			bDate.Set_background_image_pic(394);
			bMethod.Set_background_image_pic(395);
			bPatientId.Set_background_image_pic(401);
			bUser.Set_background_image_pic(402);
			bTest.Set_background_image_pic(403);
			bDateEnd.setVisible(false);
			tText0.setVisible(false);
			bButton0n[0].setVisible(true);
			bButton0n[1].setVisible(true);
			bButton0n[2].setVisible(true);
			bButton0n[3].setVisible(true);
			bButton0n[4].setVisible(true);
			bDateStart.setVisible(false);
			bTextStartDate.setVisible(false);
			bTextEnddate.setVisible(false);
			bTextStartDate.setVisible(false);
			bTextEnddate.setVisible(false);
			bTextCommon.setVisible(false);
			bReset.setVisible(false);
			bSearch.setVisible(false);

			bButton0n[0].setText("End Point");
			bButton0n[1].setText("Kinetics");
			bButton0n[2].setText("Fixed Time");
			bButton0n[3].setText("Turbi - Fix Time");
			bButton0n[4].setText("Absorbance");
		}break;
		case enSearchPatientId:
		{
			bDate.Set_background_image_pic(394);
			bMethod.Set_background_image_pic(400);
			bPatientId.Set_background_image_pic(396);
			bUser.Set_background_image_pic(402);
			bTest.Set_background_image_pic(403);
			bDateStart.setVisible(false);
			bDateEnd.setVisible(false);
			tText0.setVisible(true);
			bButton0n[0].setVisible(false);
			bButton0n[1].setVisible(false);
			bButton0n[2].setVisible(false);
			bButton0n[3].setVisible(false);
			bButton0n[4].setVisible(false);
			bTextStartDate.setVisible(false);
			bTextEnddate.setVisible(false);
			bTextStartDate.setVisible(false);
			bTextEnddate.setVisible(false);
			bTextCommon.setVisible(true);
			bTextCommon.setText("Enter Patient Id");
			tText0.setText(p_Search->arrBuff);
			bReset.setVisible(true);
			bSearch.setVisible(true);
		}break;
		case enSearchUser:
		{
			bDate.Set_background_image_pic(394);
			bMethod.Set_background_image_pic(400);
			bPatientId.Set_background_image_pic(401);
			bUser.Set_background_image_pic(397);
			bTest.Set_background_image_pic(403);
			bDateStart.setVisible(false);
			bDateEnd.setVisible(false);
			tText0.setVisible(true);
			bButton0n[0].setVisible(false);
			bButton0n[1].setVisible(false);
			bButton0n[2].setVisible(false);
			bButton0n[3].setVisible(false);
			bButton0n[4].setVisible(false);
			bTextStartDate.setVisible(false);
			bTextEnddate.setVisible(false);
			bTextStartDate.setVisible(false);
			bTextEnddate.setVisible(false);
			bTextCommon.setVisible(true);
			bReset.setVisible(true);
			bSearch.setVisible(true);
			bTextCommon.setText("Enter User Name");
			tText0.setText(p_Search->arrBuff);
		}break;
		case enSearchTestname:
		{
			bDate.Set_background_image_pic(394);
			bMethod.Set_background_image_pic(400);
			bPatientId.Set_background_image_pic(401);
			bUser.Set_background_image_pic(402);
			bTest.Set_background_image_pic(398);
			bDateStart.setVisible(false);
			bDateEnd.setVisible(false);
			tText0.setVisible(true);
			bButton0n[0].setVisible(false);
			bButton0n[1].setVisible(false);
			bButton0n[2].setVisible(false);
			bButton0n[3].setVisible(false);
			bButton0n[4].setVisible(false);
			bTextStartDate.setVisible(false);
			bTextEnddate.setVisible(false);
			bTextStartDate.setVisible(false);
			bTextEnddate.setVisible(false);
			bTextCommon.setVisible(true);
			bReset.setVisible(true);
			bSearch.setVisible(true);
			bTextCommon.setText("Enter Test Name");
			tText0.setText(p_Search->arrBuff);
		}break;
		default:
		{

		}
		break;
	}
}
void HandlerbPResultsBack(void *ptr)
{
	ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbPSearch(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	en_RESULT_HIST_SEARCH m_SearchType = GetCurrSearchType();
	BeepBuzzer();
	for(uint8_t u8Idx = 0 ; u8Idx < 15 ; ++u8Idx)
	{
		if('-' == g_arrBuff[0][u8Idx] || '.' == g_arrBuff[0][u8Idx] || ' ' == g_arrBuff[0][u8Idx])
		{
			g_arrBuff[0][u8Idx] = 0;
		}
		if('-' == g_arrBuff[1][u8Idx] || '.' == g_arrBuff[1][u8Idx] || ' ' == g_arrBuff[1][u8Idx])
		{
			g_arrBuff[1][u8Idx] = 0;
		}
	}
	g_arrBuff[0][2] = '\0';
	g_arrBuff[0][5] = '\0';
	g_arrBuff[1][2] = '\0';
	g_arrBuff[1][5] = '\0';

	g_arrBuff[0][10] = '\0';/*Termination character*/
	g_arrBuff[0][10] = '\0';/*Termination character*/

	std::string strStartDate = std::string(&g_arrBuff[0][0]);
	std::string strStartMonth = std::string(&g_arrBuff[0][3]);
	std::string strStartYear = std::string(&g_arrBuff[0][6]);
	std::string strEndDate = std::string(&g_arrBuff[1][0]);
	std::string strEndMonth = std::string(&g_arrBuff[1][3]);
	std::string strEndYear = std::string(&g_arrBuff[1][6]);

	p_Search->m_DateStart.Date =  atoi(strStartDate.c_str());
	p_Search->m_DateStart.Month =  atoi(strStartMonth.c_str());
	p_Search->m_DateStart.Year =  atoi(strStartYear.c_str());

	p_Search->m_DateEnd.Date =  atoi(strEndDate.c_str());
	p_Search->m_DateEnd.Month =  atoi(strEndMonth.c_str());
	p_Search->m_DateEnd.Year =  atoi(strEndYear.c_str());

	if(m_SearchType == enSearchPatientId && (p_Search->arrBuff[0] == 0))
	{
		if(enkeyOk == ShowMainPopUp("Results","Patient ID should not be empty", true))
		{
			ChangeWindowPage(en_WinID_HistSearch , (enWindowID)NULL);
			stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
			return;
		}
	}
	else if(m_SearchType == enSearchTestname && (p_Search->arrBuff[0] == 0))
	{
		if(enkeyOk == ShowMainPopUp("Results","Test Name should not be empty", true))
		{
			ChangeWindowPage(en_WinID_HistSearch , (enWindowID)NULL);
			stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
			return;
		}
	}
	else if(m_SearchType == enSearchUser && (p_Search->arrBuff[0] == 0))
	{
		if(enkeyOk == ShowMainPopUp("Results","Username should not be empty", true))
		{
			ChangeWindowPage(en_WinID_HistSearch , (enWindowID)NULL);
			stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
			return;
		}
	}
	else if(m_SearchType == enSearchDate)
	{
		if((p_Search->m_DateStart.Date != 0 && p_Search->m_DateStart.Month != 0 && p_Search->m_DateStart.Year != 0 ) ||
				(p_Search->m_DateEnd.Date != 0 && p_Search->m_DateEnd.Month != 0 && p_Search->m_DateEnd.Year != 0))
		{

		}
		else
		{
			if(enkeyOk == ShowMainPopUp("Results","Enter valid date", true))
			{
				ChangeWindowPage(en_WinID_HistSearch , (enWindowID)NULL);
				stcScreenNavigation.PrevWindowId = en_WinId_MainPopup;
				return;
			}
		}
	}
		ShowPleaseWaitPopup("Results",PLEASE_WAIT);
		ReadAllTestResultHistory();
//		ReadTestParameterBuffer();
		ChangeWindowPage(en_WinId_HistoryMMenu , (enWindowID)NULL);
}
void HandlerbPReset(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	strncpy(&g_arrBuff[0][0] , "" , MAX_PATIENT_ID_LENGTH);
	strncpy(&g_arrBuff[1][0] , "" , MAX_PATIENT_ID_LENGTH);
	strncpy(p_Search->arrBuff , "" , MAX_PATIENT_ID_LENGTH);
	UpdateSearchScreen();
	BeepBuzzer();
}
void HandlerbPResultsDate(void *ptr)
{
	SetCurrSearchType(enSearchDate);
	strncpy(&g_arrBuff[0][0] , "" , MAX_PATIENT_ID_LENGTH);
	strncpy(&g_arrBuff[1][0] , "" , MAX_PATIENT_ID_LENGTH);
	UpdateSearchScreen();
	BeepBuzzer();
}
void HandlerbPResultsMethod(void *ptr)
{
	SetCurrSearchType(enSearchMethod);
	UpdateSearchScreen();
	BeepBuzzer();
}
void HandlerbPResultsPatientId(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	SetCurrSearchType(enSearchPatientId);
	p_Search->arrBuff[0] = '\0';
	UpdateSearchScreen();
	BeepBuzzer();
}
void HandlerbPResultsUser(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	SetCurrSearchType(enSearchUser);
	p_Search->arrBuff[0] = '\0';
	UpdateSearchScreen();
	BeepBuzzer();
}
void HandlerbPResultsTest(void *ptr)
{
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	SetCurrSearchType(enSearchTestname);
	p_Search->arrBuff[0] = '\0';
	UpdateSearchScreen();
	BeepBuzzer();
}
void HandlerbPResultsDateStart(void *ptr)
{
	DateEntry = true;
	openKeyBoard(en_NumericKeyboard , g_arrBuff[0] , sizeof(g_arrBuff[0]) , false ,
			"Start Date (DD/MM/YY)" , g_arrBuff[0] , strlen("DD/MM/YY"));
	BeepBuzzer();
}
void HandlerbPResultsDateEnd(void *ptr)
{
	DateEntry = true;
	openKeyBoard(en_NumericKeyboard , g_arrBuff[1] , sizeof(g_arrBuff[1]) , false ,
			"End Date (DD/MM/YY)" , g_arrBuff[1] , strlen("DD/MM/YY"));
	BeepBuzzer();
}
void HandlerbPResultsText0(void *ptr)
{
	en_RESULT_HIST_SEARCH m_Search = GetCurrSearchType();
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	std::string strKeyboardname = " ";
	if(enSearchPatientId == m_Search)
	{
		strKeyboardname = std::string("Patient ID");
	}
	else if(enSearchTestname == m_Search)
	{
		strKeyboardname = std::string("Test Name");
	}
	else if(enSearchUser == m_Search)
	{
		strKeyboardname = std::string("User Name");
	}
	openKeyBoard(en_AlphaKeyboard , p_Search->arrBuff , sizeof(p_Search->arrBuff) , false ,
			strKeyboardname.c_str() , p_Search->arrBuff , MAX_PASSWORD_LENGTH);
	BeepBuzzer();
}
void HandlerbPResultsButton0(void *ptr)
{
	en_RESULT_HIST_SEARCH m_SearchType = GetCurrSearchType();
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	RTC_DateTypeDef p_TodaysDate;

	if(enSearchMethod== m_SearchType)
	{
		p_Search->m_TestType = en_Endpoint;
	}
	else if(enSearchDate == m_SearchType)
	{
		GetCurrentDate(&p_TodaysDate);
		snprintf(&g_arrBuff[0][0] , 15 , "%02d/%02d/%02d" ,p_TodaysDate.Date , p_TodaysDate.Month , p_TodaysDate.Year);
		HandlerbPSearch(NULL);
		return;
	}
	BeepBuzzer();
	ShowPleaseWaitPopup("Results",PLEASE_WAIT);
	ReadAllTestResultHistory();
	ChangeWindowPage(en_WinId_HistoryMMenu , (enWindowID)NULL);
}
void HandlerbPResultsButton1(void *ptr)
{
	en_RESULT_HIST_SEARCH m_SearchType = GetCurrSearchType();
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	if(enSearchMethod== m_SearchType)
	{
		p_Search->m_TestType = en_Kinetics;
	}
	BeepBuzzer();
	ShowPleaseWaitPopup("Results",PLEASE_WAIT);
	ReadAllTestResultHistory();
	ChangeWindowPage(en_WinId_HistoryMMenu , (enWindowID)NULL);
}
void HandlerbPResultsButton2(void *ptr)
{
	en_RESULT_HIST_SEARCH m_SearchType = GetCurrSearchType();
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	if(enSearchMethod== m_SearchType)
	{
		p_Search->m_TestType = en_FixedTime;
	}
	BeepBuzzer();
	ShowPleaseWaitPopup("Results",PLEASE_WAIT);
	ReadAllTestResultHistory();
	ChangeWindowPage(en_WinId_HistoryMMenu , (enWindowID)NULL);
}
void HandlerbPResultsButton3(void *ptr)
{
	en_RESULT_HIST_SEARCH m_SearchType = GetCurrSearchType();
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	if(enSearchMethod== m_SearchType)
	{
		p_Search->m_TestType = en_Turbidimetry;
	}
	BeepBuzzer();
	ShowPleaseWaitPopup("Results",PLEASE_WAIT);
	ReadAllTestResultHistory();
	ChangeWindowPage(en_WinId_HistoryMMenu , (enWindowID)NULL);
}

void HandlerbPResultsButton4(void *ptr)
{
	en_RESULT_HIST_SEARCH m_SearchType = GetCurrSearchType();
	SEARCH_RESHISTORY *p_Search = GetInstance_SearchResultHistory();
	if(enSearchMethod== m_SearchType)
	{
		p_Search->m_TestType = en_Absorbance;
	}
	BeepBuzzer();
	ShowPleaseWaitPopup("Results",PLEASE_WAIT);
	ReadAllTestResultHistory();
	ChangeWindowPage(en_WinId_HistoryMMenu , (enWindowID)NULL);
}


