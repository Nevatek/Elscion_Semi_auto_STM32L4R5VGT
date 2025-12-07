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

static NexButton bBack = NexButton(en_WinId_SettingsPrinter , 1 , "b5");
static NexButton bSave = NexButton(en_WinId_SettingsPrinter , 12 , "b0");
static NexText tLabName = NexText(en_WinId_SettingsPrinter, 8 , "t4");
static NexText tFirstHeader = NexText(en_WinId_SettingsPrinter, 9 , "t5");
static NexText tSecondHeader = NexText(en_WinId_SettingsPrinter, 10 , "t6");
static NexText tEndFooter = NexText(en_WinId_SettingsPrinter, 11 , "t7");
static NexButton bPrintOnOff = NexButton(en_WinId_SettingsPrinter , 13 , "b1");

static char g_arrLabName[MAX_LAB_NAME_LENGTH];
static char g_arrFirstHeader[MAX_LAB_NAME_LENGTH];
static char g_arrSecondHeader[MAX_LAB_NAME_LENGTH];
static char g_arrEndFooter[MAX_LAB_NAME_LENGTH];
static bool Printer = true;
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &tLabName,
									 &tFirstHeader,
									 &tSecondHeader,
									 &tEndFooter,
									 &bSave,
									 &bPrintOnOff,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbSave(void *ptr);
static void HandlertLabName(void *ptr);
static void HandlertFirstHeader(void *ptr);
static void HandlertSecondHeader(void *ptr);
static void HandlertEndFooter(void *ptr);
static void HandlerbPrintOnOff(void *ptr);

enWindowStatus ShowSettingsPrinterScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bSave.attachPush(HandlerbSave, &bSave);
	tLabName.attachPush(HandlertLabName, &tLabName);
	tFirstHeader.attachPush(HandlertFirstHeader, &tFirstHeader);
	tSecondHeader.attachPush(HandlertSecondHeader, &tSecondHeader);
	tEndFooter.attachPush(HandlertEndFooter, &tEndFooter);
	bPrintOnOff.attachPush(HandlerbPrintOnOff, &bPrintOnOff);
//	ReadSettingsBuffer();
	Printer = objstcSettings.u32PrinterEnable;
	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_AlphaKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
	{
		/*Reset buffer*/
		strncpy(g_arrLabName , objstcSettings.arr_CLabName , sizeof(g_arrLabName));
		strncpy(g_arrFirstHeader , objstcSettings.arr_CFirstHeader , sizeof(g_arrFirstHeader));
		strncpy(g_arrSecondHeader , objstcSettings.arr_CSecondHeader , sizeof(g_arrSecondHeader));
		strncpy(g_arrEndFooter , objstcSettings.arr_CEndfooter, sizeof(g_arrEndFooter));
	}
	tLabName.setText(g_arrLabName);
	tFirstHeader.setText(g_arrFirstHeader);
	tSecondHeader.setText(g_arrSecondHeader);
	tEndFooter.setText(g_arrEndFooter);

	if(Printer)
	{
		bPrintOnOff.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	else
	{
		bPrintOnOff.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}

	return WinStatus;
}

void HandlerSettingsPrinterScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbSave(void *ptr)
{
	BeepBuzzer();
	strncpy(objstcSettings.arr_CLabName , g_arrLabName , sizeof(objstcSettings.arr_CLabName));
	strncpy(objstcSettings.arr_CFirstHeader , g_arrFirstHeader , sizeof(objstcSettings.arr_CFirstHeader));
	strncpy(objstcSettings.arr_CSecondHeader , g_arrSecondHeader , sizeof(objstcSettings.arr_CSecondHeader));
	strncpy(objstcSettings.arr_CEndfooter , g_arrEndFooter , sizeof(objstcSettings.arr_CEndfooter));
	objstcSettings.u32PrinterEnable = Printer;
	/*save settings*/
	WriteSettingsBuffer();
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enkeyOk == ShowMainPopUp("Printer","Printer Settings Saved" , true))
	{
		stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
		ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
	}
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlertLabName(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrLabName , sizeof(g_arrLabName) , false ,
			"Enter Lab Name" , g_arrLabName , MAX_LAB_NAME_LENGTH-1);
	BeepBuzzer();
}
void HandlertFirstHeader(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrFirstHeader , sizeof(g_arrFirstHeader) , false ,
			"Enter First Header" , g_arrFirstHeader , MAX_LAB_NAME_LENGTH-1);
	BeepBuzzer();
}
void HandlertSecondHeader(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrSecondHeader , sizeof(g_arrSecondHeader) , false ,
			"Enter Second Header" , g_arrSecondHeader , MAX_LAB_NAME_LENGTH-1);
	BeepBuzzer();
}
void HandlertEndFooter(void *ptr)
{
	openKeyBoard(en_AlphaKeyboard , g_arrEndFooter , sizeof(g_arrEndFooter) , false ,
			"Enter End Footer" , g_arrEndFooter , MAX_LAB_NAME_LENGTH-1);
	BeepBuzzer();
}

void HandlerbPrintOnOff(void *ptr)
{
	if(Printer)
	{
		Printer = false;
		bPrintOnOff.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else
	{
		Printer = true;
		bPrintOnOff.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	BeepBuzzer();
}
