/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"

#define MIN_TEMP_OFFSET_VAL (-10)
#define MAX_TEMP_OFFSET_VAL (10)
#define MAX_DIAG_TEMP_SCAN_SEC (60)
#define MAX_DIAG_TEMP_VAL (50)
#define MIN_DIAG_TEMP_VAL (0)

#define DIAG_TEMP_GRAPH_PIXEL_MIN_X (206)
#define DIAG_TEMP_GRAPH_PIXEL_MIN_Y (168)
#define DIAG_TEMP_GRAPH_PIXEL_X_WIDTH (385)
#define DIAG_TEMP_GRAPH_PIXEL_Y_HEIGHT (248)
#define DIAG_TEMP_GRAPH_PIXEL_MAX_X (DIAG_TEMP_GRAPH_PIXEL_MIN_X + DIAG_TEMP_GRAPH_PIXEL_X_WIDTH)
#define DIAG_TEMP_GRAPH_PIXEL_MAX_Y (DIAG_TEMP_GRAPH_PIXEL_MIN_Y + DIAG_TEMP_GRAPH_PIXEL_Y_HEIGHT)
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_Diag_Temp , 2, "b5");
static NexButton bSave = NexButton(en_WinId_Diag_Temp , 5, "b0");

static NexButton bFlowCell = NexButton(en_WinId_Diag_Temp , 14, "b3");
static NexButton bIncubator = NexButton(en_WinId_Diag_Temp , 15, "b4");
static NexButton bMinus = NexButton(en_WinId_Diag_Temp , 12, "b1");
static NexButton bPlus = NexButton(en_WinId_Diag_Temp , 13, "b2");
static NexText tTempOffset = NexText(en_WinId_Diag_Temp , 11, "t5");
static NexText tTargetTemp = NexText(en_WinId_Diag_Temp , 10, "t4");
static NexText tActualTemp = NexText(en_WinId_Diag_Temp , 9, "t3");
static NexText textCalibGraphXMax = NexText(en_WinId_Diag_Temp , 20 , "t66");
static NexText textCalibGraphYMax = NexText(en_WinId_Diag_Temp , 17 , "t6");
static NexText textCalibGraphXMin = NexText(en_WinId_Diag_Temp , 19 , "t8");
static NexText textCalibGraphYMin = NexText(en_WinId_Diag_Temp , 18 , "t7");

static enTempControl g_TempMenu = en_ChamperTemp;
static uint16_t g_u16MaxTempScanCount = 0;
static float g_fPrevTempVal = 0;
/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bFlowCell,
									 &bIncubator,
									 &bMinus,
									 &bPlus,
									 &bSave,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbFlowcell(void *ptr);
static void HandlerbIncubator(void *ptr);
static void HandlerbMinus(void *ptr);
static void HandlerbPlus(void *ptr);
static void HandlerbSave(void *ptr);
static bool NonBlockMillsDelay(uint32_t u32DelayMs);
static void ResetDiagTempGraph(void);

enWindowStatus ShowDiagScreen_Temp (NexPage *ptr_obJCurrPage)
{
	char arr_CBuffer[64] = {0};
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bFlowCell.attachPush(HandlerbFlowcell, &bFlowCell);
	bIncubator.attachPush(HandlerbIncubator, &bIncubator);
	bMinus.attachPush(HandlerbMinus, &bMinus);
	bPlus.attachPush(HandlerbPlus, &bPlus);
	bSave.attachPush(HandlerbSave, &bSave);
//	ReadSettingsBuffer();
	g_TempMenu = en_PhotometerTemp;
	if(en_PhotometerTemp == g_TempMenu)
	{
		bFlowCell.Set_background_image_pic(356);
		bIncubator.Set_background_image_pic(358);
	}

	snprintf(arr_CBuffer , 63 , "%.02f" , objstcSettings.fTemperatureOffset[g_TempMenu]);
	tTempOffset.setText(arr_CBuffer);

	snprintf(arr_CBuffer , 63 , "%.1f" , objstcSettings.fTargetTemp[g_TempMenu]);
	tTargetTemp.setText(arr_CBuffer);

	if(GetCurrentTemperature(g_TempMenu) > 99 || GetCurrentTemperature(g_TempMenu) < 1)
	{
		tActualTemp.setText("--.-");
	}
	else
	{
		snprintf(arr_CBuffer , 63 , "%.1f" , GetCurrentTemperature(g_TempMenu));
		tActualTemp.setText(arr_CBuffer);
	}
	ResetDiagTempGraph();

	return WinStatus;
}

void HandlerDiagScreen_Temp (NexPage *ptr_obJCurrPage)
{
	float MaxXVal = MAX_DIAG_TEMP_SCAN_SEC;
	float MaxYVal = MAX_DIAG_TEMP_VAL;
	float MinXVal = 0;
	float MinYVal = MIN_DIAG_TEMP_VAL;
	float fCurrentTempVal = 0;

	nexLoop(nex_Listen_List);

	if((objstcSettings.fTempControlOnOff[en_ChamperTemp] == true && g_TempMenu == en_ChamperTemp) || g_TempMenu == en_PhotometerTemp)
	{
		if(true == NonBlockMillsDelay(1000))/*For each second*/
		{
			if(MAX_DIAG_TEMP_SCAN_SEC < g_u16MaxTempScanCount)/*if temp scan counter reaches max limit*/
			{
				/*Reset graph*/
				ResetDiagTempGraph();
			}
			else
			{
				char arr_CBuffer[64] = {0};
				fCurrentTempVal = GetCurrentTemperature(g_TempMenu);
				if(fCurrentTempVal > 99 || fCurrentTempVal < 1)
				{
					fCurrentTempVal = 0;
				}
				snprintf(arr_CBuffer , 63 , "%.1f" , fCurrentTempVal);
				tActualTemp.setText(arr_CBuffer);
				/*Update graph*/
				UpdateGraph(en_WinId_Diag_Temp , (g_u16MaxTempScanCount)/*X1*/,
						g_fPrevTempVal /*Y1*/,
						(g_u16MaxTempScanCount + 1)/*X2*/,
						fCurrentTempVal /*Y2*/,
						MinXVal/*X min value*/,
						MaxXVal/*X max value*/,
						MinYVal/*Y min value*/,
						MaxYVal/*X max value*/,
						DIAG_TEMP_GRAPH_PIXEL_MIN_X/*Graph X*/,
						DIAG_TEMP_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
						DIAG_TEMP_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
						DIAG_TEMP_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
						COLOUR_RED);

				g_u16MaxTempScanCount++;/*Increment graph scan counter*/
				g_fPrevTempVal = fCurrentTempVal;
			}
		}
	}
}

void HandlerbBack(void *ptr)
{
	ChangeWindowPage(en_WinId_DiagnosticsScreen , (enWindowID)NULL);
	BeepBuzzer();
}

void HandlerbFlowcell(void *ptr)
{
	char arr_CBuffer[64] = {0};
	g_TempMenu = en_PhotometerTemp;
	bFlowCell.Set_background_image_pic(356);
	bIncubator.Set_background_image_pic(358);
	snprintf(arr_CBuffer , 63 , "%.02f" , objstcSettings.fTemperatureOffset[g_TempMenu]);
	tTempOffset.setText(arr_CBuffer);
	snprintf(arr_CBuffer , 63 , "%.1f" , objstcSettings.fTargetTemp[g_TempMenu]);
	tTargetTemp.setText(arr_CBuffer);
	if(GetCurrentTemperature(g_TempMenu) > 99 || GetCurrentTemperature(g_TempMenu) < 1)
	{
		tActualTemp.setText("--.-");
	}
	else
	{
		snprintf(arr_CBuffer , 63 , "%.1f" , GetCurrentTemperature(g_TempMenu));
		tActualTemp.setText(arr_CBuffer);
	}
	ResetDiagTempGraph();
	BeepBuzzer();
}
void HandlerbIncubator(void *ptr)
{
	char arr_CBuffer[64] = {0};
	g_TempMenu = en_ChamperTemp;
	bFlowCell.Set_background_image_pic(359);
	bIncubator.Set_background_image_pic(357);
	snprintf(arr_CBuffer , 63 , "%.02f" , objstcSettings.fTemperatureOffset[g_TempMenu]);
	tTempOffset.setText(arr_CBuffer);

	if((objstcSettings.fTempControlOnOff[en_ChamperTemp] == true && g_TempMenu == en_ChamperTemp))
	{
		snprintf(arr_CBuffer , 63 , "%.1f" , objstcSettings.fTargetTemp[g_TempMenu]);
		tTargetTemp.setText(arr_CBuffer);

		if(GetCurrentTemperature(g_TempMenu) > 99 || GetCurrentTemperature(g_TempMenu) < 1)
		{
			tActualTemp.setText("--.-");
		}
		else
		{
			snprintf(arr_CBuffer , 63 , "%.1f" , GetCurrentTemperature(g_TempMenu));
			tActualTemp.setText(arr_CBuffer);
		}
	}
	else if(objstcSettings.fTempControlOnOff[en_ChamperTemp] == false && g_TempMenu == en_ChamperTemp)
	{
		snprintf(arr_CBuffer , 63 , "OFF");
		tTargetTemp.setText(arr_CBuffer);
		tActualTemp.setText(arr_CBuffer);
	}
	ResetDiagTempGraph();
	BeepBuzzer();
}
void HandlerbMinus(void *ptr)
{
	char arr_CBuffer[64] = {0};
	objstcSettings.fTemperatureOffset[g_TempMenu] -= (0.1);
	if(MIN_TEMP_OFFSET_VAL > objstcSettings.fTemperatureOffset[g_TempMenu])
	{
		objstcSettings.fTemperatureOffset[g_TempMenu] = MIN_TEMP_OFFSET_VAL;
	}
	snprintf(arr_CBuffer , 63 , "%.02f" , objstcSettings.fTemperatureOffset[g_TempMenu]);
	tTempOffset.setText(arr_CBuffer);
	snprintf(arr_CBuffer , 63 , "%.1f" , objstcSettings.fTargetTemp[g_TempMenu]);
	tTargetTemp.setText(arr_CBuffer);
	if(GetCurrentTemperature(g_TempMenu) > 99 || GetCurrentTemperature(g_TempMenu) < 1)
	{
		tActualTemp.setText("--.-");
	}
	else
	{
		snprintf(arr_CBuffer , 63 , "%.1f" , GetCurrentTemperature(g_TempMenu));
		tActualTemp.setText(arr_CBuffer);
	}
	BeepBuzzer();
}
void HandlerbPlus(void *ptr)
{
	char arr_CBuffer[64] = {0};
	objstcSettings.fTemperatureOffset[g_TempMenu] += (0.1);
	if(MAX_TEMP_OFFSET_VAL < objstcSettings.fTemperatureOffset[g_TempMenu])
	{
		objstcSettings.fTemperatureOffset[g_TempMenu] = MAX_TEMP_OFFSET_VAL;
	}
	snprintf(arr_CBuffer , 63 , "%.02f" , objstcSettings.fTemperatureOffset[g_TempMenu]);
	tTempOffset.setText(arr_CBuffer);
	snprintf(arr_CBuffer , 63 , "%.1f" , objstcSettings.fTargetTemp[g_TempMenu]);
	tTargetTemp.setText(arr_CBuffer);
	if(GetCurrentTemperature(g_TempMenu) > 99 || GetCurrentTemperature(g_TempMenu) < 1)
	{
		tActualTemp.setText("--.-");
	}
	else
	{
		snprintf(arr_CBuffer , 63 , "%.1f" , GetCurrentTemperature(g_TempMenu));
		tActualTemp.setText(arr_CBuffer);
	}
	BeepBuzzer();
}
void HandlerbSave(void *ptr)
{
	BeepBuzzer();
	/*save settings*/
	WriteSettingsBuffer();
	/*Save upcomming window id before show popup page*/
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enkeyOk == ShowMainPopUp("Temperature","Offset Values Saved" , true))
	{
		stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
		ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
	}
}
void ResetDiagTempGraph(void)
{
	float MaxXVal = MAX_DIAG_TEMP_SCAN_SEC;
	float MaxYVal = MAX_DIAG_TEMP_VAL;
	float MinXVal = 0;
	float MinYVal = MIN_DIAG_TEMP_VAL;

	g_u16MaxTempScanCount = 0;/*Reset temp scan counter*/
	g_fPrevTempVal = GetCurrentTemperature(g_TempMenu);
	if(g_fPrevTempVal > 99 || g_fPrevTempVal < 1)
	{
		g_fPrevTempVal = 0;
	}
//	MaxYVal = (g_fPrevTempVal + MAX_DIAG_TEMP_VAL);

	/*Clear and show standard graph*/
	ClearGraph(en_WinId_Diag_Temp ,
							DIAG_TEMP_GRAPH_PIXEL_MIN_X ,
							DIAG_TEMP_GRAPH_PIXEL_MIN_Y,
							DIAG_TEMP_GRAPH_PIXEL_X_WIDTH,
							DIAG_TEMP_GRAPH_PIXEL_Y_HEIGHT + 2);
	DrawGraphRectangle(en_WinId_Diag_Temp ,
								DIAG_TEMP_GRAPH_PIXEL_MIN_X ,
								DIAG_TEMP_GRAPH_PIXEL_MIN_Y,
								DIAG_TEMP_GRAPH_PIXEL_MAX_X,
								DIAG_TEMP_GRAPH_PIXEL_MAX_Y);

	UpdateGraphAxisData(&textCalibGraphXMax /*X Max*/, &textCalibGraphYMax /*Y Max*/,
			MaxXVal/*X max value*/,
			MaxYVal/*Y max value*/,
			&textCalibGraphXMin /*X Min*/, &textCalibGraphYMin /*Y Min*/,
			MinXVal/*X max value*/,
			MinYVal/*Y max value*/);

}
bool NonBlockMillsDelay(uint32_t u32DelayMs)
{
  unsigned long currentMillis = HAL_GetTick();
  static unsigned long PrevMills = 0;/*Initilized one time only*/
  int elapsedTime = (int)(currentMillis - PrevMills);
  if(abs((int)elapsedTime) >= u32DelayMs)
  {
   PrevMills = currentMillis;
   return true;
  }
  return false;
}
