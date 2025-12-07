/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_SettingsSysP_Barcode , 1, "b5");
static NexButton tLamp = NexButton(en_WinId_SettingsSysP_Barcode, 9 , "b0");
static NexButton tpump = NexButton(en_WinId_SettingsSysP_Barcode, 8 , "b1");
static NexButton tTempA_Flowcell = NexButton(en_WinId_SettingsSysP_Barcode, 7 , "b2");
static NexButton tPrinter = NexButton(en_WinId_SettingsSysP_Barcode, 5 , "b4");
static NexText tReceivedText = NexText(en_WinId_SettingsSysP_Barcode, 11 , "t1");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &tLamp,
									 &tpump,
									 &tPrinter,
									 &tTempA_Flowcell,
									 NULL};
//static uint32_t g_u16BaudRate = 0;

static void HandlerbBack(void *ptr);
static void HandlerbLamp(void *ptr);
static void HandlerbPump(void *ptr);
static void HandlerbPrinter(void *ptr);
static void HandlerbTemp_A(void *ptr);
//static void HandlerbBaudRate(void *ptr);
//static void HandlerSave(void *ptr);

enWindowStatus ShowSettingsSysP_Barcode (NexPage *ptr_obJCurrPage)
{
//	char arrPopUpBuff[64] = {0};
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	tLamp.attachPush(HandlerbLamp, &tLamp);
	tpump.attachPush(HandlerbPump, &tpump);
	tPrinter.attachPush(HandlerbPrinter, &tPrinter);
	tTempA_Flowcell.attachPush(HandlerbTemp_A, &tTempA_Flowcell);
//	tBaudRate.attachPush(HandlerbBaudRate, &tBaudRate);
//	tSave.attachPush(HandlerSave, &tSave);

//	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
//	if(en_WinID_NumericKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
//	{
//		g_u16BaudRate = objstcSettings.BarcodeBaudRate;
//	}
//
//	snprintf(arrPopUpBuff , 63 , "%u" , (unsigned int)g_u16BaudRate);
//	tBaudRate.setText(arrPopUpBuff);
	return WinStatus;
}

void HandlerSettingsSysP_Barcode (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}
//void HandlerSave(void *ptr)
//{
//	char arrPopUpBuff[64] = {0};
//	if(0 < g_u16BaudRate && 115200 >= g_u16BaudRate)
//	{
//		objstcSettings.BarcodeBaudRate = g_u16BaudRate;
//		/*save settings*/
//		WriteSettingsBuffer();
//		/*Save upcomming window id before show popup page*/
//		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//		if(enkeyOk == ShowMainPopUp("Saved" , true))
//		{
//			BeepBuzzer();
//			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
//		}
//	}
//	else
//	{
//		/*Show pop up*/
//		/*Save upcomming window id before show popup page*/
//		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
//		snprintf(arrPopUpBuff , 63 , "Invalid Baudrate (1 - 100)");
//		if(enkeyOk == ShowMainPopUp(arrPopUpBuff , true))
//		{
//			BeepBuzzer();
//			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
//			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
//		}
//	}
//}
//void HandlerbBaudRate(void *ptr)
//{
//	openKeyBoard(en_NumericKeyboard , &g_u16BaudRate , sizeof(g_u16BaudRate) ,false ,
//			"baud Rate" , std::to_string(g_u16BaudRate).c_str() , 6);
//	BeepBuzzer();
//}
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
void HandlerbPrinter(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Printer , (enWindowID)NULL);
	BeepBuzzer();
}
