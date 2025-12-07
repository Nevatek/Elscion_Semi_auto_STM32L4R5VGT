/*
 * WinMainMenu.cpp
 *
 *  Created on: Feb 24, 2023
 *      Author: Alvin
 */
#include "../../Screens/Inc/Screens.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "Appl_Timer.hpp"
#include "Printer.h"

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
	en_Memory_Checking,
	en_Flowcell_Temp_Check,
	en_Incubator_Temp_Check,
	en_Filter_Task_Completed,
}enStartup_DiagState;

static bool en_Memory_check = true;
static bool en_Lamp_check = true;
static bool en_Filter_sens_check = true;
static bool en_Optics_check = true;
static bool en_Flowcell_temp_check = true;
static bool en_Incubator_temp_check = true;


static enStartup_DiagState g_u8Startup_DiagState = en_Filter_Idle;
static stcTimer g_stcTimer;
static uint8_t g_u8SelectedFilter = en_FilterHome;
static uint16_t u16FilterAdc[en_FilterMax] = {0};
char arr_CBuffer[64] = {0};
/*Initialize all local buttons , sliders etc*/
/*(Format : page id = 0, component id = 1, component name = "b0")*/

static NexButton bSkip = NexButton(en_WinId_SelfTestScreen , 2, "b1");
static NexButton bAspirate = NexButton(en_WinId_SelfTestScreen , 3, "b2");
static NexButton bOkay = NexButton(en_WinId_SelfTestScreen , 1, "b0");

static NexText tAspirateText = NexText(en_WinId_SelfTestScreen , 5 , "t0");

static NexText tMemory_txt = NexText(en_WinId_SelfTestScreen , 6 , "t1");
static NexText tLamp_txt = NexText(en_WinId_SelfTestScreen , 7 , "t2");
static NexText tOpticalSetup_txt = NexText(en_WinId_SelfTestScreen , 8 , "t3");
static NexText tFilterWheelSens_txt = NexText(en_WinId_SelfTestScreen , 9 , "t4");
static NexText tFlowcellTemp_txt = NexText(en_WinId_SelfTestScreen , 10 , "t5");
static NexText tIncuTemp_txt = NexText(en_WinId_SelfTestScreen , 11 , "t6");

static NexText tMemory_Statustxt = NexText(en_WinId_SelfTestScreen , 12 , "t7");
static NexText tLamp_Statustxt = NexText(en_WinId_SelfTestScreen , 13 , "t8");
static NexText tOpticalSetup_Statustxt = NexText(en_WinId_SelfTestScreen , 14 , "t9");
static NexText tFilterWheelSens_Statustxt = NexText(en_WinId_SelfTestScreen , 15 , "t10");
static NexText tFlowcellTemp_Statustxt = NexText(en_WinId_SelfTestScreen , 16 , "t11");
static NexText tIncuTemp_Statustxt = NexText(en_WinId_SelfTestScreen , 17 , "t12");

/*This list condains all the local buttons to be monitored in realtime*/
static NexTouch *nex_Listen_List[] ={&bSkip,
									 &bAspirate,
									 &bOkay,
									 NULL};

static void HandlerbSkip(void *ptr);
static void HandlerbAspirate(void *ptr);
static void HandlerbOkay(void *ptr);
static void FilterWheel_Task(void);
static void StartFilterScan(void);
static enStartup_DiagState Get_Startup_DiagTaskState(void);

enWindowStatus ShowStartupDiagScreen (NexPage *ptr_obJCurrPage)
{
	enWindowStatus WinStatus = en_WindowShown;
	ptr_obJCurrPage->show();
	bSkip.attachPush(HandlerbSkip, &bSkip);
	bAspirate.attachPush(HandlerbAspirate, &bAspirate);
	bOkay.attachPush(HandlerbOkay, &bOkay);
	tAspirateText.setText("Aspirate DI Water");
	return WinStatus;
}

void HandlerStartupDiagScreen (NexPage *ptr_obJCurrPage)
{
	FilterWheel_Task();
	nexLoop(nex_Listen_List);
}

void HandlerbOkay(void *ptr)
{
	if(en_Filter_Idle == Get_Startup_DiagTaskState())
	{
		ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}

void HandlerbSkip(void *ptr)
{
	if(en_Filter_Idle == Get_Startup_DiagTaskState())
	{
		ChangeWindowPage(en_WinID_MainMenuScreen , (enWindowID)NULL);
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}

void HandlerStartupDiagAspButton(void)
{
	if(en_Filter_Idle == Get_Startup_DiagTaskState())
	{
		bSkip.setVisible(false);
		tAspirateText.setText("Aspirating DI Water");
		bAspirate.Set_background_image_pic(355);
		g_u8Startup_DiagState = en_Filter_StartScan;
		BeepBuzzer();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}

void HandlerbAspirate(void *ptr)
{
	HandlerStartupDiagAspButton();
}

enStartup_DiagState Get_Startup_DiagTaskState(void)
{
	return g_u8Startup_DiagState;
}

void FilterWheel_Task(void)
{
	switch(g_u8Startup_DiagState)
	{
		case en_Filter_Idle:
		break;
		case en_Filter_StartScan:
		{
			g_u8Startup_DiagState = en_Filter_Asp_500Ul;
		}
		break;
		case en_Filter_Asp_500Ul:
		{
			Aspirate_Pump(500);//Ul
			g_u8Startup_DiagState = en_Filter_Asp_500Ul_Busy;
		}break;
		case en_Filter_Asp_500Ul_Busy:
		{
			 if(enPump_AspCompleted == Pump_RunTask())
			 {
				 LongBeepBuzzer();
				 g_u8Startup_DiagState = en_Filter_Asp_500Ul_Post_Delay;
				 StartTimer(&g_stcTimer , 2 * 1000);/*Homming max delay*/
			 }
		}break;
		case en_Filter_Asp_500Ul_Post_Delay:
		{
			if(true == Timer_IsTimeout(&g_stcTimer , enTimerNormalStop))
			{
				 g_u8Startup_DiagState = en_Filter_Asp_AirGap;
			}
		}break;
		case en_Filter_Asp_AirGap:
		{
			Aspirate_Pump(objstcSettings.fAirGapVol_uL);//Ul
			g_u8Startup_DiagState = en_Filter_Asp_AirGap_Busy;
		}break;
		case en_Filter_Asp_AirGap_Busy:
		{
			 if(enPump_AspCompleted == Pump_RunTask())
			 {
				 g_u8Startup_DiagState = en_Filter_StartScan_Homming;
				 tAspirateText.setText("Running System Diagnostics");
			 }
		}break;
		case en_Filter_StartScan_Homming:
		{
			 g_u8SelectedFilter = en_FilterHome;
			 SelectFilter((en_WavelengthFilter)g_u8SelectedFilter);
			 StartTimer(&g_stcTimer , 2 * 1000);/*Homming max delay*/
			 g_u8Startup_DiagState = en_Filter_StartScan_Homming_Busy;
		}break;
		case en_Filter_StartScan_Homming_Busy:
		{
			if(true == Timer_IsTimeout(&g_stcTimer , enTimerNormalStop))
			{
				if(en_FilterMotorBusy != ReadFilterMotorStatus())
				{
					if(en_FilterMotorHomeError == ReadFilterMotorStatus())
					{
						en_Filter_sens_check = false;
						en_Optics_check = false;
						en_Lamp_check = false;
						g_u8Startup_DiagState = en_Memory_Checking;
					}
					else
					{
						g_u8Startup_DiagState = en_Filter_State_SelectingFilter;
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

			g_u8Startup_DiagState = en_Filter_Idle;/*Stop state machine*/
		}
		break;
		case en_Filter_State_SelectingFilter:
		{
			if(MAX_NUM_FILTERS_DISPLAYED > g_u8SelectedFilter)
			{
				g_u8SelectedFilter++;
				g_u8Startup_DiagState = en_Filter_State_SelectingFilter_Busy;
			}
			else
			{
				g_u8SelectedFilter = en_FilterHome;
				g_u8Startup_DiagState = en_Memory_Checking;/*Stop state machine*/
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
					g_u8Startup_DiagState = en_Filter_Reading_Adc;
				}
			}
		break;
		case en_Filter_Reading_Adc:
		{
			u16FilterAdc[g_u8SelectedFilter - 1] = ReadPhotometryMeasurement(AssignGain((en_WavelengthFilter)g_u8SelectedFilter) , false);
			if(u16FilterAdc[g_u8SelectedFilter - 1] > (g_Default_ADC_Ranges_max[g_u8SelectedFilter]) ||
					u16FilterAdc[g_u8SelectedFilter - 1] < (g_Default_ADC_Ranges_min[g_u8SelectedFilter]))
			{
				en_Optics_check = false;
			}
			if(u16FilterAdc[g_u8SelectedFilter - 1] < 150)
			{
				en_Lamp_check = false;
			}
			g_u8Startup_DiagState = en_Filter_State_SelectingFilter;
		}
		break;

		case en_Memory_Checking:
		{
			if((uint32_t)NVM_INIT_OK_FLAG != (uint32_t)objstcSettings.InitFlag1)/*If this flag is not set , reset whole nvm flash*/
			{
				en_Memory_check = false;
			}
			g_u8Startup_DiagState = en_Flowcell_Temp_Check;
		}
		break;
		case en_Flowcell_Temp_Check:
		{
			if(GetCurrentTemperature(en_PhotometerTemp) > 45 || GetCurrentTemperature(en_PhotometerTemp) < 20)
			{
				en_Flowcell_temp_check = false;
			}
			g_u8Startup_DiagState = en_Incubator_Temp_Check;
		}
		break;
		case en_Incubator_Temp_Check:
		{
			if(GetCurrentTemperature(en_ChamperTemp) > 45 || GetCurrentTemperature(en_ChamperTemp) < 20)
			{
				en_Incubator_temp_check = false;
			}
			g_u8Startup_DiagState = en_Filter_Task_Completed;
		}
		break;
		case en_Filter_Task_Completed:
		{

			/*Printing headers*/
			Print_Header();
			/*Showing print name*/

			snprintf(arr_CBuffer , 63 , "%s" , "********************************");
			PrintStringOnPrinter(&arr_CBuffer[0]);
			PrinterFeedLine(1);

			SendBoldOnOffCMD(true);
			snprintf(arr_CBuffer , 63 , "%s" , "System Diagnostics Report");
			PrintStringOnPrinter(&arr_CBuffer[0]);
			PrinterFeedLine(1);
			SendBoldOnOffCMD(false);

			tAspirateText.setVisible(false);
			bAspirate.setVisible(false);
			bOkay.setVisible(true);

			if(en_Memory_check == true)
			{
				tMemory_txt.setText("Memory");
				tMemory_Statustxt.setText(": Ok");

				snprintf(arr_CBuffer , 63 , "Memory                 : Ok");
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}
			else
			{
				tMemory_txt.setText("Memory");
				tMemory_Statustxt.setText(": Not Ok");

				snprintf(arr_CBuffer , 63 , "Memory                 : Not ok");
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}
			if(en_Lamp_check == true)
			{
				tLamp_txt.setText("Lamp");
				tLamp_Statustxt.setText(": Ok");

				snprintf(arr_CBuffer , 63 , "Lamp                   : Ok");
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}
			else
			{
				tLamp_txt.setText("Lamp");
				tLamp_Statustxt.setText(": Not Ok");

				snprintf(arr_CBuffer , 63 , "Lamp                   : Not ok");
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}

			if(en_Optics_check == true)
			{
				tOpticalSetup_txt.setText("Optical setup");
				tOpticalSetup_Statustxt.setText(": Ok");

				snprintf(arr_CBuffer , 63 , "Optical setup          : Ok");
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}
			else
			{
				tOpticalSetup_txt.setText("Optical setup");
				tOpticalSetup_Statustxt.setText(": Not Ok");

				snprintf(arr_CBuffer , 63 , "Optical setup          : Not ok");
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}

			if(en_Filter_sens_check == true)
			{
				tFilterWheelSens_txt.setText("Filter wheel sensor");
				tFilterWheelSens_Statustxt.setText(": Ok");

				snprintf(arr_CBuffer , 63 , "Filter wheel sensor    : Ok");
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}
			else
			{
				tFilterWheelSens_txt.setText("Filter wheel sensor");
				tFilterWheelSens_Statustxt.setText(": Not Ok");

				snprintf(arr_CBuffer , 63 , "Filter wheel sensor    : Not ok");
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}
			if(en_Flowcell_temp_check == true)
			{
				tFlowcellTemp_txt.setText("Flow cell temp. sensor");
				tFlowcellTemp_Statustxt.setText(": Ok");

				snprintf(arr_CBuffer , 63 , "Flow cell temp. sensor : Ok");
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}
			else
			{
				tFlowcellTemp_txt.setText("Flow cell temp. sensor");
				tFlowcellTemp_Statustxt.setText(": Not Ok");

				snprintf(arr_CBuffer , 63 , "Flow cell temp. sensor : Not ok");
				PrintStringOnPrinter(&arr_CBuffer[0]);
			}

			if(objstcSettings.fTempControlOnOff[en_ChamperTemp] == true)
			{
				if(en_Incubator_temp_check == true)
				{
					tIncuTemp_txt.setText("Incubator temp. sensor");
					tIncuTemp_Statustxt.setText(": Ok");

					snprintf(arr_CBuffer , 63 , "Incubator temp. sensor : Ok");
					PrintStringOnPrinter(&arr_CBuffer[0]);
				}
				else
				{
					tIncuTemp_txt.setText("Incubator temp. sensor");
					tIncuTemp_Statustxt.setText(": Not Ok");

					snprintf(arr_CBuffer , 63 , "Incubator temp. sensor : Not ok");
					PrintStringOnPrinter(&arr_CBuffer[0]);

				}
			}
			PrinterFeedLine(1);
			snprintf(arr_CBuffer , 63 , "%s" , "********************************");
			PrintStringOnPrinter(&arr_CBuffer[0]);
			Print_Footer();
			PrinterFeedLine(4);
			g_u8Startup_DiagState = en_Filter_Idle;
		}
		break;
	}
}
