#ifndef LCD_HPP
#define LCD_HPP
#include "../APPL/Inc/ApplLcd.h"

#define LCD_RAYSTAR (0)
#define LCD_NEXITON (1)
#define LCD_TYPE LCD_NEXITON

#if(LCD_TYPE == LCD_RAYSTAR)
#define SOB1 (0x01)//Start of byte 1
#define SOB2 (0x02)//Start of byte 2
#define SOB3 (0x04)//Start of byte 3
#define EB1 (0x0A)//End of byte 1
#define EB3 (0x0D)//End of byte 3
#define STX_ACK (0x06)
#define STX_NACK (0x15)
#define STX_TOUCH (0x41)


#define TIME_OUT_FOR_LCD_UART_RX_IN_TICKS  100
#define TIME_OUT_FOR_LCD_UART_TX_IN_TICKS  50
#define DELAY_LCD_COLD_START (3800)//Milli seconds

#endif

void HandlerThreadLcd(void);
void FlushLCDRxBuffer(void);
void InitializeHMIPheripherals(void);
void InitilizeFlash(void);
void ToggleTouchLed(void);

#if(LCD_TYPE == LCD_RAYSTAR)
void ShowWindow(uint16_t un16Xloc,uint16_t un16YLoc,uint16_t enWinID,enDisplayLayer DispLayer,enDisplayOrientation DispOrient);
uint8_t CalculateChecksum(uint8_t *ptr_Array , uint32_t NumOfBytes);
void SendDisplayCommand(uint8_t *ptr_Array , uint32_t NumOfBytes);
void PrintText(char *ptrText , uint16_t un16Xloc,uint16_t un16YLoc , uint32_t StringColour ,
		uint32_t BackgroundColour , enTextBackgndOpctacity bBackgroundTransperent);
void PrintPixelPoint(uint16_t un16Xloc,uint16_t un16YLoc , uint32_t Colour);
void PrintPixelGeometry(uint16_t un16XlocStart,uint16_t un16YLocStart,uint16_t un16XlocEnd,
		uint16_t un16YLocEnd , uint32_t Colour , uint8_t WidthSize ,enGeometry Geometry);
void EraseScreenArea(uint16_t un16XlocStart,uint16_t un16YLocStart,uint16_t un16XlocEnd,
		uint16_t un16YLocEnd,enEraseScreen EraseLoc);
void SetDisplayBrightness(uint16_t un16Brightness);
uint8_t ProcessLcdRxMessage(uint8_t *ptr_un8Msg , uint16_t *ptrXloc ,  uint16_t *ptryLoc);
void SendTpCompensationMode(void);
uint8_t InitLCDTouchPannelIntrupt(void);
void LCDTouchPannel_UART_RxCpltCallback(void);
#endif
#endif
