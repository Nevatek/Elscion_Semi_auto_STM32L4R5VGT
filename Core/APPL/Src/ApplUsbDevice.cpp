/*
 * APPLDebug.c
 *
 *  Created on: Aug 1, 2022
 *      Author: Alvin
 */
#include <math.h>
#include <string>
#include "string.h"
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "USBDevice.h"
#include "Preamp.h"
#include "lcd.h"
#include "DcMotor.hpp"
#include "FilterMotor.h"
#include "math.h"
#include "HandlerPheripherals.hpp"
#include "TempratureSens.h"
#include "../APPL/Inc/ApplUsbDevice.h"
#include "../Libraries/json/cJSON_Utils.h"
#include "../Libraries/json/cJSON.h"
#include "../APPL/Inc/NonVolatileMemory.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"

extern uint8_t e_bPrempDebugOutEnableFlag;

/*
 *
 * Sample comamnds
 * {     "cmd": "PRG", 	"task":"=", 	"value": "2" }
 * {     "cmd": "PRA", 	"task":"=", 	"value": "on" }
 * {     "cmd": "BUZ", 	"task":"=", 	"value": "on" }
 * {     "cmd": "LAM", 	"task":"=", 	"value": "off" }
 * {     "cmd": "RTC", 	"task":"?", 	"value": "NULL" }
 */
uint8_t ProcessUsbCdcCommands(const char * const ptrBuffer)
{
#if DEBUGGING_FORMAT_JSON
#else
	#define USB_CDC_CMD_LENGTH 3
	char u_n8GetorSetCmd = 0;
#endif
	uint8_t Status = false;
	/*Json validation starts*/
	if(USB_CDC_SOP != ptrBuffer[0])
	{
		UsbDebugPrintStatusMessage("Invalid CMD (SOP)",en_DebugUsbStatusError," ");
		return Status = false;
	}
#if DEBUGGING_FORMAT_JSON
    cJSON *StrMainJsonBody = cJSON_Parse(ptrBuffer);
    if (StrMainJsonBody == NULL)
    {
        const char *ptrErr = cJSON_GetErrorPtr();
        if (ptrErr != NULL)
        {
        	UsbDebugPrintStatusMessage("Json invalid",en_DebugUsbStatusError,ptrErr);
        	Status = false;
        }
    }
    else
    {
    	Status = true;
    }

    if(false == Status)
    {
    	cJSON_Delete(StrMainJsonBody);
    	return Status;
    }

    cJSON *jsonDataCmd = cJSON_GetObjectItemCaseSensitive(StrMainJsonBody, STRING_JSON_COMMAND);
    if (!cJSON_IsString(jsonDataCmd) || (jsonDataCmd->valuestring == NULL))
    {
    	UsbDebugPrintStatusMessage("Json cmd parsing failed",en_DebugUsbStatusError,jsonDataCmd->valuestring);
    	Status = false;
    }

    cJSON *jsonDataTask = cJSON_GetObjectItemCaseSensitive(StrMainJsonBody, STRING_JSON_TASK);
    if (!cJSON_IsString(jsonDataTask) || (jsonDataTask->valuestring == NULL))
    {
    	UsbDebugPrintStatusMessage("Json task parsing failed",en_DebugUsbStatusError,jsonDataTask->valuestring);
    	Status = false;
    }

    cJSON *jsonDataValue = cJSON_GetObjectItemCaseSensitive(StrMainJsonBody, STRING_JSON_VALUE);
    if (!cJSON_IsString(jsonDataValue) || (jsonDataValue->valuestring == NULL))
    {
    	UsbDebugPrintStatusMessage("Json value parsing failed",en_DebugUsbStatusError,jsonDataValue->valuestring);
    	Status = false;
    }

    if(false == Status)
    {
    	cJSON_Delete(StrMainJsonBody);
    	return Status;
    }

    /*Json validation completed*/
    /*Json parsing starts*/
    /*Json parsing completed*/
#else
	if(ptrBuffer[1 + USB_CDC_CMD_LENGTH] == USB_CDC_SET_CMD)
	{
		u_n8GetorSetCmd = USB_CDC_SET_CMD;
		ptrBuffer[1 + USB_CDC_CMD_LENGTH] = '\0';
	}
	else if(ptrBuffer[1 + USB_CDC_CMD_LENGTH] == USB_CDC_GET_CMD)
	{
		u_n8GetorSetCmd = USB_CDC_GET_CMD;
		ptrBuffer[1 + USB_CDC_CMD_LENGTH] = '\0';
	}
	else
	{
		return Status = false;
	}
#endif
	if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_PRINTER))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  PrintStringOnPrinter(jsonDataValue->valuestring);
	  }
	  else if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_GET_CMD))
	  {
		  ReadPrinterStatus();
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_RTC_DATE_TIME))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_GET_CMD))
	  {
		  USBDebugPrintCurrentDateTime();
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_BUZZER))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD) && 0 == strncmp("on",jsonDataValue->valuestring,2))
	  {
		  BeepBuzzer();
		  UsbDebugPrintStatusMessage("Buzzer",en_DebugUsbStatusOk,jsonDataCmd->valuestring);
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_RESET_FLASH_TESTPARAMS))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD) && 0 == strncmp("reset",jsonDataValue->valuestring,2))
	  {
		  UsbDebugPrintStatusMessage("Test params reset - START",en_DebugUsbStatusOk,jsonDataCmd->valuestring);
			/*Reset flash memory test parameter section*/
			for(int nI = 0 ; nI < MAX_NUM_OF_TEST_PARAMS_SAVED; nI++)
			{
				GetInstance_TestParams()[stcTestData.TestId].ValidTestParamSavedFlag = INVALID_TEST_PARAM_FLAG;
			}
			WriteTestparameterBuffer();/*Writing tets params to memory*/
		  UsbDebugPrintStatusMessage("Test params reset - END",en_DebugUsbStatusOk,jsonDataCmd->valuestring);
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_LAMP))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  if(0 == strncmp("on",jsonDataValue->valuestring,2))
		  {
			  EnableDisableLamp(en_LampEnable);
			  UsbDebugPrintStatusMessage("Lamp",en_DebugUsbStatusOk,jsonDataCmd->valuestring);
		  }
		  else if(0 == strncmp("off",jsonDataValue->valuestring,2))
		  {
			  EnableDisableLamp(en_LampDisable);
			  UsbDebugPrintStatusMessage("Lamp",en_DebugUsbStatusOk,jsonDataCmd->valuestring);
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }

	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_READ_PREAMP_GAIN))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  enPreampGain Gain = (enPreampGain)atoi(jsonDataValue->valuestring);
		  if(en_PreampGain_OFF <= Gain && en_PreampGain_Max > Gain)
		  {
			  SetPreampGain(Gain);
			  UsbDebugPrintStatusMessage("set gain",en_DebugUsbStatusOk,IntToCharConverter(Gain));
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("set gain invalid (valid range : 0 - 16)",en_DebugUsbStatusError,IntToCharConverter(Gain));
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_READ_PREAMP_ADC))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  if(0 == strncmp("on",jsonDataValue->valuestring,2))
		  {
			  e_bPrempDebugOutEnableFlag = true;
		  }
		  else if(0 == strncmp("off",jsonDataValue->valuestring,2))
		  {
			  e_bPrempDebugOutEnableFlag = false;
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_READ_MOVE_FILTER_STEPS))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  uint32_t n32MotorSteps = atoi(jsonDataValue->valuestring);
		  if(n32MotorSteps)
		  {
			 SetFilterMotorDirection(en_FilterMotorLeft);
			 RotateFilterMotor(n32MotorSteps);
			 UsbDebugPrintStatusMessage("Rotate filter motor steps",en_DebugUsbStatusOk,jsonDataValue->valuestring);
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid step",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_READ_MOVE_FILTER_ANGLE))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  float n32MotorAngle = atof(jsonDataValue->valuestring);
		  if(n32MotorAngle)
		  {
			 RotateFilterMotorAngle(n32MotorAngle, en_FilterMotorLeft);
			 UsbDebugPrintStatusMessage("Rotate filter motor angle",en_DebugUsbStatusOk,jsonDataValue->valuestring);
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid angle",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_READ_MOVE_FILTER_HOME))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  if(0 == strncmp("on",jsonDataValue->valuestring,2))
		  {
			  SetFilterMotorDirection(en_FilterMotorHome);
			 UsbDebugPrintStatusMessage("Home filter motor",en_DebugUsbStatusOk,jsonDataValue->valuestring);
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid angle",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_READ_FILTER_ENABLE_MOTOR))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  if(0 == strncmp("on",jsonDataValue->valuestring,2))
		  {
			  EnableDisableFilterMotor(enFilterMotorEnable);
			  UsbDebugPrintStatusMessage("Enable filter motor",en_DebugUsbStatusOk,jsonDataValue->valuestring);
		  }
		  else if(0 == strncmp("off",jsonDataValue->valuestring,2))
		  {
			  EnableDisableFilterMotor(enFilterMotorDisable);
			  UsbDebugPrintStatusMessage("Disable filter motor",en_DebugUsbStatusOk,jsonDataValue->valuestring);
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_MOVE_DC_MOTOR))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  uint32_t n32DutyCycle = atoi(jsonDataValue->valuestring);
		  if(0 <= n32DutyCycle && 100 >= n32DutyCycle)
		  {
#if(PUMP_CHANNEL_TYPE == PUMP_GPIO)
			  RotateDcMotor(enDcMotorEnable , enDcMotorLeft);
#else
			  RotateDcMotor(enDcMotorLeft, n32DutyCycle);
#endif
			  UsbDebugPrintStatusMessage("Motor speed set",en_DebugUsbStatusOk,jsonDataValue->valuestring);
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data (valid range : 0 - 100)",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_VALVE_CNTRL))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  enValveCntrl ValvePos = (enValveCntrl)atoi(jsonDataValue->valuestring);
		  if(en_ValvePosWaterTank <= ValvePos && en_ValvePosProbe >= ValvePos)
		  {
			  ThreeWayValeCntrl(ValvePos);
			  UsbDebugPrintStatusMessage("Valve position",en_DebugUsbStatusOk,jsonDataValue->valuestring);
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data (valid range : 0 - 1)",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_ASP_LED_CNTRL))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  if(0 == strncmp("on",jsonDataValue->valuestring,2))
		  {
			  AspSwitchLed_Red(en_AspLedON);
			  UsbDebugPrintStatusMessage("Aspiration LED",en_DebugUsbStatusOk,jsonDataValue->valuestring);
		  }
		  else if(0 == strncmp("off",jsonDataValue->valuestring,2))
		  {
			  AspSwitchLed_Red(en_AspLedOFF);
			  UsbDebugPrintStatusMessage("Aspiration LED",en_DebugUsbStatusOk,jsonDataValue->valuestring);
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data (Range on / off)",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_ASP_BUTTON_SIMULATION))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  if(0 == strncmp("press",jsonDataValue->valuestring,2))
		  {
			  DebugSimulateAspirationSwPress();
			  UsbDebugPrintStatusMessage("Aspiration Switch simulation",en_DebugUsbStatusOk,jsonDataValue->valuestring);
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data (valid value : press)",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_READ_TEMP_SENS))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  enTempControl Sensor = (enTempControl)atoi(jsonDataValue->valuestring);
		  if(en_ChamperTemp == Sensor)
		  {
			  UsbDebugPrintStatusMessage("Chamber Temperature",en_DebugUsbStatusOk,DoubleToCharConverter( Readtemperature(Sensor) , 2));
		  }
		  else if(en_PhotometerTemp == Sensor)
		  {
			  UsbDebugPrintStatusMessage("Photometer Temperature",en_DebugUsbStatusOk,DoubleToCharConverter( Readtemperature(Sensor) , 2));
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data (valid range : 0 - 1)",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_READ_LIQUID_SENS))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  if(0 == strncmp("read",jsonDataValue->valuestring,2))
		  {
			  UsbDebugPrintStatusMessage("Liquid detection sensor",en_DebugUsbStatusOk,ReadLiquidDetStatus()?"Not detected":"Detected");
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data (valid value : read)",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_READ_AUTO_WASH_DOOR_SENS))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  if(0 == strncmp("read",jsonDataValue->valuestring,2))
		  {
			  UsbDebugPrintStatusMessage("Auto wash door sensor",en_DebugUsbStatusOk,ReadAutoWashDoorStatus()?"Not detected":"Detected");
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data (valid value : read)",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_READ_FILTER_SLOT_SENS))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  if(0 == strncmp("read",jsonDataValue->valuestring,2))
		  {
			  uint8_t FHStatus = ReadFilterWheelSlotSens();
			  UsbDebugPrintStatusMessage("Filter wheel slot sensor",en_DebugUsbStatusOk,FHStatus?"Away from home":"Home");// falling edge means home
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("json invalid Data (valid value : read)",en_DebugUsbStatusError,jsonDataValue->valuestring);
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_ASP_PUMP_VOL))
	{
      uint16_t Vol = atoi(jsonDataValue->valuestring);//Ul
	  if(0 < Vol)
	  {
		  Aspirate_Pump(Vol);
		  while(enPump_Off != Pump_RunTask());
	      UsbDebugPrintStatusMessage("Aspirating pump ,VOL :",en_DebugUsbStatusOk,jsonDataValue->valuestring);
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else if(0 == strcmp(jsonDataCmd->valuestring,USB_CDC_CMD_FILTER_SELECT))
	{
	  if(0 == strcmp(jsonDataTask->valuestring,USB_CDC_SET_CMD))
	  {
		  en_WavelengthFilter FIlter = (en_WavelengthFilter)atoi(jsonDataValue->valuestring);
		  if(en_FilterHome <= FIlter && en_FilterMax > FIlter)
		  {
			  SelectFilter(FIlter);
			  UsbDebugPrintStatusMessage("set filter",en_DebugUsbStatusOk,IntToCharConverter(FIlter));
		  }
		  else
		  {
			  UsbDebugPrintStatusMessage("set filter invalid (valid range : 0 - 9)",en_DebugUsbStatusError,IntToCharConverter(FIlter));
		  }
	  }
	  else
	  {
		  UsbDebugPrintStatusMessage("json invalid task",en_DebugUsbStatusError,jsonDataTask->valuestring);
	  }
	}
	else
	{
		UsbDebugPrintStatusMessage("usb cdc Invalid CMD",en_DebugUsbStatusError,jsonDataCmd->valuestring);
	}
	cJSON_Delete(StrMainJsonBody);
	return Status = true;
}

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}


int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}
//void ftoa(float n, char* res, int afterpoint)
char const* DoubleToCharConverter(float lfVal , uint8_t Precision)
{
	#define MAX_NUM_OF_DIGITS_FC (20)
	static char res[32] = {0};
//#if 0
//
//	// Extract integer part
//	    int ipart = (int)lfVal;
//
//	    // Extract floating part
//	    float fpart = lfVal - (float)ipart;
//
//	    // convert integer part to string
//	    int i = intToStr(ipart, res, 0);
//
//	    // check for display option after point
//	    if (Precision != 0) {
//	        res[i] = '.'; // add dot
//
//	        // Get the value of fraction part upto given no.
//	        // of points after dot. The third parameter
//	        // is needed to handle cases like 233.007
//	        fpart = fpart * pow(10, Precision);
//
//	        intToStr((int)fpart, res + i + 1, Precision);
//	    }
//#else
//   // volatile std::string Str = std::to_string(256.32);
//	  snprintf(res,(MAX_NUM_OF_DIGITS_FC - 1),"%0.4f",(float)lfVal);
//#endif

    // Create a format string based on the desired precision
    char format[10];
    snprintf(format, sizeof(format), "%%.%df", Precision);

    // Use the format string in snprintf to get the desired precision
    snprintf(res, sizeof(res), format, lfVal);

    return res;
}
char const* IntToCharConverter(uint32_t u_n32Val)
{
	#define MAX_NUM_OF_DIGITS_INTC (20)
	static char ptrBuff[MAX_NUM_OF_DIGITS_INTC] = {0};
	snprintf(ptrBuff,(MAX_NUM_OF_DIGITS_INTC - 1),"%d",(int)u_n32Val);
	return ptrBuff;
}
void UsbDebugPrintStatusMessage(const char * const ptrBuffer,enDebugUsbStatus enStatus,const char *const Value)
{
	cJSON * Status = NULL;
    cJSON *strJsonBody = cJSON_CreateObject();
    cJSON * Message = cJSON_CreateString(ptrBuffer);
    cJSON * strValue = cJSON_CreateString(Value);
    if(en_DebugUsbStatusOk == enStatus)
    {
    	Status = cJSON_CreateString("success");
    }
    else if(en_DebugUsbStatusError == enStatus)
    {
    	Status = cJSON_CreateString("error");
    }
    else
    {
    	Status = cJSON_CreateString("unknown");
    }
    cJSON_AddItemToObject(strJsonBody, "status", Status);
    cJSON_AddItemToObject(strJsonBody, "message", Message);
    cJSON_AddItemToObject(strJsonBody, "Value", strValue);
    char * string = cJSON_Print(strJsonBody);
	SendStringOverUsbCDC(string);
	HAL_Delay(2);
}
void USBDebugAdcPreampValues(uint16_t n16Adc ,enPreampGain enGain,uint16_t n16SampleIntervel)
{
    cJSON *strJsonBody = cJSON_CreateObject();
    cJSON * SampleTimeIntervel = cJSON_CreateString(IntToCharConverter(n16SampleIntervel));
    cJSON * Gain = cJSON_CreateString(IntToCharConverter(enGain));
    cJSON * AdcValue = cJSON_CreateString(IntToCharConverter(n16Adc));
    cJSON * Command = cJSON_CreateString("preamp");

    cJSON_AddItemToObject(strJsonBody, STRING_JSON_COMMAND, Command);
    cJSON_AddItemToObject(strJsonBody, "adc", AdcValue);
    cJSON_AddItemToObject(strJsonBody, "gain", Gain);
    cJSON_AddItemToObject(strJsonBody, "intervel", SampleTimeIntervel);

    char * string = cJSON_Print(strJsonBody);
	SendStringOverUsbCDC(string);
}
void USBDebugPrintCurrentDateTime(void)
{
	#define MAX_DATE_TIME_STRING_PRINTF 64
	RTC_DateTypeDef sDate;
	RTC_TimeTypeDef sTime;

	GetCurrentDate(&sDate);
	GetCurrentTime(&sTime);


	char strDate[MAX_DATE_TIME_STRING_PRINTF] = {0};
	char strTime[MAX_DATE_TIME_STRING_PRINTF] = {0};
	snprintf(strDate,MAX_DATE_TIME_STRING_PRINTF,"20%02d/%02d/%02d",sDate.Year,sDate.Month,sDate.Date);
	snprintf(strTime,MAX_DATE_TIME_STRING_PRINTF,"%02d:%02d:%02d",sTime.Hours,sTime.Minutes,sTime.Seconds);

    cJSON *strJsonBody = cJSON_CreateObject();
    cJSON * Command = cJSON_CreateString("rtc");
    cJSON * Date = cJSON_CreateString(strDate);
    cJSON * Time = cJSON_CreateString(strTime);

    cJSON_AddItemToObject(strJsonBody, STRING_JSON_COMMAND, Command);
    cJSON_AddItemToObject(strJsonBody, "date", Date);
    cJSON_AddItemToObject(strJsonBody, "time", Time);

    char * string = cJSON_Print(strJsonBody);
	SendStringOverUsbCDC(string);
}
void USBDebugPrintLcdTouchCordinates(uint16_t Xloc , uint16_t Yloc)
{
	#define MAX_DATE_TIME_STRING_PRINTF 64

    cJSON *strJsonBody = cJSON_CreateObject();
    cJSON * Command = cJSON_CreateString("lcdtouch");
    cJSON * jsonXloc = cJSON_CreateString(IntToCharConverter(Xloc));
    cJSON * jsonYloc = cJSON_CreateString(IntToCharConverter(Yloc));

    cJSON_AddItemToObject(strJsonBody, STRING_JSON_COMMAND, Command);
    cJSON_AddItemToObject(strJsonBody, "X", jsonXloc);
    cJSON_AddItemToObject(strJsonBody, "Y", jsonYloc);

    char * string = cJSON_Print(strJsonBody);
	SendStringOverUsbCDC(string);
}
void USBDebugPrintTestDetails(TestData stcTestDetails)
{
	#define MAX_DATE_TIME_STRING_PRINTF 64
    uint16_t MaxNumOfMeasurement = 0;
	cJSON *strJsonBody = cJSON_CreateObject();
	cJSON * Command = cJSON_CreateString("Test Data");
	cJSON * jsonWaterBlankAdc = cJSON_CreateString(IntToCharConverter(stcTestDetails.DIPrimaryFilterWaterAdc));

//	cJSON * jsonRegentBlankAdc = cJSON_CreateString(IntToCharConverter(stcTestDetails.ReagentBlankAbsPrimary));
//	cJSON * jsonReagentBlankAbs = cJSON_CreateString(DoubleToCharConverter(stcTestDetails.ReagentBlankAbsPrimary, 3));
	cJSON * jsonSampleAbs = cJSON_CreateString(DoubleToCharConverter(stcTestDetails.SampleOpticalDensity, 3));
	cJSON * jsonTestId = cJSON_CreateString(IntToCharConverter(stcTestDetails.TestId));
	cJSON * jsonPhotometerTemp = cJSON_CreateString(DoubleToCharConverter(objstcTempControl[en_PhotometerTemp].CurrentTemp, 3));
	cJSON * jsonBlankType = NULL;
	cJSON * jsonTestType = NULL;
	if(true == GetInstance_TestParams()[stcTestDetails.TestId].SampleBlankEnable)
	{
		jsonBlankType = cJSON_CreateString("Sample blank");
	}
	else if(true == GetInstance_TestParams()[stcTestDetails.TestId].ReagentBlankEnable)
	{
		jsonBlankType = cJSON_CreateString("Reagent blank");
	}


	cJSON_AddItemToObject(strJsonBody, STRING_JSON_COMMAND, Command);
	cJSON_AddItemToObject(strJsonBody, "Blank Type", jsonBlankType);

	if(en_Endpoint == GetInstance_TestParams()[stcTestDetails.TestId].TestProcedureType)
	{
		if(en_FilterHome != GetInstance_TestParams()[stcTestDetails.TestId].SecWavelength)
		{
			jsonTestType = cJSON_CreateString("End point - Dual filter");
		}
		else
		{
			jsonTestType = cJSON_CreateString("End point");
		}
		MaxNumOfMeasurement = MAX_ADC_READING_ENDPOINT;
	}
	else if(en_FixedTime == GetInstance_TestParams()[stcTestDetails.TestId].TestProcedureType)
	{
		jsonTestType = cJSON_CreateString("Fixed time");
		MaxNumOfMeasurement = GetInstance_TestParams()[stcTestDetails.TestId].MeasurementTimeSec;
	}
	else if(en_Kinetics == GetInstance_TestParams()[stcTestDetails.TestId].TestProcedureType)
	{
		jsonTestType = cJSON_CreateString("Kinetics");
		MaxNumOfMeasurement = GetInstance_TestParams()[stcTestDetails.TestId].MeasurementTimeSec;
	}
	cJSON_AddItemToObject(strJsonBody, "Test Parameter ID", jsonTestId);
	cJSON_AddItemToObject(strJsonBody, "Test Type", jsonTestType);
	cJSON_AddItemToObject(strJsonBody, "Photometer Temperature", jsonPhotometerTemp);

	cJSON * jsonString = cJSON_CreateString("---");
	cJSON_AddItemToObject(strJsonBody, "Primary incubation ADC", jsonString);
	for(int nI = 0 ; nI < GetInstance_TestParams()[stcTestDetails.TestId].IncubationTimeSec ; nI++)
	{
		char arr_cBuff[32];
		snprintf(arr_cBuff , 16 , "INC_ADC[%d]" , nI);
		cJSON * JsonStr = cJSON_CreateString(IntToCharConverter(stcTestDetails.PrimaryAdcBuffer[nI]));
		cJSON_AddItemToObject(strJsonBody, arr_cBuff, JsonStr);
	}

	jsonString = cJSON_CreateString("---");
	cJSON_AddItemToObject(strJsonBody, "Primary incubation ABS", jsonString);
	for(int nI = 0 ; nI < GetInstance_TestParams()[stcTestDetails.TestId].IncubationTimeSec ; nI++)
	{
		char arr_cBuff[32];
		snprintf(arr_cBuff , 16 , "INC_ABS[%d]" , nI);
		cJSON * JsonStr = cJSON_CreateString(DoubleToCharConverter(stcTestDetails.PrimaryAbsBuffer[nI] , 3));
		cJSON_AddItemToObject(strJsonBody, arr_cBuff, JsonStr);
	}

	jsonString = cJSON_CreateString("---");
	cJSON_AddItemToObject(strJsonBody, "Primary ADC", jsonString);
	for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)
	{
		char arr_cBuff[32];
		snprintf(arr_cBuff , 16 , "P_ADC[%d]" , nI);
		cJSON * JsonStr = cJSON_CreateString(IntToCharConverter(stcTestDetails.PrimaryAdcBuffer[nI]));
		cJSON_AddItemToObject(strJsonBody, arr_cBuff, JsonStr);
	}

	jsonString = cJSON_CreateString("---");
	cJSON_AddItemToObject(strJsonBody, "Primary ABS", jsonString);
	for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)
	{
		char arr_cBuff[32];
		snprintf(arr_cBuff , 16 , "P_ABS[%d]" , nI);
		cJSON * JsonStr = cJSON_CreateString(DoubleToCharConverter(stcTestDetails.PrimaryAbsBuffer[nI] , 3));
		cJSON_AddItemToObject(strJsonBody, arr_cBuff, JsonStr);
	}

	if(en_FilterHome != GetInstance_TestParams()[stcTestDetails.TestId].SecWavelength)
	{
		jsonString = cJSON_CreateString("---");
		cJSON_AddItemToObject(strJsonBody, "Secondary ADC", jsonString);
		for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)
		{
			char arr_cBuff[32];
			snprintf(arr_cBuff , 16 , "S_ADC[%d]" , nI);
			cJSON * JsonStr = cJSON_CreateString(IntToCharConverter(stcTestDetails.PrimaryAdcBuffer[nI]));
			cJSON_AddItemToObject(strJsonBody, arr_cBuff, JsonStr);
		}
	}

	if(en_FilterHome != GetInstance_TestParams()[stcTestDetails.TestId].SecWavelength)
	{
		jsonString = cJSON_CreateString("---");
		cJSON_AddItemToObject(strJsonBody, "Secondary ABS", jsonString);
		for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)
		{
			char arr_cBuff[32];
			snprintf(arr_cBuff , 16 , "S_ABS[%d]" , nI);
			cJSON * JsonStr = cJSON_CreateString(DoubleToCharConverter(stcTestDetails.SecondaryAbsBuffer[nI] , 3));
			cJSON_AddItemToObject(strJsonBody, arr_cBuff, JsonStr);
		}
	}

	cJSON_AddItemToObject(strJsonBody, "DI Water Blank ADC", jsonWaterBlankAdc);
//	cJSON_AddItemToObject(strJsonBody, "Regent Blank ADC", jsonRegentBlankAdc);
//	cJSON_AddItemToObject(strJsonBody, "Regent Blank ABS", jsonReagentBlankAbs);
	cJSON_AddItemToObject(strJsonBody, "Sample OD", jsonSampleAbs);
	char * string = cJSON_Print(strJsonBody);
	SendStringOverUsbCDC(string);
}
