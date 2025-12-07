/*
 * DcMotor.c
 *
 *  Created on: Jul 14, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "math.h"
#include "cmsis_os.h"
#include "DcMotor.hpp"
#include "TimerPWM.hpp"
#include "Appl_Timer.hpp"
#include "../APPL/Inc/NonVolatileMemory.h"

static stcTimer g_stcPumpTimer;
static en_PumpStatus g_PumpStatus = enPump_Off;
static bool PumpMillsDelay(uint32_t Delay);

#if(PUMP_CHANNEL_TYPE == PUMP_GPIO)
void InitializeDcMotor(void)
{
	HAL_GPIO_WritePin(PUMP_Mode_GPIO_Port, PUMP_Mode_Pin,(GPIO_PinState)GPIO_PIN_SET);
}
void RotateDcMotor(enMotorEngageStatus State , en_DcMotorDirection Dir)
{
	uint8_t En1 = 0 , En2 = 0;
	if(State == enDcMotorEnable)/*Only if pump motor is enabled*/
	{
		switch(Dir)
		{
			case enDcMotorLeft:
				En1 = 0;
				En2 = 1;
				break;
			case enDcMotorRight:
				En1 = 1;
				En2 = 0;
				break;
			default:
				break;
		}
	}
	HAL_GPIO_WritePin(PUMP_En1_GPIO_Port, PUMP_En1_Pin,(GPIO_PinState)En1);
	HAL_GPIO_WritePin(PUMP_En2_GPIO_Port, PUMP_En2_Pin,(GPIO_PinState)En2);

}
void StopDcMotor(void)
{
	RotateDcMotor(enDcMotorDisable , enMotorDirMax);
}
void AspiratePump(uint16_t n16Vol)
{
    #define DUTYCYCLE_FOR_100UL_PER_500_MSEC (100)//duty cycle
	//2.5 sec  = 500Ul = 2500Ms
	//1250 Ms ==  250Ul
	//Y = 5 * X
	//Y = Time in Ms
	//X = Ul
	uint16_t TimeMs = 2 * (n16Vol);
	RotateDcMotor(enDcMotorEnable , enDcMotorLeft);
	HAL_Delay(TimeMs);
	StopDcMotor();
}
#else
extern TIM_HandleTypeDef htim3;//dc motor pwm generation timer
extern TIM_HandleTypeDef htim17;//Timer for time vs voleume of pump , ie: 1 sec 200ul

#define DC_PUMP_DELAY_TIMER htim17
#define DC_PUMP_PWM_CHANNEL TIM_CHANNEL_3
#define DC_PUMP_PWM_TIMER htim3

void InitializeDcMotor(void)
{
	  InitializeTimer(&DC_PUMP_PWM_TIMER , DC_MOTOR_MASTER_FREQUENCY , enTimerUnitHertz , en_DcMotorTimer);
	  HAL_GPIO_WritePin(PUMP_Mode_GPIO_Port, PUMP_Mode_Pin,(GPIO_PinState)GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(PUMP_En1_GPIO_Port, PUMP_En1_Pin,(GPIO_PinState)GPIO_PIN_RESET);
}
void RotateDcMotor(en_DcMotorDirection nDirection, uint8_t DutyCycle)
{
	if(DutyCycle)
	{
		SetTimerPwmDutycycle(&DC_PUMP_PWM_TIMER, DC_PUMP_PWM_CHANNEL, DutyCycle);//(100-DutyCycle) is done beacause of inverted pwm output channel.
		HAL_TIM_PWM_Start(&DC_PUMP_PWM_TIMER, DC_PUMP_PWM_CHANNEL);
	}
	else
	{
		StopDcMotor();
	}
}
void Aspirate_Pump(uint16_t n16Vol)
{
    #define DUTYCYCLE_FOR_100UL_PER_500_MSEC (65)//duty cycle
	uint16_t TimeMs = 0;
	if(enPump_Off == g_PumpStatus || enPump_AspCompleted == g_PumpStatus)
	{

//		TimeMs = 1.75 * (n16Vol);/* kamoer pump */
		TimeMs = objstcSettings.fPumpConstant * (n16Vol);/* kamoer pump */
		RotateDcMotor(enDcMotorLeft , objstcSettings.u_n8DcMotorPwmDuty);

		if(TimeMs > MAX_ASP_VOL_MS)
		{
			TimeMs = MAX_ASP_VOL_MS;
		}

		StartTimer(&g_stcPumpTimer , TimeMs);/*Pump delay*/
		g_PumpStatus = enPump_Running;
	}
}
en_PumpStatus Pump_RunTask(void)
{
	if(true == Timer_IsTimeout(&g_stcPumpTimer , enTimerNormalStop))
	{
		StopDcMotor();
		g_PumpStatus = enPump_AspCompleted;
	}
	return g_PumpStatus;
}
void StopDcMotor(void)
{
	HAL_TIM_PWM_Stop(&DC_PUMP_PWM_TIMER, DC_PUMP_PWM_CHANNEL);
}
#endif
bool PumpMillsDelay(uint32_t Delay)
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
