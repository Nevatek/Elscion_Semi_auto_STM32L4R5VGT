/*
 * Testrun.h
 *
 *  Created on: 09-Sep-2022
 *      Author: SPX
 */
#ifndef APPL_INC_TESTRUN_H_
#define APPL_INC_TESTRUN_H_
//#include "../Screens/Inc/CommonDisplayFunctions.h"
#include "FilterMotor.h"
#include "Preamp.h"
#include <string>
#include "RTC.h"
#include "../../APPL/Inc/NonVolatileMemory.h"
#include <sstream>
#include <iomanip>

#define TEST_GRAPH_PIXEL_MIN_X (65)
#define TEST_GRAPH_PIXEL_MIN_Y (112)
#define TEST_GRAPH_PIXEL_X_WIDTH (400)
#define TEST_GRAPH_PIXEL_Y_HEIGHT (250)
#define TEST_GRAPH_PIXEL_MAX_X (TEST_GRAPH_PIXEL_MIN_X + TEST_GRAPH_PIXEL_X_WIDTH)
#define TEST_GRAPH_PIXEL_MAX_Y (TEST_GRAPH_PIXEL_MIN_Y + TEST_GRAPH_PIXEL_Y_HEIGHT)

#define RB_OUT_RANGE			"R.blank absorbance out of range"
#define ABSORBANCE_OUT_RANGE	"Absorbance out of measuring range"
#define LINEARITY_OUT_RANGE		"Concentration Out of Linearity range"
#define DELTA_A_OUT_RANGE		"dAbs/min Absorbance Out of range"

/*Normal Test running commands*/

#define INITIALIZE					"Initializing, Please wait"
#define INITIALIZE_ERR				"Initialization Error"

#define ASPIRATE_DI_WATER			"Aspirate Water blank"
#define ASPIRATING_DI_WATER			"Aspirating Water blank"

#define ASPIRATE_REAGENT_BLANK		"Aspirate Reagent Blk"
#define ASPIRATING_REAGENT_BLANK	"Aspirating Reagent Blk"

#define ASPIRATE_REAGENT			"Aspirate Reagent"
#define ASPIRATING_REAGENT			"Aspirating Reagent"

#define ASPIRATE_STD_BLANK			"Aspirate Std. Blank"
#define ASPIRATING_STD_BLANK 		"Aspirating Std. Blank"

#define ASPIRATE_STD				"Aspirate Standard"
#define ASPIRATING_STD				"Aspirating Standard"

#define ASPIRATE_QC_BLANK			"Aspirate QC Blank"
#define ASPIRATING_QC_BLANK 		"Aspirating QC Blank"

#define ASPIRATE_QC					"Aspirate QC"
#define ASPIRATING_QC				"Aspirating QC"

#define ASPIRATE_SAMPLE_BLANK		"Aspirate Sample Blk"
#define ASPIRATING_SAMPLE_BLANK 	"Aspirating Sample Blk"

#define ASPIRATE_SAMPLE				"Aspirate Sample"
#define ASPIRATING_SAMPLE			"Aspirating Sample"

#define ASPIRATIO_COMPLETE			"Aspiration Completed"
#define INCUBATE_STARTED			"Incubation Started"
#define MEASURE_STARTED				"Measuring Started"
#define ABORT						"Aborted, Washing"
#define TEST_COMPLETE				"Test Completed"
#define AUTO_EMPTY					"Auto Emptying"
#define WASH_MSG					"Washing, Please wait"

/*Coagulation Test running commands*/
#define COAG_DI_WATER			"Ins.Cuvette, Add DI Water"
#define COAG_QC					"Ins.Cuvette, Add QC"
#define COAG_SAMPLE				"Ins.Cuvette, Add Sample"
#define COAG_ADD_REAGENT_MEASU	"Add Reagent and Measure"
#define COAG_ABORT		"Test Aborted"

#define INCUBATION_TIME_FOR_WATER_BLANK_SEC (5)
#define MEASUREMENT_TIME_FOR_WATER_BLANK_SEC (5)
#define READ_TIME_FOR_SECONDARY_ENDPOINT_SEC (5)
#define MAX_ADC_BUFFER_LENGTH_DEFAULT (600)/*Primary wil have incubation data too*/
#define MAX_ADC_BUFFER_LENGTH_PRIMARY (2 * MAX_ADC_BUFFER_LENGTH_DEFAULT)
#define MAX_ADC_READING_ENDPOINT (5)
#define MAX_PATIENT_ID_LENGTH (15)
#define STANDARD_PRECISION (4)
#define RESULT_PRECISION (2)

#define CURVE_DELAY_TIME (5)

#define MIN_LAG_TIME (2U)
#define MAX_LAG_TIME (600U)
#define MIN_READ_TIME (2U)
#define MAX_READ_TIME (600U)

#define MIN_ASP_VOL (200)/*uL*/
#define MAX_ASP_VOL (2000)/*uL*/
#define DEFAULT_ASP_VOL (400)/*uL*/

#define MAX_NUM_OF_TEST_PARAMS_SAVED (200)
#define MAX_NUM_OF_STANDARDS (6)
#define MAX_NUM_OF_STANDARD_CONC (MAX_NUM_OF_STANDARDS)
#define MAX_NUM_OF_STANDARD_OD (MAX_NUM_OF_STANDARDS)

#define MAX_TEST_NAME_CHAR_LENGTH (6)
#define MAX_FULL_TEST_NAME_CHAR_LENGTH (12)
#define VALID_TEST_PARAM_FLAG (0x01)
#define INVALID_TEST_PARAM_FLAG (0xFF)
#define MAX_NUM_OF_CAL_TRACE (4)
#define RBLK_WATER_OD (6)

typedef enum
{
	enTestStatus_Idle = 0,
	enTestStatus_ManualWash_Busy,
	enTestStatus_MoveFilterHome,
	enTestStatus_MoveFilterHome_Busy,
	enTestStatus_Initialize,
	enTestStatus_SelectDarkAdcFilter,
	enTestStatus_SelectDarkAdcFilter_Busy,
	enTestStatus_ReadDarkAdcPrimary,
	enTestStatus_ReadDarkAdcSecondary,
	enTestStatus_SwitchPrimaryWavelength,
	enTestStatus_SwitchPrimaryWavelength_Busy,
	enTestStatus_Aspiration,
	enTestStatus_Aspiration_Busy,
	enTestStatus_Aspiration_Post_Delay,
	enTestStatus_AspirationAirGap_Pre_Delay,
	enTestStatus_AspirationAirGap,
	enTestStatus_AspirationAirGap_Busy,
	enTestStatus_SwitchSecondaryWavelength,
	enTestStatus_SwitchSecondaryWavelength_Busy,
	enTestStatus_Incubation,
	enTestStatus_PrimaryMeasurement,
	enTestStatus_SecondaryMeasurement,
	enTestStatus_WaitingForNextStep,
	enTestStatus_Wash,
	enTestStatus_Wash_Busy,
	enTestStatus_Error,
	enTestStatus_InitError,
	enTestStatus_DarkADCError,
	enTestStatus_Warning,
	enTestStatus_TestComplete,
	enTestStatus_saveData,
	enTestStatus_saveDataDummy,
	enTestStatus_TestIdle,
	enTestStatus_Max
}en_TestRunStatus;

typedef enum
{
	en_MeasurementNull = 0,
	en_DIWaterAdcBlank,
	en_ReagentBlank,
	en_Reagent,
	en_StandardBlank,
	en_Standard,
	en_SampleBlank,
	en_Sample,
	en_TestQcBlank,
	en_TestQc,
	en_WashProcedure,
	en_CoagulationsampleIncubation,
	en_CoagulationQCIncubation,
	enMeasurementTypeMax
}enMeasurementType;

typedef enum
{
	en_Flowcell = 0,
	en_Cuvette,
	en_Coagulationtube,
	en_MaxCuvetteType
}enCuvetteType;

typedef enum
{
	en_Factor = 0,
	en_1PointLinear,
	en_2PointLinear,
	en_PointToPoint,
	en_LinearRegression,/*Linear regression*/
	en_CubicSpline,
	en_3PL,
	en_4PL,
	en_CalibrationMax
}enCalibrationType;

typedef enum
{
	en_Endpoint = 0,
	en_Kinetics,
	en_FixedTime,
	en_Turbidimetry,
	en_Absorbance,
	en_CoagulationTest,
	en_TestProdecureMax
}enTestProcedureType;

typedef enum
{
	en_mgdL = 0,
	en_UL,
	en_gdL,
	en_mmoll,
	en_percent,
	en_umoldL,
	en_gL,
	en_mgL,
	en_ugL,
	en_UmL,
	en_ugmL,
	en_ngmL,
	en_ABS,
	en_Custom1,
	en_Custom2,
	en_Custom3,
	en_Custom4,
	en_Custom5,
	en_Custom6,
	en_Custom7,
	en_Custom8,
	en_MaxUnits
}enUnits;


typedef enum
{
	en_RangeLow = 0,
	en_RangeHigh,
	en_RangeMax
}enRange;

typedef enum
{
	en_WashProcess_Idle = 0,
	en_WashProcess_1,
	en_WashProcess_1_Busy,
	en_WashProcess_2,
	en_WashProcess_2_Busy,
	en_WashProcess_3,
	en_WashProcess_3_Busy,
	en_WashProcess_AspPreDelay,
	en_WashProcess_AspAirGap,
	en_WashProcess_AspAirGapdelay,
	en_WashProcess_BetweenSampleCompleted,
	en_WashProcess_Completed,
}en_WashProcess;

typedef struct
{
	float SampleOpticalDensity;
	float Result;
	float SampleBlankAbs;
	float PrimaryAbsBuffer[MAX_ADC_BUFFER_LENGTH_PRIMARY];
	float SecondaryAbsBuffer[MAX_ADC_BUFFER_LENGTH_DEFAULT];
	uint16_t TestId;
	uint16_t DarkAdcPrimary;
	uint16_t DarkAdcSecondary;
	float DIPrimaryFilterWaterGain;
	float DISecondaryFilterWaterGain;
	uint16_t DIPrimaryFilterWaterAdc;
	uint16_t DISecondaryFilterWaterAdc;
	uint16_t AdcDataFetchCount;
	uint16_t PrimaryAdcBuffer[MAX_ADC_BUFFER_LENGTH_PRIMARY];/*Primary will have incubation data too*/
	uint16_t SecondaryAdcBuffer[MAX_ADC_BUFFER_LENGTH_DEFAULT];/*Secondary will only have secondary data*/
	uint16_t fPreampDarkADCVal[en_PreampGain_Max];
	char arrErrorMessage[32];
	char arrPatientIDBuffer[1 + MAX_PATIENT_ID_LENGTH];
	char arrUserName[1 + MAX_USERNAME_LENGTH];
	uint8_t SelectedStandard;
	bool WaterBlankDoneFlag;
	bool SampleBlankDoneFlag;
	bool QCDoneFlag;
	bool QCBlankDoneFlag;
	bool TestParamSaveEnabled;
	en_TestRunStatus CurrentTestStatus;/*Current test procedure status*/
	en_TestRunStatus NextTestStatus;/*Type of test procedure status ahead of current status*/
	enMeasurementType CurrentMeasType;/*Type of measurement currently selected*/
	enMeasurementType PrevMeasCompletedType;/*Type of measurement last completed*/
}TestData;

extern TestData stcTestData;

typedef struct
{
	stcNvmDateTime Datetime;
	float OD[MAX_NUM_OF_STANDARD_OD + 1];
	uint8_t NumOfOdSaved;
}CalTrace;

extern uint16_t e_NumOfTestParamDataAvailable;
//extern Testparams stcTestparams[MAX_NUM_OF_TEST_PARAMS_SAVED];

void HandlerAspSwitchPressed(void);
void AbortTestRun(void);
void WashProcedure_Task(void);
en_WashProcess Get_WashStatus(void);
void Abort_AutoWash(void);
void Start_Wash(void);
void ExecuteTestProcedure(void);
void StartTestExecution(void);
en_TestRunStatus ReadTestRunStatus(void);
void InitilizetestPheripherals(void);
TestData* GetInstance_TestData(void);
uint32_t GetCurrentgainDarkADCVal(void);
void UpdateAbsBuffer(std::string abs);
void UpdateConentrationBuffer(std::string conc);
std::string FormatDouble(double value, int precision);
#endif /* APPL_INC_TESTRUN_H_ */
