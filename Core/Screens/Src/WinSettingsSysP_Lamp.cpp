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

static NexButton bBack = NexButton(en_WinId_SettingsSysP_Lamp , 1, "b5");
static NexText tFilterGain[en_FilterMax] = {NexText(en_WinId_SettingsSysP_Lamp, 11 , "t1"),/*DUMMY for home*/
											NexText(en_WinId_SettingsSysP_Lamp, 19 , "t9"),/*340*/
											NexText(en_WinId_SettingsSysP_Lamp, 20 , "t10"),/*405*/
//											NexText(en_WinId_SettingsSysP_Lamp, 21 , "t11"),/*450*/
											NexText(en_WinId_SettingsSysP_Lamp, 24 , "t14"),/*505*/
											NexText(en_WinId_SettingsSysP_Lamp, 23 , "t13"),/*546*/
											NexText(en_WinId_SettingsSysP_Lamp, 22 , "t12"),/*578*/
											NexText(en_WinId_SettingsSysP_Lamp, 26 , "t16"),/*630*/
//											NexText(en_WinId_SettingsSysP_Lamp, 25 , "t15"),/*670*/
											NexText(en_WinId_SettingsSysP_Lamp, 27 , "t17")};/*Black*/

static NexText tAngleFilterToFilter = NexText(en_WinId_SettingsSysP_Lamp, 29 , "t19");
static NexText tLightPathOffset = NexText(en_WinId_SettingsSysP_Lamp, 30 , "t20");
static NexText tFilterRpm = NexText(en_WinId_SettingsSysP_Lamp, 32 , "t22");
static NexButton tSave = NexButton(en_WinId_SettingsSysP_Lamp, 34 , "b6");

static NexButton bPreampGainResCalc[enPreGainResValMax] = {NexButton(en_WinId_SettingsSysP_Lamp, 36 , "t25"),
												NexButton(en_WinId_SettingsSysP_Lamp, 38 , "t27"),
												NexButton(en_WinId_SettingsSysP_Lamp, 40 , "t29"),
												NexButton(en_WinId_SettingsSysP_Lamp, 42 , "t31")};

static NexButton tpump = NexButton(en_WinId_SettingsSysP_Lamp, 8 , "b1");
static NexButton tTemp = NexButton(en_WinId_SettingsSysP_Lamp, 7 , "b2");
static NexButton tBarcode = NexButton(en_WinId_SettingsSysP_Lamp, 6 , "b3");
static NexButton tPrinter = NexButton(en_WinId_SettingsSysP_Lamp, 5 , "b4");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &tFilterGain[en_FilterHome],
									 &tFilterGain[en_Filter340],
									 &tFilterGain[en_Filter405],
//									 &tFilterGain[en_Filter450],
									 &tFilterGain[en_Filter505],
									 &tFilterGain[en_Filter546],
									 &tFilterGain[en_Filter578],
									 &tFilterGain[en_Filter630],
//									 &tFilterGain[en_Filter670],
									 &tFilterGain[en_FilterBlack],
									 &bPreampGainResCalc[enPreGainResValR1],
									 &bPreampGainResCalc[enPreGainResValR2],
									 &bPreampGainResCalc[enPreGainResValR3],
									 &bPreampGainResCalc[enPreGainResValR4],
									 &tAngleFilterToFilter,
									 &tLightPathOffset,
									 &tFilterRpm,
									 &tSave,
									 &tpump,
									 &tTemp,
									 &tBarcode,
									 &tPrinter,
									 NULL};


static enPreampGain g_LocalFilterPreampGain[en_FilterMax];
static float g_LocalPreampGainCalc[enPreGainResValMax];
static uint16_t g_AngleFilterToFilter = 0;
static uint16_t g_AngleLightpathOffset = 0;
static uint16_t g_FilterRpm = 0;

static void HandlerbBack(void *ptr);
static void Handlerb340(void *ptr);
static void Handlerb405(void *ptr);
//static void Handlerb450(void *ptr);
static void Handlerb505(void *ptr);
static void Handlerb546(void *ptr);
static void Handlerb578(void *ptr);
static void Handlerb630(void *ptr);
//static void Handlerb670(void *ptr);
static void HandlerbBlack(void *ptr);
static void HandlertAngleFilterToFilter(void *ptr);
static void HandlertAngleLightPathOffset(void *ptr);
static void HandlertFilterRpm(void *ptr);
static void HandlerbSave(void *ptr);
static void HandlerbPump(void *ptr);
static void HandlerbTemp_A(void *ptr);
static void HandlerbBarcode(void *ptr);
static void HandlerbPrinter(void *ptr);
static void HandlerbPreampGainCalc_R1(void *ptr);
static void HandlerbPreampGainCalc_R2(void *ptr);
static void HandlerbPreampGainCalc_R3(void *ptr);
static void HandlerbPreampGainCalc_R4(void *ptr);

enWindowStatus ShowSettingsSysP_Lamp (NexPage *ptr_obJCurrPage)
{
	char arrPopUpBuff[64] = {0};
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	tFilterGain[en_Filter340].attachPush(Handlerb340, &tFilterGain[en_Filter340]);
	tFilterGain[en_Filter405].attachPush(Handlerb405, &tFilterGain[en_Filter405]);
//	tFilterGain[en_Filter450].attachPush(Handlerb450, &tFilterGain[en_Filter450]);
	tFilterGain[en_Filter505].attachPush(Handlerb505, &tFilterGain[en_Filter505]);
	tFilterGain[en_Filter546].attachPush(Handlerb546, &tFilterGain[en_Filter546]);
	tFilterGain[en_Filter578].attachPush(Handlerb578, &tFilterGain[en_Filter578]);
	tFilterGain[en_Filter630].attachPush(Handlerb630, &tFilterGain[en_Filter630]);
//	tFilterGain[en_Filter670].attachPush(Handlerb670, &tFilterGain[en_Filter670]);
	tFilterGain[en_FilterBlack].attachPush(HandlerbBlack, &tFilterGain[en_FilterBlack]);
	tAngleFilterToFilter.attachPush(HandlertAngleFilterToFilter, &tAngleFilterToFilter);
	tLightPathOffset.attachPush(HandlertAngleLightPathOffset, &tLightPathOffset);
	tFilterRpm.attachPush(HandlertFilterRpm, &tFilterRpm);
	tSave.attachPush(HandlerbSave, &tSave);
	tpump.attachPush(HandlerbPump, &tpump);
	tTemp.attachPush(HandlerbTemp_A, &tTemp);
	tBarcode.attachPush(HandlerbBarcode, &tBarcode);
	tPrinter.attachPush(HandlerbPrinter, &tPrinter);
	bPreampGainResCalc[enPreGainResValR1].attachPush(HandlerbPreampGainCalc_R1, &bPreampGainResCalc[enPreGainResValR1]);
	bPreampGainResCalc[enPreGainResValR2].attachPush(HandlerbPreampGainCalc_R2, &bPreampGainResCalc[enPreGainResValR2]);
	bPreampGainResCalc[enPreGainResValR3].attachPush(HandlerbPreampGainCalc_R3, &bPreampGainResCalc[enPreGainResValR3]);
	bPreampGainResCalc[enPreGainResValR4].attachPush(HandlerbPreampGainCalc_R4, &bPreampGainResCalc[enPreGainResValR4]);

	enWindowID PrevWindow = stcScreenNavigation.PrevWindowId;
	if(en_WinID_NumericKeypad != PrevWindow && en_WinId_MainPopup != PrevWindow)
	{
		/*Reset buffer if enterd freshly to this screen*/
		for(uint8_t nI = en_Filter340 ; nI <= en_FilterBlack ; ++nI)
		{
			g_LocalFilterPreampGain[nI] = objstcSettings.FilterPreampGain[nI];/*Reset gain values*/
		}

		/*Reset buffer if enterd freshly to this screen*/
		for(uint8_t nI = 0 ; nI < enPreGainResValMax ; ++nI)
		{
			g_LocalPreampGainCalc[nI] = objstcSettings.fGainResCalculatorBit[nI];/*Reset gain values*/
		}

		/*Reset other values*/
		g_FilterRpm = objstcSettings.u_n8FilterMotorRPM;
		g_AngleLightpathOffset = objstcSettings.AngleLightPathOffset;
		g_AngleFilterToFilter =  objstcSettings.AngleFilterToFilter;
	}

	/*Show text boxes*/
	for(uint8_t nI = en_Filter340 ; nI <= en_FilterBlack ; ++nI)
	{
		snprintf(arrPopUpBuff , 63 , "%u" , (unsigned int)g_LocalFilterPreampGain[nI]);
		tFilterGain[nI].setText(arrPopUpBuff);
	}
	snprintf(arrPopUpBuff , 63 , "%u" , (unsigned int)g_AngleFilterToFilter);
	tAngleFilterToFilter.setText(arrPopUpBuff);

	snprintf(arrPopUpBuff , 63 , "%u" , (unsigned int)g_AngleLightpathOffset);
	tLightPathOffset.setText(arrPopUpBuff);

	snprintf(arrPopUpBuff , 63 , "%u" , (unsigned int)g_FilterRpm);
	tFilterRpm.setText(arrPopUpBuff);

	for(uint8_t nI = 0 ; nI < enPreGainResValMax ; ++nI)
	{
		snprintf(arrPopUpBuff , 63 , "%.02f" , (float)g_LocalPreampGainCalc[nI]);
		bPreampGainResCalc[nI].setText(arrPopUpBuff);
	}
	return WinStatus;
}

void HandlerSettingsSysP_Lamp (NexPage *ptr_obJCurrPage)
{
	nexLoop(nex_Listen_List);
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
void HandlerbBarcode(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Barcode , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbPrinter(void *ptr)
{
	ChangeWindowPage(en_WinId_SettingsSysP_Printer , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbSave(void *ptr)
{
	BeepBuzzer();
	char arrPopUpBuff[64] = {0};
	bool bSaveEnable = true;
	/*validate values*/
	for(uint8_t nI = en_Filter340 ; nI <= en_FilterBlack ; ++nI)
	{
		if(en_PreampGain_OFF > g_LocalFilterPreampGain[nI] ||
				en_PreampGain_15 < g_LocalFilterPreampGain[nI])/*if gain is not in range*/
		{
				/*Show pop up*/
				/*Save upcomming window id before show popup page*/
				enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
				snprintf(arrPopUpBuff , 63 , "Invalid Gain Values (Filter : %d)" , nI);
				if(enkeyOk == ShowMainPopUp("Lamp Settings",arrPopUpBuff , true))
				{
					stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
					ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
				}
				bSaveEnable = false;
				return;
		}
		else
		{
			objstcSettings.FilterPreampGain[nI] = g_LocalFilterPreampGain[nI];
		}
	}

	for(uint8_t nI = 0 ; nI < enPreGainResValMax ; ++nI)
	{
		 objstcSettings.fGainResCalculatorBit[nI] = g_LocalPreampGainCalc[nI];/*copy gain resistance values*/
	}

	if(0 > g_AngleFilterToFilter || 360 < g_AngleFilterToFilter)/*if angle is not in range*/
	{
		/*Show pop up*/
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		snprintf(arrPopUpBuff , 63 , "Invalid Home to Filter Angle");
		if(enkeyOk == ShowMainPopUp("Filter Wheel",arrPopUpBuff , true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
		bSaveEnable = false;
	}

	else if(0 > g_AngleLightpathOffset || 360 < g_AngleLightpathOffset)/*if angle is not in range*/
	{
		/*Show pop up*/
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		snprintf(arrPopUpBuff , 63 , "Invalid Filter to Filter Angle");
		if(enkeyOk == ShowMainPopUp("Filter Wheel",arrPopUpBuff , true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
		bSaveEnable = false;
	}

	else if(20 > g_FilterRpm || 90 < g_FilterRpm)/*if angle is not in range*/
	{
		/*Show pop up*/
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		snprintf(arrPopUpBuff , 63 , "Invalid Filter RPM");
		if(enkeyOk == ShowMainPopUp("Filter Wheel",arrPopUpBuff , true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
		bSaveEnable = false;
	}

	if(true == bSaveEnable)
	{
		objstcSettings.u_n8FilterMotorRPM = g_FilterRpm;
		objstcSettings.AngleFilterToFilter = g_AngleFilterToFilter;
		objstcSettings.AngleLightPathOffset = g_AngleLightpathOffset ;

		objstcSettings.WavelegthFilterAngles[en_Filter340] = ((objstcSettings.AngleFilterToFilter * 0) + 1) + objstcSettings.AngleLightPathOffset;
		objstcSettings.WavelegthFilterAngles[en_Filter405] = ((objstcSettings.AngleFilterToFilter * 1) + 1) + objstcSettings.AngleLightPathOffset;
//		objstcSettings.WavelegthFilterAngles[en_Filter450] = ((objstcSettings.AngleFilterToFilter * 2) + 1) + objstcSettings.AngleLightPathOffset;
		objstcSettings.WavelegthFilterAngles[en_Filter505] = ((objstcSettings.AngleFilterToFilter * 3) + 1) + objstcSettings.AngleLightPathOffset;
		objstcSettings.WavelegthFilterAngles[en_Filter546] = ((objstcSettings.AngleFilterToFilter * 4) + 1) + objstcSettings.AngleLightPathOffset;
		objstcSettings.WavelegthFilterAngles[en_Filter578] = ((objstcSettings.AngleFilterToFilter * 5) + 1) + objstcSettings.AngleLightPathOffset;
		objstcSettings.WavelegthFilterAngles[en_Filter630] = ((objstcSettings.AngleFilterToFilter * 6) + 1) + objstcSettings.AngleLightPathOffset;
//		objstcSettings.WavelegthFilterAngles[en_Filter670] = ((objstcSettings.AngleFilterToFilter * 7) + 1) + objstcSettings.AngleLightPathOffset;

		CalculatePreampGainResistance();
		/*save settings*/
		WriteSettingsBuffer();
		/*Save upcomming window id before show popup page*/
		enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
		if(enkeyOk == ShowMainPopUp("System Settings","Saved" , true))
		{
			stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
			ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
		}
		bSaveEnable = false;
	}
}
void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_SystemsettingsScreen , (enWindowID)NULL);
	BeepBuzzer();
}
void HandlerbPreampGainCalc_R1(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalPreampGainCalc[enPreGainResValR1] , sizeof(g_LocalPreampGainCalc[enPreGainResValR1]) ,true ,
			"Gain 1 Value" , std::to_string(g_LocalPreampGainCalc[enPreGainResValR1]).c_str() , 5);
	BeepBuzzer();
}
void HandlerbPreampGainCalc_R2(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalPreampGainCalc[enPreGainResValR2] , sizeof(g_LocalPreampGainCalc[enPreGainResValR2]) ,true ,
			"Gain 2 Value" , std::to_string(g_LocalPreampGainCalc[enPreGainResValR2]).c_str() , 5);
	BeepBuzzer();
}
void HandlerbPreampGainCalc_R3(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalPreampGainCalc[enPreGainResValR3] , sizeof(g_LocalPreampGainCalc[enPreGainResValR3]) ,true ,
			"Gain 3 Value" , std::to_string(g_LocalPreampGainCalc[enPreGainResValR3]).c_str() , 5);
	BeepBuzzer();
}
void HandlerbPreampGainCalc_R4(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalPreampGainCalc[enPreGainResValR4] , sizeof(g_LocalPreampGainCalc[enPreGainResValR4]) ,true ,
			"Gain 4 Value" , std::to_string(g_LocalPreampGainCalc[enPreGainResValR4]).c_str() , 5);
	BeepBuzzer();
}
void Handlerb340(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalFilterPreampGain[en_Filter340] , sizeof(g_LocalFilterPreampGain[en_Filter340]) ,false ,
			"Gain - 340 (0 - 15)" , std::to_string(g_LocalFilterPreampGain[en_Filter340]).c_str() , 2);
	BeepBuzzer();
}
void Handlerb405(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalFilterPreampGain[en_Filter405] , sizeof(g_LocalFilterPreampGain[en_Filter405]) ,false ,
			"Gain - 405 (0 - 15)" , std::to_string(g_LocalFilterPreampGain[en_Filter405]).c_str() , 2);
	BeepBuzzer();
}
//void Handlerb450(void *ptr)
//{
//	openKeyBoard(en_NumericKeyboard , &g_LocalFilterPreampGain[en_Filter450] , sizeof(g_LocalFilterPreampGain[en_Filter450]) ,false ,
//			"Gain - 450 (0 - 15)" , std::to_string(g_LocalFilterPreampGain[en_Filter450]).c_str() , 2);
//	BeepBuzzer();
//}
void Handlerb505(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalFilterPreampGain[en_Filter505] , sizeof(g_LocalFilterPreampGain[en_Filter505]) ,false ,
			"Gain - 505 (0 - 15)" , std::to_string(g_LocalFilterPreampGain[en_Filter505]).c_str() , 2);
	BeepBuzzer();
}
void Handlerb546(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalFilterPreampGain[en_Filter546] , sizeof(g_LocalFilterPreampGain[en_Filter546]) ,false ,
			"Gain - 546 (0 - 15)" , std::to_string(g_LocalFilterPreampGain[en_Filter546]).c_str() , 2);
	BeepBuzzer();
}
void Handlerb578(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalFilterPreampGain[en_Filter578] , sizeof(g_LocalFilterPreampGain[en_Filter578]) ,false ,
			"Gain - 578 (0 - 15)" , std::to_string(g_LocalFilterPreampGain[en_Filter578]).c_str() , 2);
	BeepBuzzer();
}
void Handlerb630(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalFilterPreampGain[en_Filter630] , sizeof(g_LocalFilterPreampGain[en_Filter630]) ,false ,
			"Gain -630 (0 - 15)" , std::to_string(g_LocalFilterPreampGain[en_Filter630]).c_str() , 2);
	BeepBuzzer();
}
//void Handlerb670(void *ptr)
//{
//	openKeyBoard(en_NumericKeyboard , &g_LocalFilterPreampGain[en_Filter670] , sizeof(g_LocalFilterPreampGain[en_Filter670]) ,false ,
//			"Gain - 670 (0 - 15)" , std::to_string(g_LocalFilterPreampGain[en_Filter670]).c_str() , 2);
//	BeepBuzzer();
//}
void HandlerbBlack(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_LocalFilterPreampGain[en_FilterBlack] , sizeof(g_LocalFilterPreampGain[en_FilterBlack]) ,false ,
			"Gain Black (0 - 15)" , std::to_string(g_LocalFilterPreampGain[en_FilterBlack]).c_str() , 2);
	BeepBuzzer();
}
void HandlertAngleFilterToFilter(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_AngleFilterToFilter , sizeof(g_AngleFilterToFilter) ,false ,
			"Filter to Filter Angle" , std::to_string(g_AngleFilterToFilter).c_str() , 3);
	BeepBuzzer();
}
void HandlertAngleLightPathOffset(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_AngleLightpathOffset , sizeof(g_AngleLightpathOffset) ,false ,
			"Home to Filter Angle" , std::to_string(g_AngleLightpathOffset).c_str() , 3);
	BeepBuzzer();
}
void HandlertFilterRpm(void *ptr)
{
	openKeyBoard(en_NumericKeyboard , &g_FilterRpm , sizeof(g_FilterRpm) ,false ,
			"Filter Motor RPM (20 - 90)" , std::to_string(g_FilterRpm).c_str() , 3);
	BeepBuzzer();
}
