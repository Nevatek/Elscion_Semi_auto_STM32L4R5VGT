/*
 * absorbance.h
 *
 *  Created on: May 3, 2023
 *      Author: Alvin
 */

#ifndef APPL_INC_ABSORBANCE_H_
#define APPL_INC_ABSORBANCE_H_

float CalculateOD(uint16_t TestId);
void SendOD_Uart(uint16_t TestId);
float CalculateResultAbsorbance(uint16_t TestId , float Sample_OD);
float calculateLightToAbsorbance(uint16_t DiWaterAdc , uint16_t MeasuredAdc ,
		uint16_t DarkAdc , float DiWaterGain, float SampleGain, uint32_t SampleGainDarkADC);
float CalCulateOD_Kinetics_RealTime(uint16_t TestId , uint16_t CurrentTimeSec);

#endif /* APPL_INC_ABSORBANCE_H_ */
