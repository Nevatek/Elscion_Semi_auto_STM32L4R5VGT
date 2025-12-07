/*
 * Preamp.h
 *
 *  Created on: Jul 24, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "FilterMotor.h"
#ifndef INC_PREAMP_H_
#define INC_PREAMP_H_

#define AUTOMATIC_GAIN_ENABLED (1)

#define SPI1_READ_DELAY_MAX_TICKS 5

#define BITBANGING_PREAMP_SERIAL_MODE (1)
#define BITBANGING_PREAMP_SERIAL_MAX_CLK (14)
#define BITBANGING_PREAMP_SERIAL_CLK_DELAY (1)

#define MAX_MUTEX_WAIT_DELAY_PREAMP (1000)

#define DELAY_INTERVEL_PREAMP_ADC (100)//Milli seconds

#define MAX_ADC_VALUE 4000 //(12 bits)
#define MIN_ADC_VALUE 300 //(12 bits)

#define DEF_PRE_GAIN_RES_B0 (6.9)
#define DEF_PRE_GAIN_RES_B1 (20)
#define DEF_PRE_GAIN_RES_B2 (100)
#define DEF_PRE_GAIN_RES_B3 (600)

typedef enum//change gain oder
{
	en_PreampGain_OFF = 0,
	en_PreampGain_1,
	en_PreampGain_2,
	en_PreampGain_3,
	en_PreampGain_4,
	en_PreampGain_5,
	en_PreampGain_6,
	en_PreampGain_7,
	en_PreampGain_8,
	en_PreampGain_9,
	en_PreampGain_10,
	en_PreampGain_11,
	en_PreampGain_12,
	en_PreampGain_13,
	en_PreampGain_14,
	en_PreampGain_15,
	en_PreampGain_Max //Not using
}enPreampGain;

void InitPreamp(void);
void SetPreampGain(enPreampGain Gain);
float GetCurrentgainResistanceVal(void);
void CalculatePreampGainResistance(void);
uint16_t ReadPreampADC(void);
void HandlerThreadPreampReading(void);
enPreampGain AssignGain(en_WavelengthFilter Filter);
uint16_t ReadPhotometryMeasurement(enPreampGain GainIdx , bool AutogaincntrlFlag);

#endif /* INC_PREAMP_H_ */
