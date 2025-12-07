/*
 * HandlerLCD.c
 *
 *  Created on: May 4, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "Preamp.h"
#include "lcd.h"
#include "Buzzer.h"
#include <string.h>
#include "Printer.h"
#include "RTC.h"
#include "USBDevice.h"
#include "../APPL/Inc/ApplUsbDevice.h"
#include "../APPL/Inc/NonVolatileMemory.h"
#include "../Screens/Inc/Screens.h"
#include "HandlerPheripherals.hpp"
#include <string>


#define LCD_UART_RX_MAX_DATA_LENGTH 5

extern  UART_HandleTypeDef huart1;
bool bFlashStatus = false;
uint32_t FlashReadCount = 0;
extern enSystemMemoryPeripherals stcMemoryPeripherals;
/*This is a Thread which handles tasks related to LCD and touch screen
 *
 *
 */
#include "../APPL/Inc/NVM_TestResultsHistory.hpp"

void HandlerThreadLcd(void)
{
	W25qxx_EnableWp();
	InitializesystemPeripherals();
	InitilizeFlash();
	InitializeHMIPheripherals();
	InitializeAllPheripherals();
	for(;;)
	{
		ProcessScreens();
		Lamp_Task();
	}
}
void InitilizeFlash(void)
{
	do
	{
		FlashReadCount++;
		osDelay(100);//Initilization delay for FLASH
		bFlashStatus = W25qxx_Init();
	}while(true != bFlashStatus && FlashReadCount < 20);

	if(true == bFlashStatus)
	{
		osDelay(1);//Initilization delay for FLASH
		ReadSettingsBuffer();
		ReadSecondarySettingsBuffer();
		if(objstcSettings.InitFlag1 != (uint32_t)NVM_INIT_OK_FLAG || objstcSettings.InitFlag2 != (uint32_t)NVM_INIT_OK_FLAG)
		{
			stcMemoryPeripherals.SettingsMemCorrpt = true;
			MemoryCorruptionBuzzer();
		}
		if(objstcSecondarySettings.InitFlag1 != (uint32_t)NVM_INIT_OK_FLAG || objstcSecondarySettings.InitFlag2 != (uint32_t)NVM_INIT_OK_FLAG)
		{
			stcMemoryPeripherals.SecondarySettingsMemCorrpt = true;
			MemoryCorruptionBuzzer();
		}
		if(stcMemoryPeripherals.SecondarySettingsMemCorrpt != true && stcMemoryPeripherals.SettingsMemCorrpt != true)
		{
			LongBeepBuzzer();
//			MemoryCorruptionBuzzer();
		}
	}
	else
	{
		stcMemoryPeripherals.InitMemoryfailed = true;
		TestTimerCompleteBuzzer();
	}
	FlashReadCount = 0;
}
void InitializeHMIPheripherals(void)
{
	#define NEXITON_DISPLAY_BAUDRATE (115200)
	uint32_t DisplayComUpdateCount = 0;

	if(stcMemoryPeripherals.InitMemoryfailed == true || stcMemoryPeripherals.SettingsMemCorrpt == true)
	{
		objstcSettings.PrinterBaudRate = 9600;
		objstcSettings.u32BuzzerEnable = true;
		objstcSettings.u32PrinterEnable = true;
	}

#if(false) /*Reset flash memory test parameter section - only for testing purposes*/
	ResetNVMFlash();/*Reset settings*/
	TestTimerCompleteBuzzer();
#endif
	bool status = nexInit(&huart1, NEXITON_DISPLAY_BAUDRATE);
	while(true != status && DisplayComUpdateCount < 20)
	{
		BeepBuzzer();
		osDelay(500);
		status = nexInit(&huart1, NEXITON_DISPLAY_BAUDRATE);
		DisplayComUpdateCount++;
	}
	if(status != true)
	{
		stcMemoryPeripherals.InitDisplayfailed = true;
	}
	ChangeWindowPage(en_WinId_StartupScreen , (enWindowID)en_WinID_MainMenuScreen);
}

void ToggleTouchLed(void)
{
	HAL_GPIO_TogglePin(PhotometryPeltierEnableLed_GPIO_Port,
			PhotometryPeltierEnableLed_Pin);/*LED update*/
}
