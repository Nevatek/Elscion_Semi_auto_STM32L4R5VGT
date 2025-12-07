/*
 * system.hpp
 *
 *  Created on: Jan 3, 2024
 *      Author: Alvin
 */

#ifndef INC_SYSTEM_HPP_
#define INC_SYSTEM_HPP_



TIM_HandleTypeDef* GetInstance_BuzzerTimer(void);
UART_HandleTypeDef* GetInstance_PrinterUart(void);
#endif /* INC_SYSTEM_HPP_ */
