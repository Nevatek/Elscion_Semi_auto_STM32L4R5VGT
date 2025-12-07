/*
 * USBDevice.h
 *
 *  Created on: Jul 13, 2022
 *      Author: Alvin
 */

#ifndef INC_USBDEVICE_H_
#define INC_USBDEVICE_H_
#if 0
#include "usbd_cdc_if.h"
#endif
#include "Buzzer.h"
#include "Lamp.h"
#include "Preamp.h"
#include "Printer.h"
#include "RTC.h"
#include <string>
#include "../APPL/Inc/ApplUsbDevice.h"

#define USB_CDC_CMD_PRINTER 					"PRI" //Print data or read printer paper status
#define USB_CDC_CMD_RTC_DATE_TIME 				"RTC" //Set or get RTC date and time
#define USB_CDC_CMD_BUZZER 						"BUZ"
#define USB_CDC_CMD_LAMP 						"LAM"
#define USB_CDC_CMD_READ_PREAMP_GAIN 			"PRG"//#PRG=gain
#define USB_CDC_CMD_READ_PREAMP_ADC  			"PRA"//#PRA=1
#define USB_CDC_CMD_READ_MOVE_FILTER_STEPS  	"FLS"
#define USB_CDC_CMD_READ_MOVE_FILTER_ANGLE  	"FLA"
#define USB_CDC_CMD_READ_MOVE_FILTER_HOME   	"FLH"
#define USB_CDC_CMD_READ_FILTER_ENABLE_MOTOR   	"FLE"
#define USB_CDC_CMD_MOVE_DC_MOTOR  				"MOT"
#define USB_CDC_CMD_VALVE_CNTRL			  		"VAL"
#define USB_CDC_CMD_ASP_LED_CNTRL			  	"ALE"
#define USB_CDC_CMD_ASP_BUTTON_SIMULATION	    "ASP"
#define USB_CDC_CMD_READ_TEMP_SENS       	    "TMP"
#define USB_CDC_CMD_READ_LIQUID_SENS      	    "LIQ"
#define USB_CDC_CMD_READ_AUTO_WASH_DOOR_SENS    "ADR"
#define USB_CDC_CMD_READ_FILTER_SLOT_SENS       "FHS"
#define USB_CDC_CMD_ASP_PUMP_VOL                "PUM"
#define USB_CDC_CMD_FILTER_SELECT               "FIL"
#define USB_CDC_RESET_FLASH_TESTPARAMS			"RTP"

void CallBackUsbCdcRecieve(uint8_t *ptrBuffer, uint32_t RxLength);
void SendStringOverUsbCDC(char *ptrString);
void SenddataOverUsbCDC(char *ptrdata , uint32_t u_n32Length);
uint8_t ProcessUsbCdcCommands(const char * const ptrBuffer);
void DebugOvrUsbCdc(std::string Cmd , const char *ptrString);//Debuging purpose
#endif /* INC_USBDEVICE_H_ */
