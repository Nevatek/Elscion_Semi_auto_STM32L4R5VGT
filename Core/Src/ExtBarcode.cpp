/*
 * ExtBarcode.c
 *
 *  Created on: May 22, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "ExtBarcode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "USBDevice.h"
#include "FilterMotor.h"
extern  UART_HandleTypeDef huart2;
extern osSemaphoreId SemBarcodeRxHandle;
volatile uint8_t g_arr_BarcodeRxData[3 + MAX_RX_BARCODE_DATABUFFER] = {0};//Max 16 char barcode data
#define UART_BARCODE_TIMEOUT_RX 100

void InitalizeBarcodereader(void)
{
	HAL_GPIO_WritePin(USB_power_en_GPIO_Port, USB_power_en_Pin, GPIO_PIN_RESET);
}

void FlushBarcodeRxBuffer(void)
{
	HAL_UART_AbortReceive_IT(&huart2);
	HAL_UART_Receive_IT(&huart2,(uint8_t*)g_arr_BarcodeRxData, MAX_RX_BARCODE_DATABUFFER);
	g_arr_BarcodeRxData[0] = 0;
}
void Barocde_UART_RxCpltCallback(void)
{
	HAL_UART_Receive_IT(&huart2,(uint8_t*)g_arr_BarcodeRxData, MAX_BARCODE_DATA_LENGTH);
	if(0 != g_arr_BarcodeRxData[0])
	{
		osSemaphoreRelease( SemBarcodeRxHandle );
	}
	else
	{
		FlushBarcodeRxBuffer();
	}
}
void HandlerExternalBarcodeTask(void)
{

	InitBarcodeRxIntrupt();
	for(;;)
	{
         osSemaphoreWait( SemBarcodeRxHandle, ( TickType_t ) osWaitForever);
        {
        	vTaskDelay(100);
    	    if(0 != g_arr_BarcodeRxData[0])
    	    {
    	    	DecodeBarcode((char*)g_arr_BarcodeRxData);
    	    }
    	    FlushBarcodeRxBuffer();
        }
		vTaskDelay(1);
	}
}
void DecodeBarcode(char *ptrData)
{
	char BarcodeData[3 + MAX_RX_BARCODE_DATABUFFER] = {0};
	uint8_t u_n8BarcodeValid = false;
	for(int idx = 0;(MAX_RX_BARCODE_DATABUFFER - 1) > idx ;idx++)
	{
		if('\r' == ptrData[idx])
		{
			u_n8BarcodeValid = true;
			break;
		}
		BarcodeData[idx] = ptrData[idx];
	}
	if(u_n8BarcodeValid)
	{
		UsbDebugPrintStatusMessage("barcode Value :",en_DebugUsbStatusOk,BarcodeData);
	}
}
void InitBarcodeRxIntrupt(void)
{
	HAL_UART_Receive_IT(&huart2,(uint8_t*)g_arr_BarcodeRxData, MAX_BARCODE_DATA_LENGTH);
}
