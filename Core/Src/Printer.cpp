/*
 * Printer.c
 *
 *  Created on: May 22, 2022
 *      Author: Alvin
 */

#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "Printer.h"
#include "system.hpp"
#include "string.h"
#include "../APPL/Inc/NonVolatileMemory.h"
#include "Buzzer.h"
#include <HandlerPheripherals.hpp>
#include "../APPL/Inc/Testrun.h"

#define MAX_PRINTER_DATA_STRING 63
#define MAX_PRINTER_STATUS_DATA_LENGTH 4
#define TIME_OUT_FOR_PRINTER_UART_TX_IN_TICKS 100
#define MAX_MUTEX_WAIT_DELAY_INT_PRINTER (1000)
volatile uint8_t arr_PrinterRxData[MAX_RX_PRINTER_DATA] = {0};

uint8_t printer_boldfont_on[3] = {0x1B, 0x21, 0x08};
uint8_t printer_boldfont_off[3] = {0x1B, 0x21, 0x00};

extern stcSettings objstcSettings;
enSystemMemoryPeripherals stcMemoryPeripherals;

void InitPrinter(UART_HandleTypeDef *pObj)
{
	HAL_StatusTypeDef nStatus = HAL_ERROR;
	pObj->Init.BaudRate = objstcSettings.PrinterBaudRate;
	if (HAL_UART_Init(pObj) != HAL_OK)
	{
		stcMemoryPeripherals.InitPrinterfailed = true;
		InstrumentBusyBuzz();
	}
	uint8_t pBuff[2] = {0x1B , 0x40};
	UART_HandleTypeDef *obj = GetInstance_PrinterUart();
	nStatus = HAL_UART_Transmit(obj, (uint8_t*) &pBuff[0], 2,
		TIME_OUT_FOR_PRINTER_UART_TX_IN_TICKS);
}

void Startup_Print()
{
	if(objstcSettings.Elscion_Model == NVM_OEM_ENABLE_FLAG)
	{
		PrintStringOnPrinter((char*)"ELSCION BIOMEDICALS");
		PrintStringOnPrinter((char*)"ARIS 100 CHEMISTRY ANALYZER");
	}
	else if(objstcSettings.OEM1_Model == NVM_OEM_ENABLE_FLAG) // Labnova
	{
		PrintStringOnPrinter((char*)"LABCHEM 7 PRO CHEMISTRY ANALYZER");
	}
	else if(objstcSettings.OEM2_Model == NVM_OEM_ENABLE_FLAG) //
	{
		PrintStringOnPrinter((char*)"ELSCION BIOMEDICALS");
		PrintStringOnPrinter((char*)"RIVO CHEM 100 CHEMISTRY ANALYZER");
	}
	else if(objstcSettings.OEM3_Model == NVM_OEM_ENABLE_FLAG) //
	{
		PrintStringOnPrinter((char*)" ");
	}

	PrintStringOnPrinter((char*)"********************************");
	PrinterFeedLine(1);
	if(stcMemoryPeripherals.InitDisplayfailed == true)
	{
		PrintStringOnPrinter((char*)"Display                : Not OK");
	}
	if(stcMemoryPeripherals.InitMemoryfailed == true)
	{
		PrintStringOnPrinter((char*)"Memory                 : Not OK");
	}
	if(stcMemoryPeripherals.SettingsMemCorrpt == true)
	{
		PrintStringOnPrinter((char*)"Settings Memory        : Not OK");
	}
	if(stcMemoryPeripherals.SecondarySettingsMemCorrpt == true)
	{
		PrintStringOnPrinter((char*)"Sec. Settings Memory   : Not OK");
	}
	if(stcMemoryPeripherals.TestParamsBackUpMemCorrpt == true)
	{
		PrintStringOnPrinter((char*)"Test Parameter Memory  : Not OK");
	}
	if(stcMemoryPeripherals.TestResultBackUpMemCorrpt == true)
	{
		PrintStringOnPrinter((char*)"Test Result Memory     : Not OK");
	}
	if(stcMemoryPeripherals.QCSetupBackUpMemCorrpt == true)
	{
		PrintStringOnPrinter((char*)"QC Setup Memory        : Not OK");
	}
	if(stcMemoryPeripherals.QCResultBackUpMemCorrpt == true)
	{
		PrintStringOnPrinter((char*)"QC Result Memory       : Not OK");
	}

	if(en_FilterMotorBusy != ReadFilterMotorStatus())
	{
		if(en_FilterMotorHomeError == ReadFilterMotorStatus())
		{
			PrintStringOnPrinter((char*)"Filter Wheel sensor    : Not OK");
		}
	}
	if(en_FilterMotorHomeError != ReadFilterMotorStatus())
	{
		for(int nI = 0; nI < en_PreampGain_Max; ++nI)
		{
			if(stcTestData.fPreampDarkADCVal[nI] > 300)
			{
				PrintStringOnPrinter((char*)"Optical light path      : Not OK");
				break;
			}
		}
	}
	PrinterFeedLine(4);
}

void Print_Header()
{
	if(objstcSettings.u32PrinterEnable)
	{
		if (strlen(objstcSettings.arr_CLabName) > 0)
		{
			PrintStringOnPrinter((char*)objstcSettings.arr_CLabName);
		}
		if (strlen(objstcSettings.arr_CFirstHeader) > 0)
		{
			PrintStringOnPrinter((char*)objstcSettings.arr_CFirstHeader);
		}
		if (strlen(objstcSettings.arr_CSecondHeader) > 0)
		{
			PrintStringOnPrinter((char*)objstcSettings.arr_CSecondHeader);
		}
	}
}

void Print_Footer()
{
	if(objstcSettings.u32PrinterEnable)
	{
		if (strlen(objstcSettings.arr_CEndfooter) > 0)
		{
			PrintStringOnPrinter((char*)objstcSettings.arr_CEndfooter);
		}
	}
}

inline void FlushPrinterRxBuffer(void)
{
	UART_HandleTypeDef *obj = GetInstance_PrinterUart();
	HAL_UART_AbortReceive_IT(obj);
	HAL_UART_Receive_IT(obj,(uint8_t*)arr_PrinterRxData, MAX_PRINTER_STATUS_DATA_LENGTH);
	arr_PrinterRxData[0] = 0;
}
inline void Printer_UART_RxCpltCallback(void)
{

}

void InitPrinterIntrupt(void)
{

}

void ReadPrinterStatus(void)
{
	UART_HandleTypeDef *obj = GetInstance_PrinterUart();
	uint8_t arrBuff[] = {0x1D,0x99};
	HAL_UART_Transmit(obj, (uint8_t*) arrBuff, 2, TIME_OUT_FOR_PRINTER_UART_TX_IN_TICKS);
}

void PrinterFeedLine(uint8_t u8Lines)
{
	if(objstcSettings.u32PrinterEnable)
	{
		HAL_StatusTypeDef nStatus = HAL_ERROR;
		UART_HandleTypeDef *obj = GetInstance_PrinterUart();
		uint8_t arrBuff[] = {0x0A};
		for(uint8_t u8N = 0 ; u8N < u8Lines ; ++u8N)
		{
			HAL_UART_Transmit(obj,
					(uint8_t*) arrBuff, sizeof(arrBuff),
					TIME_OUT_FOR_PRINTER_UART_TX_IN_TICKS);
		}
	}
}

void PrintStringOnPrinter(char *ptrBuff)
{
	if(objstcSettings.u32PrinterEnable)
	{
		HAL_StatusTypeDef nStatus = HAL_ERROR;
		uint8_t *pBuff = NULL;
		UART_HandleTypeDef *obj = GetInstance_PrinterUart();
		uint32_t u_n32LenOfData = 0;
		u_n32LenOfData = strnlen(ptrBuff,MAX_PRINTER_DATA_STRING);

		pBuff = (uint8_t*)malloc(u_n32LenOfData + 1);
		memcpy(&pBuff[0] , ptrBuff , u_n32LenOfData);
		pBuff[u_n32LenOfData] = '\n';

		nStatus = HAL_UART_Transmit(obj, (uint8_t*) &pBuff[0], u_n32LenOfData + 1,
				TIME_OUT_FOR_PRINTER_UART_TX_IN_TICKS);
		free(pBuff);
	}
}

void PrintDataOnPrinter(uint8_t *ptrBuff , uint32_t n32Length , uint16_t u16Delay)
{
	if(objstcSettings.u32PrinterEnable)
	{
		HAL_StatusTypeDef nStatus = HAL_ERROR;
		uint8_t *pBuff = NULL;
		UART_HandleTypeDef *obj = GetInstance_PrinterUart();
		uint32_t u_n32LenOfData = 0;
		u_n32LenOfData = n32Length;

		pBuff = (uint8_t*)malloc(u_n32LenOfData);
		memcpy(&pBuff[0] , ptrBuff , u_n32LenOfData);
	//	pBuff[u_n32LenOfData] = '\n';

		nStatus = HAL_UART_Transmit(obj, (uint8_t*) &pBuff[0], u_n32LenOfData, u16Delay);
		free(pBuff);
	}

}

void SendBoldOnOffCMD(bool BoldOnOff)
{
	if(objstcSettings.u32PrinterEnable)
	{
		HAL_StatusTypeDef nStatus = HAL_ERROR;
		uint8_t *pBuff = NULL;
		UART_HandleTypeDef *obj = GetInstance_PrinterUart();
		uint32_t u_n32LenOfData = 3;
		pBuff = (uint8_t*)malloc(u_n32LenOfData);

		if(BoldOnOff)
		{
		memcpy(&pBuff[0] , printer_boldfont_on , u_n32LenOfData);
		}
		else
		{
			memcpy(&pBuff[0] , printer_boldfont_off , u_n32LenOfData);
		}
		nStatus = HAL_UART_Transmit(obj, (uint8_t*) &pBuff[0], u_n32LenOfData, 1000);
		free(pBuff);
	}

}

