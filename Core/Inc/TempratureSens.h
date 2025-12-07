/*
 * TempDS18B20.h
 *
 *  Created on: Aug 10, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#ifndef INC_TEMPRATURESENS_H_
#define INC_TEMPRATURESENS_H_

#define MAX_VALID_TEMPERATURE (45)
#define PELTIER_MASTER_FREQUENCY (20000)//Hz
#define HEATER_MASTER_FREQUENCY (20000)//Hz
#define MAX_HEATER_PWM_VALUE (100)
#define ROOM_TEMP_VAL float(0)
#define IDEAL_TEMP_TOLERANCE (0)

typedef enum
{
	en_Heating = 0,
	en_Cooling,
	en_TempCntrlDisabled,
	en_TempCntrlEnabled
}enTempCntrlStatus;

typedef enum
{
	en_ChamperTemp = 0,
	en_PhotometerTemp,
	en_MaxTempCntrl
}enTempControl;

typedef struct
{
	enTempCntrlStatus Status;
	float pid_P;
	float pid_I;
	float pid_D;
	float TargetTemp;
	float CurrentTemp;
}stcTempControl;

extern stcTempControl objstcTempControl[en_MaxTempCntrl];

float GetCurrentTemperature(enTempControl cntrl);
void HandlerTemperaturePIDThread(void);
float Readtemperature(enTempControl enCntrl);
void SetGpioPin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void SetGpioPin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t DS18B20_Start (void);
void DS18B20_Write (uint8_t data);
uint8_t DS18B20_Read (void);
void InitializeTemperatureControl(void);
int16_t CalculatePwmDutyCycle(enTempControl enCntrl , float CurrentTemperature);
int16_t CalculatePwmDutyCycle_Icubator(enTempControl enCntrl , float CurrentTemperature);
void ControlHeater(int16_t Duty);
void ContolPeltier(int16_t Duty);
void EnableDisablePeltier(enTempControl Cntrl , enTempCntrlStatus status);
void EnableDisableIncubator(enTempControl Cntrl , enTempCntrlStatus status);
void ControlPeltierfan(bool State);
#endif /* INC_TEMPRATURESENS_H_ */
