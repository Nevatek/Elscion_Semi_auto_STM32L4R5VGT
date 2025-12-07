/*
 * Printer.h
 *
 *  Created on: May 22, 2022
 *      Author: Alvin
 */

#ifndef INC_PRINTER_H_
#define INC_PRINTER_H_

typedef enum
{
	en_PrinterOK,
	en_PrinterErrNoPaper,
	en_PrinterErrUnknown
}enPrinter;
#define MAX_RX_PRINTER_DATA 32
#define TIME_OUT_FOR_PRINTER_UART_RX_IN_TICKS 50

void PrintStringOnPrinter(char *ptrBuff);
void PrintDataOnPrinter(uint8_t *ptrBuff , uint32_t n32Length , uint16_t u16Delay);
void HandlerExtPrinter(void);
void ReadPrinterStatus(void);
void PrinterFeedLine(uint8_t u8Lines);
void Printer_UART_RxCpltCallback(void);
void InitPrinterIntrupt(void);
void FlushPrinterRxBuffer(void);
void InitPrinter(UART_HandleTypeDef *pObj);
void SendBoldOnOffCMD(bool BoldOnOff);
void Startup_Print();
void Print_Header();
void Print_Footer();
#endif /* INC_PRINTER_H_ */
