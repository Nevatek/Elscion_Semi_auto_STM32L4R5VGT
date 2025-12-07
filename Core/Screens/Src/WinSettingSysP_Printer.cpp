/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "Printer.h"
#include "system.hpp"

//static uint32_t g_u16BaudRate = 0;

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_SettingsSysP_Printer , 1, "b5");
static NexButton tLamp = NexButton(en_WinId_SettingsSysP_Printer, 9 , "b0");
static NexButton tpump = NexButton(en_WinId_SettingsSysP_Printer, 8 , "b1");
static NexButton tTempA_Flowcell = NexButton(en_WinId_SettingsSysP_Printer, 7 , "b2");
static NexButton tBarcode = NexButton(en_WinId_SettingsSysP_Printer, 6 , "b3");
static NexText tBaudRate = NexText(en_WinId_SettingsSysP_Printer, 13 , "t11");

static NexButton tSave = NexButton(en_WinId_SettingsSysP_Printer, 14 , "b7");
static NexButton tTestprint = NexButton(en_WinId_SettingsSysP_Printer, 11 , "b6");
/*This list condains all the local buttons to be monitored in realtime*/

static uint32_t g_u16BaudRate = 0;

static NexTouch *nex_Listen_List[] ={&bBack,
									 &tLamp,
									 &tpump,
									 &tBarcode,
									 &tTempA_Flowcell,
									 &tBaudRate,
									 &tSave,
									 &tTestprint,
									 NULL};

static void HandlerbBack(void *ptr);

static void HandlerbLamp(void *ptr);
static void HandlerbPump(void *ptr);
static void Handlerbbarcode(void *ptr);
static void HandlerbTemp_A(void *ptr);
static void HandlerbBaudRate(void *ptr);
static void HandlerSave(void *ptr);
static void HandlerTestprint(void *ptr);

enWindowStatus ShowSettingsSysP_Printer (NexPage *ptr_obJCurrPage)
{
	char arrPopUpBuff[64] = {0};
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	tLamp.attachPush(HandlerbLamp, &tLamp);
	tpump.attachPush(HandlerbPump, &tpump);
	tBarcode.attachPush(Handlerbbarcode, &tBarcode);
	tTempA_Flowcell.attachPush(HandlerbTemp_A, &tTempA_Flowcell);
	tBaudRate.attachPush(HandlerbBaudRate, &tBaudRate);
	tSave.attachPush(HandlerSave, &tSave);
	tTestprint.attachPush(HandlerTestprint, &tTestprint);

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_NumericKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
	{
		g_u16BaudRate = objstcSettings.PrinterBaudRate;
	}

	snprintf(arrPopUpBuff , 63 , "%u" , (unsigned int)g_u16BaudRate);
	tBaudRate.setText(arrPopUpBuff);

	return WinStatus;
}

void HandlerSettingsSysP_Printer (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerTestprint(void *ptr)
{
	char arr_CBuffer[64] = {0};
	RTC_DateTypeDef m_Date;
	RTC_TimeTypeDef m_Time;
	GetCurrentDate(&m_Date);
	GetCurrentTime(&m_Time);

	PrintStringOnPrinter((char*)"ELSCION BIOMEDICALS");
	PrintStringOnPrinter((char*)"ARIYA 100 CHEMISTRY ANALYZER");
	snprintf(arr_CBuffer , 63 , "DATE : %02d/%02d/%02d  TIME : %02d:%02d" ,
			m_Date.Date,
			m_Date.Month,
			m_Date.Year,
			m_Time.Hours,
			m_Time.Minutes);
	PrintStringOnPrinter(&arr_CBuffer[0]);
	PrintStringOnPrinter((char*)"********************************");
	PrinterFeedLine(1);
	PrintStringOnPrinter((char*)"PRINTER TEST : OK");
	PrinterFeedLine(4);
	BeepBuzzer();
}

void HandlerSave(void *ptr)
{
	BeepBuzzer();
	char arrPopUpBuff[64] = {0};
	if(0 < g_u16BaudRate && 921600 >= g_u16BaudRate)
	{
		objstcSettings.PrinterBaudRate = g_u16BaudRate;
		/*save settings*/
		WriteSettingsBuffer();
		InitPrinter(GetInstance_PrinterUart());
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enkeyOk == ShowMainPopUp("Printer settings","Saved" , true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
	}
	else
	{
		/*Show pop up*/
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		snprintf(arrPopUpBuff , 63 , "Invalid Baud rate");
		if(enkeyOk == ShowMainPopUp("Printer settings",arrPopUpBuff , true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
	}
}
void HandlerbBaudRate(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_u16BaudRate , sizeof(g_u16BaudRate) ,false ,
			"Baud Rate" , std::to_string(g_u16BaudRate).c_str() , 6);
	BeepBuzzer();
}
void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbPump(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Pump , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbTemp_A(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Temp_A , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbLamp(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Lamp , (enWindowID)NULL);
	BeepBuzzer();
}
void Handlerbbarcode(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Barcode , (enWindowID)NULL);
	BeepBuzzer();
}
