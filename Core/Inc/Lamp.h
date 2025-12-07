/*
 * Lamp.h
 *
 *  Created on: Jul 14, 2022
 *      Author: Alvin
 */

#ifndef INC_LAMP_H_
#define INC_LAMP_H_

#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"

#define MAX_LAMP_HOURS (2000)

typedef enum
{
	en_LampDisable = 0,
	en_LampEnable = 1,
}enLamp;

void EnableDisableLamp(enLamp enStatus);
void Lamp_Task(void);
uint32_t Get_LampWorkHours(void);
void Reset_LampWorkHours(void);
#endif /* INC_LAMP_H_ */
