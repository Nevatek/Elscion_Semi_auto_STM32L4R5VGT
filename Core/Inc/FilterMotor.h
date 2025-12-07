/*
 * motor.h
 *
 *  Created on: Aug 2, 2022
 *      Author: Alvin
 */

#ifndef INC_FILTERMOTOR_H_
#define INC_FILTERMOTOR_H_


typedef enum
{
	en_FilterMotorIdle = 0,
	en_FilterMotorBusy = 1,
	en_FilterMotorHomeError
}enFilterMotorStatus;

typedef enum
{
	en_StepperFullstep = 1,
	en_StepperHalfstep = 2,
	en_StepperBy4step = 4,
	en_StepperBy8step = 8,
	en_StepperBy16step = 16,
	en_StepperBy32step = 32,
}enMicroStepps;

typedef enum
{
	en_FilterMotorLeft = 0,
	en_FilterMotorRight,
	en_FilterMotorHome,
}enFilterMotorDirection;

typedef enum
{
	enFilterMotorEnable = 0,
	enFilterMotorDisable,
}enFilterMotorEngage;

typedef enum
{
	en_FilterHome = 0,/*no filter*/
	en_Filter340,/*1*/
	en_Filter405,/*2*/
//	en_Filter450,/*3*/
	en_Filter505,/*4*/
	en_Filter546,/*5*/
	en_Filter578,/*6*/
	en_Filter630,/*7*/
//	en_Filter670,/*8*/
	en_FilterBlack,/*no filter*/
	en_FilterMax,
}en_WavelengthFilter;

#define MICRO_STEP_USED en_StepperBy32step
#define OFFSET_FILTER_HOME_TO_LIGHT_PATH (19)
#define FILTER_STEPPER_FULL_STEP_ANGLE (1.8)
//#define FILTERMOTOR_DEFAULT_RPM (100)
#define FILTER_MOTOR_HOME_MAX_ANGLE (90 + (2 * 360))
#define FILTER_MOTOR_HOME_MAX_STEPS ((360 / FILTER_STEPPER_FULL_STEP_ANGLE) * MICRO_STEP_USED)
#define DEFAULT_FILTER_MOTOR_RPM (60)
#define FILTER_ANGLE_OFFSET_FROM_HOME (45)

void CallBackFilterMotorHomeExternalint(void);
uint8_t SetFilterMotorSpeedRPM(enMicroStepps enUs , uint16_t n16RPM);
void RotateFilterMotor(uint32_t Steps);
void InitalizeFilterMotor(void);
void EnableDisableFilterMotor(enFilterMotorEngage enStatus);
void IRQ_FilterMotorStep(void);
void SetFilterMotorDirection(enFilterMotorDirection enDir);
void RotateFilterMotorAngle(float angle , enFilterMotorDirection enDir);
uint8_t ReadFilterWheelSlotSens(void);
enFilterMotorStatus ReadFilterMotorStatus(void);
void SelectFilter(en_WavelengthFilter Filter);
en_WavelengthFilter GetCurrentFilter(void);
#endif /* INC_FILTERMOTOR_H_ */
