/*
 * Testrun.c
 *
 *  Created on: 09-Sep-2022
 *      Author: SPX
 */
#include "main.h"
#include "cmsis_os.h"
#include "../APPL/Inc/Testrun.h"
#include "../APPL/Inc/ApplUsbDevice.h"
#include "../Screens/Inc/Screens.h"
#include "./../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/absorbance.h"
#include "../APPL/Inc/calibration.h"
#include <HandlerPheripherals.hpp>
#include "DcMotor.hpp"
#include "FilterMotor.h"
#include "Printer.h"
#include "USBDevice.h"
#include "math.h"
#include <string>
#include "Appl_Timer.hpp"
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "../../APPL/Inc/NVM_TestResultsHistory.hpp"
#include "../APPL/Inc/NVM_QCSetup.hpp"

uint16_t e_NumOfTestParamDataAvailable = 0;
static en_WashProcess g_WashProcessState = en_WashProcess_Idle;
static stcTimer g_airGapTimer;
//static stcTimer g_WashProcessTimer;
bool e_limit_error_popup = (false);
bool e_DI_Water_limit_flag = (false);
bool e_reagent_blank_limit_flag = (false);
bool e_abs_limit_flag = (false);
bool e_delAmin_limit_flag = (false);
bool e_QCTestDoneflag = (false);
bool e_StdDoneflag = (false);
uint8_t g_u8IsNewResult = false;
extern bool Manual_Wash;
bool e_ConcOutofLimit_popup = (false);
bool e_linearity_limit_flag = false;
bool WashBtwSample = false;
extern bool HyperActiveSampleflag;
extern bool extrapolated_ADC;
extern uint16_t LastTestId;
//extern bool GraphlineShift;

extern ScreenNavigation stcScreenNavigation;
extern enPreampGain g_enCurrentPreampgain;
extern NexText textMainGraphXMax;/*extern object of graph X max value*/
extern NexText textMainGraphYMax;/*extern object of graph Y max value*/
extern NexText textMainGraphXMin;/*extern object of graph X min value*/
extern NexText textMainGraphYMin;/*extern object of graph Y min value*/
extern float e_RunTimeStdOd[MAX_NUM_OF_STANDARDS];/*Buffer to store run time OD for each standard*/
extern float e_RunTimeStdBlankOd[MAX_NUM_OF_STANDARDS];/*Buffer to store run time OD for each standard blank*/
extern float e_Runtime4plA;
extern float e_Runtime4plB;
extern float e_Runtime4plC;
extern float e_Runtime4plD;
extern en_TestPopUp g_en_TestPopUp;
static uint8_t DelayTime = 0;
TestData stcTestData = {0};

std::string Limit_Err_Msg = "";
std::string ConcOutofLimit_Err_Msg = "";
//static float g_Temperaturebuffer[en_TempDegreeMax] = {25.0 , 30.0 , 37.0};
static bool TestRunMillsDelay(uint32_t Delay);
extern void UpdateTestStatusTextBuffer(std::string str);

bool TestRunMillsDelay(uint32_t Delay)
{
  unsigned long currentMillis = HAL_GetTick();
  static unsigned long PrevMills = 0;/*Initilized one time only*/
  int elapsedTime = (int)(currentMillis - PrevMills);
  if(abs((int)elapsedTime) >= Delay)
  {
   PrevMills = currentMillis;
   return true;
  }
  return false;
}
void InitilizetestPheripherals(void)
{
	stcTestData.CurrentTestStatus = enTestStatus_MoveFilterHome;
	/*Setting temperature*/
	objstcTempControl[en_PhotometerTemp].TargetTemp = GetInstance_TestParams()[stcTestData.TestId].Temperature;
}
void HandlerAspSwitchPressed(void)
{
	 if(en_WinID_MainMenuScreen == stcScreenNavigation.CurrentWindowId)
	 {
		 Start_Wash();
	 }
	 else if(en_WinId_TestRun == stcScreenNavigation.CurrentWindowId)
	 {
		 StartTestExecution();
	 }
}

uint32_t GetCurrentgainDarkADCVal(void)
{
  uint32_t DarkADCVal = stcTestData.fPreampDarkADCVal[g_enCurrentPreampgain];
  return DarkADCVal;
}
void AbortTestRun(void)
{
	stcTestData.Result = (float)0;
	DelayTime = 0;
	stcTestData.SampleOpticalDensity = (float)0;
//	GraphlineShift = false;
	/*Loop to reset primary wavelength data buffer*/
	for(uint16_t nI = 0 ; nI < MAX_ADC_BUFFER_LENGTH_PRIMARY ; ++nI)
	{
		stcTestData.PrimaryAdcBuffer[nI] = 0;
		stcTestData.PrimaryAbsBuffer[nI] = (float)0;
	}
	/*Loop to reset secondary wavelength data buffer*/
	for(uint16_t nI = 0 ; nI < MAX_ADC_BUFFER_LENGTH_DEFAULT ; ++nI)
	{
		stcTestData.SecondaryAdcBuffer[nI] = 0;
		stcTestData.SecondaryAbsBuffer[nI] = (float)0;
	}

	stcTestData.CurrentTestStatus = enTestStatus_TestIdle;
	if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
	{
		UpdateTestStatusTextBuffer(COAG_ABORT);
	}
	else{
		UpdateTestStatusTextBuffer(ABORT);
		Start_Wash();
	}

//	AspSwitchLed_Red(en_AspLedOFF);
//	AspSwitchLed_White(en_AspLedON);
	LongBeepBuzzer();

//	ClearGraph(en_WinId_TestRun /*Window ID*/,
//							TEST_GRAPH_PIXEL_MIN_X ,
//							TEST_GRAPH_PIXEL_MIN_Y,
//							TEST_GRAPH_PIXEL_X_WIDTH,
//							TEST_GRAPH_PIXEL_Y_HEIGHT);

	UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
	UpdateTestScreenMenu();
}
en_TestRunStatus ReadTestRunStatus(void)
{
	return stcTestData.CurrentTestStatus;
}

void Abort_AutoWash(void)
{
	g_WashProcessState = en_WashProcess_Idle;
}

en_WashProcess Get_WashStatus(void)
{
	return g_WashProcessState;
}

void Start_Wash(void)
{
	if(en_WashProcess_Idle == g_WashProcessState)
	{
		g_WashProcessState = en_WashProcess_2;
	}
}
void WashProcedure_Task(void)
{
	switch (g_WashProcessState)
	{
		case en_WashProcess_Idle:
		{

		}break;
		case en_WashProcess_1:
		{
			 ThreeWayValeCntrl(en_ValvePosProbe);
			 Aspirate_Pump(250);//Ul
			 g_WashProcessState = en_WashProcess_1_Busy;
		}break;
		case en_WashProcess_1_Busy:
		{
			 if(enPump_AspCompleted == Pump_RunTask())
			 {
				 g_WashProcessState = en_WashProcess_2;
			 }
		}break;
		case en_WashProcess_2:
		{
			 ThreeWayValeCntrl(en_ValvePosWaterTank);
			 if(stcTestData.CurrentTestStatus == enTestStatus_Wash_Busy)
			 {
				 Aspirate_Pump(objstcSettings.fWashVoleume_uL / 2);//auto empty
			 }
			 else
			 {
				 Aspirate_Pump(objstcSettings.fWashVoleume_uL);// Manual wash
			 }
			 g_WashProcessState = en_WashProcess_2_Busy;
		}break;
		case en_WashProcess_2_Busy:
		{
			 if(enPump_AspCompleted == Pump_RunTask())
			 {
				 LongBeepBuzzer();
				 if(WashBtwSample)
				 {
					 g_WashProcessState = en_WashProcess_BetweenSampleCompleted;
					 WashBtwSample = false;
				 }
				 else
				 {
					if(Manual_Wash)
					{
						StartTimer(&g_airGapTimer , 2 * 1000);/*Homming max delay*/
						g_WashProcessState = en_WashProcess_AspPreDelay;
					}
					else
					{
						 g_WashProcessState = en_WashProcess_Completed;
					}
				 }
			 }
		}break;
		case en_WashProcess_3:
		{
			 ThreeWayValeCntrl(en_ValvePosProbe);
			 Aspirate_Pump(250);//Ul
			 g_WashProcessState = en_WashProcess_3_Busy;
		}break;
		case en_WashProcess_3_Busy:
		{
			 if(enPump_AspCompleted == Pump_RunTask())
			 {
				 g_WashProcessState = en_WashProcess_Completed;
			 }
		}break;
		case en_WashProcess_AspPreDelay:
		{
			if(true == Timer_IsTimeout(&g_airGapTimer , enTimerNormalStop))
			{
				g_WashProcessState = en_WashProcess_AspAirGap;
			}
		}break;
		case en_WashProcess_AspAirGap:
		{
			Aspirate_Pump(objstcSettings.fWashVoleume_uL / 2);//Aspiration of Air gap //added
			g_WashProcessState = en_WashProcess_AspAirGapdelay;
		}break;
		case en_WashProcess_AspAirGapdelay:
		{
			if(enPump_AspCompleted == Pump_RunTask())
			{
				g_WashProcessState = en_WashProcess_Completed;
			}
		}break;

		case en_WashProcess_BetweenSampleCompleted:
		{
			g_WashProcessState = en_WashProcess_Idle;
		}break;

		case en_WashProcess_Completed:
		{
			g_WashProcessState = en_WashProcess_Idle;
			if(Manual_Wash)
			{
				stcTestData.CurrentTestStatus = enTestStatus_TestIdle;
			}
			UpdateTestScreenMenu();
			UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);
		}break;
	}
}
void StartTestExecution(void)
{
	/*Only continue test run if test run validity flag is TRUE*/
	bool BlanksPerformedFlag = VerifyForBlanksPerformed();
	if(false == BlanksPerformedFlag)
	{
		return;/*Retrun and abort test starting if blanks not performed*/
	}

	en_TestRunStatus Status = ReadTestRunStatus();
	if(enTestStatus_Idle == Status)
	{
		{
			AspSwitchLed_Red(en_AspLedON);
			AspSwitchLed_White(en_AspLedOFF);
			stcTestData.CurrentTestStatus = enTestStatus_Initialize;
		}
	}
}
void ExecuteTestProcedure(void)
{
	static bool NavigateScreenFlag = false;
	static bool AutoGainSwithFlag = false;
	switch(stcTestData.CurrentTestStatus)
	{
	case enTestStatus_Idle:
		NavigateScreenFlag = false;
//		AspSwitchLed_Red(en_AspLedOFF);
//		AspSwitchLed_White(en_AspLedON);
		break;
	case enTestStatus_ManualWash_Busy:

		if(LastTestId != stcTestData.TestId)
		{
			enWindowID NextWindow = stcScreenNavigation.NextWindowId;
			if(enkeyOk == ShowMainPopUp("Flowcell","Wash the Flowcell with DI Water\\r      Press 'OK' to Start Wash", true))
			{
				ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
				LastTestId = stcTestData.TestId;
				AspSwitchLed_Red(en_AspLedON);
				AspSwitchLed_White(en_AspLedOFF);
				Start_Wash();
				stcTestData.CurrentTestStatus = enTestStatus_Idle;
			}
		}
		else if(stcScreenNavigation.PrevWindowId == en_WinId_TestResultScreen && GetInstance_TestParams()[stcTestData.TestId].AspVoleume < 300 && WashBtwSample == true)
		{
			enWindowID NextWindow = stcScreenNavigation.NextWindowId;
			if(enkeyOk == ShowMainPopUp("Flowcell","Wash the Flowcell with DI Water\\r      Press 'OK' to Start Wash", true))
			{
				ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
				stcTestData.CurrentTestStatus = enTestStatus_Idle;
				AspSwitchLed_Red(en_AspLedON);
				AspSwitchLed_White(en_AspLedOFF);
				Start_Wash();
			}
		}
		else if(en_WashProcess_Idle == Get_WashStatus())
		{
			stcTestData.CurrentTestStatus = enTestStatus_Idle;
		}
		break;
	case enTestStatus_MoveFilterHome:

		HideButtonBackground(true);
		ShowHideInitializePopup(true, " ",INITIALIZE);
		AspSwitchLed_Red(en_AspLedON);
		AspSwitchLed_White(en_AspLedOFF);

		UpdateTestStatusTextBuffer(INITIALIZE);
		SelectFilter(en_FilterHome);
		stcTestData.CurrentTestStatus = enTestStatus_MoveFilterHome_Busy;
		break;
	case enTestStatus_MoveFilterHome_Busy:
	{
		if(en_FilterMotorBusy != ReadFilterMotorStatus())
		{
			if(en_FilterMotorHomeError == ReadFilterMotorStatus())
			{
				UpdateTestStatusTextBuffer(INITIALIZE_ERR);
				std::string Msg = "Filter Wheel";
				stcTestData.CurrentTestStatus = enTestStatus_InitError;
				strcpy(stcTestData.arrErrorMessage,Msg.c_str());
				ShowHideInitializePopup(false, " "," ");
				ShowHideTestScreenPopup(enTestPopUp_fitlerwheelError , stcTestData.arrErrorMessage);
			}
			else
			{
				stcTestData.CurrentTestStatus = enTestStatus_SelectDarkAdcFilter;
			}
		}
	}
	break;
	case enTestStatus_SelectDarkAdcFilter:
	{
		SelectFilter(en_FilterBlack);
		stcTestData.CurrentTestStatus = enTestStatus_SelectDarkAdcFilter_Busy;
	}break;
	case enTestStatus_SelectDarkAdcFilter_Busy:
	{
		if(en_FilterMotorBusy != ReadFilterMotorStatus())
		{
			osDelay(2000);
			stcTestData.DarkAdcPrimary = 0U;/*Resetting dark ADC - Primary*/
			stcTestData.DarkAdcSecondary = 0U;/*Resetting dark ADC - Secondary*/
			stcTestData.AdcDataFetchCount = 0;/*Resetting adc fetch count */
			stcTestData.CurrentTestStatus = enTestStatus_ReadDarkAdcPrimary;/*Goto ideal state if everything works fine*/
		}
	}
	break;
	case enTestStatus_ReadDarkAdcPrimary:
	{
		#define DARK_ADC_MEAS_COUNT_MAX (5)
		uint32_t BalanceTime = DARK_ADC_MEAS_COUNT_MAX - stcTestData.AdcDataFetchCount;/*Read Dark ADC 10 times for 1 second*/
		AutoGainSwithFlag = false;
		if(TestRunMillsDelay(50/*100 millis*/))
		{
			stcTestData.DarkAdcPrimary += ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , AutoGainSwithFlag);

			stcTestData.AdcDataFetchCount++;
			if(1 >= (BalanceTime))
			{
				stcTestData.DarkAdcPrimary /= DARK_ADC_MEAS_COUNT_MAX;
				if(stcTestData.DarkAdcPrimary > 300)
				{
					UpdateTestStatusTextBuffer(INITIALIZE_ERR);
					stcTestData.CurrentTestStatus = enTestStatus_DarkADCError;
					std::string Msg = "Optical Setup";
					strcpy(stcTestData.arrErrorMessage,Msg.c_str());
					ShowHideInitializePopup(false, " "," ");
					ShowHideTestScreenPopup(enTestPopUp_DarkADCError , stcTestData.arrErrorMessage);
				}
				else
				{
					/*If secondary wavelength is enabled*/
					if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
					{
						stcTestData.CurrentTestStatus = enTestStatus_ReadDarkAdcSecondary;
						stcTestData.AdcDataFetchCount = 0;
					}
					else/*If secondary wavelength is disabled*/
					{
						stcTestData.CurrentTestStatus = enTestStatus_SwitchPrimaryWavelength;/*Select primary wavelength*/
						stcTestData.NextTestStatus = enTestStatus_ManualWash_Busy;
					}
				}
			}
		}
	}
	break;
	case enTestStatus_ReadDarkAdcSecondary:
	{
		#define DARK_ADC_MEAS_COUNT_MAX (5)
		uint32_t BalanceTime = DARK_ADC_MEAS_COUNT_MAX - stcTestData.AdcDataFetchCount;/*Read Dark ADC 10 times for 1 second*/
		AutoGainSwithFlag = false;
		if(TestRunMillsDelay(50/*100 millis*/))
		{
			stcTestData.DarkAdcSecondary += ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].SecWavelength) , AutoGainSwithFlag);

			stcTestData.AdcDataFetchCount++;
			if(1 >= (BalanceTime))
			{
				stcTestData.DarkAdcSecondary /= DARK_ADC_MEAS_COUNT_MAX;
				if(stcTestData.DarkAdcSecondary > 300)
				{
					UpdateTestStatusTextBuffer(INITIALIZE_ERR);
					stcTestData.CurrentTestStatus = enTestStatus_DarkADCError;
					std::string Msg = "Optical Setup";
					strcpy(stcTestData.arrErrorMessage,Msg.c_str());
					ShowHideTestScreenPopup(enTestPopUp_DarkADCError , stcTestData.arrErrorMessage);
				}
				else
				{
					stcTestData.CurrentTestStatus = enTestStatus_SwitchPrimaryWavelength;/*Select primary wavelength*/
					stcTestData.NextTestStatus = enTestStatus_ManualWash_Busy;
				}
			}
		}
	}
	break;
	case enTestStatus_SwitchPrimaryWavelength:
	{
		SelectFilter(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength);
		stcTestData.CurrentTestStatus = enTestStatus_SwitchPrimaryWavelength_Busy;
	}break;

	case enTestStatus_SwitchPrimaryWavelength_Busy:
	{
		if(en_FilterMotorBusy != ReadFilterMotorStatus())
		{
			if(enTestStatus_WaitingForNextStep != stcTestData.NextTestStatus)
			{
				if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
				{
				switch(stcTestData.CurrentMeasType)
				{
				case en_DIWaterAdcBlank:
					UpdateTestStatusTextBuffer(COAG_DI_WATER);
					break;
				case en_CoagulationsampleIncubation:
					UpdateTestStatusTextBuffer(COAG_SAMPLE);
					break;
				case en_CoagulationQCIncubation:
					UpdateTestStatusTextBuffer(COAG_QC);
					break;
				case en_Sample:
				case en_TestQc:
					UpdateTestStatusTextBuffer(COAG_ADD_REAGENT_MEASU);
					break;
				case en_WashProcedure:
					break;
				default: break;
				}
			}
			else
			{
				switch(stcTestData.CurrentMeasType)
				{
					case en_DIWaterAdcBlank:
						UpdateTestStatusTextBuffer(ASPIRATE_DI_WATER);
						break;
					case en_ReagentBlank:
						UpdateTestStatusTextBuffer(ASPIRATE_REAGENT_BLANK);
						break;
					case en_Reagent:
						UpdateTestStatusTextBuffer(ASPIRATE_REAGENT);
						break;
					case en_StandardBlank:
						UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
						break;
					case en_Standard:
						UpdateTestStatusTextBuffer(ASPIRATE_STD);
						break;
					case en_SampleBlank:
						UpdateTestStatusTextBuffer(ASPIRATE_SAMPLE_BLANK);
						break;
					case en_Sample:
						UpdateTestStatusTextBuffer(ASPIRATE_SAMPLE);
						break;
					case en_TestQcBlank:
						UpdateTestStatusTextBuffer(ASPIRATE_QC_BLANK);
						break;
					case en_TestQc:
						UpdateTestStatusTextBuffer(ASPIRATE_QC);
						break;
					case en_WashProcedure:
						break;
					default: break;
				}
			}
		}
			stcTestData.CurrentTestStatus = stcTestData.NextTestStatus;
			HideButtonBackground(false);
			ShowHideInitializePopup(false, " "," ");
			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);
		}
	}
	break;
	case enTestStatus_Initialize:
		AspSwitchLed_Red(en_AspLedON);
		AspSwitchLed_White(en_AspLedOFF);
		if(TestRunMillsDelay(10))
		{
			stcTestData.Result = (float)0;
			stcTestData.SampleOpticalDensity = (float)0;
//			GraphlineShift = false;
			/*Loop to reset primary wavelength data buffer*/
			for(uint16_t nI = 0 ; nI < MAX_ADC_BUFFER_LENGTH_PRIMARY ; ++nI)
			{
				stcTestData.PrimaryAdcBuffer[nI] = 0;
				stcTestData.PrimaryAbsBuffer[nI] = (float)0;
			}
			/*Loop to reset secondary wavelength data buffer*/
			for(uint16_t nI = 0 ; nI < MAX_ADC_BUFFER_LENGTH_DEFAULT ; ++nI)
			{
				stcTestData.SecondaryAdcBuffer[nI] = 0;
				stcTestData.SecondaryAbsBuffer[nI] = (float)0;
			}
			stcTestData.CurrentTestStatus = enTestStatus_Aspiration;
		}
		break;
	case enTestStatus_Aspiration:
	{
		if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType != en_CoagulationTest)
		{
		switch(stcTestData.CurrentMeasType)
		{
			case en_DIWaterAdcBlank:
				/*Displaying current measurement status*/
				UpdateTestStatusTextBuffer(ASPIRATING_DI_WATER);
				break;
			case en_Reagent:
				/*Displaying current measurement status*/
				UpdateTestStatusTextBuffer(ASPIRATING_REAGENT);
				break;
			case en_ReagentBlank:
				/*Displaying current measurement status*/
				UpdateTestStatusTextBuffer(ASPIRATING_REAGENT_BLANK);
				break;
			case en_Standard:
				/*Displaying current measurement status*/
				UpdateTestStatusTextBuffer(ASPIRATING_STD);
				break;
			case en_StandardBlank:
				/*Displaying current measurement status*/
				UpdateTestStatusTextBuffer(ASPIRATING_STD_BLANK);
				break;
			case en_SampleBlank:
				/*Displaying current measurement status*/
				UpdateTestStatusTextBuffer(ASPIRATING_SAMPLE_BLANK);
				break;
			case en_Sample:
				/*Displaying current measurement status*/
				UpdateTestStatusTextBuffer(ASPIRATING_SAMPLE);
				break;
			case en_TestQcBlank:
				/*Displaying current measurement status*/
				UpdateTestStatusTextBuffer(ASPIRATING_QC_BLANK);
				break;
			case en_TestQc:
				/*Displaying current measurement status*/
				UpdateTestStatusTextBuffer(ASPIRATING_QC);
				break;
			default: break;

			}
		}
		if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType != en_CoagulationTest)
		{
			Aspirate_Pump(GetInstance_TestParams()[stcTestData.TestId].AspVoleume);
			stcTestData.CurrentTestStatus = enTestStatus_Aspiration_Busy;
		}
		else
		{	/*if test is coagulation and meas type is DIwater , sample or QC incbubation*/
			if(stcTestData.CurrentMeasType == en_DIWaterAdcBlank || stcTestData.CurrentMeasType == en_CoagulationsampleIncubation || stcTestData.CurrentMeasType == en_CoagulationQCIncubation)
			{
				UpdateTestStatusTextBuffer(INCUBATE_STARTED);
				LongBeepBuzzer();
				osDelay(2000);
				stcTestData.AdcDataFetchCount = 0;
				stcTestData.CurrentTestStatus = enTestStatus_Incubation;
			}
			else
			{
				/*if test is coagulation and meas type is sample or QC measure*/
				UpdateTestStatusTextBuffer(MEASURE_STARTED);
				LongBeepBuzzer();
				stcTestData.AdcDataFetchCount = 0;
				stcTestData.CurrentTestStatus = enTestStatus_PrimaryMeasurement;
			}
		}		

	}break;
	case enTestStatus_Aspiration_Busy:
	{
		if(enPump_AspCompleted == Pump_RunTask())
		{
			UpdateTestStatusTextBuffer(ASPIRATIO_COMPLETE);

			if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Turbidimetry)
			{
				StartTimer(&g_airGapTimer , 2 * 1000);/*Homming max delay*/
			}
			else
			{
				StartTimer(&g_airGapTimer , 2 * 1000);/*Homming max delay*/
			}
			stcTestData.CurrentTestStatus = enTestStatus_Aspiration_Post_Delay;
		}
	}break;

	case enTestStatus_Aspiration_Post_Delay:
	{
		if(true == Timer_IsTimeout(&g_airGapTimer , enTimerNormalStop))
		{
			LongBeepBuzzer();
			stcTestData.AdcDataFetchCount = 0;
			stcTestData.CurrentTestStatus = enTestStatus_AspirationAirGap_Pre_Delay;
			if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Turbidimetry)
			{
				StartTimer(&g_airGapTimer , 2 * 1000);/*Homming max delay*/
			}
			else
			{
				StartTimer(&g_airGapTimer , 2 * 1000);/*Homming max delay*/
			}
		}
		if(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength == en_Filter340)
		{
			ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , true);
			extrapolated_ADC = false;
		}

	}break;

	case enTestStatus_AspirationAirGap_Pre_Delay:
	{
		if(true == Timer_IsTimeout(&g_airGapTimer , enTimerNormalStop))
		{
			stcTestData.CurrentTestStatus = enTestStatus_AspirationAirGap;
		}

		if(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength == en_Filter340)
		{
			ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , true);
			extrapolated_ADC = false;
		}

	}break;
	case enTestStatus_AspirationAirGap:
	{
		if(GetInstance_TestParams()[stcTestData.TestId].AspVoleume < 300)
		{
			Aspirate_Pump(objstcSettings.fAirGapVol_uL * 1.34);//Aspiration of Air gap incrased to avoid carry over.
		}
		else
		{
			Aspirate_Pump(objstcSettings.fAirGapVol_uL);//Aspiration of Air gap //added
		}
		stcTestData.CurrentTestStatus = enTestStatus_AspirationAirGap_Busy;

	}break;
	case enTestStatus_AspirationAirGap_Busy:
	{
		if(enPump_AspCompleted == Pump_RunTask())
		{
			stcTestData.CurrentTestStatus = enTestStatus_Incubation;
		}
	}break;
	case enTestStatus_Incubation:
	{
			uint32_t BalanceTime = 0;
			if(en_DIWaterAdcBlank == stcTestData.CurrentMeasType)
			{
				BalanceTime = (INCUBATION_TIME_FOR_WATER_BLANK_SEC - stcTestData.AdcDataFetchCount);
				AutoGainSwithFlag = false;
				DelayTime = CURVE_DELAY_TIME + 1;
			}
			else
			{
				BalanceTime = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec - stcTestData.AdcDataFetchCount);
				AutoGainSwithFlag = true;
			}
			if(en_Filter340 != GetInstance_TestParams()[stcTestData.TestId].PrimWavelength)
			{
				DelayTime = CURVE_DELAY_TIME + 1;
			}

			if(DelayTime > CURVE_DELAY_TIME)
			{
				if(TestRunMillsDelay(1000))
				{
					char buff[48] = {0};
					snprintf(buff , 46 , "Incubating : %u sec" , (unsigned int)BalanceTime);
					UpdateTestStatusTextBuffer(std::string(buff));

					stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , AutoGainSwithFlag);/*1000 is the time delay before read in Ms*/
					stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] = calculateLightToAbsorbance(stcTestData.DIPrimaryFilterWaterAdc ,
																						stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] ,/*For incubation only use primary wavelength*/
																						stcTestData.DarkAdcPrimary , stcTestData.DIPrimaryFilterWaterGain , GetCurrentgainResistanceVal(), GetCurrentgainDarkADCVal());
					if(0 < stcTestData.AdcDataFetchCount)/*omited because we need two points to draw graph*/
					{
						if (en_Standard == stcTestData.CurrentMeasType || en_Sample == stcTestData.CurrentMeasType ||
								en_StandardBlank == stcTestData.CurrentMeasType || en_SampleBlank == stcTestData.CurrentMeasType ||
								en_TestQc== stcTestData.CurrentMeasType || en_TestQcBlank == stcTestData.CurrentMeasType)
						{
							float Abs = stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount];
							UpdateAbsBuffer(std::string(DoubleToCharConverter(Abs , STANDARD_PRECISION)));

							float MaxXVal = 0;
							float MaxYVal = 3;
							float MinXVal = 0;
							float MinYVal = (-1);

							if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
							{
								MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
										( 2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec));
							}
							else
							{
								MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
											GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
							}

							UpdateGraph(en_WinId_TestRun /*Window ID*/,
									(stcTestData.AdcDataFetchCount - 1)/*X1*/,
									stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount - 1] /*Y1*/,
									stcTestData.AdcDataFetchCount/*X2*/,
									stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] /*Y2*/,
									MinXVal/*X min value*/,
									(MaxXVal - 1)/*X max value*//*0 - 14*/,
									MinYVal/*Y min value*/,
									MaxYVal/*Y max value*/,
									TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
									TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
									TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
									TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
									COLOUR_MEAS_INCU);
						}
						else if(en_DIWaterAdcBlank == stcTestData.CurrentMeasType)
						{
							uint16_t DiWaterAdc = stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount];
							UpdateAbsBuffer(std::string(IntToCharConverter(DiWaterAdc)));

							float MaxXVal = 0;
							float MaxYVal = MAX_ADC_VALUE;
							float MinXVal = 0;
							float MinYVal = 0;
							if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
							{
								MaxXVal = (INCUBATION_TIME_FOR_WATER_BLANK_SEC + (2 * MEASUREMENT_TIME_FOR_WATER_BLANK_SEC));
							}
							else
							{
								MaxXVal = (INCUBATION_TIME_FOR_WATER_BLANK_SEC + MEASUREMENT_TIME_FOR_WATER_BLANK_SEC);
							}

							UpdateGraph(en_WinId_TestRun /*Window ID*/,
									(stcTestData.AdcDataFetchCount - 1)/*X1*/,
									stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount - 1] /*Y1*/,
									stcTestData.AdcDataFetchCount/*X2*/,
									stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] /*Y2*/,
									MinXVal/*X min value*/,
									(MaxXVal - 1)/*X max value*/,
									MinYVal/*Y min value*/,
									MaxYVal/*Y max value*/,
									TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
									TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
									TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
									TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
									COLOUR_MEAS_INCU);
						}
						else if(en_ReagentBlank == stcTestData.CurrentMeasType ||
								en_Reagent == stcTestData.CurrentMeasType)
						{
							float Abs = stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount];
							UpdateAbsBuffer(std::string(DoubleToCharConverter(Abs , STANDARD_PRECISION)));

							float MaxXVal = 0;
							float MaxYVal = 3;
							float MinXVal = 0;
							float MinYVal = (-1);
							if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
							{
								MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
										( 2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec));
							}
							else
							{
								MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
											GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
							}

							UpdateGraph(en_WinId_TestRun /*Window ID*/,
									(stcTestData.AdcDataFetchCount - 1)/*X1*/,
									stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount - 1] /*Y1*/,
									stcTestData.AdcDataFetchCount/*X2*/,
									stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] /*Y2*/,
									MinXVal/*X min value*/,
									(MaxXVal - 1)/*X max value*/,
									MinYVal/*Y min value*/,
									MaxYVal/*X max value*/,
									TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
									TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
									TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
									TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
									COLOUR_MEAS_INCU);
						}
						else if(en_CoagulationsampleIncubation == stcTestData.CurrentMeasType ||
								en_CoagulationQCIncubation == stcTestData.CurrentMeasType)
						{

						}
					}

					stcTestData.AdcDataFetchCount++;
					if(1 >= (BalanceTime))
					{
						DelayTime = 0;
						if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest && stcTestData.CurrentMeasType != en_DIWaterAdcBlank)
						{
							LongBeepBuzzer();
							if(stcTestData.CurrentMeasType == en_CoagulationsampleIncubation){
								stcTestData.CurrentTestStatus = enTestStatus_TestIdle;
								stcTestData.CurrentMeasType = en_Sample;
							}
							else if(stcTestData.CurrentMeasType == en_CoagulationQCIncubation){
								stcTestData.CurrentTestStatus = enTestStatus_TestIdle;
								stcTestData.CurrentMeasType = en_TestQc;
							}
						}
						/*no need to reset count*//*stcTestData.AdcDataFetchCount = 0;*/

						else{
								stcTestData.CurrentTestStatus = enTestStatus_PrimaryMeasurement;
							}
					}
				}
			}
			else if(DelayTime < 6)
			{
				if(TestRunMillsDelay(1000))
				{
					DelayTime++;
					ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , AutoGainSwithFlag);/*1000 is the time delay before read in Ms*/
				}
			}
		}break;

	case enTestStatus_SecondaryMeasurement:
	case enTestStatus_PrimaryMeasurement:
		if (en_DIWaterAdcBlank== stcTestData.CurrentMeasType)
		{
			uint32_t DataOffset = 0;
			uint32_t BalanceTime = 0;
			AutoGainSwithFlag = false;
			if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus)
			{
				BalanceTime = ((INCUBATION_TIME_FOR_WATER_BLANK_SEC + MEASUREMENT_TIME_FOR_WATER_BLANK_SEC)
														- stcTestData.AdcDataFetchCount);
			}
			else/*Secondary reading*/
			{
				BalanceTime = ((INCUBATION_TIME_FOR_WATER_BLANK_SEC + (2 * MEASUREMENT_TIME_FOR_WATER_BLANK_SEC))
														- stcTestData.AdcDataFetchCount);
				DataOffset = (INCUBATION_TIME_FOR_WATER_BLANK_SEC + MEASUREMENT_TIME_FOR_WATER_BLANK_SEC);
			}

			if(TestRunMillsDelay(1000))
			{
				uint16_t DiWaterAdc = 0;
				if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus)
				{
					DiWaterAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , AutoGainSwithFlag);
					stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] = DiWaterAdc;
				}
				else/*Secondary reading*/
				{
					DiWaterAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].SecWavelength) , AutoGainSwithFlag);
					stcTestData.SecondaryAdcBuffer[stcTestData.AdcDataFetchCount - DataOffset] = DiWaterAdc;
				}

				float MaxXVal = 0;
				float MaxYVal = MAX_ADC_VALUE;
				float MinXVal = 0;
				float MinYVal = 0;
				if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
				{
					MaxXVal = (INCUBATION_TIME_FOR_WATER_BLANK_SEC +
							( 2 * MEASUREMENT_TIME_FOR_WATER_BLANK_SEC));
				}
				else
				{
					MaxXVal = (INCUBATION_TIME_FOR_WATER_BLANK_SEC + MEASUREMENT_TIME_FOR_WATER_BLANK_SEC);
				}

				if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus)
				{
					UpdateGraph(en_WinId_TestRun /*Window ID*/,
							(stcTestData.AdcDataFetchCount - 1)/*X1*/,
							stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount - 1] /*Y1*/,
							stcTestData.AdcDataFetchCount/*X2*/,
							stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] /*Y2*/,
							MinXVal/*X min value*/,
							(MaxXVal - 1)/*X max value*/,
							MinYVal/*Y min value*/,
							MaxYVal/*Y max value*/,
							TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
							TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
							TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
							TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
							COLOUR_MEAS_PRI);

					UpdateAbsBuffer(std::string(IntToCharConverter(DiWaterAdc)));
				}
				else/*Secondary reading*/
				{
					float Y1 = 0;
					float Y2 = 0;
					if(DataOffset == stcTestData.AdcDataFetchCount)/*omited because we need two points to draw graph*/
					{
						Y1 = (float)stcTestData.PrimaryAdcBuffer[DataOffset - 1];
						Y2 = (float)stcTestData.SecondaryAdcBuffer[stcTestData.AdcDataFetchCount - DataOffset];
					}
					else/*else*/
					{
						Y1 = (float)stcTestData.SecondaryAdcBuffer[(stcTestData.AdcDataFetchCount - 1) - DataOffset];
						Y2 = (float)stcTestData.SecondaryAdcBuffer[stcTestData.AdcDataFetchCount - DataOffset];
					}
					UpdateGraph(en_WinId_TestRun /*Window ID*/,
							(stcTestData.AdcDataFetchCount - 1)/*X1*/,
							Y1 /*Y1*/,
							stcTestData.AdcDataFetchCount/*X2*/,
							Y2 /*Y2*/,
							MinXVal/*X min value*/,
							(MaxXVal - 1)/*X max value*/,
							MinYVal/*Y min value*/,
							MaxYVal/*Y max value*/,
							TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
							TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
							TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
							TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
							COLOUR_MEAS_SEC);

					UpdateConentrationBuffer(std::string(IntToCharConverter(DiWaterAdc)));
				}

				char buff[48] = {0};
				snprintf(buff , 46 , "Measuring : %u sec" ,(unsigned int)BalanceTime);
				UpdateTestStatusTextBuffer(std::string(buff));

				stcTestData.AdcDataFetchCount++;
				if(1 >= (BalanceTime))
				{

					if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus)
					{
						stcTestData.DIPrimaryFilterWaterGain = GetCurrentgainResistanceVal();/*saving di water blank gain*/
						GetInstance_TestParams()[stcTestData.TestId].DiwaterPriGain = GetCurrentgainResistanceVal();
						if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
						{
							stcTestData.CurrentTestStatus = enTestStatus_SwitchSecondaryWavelength;//Rotate secondary filter
						}
						else
						{
							stcTestData.CurrentTestStatus = enTestStatus_Wash;
						}
					}

					if(enTestStatus_SecondaryMeasurement == stcTestData.CurrentTestStatus)
					{
						stcTestData.DISecondaryFilterWaterGain = GetCurrentgainResistanceVal();/*saving di water blank gain*/
						GetInstance_TestParams()[stcTestData.TestId].DiwaterSecGain = GetCurrentgainResistanceVal();
						if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
						{
							stcTestData.CurrentTestStatus = enTestStatus_SwitchPrimaryWavelength;/*Select primary wavelength*/
							stcTestData.NextTestStatus = enTestStatus_Wash;
						}
						else
						{
							stcTestData.CurrentTestStatus = enTestStatus_Wash;
						}
					}
				}
			}
		}
		else if(en_ReagentBlank == stcTestData.CurrentMeasType ||
				en_Reagent == stcTestData.CurrentMeasType)
		{
			uint32_t DataOffset = 0;
			uint32_t BalanceTime = 0;
			float RunTimeAbs = 0;
			AutoGainSwithFlag = true;
			if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus)
			{
				BalanceTime = ((GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec)
														- stcTestData.AdcDataFetchCount);
			}
			else/*Secondary reading*/
			{
				BalanceTime = (((2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec) + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec)
														- stcTestData.AdcDataFetchCount);
				DataOffset = (GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec +
						GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec) ;
			}
			if(TestRunMillsDelay(1000))
			{
				uint16_t ReadAdc = 0;
				if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus)
				{
					ReadAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , AutoGainSwithFlag);
					stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] = ReadAdc;
					stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] = calculateLightToAbsorbance(stcTestData.DIPrimaryFilterWaterAdc ,
							ReadAdc ,
							stcTestData.DarkAdcPrimary ,
							stcTestData.DIPrimaryFilterWaterGain ,
							GetCurrentgainResistanceVal(), GetCurrentgainDarkADCVal());
					RunTimeAbs = stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount];

				}
				else/*Secondary reading*/
				{
					ReadAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].SecWavelength) , AutoGainSwithFlag);
					stcTestData.SecondaryAdcBuffer[stcTestData.AdcDataFetchCount - DataOffset] = ReadAdc;
					stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset] = calculateLightToAbsorbance(stcTestData.DISecondaryFilterWaterAdc ,
							ReadAdc ,
							stcTestData.DarkAdcSecondary ,
							stcTestData.DISecondaryFilterWaterGain ,
							GetCurrentgainResistanceVal(), GetCurrentgainDarkADCVal());
					RunTimeAbs = stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset];
				}
				UpdateAbsBuffer(std::string(DoubleToCharConverter(RunTimeAbs , STANDARD_PRECISION)));
				float MaxXVal = (0);
				float MaxYVal = 3;
				float MinXVal = 0;
				float MinYVal = (-1);
				if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
				{
					MaxXVal = ((2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec) + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec);
				}
				else
				{
					MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec);
				}

				if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus)
				{
					UpdateGraph(en_WinId_TestRun /*Window ID*/,
							(stcTestData.AdcDataFetchCount - 1)/*X1*/,
							stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount - 1] /*Y1*/,
							stcTestData.AdcDataFetchCount/*X2*/,
							stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] /*Y2*/,
							MinXVal/*X min value*/,
							(MaxXVal - 1)/*X max value*/,
							MinYVal/*Y min value*/,
							MaxYVal/*Y max value*/,
							TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
							TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
							TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
							TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
							COLOUR_MEAS_PRI);
				}
				else/*Secondary reading*/
				{
					float Y1 = 0;
					float Y2 = 0;
					if(DataOffset == stcTestData.AdcDataFetchCount)/*omited because we need two points to draw graph*/
					{
						Y1 = stcTestData.PrimaryAbsBuffer[DataOffset - 1];
						Y2 = stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset];
					}
					else/*else*/
					{
						Y1 = stcTestData.SecondaryAbsBuffer[(stcTestData.AdcDataFetchCount - 1) - DataOffset];
						Y2 = stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset];
					}

					UpdateGraph(en_WinId_TestRun /*Window ID*/,
							(stcTestData.AdcDataFetchCount - 1)/*X1*/,
							Y1 /*Y1*/,
							stcTestData.AdcDataFetchCount/*X2*/,
							Y2 /*Y2*/,
							MinXVal/*X min value*/,
							(MaxXVal - 1)/*X max value*/,
							MinYVal/*Y min value*/,
							MaxYVal/*Y max value*/,
							TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
							TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
							TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
							TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
							COLOUR_MEAS_SEC);
				}

				char buff[48] = {0};
				snprintf(buff , 46 , "Measuring : %u sec" ,	(unsigned int)BalanceTime);
				UpdateTestStatusTextBuffer(std::string(buff));

				stcTestData.AdcDataFetchCount++;
				if(1 >= (BalanceTime))
				{
					if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus &&
							en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
					{
						stcTestData.CurrentTestStatus = enTestStatus_SwitchSecondaryWavelength;//Rotate secondary filter
					}
					else if(enTestStatus_SecondaryMeasurement == stcTestData.CurrentTestStatus &&
							en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
					{
						stcTestData.CurrentTestStatus = enTestStatus_SwitchPrimaryWavelength;/*Select primary wavelength*/
						stcTestData.NextTestStatus = enTestStatus_Wash;
					}
					else
					{
						stcTestData.CurrentTestStatus = enTestStatus_Wash;
					}
				}
			}
		}
		else if (en_Standard == stcTestData.CurrentMeasType || en_Sample == stcTestData.CurrentMeasType ||
				en_StandardBlank == stcTestData.CurrentMeasType || en_SampleBlank == stcTestData.CurrentMeasType ||
				en_TestQc == stcTestData.CurrentMeasType || en_TestQcBlank == stcTestData.CurrentMeasType)
		{
			AutoGainSwithFlag = true;
			switch(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType)
			{
				case (en_Endpoint)://chnage to test paraters.
				{
					if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus &&
							en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].PrimWavelength)
					{
						uint32_t BalanceTime = ((GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec) -
																				stcTestData.AdcDataFetchCount);
						if(TestRunMillsDelay(1000))
						{
							uint16_t ReadAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , AutoGainSwithFlag);
							float CurrAdcResGain = GetCurrentgainResistanceVal();
							uint32_t CurrGainDarkADC = GetCurrentgainDarkADCVal();
							stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] = ReadAdc;

							stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] = calculateLightToAbsorbance(stcTestData.DIPrimaryFilterWaterAdc ,
									ReadAdc ,
									stcTestData.DarkAdcPrimary , stcTestData.DIPrimaryFilterWaterGain , CurrAdcResGain, CurrGainDarkADC);

							float Abs = stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount];
							UpdateAbsBuffer(std::string(DoubleToCharConverter(Abs , STANDARD_PRECISION)));

							float MaxXVal = (float)0;
							float MaxYVal = 3;
							float MinXVal = 0;
							float MinYVal = (-1);

							if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
							{
								MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
										( 2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec));
							}
							else
							{
								MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
											GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
							}
							UpdateGraph(en_WinId_TestRun /*Window ID*/,
									(stcTestData.AdcDataFetchCount - 1)/*X1*/,
									stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount - 1] /*Y1*/,
									stcTestData.AdcDataFetchCount/*X2*/,
									stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] /*Y2*/,
									MinXVal/*X min value*/,
									(MaxXVal - 1)/*X max value*/,
									MinYVal/*Y min value*/,
									MaxYVal/*X max value*/,
									TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
									TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
									TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
									TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
									COLOUR_MEAS_PRI);

							char buff[48] = {0};
							snprintf(buff , 46 , "Measuring : %u sec" ,(unsigned int)BalanceTime);
							UpdateTestStatusTextBuffer(std::string(buff));

							stcTestData.AdcDataFetchCount++;
							if(1 >= (BalanceTime))
							{
								if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus &&
										en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
								{
									stcTestData.CurrentTestStatus = enTestStatus_SwitchSecondaryWavelength;//Rotate secondary filter
								}
								else
								{
									stcTestData.CurrentTestStatus = enTestStatus_Wash;
								}
							}
						}
					}
					else if(enTestStatus_SecondaryMeasurement == stcTestData.CurrentTestStatus &&//Reading secondary wavelength
							en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
					{
						uint32_t BalanceTime = (((2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec) + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec) -
																				stcTestData.AdcDataFetchCount);
						uint32_t DataOffset = (GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec +
								GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec);

						if(TestRunMillsDelay(1000))
						{
							uint16_t ReadAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].SecWavelength) , AutoGainSwithFlag);
							float CurrAdcResGain = GetCurrentgainResistanceVal();
							uint32_t CurrGainDarkADC = GetCurrentgainDarkADCVal();
							stcTestData.SecondaryAdcBuffer[stcTestData.AdcDataFetchCount - DataOffset] = ReadAdc;

							stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset] =
									calculateLightToAbsorbance(stcTestData.DISecondaryFilterWaterAdc , ReadAdc ,
									stcTestData.DarkAdcSecondary , stcTestData.DISecondaryFilterWaterGain , CurrAdcResGain, CurrGainDarkADC);

							float Abs = stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset];
							UpdateAbsBuffer(std::string(DoubleToCharConverter(Abs , STANDARD_PRECISION)));

							float MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
									( 2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec));
							float MaxYVal = 3;
							float MinXVal = 0;
							float MinYVal = (-1);

							float Y1 = 0;
							float Y2 = 0;
							if(DataOffset == stcTestData.AdcDataFetchCount)/*omited because we need two points to draw graph*/
							{
								Y1 = stcTestData.PrimaryAbsBuffer[DataOffset - 1];
								Y2 = stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset];
							}
							else/*else*/
							{
								Y1 = stcTestData.SecondaryAbsBuffer[(stcTestData.AdcDataFetchCount - 1) - DataOffset];
								Y2 = stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset];
							}
							UpdateGraph(en_WinId_TestRun /*Window ID*/,
									((stcTestData.AdcDataFetchCount - 1))/*X1*/,
									Y1 /*Y1*/,
									(stcTestData.AdcDataFetchCount)/*X2*/,
									Y2 /*Y2*/,
									MinXVal/*X min value*/,
									(MaxXVal - 1)/*X max value*/,
									MinYVal/*Y min value*/,
									MaxYVal/*X max value*/,
									TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
									TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
									TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
									TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
									COLOUR_MEAS_SEC);

							char buff[48] = {0};
							snprintf(buff , 46 , "Measuring : %u sec" ,(unsigned int)BalanceTime);
							UpdateTestStatusTextBuffer(std::string(buff));

							stcTestData.AdcDataFetchCount++;
							if(1 >= (BalanceTime))
							{
								stcTestData.CurrentTestStatus = enTestStatus_SwitchPrimaryWavelength;/*Select primary wavelength*/
								stcTestData.NextTestStatus = enTestStatus_Wash;
							}
						}
					}
				}
				break;
				case (en_Kinetics):
				case (en_FixedTime)://chnage to test paraters.
				{
					uint32_t BalanceTime = ((GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec) -
																			stcTestData.AdcDataFetchCount);
					if(TestRunMillsDelay(1000))
					{
						uint16_t ReadAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , AutoGainSwithFlag);
						float CurrAdcResGain = GetCurrentgainResistanceVal();
						uint32_t CurrGainDarkADC = GetCurrentgainDarkADCVal();
						stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] = ReadAdc;

						stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] =
								calculateLightToAbsorbance(stcTestData.DIPrimaryFilterWaterAdc , ReadAdc ,
								stcTestData.DarkAdcPrimary , stcTestData.DIPrimaryFilterWaterGain , CurrAdcResGain, CurrGainDarkADC);

						float Abs = stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount];
						UpdateAbsBuffer(std::string(DoubleToCharConverter(Abs , STANDARD_PRECISION)));

						float MaxXVal = (float)(GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
								GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
						float MaxYVal = 3;
						float MinXVal = 0;
						float MinYVal = (-1);
						UpdateGraph(en_WinId_TestRun /*Window ID*/,
								(stcTestData.AdcDataFetchCount - 1)/*X1*/,
								stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount - 1] /*Y1*/,
								stcTestData.AdcDataFetchCount/*X2*/,
								stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] /*Y2*/,
								MinXVal/*X min value*/,
								(MaxXVal - 1)/*X max value*/,
								MinYVal/*Y min value*/,
								MaxYVal/*X max value*/,
								TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
								TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
								TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
								TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
								COLOUR_MEAS_PRI);

						char buff[48] = {0};
						snprintf(buff , 46 , "Measuring : %u sec" ,(unsigned int)BalanceTime);
						UpdateTestStatusTextBuffer(std::string(buff));

						stcTestData.AdcDataFetchCount++;

						if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics && stcTestData.AdcDataFetchCount > (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec + 59 ) && (en_Sample == stcTestData.CurrentMeasType || en_TestQc == stcTestData.CurrentMeasType ))
						{
							float SampleOD = 0;
							SampleOD = CalCulateOD_Kinetics_RealTime(stcTestData.TestId , stcTestData.AdcDataFetchCount);
							if(true == stcTestData.SampleBlankDoneFlag)
							{
								stcTestData.SampleOpticalDensity = SampleOD - stcTestData.SampleBlankAbs;
							}
							else
							{
								stcTestData.SampleOpticalDensity = SampleOD;
							}

							enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;
							if(en_1PointLinear == CalibType || en_Factor == CalibType)/*Factor*/
							{
								stcTestData.Result = CalibrateToResult(stcTestData.TestId , (stcTestData.SampleOpticalDensity - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs));
							}
							else
							{
								stcTestData.Result = CalibrateToResult(stcTestData.TestId , (stcTestData.SampleOpticalDensity));
							}

							UpdateConentrationBuffer(std::string(DoubleToCharConverter(stcTestData.Result , STANDARD_PRECISION)));
						}
						if(1 >= (BalanceTime))
						{
							stcTestData.CurrentTestStatus = enTestStatus_Wash;//Complete measurements
						}
					}
				}
				break;
				case (en_CoagulationTest):
				{
//					uint32_t BalanceTime = ((GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec) -
//																			stcTestData.AdcDataFetchCount);

					uint32_t BalanceTime = ((GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec - stcTestData.AdcDataFetchCount));
					if(TestRunMillsDelay(1000))
					{
						uint16_t ReadAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , AutoGainSwithFlag);
						float CurrAdcResGain = GetCurrentgainResistanceVal();
						uint32_t CurrGainDarkADC = GetCurrentgainDarkADCVal();
						stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] = ReadAdc;

						stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] =
								calculateLightToAbsorbance(stcTestData.DIPrimaryFilterWaterAdc , ReadAdc ,
								stcTestData.DarkAdcPrimary , stcTestData.DIPrimaryFilterWaterGain , CurrAdcResGain, CurrGainDarkADC);

						float Abs = stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount];
						UpdateAbsBuffer(std::string(DoubleToCharConverter(Abs , STANDARD_PRECISION)));

						float MaxXVal = (float)(GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
						float MaxYVal = 3;
						float MinXVal = 0;
						float MinYVal = (-1);
						UpdateGraph(en_WinId_TestRun /*Window ID*/,
								(stcTestData.AdcDataFetchCount - 1)/*X1*/,
								stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount - 1] /*Y1*/,
								stcTestData.AdcDataFetchCount/*X2*/,
								stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] /*Y2*/,
								MinXVal/*X min value*/,
								(MaxXVal - 1)/*X max value*/,
								MinYVal/*Y min value*/,
								MaxYVal/*X max value*/,
								TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
								TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
								TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
								TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
								COLOUR_MEAS_PRI);

						char buff[48] = {0};
						snprintf(buff , 46 , "Measuring : %u sec" ,	(unsigned int)BalanceTime);
						UpdateTestStatusTextBuffer(std::string(buff));

						stcTestData.AdcDataFetchCount++;
						if(1 >= (BalanceTime))
						{
							stcTestData.CurrentTestStatus = enTestStatus_Wash;//Complete measurements
						}
					}
				}
				break;
//				case (en_Differential):
//				{
//
//				}break;

				case (en_Turbidimetry):
				{
					uint32_t BalanceTime = ((GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec) -
																			stcTestData.AdcDataFetchCount);
					if(TestRunMillsDelay(1000))
					{
						uint16_t ReadAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , AutoGainSwithFlag);
						float CurrAdcResGain = GetCurrentgainResistanceVal();
						uint32_t CurrGainDarkADC = GetCurrentgainDarkADCVal();

						stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] = ReadAdc;

						stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] =
								calculateLightToAbsorbance(stcTestData.DIPrimaryFilterWaterAdc , ReadAdc ,
								stcTestData.DarkAdcPrimary , stcTestData.DIPrimaryFilterWaterGain , CurrAdcResGain, CurrGainDarkADC);

						float Abs = stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount];
						UpdateAbsBuffer(std::string(DoubleToCharConverter(Abs , STANDARD_PRECISION)));

						float MaxXVal = (float)(GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
								GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
						float MaxYVal = 3;
						float MinXVal = 0;
						float MinYVal = (-1);
						UpdateGraph(en_WinId_TestRun /*Window ID*/,
								(stcTestData.AdcDataFetchCount - 1)/*X1*/,
								stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount - 1] /*Y1*/,
								stcTestData.AdcDataFetchCount/*X2*/,
								stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] /*Y2*/,
								MinXVal/*X min value*/,
								(MaxXVal - 1)/*X max value*/,
								MinYVal/*Y min value*/,
								MaxYVal/*X max value*/,
								TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
								TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
								TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
								TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
								COLOUR_MEAS_PRI);

						char buff[48] = {0};
						snprintf(buff , 46 , "Measuring : %u sec" ,(unsigned int)BalanceTime);
						UpdateTestStatusTextBuffer(std::string(buff));

						stcTestData.AdcDataFetchCount++;
						if(1 >= (BalanceTime))
						{
							stcTestData.CurrentTestStatus = enTestStatus_Wash;//Complete measurements
						}
					}
				}break;

				case (en_Absorbance):
				{
					if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus &&
							en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].PrimWavelength)
					{
						uint32_t BalanceTime = ((GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec) -
																				stcTestData.AdcDataFetchCount);
						if(TestRunMillsDelay(1000))
						{
							uint16_t ReadAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength) , AutoGainSwithFlag);
							float CurrAdcResGain = GetCurrentgainResistanceVal();
							uint32_t CurrGainDarkADC = GetCurrentgainDarkADCVal();
							stcTestData.PrimaryAdcBuffer[stcTestData.AdcDataFetchCount] = ReadAdc;

							stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] = calculateLightToAbsorbance(stcTestData.DIPrimaryFilterWaterAdc ,
									ReadAdc ,
									stcTestData.DarkAdcPrimary , stcTestData.DIPrimaryFilterWaterGain , CurrAdcResGain, CurrGainDarkADC);

							float Abs = stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount];
							UpdateAbsBuffer(std::string(DoubleToCharConverter(Abs , STANDARD_PRECISION)));

							float MaxXVal = (float)0;
							float MaxYVal = 3;
							float MinXVal = 0;
							float MinYVal = (-1);

							if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
							{
								MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
										( 2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec));
							}
							else
							{
								MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
											GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec);
							}
							UpdateGraph(en_WinId_TestRun /*Window ID*/,
									(stcTestData.AdcDataFetchCount - 1)/*X1*/,
									stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount - 1] /*Y1*/,
									stcTestData.AdcDataFetchCount/*X2*/,
									stcTestData.PrimaryAbsBuffer[stcTestData.AdcDataFetchCount] /*Y2*/,
									MinXVal/*X min value*/,
									(MaxXVal - 1)/*X max value*/,
									MinYVal/*Y min value*/,
									MaxYVal/*X max value*/,
									TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
									TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
									TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
									TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
									COLOUR_MEAS_PRI);

							char buff[48] = {0};
							snprintf(buff , 46 , "Measuring : %u sec" ,(unsigned int)BalanceTime);
							UpdateTestStatusTextBuffer(std::string(buff));

							stcTestData.AdcDataFetchCount++;
							if(1 >= (BalanceTime))
							{
								if(enTestStatus_PrimaryMeasurement == stcTestData.CurrentTestStatus &&
										en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
								{
									stcTestData.CurrentTestStatus = enTestStatus_SwitchSecondaryWavelength;//Rotate secondary filter
								}
								else
								{
									stcTestData.CurrentTestStatus = enTestStatus_Wash;
								}
							}
						}
					}
					else if(enTestStatus_SecondaryMeasurement == stcTestData.CurrentTestStatus &&//Reading secondary wavelength
							en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)
					{
						uint32_t BalanceTime = (((2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec) + GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec) -
																				stcTestData.AdcDataFetchCount);
						uint32_t DataOffset = (GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec +
								GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec);

						if(TestRunMillsDelay(1000))
						{
							uint16_t ReadAdc = ReadPhotometryMeasurement(AssignGain(GetInstance_TestParams()[stcTestData.TestId].SecWavelength) , AutoGainSwithFlag);
							float CurrAdcResGain = GetCurrentgainResistanceVal();
							uint32_t CurrGainDarkADC = GetCurrentgainDarkADCVal();
							stcTestData.SecondaryAdcBuffer[stcTestData.AdcDataFetchCount - DataOffset] = ReadAdc;

							stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset] =
									calculateLightToAbsorbance(stcTestData.DISecondaryFilterWaterAdc , ReadAdc ,
									stcTestData.DarkAdcSecondary , stcTestData.DISecondaryFilterWaterGain , CurrAdcResGain, CurrGainDarkADC);

							float Abs = stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset];
							UpdateAbsBuffer(std::string(DoubleToCharConverter(Abs , STANDARD_PRECISION)));

							float MaxXVal = (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec +
									( 2 * GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec));
							float MaxYVal = 3;
							float MinXVal = 0;
							float MinYVal = (-1);

							float Y1 = 0;
							float Y2 = 0;
							if(DataOffset == stcTestData.AdcDataFetchCount)/*omited because we need two points to draw graph*/
							{
								Y1 = stcTestData.PrimaryAbsBuffer[DataOffset - 1];
								Y2 = stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset];
							}
							else/*else*/
							{
								Y1 = stcTestData.SecondaryAbsBuffer[(stcTestData.AdcDataFetchCount - 1) - DataOffset];
								Y2 = stcTestData.SecondaryAbsBuffer[stcTestData.AdcDataFetchCount - DataOffset];
							}
							UpdateGraph(en_WinId_TestRun /*Window ID*/,
									((stcTestData.AdcDataFetchCount - 1))/*X1*/,
									Y1 /*Y1*/,
									(stcTestData.AdcDataFetchCount)/*X2*/,
									Y2 /*Y2*/,
									MinXVal/*X min value*/,
									(MaxXVal - 1)/*X max value*/,
									MinYVal/*Y min value*/,
									MaxYVal/*X max value*/,
									TEST_GRAPH_PIXEL_MIN_X/*Graph X*/,
									TEST_GRAPH_PIXEL_MIN_Y/*Graph Y*/,
									TEST_GRAPH_PIXEL_MAX_X/*Graph Max X*/,
									TEST_GRAPH_PIXEL_MAX_Y/*Graph Max Y*/,
									COLOUR_MEAS_SEC);

							char buff[48] = {0};
							snprintf(buff , 46 , "Measuring : %u sec" ,(unsigned int)BalanceTime);
							UpdateTestStatusTextBuffer(std::string(buff));

							stcTestData.AdcDataFetchCount++;
							if(1 >= (BalanceTime))
							{
								stcTestData.CurrentTestStatus = enTestStatus_SwitchPrimaryWavelength;/*Select primary wavelength*/
								stcTestData.NextTestStatus = enTestStatus_Wash;
							}
						}
					}

				}break;
				default:/*No test condition*/
					break;
			}
		}
		break;
	case enTestStatus_SwitchSecondaryWavelength:
	{
		SelectFilter(GetInstance_TestParams()[stcTestData.TestId].SecWavelength);
		stcTestData.CurrentTestStatus = enTestStatus_SwitchSecondaryWavelength_Busy;
	}break;
	case enTestStatus_SwitchSecondaryWavelength_Busy:
	{
		if(en_FilterMotorBusy != ReadFilterMotorStatus())
		{
			stcTestData.CurrentTestStatus = enTestStatus_SecondaryMeasurement;
		}
	}
	break;
	case enTestStatus_WaitingForNextStep:
		UpdateTestStatusTextBuffer(TEST_COMPLETE);
		stcTestData.TestParamSaveEnabled = false;
		stcTestData.PrevMeasCompletedType = stcTestData.CurrentMeasType;/*Copy current completed measurement status to previous buffer*/
//		AspiratePump(GetInstance_TestParams()[stcTestData.TestId].AspVoleume); /*Emptying of cuvette */
		if (en_DIWaterAdcBlank == stcTestData.CurrentMeasType)
		{
			DrawRescaledGraph();
//			UpdateTestStatusTextBuffer("Washing");
//			stcTestData.CurrentTestStatus = enTestStatus_Wash;
			stcTestData.CurrentTestStatus = enTestStatus_saveData;
			stcTestData.WaterBlankDoneFlag = true;/*Making water blank done flag to true*/
			GetInstance_TestParams()[stcTestData.TestId].DiwaterDoneFlag = true;
			stcTestData.TestParamSaveEnabled = true;
			uint16_t ReadingStartLoc = INCUBATION_TIME_FOR_WATER_BLANK_SEC;
			uint16_t MaxNumOfMeasurement = MEASUREMENT_TIME_FOR_WATER_BLANK_SEC;
			uint32_t nAvgAdc = 0;
			{
				for(uint8_t nI = 0 ; nI < MaxNumOfMeasurement ; nI++)
				{
					nAvgAdc += stcTestData.PrimaryAdcBuffer[nI + ReadingStartLoc];
				}
				nAvgAdc /= MaxNumOfMeasurement;/*Averaging of ADC values*/
				stcTestData.DIPrimaryFilterWaterAdc = FindMaxADCValueFromList(stcTestData.PrimaryAdcBuffer, ReadingStartLoc, (ReadingStartLoc + MaxNumOfMeasurement));
				GetInstance_TestParams()[stcTestData.TestId].DiwaterPriADC = stcTestData.DIPrimaryFilterWaterAdc;

				Limit_Err_Msg = "";
				if(e_DI_Water_limit_flag == false)
				{
					if( nAvgAdc > uint32_t(g_Default_ADC_Ranges_max[(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength)])  || nAvgAdc < uint32_t(g_Default_ADC_Ranges_min[(GetInstance_TestParams()[stcTestData.TestId].PrimWavelength)]) )
					{
						std::string adc = std::string(IntToCharConverter(nAvgAdc));
						e_limit_error_popup = true;
						e_DI_Water_limit_flag = true;
						Limit_Err_Msg += "ADC "+adc+" Out Of Water blank ADC Range\\r";
					}
				}
			}
			if(en_FilterHome != GetInstance_TestParams()[stcTestData.TestId].SecWavelength)/*Process secondary filter*/
			/*Secondary filter*/
			{
				nAvgAdc = 0;
				for(uint8_t nI = 0 ; nI < MaxNumOfMeasurement ; nI++)
				{
					nAvgAdc += stcTestData.SecondaryAdcBuffer[nI];
				}
				nAvgAdc /= MaxNumOfMeasurement;/*Averaging of ADC values*/
				stcTestData.DISecondaryFilterWaterAdc = FindMaxADCValueFromList(stcTestData.SecondaryAdcBuffer, 0, MaxNumOfMeasurement);
				GetInstance_TestParams()[stcTestData.TestId].DiwaterSecADC = stcTestData.DISecondaryFilterWaterAdc;

				if(e_DI_Water_limit_flag == false)
				{
					if( nAvgAdc > uint32_t(g_Default_ADC_Ranges_max[(GetInstance_TestParams()[stcTestData.TestId].SecWavelength)]) || nAvgAdc < uint32_t(g_Default_ADC_Ranges_min[(GetInstance_TestParams()[stcTestData.TestId].SecWavelength)]) )
					{
						std::string adc = std::string(IntToCharConverter(nAvgAdc));
						e_limit_error_popup = true;
						e_DI_Water_limit_flag = true;
						Limit_Err_Msg += "ADC "+adc+" Out Of Water blank ADC Range\\r";
					}
				}
			}
			UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
//			UpdateAbsBuffer(std::string(IntToCharConverter(nAvgAdc)));
		}
		else if(en_Reagent == stcTestData.CurrentMeasType ||
				en_ReagentBlank == stcTestData.CurrentMeasType)
		{
			float CalcOD = 0;
			DrawRescaledGraph();
//			UpdateTestStatusTextBuffer("Washing");
//			stcTestData.CurrentTestStatus = enTestStatus_Wash;
			stcTestData.CurrentTestStatus = enTestStatus_saveData;
			stcTestData.TestParamSaveEnabled = true;

			if (true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable &&
					en_Reagent== stcTestData.CurrentMeasType)
			{
				GetInstance_TestParams()[stcTestData.TestId].ReagentDoneFlag = true;/*Making reagent / sample blank done flag to true*/
				GetInstance_TestParams()[stcTestData.TestId].ReagentAbs = CalcOD = CalculateOD(stcTestData.TestId);
				GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs = (GetInstance_TestParams()[stcTestData.TestId].ReagentAbs - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs);
				/*Added pop up warning for limits*/
				Limit_Err_Msg = "";
				for(uint32_t arrPriAbs = 0; arrPriAbs < (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec + GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec); arrPriAbs++)
				{
					if(e_abs_limit_flag == false)
					{
						if(stcTestData.PrimaryAbsBuffer[arrPriAbs] < DEFAULT_ABS_LOW_NEG || stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[1])
						{
							std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
							e_limit_error_popup = true;
							e_abs_limit_flag = true;
							Limit_Err_Msg += "Abs. "+abs+" Out of Measuring range\\r";
						}
					}
//					if (DEFAULT_REG_BLANK_LOW != GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[0] || DEFAULT_REG_BLANK_HIGH != GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[1])
//					{

					if((stcTestData.PrimaryAbsBuffer[arrPriAbs] < GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[0] && DEFAULT_REG_BLANK_LOW != GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[0]) ||
							(stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[1] &&  DEFAULT_REG_BLANK_HIGH != GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[1]))
					{
						if(e_reagent_blank_limit_flag == false)
						{
							std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
							e_limit_error_popup = true;
							e_reagent_blank_limit_flag = true;
							Limit_Err_Msg += "Abs." +abs+" Out of R.Blank limit\\r";
						}
					}

//					}
				}
				if (GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics)
				{
					if ((GetInstance_TestParams()[stcTestData.TestId].ReagentAbs > GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin) && (MAX_DELTA_PER_ABS_VAL != GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin))
					{
						std::string abs = std::string(DoubleToCharConverter(GetInstance_TestParams()[stcTestData.TestId].ReagentAbs  , 2));
						e_limit_error_popup = true;
						e_delAmin_limit_flag = true;
						Limit_Err_Msg += "Abs. " +abs+" higher than Max dAbs./min\\r";
					}
				   else if(HyperActiveSampleflag == true)
				   {
						e_limit_error_popup = true;
						Limit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
				   }
				}
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				stcTestData.CurrentMeasType = en_ReagentBlank;
				if(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
				{
					char arr_CBuffer[64] = {0};
					snprintf(arr_CBuffer , 63 , "%s    : %s" , "Test name" ,
							GetInstance_TestParams()[stcTestData.TestId].arrTestName);
					PrintStringOnPrinter(&arr_CBuffer[0]);

					snprintf(arr_CBuffer , 63 , "%s : %.0004f" , "R.Blank Abs." , GetInstance_TestParams()[stcTestData.TestId].ReagentAbs - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs);
					PrintStringOnPrinter(&arr_CBuffer[0]);

					if(GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
					{
						snprintf(arr_CBuffer , 63 , "%s : %.0004f" , "Reagent Abs." , GetInstance_TestParams()[stcTestData.TestId].ReagentAbs);
						PrintStringOnPrinter(&arr_CBuffer[0]);
					}
					PrinterFeedLine(1);
				}

			}
			else if(en_ReagentBlank == stcTestData.CurrentMeasType)/*Reagnet blank is doing first*/
			{
				GetInstance_TestParams()[stcTestData.TestId].ReagentBlankDoneFlag = true;/*Making reagent / sample blank done flag to true*/
				GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs = CalcOD = CalculateOD(stcTestData.TestId);
				GetInstance_TestParams()[stcTestData.TestId].ReagentAbs = float(0);/*even if he done only reagent blank he has to do reagent again*/

				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)/*Only if sample blank is switched on -> move to Reagent*/
				{
					stcTestData.CurrentMeasType = en_Reagent;
					GetInstance_TestParams()[stcTestData.TestId].ReagentDoneFlag = false;/*Making reagent / sample blank done flag to true*/
				}
				else
				{
					Limit_Err_Msg = "";
					for(uint32_t arrPriAbs = 0; arrPriAbs < (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec + GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec); arrPriAbs++)
					{
						if(e_abs_limit_flag == false)
						{
							if(stcTestData.PrimaryAbsBuffer[arrPriAbs] < DEFAULT_ABS_LOW_NEG || stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[1])
							{
								std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
								e_limit_error_popup = true;
								e_abs_limit_flag = true;
								Limit_Err_Msg += "Abs. " +abs+" Out of Measuring range\\r";
							}
						}
//						if (DEFAULT_REG_BLANK_LOW != GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[0] || DEFAULT_REG_BLANK_HIGH != GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[1])
//						{
//							if(e_reagent_blank_limit_flag == false)
//							{
//								if(stcTestData.PrimaryAbsBuffer[arrPriAbs] < GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[0] || stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[1])
//								{
//									std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
//									e_limit_error_popup = true;
//									e_reagent_blank_limit_flag = true;
//									Limit_Err_Msg += "Abs. " +abs+" Out of R.Blank limit\\r";
//								}
//							}
//						}
						if((stcTestData.PrimaryAbsBuffer[arrPriAbs] < GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[0] && DEFAULT_REG_BLANK_LOW != GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[0]) ||
								(stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[1] &&  DEFAULT_REG_BLANK_HIGH != GetInstance_TestParams()[stcTestData.TestId].ReagentBlankLimit[1]))
						{
							if(e_reagent_blank_limit_flag == false)
							{
								std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
								e_limit_error_popup = true;
								e_reagent_blank_limit_flag = true;
								Limit_Err_Msg += "Abs." +abs+" Out of R.Blank limit\\r";
							}
						}
					}
					if (GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics)
					{
							if ((GetInstance_TestParams()[stcTestData.TestId].ReagentAbs > GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin) && (MAX_DELTA_PER_ABS_VAL != GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin))
							{
								std::string abs = std::string(DoubleToCharConverter(GetInstance_TestParams()[stcTestData.TestId].ReagentAbs  , 2));
								e_limit_error_popup = true;
								e_delAmin_limit_flag = true;
								Limit_Err_Msg += "Abs. " +abs+" higher than Max dAbs./min\\r";
							}
						   else if(HyperActiveSampleflag == true)
						   {
								e_limit_error_popup = true;
								Limit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
						   }
					}

					if(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
					{
						char arr_CBuffer[64] = {0};
						snprintf(arr_CBuffer , 63 , "%s    : %s" , "Test name" ,
								GetInstance_TestParams()[stcTestData.TestId].arrTestName);
						PrintStringOnPrinter(&arr_CBuffer[0]);

						snprintf(arr_CBuffer , 63 , "%s : %.0004f" , "R.Blank Abs." , GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs);
						PrintStringOnPrinter(&arr_CBuffer[0]);

						if(GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
						{
							snprintf(arr_CBuffer , 63 , "%s : %.0004f" , "Reagent Abs." , GetInstance_TestParams()[stcTestData.TestId].ReagentAbs);
							PrintStringOnPrinter(&arr_CBuffer[0]);
						}
						PrinterFeedLine(1);
					}
				}
			}
			UpdateAbsBuffer(std::string(DoubleToCharConverter(CalcOD , STANDARD_PRECISION)));
		}
		else if(en_Standard == stcTestData.CurrentMeasType ||
				en_StandardBlank == stcTestData.CurrentMeasType)
		{
			float StdOD = 0;
			DrawRescaledGraph();
//			UpdateTestStatusTextBuffer("Washing");
			if(en_StandardBlank == stcTestData.CurrentMeasType)/*Default is standard blank first*/
			{
				e_RunTimeStdBlankOd[stcTestData.SelectedStandard] = StdOD = CalculateOD(stcTestData.TestId);
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				GetInstance_TestParams()[stcTestData.TestId].StandardBlankDoneFlag[stcTestData.SelectedStandard] = true;
				stcTestData.CurrentMeasType = en_Standard;
				/* Added pop up warning for limits*/

				Limit_Err_Msg = "";
				for(uint32_t arrPriAbs = 0; arrPriAbs < (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec + GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec); arrPriAbs++)
				{
						if(stcTestData.PrimaryAbsBuffer[arrPriAbs] < DEFAULT_ABS_LOW_NEG || stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[1])
						{
							std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
							e_limit_error_popup = true;
							e_abs_limit_flag = true;
							Limit_Err_Msg += "Abs. " +abs+" Out of Measuring range\\r";
							break;
						}
				}

			   if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics)
			   {
				  if((e_RunTimeStdBlankOd[stcTestData.SelectedStandard] > GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin) && (MAX_DELTA_PER_ABS_VAL != GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin))
				   {
					    std::string abs = std::string(DoubleToCharConverter(e_RunTimeStdBlankOd[stcTestData.SelectedStandard]  , 2));
						e_limit_error_popup = true;
						e_delAmin_limit_flag = true;
						Limit_Err_Msg += "Abs. " +abs+" higher than Max dAbs./min\\r";
						Limit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
				   }
				   else if(HyperActiveSampleflag == true)
				   {
						e_limit_error_popup = true;
						Limit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
				   }
			   }

			}
			else/*Normal standard*/
			{
				StdOD = CalculateOD(stcTestData.TestId);
				/* Added pop up warning for limits*/
				Limit_Err_Msg = "";
				for(uint32_t arrPriAbs = 0; arrPriAbs < (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec + GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec); arrPriAbs++)
				{
						if(stcTestData.PrimaryAbsBuffer[arrPriAbs] < DEFAULT_ABS_LOW_NEG || stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[1])
						{
							std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
							e_limit_error_popup = true;
							e_abs_limit_flag = true;
							Limit_Err_Msg += "Abs. " +abs+" Out of Measuring range\\r";
							break;
						}
				}
			   if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics)
			   {
				   if((e_RunTimeStdBlankOd[stcTestData.SelectedStandard] > GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin) && (MAX_DELTA_PER_ABS_VAL != GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin))
				   {
						std::string abs = std::string(DoubleToCharConverter(StdOD  , 2));
						e_limit_error_popup = true;
						e_delAmin_limit_flag = true;
						Limit_Err_Msg += "Abs. " +abs+" higher than Max dAbs./min\\r";
						Limit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
				   }

				   else if(HyperActiveSampleflag == true)
				   {
						e_limit_error_popup = true;
						Limit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
				   }
			   }
				if(true == GetInstance_TestParams()[stcTestData.TestId].StandardBlankDoneFlag[stcTestData.SelectedStandard])
				{
					e_RunTimeStdOd[stcTestData.SelectedStandard] = StdOD - e_RunTimeStdBlankOd[stcTestData.SelectedStandard];
				}
				else
				{
					e_RunTimeStdOd[stcTestData.SelectedStandard] = StdOD;
				}

				GetInstance_TestParams()[stcTestData.TestId].StandardDoneFlag[stcTestData.SelectedStandard] = true;
				NavigateScreenFlag = true;
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				stcTestData.SelectedStandard++;/*Auto increment selected standard position*/
				if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
				{
					stcTestData.CurrentMeasType = en_StandardBlank;
				}

			}
//			stcTestData.CurrentTestStatus = enTestStatus_Wash;
			stcTestData.CurrentTestStatus = enTestStatus_saveData;
			stcTestData.TestParamSaveEnabled = true;
		}
		else if (en_Sample == stcTestData.CurrentMeasType ||
				en_SampleBlank == stcTestData.CurrentMeasType)/*Sample*/
		{
			float SampleOD = 0;
			float ResultOD = 0;
			DrawRescaledGraph();
//			UpdateTestStatusTextBuffer("Washing");
			if(en_SampleBlank == stcTestData.CurrentMeasType)/*Default is Sample blank first*/
			{
				stcTestData.SampleBlankAbs = SampleOD = CalculateOD(stcTestData.TestId);
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				stcTestData.SampleBlankDoneFlag = true;
//				stcTestData.TestParamSaveEnabled = true;
				stcTestData.CurrentMeasType = en_Sample;

				Limit_Err_Msg = "";
				for(uint32_t arrPriAbs = 0; arrPriAbs < (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec + GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec); arrPriAbs++)
				{
						if(stcTestData.PrimaryAbsBuffer[arrPriAbs] < DEFAULT_ABS_LOW_NEG || stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[1])
						{
							std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
							e_limit_error_popup = true;
							e_abs_limit_flag = true;
							Limit_Err_Msg += "Abs " +abs+" Out of Measuring range\\r";
							break;
						}
				}

				 if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics)
				 {
					   if(stcTestData.SampleBlankAbs > GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin && (MAX_DELTA_PER_ABS_VAL != GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin) && (HyperActiveSampleflag != true))
					   {
							std::string abs = std::string(DoubleToCharConverter(SampleOD  , 2));
							e_limit_error_popup = true;
							e_delAmin_limit_flag = true;
							Limit_Err_Msg += "Abs. " +abs+" higher than Max dAbs./min\\r";
							Limit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
					   }
				 }
			}
			else/*Normal Sample -> en_Sample*/
			{
				SampleOD = CalculateOD(stcTestData.TestId);
//				stcTestData.SampleOpticalDensity = CalculateResultAbsorbance(stcTestData.TestId , StdOD);

				Limit_Err_Msg = "";
				for(uint32_t arrPriAbs = 0; arrPriAbs < (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec + GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec); arrPriAbs++)
				{
						if(stcTestData.PrimaryAbsBuffer[arrPriAbs] < DEFAULT_ABS_LOW_NEG || stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[1])
						{
							std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
							e_limit_error_popup = true;
							e_abs_limit_flag = true;
							Limit_Err_Msg += "Abs. " +abs+" Out of Measuring range\\r";
							break;
						}
				}

				 if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics)
				 {
					   if(SampleOD > GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin && (MAX_DELTA_PER_ABS_VAL != GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin) && (HyperActiveSampleflag != true))
					   {
							std::string abs = std::string(DoubleToCharConverter(SampleOD  , 2));
							e_limit_error_popup = true;
							e_delAmin_limit_flag = true;
							Limit_Err_Msg += "Abs. " +abs+" higher than Max dAbs./min\\r";
							Limit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
					   }
				 }
				if(true == stcTestData.SampleBlankDoneFlag)
				{
					stcTestData.SampleOpticalDensity = SampleOD - stcTestData.SampleBlankAbs;
				}
				else
				{
					stcTestData.SampleOpticalDensity = SampleOD;
				}
				/*subtract reagent blank if enabled but not saving subtracted value*/
				ResultOD = stcTestData.SampleOpticalDensity;
				if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType != en_CoagulationTest)
				{
					ConcOutofLimit_Err_Msg = "";
					enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;
					if(en_1PointLinear == CalibType || en_Factor == CalibType || en_4PL == CalibType)/*Factor*/
					{
						stcTestData.Result = CalibrateToResult(stcTestData.TestId , (ResultOD - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs));
					}
					else
					{
						stcTestData.Result = CalibrateToResult(stcTestData.TestId , (ResultOD));
					}

					if(HyperActiveSampleflag == true)
					{
						e_ConcOutofLimit_popup = true;
						std::string conc = std::string(DoubleToCharConverter(stcTestData.Result  , 2));
						ConcOutofLimit_Err_Msg += "Conc. of the sample is " +conc+" approximately\\r";
						ConcOutofLimit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
					}
					else
					{
						std::string conc = std::string(DoubleToCharConverter(stcTestData.Result , 2));
//						if (DEFAULT_LINEARITY_LOW != GetInstance_TestParams()[stcTestData.TestId].LinearityRange[0] || DEFAULT_LINEARITY_HIGH != GetInstance_TestParams()[stcTestData.TestId].LinearityRange[1])
//						{
//							if(stcTestData.Result < GetInstance_TestParams()[stcTestData.TestId].LinearityRange[0] || stcTestData.Result > GetInstance_TestParams()[stcTestData.TestId].LinearityRange[en_RangeHigh])
//							{
						if (DEFAULT_LINEARITY_HIGH != GetInstance_TestParams()[stcTestData.TestId].LinearityRange[1])
						{
							if(stcTestData.Result > GetInstance_TestParams()[stcTestData.TestId].LinearityRange[en_RangeHigh])
							{
							 e_ConcOutofLimit_popup = true;
							 e_linearity_limit_flag = true;
							 ConcOutofLimit_Err_Msg += "Conc. " +conc+" Out of Linearity range\\r";
							 ConcOutofLimit_Err_Msg += "Please dilute the sample and run again\\r";
							}
						}
					}

					if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_4PL)
					{
						float MaxStdAbs_4PL = FindMaxValueFromList(e_RunTimeStdOd, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved) - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
						float MinStdAbs_4PL = FindMinValueFromList(e_RunTimeStdOd, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved) - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

						if((ResultOD - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs) > MaxStdAbs_4PL)
						{
							e_ConcOutofLimit_popup = true;

							if(stcTestData.Result <= 0)
							{
								stcTestData.Result = 0;
							}

							std::string conc = std::string(DoubleToCharConverter(stcTestData.Result  , 2));
							std::string abs = std::string(DoubleToCharConverter(ResultOD  , 2));

							ConcOutofLimit_Err_Msg += "Conc. of the sample is " +conc+" approximately\\r";
							ConcOutofLimit_Err_Msg += "Absorbance " +abs+" out of calibration curve\\r";

						}
						else if((ResultOD - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs) < MinStdAbs_4PL)
						{
							e_ConcOutofLimit_popup = true;

							if(stcTestData.Result <= 0)
							{
								stcTestData.Result = 0;
							}

							std::string conc = std::string(DoubleToCharConverter(stcTestData.Result  , 2));
							std::string abs = std::string(DoubleToCharConverter(ResultOD  , 2));

							ConcOutofLimit_Err_Msg += "Conc. of the sample is " +conc+" approximately\\r";
							ConcOutofLimit_Err_Msg += "Absorbance " +abs+" out of calibration curve\\r";
						}
					}
					else if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_CubicSpline)
					{
						float MaxStdAbs_Cubic = FindMaxValueFromList(e_RunTimeStdOd, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
						float MaxStdConc_Cubic = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);

						if(ResultOD > MaxStdAbs_Cubic)
						{
							e_ConcOutofLimit_popup = true;

							if(stcTestData.Result <= 0)
							{
								stcTestData.Result = MaxStdConc_Cubic;
							}

							std::string conc = std::string(DoubleToCharConverter(stcTestData.Result  , 2));
							std::string abs = std::string(DoubleToCharConverter(ResultOD  , 2));

							ConcOutofLimit_Err_Msg += "Conc. of the sample is " +conc+" approximately\\r";
							ConcOutofLimit_Err_Msg += "Absorbance " +abs+" out of calibration curve\\r";

						}

						else if(ResultOD < 0)
						{
							e_ConcOutofLimit_popup = true;

							if(stcTestData.Result <= 0)
							{
								stcTestData.Result = 0;
							}

							std::string conc = std::string(DoubleToCharConverter(stcTestData.Result  , 2));
							std::string abs = std::string(DoubleToCharConverter(ResultOD  , 2));

							ConcOutofLimit_Err_Msg += "Conc. of the sample is " +conc+" approximately\\r";
							ConcOutofLimit_Err_Msg += "Absorbance " +abs+" out of calibration curve\\r";

						}
					}
				}
				NavigateScreenFlag = true;
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
				{
					stcTestData.CurrentMeasType = en_SampleBlank;
				}
			}
//			stcTestData.CurrentTestStatus = enTestStatus_Wash;
			stcTestData.CurrentTestStatus = enTestStatus_saveData;
//			UpdateAbsBuffer(std::string(DoubleToCharConverter(stcTestData.SampleOpticalDensity , STANDARD_PRECISION)));
//			UpdateConentrationBuffer(std::string(DoubleToCharConverter(stcTestData.Result , STANDARD_PRECISION)));
		}
		else if (en_TestQc == stcTestData.CurrentMeasType ||
				  en_TestQcBlank == stcTestData.CurrentMeasType)/*QC*/
		{
			float QC_OD = 0;
			float ResultQC_OD = 0;
			DrawRescaledGraph();
//			UpdateTestStatusTextBuffer("Washing");
			if(en_TestQcBlank == stcTestData.CurrentMeasType)/*Default is QC blank first*/
			{
				stcTestData.SampleBlankAbs = QC_OD = CalculateOD(stcTestData.TestId);
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				stcTestData.QCBlankDoneFlag = true;
				stcTestData.CurrentMeasType = en_TestQc;

				Limit_Err_Msg = "";

				for(uint32_t arrPriAbs = 0; arrPriAbs < (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec + GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec); arrPriAbs++)
				{
						if(stcTestData.PrimaryAbsBuffer[arrPriAbs] < DEFAULT_ABS_LOW_NEG || stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[1])
						{
							std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
							e_limit_error_popup = true;
							e_abs_limit_flag = true;
							Limit_Err_Msg += "Abs. " +abs+" Out of Measuring range\\r";
							break;
						}
				}
				 if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics)
				 {
					   if(stcTestData.SampleBlankAbs > GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin && (MAX_DELTA_PER_ABS_VAL != GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin) && (HyperActiveSampleflag != true))
					   {
							std::string abs = std::string(DoubleToCharConverter(QC_OD  , 2));
							e_limit_error_popup = true;
							e_delAmin_limit_flag = true;
							Limit_Err_Msg += "Abs. " +abs+" higher than Max dAbs./min\\r";
							Limit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
					   }
				 }
			}
			else/*Normal QC -> en_Sample*/
			{
				stcTestData.QCDoneFlag = true;
				QC_OD = CalculateOD(stcTestData.TestId);

				Limit_Err_Msg = "";
				for(uint32_t arrPriAbs = 0; arrPriAbs < (GetInstance_TestParams()[stcTestData.TestId].IncubationTimeSec + GetInstance_TestParams()[stcTestData.TestId].MeasurementTimeSec); arrPriAbs++)
				{
						if(stcTestData.PrimaryAbsBuffer[arrPriAbs] < DEFAULT_ABS_LOW_NEG || stcTestData.PrimaryAbsBuffer[arrPriAbs] > GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[1])
						{
							std::string abs = std::string(DoubleToCharConverter(stcTestData.PrimaryAbsBuffer[arrPriAbs]  , 2));
							e_limit_error_popup = true;
							e_abs_limit_flag = true;
							Limit_Err_Msg += "Abs. " +abs+" Out of Measuring range\\r";
							break;
						}
				}
				 if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_Kinetics)
				 {
					   if(QC_OD > GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin && (MAX_DELTA_PER_ABS_VAL != GetInstance_TestParams()[stcTestData.TestId].MaxDeltaAbsPerMin) &&(HyperActiveSampleflag != true))
					   {
							std::string abs = std::string(DoubleToCharConverter(QC_OD  , 2));
							e_limit_error_popup = true;
							e_delAmin_limit_flag = true;
							Limit_Err_Msg += "Abs. " +abs+" higher than Max dAbs./min\\r";
							Limit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
					   }
				 }
				 if(true == stcTestData.QCBlankDoneFlag)
				 {
					stcTestData.SampleOpticalDensity = QC_OD - stcTestData.SampleBlankAbs;
				 }
				 else
				 {
					stcTestData.SampleOpticalDensity = QC_OD;
				 }
				/*subtract reagent blank if enabled */
				ResultQC_OD = stcTestData.SampleOpticalDensity;

				if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType != en_CoagulationTest)
				{
					ConcOutofLimit_Err_Msg = "";
					enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;
					if(en_1PointLinear == CalibType || en_Factor == CalibType || en_4PL == CalibType)/*Factor*/
					{
						stcTestData.Result = CalibrateToResult(stcTestData.TestId , (ResultQC_OD - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs));
					}
					else
					{
						stcTestData.Result = CalibrateToResult(stcTestData.TestId , (ResultQC_OD));
					}

					if(HyperActiveSampleflag == true)
					{
						e_ConcOutofLimit_popup = true;
						std::string conc = std::string(DoubleToCharConverter(stcTestData.Result  , 2));
						ConcOutofLimit_Err_Msg += "Conc. of the sample is " +conc+" approximately\\r";
						ConcOutofLimit_Err_Msg += "Hyper Active sample detected. Dilute and re-run\\r";
					}

					else
					{
						std::string conc = std::string(DoubleToCharConverter(stcTestData.Result , 2));
						if (DEFAULT_LINEARITY_LOW != GetInstance_TestParams()[stcTestData.TestId].LinearityRange[0] || DEFAULT_LINEARITY_HIGH != GetInstance_TestParams()[stcTestData.TestId].LinearityRange[1])
						{
							if(stcTestData.Result < GetInstance_TestParams()[stcTestData.TestId].LinearityRange[0] || stcTestData.Result > GetInstance_TestParams()[stcTestData.TestId].LinearityRange[1])
							{
							 e_ConcOutofLimit_popup = true;
//							 e_linearity_limit_flag = true;
							 ConcOutofLimit_Err_Msg += "Conc. " +conc+" Out of Linearity range\\r";
							 ConcOutofLimit_Err_Msg += "Please dilute the sample and run again\\r";
							}
						}
					}

					if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_4PL)
					{
						float MaxStdAbs_4PL = FindMaxValueFromList(e_RunTimeStdOd, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved) - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
						float MinStdAbs_4PL = FindMinValueFromList(e_RunTimeStdOd, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved) - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

						float MaxStdConc_4PL = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);

						if( (ResultQC_OD - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs) > MaxStdAbs_4PL)
						{
							e_ConcOutofLimit_popup = true;

							if(stcTestData.Result <= 0)
							{
								stcTestData.Result = MaxStdConc_4PL;
							}

							std::string conc = std::string(DoubleToCharConverter(stcTestData.Result   , 2));
							std::string abs = std::string(DoubleToCharConverter(ResultQC_OD  , 2));

							ConcOutofLimit_Err_Msg += "Conc. of the sample is " +conc+" approximately\\r";
							ConcOutofLimit_Err_Msg += "Absorbance " +abs+" out of calibration curve\\r";

							if(stcTestData.Result <= 0)
							{
								stcTestData.Result = MaxStdConc_4PL;
							}

						}
						else if((ResultQC_OD - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs) < MinStdAbs_4PL)
						{
							e_ConcOutofLimit_popup = true;

							if(stcTestData.Result <= 0)
							{
								stcTestData.Result = 0;
							}

							std::string conc = std::string(DoubleToCharConverter(stcTestData.Result  , 2));
							std::string abs = std::string(DoubleToCharConverter(ResultQC_OD  , 2));

							ConcOutofLimit_Err_Msg += "Conc. of the sample is " +conc+" approximately\\r";
							ConcOutofLimit_Err_Msg += "Absorbance " +abs+" out of calibration curve\\r";
						}
					}
					else if(GetInstance_TestParams()[stcTestData.TestId].Calib == en_CubicSpline)
					{
						float MaxStdAbs_Cubic = FindMaxValueFromList(e_RunTimeStdOd, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);
						float MaxStdConc_Cubic = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc, GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved);

						if(ResultQC_OD > MaxStdAbs_Cubic)
						{
							e_ConcOutofLimit_popup = true;

							if(stcTestData.Result <= 0)
							{
								stcTestData.Result = MaxStdConc_Cubic;
							}

							std::string conc = std::string(DoubleToCharConverter(stcTestData.Result  , 2));
							std::string abs = std::string(DoubleToCharConverter(ResultQC_OD  , 2));

							ConcOutofLimit_Err_Msg += "Conc. of the sample is " +conc+" approximately\\r";
							ConcOutofLimit_Err_Msg += "Absorbance " +abs+" out of calibration curve\\r";
						}

						else if(ResultQC_OD < 0)
						{
							e_ConcOutofLimit_popup = true;

							if(stcTestData.Result <= 0)
							{
								stcTestData.Result = 0;
							}

							std::string conc = std::string(DoubleToCharConverter(stcTestData.Result  , 2));
							std::string abs = std::string(DoubleToCharConverter(ResultQC_OD  , 2));

							ConcOutofLimit_Err_Msg += "Conc. of the sample is " +conc+" approximately\\r";
							ConcOutofLimit_Err_Msg += "Absorbance " +abs+" out of calibration curve\\r";
						}
					}

				}
				NavigateScreenFlag = true;
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
				{
					stcTestData.CurrentMeasType = en_TestQcBlank;
				}
			}
//			stcTestData.CurrentTestStatus = enTestStatus_Wash;
			stcTestData.CurrentTestStatus = enTestStatus_saveData;
//			UpdateAbsBuffer(std::string(DoubleToCharConverter(stcTestData.SampleOpticalDensity , STANDARD_PRECISION)));
//			UpdateConentrationBuffer(std::string(DoubleToCharConverter(stcTestData.Result , STANDARD_PRECISION)));
		}
		break;
	case enTestStatus_Error:
		UpdateTestStatusTextBuffer(COAG_ABORT);
		stcTestData.CurrentTestStatus = enTestStatus_Wash;
	case enTestStatus_InitError:

		/*Do nothing*/
		break;
	case	enTestStatus_DarkADCError:
		/*Do nothing*/
		break;
	case enTestStatus_Wash:
			if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
			{
			UpdateTestStatusTextBuffer(TEST_COMPLETE);
			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);
			LongBeepBuzzer();
			stcTestData.CurrentTestStatus = enTestStatus_WaitingForNextStep;
			}
	
		else{
			Start_Wash();
			UpdateTestStatusTextBuffer(AUTO_EMPTY);
			stcTestData.CurrentTestStatus = enTestStatus_Wash_Busy;
			}
		break;
	case enTestStatus_Wash_Busy:
	{
		if(en_WashProcess_Idle == Get_WashStatus())
		{

			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);
//			LongBeepBuzzer();
			/*Reset status message test in UI*/
			stcTestData.CurrentTestStatus = enTestStatus_WaitingForNextStep;
//			SendOD_Uart(stcTestData.TestId);
		}
		
	}
	//		stcTestData.CurrentTestStatus = enTestStatus_saveData;

	
	break;
	case enTestStatus_saveData:
	{
		/*Save Test params*/
		if(true == stcTestData.TestParamSaveEnabled)
		{
			HandlerSaveTestParams();
		}
		stcTestData.CurrentTestStatus = enTestStatus_saveDataDummy;
	}
	break;
	case enTestStatus_saveDataDummy:
	{
		 if(e_limit_error_popup == true)
		 {
				enWindowID NextWindow = stcScreenNavigation.NextWindowId;
				if(enkeyOk == ShowMainPopUp("Abs. Limits Warning",Limit_Err_Msg, true))
				{
					ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
					e_limit_error_popup = false;
					e_DI_Water_limit_flag = false;
					e_reagent_blank_limit_flag = false;
					e_abs_limit_flag = false;
					e_delAmin_limit_flag = false;
				}
		 }
		 if(e_ConcOutofLimit_popup == true)
		 {
				enWindowID NextWindow = stcScreenNavigation.NextWindowId;
				if(enkeyOk == ShowMainPopUp("Conc. Limits Warning",ConcOutofLimit_Err_Msg, true))
				{
					ChangeWindowPage(en_WinId_TestRun , (enWindowID)NextWindow);
					e_ConcOutofLimit_popup = false;
//					e_linearity_limit_flag = false;
//					HyperActiveSampleflag = false;
				}
		 }
		 if(e_limit_error_popup == false && e_ConcOutofLimit_popup == false)
		 {
			stcTestData.CurrentTestStatus = enTestStatus_TestComplete;
		 }
	}
	break;

	case enTestStatus_TestComplete:
	{
		if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
		{
			if(g_en_TestPopUp == enTestPopUp_Null || g_en_TestPopUp == enTestPopUp_Abort )
			{
				ShowHideTestScreenPopup(enTestPopUp_Null , std::string(" "));
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				switch(stcTestData.CurrentMeasType)
				{
				case en_DIWaterAdcBlank:
					UpdateTestStatusTextBuffer(COAG_DI_WATER);
					if(false != stcTestData.WaterBlankDoneFlag)
					{
						enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Sample?", false);
						if(enKeyYes == Key)
						{
							stcTestData.CurrentMeasType = en_CoagulationsampleIncubation;
						}
						else
						{
							stcTestData.CurrentMeasType = en_DIWaterAdcBlank;
						}
						ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
					}
//					ClearGraph(en_WinId_TestRun /*Window ID*/,
//									TEST_GRAPH_PIXEL_MIN_X ,
//									TEST_GRAPH_PIXEL_MIN_Y,
//									TEST_GRAPH_PIXEL_X_WIDTH,
//									TEST_GRAPH_PIXEL_Y_HEIGHT);
//						UpdateTestScreenMenu();
//						UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
					break;
				case en_CoagulationsampleIncubation:
					UpdateTestStatusTextBuffer(COAG_SAMPLE);
					break;
				case en_CoagulationQCIncubation:
					UpdateTestStatusTextBuffer(COAG_QC);
					break;
				case en_Sample:
					UpdateTestStatusTextBuffer(COAG_ADD_REAGENT_MEASU);
					break;
				case en_TestQc:
					UpdateTestStatusTextBuffer(COAG_ADD_REAGENT_MEASU);
					break;
				case en_WashProcedure:
					break;
				default: break;
			}
			/*Reset Test status variable to TEST IDLE state*/
			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);
			stcTestData.CurrentTestStatus = enTestStatus_Idle;
			if (NavigateScreenFlag)
			{
				if (en_Sample == stcTestData.CurrentMeasType ||
						en_SampleBlank == stcTestData.CurrentMeasType)/*Sample*/
				{
					stcTestData.CurrentMeasType = en_CoagulationsampleIncubation;
					AddNewResultHistoryData(stcTestData.TestId , &stcTestData , stcTestData.arrUserName);
					g_u8IsNewResult = true;
					ShowResultScreen();/*Shows result in result screen*/
				}
				else if (en_TestQc == stcTestData.CurrentMeasType ||
						  en_TestQcBlank == stcTestData.CurrentMeasType)/*QC*/
				{
					e_QCTestDoneflag = true;
					stcTestData.CurrentMeasType = en_CoagulationQCIncubation;
					AppendQcResData(stcTestData.TestId , GetSelectedTestQcRow() , stcTestData.Result);
					ShowQCResLJPlotScreen();
				}
			}
		}
		else
			{
				stcTestData.CurrentTestStatus = enTestStatus_TestComplete;
			}
			break;
		}
		else
		{
//			char arrBuff[64] = {0};
			if(g_en_TestPopUp == enTestPopUp_Null || g_en_TestPopUp == enTestPopUp_Abort )
			{
				ShowHideTestScreenPopup(enTestPopUp_Null , std::string(" "));
				UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
				switch(stcTestData.CurrentMeasType)
				{
				case en_DIWaterAdcBlank:
					UpdateTestStatusTextBuffer(ASPIRATE_DI_WATER);
					if(false != stcTestData.WaterBlankDoneFlag)
					{
						if(true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable && GetInstance_TestParams()[stcTestData.TestId].ReagentBlankDoneFlag == false)
						{
							enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Reagent Blank?", false);
							if(enKeyYes == Key)
							{
								stcTestData.CurrentMeasType = en_ReagentBlank;
							}
							else
							{
								stcTestData.CurrentMeasType = en_DIWaterAdcBlank;
							}
							ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
						}
						else if(0 != GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)
						{
							bool AllStd_Doneflag = true;
							uint8_t NoOfStd = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
							for( uint8_t nI = 0; nI < NoOfStd; ++nI)
							{
								if(GetInstance_TestParams()[stcTestData.TestId].StandardDoneFlag[nI] != true)
								{
									AllStd_Doneflag = false;
									enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Standards?", false);
									if(enKeyYes == Key)
									{
										if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
										{
											stcTestData.CurrentMeasType = en_StandardBlank;
											stcTestData.SelectedStandard = 0;/*S0*/
										}
										else
										{
											stcTestData.CurrentMeasType = en_Standard;
											stcTestData.SelectedStandard = 0;/*S0*/
										}
										ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
									}
									else
									{
										stcTestData.CurrentMeasType = en_DIWaterAdcBlank;
										ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
									}
									break;
								}
							}
							if(AllStd_Doneflag)
							{
								enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Sample?", false);
								if(enKeyYes == Key)
								{
									if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
									{
										stcTestData.CurrentMeasType = en_SampleBlank;
									}
									else
									{
										stcTestData.CurrentMeasType = en_Sample;
									}
									ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
								}
								else
								{
									stcTestData.CurrentMeasType = en_DIWaterAdcBlank;
									ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
								}
							}
						}
						else
						{
							enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Sample?", false);
							if(enKeyYes == Key)
							{
								if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
								{
									stcTestData.CurrentMeasType = en_SampleBlank;
								}
								else
								{
									stcTestData.CurrentMeasType = en_Sample;
								}
								ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
							}
							else
							{
								stcTestData.CurrentMeasType = en_DIWaterAdcBlank;
								ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
							}
						}
//						ClearGraph(en_WinId_TestRun /*Window ID*/,
//												TEST_GRAPH_PIXEL_MIN_X ,
//												TEST_GRAPH_PIXEL_MIN_Y,
//												TEST_GRAPH_PIXEL_X_WIDTH,
//												TEST_GRAPH_PIXEL_Y_HEIGHT);
//
//						UpdateTestScreenMenu();
//						UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
					}
					break;
				case en_ReagentBlank:
					UpdateTestStatusTextBuffer(ASPIRATE_REAGENT_BLANK);
					if(false != GetInstance_TestParams()[stcTestData.TestId].ReagentBlankDoneFlag)
					{
						if(0 != GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)
						{
							bool AllStd_Doneflag = true;
							uint8_t NoOfStd = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
							for( uint8_t nI = 0; nI < NoOfStd; ++nI)
							{
								if(GetInstance_TestParams()[stcTestData.TestId].StandardDoneFlag[nI] != true)
								{
									AllStd_Doneflag = false;
									enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Standards?", false);
									if(enKeyYes == Key)
									{
										if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
										{
											stcTestData.CurrentMeasType = en_StandardBlank;
											stcTestData.SelectedStandard = 0;/*S0*/
										}
										else
										{
											stcTestData.CurrentMeasType = en_Standard;
											stcTestData.SelectedStandard = 0;/*S0*/
										}
										ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
									}
									else
									{
										stcTestData.CurrentMeasType = en_ReagentBlank;
										ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
									}
									break;
								}
							}
							if(AllStd_Doneflag)
							{
								enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Sample?", false);
								if(enKeyYes == Key)
								{
									if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
									{
										stcTestData.CurrentMeasType = en_SampleBlank;
									}
									else
									{
										stcTestData.CurrentMeasType = en_Sample;
									}
									ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
								}
								else
								{
									stcTestData.CurrentMeasType = en_ReagentBlank;
									ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
								}
							}
						}
						else
						{
							enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Sample?", false);
							if(enKeyYes == Key)
							{
								if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
								{
									stcTestData.CurrentMeasType = en_SampleBlank;
								}
								else
								{
									stcTestData.CurrentMeasType = en_Sample;
								}
								ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
							}
							else
							{
								stcTestData.CurrentMeasType = en_ReagentBlank;
								ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
							}
						}
//							ClearGraph(en_WinId_TestRun /*Window ID*/,
//												TEST_GRAPH_PIXEL_MIN_X ,
//												TEST_GRAPH_PIXEL_MIN_Y,
//												TEST_GRAPH_PIXEL_X_WIDTH,
//												TEST_GRAPH_PIXEL_Y_HEIGHT);
//
//							UpdateTestScreenMenu();
//							UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
					}
					break;

				case en_Reagent:
					UpdateTestStatusTextBuffer(ASPIRATE_REAGENT);

					if(false != GetInstance_TestParams()[stcTestData.TestId].ReagentDoneFlag && false != GetInstance_TestParams()[stcTestData.TestId].ReagentBlankDoneFlag)
					{
						if(0 != GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)
						{
							bool AllStd_Doneflag = true;
							uint8_t NoOfStd = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
							for( uint8_t nI = 0; nI < NoOfStd; ++nI)
							{
								if(GetInstance_TestParams()[stcTestData.TestId].StandardDoneFlag[nI] != true)
								{
									AllStd_Doneflag = false;
									enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Standards?", false);
									if(enKeyYes == Key)
									{
										if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
										{
											stcTestData.CurrentMeasType = en_StandardBlank;
											stcTestData.SelectedStandard = 0;/*S0*/
										}
										else
										{
											stcTestData.CurrentMeasType = en_Standard;
											stcTestData.SelectedStandard = 0;/*S0*/
										}
										ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
									}
									else
									{
										stcTestData.CurrentMeasType = en_ReagentBlank;
										ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
									}
									break;
								}
							}
							if(AllStd_Doneflag)
							{
								enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Sample?", false);
								if(enKeyYes == Key)
								{
									if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
									{
										stcTestData.CurrentMeasType = en_SampleBlank;
									}
									else
									{
										stcTestData.CurrentMeasType = en_Sample;
									}
									ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
								}
								else
								{
									stcTestData.CurrentMeasType = en_ReagentBlank;
									ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
								}
							}
						}
						else
						{
							enPopUpKeys Key = ShowMainPopUp("Run Test","Do you want to perform Sample?", false);
							if(enKeyYes == Key)
							{
								if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
								{
									stcTestData.CurrentMeasType = en_SampleBlank;
								}
								else
								{
									stcTestData.CurrentMeasType = en_Sample;
								}
								ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
							}
							else
							{
								stcTestData.CurrentMeasType = en_ReagentBlank;
								ChangeWindowPage(en_WinId_TestRun , (enWindowID)NULL);
							}
						}
//						ClearGraph(en_WinId_TestRun /*Window ID*/,
//												TEST_GRAPH_PIXEL_MIN_X ,
//												TEST_GRAPH_PIXEL_MIN_Y,
//												TEST_GRAPH_PIXEL_X_WIDTH,
//												TEST_GRAPH_PIXEL_Y_HEIGHT);
//
//						UpdateTestScreenMenu();
//						UpdateRealTimeBuffers(stcTestData.CurrentMeasType);
					}
					break;
				case en_StandardBlank:
					UpdateTestScreenMenu();
					UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
					break;
				case en_Standard:
				{
					if( GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable != true)
					{
						UpdateTestScreenMenu();
					}
					UpdateTestStatusTextBuffer(ASPIRATE_STD);
				}
					break;
				case en_SampleBlank:
					UpdateTestStatusTextBuffer(ASPIRATE_SAMPLE_BLANK);
					break;
				case en_Sample:
					UpdateTestStatusTextBuffer(ASPIRATE_SAMPLE);
					break;
				case en_TestQcBlank:
					UpdateTestStatusTextBuffer(ASPIRATE_QC_BLANK);
					break;
				case en_TestQc:
					UpdateTestStatusTextBuffer(ASPIRATE_QC);
					break;
				case en_WashProcedure:
					break;
				default: break;
			}

			/*Reset Test status variable to TEST IDLE state*/
			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);
			stcTestData.CurrentTestStatus = enTestStatus_Idle;
			if (NavigateScreenFlag)
			{
				if (en_Sample == stcTestData.CurrentMeasType ||
						en_SampleBlank == stcTestData.CurrentMeasType)/*Sample*/
				{
					g_u8IsNewResult = true;
					AddNewResultHistoryData(stcTestData.TestId , &stcTestData , stcTestData.arrUserName);
					ShowResultScreen();/*Shows result in result screen*/
				}
				else if (en_TestQc == stcTestData.CurrentMeasType ||
						  en_TestQcBlank == stcTestData.CurrentMeasType)/*QC*/
				{
					e_QCTestDoneflag = true;
					AppendQcResData(stcTestData.TestId , GetSelectedTestQcRow() , stcTestData.Result);
					ShowQCResLJPlotScreen();/*Shows result in result screen*/
				}
				else if(en_Standard == stcTestData.CurrentMeasType ||
						en_StandardBlank == stcTestData.CurrentMeasType)
				{
					/*Auto navigate to calib screen after final standard aspiration*/
					if((stcTestData.SelectedStandard) >= GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved)
					{
						stcTestData.SelectedStandard = 0xFF;/*For Showing previous standard values*/
						ShowTestCalibScreen();/*Shows Test Calib screen*/
					}
				}

			}
		}
		else
			{
				stcTestData.CurrentTestStatus = enTestStatus_TestComplete;
			}
			break;
		}
	}
	case enTestStatus_TestIdle:
	{
		if(GetInstance_TestParams()[stcTestData.TestId].TestProcedureType == en_CoagulationTest)
		{
			switch(stcTestData.CurrentMeasType)
			{
			case en_DIWaterAdcBlank:
				UpdateTestStatusTextBuffer(COAG_DI_WATER);
				break;
			case en_CoagulationsampleIncubation:
				UpdateTestStatusTextBuffer(COAG_SAMPLE);
				break;
			case en_CoagulationQCIncubation:
				UpdateTestStatusTextBuffer(COAG_QC);
				break;
			case en_Sample:
			case en_TestQc:
				UpdateTestStatusTextBuffer(COAG_ADD_REAGENT_MEASU);
				break;
			case en_WashProcedure:
				break;
			default: break;
			}
			stcTestData.CurrentTestStatus = enTestStatus_Idle;
			break;
		}
		else if(en_WashProcess_Idle == Get_WashStatus())
		{
			AspSwitchLed_Red(en_AspLedOFF);
			AspSwitchLed_White(en_AspLedON);

			switch(stcTestData.CurrentMeasType)
			{
				case en_DIWaterAdcBlank:
					UpdateTestStatusTextBuffer(ASPIRATE_DI_WATER);
					break;
				case en_ReagentBlank:
					UpdateTestStatusTextBuffer(ASPIRATE_REAGENT_BLANK);
					break;
				case en_Reagent:
					UpdateTestStatusTextBuffer(ASPIRATE_REAGENT);
					break;
				case en_StandardBlank:
					UpdateTestScreenMenu();
					UpdateTestStatusTextBuffer(ASPIRATE_STD_BLANK);
					break;
				case en_Standard:
					UpdateTestScreenMenu();
					UpdateTestStatusTextBuffer(ASPIRATE_STD);
					break;
				case en_SampleBlank:
					UpdateTestStatusTextBuffer(ASPIRATE_SAMPLE_BLANK);
					break;
				case en_Sample:
					UpdateTestStatusTextBuffer(ASPIRATE_SAMPLE);
					break;
				case en_TestQcBlank:
					UpdateTestStatusTextBuffer(ASPIRATE_QC_BLANK);
					break;
				case en_TestQc:
					UpdateTestStatusTextBuffer(ASPIRATE_QC);
					break;
				case en_WashProcedure:
					break;
				default: break;
			}
			stcTestData.CurrentTestStatus = enTestStatus_Idle;
			break;
		}
	}
	default:
		{
			break;
		}
	}
}

TestData* GetInstance_TestData(void)
{
	return &stcTestData;
}
