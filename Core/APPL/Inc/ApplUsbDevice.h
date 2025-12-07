/*
 * ApplDebug.h
 *
 *  Created on: Aug 1, 2022
 *      Author: Alvin
 */

#ifndef APPL_INC_APPLDEBUG_H_
#define APPL_INC_APPLDEBUG_H_
#include "../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/Testrun.h"
#include "../APPL/Inc/ApplLcd.h"
#include "Preamp.h"

#define DEBUGGING_FORMAT_JSON (true)
#if DEBUGGING_FORMAT_JSON
	#define USB_CDC_SOP '{'
	#define STRING_JSON_COMMAND "cmd"
	#define STRING_JSON_TASK "task"
	#define STRING_JSON_VALUE "value"
#else
	#define USB_CDC_SOP '#'
#endif
#define USB_CDC_SET_CMD "="
#define USB_CDC_GET_CMD "?"
typedef enum
{
	en_DebugUsbStatusUnknown = 0,
	en_DebugUsbStatusError,
	en_DebugUsbStatusOk,
}enDebugUsbStatus;
void reverse(char* str, int len);
int intToStr(int x, char str[], int d);
char const* IntToCharConverter(uint32_t u_n32Val);
char const* DoubleToCharConverter(float lfVal , uint8_t Precision);
void USBDebugAdcPreampValues(uint16_t n16Adc ,enPreampGain enGain,uint16_t n16SampleIntervel);
void UsbDebugPrintStatusMessage(const char * const ptrBuffer,enDebugUsbStatus enStatus,const char *const Value);
void USBDebugPrintCurrentDateTime(void);
void USBDebugPrintLcdTouchCordinates(uint16_t Xloc , uint16_t Yloc);
void USBDebugPrintTestDetails(TestData stcTestDetails);
#endif /* APPL_INC_APPLDEBUG_H_ */
