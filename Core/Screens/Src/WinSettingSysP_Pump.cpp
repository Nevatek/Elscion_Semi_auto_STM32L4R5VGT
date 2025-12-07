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

static NexButton bBack = NexButton(en_WinId_SettingsSysP_Pump , 1, "b5");
static NexButton tLamp = NexButton(en_WinId_SettingsSysP_Pump, 9 , "b0");
static NexButton tTempA_Flowcell = NexButton(en_WinId_SettingsSysP_Pump, 7 , "b2");
static NexButton tBarcode = NexButton(en_WinId_SettingsSysP_Pump, 6 , "b3");
static NexButton tPrinter = NexButton(en_WinId_SettingsSysP_Pump, 5 , "b4");
static NexButton tSave = NexButton(en_WinId_SettingsSysP_Pump, 12 , "b6");

static NexText tPwnDuty = NexText(en_WinId_SettingsSysP_Pump, 10 , "t11");
static NexText tPumpConst = NexText(en_WinId_SettingsSysP_Pump, 14 , "t1");
static uint8_t g_LocalPwmDuty = 0;
static float g_LocalPumpconst = 0;
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &tLamp,
									 &tBarcode,
									 &tPrinter,
									 &tTempA_Flowcell,
									 &tPwnDuty,
									 &tPumpConst,
									 &tSave,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbLamp(void *ptr);
static void Handlerbbarcode(void *ptr);
static void HandlerbPrinter(void *ptr);
static void HandlerbTemp_A(void *ptr);
static void HandlerbPwmDuty(void *ptr);
static void HandlerbPumpConst(void *ptr);
static void HandlerSave(void *ptr);

enWindowStatus ShowSettingsSysP_Pump (NexPage *ptr_obJCurrPage)
{
	char arrPopUpBuff[64] = {0};
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	tLamp.attachPush(HandlerbLamp, &tLamp);
	tBarcode.attachPush(Handlerbbarcode, &tBarcode);
	tPrinter.attachPush(HandlerbPrinter, &tPrinter);
	tTempA_Flowcell.attachPush(HandlerbTemp_A, &tTempA_Flowcell);
	tPwnDuty.attachPush(HandlerbPwmDuty, &tPwnDuty);
	tPumpConst.attachPush(HandlerbPumpConst, &tPumpConst);
	tSave.attachPush(HandlerSave, &tSave);
	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_NumericKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
	{
		g_LocalPwmDuty = objstcSettings.u_n8DcMotorPwmDuty;
		g_LocalPumpconst = objstcSettings.fPumpConstant;
	}

	snprintf(arrPopUpBuff , 63 , "%u" , (unsigned int)g_LocalPwmDuty);
	tPwnDuty.setText(arrPopUpBuff);

	snprintf(arrPopUpBuff , 63 , "%0.003f" ,  (float)g_LocalPumpconst);
	tPumpConst.setText(arrPopUpBuff);
	return WinStatus;
}

void HandlerSettingsSysP_Pump (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerSave(void *ptr)
{
	BeepBuzzer();
	char arrPopUpBuff[64] = {0};
	if(40 > g_LocalPwmDuty || 100 < g_LocalPwmDuty)
	{
		/*Show pop up*/
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		snprintf(arrPopUpBuff , 63 , "Invalid Duty Cycle");
		if(enkeyOk == ShowMainPopUp("Pump Settings",arrPopUpBuff , true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
	}
	else if ( 1 > g_LocalPumpconst || 10 < g_LocalPumpconst)
	{
		/*Show pop up*/
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		snprintf(arrPopUpBuff , 63 , "Invalid Pump Constant");
		if(enkeyOk == ShowMainPopUp("Pump Settings",arrPopUpBuff , true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
	}
	else
	{
		objstcSettings.u_n8DcMotorPwmDuty = g_LocalPwmDuty;
		objstcSettings.fPumpConstant = g_LocalPumpconst;
		/*save settings*/
		WriteSettingsBuffer();
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enkeyOk == ShowMainPopUp("Pump Settings","Saved" , true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
	}
}
void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbPwmDuty(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalPwmDuty , sizeof(g_LocalPwmDuty) ,false ,
			"Duty (40 - 100)" , std::to_string(g_LocalPwmDuty).c_str() , 3);
	BeepBuzzer();
}

void HandlerbPumpConst(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalPumpconst , sizeof(g_LocalPumpconst) ,true ,
			"Pump Constant (1 - 10)" ," " , 7);
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

void HandlerbPrinter(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Printer , (enWindowID)NULL);
	BeepBuzzer();
}
