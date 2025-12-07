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

static NexButton bHomeFavBack = NexButton(en_WinId_HomeFavouriteScreen , 5, "b0");

static NexText textTemperature = NexText(en_WinId_TestRun , 15 , "t7");
static NexText textDateTime = NexText(en_WinId_TestRun , 16 , "t6");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bHomeFavBack,
									 NULL};

static void HandlerbHomeFavBack(void *ptr);

enWindowStatus ShowHomeFavScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bHomeFavBack.attachPush(HandlerbHomeFavBack, &bHomeFavBack);
	return WinStatus;
}

void HandlerHomeFavScreen (NexPage *ptr_obJCurrPage)
{
	if(StatusBarUpdateMillsDelay())
	{
//		UpdateHomeSreenDateTimeTemp(&textDateTime, &textPeltTemp,  &textIncuTemp);
	}
	nexLoop(nex_Listen_List);
}

void HandlerbHomeFavBack(void *ptr)
{
	ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
	BeepBuzzer();
}


