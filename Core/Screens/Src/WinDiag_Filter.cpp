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

typedef enum
{
	enFilter_Idle = 0,
	enFilter_StartTest,
	enFilter_StartTest_Busy,
	enFilter_TestCompleted,
	enFilter_Select450,
	enFilter_Select670,
}enFilterTest;

enFilterTest g_FilterTestState = enFilter_Idle;
static stcTimer g_stcTimer;
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_Diag_Filter , 1, "b5");
static NexButton bAspirate = NexButton(en_WinId_Diag_Filter , 4, "b1");
static NexText tMsgBoxMain = NexText(en_WinId_Diag_Filter, 5 , "t0");
static NexText tMsgBox1 = NexText(en_WinId_Diag_Filter, 7 , "t1");
static NexText tMsgBox2 = NexText(en_WinId_Diag_Filter, 8 , "t2");
static NexText tMsgBox3 = NexText(en_WinId_Diag_Filter, 9 , "t3");
static NexPicture pMsgpox = NexPicture(en_WinId_Diag_Filter, 6 , "p0");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bAspirate,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbAspirate(void *ptr);
void ShowMsgBox(std::string MsgHomePos , std::string MsgFilterWheel);
static void HideMsgBox(void);
static enFilterTest Get_FilterTaskState(void);
static void FilterWheel_Task(void);

enWindowStatus ShowDiagScreen_Filter (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bAspirate.attachPush(HandlerbAspirate, &bAspirate);
	bAspirate.Set_background_image_pic(354);
	HideMsgBox();
	tMsgBoxMain.setText("Test");
	g_FilterTestState = enFilter_Idle;
	AspSwitchLed_Red(en_AspLedOFF);
	AspSwitchLed_White(en_AspLedON);

	return WinStatus;
}

void HandlerDiagScreen_Filter (NexPage *ptr_obJCurrPage)
{
	FilterWheel_Task();
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	if(enFilter_TestCompleted == Get_FilterTaskState() || enFilter_Idle == Get_FilterTaskState())
	{
		RotateFilterMotorAngle(25 , en_FilterMotorRight); // home angle from 670
//		RotateFilterMotorAngle(250 , en_FilterMotorRight); // home angle from 450
		ChangeWindowPage(en_WinId_DiagnosticsScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
enFilterTest Get_FilterTaskState(void)
{
	return g_FilterTestState;
}
void HandlerDiagFilterAspButton(void)
{
	if(enFilter_Idle == Get_FilterTaskState())
	{
		AspSwitchLed_Red(en_AspLedON);
		AspSwitchLed_White(en_AspLedOFF);

		bAspirate.Set_background_image_pic(355);
		g_FilterTestState = enFilter_StartTest;
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void HandlerbAspirate(void *ptr)
{
	HandlerDiagFilterAspButton();
}
void ShowMsgBox(std::string MsgHomePos , std::string MsgFilterWheel)
{
	pMsgpox.setVisible(true);
	tMsgBox1.setVisible(true);
	tMsgBox2.setVisible(true);
	tMsgBox3.setVisible(true);
	tMsgBox1.setText("Test Completed");
	tMsgBox2.setText(MsgFilterWheel.c_str());
	tMsgBox3.setText(MsgHomePos.c_str());
}
void HideMsgBox(void)
{
	pMsgpox.setVisible(false);
	tMsgBox1.setVisible(false);
	tMsgBox2.setVisible(false);
	tMsgBox3.setVisible(false);
}
void FilterWheel_Task(void)
{
	switch(g_FilterTestState)
	{
		case enFilter_Idle:
		break;
		case enFilter_StartTest:
		{
			SelectFilter((en_WavelengthFilter)en_FilterHome);
			StartTimer(&g_stcTimer , 1 * 2000);/*Homming max delay*/
			g_FilterTestState = enFilter_StartTest_Busy;
			tMsgBoxMain.setText("Testing");
		}
		break;
		case enFilter_StartTest_Busy:
		{
			if(true == Timer_IsTimeout(&g_stcTimer , enTimerNormalStop))
			{
				if(en_FilterMotorBusy != ReadFilterMotorStatus())
				{
					if(en_FilterMotorHomeError == ReadFilterMotorStatus())
					{
						ShowMsgBox("Home Position : ERROR" , "Filter Wheel : ERROR");
					}
					else
					{
						ShowMsgBox("Home Position : OK" , "Filter Wheel : OK");
					}

					g_FilterTestState = enFilter_Select670;
				}
				else
				{
					StartTimer(&g_stcTimer , 1 * 1000);/*Homming max delay*/
				}
			}
		}
		break;

		case enFilter_TestCompleted:
		{
//			g_FilterTestState = enFilter_Idle;
			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);
		}
		break;

		case enFilter_Select450:
		{
			RotateFilterMotorAngle(110 , en_FilterMotorRight);
			g_FilterTestState = enFilter_TestCompleted;
		}
		break;

		case enFilter_Select670:
		{
			RotateFilterMotorAngle(335 , en_FilterMotorRight);
			g_FilterTestState = enFilter_TestCompleted;
		}
		break;
	}
}

