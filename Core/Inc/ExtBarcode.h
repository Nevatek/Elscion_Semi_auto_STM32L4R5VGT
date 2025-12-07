/*
 * ExtBarcode.h
 *
 *  Created on: May 22, 2022
 *      Author: Alvin
 */

#ifndef INC_EXTBARCODE_H_
#define INC_EXTBARCODE_H_

#define MAX_BARCODE_DATA_LENGTH 11
#define MAX_RX_BARCODE_DATABUFFER 16
#define TIME_OUT_FOR_BARCODE_UART_RX_IN_TICKS 50


void HandlerExternalBarcodeTask(void);
void DecodeBarcode(char *ptrData);
void Barocde_UART_RxCpltCallback(void);
void InitBarcodeRxIntrupt(void);
void FlushBarcodeRxBuffer(void);
void InitalizeBarcodereader(void);

#endif /* INC_EXTBARCODE_H_ */
