/*
 * Preamp.c
 *
 *  Created on: Jul 24, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "Preamp.h"
#include "TimerPWM.hpp"
#include "USBDevice.h"
#include <stdio.h>
#include <stdlib.h>
#include <TempratureSens.h>
#include "../APPL/Inc/NonVolatileMemory.h"

extern osMutexId MutexPreampSpiHandle;
extern TIM_HandleTypeDef htim6;
static bool PreampMillsDelay(uint32_t Delay);
static bool Gainchange_event = false;

bool extrapolated_ADC = false;
uint8_t Gainchange_delay = 0;


#if (!BITBANGING_PREAMP_SERIAL_MODE)
extern SPI_HandleTypeDef hspi1;
#endif
enPreampGain g_enCurrentPreampgain = en_PreampGain_OFF;
const enPreampGain g_AutoGainSelectionArray[en_PreampGain_Max] = {en_PreampGain_OFF,//0
															en_PreampGain_15,//1
															en_PreampGain_7,//2
															en_PreampGain_11,//3
															en_PreampGain_3,//4
															en_PreampGain_13,//5
															en_PreampGain_5,//6
															en_PreampGain_9,//7
															en_PreampGain_1,//8
															en_PreampGain_14,//9
															en_PreampGain_6,//10
															en_PreampGain_10,//11
															en_PreampGain_2,//12
															en_PreampGain_12,//13
															en_PreampGain_4,//14
															en_PreampGain_8};//15

void InitPreamp(void)
{
	SetPreampGain(en_PreampGain_1);
}

enPreampGain AssignGain(en_WavelengthFilter Filter)
{
	enPreampGain Gain = objstcSettings.FilterPreampGain[Filter];
	return Gain;
}

uint16_t ReadPhotometryMeasurement(enPreampGain GainIdx , bool AutogaincntrlFlag)
{
	bool PreampAdcvalid = false;
	EnableDisablePeltier(en_PhotometerTemp , en_TempCntrlDisabled);//Disable peltier
	EnableDisableIncubator(en_ChamperTemp , en_TempCntrlDisabled);//Disable peltier
	uint8_t GainArrayIndex = 0;
	uint8_t PrevGainArrayIndex = 0;


	for(;GainArrayIndex < en_PreampGain_Max ; GainArrayIndex++)//Finding filter gain position in array location
	{
		if(GainIdx == g_AutoGainSelectionArray[GainArrayIndex])
		{
			break;
		}
	}
#if (AUTOMATIC_GAIN_ENABLED)
	/***************Logic for automatic gain selection************************/
	if(true == AutogaincntrlFlag)
	{
		do
		{
			while(!PreampMillsDelay(1));
			uint16_t PreampReading = ReadPreampADC();
			uint8_t GainArrayIndex_current = 0;
			for(;GainArrayIndex_current < en_PreampGain_Max ; GainArrayIndex_current++)//Finding filter gain position in array location
			{
				if(g_enCurrentPreampgain == g_AutoGainSelectionArray[GainArrayIndex_current])
				{
					GainArrayIndex = GainArrayIndex_current;
					break;
				}
			}

			if(MAX_ADC_VALUE <= PreampReading  && GainArrayIndex > en_PreampGain_1 )
			{
				Gainchange_event = true;
				PrevGainArrayIndex = GainArrayIndex;
				GainArrayIndex--;
				if(en_PreampGain_OFF > GainArrayIndex)
				{
					GainArrayIndex = en_PreampGain_1;
					PreampAdcvalid = true;
				}
				SetPreampGain(g_AutoGainSelectionArray[GainArrayIndex]);
				HAL_Delay(500);
//				DebugOvrUsbCdc("ADC gain increased",IntToCharConverter(g_AutoGainSelectionArray[GainArrayIndex]));
			}
			else if(MIN_ADC_VALUE >= PreampReading && GainArrayIndex < en_PreampGain_15)
			{
				GainArrayIndex++;
				Gainchange_event = true;
				if(en_PreampGain_Max <= GainArrayIndex)
				{
					GainArrayIndex = en_PreampGain_15;
					PreampAdcvalid = true;
				}
				SetPreampGain(g_AutoGainSelectionArray[GainArrayIndex]);
				HAL_Delay(500);
//				DebugOvrUsbCdc("ADC gain decreased",IntToCharConverter(g_AutoGainSelectionArray[GainArrayIndex]));
			}
			else
			{
				PreampAdcvalid = true;
				DebugOvrUsbCdc("ADC,no gain change",IntToCharConverter(g_AutoGainSelectionArray[GainArrayIndex]));
			}
		}
		while(false == PreampAdcvalid);
		DebugOvrUsbCdc("ADC-OK,final gain: ",IntToCharConverter(g_enCurrentPreampgain));
	}
	else
	{
		SetPreampGain(g_AutoGainSelectionArray[GainArrayIndex]);
	}
	/***************Logic for automatic gain selection************************/
#endif

	uint16_t AdcVal = ReadPreampADC();
    EnableDisablePeltier(en_PhotometerTemp , en_TempCntrlEnabled);//Enable peltier
    EnableDisableIncubator(en_ChamperTemp , en_TempCntrlEnabled);//Disable peltier
    if(Gainchange_event == true)
    {
		if(GainArrayIndex == en_PreampGain_15 || PrevGainArrayIndex == en_PreampGain_15 )
		{
			Gainchange_delay = 10;
	    	extrapolated_ADC = true;
		}
		else if(GainArrayIndex == en_PreampGain_14)
		{
			Gainchange_delay = 5;
	    	extrapolated_ADC = true;
		}
		else if(GainArrayIndex == en_PreampGain_13)
		{
			Gainchange_delay = 5;
	    	extrapolated_ADC = true;
		}
	    else if(extrapolated_ADC != true)
	    {
	    	extrapolated_ADC = false;
	    }
		Gainchange_event = false;
    }


    return AdcVal;
}
uint16_t ReadPreampADC(void)
{
	osMutexWait(MutexPreampSpiHandle,MAX_MUTEX_WAIT_DELAY_PREAMP);
	volatile uint16_t u_n16ReadData = 0;
	HAL_GPIO_WritePin(Preamp_SPI_1_CS_Pin_GPIO_Port, Preamp_SPI_1_CS_Pin_Pin, GPIO_PIN_RESET);
	osDelay(1);//DWTDelay_us(1);
#if BITBANGING_PREAMP_SERIAL_MODE
	for(uint8_t u_n8Clk = 0; u_n8Clk < BITBANGING_PREAMP_SERIAL_MAX_CLK ; ++u_n8Clk)
	{
		volatile uint16_t u_n16TempReadBuff = 0;
		u_n16TempReadBuff = (HAL_GPIO_ReadPin(Preamp_SPI1_MISO_Pin_GPIO_Port, Preamp_SPI1_MISO_Pin_Pin));
		u_n16ReadData |= (u_n16TempReadBuff << ((BITBANGING_PREAMP_SERIAL_MAX_CLK - 1) - u_n8Clk));

		HAL_GPIO_WritePin(Preamp_SPI1_SCK_Pin_GPIO_Port, Preamp_SPI1_SCK_Pin_Pin, GPIO_PIN_SET);
		osDelay(BITBANGING_PREAMP_SERIAL_CLK_DELAY);
		HAL_GPIO_WritePin(Preamp_SPI1_SCK_Pin_GPIO_Port, Preamp_SPI1_SCK_Pin_Pin, GPIO_PIN_RESET);
		osDelay(BITBANGING_PREAMP_SERIAL_CLK_DELAY);
	}
#else
	HAL_SPI_Receive(&hspi1,(uint8_t*)&u_n16ReadData, 2, SPI1_READ_DELAY_MAX_TICKS);
#endif
	HAL_GPIO_WritePin(Preamp_SPI_1_CS_Pin_GPIO_Port, Preamp_SPI_1_CS_Pin_Pin, GPIO_PIN_SET);
    osMutexRelease(MutexPreampSpiHandle);
	return u_n16ReadData;
}

float GetCurrentgainResistanceVal(void)
{
  float ResVal = objstcSettings.fPreampGainResistanceVal[g_enCurrentPreampgain];
  return ResVal;
}
void SetPreampGain(enPreampGain Gain)
{
	uint8_t u_n8Gain = 0;
	g_enCurrentPreampgain = Gain;
	u_n8Gain = (uint8_t)Gain;
	if(u_n8Gain & 0x01)
	{
		HAL_GPIO_WritePin(GPIOE,Preamp_Gain_D_Pin_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE,Preamp_Gain_D_Pin_Pin, GPIO_PIN_RESET);
	}

	if((u_n8Gain >> 1) & 0x01)
	{
		HAL_GPIO_WritePin(GPIOE,Preamp_Gain_C_Pin_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE,Preamp_Gain_C_Pin_Pin, GPIO_PIN_RESET);
	}

	if((u_n8Gain >> 2) & 0x01)
	{
		HAL_GPIO_WritePin(GPIOE,Preamp_Gain_B_Pin_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE,Preamp_Gain_B_Pin_Pin, GPIO_PIN_RESET);
	}

	if((u_n8Gain >> 3) & 0x01)
	{
		HAL_GPIO_WritePin(GPIOE,Preamp_Gain_A_Pin_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOE,Preamp_Gain_A_Pin_Pin, GPIO_PIN_RESET);
	}
}
void CalculatePreampGainResistance(void)
{
	objstcSettings.fPreampGainResistanceVal[en_PreampGain_OFF] = 0;
	objstcSettings.fPreampGainResistanceVal[en_PreampGain_1] = objstcSettings.fGainResCalculatorBit[enPreGainResValR1];
	objstcSettings.fPreampGainResistanceVal[en_PreampGain_2] = objstcSettings.fGainResCalculatorBit[enPreGainResValR2];
	objstcSettings.fPreampGainResistanceVal[en_PreampGain_3] = ((objstcSettings.fGainResCalculatorBit[enPreGainResValR1] *
																objstcSettings.fGainResCalculatorBit[enPreGainResValR2])
														/(objstcSettings.fGainResCalculatorBit[enPreGainResValR1] +
																objstcSettings.fGainResCalculatorBit[enPreGainResValR2]));
	objstcSettings.fPreampGainResistanceVal[en_PreampGain_4] = objstcSettings.fGainResCalculatorBit[enPreGainResValR3];
	objstcSettings.fPreampGainResistanceVal[en_PreampGain_5]  = ((objstcSettings.fGainResCalculatorBit[enPreGainResValR1] *
																objstcSettings.fGainResCalculatorBit[enPreGainResValR3])
														/(objstcSettings.fGainResCalculatorBit[enPreGainResValR1] +
																objstcSettings.fGainResCalculatorBit[enPreGainResValR3]));

	objstcSettings.fPreampGainResistanceVal[en_PreampGain_6]  = ((objstcSettings.fGainResCalculatorBit[enPreGainResValR2] *
																objstcSettings.fGainResCalculatorBit[enPreGainResValR3])
														/(objstcSettings.fGainResCalculatorBit[enPreGainResValR2] +
																objstcSettings.fGainResCalculatorBit[enPreGainResValR3]));

	objstcSettings.fPreampGainResistanceVal[en_PreampGain_7]  = ((1)/((1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR1])) +
			(1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR2])) + (1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR3]))));

	objstcSettings.fPreampGainResistanceVal[en_PreampGain_8]  = objstcSettings.fGainResCalculatorBit[enPreGainResValR4];

	objstcSettings.fPreampGainResistanceVal[en_PreampGain_9] = ((objstcSettings.fGainResCalculatorBit[enPreGainResValR1] *
																objstcSettings.fGainResCalculatorBit[enPreGainResValR4])
														/(objstcSettings.fGainResCalculatorBit[enPreGainResValR1] +
																objstcSettings.fGainResCalculatorBit[enPreGainResValR4]));

	objstcSettings.fPreampGainResistanceVal[en_PreampGain_10] = ((objstcSettings.fGainResCalculatorBit[enPreGainResValR2] *
																objstcSettings.fGainResCalculatorBit[enPreGainResValR4])
														/(objstcSettings.fGainResCalculatorBit[enPreGainResValR2] +
																objstcSettings.fGainResCalculatorBit[enPreGainResValR4]));

	objstcSettings.fPreampGainResistanceVal[en_PreampGain_11]  = ((1)/((1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR1])) +
			(1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR2])) + (1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR4]))));

	objstcSettings.fPreampGainResistanceVal[en_PreampGain_12] = ((objstcSettings.fGainResCalculatorBit[enPreGainResValR3] *
																objstcSettings.fGainResCalculatorBit[enPreGainResValR4])
														/(objstcSettings.fGainResCalculatorBit[enPreGainResValR3] +
																objstcSettings.fGainResCalculatorBit[enPreGainResValR4]));

	objstcSettings.fPreampGainResistanceVal[en_PreampGain_13]  = ((1)/((1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR1])) +
			(1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR3])) + (1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR4]))));

	objstcSettings.fPreampGainResistanceVal[en_PreampGain_14]  = ((1)/((1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR2])) +
			(1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR3])) + (1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR4]))));

	objstcSettings.fPreampGainResistanceVal[en_PreampGain_15]  = ((1)/((1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR1])) +
			(1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR2])) +
			(1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR3])) + (1/(objstcSettings.fGainResCalculatorBit[enPreGainResValR4]))));
}
bool PreampMillsDelay(uint32_t Delay)
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
