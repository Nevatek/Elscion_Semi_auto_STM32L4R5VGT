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

static NexButton bBack = NexButton(en_WinId_OEMSettings , 1 , "b5");
static NexButton bSave = NexButton(en_WinId_OEMSettings , 6 , "b4");

static NexText tELSCION_NAME = NexText(en_WinId_OEMSettings, 7 , "t1");
static NexText tOEM1_NAME = NexText(en_WinId_OEMSettings, 10 , "t3");
static NexText tOEM2_NAME = NexText(en_WinId_OEMSettings, 13 , "t5");
static NexText tOEM3_NAME = NexText(en_WinId_OEMSettings, 16 , "t7");

static NexButton bELSCION_MODEL = NexButton(en_WinId_OEMSettings , 4 , "b0");
static NexButton bOEM1_MODEL = NexButton(en_WinId_OEMSettings , 8 , "b1");
static NexButton bOEM2_MODEL = NexButton(en_WinId_OEMSettings , 11 , "b2");
static NexButton bOEM3_MODEL = NexButton(en_WinId_OEMSettings , 14 , "b3");

static char g_ELSCION_NAME[MAX_OEM_NAME_LENGTH];
static char g_OEM1_NAME[MAX_OEM_NAME_LENGTH];
static char g_OEM2_NAME[MAX_OEM_NAME_LENGTH];
static char g_OEM3_NAME[MAX_OEM_NAME_LENGTH];

static uint16_t Elscion = NVM_OEM_DISABLE_FLAG;
static uint16_t OEM1 = NVM_OEM_DISABLE_FLAG;
static uint16_t OEM2 = NVM_OEM_DISABLE_FLAG;
static uint16_t OEM3 = NVM_OEM_DISABLE_FLAG;

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bSave,
									 &bELSCION_MODEL,
									 &bOEM1_MODEL,
									 &bOEM2_MODEL,
									 &bOEM3_MODEL,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbSave(void *ptr);
static void HandlerELSCION_MODEL(void *ptr);
static void HandlerOEM1_MODEL(void *ptr);
static void HandlerOEM2_MODEL(void *ptr);
static void HandlerOEM3_MODEL(void *ptr);
void DisableAllModels(void);

enWindowStatus ShowOEMSettingsScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();

	bBack.attachPush(HandlerbBack, &bBack);
	bSave.attachPush(HandlerbSave, &bSave);

	bELSCION_MODEL.attachPush(HandlerELSCION_MODEL, &bELSCION_MODEL);
	bOEM1_MODEL.attachPush(HandlerOEM1_MODEL, &bOEM1_MODEL);
	bOEM2_MODEL.attachPush(HandlerOEM2_MODEL, &bOEM2_MODEL);
	bOEM3_MODEL.attachPush(HandlerOEM3_MODEL, &bOEM3_MODEL);

	DisableAllModels();

	Elscion = objstcSettings.Elscion_Model;
	OEM1 = objstcSettings.OEM1_Model;
	OEM2 = objstcSettings.OEM2_Model;
	OEM3 = objstcSettings.OEM3_Model;

//	Printer = objstcSettings.u32PrinterEnable;
//	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
//	if(en_WinID_AlphaKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
//	{
//		/*Reset buffer*/
//		strncpy(g_ELSCION_NAME , DEFAULT_ELSCION_NAME , sizeof(g_ELSCION_NAME));
//		strncpy(g_OEM1_NAME , objstcSettings.arr_CFirstHeader , sizeof(g_OEM1_NAME));
//		strncpy(g_OEM2_NAME , objstcSettings.arr_CSecondHeader , sizeof(g_OEM2_NAME));
//		strncpy(g_OEM3_NAME , objstcSettings.arr_CEndfooter, sizeof(g_OEM3_NAME));
//	}

	tELSCION_NAME.setText(DEFAULT_ELSCION_NAME);
	tOEM1_NAME.setText(DEFAULT_OEM1_NAME);
	tOEM2_NAME.setText(DEFAULT_OEM2_NAME);
	tOEM3_NAME.setText(DEFAULT_OEM3_NAME);


	if(Elscion == NVM_OEM_ENABLE_FLAG)
	{
		bELSCION_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	else if(OEM1 == NVM_OEM_ENABLE_FLAG)
	{
		bOEM1_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	else if(OEM2 == NVM_OEM_ENABLE_FLAG)
	{
		bOEM2_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	else if(OEM3 == NVM_OEM_ENABLE_FLAG)
	{
		bOEM3_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}

	return WinStatus;
}

void HandlerOEMSettingsScreen (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void DisableAllModels(void)
{
	bELSCION_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	bOEM1_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	bOEM2_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	bOEM3_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);

	Elscion = NVM_OEM_DISABLE_FLAG;
	OEM1 = NVM_OEM_DISABLE_FLAG;
	OEM2 = NVM_OEM_DISABLE_FLAG;
	OEM3 = NVM_OEM_DISABLE_FLAG;
}
void HandlerbSave(void *ptr)
{
	BeepBuzzer();

	objstcSettings.Elscion_Model = Elscion;
	objstcSettings.OEM1_Model = OEM1;
	objstcSettings.OEM2_Model = OEM2;
	objstcSettings.OEM3_Model = OEM3;

	/*save settings*/
	WriteSettingsBuffer();
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enkeyOk == ShowMainPopUp("OEM Settings","Settings Saved" , true))
	{
		stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
		ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
	}
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_OEMLogin , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerELSCION_MODEL(void *ptr)
{
	BeepBuzzer();

	if(Elscion == NVM_OEM_ENABLE_FLAG)
	{
		DisableAllModels();
		Elscion = NVM_OEM_DISABLE_FLAG;
		bELSCION_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else
	{
		DisableAllModels();
		Elscion = NVM_OEM_ENABLE_FLAG;
		bELSCION_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
}

void HandlerOEM1_MODEL(void *ptr)
{
	BeepBuzzer();

	if(OEM1 == NVM_OEM_ENABLE_FLAG)
	{
		DisableAllModels();
		OEM1 = NVM_OEM_DISABLE_FLAG;
		bOEM1_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else
	{
		DisableAllModels();
		OEM1 = NVM_OEM_ENABLE_FLAG;
		bOEM1_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
}

void HandlerOEM2_MODEL(void *ptr)
{
	BeepBuzzer();

	if(OEM2 == NVM_OEM_ENABLE_FLAG)
	{
		DisableAllModels();
		OEM2 = NVM_OEM_DISABLE_FLAG;
		bOEM2_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else
	{
		DisableAllModels();
		OEM2 = NVM_OEM_ENABLE_FLAG;
		bOEM2_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
}

void HandlerOEM3_MODEL(void *ptr)
{
	BeepBuzzer();

	if(OEM3 == NVM_OEM_ENABLE_FLAG)
	{
		DisableAllModels();
		OEM3 = NVM_OEM_DISABLE_FLAG;
		bOEM3_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	else
	{
		DisableAllModels();
		OEM3 = NVM_OEM_ENABLE_FLAG;
		bOEM3_MODEL.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
}
