/*
 * NVM_TestParameters.hpp
 *
 *  Created on: Feb 13, 2024
 *      Author: Alvin
 */

#ifndef APPL_INC_NVM_TESTPARAMETERS_HPP_
#define APPL_INC_NVM_TESTPARAMETERS_HPP_

#define DEFAULT_LAG_TIME (5U)
#define DEFAULT_READ_TIME (5U)
#define DEFAULT_TEMPERATURE (37)
#define DEFAULT_NUM_OF_STANDRADS (0U)
#define DEFAULT_K_FACTOR (1)
#define DEFAULT_TURBIABSLIMIT (1.5)
#define DEFAULT_ABS_LOW (0)
#define DEFAULT_ABS_LOW_NEG (-1)
#define DEFAULT_ABS_HIGH (3)
#define DEFAULT_REF_LOW (0)
#define DEFAULT_REF_HIGH (0)
#define DEFAULT_REG_BLANK_LOW (0)
#define DEFAULT_REG_BLANK_HIGH (0)
#define DEFAULT_LINEARITY_LOW (0)
#define DEFAULT_LINEARITY_HIGH (0)
#define MAX_DELTA_PER_ABS_VAL (0)
#define MAX_CUSTOM_UNITS_LENGTH (8)


typedef struct
{
	uint32_t ValidTestParamSavedFlag:8;
	uint32_t NumOfCalTraceSaved:8;
	uint32_t : 10;
	uint32_t DiwaterDoneFlag:1;
	uint32_t bCustomUNitEnable:1;
	uint32_t ReagentDoneFlag:1;
	uint32_t ReagentBlankDoneFlag:1;
	uint32_t SampleBlankEnable:1;
	uint32_t ReagentBlankEnable:1;

	uint32_t NumOfStandardsSaved;
	uint32_t IncubationTimeSec;
	uint32_t MeasurementTimeSec;
	uint32_t AspVoleume;

	float DiwaterPriADC;
	float DiwaterSecADC;
	float DiwaterPriGain;
	float DiwaterSecGain;
	float ReagentAbs;
	float ReagentBlankAbs;
	float ReagentBlankLimit[en_RangeMax];//low and high
	float AbsorbanceLimit[en_RangeMax];
	float RefrenceRange[en_RangeMax];
	float LinearityRange[en_RangeMax];
	float MaxDeltaAbsPerMin;
	float Temperature;
	float KFactor;
	float TurbiAbsLimit;
	float StandardConc[MAX_NUM_OF_STANDARD_CONC];
	float StandardOD[MAX_NUM_OF_STANDARD_OD];
	float StandardODBlank[MAX_NUM_OF_STANDARD_OD];
	float FourplA;
	float FourplB;
	float FourplC;
	float FourplD;
	float Corr_A;
	float Corr_B;
	bool StandardDoneFlag[MAX_NUM_OF_STANDARD_OD];
	bool StandardBlankDoneFlag[MAX_NUM_OF_STANDARD_OD];
	char arrTestName[MAX_TEST_NAME_CHAR_LENGTH + 1];
	char arrFullTestName[MAX_FULL_TEST_NAME_CHAR_LENGTH + 1];
	char arrCustomUnit[MAX_CUSTOM_UNITS_LENGTH];
	CalTrace stcCalTrace[MAX_NUM_OF_CAL_TRACE];
	en_WavelengthFilter PrimWavelength;
	en_WavelengthFilter SecWavelength;
	enTestProcedureType TestProcedureType;
	enUnits Unit;
	enCuvetteType CuvetteType;
	enCalibrationType Calib;
}Testparams;

bool ReadTestParameterBuffer(void);
void InitALLTestParameterBuffer(void);
void InitTestParameterBuffer(uint32_t nI);
uint32_t WriteTestparameterBuffer(void);
uint16_t GetTestParamsTotalItemSaved(void);
void ArrangeTestparamMemory(void);
uint8_t GetSlotForSavingTestParam(void);
Testparams* GetInstance_TestParams(void);
#endif /* APPL_INC_NVM_TESTPARAMETERS_HPP_ */
