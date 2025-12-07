/*
 * BarcodeKeypad.c
 *
 *  Created on: Sep 2, 2022
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include <HandlerPheripherals.hpp>

#define ID_PLEASE_WAIT_POPUP (2)
uint8_t g_OKEnable = false;
std::string g_strPopUpMsg;
std::string g_strPopUpTitle;
enPopUpKeys g_KeyPressed = enKeyNull;

extern WindowMapping stcWindowMapping[en_WinID_Max];

/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/
static NexButton bYes = NexButton(en_WinId_MainPopup , 5, "b0");
static NexButton bNo = NexButton(en_WinId_MainPopup , 4, "b1");
static NexButton bOK = NexButton(en_WinId_MainPopup , 6, "b2");

static NexText tMsg = NexText(en_WinId_MainPopup , 3 , "t0");
static NexText tMsgTitle = NexText(en_WinId_MainPopup , 7 , "t1");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bYes,
									&bNo,
									&bOK,
									NULL};

static void HandlerButtonYes(void *ptr);
static void HandlerButtonNo(void *ptr);
static void HandlerButtonOk(void *ptr);

enWindowStatus ShowMainPopUpScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	 if(true == g_OKEnable)
	 {
		 bYes.setVisible(false);
		 bNo.setVisible(false);
		 bOK.setVisible(true);
	 }
	 else if(ID_PLEASE_WAIT_POPUP == g_OKEnable)
	 {
		 bYes.setVisible(false);
		 bNo.setVisible(false);
		 bOK.setVisible(false);
	 }
	 else
	 {
		 bOK.setVisible(false);
		 bYes.setVisible(true);
		 bNo.setVisible(true);

	 }

	bYes.attachPush(HandlerButtonYes, &bYes);
	bNo.attachPush(HandlerButtonNo, &bNo);
	bOK.attachPush(HandlerButtonOk, &bOK);


	tMsg.setText(g_strPopUpMsg.c_str());
	tMsgTitle.Set_font_color_pco(COLOUR_WHITE);
	tMsgTitle.setText(g_strPopUpTitle.c_str());
	g_KeyPressed = enKeyNull;
	return WinStatus;
}

void HandlerMainPopUpScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

static void HandlerButtonYes(void *ptr)
{
	g_KeyPressed = enKeyYes;
	BeepBuzzer();
}
static void HandlerButtonNo(void *ptr)
{
	g_KeyPressed = enKeyNo;
	BeepBuzzer();
}
static void HandlerButtonOk(void *ptr)
{
	g_KeyPressed = enkeyOk;
	BeepBuzzer();
}

enPopUpKeys ShowMainPopUp(std::string ptrTitle ,std::string ptrMsg , bool OK_Flag)
{
	g_OKEnable = OK_Flag;
	g_strPopUpMsg = ptrMsg;
	g_strPopUpTitle = ptrTitle;
	enWindowID enCurrentWindow = en_WinId_MainPopup;
	ShowMainPopUpScreen(&stcWindowMapping[enCurrentWindow].obJCurrPage);
	while(1)
	{
		HandlerMainPopUpScreen(&stcWindowMapping[enCurrentWindow].obJCurrPage);
		switch(g_KeyPressed)
		{
		 case enKeyYes:
		 case enKeyNo:
		 case enkeyOk:
			 return g_KeyPressed;
			 break;
		 default:break;
		}
	}
}

void ShowPleaseWaitPopup(std::string ptrTitle, std::string ptrMsg)
{
	g_OKEnable = ID_PLEASE_WAIT_POPUP;
	g_strPopUpMsg = ptrMsg;
	g_strPopUpTitle = ptrTitle;
	enWindowID enCurrentWindow = en_WinId_MainPopup;
	ShowMainPopUpScreen(&stcWindowMapping[enCurrentWindow].obJCurrPage);
}

void HidePleaseWaitPopup(enWindowID CurrWindow)
{
	ChangeWindowPage(CurrWindow, (enWindowID)NULL);
}
