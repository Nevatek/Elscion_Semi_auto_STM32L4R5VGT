/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "./../Screens/Inc/CommonDisplayFunctions.h"

static NexText textTemperature = NexText(en_WinId_TestQCSummaryScreen , 7 , "t7");
static NexText textTestName = NexText(en_WinId_TestQCSummaryScreen , 4 , "t0");
static NexText textDateTime = NexText(en_WinId_TestQCSummaryScreen , 6 , "t6");
static NexText textTestTypeName = NexText(en_WinId_TestQCSummaryScreen , 5 , "t5");
static NexPicture pTestTypePic = NexPicture(en_WinId_TestQCSummaryScreen , 3 , "p0");

/*Below is IDs for test abort POP UP */
static NexButton bTestQCSummaryBack = NexButton(en_WinId_TestQCSummaryScreen , 2 , "b5");
static NexButton bTestQCSummaryLJ = NexButton(en_WinId_TestQCSummaryScreen , 19 , "b4");
static NexButton bTestQCSummarySum = NexButton(en_WinId_TestQCSummaryScreen , 16 , "b1");
static NexButton bTestQCSummaryAccept = NexButton(en_WinId_TestQCSummaryScreen , 17 , "b2");
static NexButton bTestQCSummaryDecline = NexButton(en_WinId_TestQCSummaryScreen , 18 , "b3");
static NexButton bTestQCSummaryPrint = NexButton(en_WinId_TestQCSummaryScreen , 14 , "b0");
/*Below is IDs for test abort POP UP  - END*/

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bTestQCSummaryBack,
									&bTestQCSummaryLJ,
									&bTestQCSummarySum,
									&bTestQCSummaryAccept,
									&bTestQCSummaryDecline,
									&bTestQCSummaryPrint,
									NULL};

static void HandlerbTestQCSummaryBack(void *ptr);
static void HandlerbTestQCSummaryLJ(void *ptr);
static void HandlerbTestQCSummarySum(void *ptr);
static void HandlerbTestQCSummaryAccept(void *ptr);
static void HandlerbTestQCSummaryDecline(void *ptr);
static void HandlerbTestQCSummaryPrint(void *ptr);

enWindowStatus ShowTestQCSummaryScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	/*Outputting result - ENDS*/

	bTestQCSummaryBack.attachPush(HandlerbTestQCSummaryBack, &bTestQCSummaryBack);
	bTestQCSummaryLJ.attachPush(HandlerbTestQCSummaryLJ, &bTestQCSummaryLJ);
	bTestQCSummarySum.attachPush(HandlerbTestQCSummarySum, &bTestQCSummarySum);
	bTestQCSummaryAccept.attachPush(HandlerbTestQCSummaryAccept, &bTestQCSummaryAccept);
	bTestQCSummaryDecline.attachPush(HandlerbTestQCSummaryDecline, &bTestQCSummaryDecline);
	bTestQCSummaryPrint.attachPush(HandlerbTestQCSummaryPrint, &bTestQCSummaryPrint);

	return WinStatus;
}

void HandlerTestQCSummaryScreen(NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}



void HandlerbTestQCSummaryBack(void *ptr)
{
	ChangeWindowPage(stcScreenNavigation.NextWindowId , stcScreenNavigation.PrevWindowId);
	BeepBuzzer();
}

void HandlerbTestQCSummaryLJ(void *ptr)
{
	BeepBuzzer();
}

void HandlerbTestQCSummarySum(void *ptr)
{
	BeepBuzzer();
}

void HandlerbTestQCSummaryAccept(void *ptr)
{
	BeepBuzzer();
}

void HandlerbTestQCSummaryDecline(void *ptr)
{
	BeepBuzzer();
}

void HandlerbTestQCSummaryPrint(void *ptr)
{
	BeepBuzzer();
}


