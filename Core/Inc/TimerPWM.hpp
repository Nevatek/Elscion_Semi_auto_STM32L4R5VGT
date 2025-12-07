/*
 * TimerPWM.h
 *
 *  Created on: Jul 14, 2022
 *      Author: Alvin
 */

#ifndef INC_TIMERPWM_H_
#define INC_TIMERPWM_H_

typedef enum
{
	enTimerUnitMicroSeconds = 0,
	enTimerUnitMilliseconds,
	enTimerUnitSeconds,
	enTimerUnitHertz,
	enTimerUnitKiloHertz,
	enTimerUnitMegaHertz,
}enTimerUnit;

typedef enum
{
	en_PeltierHeaterTimer = 0,
	en_ChamberHeaterTimer,
	en_DcMotorTimer,
	en_FilterMotorTimer,
	en_BuzzerTimer,
	en_PheripheralTimerMax,
}enPheriphealTimer;

#define APB_1_TIMER_CLOCK_FREQ 60000000UL
#define APB_2_TIMER_CLOCK_FREQ 60000000UL

uint8_t getAPBTimerChannel(TIM_HandleTypeDef *htim);
uint32_t GetTimerPrescalar(enPheriphealTimer Timer , float TimerInpFreqHz , float OutputFreqHz);
void SetTimerPwmDutycycle(TIM_HandleTypeDef* htim , uint32_t Channel , uint8_t DutyCyclePercentage);
void InitializeTimer(TIM_HandleTypeDef* htim , float n16OutputFrqHz , enTimerUnit Unit , enPheriphealTimer Timer);
void delayUs (TIM_HandleTypeDef *htim , uint16_t time);

#endif /* INC_TIMERPWM_H_ */
