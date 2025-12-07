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

static NexButton bBack = NexButton(en_WinId_SettingsSysP_Temp_B , 1, "b5");
static NexButton tLamp = NexButton(en_WinId_SettingsSysP_Temp_B, 9 , "b0");
static NexButton tpump = NexButton(en_WinId_SettingsSysP_Temp_B, 8 , "b1");
static NexButton tBarcode = NexButton(en_WinId_SettingsSysP_Temp_B, 6 , "b3");
static NexButton tPrinter = NexButton(en_WinId_SettingsSysP_Temp_B, 5 , "b4");
static NexButton tSave = NexButton(en_WinId_SettingsSysP_Temp_B, 25 , "b8");
static NexButton tTempA_Flowcell = NexButton(en_WinId_SettingsSysP_Temp_B, 11 , "b6");

static NexButton bIncubatorOnOff = NexButton(en_WinId_SettingsSysP_Temp_B, 27 , "b9");

static NexText tSensorK = NexText(en_WinId_SettingsSysP_Temp_B, 14 , "t7");
static NexText tSensorD = NexText(en_WinId_SettingsSysP_Temp_B, 16 , "t9");
static NexText tTargetTemp = NexText(en_WinId_SettingsSysP_Temp_B, 18 , "t11");
static NexText tOffsetTemp = NexText(en_WinId_SettingsSysP_Temp_A, 29 , "t14");
static NexText tPid_P = NexText(en_WinId_SettingsSysP_Temp_B, 20 , "t1");
static NexText tPid_I = NexText(en_WinId_SettingsSysP_Temp_B, 22 , "t4");
static NexText tPid_D = NexText(en_WinId_SettingsSysP_Temp_B, 24 , "t6");



static float g_fSensorK = 0;
static float g_fSensorD = 0;
static float g_fDefaultTemp = 0;
static float g_fOffsetTemp = 0;
static float g_fPid_P = 0;
static float g_fPid_I = 0;
static float g_fPid_D = 0;


/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &tLamp,
									 &tpump,
									 &tBarcode,
									 &tPrinter,
									 &tTempA_Flowcell,
									 &tSave,
									 &tSensorK,
									 &tSensorD,
									 &tTargetTemp,
									 &tOffsetTemp,
									 &tPid_P,
									 &tPid_I,
									 &tPid_D,
									 &bIncubatorOnOff,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbLamp(void *ptr);
static void HandlerbPump(void *ptr);
static void Handlerbbarcode(void *ptr);
static void HandlerbPrinter(void *ptr);
static void HandlerbTemp_A(void *ptr);
static void HandlerbSave(void *ptr);
static void HandlerbSensorK(void *ptr);
static void HandlerbSensorD(void *ptr);
static void HandlerbSensorTaretTemp(void *ptr);
static void HandlerbSensorOffsetTemp(void *ptr);
static void HandlerbPid_P(void *ptr);
static void HandlerbPid_I(void *ptr);
static void HandlerbPid_D(void *ptr);
static void HandlerbIncuabtorOnOff(void *ptr);


enWindowStatus ShowSettingsSysP_Temp_B (NexPage *ptr_obJCurrPage)
{
	char arrPopUpBuff[64] = {0};
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	tLamp.attachPush(HandlerbLamp, &tLamp);
	tpump.attachPush(HandlerbPump, &tpump);
	tBarcode.attachPush(Handlerbbarcode, &tBarcode);
	tPrinter.attachPush(HandlerbPrinter, &tPrinter);
	tTempA_Flowcell.attachPush(HandlerbTemp_A, &tTempA_Flowcell);
	tSave.attachPush(HandlerbSave, &tSave);
	tSensorK.attachPush(HandlerbSensorK, &tSensorK);
	tSensorD.attachPush(HandlerbSensorD, &tSensorD);
	tTargetTemp.attachPush(HandlerbSensorTaretTemp, &tTargetTemp);
	tOffsetTemp.attachPush(HandlerbSensorOffsetTemp, &tOffsetTemp);
	tPid_P.attachPush(HandlerbPid_P, &tPid_P);
	tPid_I.attachPush(HandlerbPid_I, &tPid_I);
	tPid_D.attachPush(HandlerbPid_D, &tPid_D);
	bIncubatorOnOff.attachPush(HandlerbIncuabtorOnOff, &bIncubatorOnOff);

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_NumericKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
	{
		g_fSensorK = objstcSettings.fTempSensSlope[en_ChamperTemp];
		g_fSensorD = objstcSettings.fTempSensIntercept[en_ChamperTemp];
		g_fDefaultTemp = objstcSettings.fTargetTemp[en_ChamperTemp];
		g_fPid_P = objstcSettings.fPID_kP[en_ChamperTemp];
		g_fPid_I = objstcSettings.fPID_kI[en_ChamperTemp];
		g_fPid_D = objstcSettings.fPID_kD[en_ChamperTemp];
		g_fOffsetTemp = objstcSettings.fOffsetTemp[en_ChamperTemp];
	}

	snprintf(arrPopUpBuff , 63 , "%.02f" , (float)g_fSensorK);
	tSensorK.setText(arrPopUpBuff);
	snprintf(arrPopUpBuff , 63 , "%.02f" , (float)g_fSensorD);
	tSensorD.setText(arrPopUpBuff);
	snprintf(arrPopUpBuff , 63 , "%.02f" , (float)g_fDefaultTemp);
	tTargetTemp.setText(arrPopUpBuff);
	snprintf(arrPopUpBuff , 63 , "%.02f" , (float)g_fOffsetTemp);
	tOffsetTemp.setText(arrPopUpBuff);
	snprintf(arrPopUpBuff , 63 , "%.02f" , (float)g_fPid_P);
	tPid_P.setText(arrPopUpBuff);
	snprintf(arrPopUpBuff , 63 , "%.02f" , (float)g_fPid_I);
	tPid_I.setText(arrPopUpBuff);
	snprintf(arrPopUpBuff , 63 , "%.02f" , (float)g_fPid_D);
	tPid_D.setText(arrPopUpBuff);

	if(objstcSettings.fTempControlOnOff[en_ChamperTemp] == true)
	{
		bIncubatorOnOff.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
	}
	else
	{
		bIncubatorOnOff.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
	}
	return WinStatus;
}

void HandlerSettingsSysP_Temp_B (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
}

void HandlerbSave(void *ptr)
{
	BeepBuzzer();
	objstcSettings.fTempSensSlope[en_ChamperTemp] = g_fSensorK;
	objstcSettings.fTempSensIntercept[en_ChamperTemp] = g_fSensorD;
	objstcSettings.fTargetTemp[en_ChamperTemp] = g_fDefaultTemp;
	objstcSettings.fOffsetTemp[en_ChamperTemp] = g_fOffsetTemp;
	objstcSettings.fPID_kP[en_ChamperTemp] = g_fPid_P;
	objstcSettings.fPID_kI[en_ChamperTemp] = g_fPid_I;
	objstcSettings.fPID_kD[en_ChamperTemp] = g_fPid_D;
	/*save settings*/
	WriteSettingsBuffer();
	/*Save upcomming window id before show popup page*/
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enkeyOk == ShowMainPopUp("Incubator","Temperature Parameters Saved" , true))
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

void HandlerbPump(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Pump , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbTemp_A(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Temp_A , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbLamp(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Lamp , (enWindowID)NULL);
	BeepBuzzer();
}
void Handlerbbarcode(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Barcode , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbPrinter(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Printer , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbSensorK(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_fSensorK , sizeof(g_fSensorK) ,true ,
			"Sensor K" , std::to_string(g_fSensorK).c_str() , 4);
	BeepBuzzer();
}
void HandlerbSensorD(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_fSensorD , sizeof(g_fSensorD) ,true ,
			"Sensor D" , std::to_string(g_fSensorD).c_str() , 4);
	BeepBuzzer();
}
void HandlerbSensorTaretTemp(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_fDefaultTemp , sizeof(g_fDefaultTemp) ,true ,
			"Default Temperature" , std::to_string(g_fDefaultTemp).c_str() , 4);
	BeepBuzzer();
}
void HandlerbSensorOffsetTemp(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_fOffsetTemp , sizeof(g_fOffsetTemp) ,true ,
			"Offset Temperature" , std::to_string(g_fOffsetTemp).c_str() , 4);
	BeepBuzzer();
}
void HandlerbPid_P(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_fPid_P , sizeof(g_fPid_P) ,true ,
			"PID (P)" , std::to_string(g_fPid_P).c_str() , 4);
	BeepBuzzer();
}
void HandlerbPid_I(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_fPid_I , sizeof(g_fPid_I) ,true ,
			"PID (I)" , std::to_string(g_fPid_I).c_str() , 4);
	BeepBuzzer();
}
void HandlerbPid_D(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_fPid_D , sizeof(g_fPid_D) ,true ,
			"PID (D)" , std::to_string(g_fPid_D).c_str() , 4);
	BeepBuzzer();
}

void HandlerbIncuabtorOnOff(void *ptr)
{
	if(objstcSettings.fTempControlOnOff[en_ChamperTemp] == true)
	{
		bIncubatorOnOff.Set_background_image_pic(PICTURE_ID_BUTTON_OFF);
		objstcSettings.fTempControlOnOff[en_ChamperTemp] = false;
	}
	else
	{
		bIncubatorOnOff.Set_background_image_pic(PICTURE_ID_BUTTON_ON);
		objstcSettings.fTempControlOnOff[en_ChamperTemp] = true;
	}
	BeepBuzzer();
}
