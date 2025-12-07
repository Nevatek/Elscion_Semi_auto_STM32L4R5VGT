/*
 * SystemInit.c
 *
 *  Created on: Aug 3, 2022
 *      Author: Alvin
 */

#include <HandlerPheripherals.hpp>
#include "Buzzer.h"
#include "FilterMotor.h"
#include "Lamp.h"
#include "lcd.h"
#include "DcMotor.hpp"
#include "system.hpp"
#include "Preamp.h"
#include "Printer.h"
#include "ExtBarcode.h"
#include "USBDevice.h"
#include "../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/Testrun.h"
#include "../Screens/Inc/Screens.h"

uint8_t u_n8AspSwEnableInt = true;
extern osSemaphoreId SemAspSwHandle;
uint8_t e_bPrempDebugOutEnableFlag = false;
extern enPreampGain g_enCurrentPreampgain;
static enValveCntrl g_enValveCntrl = en_ValvePosProbe;

void CallBackAspSwExternalint(void)//Configured as rising edge
{
	uint16_t CurrWindowId = stcScreenNavigation.CurrentWindowId;
	en_TestRunStatus Status = ReadTestRunStatus();
	if(CurrWindowId == en_WinId_TestRun && enTestStatus_Idle == Status)
	{
		HandlerAspButton();
	}
	else if(CurrWindowId == en_WinId_MaintananceScreen)
	{
		HandlerMaintenanceAspButton();
	}
	else if(CurrWindowId == en_WinId_PowerOffMenu)
	{
		HandlerPowerOffAspButton();
	}
	else if(CurrWindowId == en_WinId_Diag_Optical)
	{
		HandlerDiagOpticalAspButton();
	}
	else if(CurrWindowId == en_WinId_Diag_Pump)
	{
		HandlerDiagPumpAspButton();
	}
	else if(CurrWindowId == en_WinId_Diag_Filter)
	{
		HandlerDiagFilterAspButton();
	}
	else if(CurrWindowId == en_WinId_SelfTestScreen)
	{
		HandlerStartupDiagAspButton();
	}
	else
	{
		InstrumentBusyBuzz();
	}
}
void TriggerTestProcess(void)
{
	if(u_n8AspSwEnableInt)
	{
		osSemaphoreRelease( SemAspSwHandle );
		u_n8AspSwEnableInt = false;//disabling asp switch intrupt.
	}
}
void DebugSimulateAspirationSwPress(void)
{
	if(u_n8AspSwEnableInt)
	{
		osSemaphoreRelease( SemAspSwHandle );
		u_n8AspSwEnableInt = false;//disabling asp switch.
	}
}
void AspSwitchLed_Red(enAspLed enStatus)
{
	HAL_GPIO_WritePin(AspirartionLed_Red_GPIO_Port, AspirartionLed_Red_Pin, GPIO_PinState(enStatus));
}
void AspSwitchLed_White(enAspLed enStatus)
{
	HAL_GPIO_WritePin(AspirartionLed_White_GPIO_Port, AspirartionLed_White_Pin, GPIO_PinState(enStatus));
}
void AspSwitchLed_Blue(enAspLed enStatus)
{
	HAL_GPIO_WritePin(AspirartionLed_Blue_GPIO_Port, AspirartionLed_Blue_Pin, GPIO_PinState(enStatus));
}
void ThreeWayValeCntrl(enValveCntrl Cntrl)
{
	g_enValveCntrl = Cntrl;
	HAL_GPIO_WritePin(ThreeWayValve_Pin_GPIO_Port, ThreeWayValve_Pin_Pin, GPIO_PinState(Cntrl));
}
enValveCntrl Get_ValveState(void)
{
	return g_enValveCntrl;
}
uint8_t ReadAspSwitch(void)
{
	#define DEBOUNCE_DELAY_ASP_SWITCH (500)//Ms
	int32_t n32SemStatus = 255;
	uint8_t AspSwStatus = 0xFF;
	n32SemStatus = osSemaphoreWait( SemAspSwHandle, ( TickType_t ) 0);
	if(!n32SemStatus)
	{
		osDelay(DEBOUNCE_DELAY_ASP_SWITCH);//Ms
	    AspSwStatus = 0;
		u_n8AspSwEnableInt = true;//enabling asp swithc intrupt;
	}
	return AspSwStatus;
}
uint8_t ReadAutoWashDoorStatus(void)
{
	return HAL_GPIO_ReadPin(AutoWashDoorFB_Pin_GPIO_Port, AutoWashDoorFB_Pin_Pin);
}
uint8_t ReadLiquidDetStatus(void)
{
	return HAL_GPIO_ReadPin(LiquidDetection_Pin_GPIO_Port, LiquidDetection_Pin_Pin);
}
void InitializeAllPheripherals(void)
{
//	EnableDisableLamp(en_LampEnable);
	AspSwitchLed_White(en_AspLedON);
	AspSwitchLed_Blue(en_AspLedOFF);
	AspSwitchLed_Red(en_AspLedOFF);

//	ReadSettingsBuffer();

	ThreeWayValeCntrl(en_ValvePosProbe);
	InitPreamp();
	InitalizeFilterMotor();
	InitalizeBarcodereader();

	InitPrinter(GetInstance_PrinterUart());


#if(PUMP_CHANNEL_TYPE == PUMP_GPIO)
	InitializeDcMotor();
#else
	InitializeDcMotor();
#endif
//	USBDebugPrintCurrentDateTime();
//	BeepBuzzer();
	osSemaphoreWait( SemAspSwHandle, ( TickType_t ) 1);//Initialize semaphore
}
/*
 * This is the main test running thread.
 *
 */
void HandlerThreadPheripheral(void)
{
	static uint8_t n32AspStatus = 255;
	n32AspStatus = ReadAspSwitch();
	en_TestRunStatus Status = ReadTestRunStatus();
	if(enTestStatus_Idle == Status && !n32AspStatus)
	{
		HandlerAspSwitchPressed();
	}
	else
	{
		ExecuteTestProcedure();
	}
}
void InitializesystemPeripherals(void)
{
	stcMemoryPeripherals.InitMemoryfailed = false;
	stcMemoryPeripherals.InitDisplayfailed = false;
	stcMemoryPeripherals.InitPrinterfailed = false;

	stcMemoryPeripherals.SettingsMemCorrpt = false;
	stcMemoryPeripherals.SettingsBackUpMemCorrpt = false;
	stcMemoryPeripherals.SettingsBackUpWritefailed = false;

	stcMemoryPeripherals.SecondarySettingsMemCorrpt = false;
	stcMemoryPeripherals.SecondarySettingsBackUpMemCorrpt = false;
	stcMemoryPeripherals.SecondarySettingsBackUpWritefailed = false;

	stcMemoryPeripherals.TestParamsMemCorrpt = false;
	stcMemoryPeripherals.TestParamsBackUpMemCorrpt = false;
	stcMemoryPeripherals.TestParamsBackUpWritefaile = false;

	stcMemoryPeripherals.TestResultMemCorrpt = false;
	stcMemoryPeripherals.TestResultBackUpMemCorrpt = false;
	stcMemoryPeripherals.TestResultBackUpWritefailed = false;

	stcMemoryPeripherals.QCSetupMemCorrpt = false;
	stcMemoryPeripherals.QCSetupBackUpMemCorrpt = false;
	stcMemoryPeripherals.QCSetupBackUpWritefailed = false;

	stcMemoryPeripherals.QCResultMemCorrpt = false;
	stcMemoryPeripherals.QCResultBackUpMemCorrpt = false;
	stcMemoryPeripherals.QCResultBackUpWritefailed = false;
}
