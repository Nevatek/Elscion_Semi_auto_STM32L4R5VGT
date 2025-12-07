/*
 * Autowash.cpp
 *
 *  Created on: Jan 8, 2024
 *      Author: Alvin
 */
#include "main.h"
#include "Autowash.hpp"

bool Read_AutowashPin(void)
{
	bool bStatus = false;
	GPIO_PinState pinState = HAL_GPIO_ReadPin(AutoWashDoorFB_Pin_GPIO_Port, AutoWashDoorFB_Pin_Pin);
	return bStatus = (bool)pinState;
}

bool Read_FluidicSensorPin(void)
{
	bool bStatus = false;
	GPIO_PinState pinState = HAL_GPIO_ReadPin(LiquidDetection_Pin_GPIO_Port, LiquidDetection_Pin_Pin);
	return bStatus = (bool)pinState;
}



