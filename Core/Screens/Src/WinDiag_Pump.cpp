/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "Appl_Timer.hpp"
#include <HandlerPheripherals.hpp>

#define DEFAULT_PUMP_CALIB_ASP_VOL_UL (500)
typedef enum
{
	enDiagPump_Idle = 0,
	enDiagPump_Asp1000Ul,
	enDiagPump_Asp1000Ul_Busy,
	enDiagPump_Asp1000Ul_Busy_PostDelay,
	enDiagPump_AspAirgap_Delay,
	enDiagPump_AspAirgap_PostDelay,
	enDiagPump_Asp1000Ul_Completed,
}en_DiagPumpState;

static uint32_t g_u32RemainigVoleume = 0;
static en_DiagPumpState g_DiagPumpState = enDiagPump_Idle;
static stcTimer g_DelayGapTimer;

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_Diag_Pump , 1, "b5");
static NexButton bAspirate = NexButton(en_WinId_Diag_Pump , 4, "b1");
static NexText tMsgBoxMain = NexText(en_WinId_Diag_Filter, 5 , "t0");
static NexText tMsgBox1 = NexText(en_WinId_Diag_Pump, 7 , "t1");
static NexText tMsgBox2 = NexText(en_WinId_Diag_Pump, 8 , "t2");
static NexText tMsgBox3 = NexText(en_WinId_Diag_Pump, 9 , "t3");
static NexPicture pMsgpox = NexPicture(en_WinId_Diag_Pump, 6 , "p0");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bAspirate,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbAspirate(void *ptr);
static void Pump_DiagTask(void);
static void Start_PumpAsp(void);
static en_DiagPumpState Get_DiagPumpStatus(void);
static void ShowMsgBox(std::string Msg);
static void HideMsgBox(void);
static float Calc_PumpConst(uint32_t u32RemVol);

enWindowStatus ShowDiagScreen_Pump (NexPage *ptr_obJCurrPage)
{
	char arrBuff[64] = {0};
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bAspirate.attachPush(HandlerbAspirate, &bAspirate);

	AspSwitchLed_Red(en_AspLedOFF);
	AspSwitchLed_White(en_AspLedON);

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_NumericKeypad == PrevWindow && g_u32RemainigVoleume != 0)
	{
		objstcSettings.fPumpConstant = Calc_PumpConst(g_u32RemainigVoleume);
		if(objstcSettings.fPumpConstant > 4 || objstcSettings.fPumpConstant < 1.5)
		{
			snprintf(arrBuff , 63 , "New Pump Constant : %.03f Out of range" , objstcSettings.fPumpConstant);
		}
		else
		{
			snprintf(arrBuff , 63 , "New Pump Constant : %.03f" , objstcSettings.fPumpConstant);
		}
		ShowMsgBox(std::string(arrBuff));
		WriteSettingsBuffer();
	}
	else
	{
		g_u32RemainigVoleume = 0;/*Resets remaining voleume*/
		HideMsgBox();
		tMsgBoxMain.setText("Aspirate 1mL of DI Water");
	}
	return WinStatus;
}
float Calc_PumpConst(uint32_t u32RemVol)
{
	float fPumpK = 0;
	float DEFAULT_ASP_VOL_UL = DEFAULT_PUMP_CALIB_ASP_VOL_UL;
	float RemVol = u32RemVol;
	fPumpK = objstcSettings.fPumpConstant * (DEFAULT_ASP_VOL_UL / (1000 - RemVol));
	return fPumpK;
}
void HandlerDiagScreen_Pump (NexPage *ptr_obJCurrPage)
{
	Pump_DiagTask();
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	if(enDiagPump_Idle == Get_DiagPumpStatus())
	{
		ChangeWindowPage(en_WinId_DiagnosticsScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void ShowMsgBox(std::string Msg)
{
	pMsgpox.setVisible(true);
	tMsgBox1.setVisible(true);
	tMsgBox2.setVisible(true);
	tMsgBox3.setVisible(true);
	tMsgBox1.setText("Calibration Successful !");
	tMsgBox2.setText("Aspiration Volume : 500uL");
	tMsgBox3.setText(Msg.c_str());
}
void HideMsgBox(void)
{
	pMsgpox.setVisible(false);
	tMsgBox1.setVisible(false);
	tMsgBox2.setVisible(false);
	tMsgBox3.setVisible(false);
}
void HandlerDiagPumpAspButton(void)
{
	if(enDiagPump_Idle != Get_DiagPumpStatus())
	{
		InstrumentBusyBuzz();
		return;
	}
	BeepBuzzer();

	AspSwitchLed_Red(en_AspLedON);
	AspSwitchLed_White(en_AspLedOFF);

	bAspirate.Set_background_image_pic(355);
	Start_PumpAsp();
	tMsgBoxMain.setText("Aspirating");

}
void HandlerbAspirate(void *ptr)
{
	HandlerDiagPumpAspButton();
}

en_DiagPumpState Get_DiagPumpStatus(void)
{
	return g_DiagPumpState;
}
void Start_PumpAsp(void)
{
	g_DiagPumpState = enDiagPump_Asp1000Ul;
}
void Pump_DiagTask(void)
{
	switch(g_DiagPumpState)
	{
		case enDiagPump_Idle:
		{

		}break;
		case enDiagPump_Asp1000Ul:
		{
			Aspirate_Pump(DEFAULT_PUMP_CALIB_ASP_VOL_UL);
			g_DiagPumpState = enDiagPump_Asp1000Ul_Busy;
		}break;
		case enDiagPump_Asp1000Ul_Busy:
		{
			if(enPump_AspCompleted == Pump_RunTask())
			{
				g_DiagPumpState = enDiagPump_Asp1000Ul_Busy_PostDelay;
				LongBeepBuzzer();
				StartTimer(&g_DelayGapTimer , 2 * 1000);/*Homming max delay*/
			}
		}break;
		case enDiagPump_Asp1000Ul_Busy_PostDelay:
		{
			if(true == Timer_IsTimeout(&g_DelayGapTimer , enTimerNormalStop))
			{
				g_DiagPumpState = enDiagPump_Asp1000Ul_Completed;
			}
		}break;

		case enDiagPump_AspAirgap_Delay:
		{
			Aspirate_Pump(objstcSettings.fAirGapVol_uL);//Aspiration of Air gap //added
			g_DiagPumpState = enDiagPump_AspAirgap_PostDelay;
		}break;

		case enDiagPump_AspAirgap_PostDelay:
		{
			if(enPump_AspCompleted == Pump_RunTask())
			{
				g_DiagPumpState = enDiagPump_Asp1000Ul_Completed;
			}
		}break;
		case enDiagPump_Asp1000Ul_Completed:
		{
			g_DiagPumpState = enDiagPump_Idle;
			openKeyBoard(en_NumericKeyboard , &g_u32RemainigVoleume , sizeof(g_u32RemainigVoleume) ,false ,
					"Enter Remaining Volume (uL)" , std::to_string(g_u32RemainigVoleume).c_str() , 4);
			bAspirate.Set_background_image_pic(354);
		}break;
	}
}
