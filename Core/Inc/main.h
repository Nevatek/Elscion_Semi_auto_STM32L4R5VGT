/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Preamp_Gain_B_Pin_Pin GPIO_PIN_2
#define Preamp_Gain_B_Pin_GPIO_Port GPIOE
#define Preamp_Gain_C_Pin_Pin GPIO_PIN_3
#define Preamp_Gain_C_Pin_GPIO_Port GPIOE
#define Preamp_Gain_D_Pin_Pin GPIO_PIN_4
#define Preamp_Gain_D_Pin_GPIO_Port GPIOE
#define USB_power_en_Pin GPIO_PIN_5
#define USB_power_en_GPIO_Port GPIOE
#define PhotometerTemp_Pin_Pin GPIO_PIN_0
#define PhotometerTemp_Pin_GPIO_Port GPIOA
#define ChamberTemp_pin_Pin GPIO_PIN_1
#define ChamberTemp_pin_GPIO_Port GPIOA
#define Peltier_2_Pin_Pin GPIO_PIN_2
#define Peltier_2_Pin_GPIO_Port GPIOA
#define LampEnable_Pin GPIO_PIN_3
#define LampEnable_GPIO_Port GPIOA
#define Preamp_SPI1_SCK_Pin_Pin GPIO_PIN_5
#define Preamp_SPI1_SCK_Pin_GPIO_Port GPIOA
#define Preamp_SPI1_MISO_Pin_Pin GPIO_PIN_6
#define Preamp_SPI1_MISO_Pin_GPIO_Port GPIOA
#define Preamp_SPI_1_CS_Pin_Pin GPIO_PIN_4
#define Preamp_SPI_1_CS_Pin_GPIO_Port GPIOC
#define PUMP_En1_Pin GPIO_PIN_5
#define PUMP_En1_GPIO_Port GPIOC
#define DC_PUMP_PWM_CHANNEL_Pin GPIO_PIN_0
#define DC_PUMP_PWM_CHANNEL_GPIO_Port GPIOB
#define PUMP_Mode_Pin GPIO_PIN_1
#define PUMP_Mode_GPIO_Port GPIOB
#define AspirartionLed_White_Pin GPIO_PIN_7
#define AspirartionLed_White_GPIO_Port GPIOE
#define AspirartionLed_Red_Pin GPIO_PIN_8
#define AspirartionLed_Red_GPIO_Port GPIOE
#define FM_Ext_HomePin_Pin GPIO_PIN_9
#define FM_Ext_HomePin_GPIO_Port GPIOE
#define FM_Dir_Pin_Pin GPIO_PIN_10
#define FM_Dir_Pin_GPIO_Port GPIOE
#define FM_Reset_Pin_Pin GPIO_PIN_11
#define FM_Reset_Pin_GPIO_Port GPIOE
#define FM_Home_Inp_Pin_Pin GPIO_PIN_12
#define FM_Home_Inp_Pin_GPIO_Port GPIOE
#define FM_Step_Pin_Pin GPIO_PIN_13
#define FM_Step_Pin_GPIO_Port GPIOE
#define FM_Sleep_Pin_Pin GPIO_PIN_14
#define FM_Sleep_Pin_GPIO_Port GPIOE
#define FM_Enable_Pin_Pin GPIO_PIN_15
#define FM_Enable_Pin_GPIO_Port GPIOE
#define Peltier_FAN_Pin_Pin GPIO_PIN_13
#define Peltier_FAN_Pin_GPIO_Port GPIOB
#define ThreeWayValve_Pin_Pin GPIO_PIN_14
#define ThreeWayValve_Pin_GPIO_Port GPIOB
#define Peltier_Pwm_Pin_Pin GPIO_PIN_15
#define Peltier_Pwm_Pin_GPIO_Port GPIOB
#define Printer_Uart_TX_Pin GPIO_PIN_8
#define Printer_Uart_TX_GPIO_Port GPIOD
#define Printer_Uart_RX_Pin GPIO_PIN_9
#define Printer_Uart_RX_GPIO_Port GPIOD
#define HeaterConrol_Pin_Pin GPIO_PIN_12
#define HeaterConrol_Pin_GPIO_Port GPIOD
#define AspirationSwInt_Pin_Pin GPIO_PIN_13
#define AspirationSwInt_Pin_GPIO_Port GPIOD
#define AspirationSwInt_Pin_EXTI_IRQn EXTI15_10_IRQn
#define AspirartionLed_Blue_Pin GPIO_PIN_14
#define AspirartionLed_Blue_GPIO_Port GPIOD
#define AutoWashDoorFB_Pin_Pin GPIO_PIN_15
#define AutoWashDoorFB_Pin_GPIO_Port GPIOD
#define LiquidDetection_Pin_Pin GPIO_PIN_8
#define LiquidDetection_Pin_GPIO_Port GPIOA
#define Display_Tx_Pin GPIO_PIN_9
#define Display_Tx_GPIO_Port GPIOA
#define Display_Rx_Pin GPIO_PIN_10
#define Display_Rx_GPIO_Port GPIOA
#define Flash_WP_Pin GPIO_PIN_4
#define Flash_WP_GPIO_Port GPIOD
#define Barcode_Tx_Pin GPIO_PIN_5
#define Barcode_Tx_GPIO_Port GPIOD
#define Barcode_Rx_Pin GPIO_PIN_6
#define Barcode_Rx_GPIO_Port GPIOD
#define FLASH_CS_PIN_Pin GPIO_PIN_7
#define FLASH_CS_PIN_GPIO_Port GPIOD
#define Buzzer_Pin_Pin GPIO_PIN_6
#define Buzzer_Pin_GPIO_Port GPIOB
#define Flash_Reset_Pin GPIO_PIN_7
#define Flash_Reset_GPIO_Port GPIOB
#define PhotometryPeltierEnableLed_Pin GPIO_PIN_8
#define PhotometryPeltierEnableLed_GPIO_Port GPIOB
#define Preamp_Gain_A_Pin_Pin GPIO_PIN_1
#define Preamp_Gain_A_Pin_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */
typedef struct pepherals_status{
	bool sdcard_status;
}Pepherals_Status;

typedef struct Device_Status{
	Pepherals_Status peripherals;

	bool file_system;

}Device_Status;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
