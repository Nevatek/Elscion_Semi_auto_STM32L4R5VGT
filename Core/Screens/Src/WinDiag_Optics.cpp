/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "Appl_Timer.hpp"
#include <HandlerPheripherals.hpp>

#define MAX_NUM_FILTERS_DISPLAYED (6)
typedef enum
{
	en_Filter_Idle = 0,
	en_Filter_StartScan,
	en_Filter_Asp_500Ul,
	en_Filter_Asp_500Ul_Busy,
	en_Filter_Asp_500Ul_Post_Delay,
	en_Filter_Asp_AirGap,
	en_Filter_Asp_AirGap_Busy,
	en_Filter_StartScan_Homming,
	en_Filter_StartScan_Homming_Busy,
	en_Filter_State_SelectingFilter ,
	en_Filter_Home_Error,
	en_Filter_State_SelectingFilter_Busy,
	en_Filter_Reading_Adc,
	en_Filter_AutoEmptying,
	enFilter_Task_Completed,
}enFilterState;
static enFilterState g_u8FilterState = en_Filter_Idle;
static stcTimer g_stcTimer;
static uint8_t g_u8SelectedFilter = en_FilterHome;
static uint16_t u16FilterAdc[en_FilterMax] = {0};
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bBack = NexButton(en_WinId_Diag_Optical , 1, "b5");
static NexButton bAspirate = NexButton(en_WinId_Diag_Optical , 30, "b1");
static NexButton bDelete = NexButton(en_WinId_Diag_Optical , 31, "b2");
static NexButton bSave = NexButton(en_WinId_Diag_Optical , 4, "b0");
static NexText tAspirateText = NexText(en_WinId_Diag_Optical , 35 , "t27");


static NexText tAdc[MAX_NUM_FILTERS_DISPLAYED] = {	NexText(en_WinId_Diag_Optical , 22 , "t16"),
							NexText(en_WinId_Diag_Optical , 21 , "t15"),
							NexText(en_WinId_Diag_Optical , 20 , "t14"),
							NexText(en_WinId_Diag_Optical , 19 , "t13"),
							NexText(en_WinId_Diag_Optical , 18 , "t12"),
							NexText(en_WinId_Diag_Optical , 17 , "t11"),
//							NexText(en_WinId_Diag_Optical , 16 , "t10"),
//							NexText(en_WinId_Diag_Optical , 33 , "t25")
};

static NexText tNewAdc[MAX_NUM_FILTERS_DISPLAYED] = {	NexText(en_WinId_Diag_Optical , 29 , "t23"),
								NexText(en_WinId_Diag_Optical , 28 , "t22"),
								NexText(en_WinId_Diag_Optical , 27 , "t21"),
								NexText(en_WinId_Diag_Optical , 26 , "t20"),
								NexText(en_WinId_Diag_Optical , 25 , "t19"),
								NexText(en_WinId_Diag_Optical , 24 , "t18"),
//								NexText(en_WinId_Diag_Optical , 23 , "t17"),
//								NexText(en_WinId_Diag_Optical , 34 , "t26")
};

static NexText tWavelength[MAX_NUM_FILTERS_DISPLAYED] = {	NexText(en_WinId_Diag_Optical , 9 , "t3"),
									NexText(en_WinId_Diag_Optical , 10 , "t4"),
									NexText(en_WinId_Diag_Optical , 11 , "t5"),
									NexText(en_WinId_Diag_Optical , 12 , "t6"),
									NexText(en_WinId_Diag_Optical , 13 , "t7"),
									NexText(en_WinId_Diag_Optical , 14 , "t8"),
//									NexText(en_WinId_Diag_Optical , 15 , "t9"),
//									NexText(en_WinId_Diag_Optical , 32 , "t24")
};

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bBack,
									 &bAspirate,
									 &bDelete,
									 &bSave,
									 NULL};

static void HandlerbBack(void *ptr);
static void HandlerbAspirate(void *ptr);
static void HandlerbDelete(void *ptr);
static void HandlerbSave(void *ptr);
static void FilterWheel_Task(void);
void StartFilterScan(void);
static enFilterState Get_FilterTaskState(void);

enWindowStatus ShowDiagScreen_Optics (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bBack.attachPush(HandlerbBack, &bBack);
	bAspirate.attachPush(HandlerbAspirate, &bAspirate);
	bDelete.attachPush(HandlerbDelete, &bDelete);
	bSave.attachPush(HandlerbSave, &bSave);

	AspSwitchLed_Red(en_AspLedOFF);
	AspSwitchLed_White(en_AspLedON);

	for(g_u8SelectedFilter = en_Filter340 ; g_u8SelectedFilter <= MAX_NUM_FILTERS_DISPLAYED ; ++g_u8SelectedFilter)
	{
		tWavelength[g_u8SelectedFilter - 1].setText(&g_arrFilterNames_OpticsDiag[g_u8SelectedFilter][0]);
		char arrBuffAdc[64] = {0};
		snprintf(arrBuffAdc , 64 - 1 , "%u", objstcSettings.u16SavedFilterAdc[g_u8SelectedFilter - 1]);

		if(objstcSettings.u16SavedFilterAdc[g_u8SelectedFilter - 1] > (g_Default_ADC_Ranges_max[g_u8SelectedFilter]) ||
				objstcSettings.u16SavedFilterAdc[g_u8SelectedFilter - 1] < (g_Default_ADC_Ranges_min[g_u8SelectedFilter]))
		{
			tAdc[g_u8SelectedFilter - 1].Set_font_color_pco(COLOUR_RED);
		}
		else
		{
			tAdc[g_u8SelectedFilter - 1].Set_font_color_pco(COLOUR_BLACK);
		}
		tAdc[g_u8SelectedFilter - 1].setText(arrBuffAdc);
	}
	tAspirateText.setText("Aspirate DI Water");
	return WinStatus;
}

void HandlerDiagScreen_Optics (NexPage *ptr_obJCurrPage)
{
	FilterWheel_Task();
	nexLoop(nex_Listen_List);
}

void HandlerbBack(void *ptr)
{
	if(en_Filter_Idle == Get_FilterTaskState())
	{
		ChangeWindowPage(en_WinId_DiagnosticsScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}

void HandlerDiagOpticalAspButton(void)
{
	if(en_Filter_Idle == Get_FilterTaskState())
	{
		AspSwitchLed_Red(en_AspLedON);
		AspSwitchLed_White(en_AspLedOFF);

		tAspirateText.setText("Aspirating DI Water");
		bAspirate.Set_background_image_pic(355);
		StartFilterScan();
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}

void HandlerbAspirate(void *ptr)
{
	HandlerDiagOpticalAspButton();
}
void HandlerbDelete(void *ptr)
{
	if(en_Filter_Idle != Get_FilterTaskState())
	{
		InstrumentBusyBuzz();
		return;
	}
	BeepBuzzer();
	for(g_u8SelectedFilter = en_Filter340 ; g_u8SelectedFilter <= MAX_NUM_FILTERS_DISPLAYED ; ++g_u8SelectedFilter)
	{
		objstcSettings.u16SavedFilterAdc[g_u8SelectedFilter - 1] = u16FilterAdc[g_u8SelectedFilter - 1] = 0;
		char arrBuffAdc[64] = {0};
		snprintf(arrBuffAdc , 64 - 1 , "%u", objstcSettings.u16SavedFilterAdc[g_u8SelectedFilter - 1]);
		tAdc[g_u8SelectedFilter - 1].setText(arrBuffAdc);
	}
	/*save settings*/
	WriteSettingsBuffer();
	/*Save upcomming window id before show popup page*/
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enkeyOk == ShowMainPopUp("Optical","ADC Values Deleted" , true))
	{
		stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
		ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
	}
}
void HandlerbSave(void *ptr)
{
	if(en_Filter_Idle != Get_FilterTaskState())
	{
		InstrumentBusyBuzz();
		return;
	}
	BeepBuzzer();
	for(g_u8SelectedFilter = en_Filter340 ; g_u8SelectedFilter <= MAX_NUM_FILTERS_DISPLAYED ; ++g_u8SelectedFilter)
	{
		objstcSettings.u16SavedFilterAdc[g_u8SelectedFilter - 1] = u16FilterAdc[g_u8SelectedFilter - 1];
		char arrBuffAdc[64] = {0};
		snprintf(arrBuffAdc , 64 - 1 , "%u", objstcSettings.u16SavedFilterAdc[g_u8SelectedFilter - 1]);

		if(objstcSettings.u16SavedFilterAdc[g_u8SelectedFilter - 1] > (g_Default_ADC_Ranges_max[g_u8SelectedFilter]) ||
				objstcSettings.u16SavedFilterAdc[g_u8SelectedFilter - 1] < (g_Default_ADC_Ranges_min[g_u8SelectedFilter]))
		{
			tAdc[g_u8SelectedFilter - 1].Set_font_color_pco(COLOUR_RED);
		}
		else
		{
			tAdc[g_u8SelectedFilter - 1].Set_font_color_pco(COLOUR_BLACK);
		}
		tAdc[g_u8SelectedFilter - 1].setText(arrBuffAdc);
	}

	/*save settings*/
	WriteSettingsBuffer();
	/*Save upcomming window id before show popup page*/
	enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
	if(enkeyOk == ShowMainPopUp("Optical","ADC Values Saved" , true))
	{
		stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
		ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
	}
}
void StartFilterScan(void)
{
	for(uint8_t nIdx = en_Filter340 ; nIdx <= MAX_NUM_FILTERS_DISPLAYED ; ++nIdx)
	{
		tNewAdc[nIdx - 1].setText(" ");/*Clear buffer*/
	}
	g_u8FilterState = en_Filter_StartScan;
}

enFilterState Get_FilterTaskState(void)
{
	return g_u8FilterState;
}

void FilterWheel_Task(void)
{
	switch(g_u8FilterState)
	{
		case en_Filter_Idle:
		break;
		case en_Filter_StartScan:
		{
			g_u8FilterState = en_Filter_Asp_500Ul;
		}
		break;
		case en_Filter_Asp_500Ul:
		{
			Aspirate_Pump(500);//Ul
			g_u8FilterState = en_Filter_Asp_500Ul_Busy;
		}break;
		case en_Filter_Asp_500Ul_Busy:
		{
			 if(enPump_AspCompleted == Pump_RunTask())
			 {
				 LongBeepBuzzer();
				 g_u8FilterState = en_Filter_Asp_500Ul_Post_Delay;
				 StartTimer(&g_stcTimer , 2 * 1000);/*Homming max delay*/
			 }
		}break;
		case en_Filter_Asp_500Ul_Post_Delay:
		{
			if(true == Timer_IsTimeout(&g_stcTimer , enTimerNormalStop))
			{
				 g_u8FilterState = en_Filter_Asp_AirGap;
			}
		}break;
		case en_Filter_Asp_AirGap:
		{
			Aspirate_Pump(objstcSettings.fAirGapVol_uL);//Ul
			g_u8FilterState = en_Filter_Asp_AirGap_Busy;
		}break;
		case en_Filter_Asp_AirGap_Busy:
		{
			 if(enPump_AspCompleted == Pump_RunTask())
			 {
				 g_u8FilterState = en_Filter_StartScan_Homming;
				 tAspirateText.setText("Measuring");
			 }
		}break;
		case en_Filter_StartScan_Homming:
		{
			 g_u8SelectedFilter = en_FilterHome;
			 SelectFilter((en_WavelengthFilter)g_u8SelectedFilter);
			 StartTimer(&g_stcTimer , 2 * 1000);/*Homming max delay*/
			 g_u8FilterState = en_Filter_StartScan_Homming_Busy;
		}break;
		case en_Filter_StartScan_Homming_Busy:
		{
			if(true == Timer_IsTimeout(&g_stcTimer , enTimerNormalStop))
			{
				if(en_FilterMotorBusy != ReadFilterMotorStatus())
				{
					if(en_FilterMotorHomeError == ReadFilterMotorStatus())
					{
						g_u8FilterState = en_Filter_Home_Error;
					}
					else
					{
						g_u8FilterState = en_Filter_State_SelectingFilter;
					}
				}
				else
				{
					 StartTimer(&g_stcTimer , 1 * 1000);/*Homming max delay*/
				}
			}
		}
		break;
		case en_Filter_Home_Error:
		{
			/*Save upcomming window id before show popup page*/
			enWindowID NextWindow = stcScreenNavigation.CurrentWindowId;
			if(enkeyOk == ShowMainPopUp("Filter Wheel","Home Position Error" , true))
			{
				stcScreenNavigation.CurrentWindowId = en_WinId_MainPopup;
				ChangeWindowPage((enWindowID)NextWindow , (enWindowID)NextWindow);
			}
			g_u8FilterState = en_Filter_Idle;/*Stop state machine*/
		}
		break;
		case en_Filter_State_SelectingFilter:
		{
			if(MAX_NUM_FILTERS_DISPLAYED > g_u8SelectedFilter)
			{
				g_u8SelectedFilter++;
				g_u8FilterState = en_Filter_State_SelectingFilter_Busy;
			}
			else
			{
				g_u8SelectedFilter = en_FilterHome;
				g_u8FilterState = en_Filter_AutoEmptying;/*Stop state machine*/
			}
			SelectFilter((en_WavelengthFilter)g_u8SelectedFilter);
			StartTimer(&g_stcTimer , 1 * 1000);/*Homming max delay*/
		}
		break;
		case en_Filter_State_SelectingFilter_Busy:
			if(true == Timer_IsTimeout(&g_stcTimer , enTimerNormalStop))
			{
				if(en_FilterMotorBusy != ReadFilterMotorStatus())
				{
					g_u8FilterState = en_Filter_Reading_Adc;
				}
			}
		break;
		case en_Filter_Reading_Adc:
		{
			u16FilterAdc[g_u8SelectedFilter - 1] = ReadPhotometryMeasurement(AssignGain((en_WavelengthFilter)g_u8SelectedFilter) , false);
			char arrBuffAdc[64] = {0};
			snprintf(arrBuffAdc , 64 - 1 , "%u", u16FilterAdc[g_u8SelectedFilter - 1]);

			if(u16FilterAdc[g_u8SelectedFilter - 1] > (g_Default_ADC_Ranges_max[g_u8SelectedFilter]) ||
					u16FilterAdc[g_u8SelectedFilter - 1] < (g_Default_ADC_Ranges_min[g_u8SelectedFilter]))
			{
				tNewAdc[g_u8SelectedFilter - 1].Set_font_color_pco(COLOUR_RED);
			}
			else
			{
				tNewAdc[g_u8SelectedFilter - 1].Set_font_color_pco(COLOUR_BLACK);
			}
			tNewAdc[g_u8SelectedFilter - 1].setText(arrBuffAdc);

			g_u8FilterState = en_Filter_State_SelectingFilter;
		}
		break;
		case en_Filter_AutoEmptying:
		{
			if(true == Timer_IsTimeout(&g_stcTimer , enTimerNormalStop))
			{
				if(en_FilterMotorBusy != ReadFilterMotorStatus())
				{
					Aspirate_Pump(500);//Ul
					g_u8FilterState = enFilter_Task_Completed;
				}
			}
		}
		break;
		case enFilter_Task_Completed:
		{
			 if(enPump_AspCompleted == Pump_RunTask())
			 {
				bAspirate.Set_background_image_pic(354);
				tAspirateText.setText("Aspirate DI Water");
				g_u8FilterState = en_Filter_Idle;

				AspSwitchLed_Red(en_AspLedOFF);
				AspSwitchLed_White(en_AspLedON);
			 }
		}
		break;
	}
}
