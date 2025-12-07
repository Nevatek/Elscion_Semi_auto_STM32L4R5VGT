/*
 * SystemInit.h
 *
 *  Created on: Aug 3, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "math.h"
#ifndef INC_HANDLERPHERIPHERALS_H_
#define INC_HANDLERPHERIPHERALS_H_

#define PIN_ASP_SWITCH (GPIO_PIN_13)/*Refer main.cpp for usage*/

typedef enum
{
	en_AspLedOFF = 0,
	en_AspLedON
}enAspLed;
 typedef enum
 {
	 en_ValvePosProbe = 0,
	 en_ValvePosWaterTank,
	 en_MaxValvePos
 }enValveCntrl;


 typedef struct
 {
	 bool InitMemoryfailed;
	 bool InitDisplayfailed;
	 bool InitPrinterfailed;

	 bool SettingsMemCorrpt;
	 bool SettingsBackUpMemCorrpt;
	 bool SettingsBackUpWritefailed;

	 bool SecondarySettingsMemCorrpt;
	 bool SecondarySettingsBackUpMemCorrpt;
	 bool SecondarySettingsBackUpWritefailed;

	 bool TestParamsMemCorrpt;
	 bool TestParamsBackUpMemCorrpt;
	 bool TestParamsBackUpWritefaile;

	 bool TestResultMemCorrpt;
	 bool TestResultBackUpMemCorrpt;
	 bool TestResultBackUpWritefailed;

	 bool QCSetupMemCorrpt;
	 bool QCSetupBackUpMemCorrpt;
	 bool QCSetupBackUpWritefailed;

	 bool QCResultMemCorrpt;
	 bool QCResultBackUpMemCorrpt;
	 bool QCResultBackUpWritefailed;

}enSystemMemoryPeripherals;

extern enSystemMemoryPeripherals stcMemoryPeripherals;


void HandlerThreadPheripheral(void);
void InitializeAllPheripherals(void);
void InitializesystemPeripherals(void);
void CallBackAspSwExternalint(void);
void AspSwitchLed_Red(enAspLed enStatus);
void AspSwitchLed_White(enAspLed enStatus);
void AspSwitchLed_Blue(enAspLed enStatus);
void ThreeWayValeCntrl(enValveCntrl Cntrl);
enValveCntrl Get_ValveState(void);
uint8_t ReadAspSwitch(void);
void DebugSimulateAspirationSwPress(void);
uint8_t ReadAutoWashDoorStatus(void);
uint8_t ReadLiquidDetStatus(void);
void HandlerAspButton(void);
void HandlerMaintenanceAspButton(void);
void HandlerPowerOffAspButton(void);
void HandlerDiagOpticalAspButton(void);
void HandlerStartupDiagAspButton(void);
void HandlerDiagPumpAspButton(void);
void HandlerDiagFilterAspButton(void);
void TriggerTestProcess(void);
#endif /* INC_HANDLERPHERIPHERALS_H_ */
