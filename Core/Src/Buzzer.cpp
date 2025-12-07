/*
 * Buzzer.c
 *
 *  Created on: May 18, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "system.hpp"
#include "cmsis_os.h"
#include "TimerPWM.hpp"
#include "Buzzer.h"
#include "../APPL/Inc/NonVolatileMemory.h"

//extern TIM_HandleTypeDef htim5;
//#define BUZZER_DELAY_TIMER  htim5
/*Note : if this timer changes ,
 please change in main.cpp also ->void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)*/
#define BUZZER_FREQ 2000//Hz
#define BUZZER_DURATION_NORMAL 30//ms

typedef enum {
    BUZ_STATE_IDLE,
    BUZ_STATE_BEEP_ON,
    BUZ_STATE_BEEP_OFF
} BuzzerState_t;

static BuzzerState_t buzzerState = BUZ_STATE_IDLE;
static uint8_t beepCount = 0;
static bool TimerCompleteBuzzer = false;
static bool InstrumetBusy = false;
static bool MemCorrptBuzzer = false;
static bool BuzzerBusy = false;

void BeepBuzzer(void)
{
	TIM_HandleTypeDef *pTim =  GetInstance_BuzzerTimer();
	if(objstcSettings.u32BuzzerEnable)
	{
		 if (!BuzzerBusy)
		  {
				BuzzerBusy = true;
				InitializeTimer(pTim , (1 * 60)/*Khz*/ , enTimerUnitHertz , en_BuzzerTimer);
				HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin,GPIO_PIN_SET);
				HAL_TIM_Base_Start_IT(pTim);
		  }
	}
}
void LongBeepBuzzer(void)
{
	 if (!BuzzerBusy)
	 {
		BuzzerBusy = true;
		TIM_HandleTypeDef *pTim =  GetInstance_BuzzerTimer();
		InitializeTimer(pTim , (1 * 3)/*Khz*/ , enTimerUnitHertz , en_BuzzerTimer);
		HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin,GPIO_PIN_SET);
		HAL_TIM_Base_Start_IT(pTim);
	 }
}
void TestTimerCompleteBuzzer(void)
{
	 if (!BuzzerBusy)
	 {
		beepCount = 0;
		BuzzerBusy = true;
		TimerCompleteBuzzer = true;
		buzzerState = BUZ_STATE_BEEP_ON;
		TIM_HandleTypeDef *pTim =  GetInstance_BuzzerTimer();
		InitializeTimer(pTim , (1 * 2)/*Khz*/ , enTimerUnitHertz , en_BuzzerTimer);
		HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin,GPIO_PIN_SET);
		HAL_TIM_Base_Start_IT(pTim);
	 }
}
void InstrumentBusyBuzz(void)
{
	 if (!BuzzerBusy)
	 {
		beepCount = 0;
		BuzzerBusy = true;
		InstrumetBusy = true;
		buzzerState = BUZ_STATE_BEEP_ON;
		TIM_HandleTypeDef *pTim =  GetInstance_BuzzerTimer();
		InitializeTimer(pTim , (1 * 10)/*Khz*/ , enTimerUnitHertz , en_BuzzerTimer);
		HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin,GPIO_PIN_SET);
		HAL_TIM_Base_Start_IT(pTim);
	 }
}

void MemoryCorruptionBuzzer(void)
{
	 if (!BuzzerBusy)
	 {
		beepCount = 0;
		BuzzerBusy = true;
		MemCorrptBuzzer = true;
		buzzerState = BUZ_STATE_BEEP_ON;
		TIM_HandleTypeDef *pTim =  GetInstance_BuzzerTimer();
		InitializeTimer(pTim , (1 * 10)/*Khz*/ , enTimerUnitHertz , en_BuzzerTimer);
		HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin,GPIO_PIN_SET);
		HAL_TIM_Base_Start_IT(pTim);
	 }
}


//void IRQ_BuzzerTimerExpiry(void)
//{
//	TIM_HandleTypeDef *pTim =  GetInstance_BuzzerTimer();
//	HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin,GPIO_PIN_RESET);
//	HAL_TIM_Base_Stop(pTim);
//}

void IRQ_BuzzerTimerExpiry(void)
{
    TIM_HandleTypeDef *pTim = GetInstance_BuzzerTimer();

    if (TimerCompleteBuzzer)
    {
        switch (buzzerState)
        {
            case BUZ_STATE_BEEP_ON:
                HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_RESET);
                buzzerState = BUZ_STATE_BEEP_OFF;
                InitializeTimer(pTim, (1 * 2)/*Khz*/, enTimerUnitHertz, en_BuzzerTimer); // Timer for beep off duration
                HAL_TIM_Base_Start_IT(pTim);
                break;

            case BUZ_STATE_BEEP_OFF:
                beepCount++;
                if (beepCount < 5)
                {
                    HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_SET);
                    buzzerState = BUZ_STATE_BEEP_ON;
                    InitializeTimer(pTim, (1 * 2)/*Khz*/, enTimerUnitHertz, en_BuzzerTimer); // Timer for beep on duration
                    HAL_TIM_Base_Start_IT(pTim);
                }
                else
                {
                    buzzerState = BUZ_STATE_IDLE;
                    HAL_TIM_Base_Stop(pTim);
                    TimerCompleteBuzzer = false;
    				BuzzerBusy = false;
                }
                break;

            default:
                HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_RESET);
                HAL_TIM_Base_Stop(pTim);
                buzzerState = BUZ_STATE_IDLE;
                TimerCompleteBuzzer = false;
				BuzzerBusy = false;
                break;
        }
    }
    else if (InstrumetBusy)
    {
        switch (buzzerState)
        {
            case BUZ_STATE_BEEP_ON:
                HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_RESET);
                buzzerState = BUZ_STATE_BEEP_OFF;
                InitializeTimer(pTim, (1 * 10)/*Khz*/, enTimerUnitHertz, en_BuzzerTimer); // Timer for beep off duration
                HAL_TIM_Base_Start_IT(pTim);
                break;

            case BUZ_STATE_BEEP_OFF:
                beepCount++;
                if (beepCount < 2)
                {
                    HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_SET);
                    buzzerState = BUZ_STATE_BEEP_ON;
                    InitializeTimer(pTim, (1 * 10)/*Khz*/, enTimerUnitHertz, en_BuzzerTimer); // Timer for beep on duration
                    HAL_TIM_Base_Start_IT(pTim);
                }
                else
                {
                    buzzerState = BUZ_STATE_IDLE;
                    HAL_TIM_Base_Stop(pTim);
                    InstrumetBusy = false;
    				BuzzerBusy = false;
                }
                break;

            default:
                HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_RESET);
                HAL_TIM_Base_Stop(pTim);
                buzzerState = BUZ_STATE_IDLE;
                InstrumetBusy = false;
				BuzzerBusy = false;
                break;
        }
    }
    else if(MemCorrptBuzzer)
    {
        switch (buzzerState)
        {
            case BUZ_STATE_BEEP_ON:
                HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_RESET);
                buzzerState = BUZ_STATE_BEEP_OFF;
                InitializeTimer(pTim, (1 * 10)/*Khz*/, enTimerUnitHertz, en_BuzzerTimer); // Timer for beep off duration
                HAL_TIM_Base_Start_IT(pTim);
                break;

            case BUZ_STATE_BEEP_OFF:
                beepCount++;
                if (beepCount < 10)
                {
                    HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_SET);
                    buzzerState = BUZ_STATE_BEEP_ON;
                    InitializeTimer(pTim, (1 * 10)/*Khz*/, enTimerUnitHertz, en_BuzzerTimer); // Timer for beep on duration
                    HAL_TIM_Base_Start_IT(pTim);
                }
                else
                {
                    buzzerState = BUZ_STATE_IDLE;
                    HAL_TIM_Base_Stop(pTim);
                    MemCorrptBuzzer = false;
    				BuzzerBusy = false;
                }
                break;

            default:
                HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin, GPIO_PIN_RESET);
                HAL_TIM_Base_Stop(pTim);
                buzzerState = BUZ_STATE_IDLE;
                MemCorrptBuzzer = false;
				BuzzerBusy = false;
                break;
        }
    }

    else
    {
    	TIM_HandleTypeDef *pTim =  GetInstance_BuzzerTimer();
    	HAL_GPIO_WritePin(Buzzer_Pin_GPIO_Port, Buzzer_Pin_Pin,GPIO_PIN_RESET);
    	HAL_TIM_Base_Stop(pTim);
		BuzzerBusy = false;
    }
}
