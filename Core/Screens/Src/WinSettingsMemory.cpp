/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "../../APPL/Inc/NVM_TestResultsHistory.hpp"

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static enMemoryEraiseType g_Selmemtype = enTestPrograms;

static NexButton bBack = NexButton(en_WinId_SettingsMemory , 2, "b5");
static NexButton bAllTestResult = NexButton(en_WinId_SettingsMemory , 6, "b1");
static NexButton bAllTestPrograms = NexButton(en_WinId_SettingsMemory , 5, "b0");
static NexButton bDeleteMemory = NexButton(en_WinId_SettingsMemory , 7, "b2");
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bAllTestResult,
									 &bAllTestPrograms,
									 &bDeleteMemory,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbAllTestResult(void *ptr);
static void HandlerbAllTestPrograms(void *ptr);
static void HandlerbDeleteMemory(void *ptr);

enWindowStatus ShowSettingsMemoryScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bAllTestResult.attachPush(HandlerbAllTestResult, &bAllTestResult);
	bAllTestPrograms.attachPush(HandlerbAllTestPrograms, &bAllTestPrograms);
	bDeleteMemory.attachPush(HandlerbDeleteMemory, &bDeleteMemory);

	if(enTestPrograms == g_Selmemtype)
	{
		bAllTestPrograms.Set_background_image_pic(341);
		bAllTestResult.Set_background_image_pic(340);
	}
	else if(enTestResults == g_Selmemtype)
	{
		bAllTestPrograms.Set_background_image_pic(339);
		bAllTestResult.Set_background_image_pic(342);
	}
	return WinStatus;
}

void HandlerSettingsMemoryScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}
void HandlerbAllTestResult(void *ptr)
{
	if(enTestPrograms == g_Selmemtype)
	{
		g_Selmemtype = enTestResults;
		bAllTestPrograms.Set_background_image_pic(339);
		bAllTestResult.Set_background_image_pic(342);
		BeepBuzzer();
	}
}
void HandlerbAllTestPrograms(void *ptr)
{
	 if(enTestResults == g_Selmemtype)
	{
		g_Selmemtype = enTestPrograms;
		bAllTestPrograms.Set_background_image_pic(341);
		bAllTestResult.Set_background_image_pic(340);
		BeepBuzzer();
	}
}

static void HandlerbDeleteMemory(void *ptr)
{
	BeepBuzzer();
	if(enTestPrograms == g_Selmemtype)
	{
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enKeyYes == ShowMainPopUp("Memory","Do You Want to Clear All Tests?", false))
		{
			ShowPleaseWaitPopup("Memory",PLEASE_WAIT);
			InitALLTestParameterBuffer();
			WriteTestparameterBuffer();/*Writing tets params to memory*/
		}
		stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
		ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
	}
	else if(enTestResults == g_Selmemtype)
	{
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enKeyYes == ShowMainPopUp("Memory","Do You Want to Clear All Results?", false))
		{
			ShowPleaseWaitPopup("Memory",PLEASE_WAIT);
			DeleteAllestHistory();
		}
		stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
		ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
	}
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}
