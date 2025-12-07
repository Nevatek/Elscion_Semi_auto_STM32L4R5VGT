/*
 * DcMotor.h
 *
 *  Created on: Jul 14, 2022
 *      Author: Alvin
 */

#ifndef INC_DCMOTOR_H_
#define INC_DCMOTOR_H_

#define PUMP_GPIO (0)
#define PUMP_PWM (1)
#define PUMP_CHANNEL_TYPE PUMP_PWM
#define DC_MOTOR_MASTER_FREQUENCY (65)//Hz
#define DEFAULT_DC_MOTOR_DUTY_ON (75)
#define DEFAULT_AIRGAP_VOLEUME (90)
#define DEFAULT_WASH_VOL_UL (500)
#define MAX_ASP_VOL_MS (13500)
#define DEFAULT_PUMP_CONSTANT (2.7)
typedef enum
{
	enPump_Off = 0,
	enPump_Running,
	enPump_AspCompleted,
}en_PumpStatus;

typedef enum
{
	enDcMotorLeft = 0,
	enDcMotorRight,
	enMotorDirMax,
}en_DcMotorDirection;

typedef enum
{
	enDcMotorDisable = 0,
	enDcMotorEnable
}enMotorEngageStatus;

#if(PUMP_CHANNEL_TYPE == PUMP_GPIO)
void RotateDcMotor(enMotorEngageStatus State , en_DcMotorDirection Dir);
void StopDcMotor(void);
void AspiratePump(uint16_t n16Vol);
void InitializeDcMotor(void);
#else
void InitializeDcMotor(void);
void RotateDcMotor(en_DcMotorDirection nDirection, uint8_t DutyCycle);
void StopDcMotor(void);
void Aspirate_Pump(uint16_t n16Vol);
en_PumpStatus Pump_RunTask(void);
#endif
#endif /* INC_DCMOTOR_H_ */
