/*
 * motor.c
 *
 *  Created on: Aug 2, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "FilterMotor.h"
#include "TimerPWM.hpp"
#include "../APPL/Inc/ApplUsbDevice.h"
#include "../APPL/Inc/NonVolatileMemory.h"

extern TIM_HandleTypeDef htim6;//Filter motor drive timer
//freq of filter motor is : 1Khz
#define FILTER_MOTOR_PULSE_TIMER htim6
/*Note : if this timer changes ,
 please change in main.cpp also ->void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)*/
uint8_t g_n8HomeEnable = false;
uint32_t g_n32MaxFilterSteps = 0 , g_n32Steps = 0;
enMicroStepps g_enFilterMicroSteps = MICRO_STEP_USED;

void IRQ_FilterMotorStep(void)
{
	static uint8_t PrevHomePinStatus = GPIO_PIN_SET;
	if(g_n8HomeEnable)/*Home sensor testing section*/
	{
		uint8_t CurrHomePInStatus = ReadFilterWheelSlotSens();
		if(GPIO_PIN_RESET == PrevHomePinStatus && GPIO_PIN_SET == CurrHomePInStatus)//Blocking sensor will give 0
		{
			g_n32Steps = g_n32MaxFilterSteps;
			g_n8HomeEnable = 2;
		}
		PrevHomePinStatus = CurrHomePInStatus;
	}
	if((g_n32MaxFilterSteps) && (g_n32Steps >= g_n32MaxFilterSteps)) // left shifting for *2 multiplication
	{
		HAL_TIM_Base_Stop_IT(&FILTER_MOTOR_PULSE_TIMER);
		g_n32MaxFilterSteps = g_n32Steps = 0;
		if(true == g_n8HomeEnable)
		{
			UsbDebugPrintStatusMessage("Filter home",en_DebugUsbStatusError,"NOT DETECTED");
		}
		return;
	}

    ++g_n32Steps;
	HAL_GPIO_TogglePin(FM_Step_Pin_GPIO_Port, FM_Step_Pin_Pin);
}
enFilterMotorStatus ReadFilterMotorStatus(void)
{
	if(0 < g_n32MaxFilterSteps)
	{
	   return en_FilterMotorBusy;
	}
	else if(true == g_n8HomeEnable)
	{
		return en_FilterMotorHomeError;
	}
	else
	{
		return (en_FilterMotorIdle);
	}
}

en_WavelengthFilter g_PrevFilter = en_FilterHome;
en_WavelengthFilter GetCurrentFilter(void)
{
 return g_PrevFilter;
}
void SelectFilter(en_WavelengthFilter enFilter)
{

	 float PrevFilterAngle = objstcSettings.WavelegthFilterAngles[g_PrevFilter];
	 float Angle = (objstcSettings.WavelegthFilterAngles[enFilter] - PrevFilterAngle);
	 if(0 > Angle)
	 {
		 Angle = 360 + Angle;
	 }
	 g_PrevFilter = enFilter;
	 switch (enFilter)
	 {
	 case en_FilterHome:
		 RotateFilterMotorAngle(Angle , en_FilterMotorHome);
		 break;
	 case en_FilterBlack:
		 RotateFilterMotorAngle(Angle , en_FilterMotorRight);
		 SetPreampGain((enPreampGain)objstcSettings.FilterPreampGain[en_FilterBlack]);
		 break;
	 case en_Filter340:
		 RotateFilterMotorAngle(Angle , en_FilterMotorRight);
		 SetPreampGain((enPreampGain)objstcSettings.FilterPreampGain[en_Filter340]);
		 break;
	 case en_Filter405:
		 RotateFilterMotorAngle(Angle , en_FilterMotorRight);
		 SetPreampGain((enPreampGain)objstcSettings.FilterPreampGain[en_Filter405]);
		 break;
//	 case en_Filter450:
//		 RotateFilterMotorAngle(Angle , en_FilterMotorRight);
//		 SetPreampGain((enPreampGain)objstcSettings.FilterPreampGain[en_Filter450]);
//		 break;
	 case en_Filter505:
		 RotateFilterMotorAngle(Angle , en_FilterMotorRight);
		 SetPreampGain((enPreampGain)objstcSettings.FilterPreampGain[en_Filter505]);
		 break;
	 case en_Filter546:
		 RotateFilterMotorAngle(Angle , en_FilterMotorRight);
		 SetPreampGain((enPreampGain)objstcSettings.FilterPreampGain[en_Filter546]);
		 break;
	 case en_Filter578:
		 RotateFilterMotorAngle(Angle , en_FilterMotorRight);
		 SetPreampGain((enPreampGain)objstcSettings.FilterPreampGain[en_Filter578]);
		 break;
	 case en_Filter630:
		 RotateFilterMotorAngle(Angle , en_FilterMotorRight);
		 SetPreampGain((enPreampGain)objstcSettings.FilterPreampGain[en_Filter630]);
		 break;
//	 case en_Filter670:
//		 RotateFilterMotorAngle(Angle , en_FilterMotorRight);
//		 SetPreampGain((enPreampGain)objstcSettings.FilterPreampGain[en_Filter670]);
//		 break;
	 default:break;
	 }
}
void InitalizeFilterMotor(void)
{
	HAL_GPIO_WritePin(FM_Reset_Pin_GPIO_Port,FM_Reset_Pin_Pin, GPIO_PIN_SET);		/*RESET*/
	HAL_GPIO_WritePin(FM_Sleep_Pin_GPIO_Port,FM_Sleep_Pin_Pin, GPIO_PIN_SET);		/*SLEEP*/
	SetFilterMotorSpeedRPM(g_enFilterMicroSteps , DEFAULT_FILTER_MOTOR_RPM);
}
void EnableDisableFilterMotor(enFilterMotorEngage enStatus)
{
	HAL_GPIO_WritePin(FM_Enable_Pin_GPIO_Port, FM_Enable_Pin_Pin, GPIO_PinState(enStatus));
}
uint8_t ReadFilterWheelSlotSens(void)
{
	return HAL_GPIO_ReadPin(FM_Ext_HomePin_GPIO_Port, FM_Ext_HomePin_Pin);
}
void SetFilterMotorDirection(enFilterMotorDirection enDir)
{
	g_n8HomeEnable = false;
	if(en_FilterMotorHome == enDir)
	{
		g_n8HomeEnable = true;
		enDir = en_FilterMotorRight;
	}
	HAL_GPIO_WritePin(FM_Dir_Pin_GPIO_Port, FM_Dir_Pin_Pin, GPIO_PinState(enDir));
}
uint8_t SetFilterMotorSpeedRPM(enMicroStepps enUs , uint16_t n16RPM)
{
	uint8_t Status = false;
	/*RPM = a/360 * fz * 60*/
	//Fz = (RPM) / ((a / 360) * 60)
	float StepAngle = (FILTER_STEPPER_FULL_STEP_ANGLE / enUs);
	float stepsperrev = (StepAngle / 360) * 60;
	float Freq =  ((n16RPM / stepsperrev));//in hertz
	if(Freq)
	{
		InitializeTimer(&FILTER_MOTOR_PULSE_TIMER , (1 * Freq) /*hz*/ , enTimerUnitHertz , en_FilterMotorTimer);
		Status = true;
	}
	else
	{
		UsbDebugPrintStatusMessage("Filter stepper RPM calculation",en_DebugUsbStatusError,"0");
	}
	return Status;
}
void RotateFilterMotorAngle(float angle , enFilterMotorDirection enDir)
{
	SetFilterMotorDirection(enDir);
	if(en_FilterMotorHome == enDir)
	{
		angle = FILTER_MOTOR_HOME_MAX_ANGLE;
	}
	float StepAngle = (FILTER_STEPPER_FULL_STEP_ANGLE / g_enFilterMicroSteps);
	uint32_t Steps = (uint32_t)angle / StepAngle;
//	Steps *= 2;//fixing issue of rotation half angle -> root cause not identified as of 8/2/2022
	if(Steps)
	{
		RotateFilterMotor(Steps);
	}
	else
	{
		UsbDebugPrintStatusMessage("Filter stepper angle calculation",en_DebugUsbStatusError,"0");
	}
}
void RotateFilterMotor(uint32_t Steps)
{
	HAL_TIM_StateTypeDef state = HAL_TIM_Base_GetState(&FILTER_MOTOR_PULSE_TIMER);
	if(HAL_TIM_STATE_READY == state)
	{
		{
			g_n32MaxFilterSteps = (2 * Steps);// to could both positive edge and negative edge of pulse.
			EnableDisableFilterMotor(enFilterMotorEnable);
			HAL_TIM_Base_Start_IT(&FILTER_MOTOR_PULSE_TIMER);
		}
	}
	else
	{
		UsbDebugPrintStatusMessage("Filter timer not ready",en_DebugUsbStatusError,IntToCharConverter(state));
	}
}
