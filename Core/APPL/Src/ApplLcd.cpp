/*
 * ApplLcd.cpp
 *
 *  Created on: May 4, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "lcd.h"
#include <string.h>

#if(LCD_TYPE == LCD_RAYSTAR)
uint8_t ProcessLcdRxMessage(uint8_t *ptr_un8Msg , uint16_t *ptrXloc ,  uint16_t *ptryLoc)
{
  uint8_t un8Status = en_LCD_Responsefailed;
  switch((uint8_t)ptr_un8Msg[0])
  {
  	case STX_TOUCH:
  		un8Status = en_LCDTouchcordinates;
  		uint8_t Xmsb = ptr_un8Msg[1];
  		uint8_t Xlsb = ptr_un8Msg[2];
  		*ptrXloc = (uint16_t)((Xmsb << 8 ) | (Xlsb & 0xff));

  		uint8_t Ymsb = ptr_un8Msg[3];
  		uint8_t Ylsb = ptr_un8Msg[4];
  		*ptryLoc = (uint16_t)((Ymsb << 8 ) | (Ylsb & 0xff));
  		break;
  	case STX_ACK:
  		un8Status = en_LCDAcknolodgement;
  	break;
  	case STX_NACK:
  		un8Status = en_LCDNack;
  	break;
  	default:
  		break;
  }
  return un8Status;
}
#endif

