/*
 * TempDS18B20.c
 *
 *  Created on: Aug 10, 2022
 *      Author: Alvin
 */
#include <TempratureSens.h>
#include "USBDevice.h"
#include "TimerPWM.hpp"
#include "../APPL/Inc/NonVolatileMemory.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>

extern TIM_HandleTypeDef htim15;//Photometer Peltier PWM Timer (Channel 2)
extern TIM_HandleTypeDef htim4;//Chamber Heater PWM Timer
extern bool bFlashStatus;
#define PHOTOMETER_PWM_TEMP_TIMER htim15
#define CHAMBER_PWM_TEMP_TIMER htim4
#define PHOTOMETER_PWM_TEMP_TIMER_CHANNEL TIM_CHANNEL_2
#define CHAMBER_PWM_TEMP_TIMER_CHANNEL TIM_CHANNEL_1
#define PELTIER_DIR_CONTROL_PORT Peltier_2_Pin_GPIO_Port
#define PELTIER_DIR_CONTROL_PIN Peltier_2_Pin_Pin

stcTempControl objstcTempControl[en_MaxTempCntrl] = {};
extern TIM_HandleTypeDef htim7;

#define H_BRIDGE_PELTIER_DRIVE (true)
#define DELAY_TIMER_ONE_WIRE htim7

#define DS18B20_PORT GPIOA
uint16_t g_DS18B20_PIN  = GPIO_PIN_0;
uint16_t CurrentPWMDuty = 0;
static void PeltierDirection(enTempCntrlStatus state);


float GetCurrentTemperature(enTempControl cntrl)
{
	return objstcTempControl[cntrl].CurrentTemp;
}
void HandlerTemperaturePIDThread(void)
{
	do
	{
		osDelay(10);
	}
	while(objstcSettings.InitFlag1 != (uint32_t)NVM_INIT_OK_FLAG && objstcSettings.InitFlag2 != NVM_INIT_OK_FLAG);

	InitializeTemperatureControl();
	float ChamperTemp = 0 , PhotoMeterTemp = 0;
	for(;;)
	{
		objstcTempControl[en_PhotometerTemp].CurrentTemp = PhotoMeterTemp = (Readtemperature(en_PhotometerTemp) - objstcSettings.fOffsetTemp[en_PhotometerTemp]);
		osDelay(100);//50 millisecond
		if(objstcSettings.fTempControlOnOff[en_ChamperTemp] == true)
		{
			objstcTempControl[en_ChamperTemp].CurrentTemp = ChamperTemp = (Readtemperature(en_ChamperTemp) - objstcSettings.fOffsetTemp[en_ChamperTemp]);
			osDelay(100);//50 millisecond
		}
		else
		{
			osDelay(100);//50 millisecond
//			SetTimerPwmDutycycle(&CHAMBER_PWM_TEMP_TIMER, CHAMBER_PWM_TEMP_TIMER_CHANNEL, (0));
		}
		/*Disable peltier if target temperature is room temperature*/
		if(ROOM_TEMP_VAL == objstcTempControl[en_PhotometerTemp].TargetTemp)
		{
			objstcTempControl[en_PhotometerTemp].Status = en_TempCntrlDisabled;
		}
		ContolPeltier(CalculatePwmDutyCycle(en_PhotometerTemp,PhotoMeterTemp));
		osDelay(100);//50 millisecond
		ControlHeater(CalculatePwmDutyCycle_Icubator(en_ChamperTemp,ChamperTemp));
		osDelay(100);//50 millisecond
	}
}
void ContolPeltier(int16_t Duty)/*Photometer peltier control*/
{
	if(0 < Duty)//above 0 means, need to heat
	{
#if (H_BRIDGE_PELTIER_DRIVE)
		PeltierDirection(en_Heating);
#endif
	}
	else if(0 > Duty)//Less than 0 means, need to cool
	{
#if (H_BRIDGE_PELTIER_DRIVE)
//		Duty = 100 + Duty;/*Suppose duty = (-20)  , then 100 + Duty = 80*/
//		Duty = (-1.5)* Duty;
		PeltierDirection(en_Cooling);

		if(objstcTempControl[en_PhotometerTemp].TargetTemp == 25)
		{
			Duty = (-1.5)* Duty;
			if(Duty > 30)
			{
				Duty = 100;
			}
		}
		else
		{
			Duty = (-1)* Duty;
		}

#else
	    Duty = 0;
#endif
	}


	if(en_TempCntrlDisabled == objstcTempControl[en_PhotometerTemp].Status)
	{
		Duty = 0;
	}
	else if(en_TempCntrlEnabled == objstcTempControl[en_PhotometerTemp].Status)
	{
		Duty = 0;
	}
//	else
//	{
		CurrentPWMDuty = Duty;
		SetTimerPwmDutycycle(&PHOTOMETER_PWM_TEMP_TIMER, PHOTOMETER_PWM_TEMP_TIMER_CHANNEL, (Duty));
//	}

//	if(0 == Duty)
//	{
//		HAL_GPIO_WritePin(PhotometryPeltierEnableLed_GPIO_Port,
//				PhotometryPeltierEnableLed_Pin, GPIO_PIN_RESET);/*LED update*/
//	}
//	else
//	{
//		HAL_GPIO_WritePin(PhotometryPeltierEnableLed_GPIO_Port,
//				PhotometryPeltierEnableLed_Pin, GPIO_PIN_SET);/*LED update*/
//	}
}
void PeltierDirection(enTempCntrlStatus state)
{
	if(en_Heating == state)
	{
		HAL_GPIO_WritePin(PELTIER_DIR_CONTROL_PORT,
				PELTIER_DIR_CONTROL_PIN, GPIO_PIN_RESET);
		ControlPeltierfan(false);
	}
	else if(en_Cooling == state)
	{
		HAL_GPIO_WritePin(PELTIER_DIR_CONTROL_PORT,
				PELTIER_DIR_CONTROL_PIN, GPIO_PIN_SET);
		ControlPeltierfan(true);/*Enable fan only when peltier is cooling*/
	}
}
void ControlPeltierfan(bool State)
{
	HAL_GPIO_WritePin(Peltier_FAN_Pin_GPIO_Port,Peltier_FAN_Pin_Pin,(GPIO_PinState)State);
}
void ControlHeater(int16_t Duty)/*Chamber heater control*/
{
	if(en_TempCntrlDisabled == objstcTempControl[en_ChamperTemp].Status)
	{
		Duty = 0;
	}
	else if(en_TempCntrlEnabled == objstcTempControl[en_ChamperTemp].Status)
	{
		if(objstcSettings.fTempControlOnOff[en_ChamperTemp] == true)
		{
			if(Duty > 0 && Duty < 25)
			{
				SetTimerPwmDutycycle(&CHAMBER_PWM_TEMP_TIMER, CHAMBER_PWM_TEMP_TIMER_CHANNEL, (25));
			}
			else if(Duty > 25)
			{
				SetTimerPwmDutycycle(&CHAMBER_PWM_TEMP_TIMER, CHAMBER_PWM_TEMP_TIMER_CHANNEL, (Duty));
			}
			else
			{
				Duty = 0;
				SetTimerPwmDutycycle(&CHAMBER_PWM_TEMP_TIMER, CHAMBER_PWM_TEMP_TIMER_CHANNEL, (Duty));
			}
		}
		else
		{
			SetTimerPwmDutycycle(&CHAMBER_PWM_TEMP_TIMER, CHAMBER_PWM_TEMP_TIMER_CHANNEL, (0));
		}
	}
}
void EnableDisablePeltier(enTempControl Cntrl , enTempCntrlStatus status)
{
	//HAL_GPIO_WritePin(Peltier_Gnd_Pin_GPIO_Port,Peltier_Gnd_Pin_Pin, Dir);
	if(Cntrl == en_PhotometerTemp)
	{
		objstcTempControl[en_PhotometerTemp].Status = status;
		if(en_TempCntrlDisabled == status)
		{
			__HAL_TIM_DISABLE(&PHOTOMETER_PWM_TEMP_TIMER);
		}
		else if(en_TempCntrlEnabled == status)
		{
			__HAL_TIM_ENABLE(&PHOTOMETER_PWM_TEMP_TIMER);
			objstcTempControl[en_PhotometerTemp].Status = en_Heating;
		}
	}
}

void EnableDisableIncubator(enTempControl Cntrl , enTempCntrlStatus status)
{
	//HAL_GPIO_WritePin(Peltier_Gnd_Pin_GPIO_Port,Peltier_Gnd_Pin_Pin, Dir);
	if(Cntrl == en_ChamperTemp)
	{
		objstcTempControl[en_ChamperTemp].Status = status;
		if(en_TempCntrlDisabled == status)
		{
			__HAL_TIM_DISABLE(&CHAMBER_PWM_TEMP_TIMER);
		}
		else if(en_TempCntrlEnabled == status)
		{
			__HAL_TIM_ENABLE(&CHAMBER_PWM_TEMP_TIMER);
			objstcTempControl[en_ChamperTemp].Status = en_TempCntrlEnabled;
		}
	}
}

int16_t CalculatePwmDutyCycle(enTempControl enCntrl , float CurrentTemperature)
{
		int16_t Duty = 0;

		float kP = 0;
		float kD = 0;
		static float kI = 0;
		static float PrevError = 0;
		float TargetTemp=0;

		float integral_windup_limit = 20; // Define an appropriate limit for anti-windup
		float error = 0;
		float alpha = 0.1; // Smoothing factor for the derivative term
	    static float PrevDerivative = 0;
	    float integral = 0;

	    static float Ts = 0.1, Outmin = -100, Outmax = 100, antiwinduperror = 0.5;
	    bool windup = false;
	    static float  prev_input = 0, Ki_sum = 0, prev_error = 0;

		TargetTemp = objstcTempControl[enCntrl].TargetTemp + objstcSettings.fTemperatureOffset[enCntrl];

		if(CurrentTemperature > (40.0))// elimating wrong cuurent termoerature values
		{
			return Duty = 0;
		}
//		error = TargetTemp - CurrentTemperature;
//		kP = error;
//		kI = (kI + kP)/1.2 ;//integral
//		kD = (error  - PrevError);
//		PrevError = error;
//		Duty = (int16_t)(kP * objstcTempControl[enCntrl].pid_P) + (kI * objstcTempControl[enCntrl].pid_I) + (kD * objstcTempControl[enCntrl].pid_D);

//		error = TargetTemp - CurrentTemperature;
//
//		kP = error * objstcTempControl[enCntrl].pid_P;
//
//		if(( error - PrevError == 0) && (error != 0))
//		{
//			kI += error;
//		    if (kI > integral_windup_limit) kI = integral_windup_limit;
//		    if (kI < -integral_windup_limit) kI = -integral_windup_limit;
//		    integral  = kI * objstcTempControl[enCntrl].pid_I;
//		}
//		else
//		{
////			kI = 0;
//			integral  = kI * objstcTempControl[enCntrl].pid_I;
//		}
//	    float derivative = error - PrevError;
//		kD = objstcTempControl[enCntrl].pid_D * (alpha * derivative + (1 - alpha) * PrevDerivative);
//		PrevDerivative = derivative;
//		PrevError = error;
//
//		Duty = (int16_t)(kP) + (int16_t)(integral) + (int16_t)(kD);
			float Kp = objstcTempControl[enCntrl].pid_P;
			float Ki = objstcTempControl[enCntrl].pid_I;
			float Kd = objstcTempControl[enCntrl].pid_D;

			error = TargetTemp - CurrentTemperature;

	        if (fabs(error) < 0.1 || fabs(error) < -0.1 )
	        {
	        	windup = false;
	        }
	        else
	        {
	        	windup = true;
	        }
		   if (windup)
		    {
		        if (fabs(error) > antiwinduperror)
		        {
		        	Duty = (Kp * error) + (Kd * (CurrentTemperature - prev_input) / Ts);
		        }
		        else
		        {
		        	Duty = (Kp * error) + (Ki * Ki_sum * Ts) - ((Kd * (CurrentTemperature - prev_input)) / Ts);
		        }
		    }
		    else
		    {
		    	Duty = (Kp * error) + (Ki * Ki_sum * Ts) - ((Kd * (CurrentTemperature - prev_input)) / Ts);
		    }
		    if((error > 1) || (error < -1))
		    {
		    	Ki_sum = 0;
		    }

		    Ki_sum += error * Ts;

		    if (Ki_sum > 100)
		    {
		    	Ki_sum = 100;
		    }
		    if (Ki_sum < -100)
		    {
		    	Ki_sum = -100;
		    }
		    prev_input = CurrentTemperature;
		    prev_error = error;

		if(MAX_VALID_TEMPERATURE < CurrentTemperature)
		{
			Duty = 0;
			ControlPeltierfan(true);/*if this happens means peltier cooling the block so fan shouldbe on */
		}
		else if(100 < Duty)
		{
			Duty = 100;
		}
		else if((-100) > Duty)
		{
			Duty = (-100);
		}
		return Duty;
}

int16_t CalculatePwmDutyCycle_Icubator(enTempControl enCntrl , float CurrentTemperature)
{
		int16_t Duty_In = 0;
		float kP = 0;
		float kD = 0;
		static float kI = 0;
		static float PrevError = 0;
		float TargetTemp_In=0;

		float integral_windup_limit = 20; // Define an appropriate limit for anti-windup
		float error_In = 0;
		float alpha = 0.1; // Smoothing factor for the derivative term
	    static float PrevDerivative = 0;
	    float integral = 0;

	    static float Ts_In = 0.1, Outmin = -100, Outmax = 100, antiwinduperror_In = 0.5;
	    bool windup_In = false;
	    static float  prev_input_In = 0, Ki_sum_In = 0, prev_error_In = 0;

		TargetTemp_In = objstcTempControl[enCntrl].TargetTemp + objstcSettings.fTemperatureOffset[enCntrl];

		if(CurrentTemperature > (MAX_VALID_TEMPERATURE))// elimating wrong cuurent termoerature values
		{
			return Duty_In = 0;
		}
//		error = TargetTemp - CurrentTemperature;
//		kP = error;
//		kI = (kI + kP)/1.2 ;//integral
//		kD = (error  - PrevError);
//		PrevError = error;
//		Duty = (int16_t)(kP * objstcTempControl[enCntrl].pid_P) + (kI * objstcTempControl[enCntrl].pid_I) + (kD * objstcTempControl[enCntrl].pid_D);

//		error = TargetTemp - CurrentTemperature;
//
//		kP = error * objstcTempControl[enCntrl].pid_P;
//
//		if(( error - PrevError == 0) && (error != 0))
//		{
//			kI += error;
//		    if (kI > integral_windup_limit) kI = integral_windup_limit;
//		    if (kI < -integral_windup_limit) kI = -integral_windup_limit;
//		    integral  = kI * objstcTempControl[enCntrl].pid_I;
//		}
//		else
//		{
////			kI = 0;
//			integral  = kI * objstcTempControl[enCntrl].pid_I;
//		}
//	    float derivative = error - PrevError;
//		kD = objstcTempControl[enCntrl].pid_D * (alpha * derivative + (1 - alpha) * PrevDerivative);
//		PrevDerivative = derivative;
//		PrevError = error;
//
//		Duty = (int16_t)(kP) + (int16_t)(integral) + (int16_t)(kD);
			float Kp = objstcTempControl[enCntrl].pid_P;
			float Ki = objstcTempControl[enCntrl].pid_I;
			float Kd = objstcTempControl[enCntrl].pid_D;

			error_In = TargetTemp_In - CurrentTemperature;

	        if (fabs(error_In) < 0.1 || fabs(error_In) < -0.1 )
	        {
	        	windup_In = false;
	        }
	        else
	        {
	        	windup_In = true;
	        }
		   if (windup_In)
		    {
		        if (fabs(error_In) > antiwinduperror_In)
		        {
		        	Duty_In = (Kp * error_In) + (Kd * (CurrentTemperature - prev_input_In) / Ts_In);
		        }
		        else
		        {
		        	Duty_In = (Kp * error_In) + (Ki * Ki_sum_In * Ts_In) - ((Kd * (CurrentTemperature - prev_input_In)) / Ts_In);
		        }
		    }
		    else
		    {
		    	Duty_In = (Kp * error_In) + (Ki * Ki_sum_In * Ts_In) - ((Kd * (CurrentTemperature - prev_input_In)) / Ts_In);
		    }

		    if((error_In > 1) || (error_In < -1))
		    {
		    	Ki_sum_In = 0;
		    }

		    Ki_sum_In += error_In * Ts_In;

		    if (Ki_sum_In > 100)
		    {
		    	Ki_sum_In = 100;
		    }
		    if (Ki_sum_In < -100)
		    {
		    	Ki_sum_In = -100;
		    }
		    prev_input_In = CurrentTemperature;
		    prev_error_In = error_In;

		if(MAX_VALID_TEMPERATURE < CurrentTemperature)
		{
			Duty_In = 0;
//			ControlPeltierfan(true);/*if this happens means peltier cooling the block so fan shouldbe on */
		}
		else if(100 < Duty_In)
		{
			Duty_In = 100;
		}
		else if((-100) > Duty_In)
		{
			Duty_In = (-100);
		}
		return Duty_In;
}


void InitializeTemperatureControl(void)
{
	InitializeTimer(&PHOTOMETER_PWM_TEMP_TIMER , (20 * 1000)/*Khz*/ , enTimerUnitHertz , en_PeltierHeaterTimer);
	InitializeTimer(&CHAMBER_PWM_TEMP_TIMER , (1 * 1000)/*Khz*/ , enTimerUnitHertz , en_ChamberHeaterTimer);
	SetTimerPwmDutycycle(&PHOTOMETER_PWM_TEMP_TIMER, PHOTOMETER_PWM_TEMP_TIMER_CHANNEL, (0));
	SetTimerPwmDutycycle(&CHAMBER_PWM_TEMP_TIMER, CHAMBER_PWM_TEMP_TIMER_CHANNEL, (0));
	HAL_TIM_Base_Start(&DELAY_TIMER_ONE_WIRE);
	objstcTempControl[en_PhotometerTemp].TargetTemp = objstcSettings.fTargetTemp[en_PhotometerTemp];
	objstcTempControl[en_PhotometerTemp].pid_P = objstcSettings.fPID_kP[en_PhotometerTemp];
	objstcTempControl[en_PhotometerTemp].pid_I = objstcSettings.fPID_kI[en_PhotometerTemp];
	objstcTempControl[en_PhotometerTemp].pid_D = objstcSettings.fPID_kD[en_PhotometerTemp];

	objstcTempControl[en_ChamperTemp].Status = en_TempCntrlEnabled;
	objstcTempControl[en_ChamperTemp].TargetTemp = objstcSettings.fTargetTemp[en_ChamperTemp];
	objstcTempControl[en_ChamperTemp].pid_P = objstcSettings.fPID_kP[en_ChamperTemp];
	objstcTempControl[en_ChamperTemp].pid_I = objstcSettings.fPID_kI[en_ChamperTemp];
	objstcTempControl[en_ChamperTemp].pid_D = objstcSettings.fPID_kD[en_ChamperTemp];

	HAL_TIM_PWM_Start(&PHOTOMETER_PWM_TEMP_TIMER, PHOTOMETER_PWM_TEMP_TIMER_CHANNEL);
	HAL_TIM_PWM_Start(&CHAMBER_PWM_TEMP_TIMER, CHAMBER_PWM_TEMP_TIMER_CHANNEL);
}
float Readtemperature(enTempControl enCntrl)
{
	uint8_t  Temp_byte1 = 0, Temp_byte2 = 0;
	float Temperature = 0;

	if(en_ChamperTemp == enCntrl)
	{
		g_DS18B20_PIN = GPIO_PIN_1;
	}
	else
	{
		g_DS18B20_PIN = GPIO_PIN_0;
	}
	uint8_t Presence = DS18B20_Start ();
	if(!Presence)
	{
		return (4094);//returning high value for worst case scenario
	}
	delayUs(&DELAY_TIMER_ONE_WIRE,1000);
	DS18B20_Write (0xCC);  // skip ROM
	DS18B20_Write (0x44);  // convert t
	delayUs(&DELAY_TIMER_ONE_WIRE,800);
	Presence = DS18B20_Start ();
	if(!Presence)
	{
		return (4094);//returning high value for worst case scenario
	}
	delayUs(&DELAY_TIMER_ONE_WIRE,1000);
	DS18B20_Write (0xCC);  // skip ROM
	DS18B20_Write (0xBE);  // Read Scratch-pad
	Temp_byte1 = DS18B20_Read();
	Temp_byte2 = DS18B20_Read();
	uint16_t TEMP = (Temp_byte2<<8)|Temp_byte1;
	Temperature = (float)TEMP/16;
	return(Temperature);
}

void SetGpioPin_Output (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void SetGpioPin_Input (GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

uint8_t DS18B20_Start (void)
{
	volatile uint8_t Response = false;
	bool Sensor_Pulse = false;
	uint32_t read_count = 0;
	SetGpioPin_Output(DS18B20_PORT, g_DS18B20_PIN);   // set the pin as output
	HAL_GPIO_WritePin (DS18B20_PORT, g_DS18B20_PIN, GPIO_PinState(0));  // pull the pin low
	delayUs(&DELAY_TIMER_ONE_WIRE,600);   // delay according to datasheet

	SetGpioPin_Input(DS18B20_PORT, g_DS18B20_PIN);    // set the pin as input
	delayUs(&DELAY_TIMER_ONE_WIRE,70);    // delay according to datasheet

	do
	{
		if (!(HAL_GPIO_ReadPin (DS18B20_PORT, g_DS18B20_PIN)))
		{
			 Response = (true);    // if the pin is low i.e the presence pulse is detected
			 Sensor_Pulse = true;
		}
		else
		{
			Response = (false);
		}
		read_count++;
		delayUs(&DELAY_TIMER_ONE_WIRE,10);
	}
	while(Sensor_Pulse == false && read_count < 20);
	delayUs(&DELAY_TIMER_ONE_WIRE,480); // 480 us delay totally.

	return Response;
}

void DS18B20_Write (uint8_t data)
{
	SetGpioPin_Output(DS18B20_PORT, g_DS18B20_PIN);  // set as output
	delayUs(&DELAY_TIMER_ONE_WIRE,10);  // wait for 1 us
	for (int i=0; i<8; i++)
	{

		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1

			SetGpioPin_Output(DS18B20_PORT, g_DS18B20_PIN);  // set as output
			HAL_GPIO_WritePin (DS18B20_PORT, g_DS18B20_PIN, GPIO_PinState(0));  // pull the pin LOW
			delayUs(&DELAY_TIMER_ONE_WIRE,6);  // wait for 1 us

			SetGpioPin_Input(DS18B20_PORT, g_DS18B20_PIN);  // set as input
			delayUs(&DELAY_TIMER_ONE_WIRE,64);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0

			SetGpioPin_Output(DS18B20_PORT, g_DS18B20_PIN);
			HAL_GPIO_WritePin (DS18B20_PORT, g_DS18B20_PIN, GPIO_PinState(0));  // pull the pin LOW
			delayUs(&DELAY_TIMER_ONE_WIRE,60);  // wait for 60 us

			SetGpioPin_Input(DS18B20_PORT, g_DS18B20_PIN);
			delayUs(&DELAY_TIMER_ONE_WIRE,10);
		}
	}
}

uint8_t DS18B20_Read (void)
{
	volatile uint8_t value=0;

	SetGpioPin_Input(DS18B20_PORT, g_DS18B20_PIN);

	for (int i=0;i<8;i++)
	{
		SetGpioPin_Output(DS18B20_PORT, g_DS18B20_PIN);   // set as output

		HAL_GPIO_WritePin (DS18B20_PORT, g_DS18B20_PIN, GPIO_PinState(0));  // pull the data pin LOW
		delayUs(&DELAY_TIMER_ONE_WIRE,2);  // wait for > 1us

		SetGpioPin_Input(DS18B20_PORT, g_DS18B20_PIN);  // set as input
		delayUs(&DELAY_TIMER_ONE_WIRE,10);  // wait for > 1us
		if (HAL_GPIO_ReadPin (DS18B20_PORT, g_DS18B20_PIN))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		delayUs(&DELAY_TIMER_ONE_WIRE,50);  // wait for 60 us
	}
	return value;
}
