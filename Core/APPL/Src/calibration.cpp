/*
 * calibration.cpp
 *
 *  Created on: May 3, 2023
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "math.h"
#include "../APPL/Inc/calibration.h"
#include "../APPL/Inc/Testrun.h"
#include "../../Screens/Inc/CommonDisplayFunctions.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"

#define DEFAULT_ERROR_VALUE (-1)

extern float e_RunTimeStdOd[MAX_NUM_OF_STANDARDS];/*Buffer to store run time OD for each standard*/
extern float e_RunTimeStdBlankOd[MAX_NUM_OF_STANDARDS];/*Buffer to store run time OD for each standard blank*/

extern float e_Runtime4plA;
extern float e_Runtime4plB;
extern float e_Runtime4plC;
extern float e_Runtime4plD;
double e_Runtime_R2value_linear = 0.0;
double e_Runtime_R2value_cubic = 0.0;
double e_Runtime_R2value_4pl = 0.0;
double e_Runtime_R2value_3pl = 0.0;


static float Calibrate_Factor1Point(uint16_t TestId , float ResAbs);
static float Calibrate_2Point(uint16_t TestId , float ResAbs);
static float Calibrate_LinearRegression(uint16_t TestId , float ResAbs);
static float Calibrate_PointToPoint(uint16_t TestId , float ResAbs);
static float Calibrate_CubicSpline(uint16_t TestId , float ResAbs);

static float CalibrateRuntime_Factor1Point(uint16_t TestId , float ResAbs);
static float CalibrateRuntime_2Point(uint16_t TestId , float ResAbs);
static float CalibrateRuntime_LinearRegression(uint16_t TestId , float ResAbs);
static float CalibrateRuntime_PointToPoint(uint16_t TestId , float ResAbs);
static float CalibrateRuntime_CubicSpline(uint16_t TestId , float ResAbs);

using namespace std;


void refinedGridSearch(const double X[], const double Y[], int dataSize, double& A, double& B, double& C, double& D);

static double calculateMean(double* data, int size);
static double calculateR2(double* yData, double* yPred, int size) ;

struct CubicSpline {
    double a, b, c, d;
};

double evaluateCubicSpline(double xVal, CubicSpline spline[], int n, double x[]);
double evaluateCubicSpline_Inverse(double yVal, CubicSpline spline[], int n, double x[]);
void computeCubicSpline(double x[], double y[], int n, CubicSpline spline[]);

double calculateCorrelationCoefficient_cubicSpline(double actual[], double predicted[], int n);
double calculateR2CubicSpline(double x[], double y[], int n, CubicSpline spline[]);
double evaluateCubicSpline_r2(double x, CubicSpline cubicSpline[], int n, double dataPointsX[]);

/* This is only for sample and QC result calculation*/
float CalibrateToResult(uint16_t TestId , float Sample_OD)
{
	float Res = 0;

    float ResAbs = Sample_OD;
//	float AbsSample = Sample_OD;
//	float AbsSampleBlank = stcTestData.SampleBlankAbs;
//	float AbsReagent = GetInstance_TestParams()[TestId].ReagentAbs;
//	float AbsReagentBlank = GetInstance_TestParams()[TestId].ReagentBlankAbs;

//	if(true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
//	{
//		 ResAbs = ((AbsSample) - (AbsReagentBlank));
//	}
//	/*Both sample blank and reagent blank is disabled*/
//	else
//	{
//		 ResAbs = AbsSample;
//	}

	enCalibrationType CalibType = GetInstance_TestParams()[TestId].Calib;

	if(en_1PointLinear == CalibType || en_Factor == CalibType)/*Factor*/
	{
		Res = Calibrate_Factor1Point(TestId , ResAbs);
	}
	else if(en_2PointLinear == CalibType)
	{
		Res = Calibrate_2Point(TestId , ResAbs);
	}
	else if(en_PointToPoint == CalibType)
	{
		Res = Calibrate_PointToPoint(TestId , ResAbs);
	}
	else if(en_LinearRegression == CalibType)
	{
		Res = Calibrate_LinearRegression(TestId , ResAbs);
	}
	else if(en_3PL == CalibType)
	{
		Res = Calibrate_3PL(TestId , ResAbs);
	}
	else if(en_4PL == CalibType)
	{
		Res = Calibrate_4PL(TestId , ResAbs);
	}
	else if(en_CubicSpline == CalibType)
	{
		Res = Calibrate_CubicSpline(TestId , ResAbs);
	}
	else
	{
		Res = (float)(DEFAULT_ERROR_VALUE);
	}

	Res = Res * GetInstance_TestParams()[TestId].Corr_A + GetInstance_TestParams()[TestId].Corr_B;

	return Res;
}

/* This is only for standard curve plotting calculation*/

float CalibrateToRuntimeStdConc(uint16_t TestId , float std_OD)
{
	float Res = 0;
	float AbsSample = std_OD;

	enCalibrationType CalibType = GetInstance_TestParams()[TestId].Calib;

	if(en_1PointLinear == CalibType || en_Factor == CalibType)/*Factor*/
	{
		Res = Calibrate_Factor1Point(TestId , AbsSample);
	}
	else if(en_2PointLinear == CalibType)
	{
		Res = CalibrateRuntime_2Point(TestId , AbsSample);
	}
	else if(en_PointToPoint == CalibType)
	{
		Res = CalibrateRuntime_PointToPoint(TestId , AbsSample);
	}
	else if(en_LinearRegression == CalibType)
	{
		Res = CalibrateRuntime_LinearRegression(TestId , AbsSample);
	}
	else if(en_3PL == CalibType)
	{
		Res = CalibrateRuntime_3PL(TestId , AbsSample);
	}
	else if(en_4PL == CalibType)
	{
		Res = CalibrateRuntime_4PL(TestId, AbsSample);
	}
	else if(en_CubicSpline == CalibType)
	{
		Res = CalibrateRuntime_CubicSpline(TestId , AbsSample);
	}
	else
	{
		Res = (float)(DEFAULT_ERROR_VALUE);
	}

	Res = Res * GetInstance_TestParams()[TestId].Corr_A + GetInstance_TestParams()[TestId].Corr_B;

	return Res;
}


/*Calibration function - 1Point or Factor*/
float Calibrate_Factor1Point(uint16_t TestId , float ResAbs)
{
	return GetInstance_TestParams()[TestId].KFactor * ResAbs;
}
/*Calibration function - 2Point*/

float Calibrate_2Point(uint16_t TestId , float ResAbs)
{
    // Input data arrays (max 6 standards + 1 fixed point at origin)
    double x[7] = {0.0}; // Concentration
    double y[7] = {0.0}; // Absorbance

    uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

    y[0] = (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

    if (numPoints < 1 || numPoints > 6)
        return 0.0f;

    // Always include the origin (0,0) as the first point

    for (uint8_t i = 0; i < numPoints; ++i)
    {
        x[i + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[i];
        y[i + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardOD[i];
    }

    uint8_t totalPoints = numPoints + 1;

    // Calculate averages
    double sumX = 0, sumY = 0;
    for (uint8_t i = 0; i < totalPoints; ++i)
    {
        sumX += x[i];
        sumY += y[i];
    }
    double avgX = sumX / totalPoints;
    double avgY = sumY / totalPoints;

    // Calculate slope (K) and intercept (B)
    double numerator = 0, denominator = 0;
    for (uint8_t i = 0; i < totalPoints; ++i)
    {
        numerator += (x[i] - avgX) * (y[i] - avgY);
        denominator += (x[i] - avgX) * (x[i] - avgX);
    }

    if (denominator == 0)
        return 0.0f;

    double K = numerator / denominator;
    double B = avgY - K * avgX;

    // Calculate concentration for given absorbance
    double estimatedConc = (ResAbs - B) / K;
    return (float)estimatedConc;
}

//float Calibrate_2Point(uint16_t TestId , float ResAbs)
//{
//	// Input data points
//	double x[] = {0, 0, 0, 0, 0, 0, 0};/*Conc*/
//	double y[] = {0, 0, 0, 0, 0, 0, 0};/*Abs*/
//
//	uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
//	for(uint8_t nI = 0 ; nI < numPoints ; ++nI)
//	{
//		x[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI] ;
//		y[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI] - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
//	}
//
//	// Extract data values
//	double x0 = x[0];
//	double y0 = y[0];
//	double x1 = x[1];
//	double y1 = y[1];
//
//	// Perform extrapolation
//	double desiredY = ResAbs;
//	double unknownX = x0 + (desiredY - y0) * (x1 - x0) / (y1 - y0);
//	return (float)unknownX;
//}

//float CalibrateRuntime_2Point(uint16_t TestId , float ResAbs)
//{
//	// Input data points
//	double x[] = {0, 0, 0, 0, 0, 0, 0};/*Conc*/
//	double y[] = {0, 0, 0, 0, 0, 0, 0};/*Abs*/
//
//	uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
//	for(uint8_t nI = 0 ; nI < numPoints ; ++nI)
//	{
//		x[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI] ;
//		y[nI] = (double)e_RunTimeStdOd[nI] - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
//	}
//
//	// Extract data values
//	double x0 = x[0];
//	double y0 = y[0];
//	double x1 = x[1];
//	double y1 = y[1];
//
//	// Perform extrapolation
//	double desiredY = ResAbs;
//	double unknownX = x0 + (desiredY - y0) * (x1 - x0) / (y1 - y0);
//	return (float)unknownX;
//}

float CalibrateRuntime_2Point(uint16_t TestId , float ResAbs)
{
    // Input data arrays (max 6 standards + 1 fixed point at origin)
    double x[7] = {0.0}; // Concentration
    double y[7] = {0.0}; // Absorbance

    uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

    y[0] = (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

    if (numPoints < 1 || numPoints > 6)
        return 0.0f;

    // Always include the origin (0,0) as the first point


    for (uint8_t i = 0; i < numPoints; ++i)
    {
        x[i + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[i];
        y[i + 1] =  (double)e_RunTimeStdOd[i];
    }

    uint8_t totalPoints = numPoints + 1;

    // Calculate averages
    double sumX = 0, sumY = 0;
    for (uint8_t i = 0; i < totalPoints; ++i)
    {
        sumX += x[i];
        sumY += y[i];
    }
    double avgX = sumX / totalPoints;
    double avgY = sumY / totalPoints;

    // Calculate slope (K) and intercept (B)
    double numerator = 0, denominator = 0;
    for (uint8_t i = 0; i < totalPoints; ++i)
    {
        numerator += (x[i] - avgX) * (y[i] - avgY);
        denominator += (x[i] - avgX) * (x[i] - avgX);
    }

    if (denominator == 0)
        return 0.0f;

    double K = numerator / denominator;
    double B = avgY - K * avgX;

    // Calculate concentration for given absorbance
    double estimatedConc = (ResAbs - B) / K;
    return (float)estimatedConc;
}


/*Calibration function - Linear regression - Start*/

// Function to calculate the mean of an array
double calculateMean(double* data, int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += data[i];
    }
    return sum / size;
}

// Function to calculate the R-square value
double calculateR2(double* yData, double* yPred, int size) {
    double yMean = calculateMean(yData, size);
    double ssTotal = 0;
    double ssResidual = 0;
    for (int i = 0; i < size; i++) {
        ssTotal += pow(yData[i] - yMean, 2);
        ssResidual += pow(yData[i] - yPred[i], 2);
    }
    return 1.0 - (ssResidual / ssTotal);
}

float Calibrate_LinearRegression(uint16_t TestId, float ResAbs)
{
    // Include up to 6 user points + 1 fixed origin
    double xData[7] = {0.0};  // Concentration
    double yData[7] = {0.0};  // Absorbance

    uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

    yData[0] = (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

    if (numPoints < 1 || numPoints > 6)
        return 0.0f;

    // Fill user-provided standard points (start from index 1 because index 0 is (0,0))
    for (uint8_t i = 0; i < numPoints; ++i) {
        xData[i + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[i];
        yData[i + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardOD[i];
    }

    uint8_t totalPoints = numPoints + 1; // Including origin (0,0)

    // Calculate means
    double xMean = calculateMean(xData, totalPoints);
    double yMean = calculateMean(yData, totalPoints);

    // Linear regression slope and intercept
    double numerator = 0, denominator = 0;
    for (int i = 0; i < totalPoints; i++) {
        numerator += (xData[i] - xMean) * (yData[i] - yMean);
        denominator += pow(xData[i] - xMean, 2);
    }

    if (denominator == 0)
        return 0.0f;

    double slope = numerator / denominator;
    double intercept = yMean - slope * xMean;

    // Calculate unknown concentration
    double unknownY = ResAbs;
    double unknownX = (unknownY - intercept) / slope;

    // Predict Y values for R² calculation
    double yPred[7];
    for (int i = 0; i < totalPoints; i++) {
        yPred[i] = slope * xData[i] + intercept;
    }

    // Calculate R²
    double r2 = calculateR2(yData, yPred, totalPoints);
    e_Runtime_R2value_linear = r2;

    return (float)unknownX;
}


float CalibrateRuntime_LinearRegression(uint16_t TestId , float ResAbs)
{

    // Include up to 6 user points + 1 fixed origin
    double xData[7] = {0.0};  // Concentration
    double yData[7] = {0.0};  // Absorbance

    uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
    yData[0] = (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

    if (numPoints < 1 || numPoints > 6)
        return 0.0f;

    // Fill user-provided standard points (start from index 1 because index 0 is (0,0))
    for (uint8_t i = 0; i < numPoints; ++i) {
        xData[i + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[i];
        yData[i + 1] = (double)e_RunTimeStdOd[i];
    }

    uint8_t totalPoints = numPoints + 1; // Including origin (0,0)

    // Calculate means
    double xMean = calculateMean(xData, totalPoints);
    double yMean = calculateMean(yData, totalPoints);

    // Linear regression slope and intercept
    double numerator = 0, denominator = 0;
    for (int i = 0; i < totalPoints; i++) {
        numerator += (xData[i] - xMean) * (yData[i] - yMean);
        denominator += pow(xData[i] - xMean, 2);
    }

    if (denominator == 0)
        return 0.0f;

    double slope = numerator / denominator;
    double intercept = yMean - slope * xMean;

    // Calculate unknown concentration
    double unknownY = ResAbs;
    double unknownX = (unknownY - intercept) / slope;

    // Predict Y values for R² calculation
    double yPred[7];
    for (int i = 0; i < totalPoints; i++) {
        yPred[i] = slope * xData[i] + intercept;
    }

    // Calculate R²
    double r2 = calculateR2(yData, yPred, totalPoints);
    e_Runtime_R2value_linear = r2;

    return (float)unknownX;

}

//float Calibrate_LinearRegression(uint16_t TestId , float ResAbs)
//{
//	// Input data points
//	double xData[] = {0, 0, 0, 0, 0, 0, 0};/*Conc*/
//	double yData[] = {0, 0, 0, 0, 0, 0, 0};/*Abs*/
//
//	uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
//	for(uint8_t nI = 0 ; nI < numPoints ; ++nI)
//	{
//		xData[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI];
//		yData[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI] - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
//	}
//
//    // Calculate the parameters of the linear regression
//    double xMean = calculateMean(xData, numPoints);
//    double yMean = calculateMean(yData, numPoints);
//    double numerator = 0;
//    double denominator = 0;
//    for (int i = 0; i < numPoints; i++) {
//        numerator += (xData[i] - xMean) * (yData[i] - yMean);
//        denominator += pow(xData[i] - xMean, 2);
//    }
//    double slope = numerator / denominator;
//    double intercept = yMean - slope * xMean;
//
//    // Calculate the unknown x for y = 1.2
//    double unknownY = ResAbs;
//    double unknownX = (unknownY - intercept) / slope;
//
//    // Generate points for the regression line
//    double yPred[numPoints];
//    for (int i = 0; i < numPoints; i++) {
//        yPred[i] = slope * xData[i] + intercept;
//    }
//
//    // Calculate R-square value
//    double r2 = calculateR2(yData, yPred, numPoints);
//    e_Runtime_R2value_linear = r2;
//	return (float)unknownX;
//}

//float CalibrateRuntime_LinearRegression(uint16_t TestId , float ResAbs)
//{
//	// Input data points
//	double xData[] = {0, 0, 0, 0, 0, 0, 0};/*Conc*/
//	double yData[] = {0, 0, 0, 0, 0, 0, 0};/*Abs*/
//
//	uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
//	for(uint8_t nI = 0 ; nI < numPoints ; ++nI)
//	{
//		xData[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI];
//		yData[nI] = (double)e_RunTimeStdOd[nI] - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
//	}
//
//    // Calculate the parameters of the linear regression
//    double xMean = calculateMean(xData, numPoints);
//    double yMean = calculateMean(yData, numPoints);
//    double numerator = 0;
//    double denominator = 0;
//    for (int i = 0; i < numPoints; i++) {
//        numerator += (xData[i] - xMean) * (yData[i] - yMean);
//        denominator += pow(xData[i] - xMean, 2);
//    }
//    double slope = numerator / denominator;
//    double intercept = yMean - slope * xMean;
//
//    // Calculate the unknown x for y = 1.2
//    double unknownY = ResAbs;
//    double unknownX = (unknownY - intercept) / slope;
//
//    // Generate points for the regression line
//    double yPred[numPoints];
//    for (int i = 0; i < numPoints; i++) {
//        yPred[i] = slope * xData[i] + intercept;
//    }
//
//    // Calculate R-square value
//    double r2 = calculateR2(yData, yPred, numPoints);
//    e_Runtime_R2value_linear = r2;
//	return (float)unknownX;
//}


/*Calibration function - Linear regression - End*/

/*Calibration function - Point to point - Start*/
float Calibrate_PointToPoint(uint16_t TestId , float ResAbs)
{
	// Input data points
	double x[] = {0, 0, 0, 0, 0, 0, 0};/*Conc*/
	double y[] = {0, 0, 0, 0, 0, 0, 0};/*Abs*/

	uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
    y[0] = (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

	for(uint8_t nI = 0 ; nI < numPoints ; ++nI)
	{
		x[nI + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI];
		y[nI + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI];
	}

    uint8_t totalPoints = numPoints + 1; // Including origin (0,0)

	// Finding the unknown x at y = 1.4
	double unknownY = ResAbs;
	double unknownX = NAN;

	// Check if the target y-value is within the known range
	if (y[0] <= unknownY && unknownY <= y[totalPoints - 1]) {
		for (int i = 1; i < totalPoints; i++) {
			if (y[i - 1] <= unknownY && unknownY <= y[i]) {
				double slope = (y[i] - y[i - 1]) / (x[i] - x[i - 1]);
				unknownX = x[i - 1] + (unknownY - y[i - 1]) / slope;
				break;
			}
		}
	} else if (unknownY > y[totalPoints - 1]) {
		// Extrapolation using the last two points
		double slope = (y[totalPoints - 1] - y[totalPoints - 2]) / (x[totalPoints - 1] - x[totalPoints - 2]);
		unknownX = x[totalPoints - 1] + (unknownY - y[totalPoints - 1]) / slope;
	}
	return (float)unknownX;
}


float CalibrateRuntime_PointToPoint(uint16_t TestId , float ResAbs)
{
	// Input data points
	double x[] = {0, 0, 0, 0, 0, 0, 0};/*Conc*/
	double y[] = {0, 0, 0, 0, 0, 0, 0};/*Abs*/

	uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

    y[0] = (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

	for(uint8_t nI = 0 ; nI < numPoints ; ++nI)
	{
		x[nI + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI];
		y[nI + 1] = (double)e_RunTimeStdOd[nI];
	}

    uint8_t totalPoints = numPoints + 1; // Including origin (0,0)
	// Finding the unknown x at y = 1.4
	double unknownY = ResAbs;
	double unknownX = NAN;

	// Check if the target y-value is within the known range
	if (y[0] <= unknownY && unknownY <= y[totalPoints - 1]) {
		for (int i = 1; i < totalPoints; i++) {
			if (y[i - 1] <= unknownY && unknownY <= y[i]) {
				double slope = (y[i] - y[i - 1]) / (x[i] - x[i - 1]);
				unknownX = x[i - 1] + (unknownY - y[i - 1]) / slope;
				break;
			}
		}
	} else if (unknownY > y[totalPoints - 1]) {
		// Extrapolation using the last two points
		double slope = (y[totalPoints - 1] - y[totalPoints - 2]) / (x[totalPoints - 1] - x[totalPoints - 2]);
		unknownX = x[totalPoints - 1] + (unknownY - y[totalPoints - 1]) / slope;
	}
	return (float)unknownX;
}


/*Calibration function - Point to point - End*/

/*Calibration function - Cubic spline -Start*/
float CalibrateRuntime_CubicSpline(uint16_t TestId , float ResAbs)
{
    double x[7] = {0}, y[7] = {0}; // Conc and Abs (max 7 points)

	uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

    y[0] = (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

	for(uint8_t nI = 0 ; nI < numPoints ; ++nI)
	{
		x[nI + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI];
		y[nI + 1] = (double)e_RunTimeStdOd[nI];
	}

    uint8_t totalPoints = numPoints + 1; // Including origin (0,0)

    CubicSpline cubicSpline[totalPoints - 1]; //

    computeCubicSpline(x, y, totalPoints, cubicSpline);

    double desired_y = (double)ResAbs;

    double unknown_concentration = evaluateCubicSpline_Inverse(desired_y, cubicSpline, totalPoints, x);

    e_Runtime_R2value_cubic = calculateR2CubicSpline(x, y, totalPoints, cubicSpline);

    return (float)unknown_concentration;
}

float Calibrate_CubicSpline(uint16_t TestId, float ResAbs)
{
    double x[7] = {0}, y[7] = {0}; // Conc and Abs (max 7 points)
    uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

    y[0] = (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

    for (uint8_t i = 0; i < numPoints; ++i)
    {
        x[i + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[i];
        y[i + 1] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardOD[i];
    }

    uint8_t totalPoints = numPoints + 1; // Including origin (0,0)

    CubicSpline cubicSpline[totalPoints - 1]; //

    computeCubicSpline(x, y, totalPoints, cubicSpline);

    double desired_y = (double)ResAbs;

    double unknown_concentration = evaluateCubicSpline_Inverse(desired_y, cubicSpline, totalPoints, x);

    e_Runtime_R2value_cubic = calculateR2CubicSpline(x, y, totalPoints, cubicSpline);

    return (float)unknown_concentration;
}

// Compute spline coefficients
void computeCubicSpline(double x[], double y[], int n, CubicSpline spline[])
{
    double h[n - 1], alpha[n], l[n], mu[n], z[n];
    double c[n], b[n - 1], d[n - 1];

    for (int i = 0; i < n - 1; i++)
        h[i] = x[i + 1] - x[i];

    for (int i = 1; i < n - 1; i++)
        alpha[i] = (3.0 / h[i]) * (y[i + 1] - y[i]) - (3.0 / h[i - 1]) * (y[i] - y[i - 1]);

    l[0] = 1.0;
    mu[0] = 0.0;
    z[0] = 0.0;

    for (int i = 1; i < n - 1; i++) {
        l[i] = 2.0 * (x[i + 1] - x[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }

    l[n - 1] = 1.0;
    z[n - 1] = 0.0;
    c[n - 1] = 0.0;

    for (int j = n - 2; j >= 0; j--) {
        c[j] = z[j] - mu[j] * c[j + 1];
        b[j] = (y[j + 1] - y[j]) / h[j] - h[j] * (c[j + 1] + 2.0 * c[j]) / 3.0;
        d[j] = (c[j + 1] - c[j]) / (3.0 * h[j]);
        spline[j] = (CubicSpline){ y[j], b[j], c[j], d[j] };
    }
}

// Evaluate spline at given x
double evaluateCubicSpline(double xVal, CubicSpline spline[], int n, double x[])
{
    for (int i = 0; i < n - 1; i++) {
        if (xVal >= x[i] && xVal <= x[i + 1]) {
            double dx = xVal - x[i];
            return spline[i].a + spline[i].b * dx + spline[i].c * dx * dx + spline[i].d * dx * dx * dx;
        }
    }
    return 0.0; // Out of bounds
}

// Evaluate spline at known y to find x (inverse interpolation)
double evaluateCubicSpline_Inverse(double yVal, CubicSpline spline[], int n, double x[])
{
    // Bisection method on each interval to find x such that spline(x) = yVal
    for (int i = 0; i < n - 1; i++) {
        double x_low = x[i];
        double x_high = x[i + 1];
        double mid, fx;

        // Clamp to valid range
        for (int iter = 0; iter < 30; iter++) {
            mid = (x_low + x_high) / 2.0;
            double dx = mid - x[i];

            fx = spline[i].a + spline[i].b * dx + spline[i].c * dx * dx + spline[i].d * dx * dx * dx - yVal;

            if (fabs(fx) < 1e-6)
                return mid; // Found valid x

            if (fx > 0)
                x_high = mid;
            else
                x_low = mid;
        }
    }

    return -1.0; // Not found
}


// Calculate R²
double calculateR2CubicSpline(double x[], double y[], int n, CubicSpline spline[])
{
    double interpolatedY[n];
    for (int i = 0; i < n; i++) {
        interpolatedY[i] = evaluateCubicSpline(x[i], spline, n, x);
    }
    return calculateCorrelationCoefficient_cubicSpline(y, interpolatedY, n);
}

// Correlation coefficient
double calculateCorrelationCoefficient_cubicSpline(double actual[], double predicted[], int n)
{
    double sum_actual = 0, sum_predicted = 0;
    double sum_actual_sq = 0, sum_predicted_sq = 0;
    double sum_product = 0;

    for (int i = 0; i < n; i++) {
        sum_actual += actual[i];
        sum_predicted += predicted[i];
        sum_actual_sq += actual[i] * actual[i];
        sum_predicted_sq += predicted[i] * predicted[i];
        sum_product += actual[i] * predicted[i];
    }

    double numerator = n * sum_product - sum_actual * sum_predicted;
    double denominator = sqrt((n * sum_actual_sq - sum_actual * sum_actual) *
                              (n * sum_predicted_sq - sum_predicted * sum_predicted));

    if (denominator == 0)
        return 0.0;

    double corr = numerator / denominator;
    return corr * corr; // R²
}


float Calibrate_3PL(uint16_t TestId, float targetY)
{
    if ( GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved != 2) {
        return -1.0f; // Error: Method supports only 3 standards
    }

    // Load standard values
    float x[3] =
    {
        0.0f,
        GetInstance_TestParams()[stcTestData.TestId].StandardConc[0],
        GetInstance_TestParams()[stcTestData.TestId].StandardConc[1]
    };

    float y[3] =
    {
    	GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs,
        GetInstance_TestParams()[stcTestData.TestId].StandardOD[0] ,
        GetInstance_TestParams()[stcTestData.TestId].StandardOD[1]
    };

    // Calculate 3PL parameters
    float numerator = (y[2] - y[0]) * x[1] - (y[1] - y[0]) * x[2];
    float denominator = (y[1] - y[2]) * x[1] * x[2];
    float a = numerator / denominator;

    if (fabsf(a) < 1e-6f) return -1.0; // avoid division by zero

    float K = (y[1] - y[0]) * (1.0f + a * x[1]) / (-a * x[1]);
    float B = y[0] - K;

    // Calculate concentration for given targetY
    float diff = targetY - B;
    if (fabsf(diff) < 1e-6f) return -1.0; // avoid division by zero

    float concentration = (1.0f / a) * (K / diff - 1.0f);

    // --- R² Calculation ---
    float y_mean = (y[0] + y[1] + y[2]) / 3.0f;
    float SSres = 0.0f;
    float SStot = 0.0f;

    for (int i = 0; i < 3; i++) {
        float y_pred = B + K / (1.0f + a * x[i]);
        float err = y[i] - y_pred;
        SSres += err * err;

        float dev = y[i] - y_mean;
        SStot += dev * dev;
    }

    if (fabsf(SStot) > 1e-6f) {
    	e_Runtime_R2value_3pl = 1.0f - (SSres / SStot);
    }

    return concentration;

}

float CalibrateRuntime_3PL(uint16_t TestId, float targetY)
{
    // Load standard values
    if ( GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved != 2) {
        return -1.0f; // Error: Method supports only 3 standards
    }

    float x[3] =
    {
        0.0f,
        GetInstance_TestParams()[stcTestData.TestId].StandardConc[0],
        GetInstance_TestParams()[stcTestData.TestId].StandardConc[1]
    };

    float y[3] =
    {
    	GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs,
		e_RunTimeStdOd[0],
		e_RunTimeStdOd[1]
    };

    // Calculate 3PL parameters
    float numerator = (y[2] - y[0]) * x[1] - (y[1] - y[0]) * x[2];
    float denominator = (y[1] - y[2]) * x[1] * x[2];
    float a = numerator / denominator;

    if (fabsf(a) < 1e-6f) return -1.0; // avoid division by zero

    float K = (y[1] - y[0]) * (1.0f + a * x[1]) / (-a * x[1]);
    float B = y[0] - K;

    // Calculate concentration for given targetY
    float diff = targetY - B;
    if (fabsf(diff) < 1e-6f) return -1.0; // avoid division by zero

    float concentration = (1.0f / a) * (K / diff - 1.0f);

    // --- R² Calculation ---
    float y_mean = (y[0] + y[1] + y[2]) / 3.0f;
    float SSres = 0.0f;
    float SStot = 0.0f;

    for (int i = 0; i < 3; i++) {
        float y_pred = B + K / (1.0f + a * x[i]);
        float err = y[i] - y_pred;
        SSres += err * err;

        float dev = y[i] - y_mean;
        SStot += dev * dev;
    }

    if (fabsf(SStot) > 1e-6f) {
    	e_Runtime_R2value_3pl = 1.0f - (SSres / SStot);
    }

    return concentration;

}
//float Calibrate_CubicSpline(uint16_t TestId , float ResAbs)
//{
//	// Input data points
//	double x[] = {0, 0, 0, 0, 0, 0, 0};/*Conc*/
//	double y[] = {0, 0, 0, 0, 0, 0, 0};/*Abs*/
//
//	uint8_t numPoints = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
//	for(uint8_t nI = 0 ; nI < numPoints ; ++nI)
//	{
//		x[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI];
//		y[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI] - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
//	}
//
//	 CubicSpline cubicSpline[numPoints - 1]; // Array to store cubic spline coefficients
//
//	    // Compute cubic spline coefficients
//	    computeCubicSpline(y, x, numPoints, cubicSpline);
//
//	    // Interpolate for a new x value
//	    double desired_y = (double)ResAbs;
////	    double new_x = 2.5;
//	    double unknown_concentration = evaluateCubicSpline(y, cubicSpline, numPoints, desired_y);
//	    e_Runtime_R2value_cubic = calculateR2CubicSpline(x, y, numPoints, cubicSpline);
//
////    e_Runtime_R2value_cubic = calculateR2CubicSpline(x, y, numPoints, cubicSpline);
//    // Evaluate the spline curve at a desired y-value to find unknown X
//
////    double unknown_concentration = cubicSplineInterpolation(y, x, numPoints, desired_y);
//	return (float)unknown_concentration;
//}



//
//void computeCubicSpline(double x[], double y[], int n, CubicSpline cubicSpline[]) {
//    double h[n], alpha[n], l[n], mu[n], z[n];
//
//    for (int i = 0; i < n - 1; i++) {
//        h[i] = x[i + 1] - x[i];
//        alpha[i] = (3.0 / h[i]) * (y[i + 1] - y[i]) - (3.0 / h[i - 1]) * (y[i] - y[i - 1]);
//    }
//
//    l[0] = 1.0;
//    mu[0] = 0.0;
//    z[0] = 0.0;
//
//    for (int i = 1; i < n - 1; i++) {
//        l[i] = 2.0 * (x[i + 1] - x[i - 1]) - h[i - 1] * mu[i - 1];
//        mu[i] = h[i] / l[i];
//        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
//    }
//
//    l[n - 1] = 1.0;
//    z[n - 1] = 0.0;
//    double c[n], b[n], d[n];
//
//    for (int j = n - 2; j >= 0; j--) {
//        c[j] = z[j] - mu[j] * c[j + 1];
//        b[j] = (y[j + 1] - y[j]) / h[j] - h[j] * (c[j + 1] + 2.0 * c[j]) / 3.0;
//        d[j] = (c[j + 1] - c[j]) / (3.0 * h[j]);
//        cubicSpline[j] = {y[j], b[j], c[j], d[j]};
//    }
//}

//// Function to evaluate the interpolated value using the computed spline
//double evaluateCubicSpline(double x[], CubicSpline cubicSpline[], int n, double xValue) {
//    for (int i = 0; i < n - 1; i++) {
//        if (xValue >= x[i] && xValue <= x[i + 1]) {
//            double dx = xValue - x[i];
//            double interpolatedValue = cubicSpline[i].a + cubicSpline[i].b * dx +
//                                       cubicSpline[i].c * dx * dx + cubicSpline[i].d * dx * dx * dx;
//            return interpolatedValue;
//        }
//    }
//    return 0.0; // If the value is outside the range, return 0.0 or some other appropriate value
//}

//// Function to calculate R^2 for cubic spline interpolation
//double calculateR2CubicSpline(double x[], double y[], int n, CubicSpline cubicSpline[]) {
//    double interpolatedY[n];
//
//    // Interpolate values using the cubic spline
//    for (int i = 0; i < n; i++) {
//    	interpolatedY[i] = evaluateCubicSpline_r2(x[i], cubicSpline, n, x);
//
//    }
//    // Calculate correlation coefficient
//    double correlationCoefficient = calculateCorrelationCoefficient(y, interpolatedY, n);
//    // Calculate R^2
//    return correlationCoefficient * correlationCoefficient;
//}
//
//double evaluateCubicSpline_r2(double x, CubicSpline cubicSpline[], int n, double dataPointsX[]) {
//    // Find the interval in which x lies
//    int interval = 0;
//    while (interval < n && x > dataPointsX[interval + 1]) {
//        interval++;
//    }
//
//    // Evaluate the cubic spline equation for the interval
//    double deltaX = x - dataPointsX[interval];
//    double result = cubicSpline[interval].a + cubicSpline[interval].b * deltaX +
//                    cubicSpline[interval].c * deltaX * deltaX +
//                    cubicSpline[interval].d * deltaX * deltaX * deltaX;
//
//    return result;
//}


/*Calibration function - Cubic spline -End*/


/*Calibration function - 4PL - Start*/

// Define the 4-parameter logistic (4PL) curve function


float CalibrateRuntime_4PL(uint16_t TestId, float targetY)
{

    double bestDiff = 1e9;
    double bestX = -1.0;

	uint8_t NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
	float MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc, NumOfStdSaved);

	for (double x = 0.0; x <= MaxXVal; x += 0.02)
	{
		 double xOverC = x / e_Runtime4plC;
		 double xOverCpowB = pow(xOverC, e_Runtime4plB);
		 double calculatedY = e_Runtime4plA + (e_Runtime4plD - e_Runtime4plA) / (1.0 + xOverCpowB);

		 double diff = fabs(calculatedY - targetY);

		if (diff < bestDiff)
		{
			bestDiff = diff;
			bestX = x;
		}
		if (diff < 0.01)  // epsilon match for early break
			break;
	 }
	    return (float)bestX;
}

float CalibrateRuntime_4PL_initial_params(uint16_t TestId)
{
	// Input data points
	double X[] = {0, 0, 0, 0, 0, 0, 0};/*Conc*/
	double Y[] = {0, 0, 0, 0, 0, 0, 0};/*Abs*/

	uint8_t dataSize = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;

//    Y[0] = (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

	for(uint8_t nI = 0 ; nI < dataSize ; ++nI)
	{
		X[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI];
		Y[nI] = (double)e_RunTimeStdOd[nI] - (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
	}

//	uint8_t totalPoints = dataSize + 1; // Including origin (0,0)
	uint8_t totalPoints = dataSize;

	e_Runtime4plA = 0.5 * (Y[0] + Y[totalPoints - 1]);
	e_Runtime4plD = 1.5 * (Y[0] + Y[totalPoints - 1]);
	e_Runtime4plC = 0.5 * (X[0] + X[totalPoints - 1]);
	e_Runtime4plB = 1.0;

	double a = e_Runtime4plA;
	double b = e_Runtime4plB;
	double c = e_Runtime4plC;
	double d = e_Runtime4plD;

	 refinedGridSearch(X, Y, totalPoints, a, b, c, d);

	 e_Runtime4plA = a;
	 e_Runtime4plB = b;
	 e_Runtime4plC = c;
	 e_Runtime4plD = d;

}


float Calibrate_4PL(uint16_t TestId, float targetY)
{
    double bestDiff = 1e9;
    double bestX = -1.0;
    uint8_t NumOfStdSaved = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
    float MaxXVal = FindMaxValueFromList(GetInstance_TestParams()[stcTestData.TestId].StandardConc, NumOfStdSaved);

    for (double x = 0.0; x <= MaxXVal; x += 0.02)
    {
        double xOverC = x / GetInstance_TestParams()[stcTestData.TestId].FourplC;
        double xOverCpowB = pow(xOverC, GetInstance_TestParams()[stcTestData.TestId].FourplB);
        double calculatedY = GetInstance_TestParams()[stcTestData.TestId].FourplA +
                             (GetInstance_TestParams()[stcTestData.TestId].FourplD -
                              GetInstance_TestParams()[stcTestData.TestId].FourplA) / (1.0 + xOverCpowB);

        double diff = fabs(calculatedY - targetY);
        if (diff < bestDiff)
        {
            bestDiff = diff;
            bestX = x;
        }

        if (diff < 0.01)  // epsilon match for early break
            break;
    }

    return (float)bestX;
}



float Calibrate_4PL_initial_params(uint16_t TestId)
{
	// Input data points
	double X[] = {0, 0, 0, 0, 0, 0, 0};/*Conc*/
	double Y[] = {0, 0, 0, 0, 0, 0, 0};/*Abs*/

	uint8_t dataSize = GetInstance_TestParams()[stcTestData.TestId].NumOfStandardsSaved;
//    Y[0] = (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

	for(uint8_t nI = 0 ; nI < dataSize ; ++nI)
	{
		X[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardConc[nI];
		Y[nI] = (double)GetInstance_TestParams()[stcTestData.TestId].StandardOD[nI] - (double)GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
	}

//	uint8_t totalPoints = dataSize + 1; // Including origin (0,0)
	uint8_t totalPoints = dataSize;

	GetInstance_TestParams()[stcTestData.TestId].FourplA = 0.5 * (Y[0] + Y[totalPoints - 1]);
	GetInstance_TestParams()[stcTestData.TestId].FourplD = 1.5 * (Y[0] + Y[totalPoints - 1]);
	GetInstance_TestParams()[stcTestData.TestId].FourplC = 0.5 * (X[0] + X[totalPoints - 1]);
	GetInstance_TestParams()[stcTestData.TestId].FourplB = 1.0;

	double a = GetInstance_TestParams()[stcTestData.TestId].FourplA;
	double b = GetInstance_TestParams()[stcTestData.TestId].FourplB;
	double c = GetInstance_TestParams()[stcTestData.TestId].FourplC;
	double d = GetInstance_TestParams()[stcTestData.TestId].FourplD;

	 refinedGridSearch(X, Y, totalPoints, a, b, c, d);

	 GetInstance_TestParams()[stcTestData.TestId].FourplA = a;
	 GetInstance_TestParams()[stcTestData.TestId].FourplB = b;
	 GetInstance_TestParams()[stcTestData.TestId].FourplC = c;
	 GetInstance_TestParams()[stcTestData.TestId].FourplD = d;

}

void refinedGridSearch(const double X[], const double Y[], int dataSize, double& A, double& B, double& C, double& D) {
    const double initialStep = 0.44;
    const double refinedStep = 0.22;

    double bestRSquared = -1.0;
    double sumSquaredError, rsquared;

    for (double a = Y[0]; a <= Y[dataSize - 1]; a += initialStep) {
        for (double d = Y[dataSize - 1]; d >= Y[0]; d -= initialStep) {
            for (double c = X[0]; c <= X[dataSize - 1]; c += initialStep) {
                for (double b = 0.1; b <= 3.0; b += 0.1) {
                    sumSquaredError = 0.0;

                    for (int i = 0; i < dataSize; ++i) {
                        double xOverC = X[i] / c;
                        double xOverCpowB = pow(xOverC, b);
                        double yPredicted = a + (d - a) / (1.0 + xOverCpowB);
                        double error = Y[i] - yPredicted;
                        sumSquaredError += error * error;
                    }

                    rsquared = 1.0 - (sumSquaredError / dataSize);

                    if (rsquared > bestRSquared) {
                        bestRSquared = rsquared;
                        A = a;
                        D = d;
                        C = c;
                        B = b;
                    }
                }
            }
        }
    }

    for (double a = A - initialStep; a <= A + initialStep; a += refinedStep) {
        for (double d = D - initialStep; d <= D + initialStep; d += refinedStep) {
            for (double c = max(X[0], C - initialStep); c <= min(X[dataSize - 1], C + initialStep); c += refinedStep) {
                for (double b = B - initialStep; b <= B + initialStep; b += refinedStep) {
                    sumSquaredError = 0.0;

                    for (int i = 0; i < dataSize; ++i) {
                        double xOverC = X[i] / c;
                        double xOverCpowB = pow(xOverC, b);
                        double yPredicted = a + (d - a) / (1.0 + xOverCpowB);
                        double error = Y[i] - yPredicted;
                        sumSquaredError += error * error;
                    }

                    rsquared = 1.0 - (sumSquaredError / dataSize);

                    if (rsquared > bestRSquared) {
                        bestRSquared = rsquared;
                        A = a;
                        D = d;
                        C = c;
                        B = b;
                    }
                }
            }
        }
    }

    e_Runtime_R2value_4pl = bestRSquared;
}

/*Calibration function - 4PL - End*/


/*Factor calculation - START*/
float CalculateFactor(void)
{
	float Kfactor = 0;
	float ConcOfStd1 = GetInstance_TestParams()[stcTestData.TestId].StandardConc[0];
	float AbsOfStd1 = e_RunTimeStdOd[0];//GetInstance_TestParams()[stcTestData.TestId].StandardOD[0];
	float AbsOfRegBlank = GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

	enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;
	if(en_1PointLinear == CalibType)
	{
		/*Both R blank and Sample blank enabled*/
		if(	true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
		{
			Kfactor = ConcOfStd1 / ((AbsOfStd1) -  AbsOfRegBlank);
		}
		/*Both sample blank and reagent blank is disabled*/
		else
		{
			Kfactor = ConcOfStd1 / ((AbsOfStd1));
		}
	}
	return Kfactor;
}

float CalculateFactor_Graph(void)
{
	float Kfactor = 0;
	float ConcOfStd1 = GetInstance_TestParams()[stcTestData.TestId].StandardConc[0];
	float AbsOfStd1 = GetInstance_TestParams()[stcTestData.TestId].StandardOD[0];
	float AbsOfRegBlank = GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;

	enCalibrationType CalibType = GetInstance_TestParams()[stcTestData.TestId].Calib;
	if(en_1PointLinear == CalibType)
	{
		/*Both R blank and Sample blank enabled*/
		if(	true == GetInstance_TestParams()[stcTestData.TestId].ReagentBlankEnable)
		{
			Kfactor = ConcOfStd1 / ((AbsOfStd1) -  AbsOfRegBlank);
		}
		/*Both sample blank and reagent blank is disabled*/
		else
		{
			Kfactor = ConcOfStd1 / ((AbsOfStd1));
		}
	}
	return Kfactor;
}

/*Factor calculation - END*/
