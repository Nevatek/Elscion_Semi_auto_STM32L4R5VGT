/*
 * calibration.h
 *
 *  Created on: May 3, 2023
 *      Author: Alvin
 */

#ifndef APPL_INC_CALIBRATION_H_
#define APPL_INC_CALIBRATION_H_


float Calibrate_3PL(uint16_t TestId, float targetY);
float CalibrateRuntime_3PL(uint16_t TestId, float targetY);

float CalibrateToResult(uint16_t TestId , float Sample_OD);
float Calibrate_4PL_initial_params(uint16_t TestId );
float Calibrate_4PL(uint16_t TestId, float targetY);

float CalibrateToRuntimeStdConc(uint16_t TestId , float std_OD);
float CalibrateRuntime_4PL_initial_params(uint16_t TestId );
float CalibrateRuntime_4PL(uint16_t TestId, float targetY);

float CalculateFactor(void);
float CalculateFactor_Graph(void);


#endif /* APPL_INC_CALIBRATION_H_ */
