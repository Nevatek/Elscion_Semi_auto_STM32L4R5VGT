/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "./../Screens/Inc/CommonDisplayFunctions.h"

static NexText textTemperature = NexText(en_WinId_TestTimerScreen , 7 , "t7");
static NexText textTestName = NexText(en_WinId_TestTimerScreen , 4 , "t0");
static NexText textDateTime = NexText(en_WinId_TestTimerScreen , 6 , "t6");
static NexText textTestTypeName = NexText(en_WinId_TestTimerScreen , 5 , "t5");
static NexPicture pTestTypePic = NexPicture(en_WinId_TestTimerScreen , 3 , "p0");

/*Below is IDs for Timer start stop back - Start */
static NexButton bTestTimerBack = NexButton(en_WinId_TestTimerScreen , 2 , "b5");
static NexButton bTestTimerStartStop = NexButton(en_WinId_TestTimerScreen , 13 , "b0");
static NexButton bTestTimerResettimer = NexButton(en_WinId_TestTimerScreen , 14 , "b1");
static NexButton bTestTimerIcrease = NexButton(en_WinId_TestTimerScreen , 15 , "b2");
static NexButton bTestTimerDecrease = NexButton(en_WinId_TestTimerScreen , 16 , "b3");
/*Below is IDs for Timer start stop back  - END*/

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bTestTimerBack,
									&bTestTimerStartStop,
									&bTestTimerResettimer,
									&bTestTimerIcrease,
									&bTestTimerDecrease,
									NULL};

static void HandlerbTestTimerBack(void *ptr);
static void HandlerbTestTimerStartStop(void *ptr);
static void HandlerbTestTimerResettimer(void *ptr);
static void HandlerbTestTimerIcrease(void *ptr);
static void HandlerbTestTimerDecrease(void *ptr);

enWindowStatus ShowTestTimerScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

//	UpdateStatusBar(&pTestTypePic , &textTestName ,
//			&textTestTypeName , &textTemperature , &textDateTime);
//	UpdateStatusBar(NexPicture *pPicTestType , NexText *pTestName ,
//			NexText *pTxtDate, NexText *pTxtTime, NexText *pTxtPelTemp ,NexText *pTxtIncbTemp);

	/*Outputting result - ENDS*/

	bTestTimerBack.attachPush(HandlerbTestTimerBack, &bTestTimerBack);
	bTestTimerStartStop.attachPush(HandlerbTestTimerStartStop, &bTestTimerStartStop);
	bTestTimerResettimer.attachPush(HandlerbTestTimerResettimer, &bTestTimerResettimer);
	bTestTimerIcrease.attachPush(HandlerbTestTimerIcrease, &bTestTimerIcrease);
	bTestTimerDecrease.attachPush(HandlerbTestTimerDecrease, &bTestTimerDecrease);

	return WinStatus;
}

void HandlerTestTimerScreen(NexPage *ptr_obJCurrPage)
{
	if(StatusBarUpdateMillsDelay())
	{
//		UpdateStatusBar(&pTestTypePic , &textTestName ,
//				&textTestTypeName , &textTemperature , &textDateTime);
//		UpdateStatusBar(NexPicture *pPicTestType , NexText *pTestName ,
//				NexText *pTxtDate, NexText *pTxtTime, NexText *pTxtPelTemp ,NexText *pTxtIncbTemp);
	}
	nexLoop(nex_Listen_List);
}

void HandlerbTestTimerBack(void *ptr)
{
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}

void HandlerbTestTimerStartStop(void *ptr)
{
	BeepBuzzer();
}
void HandlerbTestTimerResettimer(void *ptr)
{
	BeepBuzzer();
}
void HandlerbTestTimerIcrease(void *ptr)
{
	BeepBuzzer();
}
void HandlerbTestTimerDecrease(void *ptr)
{
	BeepBuzzer();
}
