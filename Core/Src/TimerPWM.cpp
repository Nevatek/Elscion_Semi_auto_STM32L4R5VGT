/*
 * TimerPWM.c
 *
 *  Created on: Jul 14, 2022
 *      Author: Alvin
 */

#include "main.h"
#include "cmsis_os.h"
#include "TimerPWM.hpp"

uint32_t g_TimerPrescalar[en_PheripheralTimerMax] = {//Please add prescalar to this lookup table
		22,//en_PeltierHeaterTimer
		22,//en_ChamberHeaterTimer
		39,//en_DcMotorTimer
		0,//en_FilterMotorTimer
		38,//en_BuzzerTimer
};

uint32_t g_TimerArrReg[en_PheripheralTimerMax] = {//Please add ARR to this lookup table
		99,//en_PeltierHeaterTimer
		99,//en_ChamberHeaterTimer
		59999,//en_DcMotorTimer
		59999,//en_FilterMotorTimer
		100,//en_BuzzerTimer
};
uint32_t GetTimerPrescalar(enPheriphealTimer Timer , float TimerInpFreqHz , float OutputFreqHz)
{
	//Output freq = Input clokc / (Prescalar + 1)
	//1 / (Prescalar + 1) = Output freq / input clock
	//prescalar = (Input freq / Output clock) - 1;
#if 0
	uint32_t Prescalar = 0;
	Prescalar = (uint32_t)(TimerInpFreqHz / OutputFreqHz) - 1;
	return Prescalar;
#else
	return g_TimerPrescalar[Timer];
#endif
}
void InitializeTimer(TIM_HandleTypeDef* htim , float n16OutputFrqHz , enTimerUnit Unit , enPheriphealTimer Timer)
{
   float AutoReloadReg = 0;
   float OutputFreqHz = n16OutputFrqHz;
   float TimerInpFreqHz = 0;
   uint32_t TimerCounter = 0;
   if(enTimerUnitSeconds == Unit /*|| enTimerUnitMilliseconds == Unit ||
		   enTimerUnitMicroSeconds == Unit*/)//converting to freq
   {
	   OutputFreqHz = (1 / n16OutputFrqHz);
   }
	if(true == getAPBTimerChannel(htim))
	{
		TimerInpFreqHz = APB_1_TIMER_CLOCK_FREQ;
	}
	else
	{
		TimerInpFreqHz = APB_2_TIMER_CLOCK_FREQ;
	}
   uint32_t Prescalar = GetTimerPrescalar(Timer , TimerInpFreqHz , n16OutputFrqHz);
#if 1
   AutoReloadReg = (TimerInpFreqHz / ((Prescalar + 1) * OutputFreqHz)) - 1;
#else
   AutoReloadReg = g_TimerArrReg[Timer];
#endif
   __HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_UPDATE);
   __HAL_TIM_SET_PRESCALER(htim , Prescalar);
   __HAL_TIM_SET_AUTORELOAD(htim, (uint32_t)AutoReloadReg);
   __HAL_TIM_SET_COUNTER(htim, (uint32_t)TimerCounter);
}
void SetTimerPwmDutycycle(TIM_HandleTypeDef* htim , uint32_t Channel , uint8_t DutyCyclePercentage)
{
   uint32_t ArrRegValue = __HAL_TIM_GET_AUTORELOAD(htim);
   //Percentage = ( value / Total value ) * 100;
   //Value / Total value = (Percentage / 100)
   //Value = (Percentage / 100) * Total value;
   uint32_t CompareValue = ((DutyCyclePercentage * ArrRegValue)/100);
   __HAL_TIM_SET_COMPARE(htim, Channel, CompareValue);
}
/* uint8_t getAPBTimerChannel(TIM_HandleTypeDef *htim)
 * use : To get which APB bus, the corresponding Timer reside.
 * Arguments:
 * htim : Timer handle
 * return: APB bus number
 */
uint8_t getAPBTimerChannel(TIM_HandleTypeDef *htim)
{
	uint8_t ret = false;
	switch((uintptr_t)htim->Instance)
	{
	case (uintptr_t)TIM2_BASE:
	case (uintptr_t)TIM3_BASE:
	case (uintptr_t)TIM4_BASE:
	case (uintptr_t)TIM5_BASE:
	case (uintptr_t)TIM6_BASE:
	case (uintptr_t)TIM7_BASE:
	case (uintptr_t)TIM15_BASE:
	case (uintptr_t)TIM17_BASE:
		ret = true;
		break;
	case (uintptr_t)TIM1_BASE:
	case (uintptr_t)TIM8_BASE:
	case (uintptr_t)TIM16_BASE:
		ret = false;
		break;
	default:
		break;
	}
	return ret;
}
/*
 * Prescalar for Us : (50 - 1)
 */
void delayUs (TIM_HandleTypeDef *htim , uint16_t time)
{
	__HAL_TIM_SET_COUNTER(htim, 0);
	while ((__HAL_TIM_GET_COUNTER(htim)) < time);
}
