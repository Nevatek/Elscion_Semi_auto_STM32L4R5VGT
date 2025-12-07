/*
 * USBdevice.c
 *
 *  Created on: Jul 13, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "USBDevice.h"


#define MAX_MUTEX_WAIT_DELAY_USB_CDC 100//ms
extern osMutexId MutexUsbcdcComHandle;
#define USB_DEVICE_DEBUG_ENABLE_FLAG (false)

void CallBackUsbCdcRecieve(uint8_t *ptrBuffer, uint32_t RxLength)
{
#if (USB_DEVICE_DEBUG_ENABLE_FLAG)
	#define MAX_DATA_RX_USB_CDC 2048
	char strBuffer[MAX_DATA_RX_USB_CDC] = {0};
	strncpy(strBuffer,(char*)ptrBuffer,RxLength);
	ProcessUsbCdcCommands(strBuffer);
#endif
}

void SendStringOverUsbCDC(char *ptrString)
{
#if (USB_DEVICE_DEBUG_ENABLE_FLAG)
	#define MAX_DATA_TX_USB_CDC 2048
	osMutexWait(MutexUsbcdcComHandle,MAX_MUTEX_WAIT_DELAY_USB_CDC);
	CDC_Transmit_FS ((uint8_t*)ptrString, strnlen((char*)ptrString,MAX_DATA_TX_USB_CDC));
	osMutexRelease(MutexUsbcdcComHandle);
#endif
}

void SenddataOverUsbCDC(char *ptrdata , uint32_t u_n32Length)
{
#if (USB_DEVICE_DEBUG_ENABLE_FLAG)
	osMutexWait(MutexUsbcdcComHandle,MAX_MUTEX_WAIT_DELAY_USB_CDC);
	CDC_Transmit_FS ((uint8_t*)ptrdata,u_n32Length);
	osMutexRelease(MutexUsbcdcComHandle);
#endif
}

void DebugOvrUsbCdc(std::string Cmd , const char *ptrString)//Debuging purpose
{
#if (USB_DEVICE_DEBUG_ENABLE_FLAG)
	#define MAX_DEBUG_DATA_TX_USB_CDC 128
	char arr_cBuf[MAX_DEBUG_DATA_TX_USB_CDC] = {0};
	snprintf(arr_cBuf , (MAX_DEBUG_DATA_TX_USB_CDC - 2) , "\n\r%s : %s" , Cmd.c_str() ,  ptrString);

	osMutexWait(MutexUsbcdcComHandle,MAX_MUTEX_WAIT_DELAY_USB_CDC);
	CDC_Transmit_FS ((uint8_t*)arr_cBuf, strnlen((char*)arr_cBuf,MAX_DEBUG_DATA_TX_USB_CDC));
	osMutexRelease(MutexUsbcdcComHandle);
#endif
}
