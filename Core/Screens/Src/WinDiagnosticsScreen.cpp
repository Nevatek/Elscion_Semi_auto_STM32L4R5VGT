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

static NexButton bDiagnosticsBack = NexButton(en_WinId_DiagnosticsScreen , 2, "b5");

static NexButton bOptical = NexButton(en_WinId_DiagnosticsScreen , 3, "b0");
static NexButton bPump = NexButton(en_WinId_DiagnosticsScreen , 5, "b1");
static NexButton bFilterWheel = NexButton(en_WinId_DiagnosticsScreen , 8, "b2");
static NexButton bReplacement = NexButton(en_WinId_DiagnosticsScreen , 4, "b3");
static NexButton bTemperature = NexButton(en_WinId_DiagnosticsScreen , 6, "b4");
static NexButton bAutowash = NexButton(en_WinId_DiagnosticsScreen , 7, "b6");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bDiagnosticsBack,
									 &bOptical,
									 &bPump,
									 &bFilterWheel,
									 &bReplacement,
									 &bTemperature,
									 &bAutowash,
									 NULL};

static void HandlerbDiagnosticsBack(void *ptr);
static void HandlerbDiagnosticsOptical(void *ptr);
static void HandlerbDiagnosticsPump(void *ptr);
static void HandlerbDiagnosticsFilterWheel(void *ptr);
static void HandlerbDiagnosticsReplacement(void *ptr);
static void HandlerbDiagnosticsTemperature(void *ptr);
static void HandlerbDiagnosticsAutowash(void *ptr);

enWindowStatus ShowDiagnosticsScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bDiagnosticsBack.attachPush(HandlerbDiagnosticsBack, &bDiagnosticsBack);
	bOptical.attachPush(HandlerbDiagnosticsOptical, &bOptical);
	bPump.attachPush(HandlerbDiagnosticsPump, &bPump);
	bFilterWheel.attachPush(HandlerbDiagnosticsFilterWheel, &bFilterWheel);
	bReplacement.attachPush(HandlerbDiagnosticsReplacement, &bReplacement);
	bTemperature.attachPush(HandlerbDiagnosticsTemperature, &bTemperature);
	bAutowash.attachPush(HandlerbDiagnosticsAutowash, &bAutowash);

	/*Read settings buffer*/
//	ReadSettingsBuffer();
	return WinStatus;
}

void HandlerDiagnosticsScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbDiagnosticsBack(void *ptr)
{
	ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbDiagnosticsOptical(void *ptr)
{
	ChangeWindowPage(en_WinId_Diag_Optical , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbDiagnosticsPump(void *ptr)
{
	ChangeWindowPage(en_WinId_Diag_Pump , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbDiagnosticsFilterWheel(void *ptr)
{
	ChangeWindowPage(en_WinId_Diag_Filter , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbDiagnosticsReplacement(void *ptr)
{
	ChangeWindowPage(en_WinId_LampReplaceLogin , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbDiagnosticsTemperature(void *ptr)
{
	ChangeWindowPage(en_WinId_Diag_Temp , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbDiagnosticsAutowash(void *ptr)
{
	ChangeWindowPage(en_WinId_Diag_AutoWash_A , (enWindowID)NULL);
	BeepBuzzer();
}
