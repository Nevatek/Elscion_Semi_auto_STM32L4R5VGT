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

int16_t g_n16ResId = (-1);


/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinID_HistView , 2, "b5");
static NexButton bPrintButton = NexButton(en_WinID_HistView , 5 , "b2");
static NexButton bDeleteButton = NexButton(en_WinID_HistView , 6 , "b4");

static NexPicture pPicTestType = NexPicture(en_WinID_HistView , 21 , "p0");

static NexText PatientId = NexText(en_WinID_HistView , 14 , "t7");
static NexText Date = NexText(en_WinID_HistView , 15 , "t8");
static NexText TestName = NexText(en_WinID_HistView , 16 , "t9");
static NexText TestType = NexText(en_WinID_HistView , 19 , "t12");
static NexText Absorbance = NexText(en_WinID_HistView , 18 , "t11");
static NexText UserName = NexText(en_WinID_HistView , 17 , "t10");
static NexText Result = NexText(en_WinID_HistView , 22 , "t14");
static NexText Status = NexText(en_WinID_HistView , 20 , "t13");



/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bPrintButton,
									 &bDeleteButton,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbPrint(void *ptr);
static void HandlerbDelete(void *ptr);

enWindowStatus ShowResHistViewScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bPrintButton.attachPush(HandlerbPrint, &bPrintButton);
	bDeleteButton.attachPush(HandlerbDelete, &bDeleteButton);

	ResHistoryDatabase *p_HistDatabase = GetInstance_ResultHistoryDatabase();
	uint16_t u16Block = 0;
	uint16_t u16TarIndexPos = 0;

	char arr_cTimeStamp[32] = {0};
	char arr_cResult[32] = {0};

	if(0 <= g_n16ResId)
	{
		u16Block = uint32_t(g_n16ResId / MAX_RESULTS_IN_EACH_BLOCK);
		u16TarIndexPos = g_n16ResId - (u16Block * MAX_RESULTS_IN_EACH_BLOCK);

		/* start filling screeen*/
		PatientId.setText(p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].arrPatientId);

		snprintf(arr_cTimeStamp , 31 , "%02u/%02u/%02u  %02u:%02u" , (unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_day ,
				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_Month ,
				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_Year,
				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Hour,
				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Minute/*,
				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Second*/);
		Date.setText(arr_cTimeStamp);

		TestName.setText(p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].arrTestName);
		UserName.setText(p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].arrUserName);

		snprintf(arr_cResult , 31 , "%.4f %s" ,p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].fResult ,
				p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].Unit);
		Result.setText(arr_cResult);

		snprintf(arr_cResult , 31 , "%.4f" ,p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].fAbs);
		Absorbance.setText(arr_cResult);

		TestType.setText(&g_arrTestmethodnameBuffer[p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].TestProcedure][0]);

		switch(p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].TestProcedure)
			{
				case en_Endpoint:
					pPicTestType.Set_background_image_pic(IMAGE_ID_ENDPOINT);
					break;
				case en_FixedTime:
					pPicTestType.Set_background_image_pic(IMAGE_ID_FIXEDPOINT);
					break;
				case en_Kinetics:
					pPicTestType.Set_background_image_pic(IMAGE_ID_KINETICS);
					break;
				case en_Turbidimetry:
					pPicTestType.Set_background_image_pic(IMAGE_ID_TURBIDIMETRY);
					break;
				case en_CoagulationTest:
					pPicTestType.Set_background_image_pic(IMAGE_ID_COAGULATION);
					break;
				case en_Absorbance:
					pPicTestType.Set_background_image_pic(IMAGE_ID_ENDPOINT);
					break;
				default:
					pPicTestType.Set_background_image_pic(DELAULT_NO_TEST_IMAGE_ID);
					break;
			}
	}
	return WinStatus;
}

void HandlerResHistViewScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void SetResultViewId(int16_t n16ResId)
{
	g_n16ResId = n16ResId;
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_HistoryMMenu , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbPrint(void *ptr)
{
	char arr_PrintBuffer[128] = {0};
	BeepBuzzer();
	bPrintButton.setVisible(false);

	if(0 <= g_n16ResId)
	{
		ResHistoryDatabase *p_HistDatabase = GetInstance_ResultHistoryDatabase();

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

		uint16_t u16Block = uint32_t(g_n16ResId / MAX_RESULTS_IN_EACH_BLOCK);
		uint16_t u16TarIndexPos = g_n16ResId - (u16Block * MAX_RESULTS_IN_EACH_BLOCK);

		snprintf(arr_PrintBuffer , 63 , "Patient Id  : %s" ,
				p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].arrPatientId);
		PrintStringOnPrinter(&arr_PrintBuffer[0]);

		snprintf(arr_PrintBuffer , 127 ,"Date & Time : %02u/%02u/%02u  %02u:%02u",
				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_day ,
				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_Month ,
				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Date_Year,

				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Hour,
				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Minute/*,
				(unsigned int)p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].u16Second*/);
		PrintStringOnPrinter(&arr_PrintBuffer[0]);

		snprintf(arr_PrintBuffer , 63 , "Test name   : %s" ,
				p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].arrTestName);
		PrintStringOnPrinter(&arr_PrintBuffer[0]);
		snprintf(arr_PrintBuffer , 63 , "Absorbance  : %.0004f" ,p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].fAbs);
		PrintStringOnPrinter(&arr_PrintBuffer[0]);

		SendBoldOnOffCMD(true);
		snprintf(arr_PrintBuffer , 63 , "Result      : %.0004f %s" ,p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].fResult ,
					p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].Unit);
		PrintStringOnPrinter(&arr_PrintBuffer[0]);
		SendBoldOnOffCMD(false);
		/*Print user name*/
		snprintf(arr_PrintBuffer , 63 , "User name   : %s" ,
				p_HistDatabase->m_DataBase[u16Block][u16TarIndexPos].arrUserName);
		PrintStringOnPrinter(&arr_PrintBuffer[0]);
		/*Feed*/
		strncpy(arr_PrintBuffer , "********************************" , 63);
		PrintStringOnPrinter(&arr_PrintBuffer[0]);
		Print_Footer();
		PrinterFeedLine(4);
	}
	bPrintButton.setVisible(true);
}

void HandlerbDelete(void *ptr)
{
	BeepBuzzer();
	if(0 <= g_n16ResId)
	{
		uint16_t u16Block = uint32_t(g_n16ResId / MAX_RESULTS_IN_EACH_BLOCK);
		uint16_t u16TarIndexPos = g_n16ResId - (u16Block * MAX_RESULTS_IN_EACH_BLOCK);

		/*Save upcomming window id before show popup page*/
		if(enKeyYes == ShowMainPopUp("Results","Do you want to delete the result?", false))
		{
			ShowPleaseWaitPopup("Results",PLEASE_WAIT);
			DeleteTestResultHistory(u16TarIndexPos);
			SaveTestResultHistoryBlockWise(u16Block);
		}
		stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
		ChangeWindowPage(en_WinId_HistoryMMenu , (enWindowID)NULL);
	}

}
