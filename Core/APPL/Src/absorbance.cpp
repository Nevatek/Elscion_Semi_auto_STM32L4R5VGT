/*
 * absorbance.cpp
 *
 *  Created on: May 3, 2023
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "math.h"
#include "string.h"
#include "../APPL/Inc/absorbance.h"
#include "../APPL/Inc/Testrun.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "../../APPL/Inc/NonVolatileMemory.h"

#define DEFAULT_ERROR_OD (-1)
#define DATA_PREV_MEAS (5)
float HyperActive_deltabs_extrapolated = 0;
bool HyperActiveSampleflag = false;

extern bool extrapolated_ADC;
extern uint8_t Gainchange_delay;
extern UART_HandleTypeDef huart2;

static float CalCulateOD_EndPoint(uint16_t TestId);
static float CalCulateOD_FixedTime(uint16_t TestId);
static float CalCulateOD_Kinetics(uint16_t TestId);
static float CalCulateOD_Turbidimetry(uint16_t TestId);
static float Calculate_Coagulationtime(uint16_t TestId);
float CalculateMaxDeltaAbsForHyperactiveSample(uint16_t TestId);
float CalculateMaxDeltaAbsTurbi(uint16_t TestId);
void SmoothAbsData(float *smoothed, const float *original, int totalPoints, int kernelSize);
float CalculateR2(float* data, int start, int size);
float PredictABS(float *buffer, size_t dataCount, size_t numPoints);
//static float CalCulateResultAbs_EndPoint(uint16_t TestId , float Sample_OD);
//static float CalCulateResultAbs_FixedTime(uint16_t TestId , float Sample_OD);
//static float CalCulateResultAbs_Kinetics(uint16_t TestId , float Sample_OD);
//static float CalCulateResultAbs_Turbidimetry(uint16_t TestId , float Sample_OD);

float sigmoidal(float x, float a, float b, float c, float d);
float objectiveFunction(float *y, float a, float b, float c, float d, int numPoints, int startIndex);
void nelderMead(float *y, int numPoints, float *params, int startIndex);
float interpolateMidpointTime(float *y, float midpointValue, int numPoints, int startIndex);

/*
 * Function to calculate light to absorbance
 * parameters : Di water ADC , Sample ADC , Dark ADC , Di water gain used in resistance , Sample Gain used in resistance
 * Return : Absorbance (float)
 */


extern UART_HandleTypeDef huart2; // Or whichever UART you use

void SendOD_Uart(uint16_t TestId)
{
    uint16_t MaxNumOfMeasurement =
        (GetInstance_TestParams()[TestId].MeasurementTimeSec +
         GetInstance_TestParams()[TestId].IncubationTimeSec);

    char txBuffer[32];
    float Abs = 0;
    uint16_t ADC = 0;

    memset(txBuffer, 0, sizeof(txBuffer));
    sprintf(txBuffer, "ADC Values:\n");
    HAL_UART_Transmit(&huart2, (uint8_t*)txBuffer, sizeof(txBuffer), HAL_MAX_DELAY);


    for (int nI = 0; nI < MaxNumOfMeasurement; nI++)
    {

    	memset(txBuffer, 0, sizeof(txBuffer));
        ADC = stcTestData.PrimaryAdcBuffer[nI];
        sprintf(txBuffer, "%u\n",ADC);
        HAL_UART_Transmit(&huart2, (uint8_t*)txBuffer, sizeof(txBuffer), HAL_MAX_DELAY);
    }

    memset(txBuffer, 0, sizeof(txBuffer));
    sprintf(txBuffer, "Absorbance Values:\n");
    HAL_UART_Transmit(&huart2, (uint8_t*)txBuffer, sizeof(txBuffer), HAL_MAX_DELAY);

    for (int nI = 0; nI < MaxNumOfMeasurement; nI++)
    {
    	memset(txBuffer, 0, sizeof(txBuffer));
        Abs = stcTestData.PrimaryAbsBuffer[nI];
        sprintf(txBuffer, "%.4f\n", Abs);
        HAL_UART_Transmit(&huart2, (uint8_t*)txBuffer, sizeof(txBuffer), HAL_MAX_DELAY);
    }
}



float calculateLightToAbsorbance(uint16_t DiWaterAdc , uint16_t MeasuredAdc ,
		uint16_t DarkAdc , float DiWaterGain, float SampleGain, uint32_t SampleGainDarkADC)
{
    float Abs = 0;

    SampleGainDarkADC = DarkAdc;

    if(MeasuredAdc <= SampleGainDarkADC)
    {
        MeasuredAdc = SampleGainDarkADC + 1;
    }
     if(DiWaterAdc <= DarkAdc)
    {
    	 DiWaterAdc = DarkAdc + 1;
    }
    float numerator = ((float)DiWaterAdc - (float)DarkAdc) / DiWaterGain;
    float denominator = ((float)MeasuredAdc - (float)SampleGainDarkADC) / SampleGain;
    if (numerator > 0 && denominator > 0)
    {
        Abs = (float)log10(numerator / denominator);
    }
    else
    {
        Abs = 0;  // Or some default value that indicates an error
    }

    if(extrapolated_ADC == true && stcTestData.AdcDataFetchCount > DATA_PREV_MEAS && Gainchange_delay > 0)
    {
    	if(stcTestData.CurrentTestStatus == enTestStatus_PrimaryMeasurement || stcTestData.CurrentTestStatus == enTestStatus_Incubation)
    	{
    		Abs = PredictABS(stcTestData.PrimaryAbsBuffer , stcTestData.AdcDataFetchCount , 5);
    	}
    	else if(stcTestData.CurrentTestStatus == enTestStatus_SecondaryMeasurement)
    	{
    		Abs = PredictABS(stcTestData.SecondaryAbsBuffer , stcTestData.AdcDataFetchCount , 5);
    	}
    	Gainchange_delay--;
    }
    else
    {
    	extrapolated_ADC = false;
    }

    return Abs;
}
/*
 * Function to calculate optical density
 * parameters : Test ID (uint16_t)
 * Return : Optical density (float)
 */

float PredictABS(float *buffer, size_t dataCount, size_t numPoints)
{
    if (dataCount < numPoints) {
        // Not enough data to predict, return the last known value
        return buffer[dataCount - 1];
    }

    // Calculate the slope (rate of change) and intercept for linear regression
    float sumX = 0.0f, sumY = 0.0f, sumXY = 0.0f, sumX2 = 0.0f;
    for (size_t i = 0; i < numPoints; i++) {
        size_t index = dataCount - numPoints + i;
        float x = i ; // Time in milliseconds
        float y = (float)buffer[index];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }

    float slope = (numPoints * sumXY - sumX * sumY) / (numPoints * sumX2 - sumX * sumX);
    float intercept = (sumY - slope * sumX) / numPoints;

    // Predict the ADC value for the next time step
    float predictionX = (numPoints); // Predict for the next time step
    float predictedValue = slope * predictionX + intercept;

    // Ensure predicted value is within valid ADC range
    if (predictedValue < 0) {
        predictedValue = 0;

    } else if (predictedValue > 4000) { // Assuming 12-bit ADC
        predictedValue = 4000;
    }
    return (float)predictedValue;
}
float CalculateOD(uint16_t TestId)
{
  float OD = 0;
  switch(GetInstance_TestParams()[TestId].TestProcedureType)
  {
	  case en_Endpoint:
	  {
		  OD = CalCulateOD_EndPoint(TestId);
	  }break;
	  case en_FixedTime:
	  {
		  OD = CalCulateOD_FixedTime(TestId);
	  }break;
	  case en_Kinetics:
	  {
		  OD = CalCulateOD_Kinetics(TestId);
	  }break;
	  case en_Turbidimetry:
	  {
		  OD = CalCulateOD_Turbidimetry(TestId);
	  }break;
	  case en_CoagulationTest:
	  {
		  OD = Calculate_Coagulationtime(TestId);
	  }break;
	  case en_Absorbance:
	  {
		  OD = CalCulateOD_EndPoint(TestId);
	  }break;
	  default:
	  {
		  OD = float(DEFAULT_ERROR_OD);
	  }break;
  }
  return OD;//Returning optical density
}
/***************************************************OD Calculation - START************************************************/
/*Function Calculate OD - Endpoint*/
float CalCulateOD_EndPoint(uint16_t TestId)
{
	float OD = 0;
	uint16_t ReadingStartLoc = GetInstance_TestParams()[TestId].IncubationTimeSec;
	float PrimaryAvgAbs = 0 , SecondaryAvgAbs = 0;
	uint16_t MaxNumOfMeasurement = GetInstance_TestParams()[TestId].MeasurementTimeSec;//(For end point max measurements is 5 by default)
	for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)//Loop to calculate sum of all abs.
	{
		 PrimaryAvgAbs += stcTestData.PrimaryAbsBuffer[ReadingStartLoc + nI];//Summing all abs
	}
	PrimaryAvgAbs /= MaxNumOfMeasurement;//dividing with max measurements to find average (For end point max measurements is 5 by default)

	/*For test with secondary filter*/
	/*Bi chromatic end point*/
	 if(en_FilterHome != GetInstance_TestParams()[TestId].SecWavelength)//if secondary wavelength present
	 {
		 for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)//Loop to calculate sum of all abs.
		 {
			 SecondaryAvgAbs += stcTestData.SecondaryAbsBuffer[nI];//Summing all abs
		 }
		 SecondaryAvgAbs /= MaxNumOfMeasurement;//dividing with max measurements to find average (For end point max measurements is 5 by default)
		 OD = PrimaryAvgAbs  - SecondaryAvgAbs;//Calculation of final OD
	 }
	 else
	 {
		 OD = PrimaryAvgAbs;//Final OD == average of primary abs if secondary wavelength is not there.
	 }
	return OD;
}
/*Function Calculate OD - Fixed time*/
float CalCulateOD_FixedTime(uint16_t TestId)
{
	float OD = 0;
	uint16_t ReadingStartLoc = GetInstance_TestParams()[TestId].IncubationTimeSec;
	uint16_t MaxNumOfMeasurement = (GetInstance_TestParams()[TestId].MeasurementTimeSec + GetInstance_TestParams()[TestId].IncubationTimeSec) - 1;
	OD = (stcTestData.PrimaryAbsBuffer[MaxNumOfMeasurement] - stcTestData.PrimaryAbsBuffer[ReadingStartLoc]);
	return OD;
}

/*Function Calculate OD - Kinetics*/
float CalCulateOD_Kinetics(uint16_t TestId)
{
	float OD = 0;
	float KineticAvgTimeX = 0 , KineticAvgAbsY = 0;
    float SumOfxXyY = 0;
    float SumOfxX2 = 0;
    HyperActive_deltabs_extrapolated = 0;

    uint16_t ReadingStartLoc = GetInstance_TestParams()[TestId].IncubationTimeSec;
    uint16_t MaxNumOfMeasurement = GetInstance_TestParams()[TestId].MeasurementTimeSec;
    uint16_t TotalMinutes = MaxNumOfMeasurement / 60;

    /*Calculation of avg time (x')*/
    for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)//Loop to calculate average time
    {
	    KineticAvgTimeX += (nI + 1);//summing with time values.
    }
    KineticAvgTimeX /= MaxNumOfMeasurement;//divideing to calculate average time.

    /*Calculation of avg absorbance (y')*/
    for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)//Loop to calculate sum of all abs.
    {
  	    KineticAvgAbsY += stcTestData.PrimaryAbsBuffer[ReadingStartLoc + nI];//Summing all abs
    }
    KineticAvgAbsY /= MaxNumOfMeasurement;//dividing with max measurements to find average.

     /*loop for summing of Multiplication of (x - x') * (y - y') and Calculation of (x - x') Square*/
     for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)//Loop
     {
	     /*Calculation of sum of all (x - x') * (y - y')*/
	     SumOfxXyY += (((nI + 1) - KineticAvgTimeX) * (stcTestData.PrimaryAbsBuffer[ReadingStartLoc + nI] - KineticAvgAbsY));//xX[nI] * yY[nI];

	     /*Calculation of sum of all (x - x') Square*/
	     SumOfxX2 += (((nI + 1) - KineticAvgTimeX) * ((nI + 1) - KineticAvgTimeX));//xX[nI] * xX[nI];
     }

#if 0/*Commented because (y - y')2 seems to be no use individually in equation*/
  /*Calculation of (y - y') Square*/
  for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)//Loop
  {
	  yY2[nI] = yY[nI] * yY[nI];
  }
#endif

     //Calculation of bSlope : Summ of((x - x') * (y - y')) / Sum of(x - x')2
     float bSlope = (SumOfxXyY / SumOfxX2); // changed to min // slope abs value per second. Using this value, we need to calculate the final result

     /*Converting to minutes*/
     OD = (bSlope * 60);//Abs/min -> min converting to 60sec(1min)

    /*Calculatoin starts for detecitng the sample is high or linearity of hte sample is less */

     // Calculate delta absorbance per minute and check for non-linearity
     bool NonLinearflag = false;

     for (int minute = 1; minute < TotalMinutes; minute++)
     {
         float DeltaAbsMin1 = 0;
         float DeltaAbsMin2 = 0;

         // Calculate DeltaAbsMin1 for current minute
         for (int sec = (minute - 1) * 60; sec < minute * 60; sec++)
         {
             DeltaAbsMin1 += ((stcTestData.PrimaryAbsBuffer[ReadingStartLoc + sec] - stcTestData.PrimaryAbsBuffer[ReadingStartLoc + sec - 1]));
         }

         // Calculate DeltaAbsMin2 for the next minute
         for (int sec = minute * 60; sec < (minute + 1) * 60; sec++)
         {
             DeltaAbsMin2 += ((stcTestData.PrimaryAbsBuffer[ReadingStartLoc + sec] - stcTestData.PrimaryAbsBuffer[ReadingStartLoc + sec -1]));
         }

		 float DeltaAbs = (DeltaAbsMin1 - DeltaAbsMin2);
		 float NonLinearityPercentage = (DeltaAbs / OD) * 100;

		 if (((NonLinearityPercentage > 20.0f) || (NonLinearityPercentage < -20.0f)) && ((OD > 0.15f) || (OD < -0.15f)))
		 {
			 NonLinearflag = true;
			 break;
		 }
     }

     uint16_t TotalMeasurements = ReadingStartLoc + MaxNumOfMeasurement;
     // Calculate delta absorbance for both incubation and measurement periods
     float sumDeltaAbsIncubation = 0;
     float sumDeltaAbsMeasurement = 0;

     for (int nI = 1; nI < TotalMeasurements; nI++)
     {
     	float deltaAbs = (stcTestData.PrimaryAbsBuffer[nI] - stcTestData.PrimaryAbsBuffer[nI - 1]);
         if (nI <= ReadingStartLoc)
         {
             sumDeltaAbsIncubation += deltaAbs;
         }
         else
         {
             sumDeltaAbsMeasurement += deltaAbs;
         }
     }
     float avgDeltaAbsIncubation = sumDeltaAbsIncubation / (ReadingStartLoc);
     float avgDeltaAbsMeasurement = sumDeltaAbsMeasurement / MaxNumOfMeasurement;

     // Calculate R2 values for incubation and measurement periods
     float R2_Incubation = CalculateR2(stcTestData.PrimaryAbsBuffer, 0, ReadingStartLoc);
     float R2_Measurement = CalculateR2(stcTestData.PrimaryAbsBuffer, ReadingStartLoc, MaxNumOfMeasurement);

     // Check for hyperactivity by comparing average delta absorbance values
// 	if (((avgDeltaAbsIncubation > (avgDeltaAbsMeasurement * 1.25)) && (R2_Incubation > R2_Measurement)) || ((R2_Measurement < 0.95) && (R2_Incubation > R2_Measurement)) || NonLinearflag == true)
// 	{
 	 if (((avgDeltaAbsIncubation > (avgDeltaAbsMeasurement * 2)) && (R2_Incubation > R2_Measurement) && ((OD > 0.15f) || (OD < -0.15f))) || NonLinearflag == true)
 	 {
 		HyperActiveSampleflag = true;
 		HyperActive_deltabs_extrapolated = CalculateMaxDeltaAbsForHyperactiveSample(stcTestData.TestId);
 	 }

     if(OD > HyperActive_deltabs_extrapolated) // Comparing OD and extrapolated OD
     {
    	 HyperActiveSampleflag = false;
     }
     else if(HyperActiveSampleflag == true)
     {
    	 OD = HyperActive_deltabs_extrapolated; // if it is hyperactve sample copying OD
     }

	 if(OD < 0)    // to avoid displaying negative result mult * -1 only for decreasing curve.
	 {
		 OD *=(-1);
	 }
	 return OD;
}

float CalculateR2(float* data, int start, int size)
{
    float avgX = 0, avgY = 0;
    float sumXY = 0;
    float sumX2 = 0;
    float sumSquaredResiduals = 0;
    float totalSumOfSquares = 0;

    for (int i = 0; i < size; i++)
    {
        avgX += (i + 1);
        avgY += data[start + i];
    }
    avgX /= size;
    avgY /= size;

    for (int i = 0; i < size; i++)
    {
        float x = (i + 1);
        float y = data[start + i];

        sumXY += ((x - avgX) * (y - avgY));
        sumX2 += ((x - avgX) * (x - avgX));

        float predictedY = avgY + ((x - avgX) * (sumXY / sumX2));
        sumSquaredResiduals += pow(y - predictedY, 2);
        totalSumOfSquares += pow(y - avgY, 2);
    }

    return 1 - (sumSquaredResiduals / totalSumOfSquares);
}

float CalculateMaxDeltaAbsForHyperactiveSample(uint16_t TestId)
{
    float maxR2 = 0;
    float maxDeltaAbsPerMin = 0;
    int windowSize = 30; // Size of the window to analyze
    int totalPoints = GetInstance_TestParams()[TestId].IncubationTimeSec + GetInstance_TestParams()[TestId].MeasurementTimeSec;

    // Ensure there are enough data points to perform the analysis
    if (totalPoints < windowSize)
    {
        return 0; // Not enough data points to analyze
    }
    for (int start = 0; start < totalPoints - windowSize; start++)
    {
        float KineticAvgTimeX = 0, KineticAvgAbsY = 0;
        float SumOfxXyY = 0;
        float SumOfxX2 = 0;
        float SumOfSquaredResiduals = 0;
        float TotalSumOfSquares = 0;
        // Calculate average time (x') and average absorbance (y') for the current window
        for (int i = 0; i < windowSize; i++)
        {
            KineticAvgTimeX += (i + 1);
            KineticAvgAbsY += stcTestData.PrimaryAbsBuffer[start + i];
        }
        KineticAvgTimeX /= windowSize;
        KineticAvgAbsY /= windowSize;
        // Calculate the necessary summations for R² calculation
        for (int i = 0; i < windowSize; i++)
        {
            float time = (i + 1);
            float absorbance = stcTestData.PrimaryAbsBuffer[start + i];

            SumOfxXyY += ((time - KineticAvgTimeX) * (absorbance - KineticAvgAbsY));
            SumOfxX2 += ((time - KineticAvgTimeX) * (time - KineticAvgTimeX));

            float predictedY = KineticAvgAbsY + ((time - KineticAvgTimeX) * (SumOfxXyY / SumOfxX2));
            SumOfSquaredResiduals += pow(absorbance - predictedY, 2);
            TotalSumOfSquares += pow(absorbance - KineticAvgAbsY, 2);
        }
        // Calculate R² for the current window
        float R2 = 1 - (SumOfSquaredResiduals / TotalSumOfSquares);

        // Update maxR2 if the current R² is higher and calculate delta absorbance per minute
        if (R2 > maxR2)
        {
            maxR2 = R2;
            float sumDeltaAbs = 0;
            for (int i = 1; i < windowSize; i++)
            {
                float deltaAbs = (stcTestData.PrimaryAbsBuffer[start + i] - stcTestData.PrimaryAbsBuffer[start + i - 1]);
                sumDeltaAbs += deltaAbs;
            }
            maxDeltaAbsPerMin = (sumDeltaAbs / (windowSize - 1)) * 60; // Convert to per minute
        }
    }
//    if(maxDeltaAbsPerMin < 0)    // to avoid displaying negative result mult * -1 only for decreasing curve.
//    {
//    	maxDeltaAbsPerMin *=(-1);
//    }
    return maxDeltaAbsPerMin; // Return the delta absorbance per minute corresponding to max R²
}

float CalCulateOD_Kinetics_RealTime(uint16_t TestId , uint16_t CurrentTimeSec)
{
	float OD = 0;
	float KineticAvgTimeX = 0 , KineticAvgAbsY = 0;
    float SumOfxXyY = 0;
    float SumOfxX2 = 0;
    uint16_t ReadingStartLoc = 0;
    uint16_t MaxNumOfMeasurement = 0;
    if(GetInstance_TestParams()[TestId].IncubationTimeSec < 15)
    {
        ReadingStartLoc = 30;
        MaxNumOfMeasurement = CurrentTimeSec - ReadingStartLoc;
    }
    else
    {
        ReadingStartLoc = GetInstance_TestParams()[TestId].IncubationTimeSec;
        MaxNumOfMeasurement = CurrentTimeSec - GetInstance_TestParams()[TestId].IncubationTimeSec;
    }

    /*Calculation of avg time (x')*/
    for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)//Loop to calculate average time
    {
	    KineticAvgTimeX += (nI + 1);//summing with time values.
    }
    KineticAvgTimeX /= MaxNumOfMeasurement;//divideing to calculate average time.

    /*Calculation of avg absorbance (y')*/
    for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)//Loop to calculate sum of all abs.
    {
  	    KineticAvgAbsY += stcTestData.PrimaryAbsBuffer[ReadingStartLoc + nI];//Summing all abs
    }
    KineticAvgAbsY /= MaxNumOfMeasurement;//dividing with max measurements to find average.

     /*loop for summing of Multiplication of (x - x') * (y - y') and Calculation of (x - x') Square*/
     for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)//Loop
     {
	     /*Calculation of sum of all (x - x') * (y - y')*/
	     SumOfxXyY += (((nI + 1) - KineticAvgTimeX) * (stcTestData.PrimaryAbsBuffer[ReadingStartLoc + nI] - KineticAvgAbsY));//xX[nI] * yY[nI];

	     /*Calculation of sum of all (x - x') Square*/
	     SumOfxX2 += (((nI + 1) - KineticAvgTimeX) * ((nI + 1) - KineticAvgTimeX));//xX[nI] * xX[nI];
     }

#if 0/*Commented because (y - y')2 seems to be no use individually in equation*/
  /*Calculation of (y - y') Square*/
  for(int nI = 0 ; nI < MaxNumOfMeasurement ; nI++)//Loop
  {
	  yY2[nI] = yY[nI] * yY[nI];
  }
#endif

     //Calculation of bSlope : Summ of((x - x') * (y - y')) / Sum of(x - x')2
     float bSlope = (SumOfxXyY / SumOfxX2); // changed to min // slope abs value per second. Using this value, we need to calculate the final result

     /*Converting to minutes*/
     OD = (bSlope * 60);//Abs/min -> min converting to 60sec(1min)

     if(OD < 0)    // to avoid displaying negative result mult * -1 only for decreasing curve.
     {
    	 OD *=(-1);
     }
	 return OD;
}

float CalCulateOD_Turbidimetry(uint16_t TestId)
{
	float OD = 0;
//	uint16_t ReadingStartLoc = GetInstance_TestParams()[TestId].IncubationTimeSec;
//	uint16_t MaxNumOfMeasurement = (GetInstance_TestParams()[TestId].MeasurementTimeSec + GetInstance_TestParams()[TestId].IncubationTimeSec) - 1;
//	OD = (stcTestData.PrimaryAbsBuffer[MaxNumOfMeasurement] - stcTestData.PrimaryAbsBuffer[ReadingStartLoc]);
	OD = CalculateMaxDeltaAbsTurbi(TestId);
	return OD;
}


//float CalculateMaxDeltaAbsTurbi(uint16_t TestId)
//{
//	float MaxAbsLimit = GetInstance_TestParams()[stcTestData.TestId].AbsorbanceLimit[en_RangeHigh];
//	float dAbs = 0;
//	uint16_t ReadingStartLoc = GetInstance_TestParams()[TestId].IncubationTimeSec;
//	uint16_t MaxNumOfMeasurement = (GetInstance_TestParams()[TestId].MeasurementTimeSec + GetInstance_TestParams()[TestId].IncubationTimeSec) - 1;
//
//	float startAbs = stcTestData.PrimaryAbsBuffer[ReadingStartLoc];
//
//	    for (uint16_t i = ReadingStartLoc + 1; i < MaxNumOfMeasurement; ++i)
//	    {
//	        float currentAbs = stcTestData.PrimaryAbsBuffer[i];
//
//	        if (currentAbs > MaxAbsLimit)
//	        {
//	            float deltaAbs = currentAbs - startAbs;
//	            float deltaTimeSec = (float)(i - ReadingStartLoc);
//
//	            if (deltaTimeSec > 0)
//	            {
//	                // Convert rate to Abs/min
//	                float dAbs = (deltaAbs / deltaTimeSec) * GetInstance_TestParams()[TestId].MeasurementTimeSec;
//	                return dAbs;
//	            }
//	        }
//	    }
//
//	    // No value crossed the threshold – return final dAbs only
//	    dAbs = stcTestData.PrimaryAbsBuffer[MaxNumOfMeasurement] - startAbs;
//	    return dAbs;
//}


float CalculateMaxDeltaAbsTurbi(uint16_t TestId)
{
    float MaxAbsLimit = GetInstance_TestParams()[TestId].TurbiAbsLimit;
    float dAbs = 0.0f;

    uint16_t ReadingStartLoc = GetInstance_TestParams()[TestId].IncubationTimeSec;
    uint16_t MaxNumOfMeasurement = ReadingStartLoc + GetInstance_TestParams()[TestId].MeasurementTimeSec - 1;

    float startAbs = stcTestData.PrimaryAbsBuffer[ReadingStartLoc];

    // Condition 1: If the first point itself is above limit, skip everything
    if (startAbs > MaxAbsLimit)
    {
        dAbs = stcTestData.PrimaryAbsBuffer[MaxNumOfMeasurement] - startAbs;
        return dAbs;
    }

    //  Condition 2: Start checking from ReadingStartLoc
    uint16_t CheckStart = ReadingStartLoc;

    for (uint16_t i = CheckStart; i <= MaxNumOfMeasurement - 5; ++i)
    {
        bool sixPointsAboveLimit = true;

        for (uint16_t j = 0; j < 6; ++j)
        {
            if (stcTestData.PrimaryAbsBuffer[i + j] <= MaxAbsLimit)
            {
                sixPointsAboveLimit = false;
                break;
            }
        }

        if (sixPointsAboveLimit)
        {
            float deltaAbs = stcTestData.PrimaryAbsBuffer[i] - startAbs;
            float deltaTimeSec = (float)(i - ReadingStartLoc);

            if (deltaTimeSec > 0)
            {
            	dAbs = (deltaAbs / deltaTimeSec) * GetInstance_TestParams()[TestId].MeasurementTimeSec;
                return dAbs;
            }
        }
    }

    // No threshold crossing found — return final dAbs
    dAbs = stcTestData.PrimaryAbsBuffer[MaxNumOfMeasurement] - startAbs;
    return dAbs;
}


//float CalculateMaxDeltaAbsTurbi(uint16_t TestId)
//{
//
//	float maxDeltaAbs = 0;
//    const int MinDataPoints = 30;
//
//    int totalPoints = GetInstance_TestParams()[TestId].IncubationTimeSec +
//                      GetInstance_TestParams()[TestId].MeasurementTimeSec;
//
//    if (totalPoints < MinDataPoints)
//    {
//    	uint16_t ReadingStartLoc = GetInstance_TestParams()[TestId].IncubationTimeSec;
//    	uint16_t MaxNumOfMeasurement = (GetInstance_TestParams()[TestId].MeasurementTimeSec + GetInstance_TestParams()[TestId].IncubationTimeSec) - 1;
//    	return (stcTestData.PrimaryAbsBuffer[MaxNumOfMeasurement] - stcTestData.PrimaryAbsBuffer[ReadingStartLoc]);
//    }
//
//    float smoothed[600] = {0};  // assuming max 512 data points
//
//    SmoothAbsData(smoothed, stcTestData.PrimaryAbsBuffer, totalPoints, 2);  // 5-point moving avg
//
//    int bestStart = -1, bestEnd = -1;
//    float bestR2 = -1.0f;
//
//    for (int start = 0; start <= totalPoints - MinDataPoints; start++)
//    {
//        for (int end = start + MinDataPoints; end < totalPoints; end++) // min 5 points to reduce noise
//        {
//            int n = end - start + 1;
//            float avgX = 0, avgY = 0;
//            for (int i = 0; i < n; i++)
//            {
//                avgX += i + 1;
//                avgY += smoothed[start + i];
//            }
//            avgX /= n;
//            avgY /= n;
//
//            float sumXY = 0, sumX2 = 0, ssRes = 0, ssTot = 0;
//            for (int i = 0; i < n; i++)
//            {
//                float x = i + 1;
//                float y = smoothed[start + i];
//                sumXY += (x - avgX) * (y - avgY);
//                sumX2 += (x - avgX) * (x - avgX);
//            }
//
//            float slope = sumXY / sumX2;
//
//            for (int i = 0; i < n; i++)
//            {
//                float x = i + 1;
//                float y = smoothed[start + i];
//                float yFit = avgY + slope * (x - avgX);
//                ssRes += powf(y - yFit, 2);
//                ssTot += powf(y - avgY, 2);
//            }
//
//            float R2 = (ssTot > 0.0f) ? (1.0f - (ssRes / ssTot)) : 0.0f;
//
//            // Favor longest, most linear region
//            if ((R2 > bestR2) || (fabsf(R2 - bestR2) < 0.0005f && (end - start) > (bestEnd - bestStart)))
//            {
//                bestR2 = R2;
//                bestStart = start;
//                bestEnd = end;
//            }
//        }
//    }
//
//    if (bestStart >= 0 && bestEnd > bestStart)
//    {
//        float sumDelta = 0;
//        for (int i = bestStart + 1; i <= bestEnd; i++)
//        {
//            sumDelta += smoothed[i] - smoothed[i - 1];
//        }
//
//        float slopePerPoint = sumDelta / (bestEnd - bestStart);
//
//        maxDeltaAbs = slopePerPoint * GetInstance_TestParams()[TestId].MeasurementTimeSec;
//
//        return maxDeltaAbs; // delta absorbance per second
//    }
//
//    return maxDeltaAbs; // Return the delta absorbance per minute corresponding to max R²
//}

void SmoothAbsData(float *smoothed, const float *original, int totalPoints, int kernelSize)
{
    for (int i = 0; i < totalPoints; i++)
    {
        float sum = 0;
        int count = 0;
        for (int k = -kernelSize; k <= kernelSize; k++)
        {
            int idx = i + k;
            if (idx >= 0 && idx < totalPoints)
            {
                sum += original[idx];
                count++;
            }
        }
        smoothed[i] = sum / count;
    }
}


//float CalculateMaxDeltaAbsTurbi(uint16_t TestId)
//{
//    float maxR2 = 0;
//    float maxDeltaAbs = 0;
//    int windowSize = 15; // Size of the window to analyze
//    int totalPoints = GetInstance_TestParams()[TestId].IncubationTimeSec + GetInstance_TestParams()[TestId].MeasurementTimeSec;
//
//    // Ensure there are enough data points to perform the analysis
//    if (totalPoints < windowSize)
//    {
//        return 0; // Not enough data points to analyze
//    }
//    for (int start = 5; start < totalPoints - windowSize; start++)
//    {
//        float KineticAvgTimeX = 0, KineticAvgAbsY = 0;
//        float SumOfxXyY = 0;
//        float SumOfxX2 = 0;
//        float SumOfSquaredResiduals = 0;
//        float TotalSumOfSquares = 0;
//        // Calculate average time (x') and average absorbance (y') for the current window
//        for (int i = 0; i < windowSize; i++)
//        {
//            KineticAvgTimeX += (i + 1);
//            KineticAvgAbsY += stcTestData.PrimaryAbsBuffer[start + i];
//        }
//        KineticAvgTimeX /= windowSize;
//        KineticAvgAbsY /= windowSize;
//        // Calculate the necessary summations for R² calculation
//        for (int i = 0; i < windowSize; i++)
//        {
//            float time = (i + 1);
//            float absorbance = stcTestData.PrimaryAbsBuffer[start + i];
//
//            SumOfxXyY += ((time - KineticAvgTimeX) * (absorbance - KineticAvgAbsY));
//            SumOfxX2 += ((time - KineticAvgTimeX) * (time - KineticAvgTimeX));
//
//            float predictedY = KineticAvgAbsY + ((time - KineticAvgTimeX) * (SumOfxXyY / SumOfxX2));
//            SumOfSquaredResiduals += pow(absorbance - predictedY, 2);
//            TotalSumOfSquares += pow(absorbance - KineticAvgAbsY, 2);
//        }
//        // Calculate R² for the current window
//        float R2 = 1 - (SumOfSquaredResiduals / TotalSumOfSquares);
//
//        // Update maxR2 if the current R² is higher and calculate delta absorbance per minute
//        if (R2 > maxR2)
//        {
//            maxR2 = R2;
//            float sumDeltaAbs = 0;
//            for (int i = 1; i < windowSize; i++)
//            {
//                float deltaAbs = (stcTestData.PrimaryAbsBuffer[start + i] - stcTestData.PrimaryAbsBuffer[start + i - 1]);
//                sumDeltaAbs += deltaAbs;
//            }
//            maxDeltaAbs = (sumDeltaAbs / (windowSize - 1)) * GetInstance_TestParams()[TestId].MeasurementTimeSec; // Convert to per minute
//        }
//    }
//    return maxDeltaAbs; // Return the delta absorbance per minute corresponding to max R²
//}

/*Function Calculate Coagulation time*/
float Calculate_Coagulationtime(uint16_t TestId)
{
		float OD = 0;
		uint16_t ReadingStartLoc = 0;
		uint16_t MaxNumOfMeasurement = GetInstance_TestParams()[TestId].MeasurementTimeSec;
		float params[] = {1.0, 0.0, 1.0, 1.0};
    	// Optimize parameters using Nelder-Mead, starting from the 11th data point
    	nelderMead(stcTestData.PrimaryAbsBuffer, MaxNumOfMeasurement, params, ReadingStartLoc);
    	// Find the midpoint of the slope
    	float midpointValue = OD = sigmoidal((MaxNumOfMeasurement - ReadingStartLoc) / 2.0, params[0], params[1], params[2], params[3]);
    	// Find the time corresponding to the midpoint using interpolation
    	float midpointTime = interpolateMidpointTime(stcTestData.PrimaryAbsBuffer, midpointValue, MaxNumOfMeasurement, ReadingStartLoc);
    	stcTestData.Result = midpointTime;
    	return OD;
}

// Sigmoidal function
float sigmoidal(float x, float a, float b, float c, float d) {
    return a / (1 + exp(-c * (x - d))) + b;
}

// Calculate the sum of squared differences between the sigmoidal function and data
float objectiveFunction(float *y, float a, float b, float c, float d, int numPoints, int startIndex) {
	float sum = 0.0;
    for (int i = startIndex; i < numPoints; i++) {
    	float diff = y[i] - sigmoidal(i, a, b, c, d);
        sum += diff * diff;
    }
    return sum;
}

// Nelder-Mead algorithm for optimization
void nelderMead(float *y, int numPoints, float *params, int startIndex) {
	float alpha = 1.0;
	float beta = 0.5;
	float gamma = 2.0;

	float simplex[3][4];
    simplex[0][0] = params[0];
    simplex[0][1] = params[1];
    simplex[0][2] = params[2];
    simplex[0][3] = params[3];

    simplex[1][0] = params[0] + 1.0;
    simplex[1][1] = params[1];
    simplex[1][2] = params[2];
    simplex[1][3] = params[3];

    simplex[2][0] = params[0];
    simplex[2][1] = params[1] + 1.0;
    simplex[2][2] = params[2];
    simplex[2][3] = params[3];

    int bestIndex, worstIndex, secondWorstIndex;

    while (1) {
        // Sort simplex by function values
    	float values[3];
        for (int i = 0; i < 3; i++) {
            values[i] = objectiveFunction(y, simplex[i][0], simplex[i][1], simplex[i][2], simplex[i][3], numPoints, startIndex);
        }

        bestIndex = 0;
        worstIndex = 0;
        secondWorstIndex = 0;

        for (int i = 1; i < 3; i++) {
            if (values[i] < values[bestIndex]) {
                bestIndex = i;
            } else if (values[i] > values[worstIndex]) {
                worstIndex = i;
            }
        }

        for (int i = 0; i < 3; i++) {
            if (i != bestIndex && (i != worstIndex || values[i] > values[secondWorstIndex])) {
                secondWorstIndex = i;
            }
        }

        // Calculate centroid (excluding worst)
        float centroid[4];
        for (int i = 0; i < 4; i++) {
            centroid[i] = (simplex[0][i] + simplex[1][i] + simplex[2][i] - simplex[worstIndex][i]) / 2.0;
        }

        // Reflect worst point through centroid
        float reflected[4];
        for (int i = 0; i < 4; i++) {
            reflected[i] = centroid[i] + alpha * (centroid[i] - simplex[worstIndex][i]);
        }

        float reflectedValue = objectiveFunction(y, reflected[0], reflected[1], reflected[2], reflected[3], numPoints, startIndex);

        // Check if reflection is better than the second worst
        if (reflectedValue < values[secondWorstIndex]) {
            // If reflection is better than the best, attempt expansion
            if (reflectedValue < values[bestIndex]) {
            	float expanded[4];
                for (int i = 0; i < 4; i++) {
                    expanded[i] = centroid[i] + gamma * (reflected[i] - centroid[i]);
                }

                float expandedValue = objectiveFunction(y, expanded[0], expanded[1], expanded[2], expanded[3], numPoints, startIndex);

                if (expandedValue < reflectedValue) {
                    // Accept the expansion
                    for (int i = 0; i < 4; i++) {
                        simplex[worstIndex][i] = expanded[i];
                    }
                } else {
                    // Accept the reflection
                    for (int i = 0; i < 4; i++) {
                        simplex[worstIndex][i] = reflected[i];
                    }
                }
            } else {
                // Accept the reflection
                for (int i = 0; i < 4; i++) {
                    simplex[worstIndex][i] = reflected[i];
                }
            }
        } else {
            // If reflection is worse than the second worst, perform contraction
        	float contracted[4];
            for (int i = 0; i < 4; i++) {
                contracted[i] = centroid[i] + beta * (simplex[worstIndex][i] - centroid[i]);
            }

            float contractedValue = objectiveFunction(y, contracted[0], contracted[1], contracted[2], contracted[3], numPoints, startIndex);

            if (contractedValue < values[worstIndex]) {
                // Accept the contraction
                for (int i = 0; i < 4; i++) {
                    simplex[worstIndex][i] = contracted[i];
                }
            } else {
                // Shrink the simplex towards the best point
                for (int i = 0; i < 3; i++) {
                    if (i != bestIndex) {
                        for (int j = 0; j < 4; j++) {
                            simplex[i][j] = simplex[bestIndex][j] + 0.5 * (simplex[i][j] - simplex[bestIndex][j]);
                        }
                    }
                }
            }
        }

        // Check convergence (arbitrary threshold)
        float maxDiff = 0.0;
        for (int i = 0; i < 4; i++) {
        	float diff = (simplex[bestIndex][i] - simplex[secondWorstIndex][i]);
            if (diff > maxDiff) {
                maxDiff = diff;
            }
        }

        if (maxDiff < 1e-6) {
            break;
        }
    }

    // Set the optimized parameters
    for (int i = 0; i < 4; i++) {
        params[i] = simplex[bestIndex][i];
    }
}

// Interpolate to find the exact time corresponding to the midpoint value
float interpolateMidpointTime(float *y, float midpointValue, int numPoints, int startIndex) {
    for (int i = startIndex; i < numPoints - 1; i++) {
    	float currentAbsorbance = y[i];
    	float nextAbsorbance = y[i + 1];

        if ((currentAbsorbance <= midpointValue && midpointValue <= nextAbsorbance) ||
            (currentAbsorbance >= midpointValue && midpointValue >= nextAbsorbance)) {

            float t = i + (midpointValue - currentAbsorbance) / (nextAbsorbance - currentAbsorbance);
            return t - startIndex;
        }
    }

    // If midpointValue is not within the range, return -1 to indicate an error
    return -1.0;
}

/***************************************************OD Calculation - END**************************************************/

/***************************************************Result Absorbance Calculation - START*********************************/
/*
 * Function to calculate result absorbance
 * parameters : Test ID (uint16_t)
 * Return : Result absorbance (float)
 */
//
//float CalculateReagentAbsorbance(uint16_t TestId , float Reagent_OD)
//{
//    float ResAbs = 0;
//	float AbsReagent = Sample_OD;
//	float AbsSampleBlank = stcTestData.SampleBlankAbs;
//
//	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
//	{
//		float ResAbs = AbsSample - AbsSampleBlank;
//	}
//
//	/*Both sample blank and reagent blank is disabled*/
//	else
//	{
//		float ResAbs = AbsSample;
//	}
//	return ResAbs;
//}


//float CalculateResultAbsorbance(uint16_t TestId , float Sample_OD)
//{
//    float ResAbs = 0;
//	float AbsSample = Sample_OD;
//	float AbsSampleBlank = stcTestData.SampleBlankAbs;
//
//	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
//	{
//		float ResAbs = AbsSample - AbsSampleBlank;
//	}
//
//	/*Both sample blank and reagent blank is disabled*/
//	else
//	{
//		float ResAbs = AbsSample;
//	}
//	return ResAbs;
//}



///*Calculate result absorbance - End point*/
//float CalCulateResultAbs_EndPoint(uint16_t TestId , float Sample_OD)
//{
//	/* ((Abs of sample – Abs sample blank) – (Abs Reagent - Abs Reagent blank)) */
//	float AbsSample = Sample_OD;
//	float AbsSampleBlank = stcTestData.SampleBlankAbs;
//	float AbsReagent = GetInstance_TestParams()[TestId].ReagentAbs;
//	float AbsReagentBlank = GetInstance_TestParams()[TestId].ReagentBlankAbs;
//	//	float ResAbs = ((AbsSample - AbsSampleBlank) - (AbsReagent - AbsReagentBlank));
//
//	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable && true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//	{
//		float ResAbs = ((AbsSample - AbsSampleBlank) - (AbsReagent - AbsReagentBlank));
//		return ResAbs;
//	}
//	else if(false == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable &&
//			true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//	{
//		float ResAbs = ((AbsSample) - (AbsReagentBlank));
//		return ResAbs;
//	}
//	else if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable &&
//			false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//	{
//		float ResAbs = ((AbsSample - AbsSampleBlank));
//		return ResAbs;
//	}
//	/*Both sample blank and reagent blank is disabled*/
//	else
//	{
//		float ResAbs = AbsSample;
//		return ResAbs;
//	}
//
//}
///*Calculate result absorbance - Fixed time*/
//float CalCulateResultAbs_FixedTime(uint16_t TestId , float Sample_OD)
//{
//	/* (((A2-A1) Abs of sample –(A2-A1) Abs sample blank) – ((A2-A1) Abs Reagent - (A2-A1) Abs Reagent blank))  */
//	float AbsSample = Sample_OD;
//	float AbsSampleBlank = stcTestData.SampleBlankAbs;
//	float AbsReagent = GetInstance_TestParams()[TestId].ReagentAbs;
//	float AbsReagentBlank = GetInstance_TestParams()[TestId].ReagentBlankAbs;
////	float ResAbs = ((AbsSample - AbsSampleBlank) - (AbsReagent - AbsReagentBlank));
//
//	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable &&
//			true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//	{
//
//		float ResAbs = ((AbsSample - AbsSampleBlank) - (AbsReagent - AbsReagentBlank));
//		return ResAbs;
//	}
//	else if(false == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable &&
//			true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//	{
//		float ResAbs = ((AbsSample) - (AbsReagentBlank));
//		return ResAbs;
//	}
//	else if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable &&
//			false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//	{
//		float ResAbs = ((AbsSample - AbsSampleBlank));
//		return ResAbs;
//	}
//	/*Both sample blank and reagent blank is disabled*/
//	else
//	{
//		float ResAbs = AbsSample;
//		return ResAbs;
//	}
//}
///*Calculate result absorbance - Kinetics*/
//float CalCulateResultAbs_Kinetics(uint16_t TestId , float Sample_OD)
//{
//	/* ((Delta Abs of sample – Delta Abs sample blank) – (Delta Abs Reagent - Delta Abs Reagent blank))  */
//	float AbsSample = Sample_OD;
//	float AbsSampleBlank = stcTestData.SampleBlankAbs;
//	float AbsReagent = GetInstance_TestParams()[TestId].ReagentAbs;
//	float AbsReagentBlank = GetInstance_TestParams()[TestId].ReagentBlankAbs;
////	float ResAbs = ((AbsSample - AbsSampleBlank) - (AbsReagent - AbsReagentBlank));
//
//	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable &&
//			true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//	{
//
//		float ResAbs = ((AbsSample - AbsSampleBlank) - (AbsReagent - AbsReagentBlank));
//		return ResAbs;
//	}
//	else if(false == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable &&
//			true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//	{
//		float ResAbs = ((AbsSample) - (AbsReagentBlank));
//		return ResAbs;
//	}
//	else if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable &&
//			false == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//	{
//		float ResAbs = ((AbsSample - AbsSampleBlank));
//		return ResAbs;
//	}
//	/*Both sasmple blank and reagent blank is disabled*/
//	else
//	{
//		float ResAbs = AbsSample;
//		return ResAbs;
//	}
//
//}

//float CalculateStandardAbsorbance(uint16_t TestId , float Standard_OD)
//{
//	float ResAbs = 0;
//	float AbsStandard = Standard_OD;
////	float AbsStandardBlank = e_RunTimeStdBlankOd[stcTestData.SelectedStandard];
////	float AbsReagent = GetInstance_TestParams()[TestId].ReagentAbs;
////	float AbsReagentBlank = GetInstance_TestParams()[TestId].ReagentBlankAbs;
//
////	if(true == GetInstance_TestParams()[stcTestData.TestId].StandardBlankDoneFlag[stcTestData.SelectedStandard] &&
////		true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//
//	if(true == GetInstance_TestParams()[stcTestData.TestId].SampleBlankEnable)
//	{
//		float ResAbs = (AbsStandard - AbsStandardBlank);
//	}
//	/*Both sample blank and reagent blank is disabled*/
//	else
//	{
//		float ResAbs = AbsStandard;
//	}
//	return ResAbs;
//}

/***************************************************Result Absorbance Calculation - END***********************************/
