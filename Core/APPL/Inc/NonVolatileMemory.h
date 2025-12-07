/*
 * NonVolatileMemory.h
 *
 *  Created on: Aug 31, 2022
 *      Author: Alvin
 */
//#include "SDCard.h"
#include "TempratureSens.h"
#include "FilterMotor.h"
#include "Preamp.h"
#include "DcMotor.hpp"
#include <stdlib.h>
#include "w25qxxFlash.h"

#ifndef APPL_INC_NONVOLATILEMEMORY_H_
#define APPL_INC_NONVOLATILEMEMORY_H_


#define MAX_LAB_NAME_LENGTH (22)
#define MAX_OEM_NAME_LENGTH (32)

#define DEFAULT_LAB_NAME ""
#define SETTINGS_FILE "settings.json"
#define TEST_PARAMS_FILE "TP"

#define DEFAULT_ADMIN_PASSWORD 		"ADMIN"/*default password can be used to reset*/
#define DEFAULT_SERVICE_PASSWORD 	"ROOT"/*default password can be used to reset*/
#define DEFAULT_SUPER_USER_ADMIN_PASSWORD "SADMIN"/*password for super user to reset all passwords*/
#define DEFAULT_SUPER_USER_SERVICE_PASSWORD "SROOT"/*password for super user to reset all passwords*/

#define DEFAULT_ELSCION_NAME 		"ARIS 100 CHEMISTRY ANALYZER"
#define DEFAULT_OEM1_NAME			"LABCHEM 7 PRO CHEMISTRY ANALYZER"
#define DEFAULT_OEM2_NAME			"RIVO CHEM 100 CHEMISTRY ANALYZER"
#define DEFAULT_OEM3_NAME			" "

#define DEFAULT_OEMEDIT_PASSWORD 	"ES25MS"/*default password can be used to reset*/

#define MAX_USERNAME_LENGTH (7)
#define MAX_PASSWORD_LENGTH (7)
#define MAX_NUM_OF_USER (4)
#define DEFAULT_WASH_VOLUME_uL (1000)/*uL - Micro Liter*/
#define DEFAULT_LAMP_DELAY_MIN (15)/*Minute*/
#define DEFAULT_TURBI_ABS (1.5)/*Minute*/
#define NVM_INIT_OK_FLAG (0xA2)
#define NVM_INIT_NOT_OK_FLAG (0xFF)

#define NVM_OEM_ENABLE_FLAG (0xA5)
#define NVM_OEM_DISABLE_FLAG (0xFF)

//typedef enum
//{
//	enFlashBlockAddressSettings = 0,/*1 block (64KB)*//*W25Q128 FLASH chip*/
//	enFlashBlockAddressErrWarnings = 1,/*1 block (64KB)*//*W25Q128 FLASH chip*/
//	enFlashBlockAddressTestParams = 2,/*1 block (64KB)*//*W25Q128 FLASH chip*/
//	enFlashBlockAddressResHistory_B0_Start = 4,/*2500 results per block - 1 block equals 64KB in W25Q128 FLASH chip*/
//	enFlashBlockAddressResHistory_End = 7,/*2500 results per block - 1 block equals 64KB in W25Q128 FLASH chip*/
//	enFlashBlockQcSetup = 8,
//	enFlashBlockAddsQcData_Start = 9,/*1024 per data , (200 tests / 64 data per block = 3.125)*/
//	enFlashBlockAddsQcData_End = 12,
//}enFlashAddressMapping;

typedef enum
{
	/* Main Blocks */
	enFlashBlockAddressSettings = 0,
	enFlashBlockAddressSecondarySettings = 1,
	enFlashBlockAddressTestParams = 2,

	/* Result History (Main) */
	enFlashBlockAddressResHistory_B0_Start = 4,
	enFlashBlockAddressResHistory_End = 7,

	/* QC Setup */
	enFlashBlockQcSetup = 8,

	/* QC Data (Main) */
	enFlashBlockAddsQcData_Start = 9,
	enFlashBlockAddsQcData_End = 12,

	/* Backup Blocks */
	enFlashBlockAddressSettings_Backup = 13,
	enFlashBlockAddressSecondarySettings_Backup = 14,
	enFlashBlockAddressTestParams_Backup = 15,

	/* Result History (Backup) */
	enFlashBlockAddressResHistory_B0_Start_Backup = 17,
	enFlashBlockAddressResHistory_End_Backup = 20,

	/* QC Setup Backup */
	enFlashBlockQcSetup_Backup = 21,

	/* QC Data (Backup) */
	enFlashBlockAddsQcData_Start_Backup = 22,
	enFlashBlockAddsQcData_End_Backup = 25,

} enFlashAddressMapping;


typedef enum
{
	enPreGainResValR1 = 0,
	enPreGainResValR2,
	enPreGainResValR3,
	enPreGainResValR4,
	enPreGainResValMax,
}en_PreGainResVal;

typedef enum
{
	en_Temp25Degree = 0,
	en_Temp30Degree,
	en_Temp37Degree,
	en_TempDegreeMax
}enTemperatureSetting;

typedef struct
{
	uint32_t Enable;
	uint32_t reserved;
	char arrUsername[1 + MAX_USERNAME_LENGTH]; //MAX_USERNAME_LENGTH = 7
	char arrPassword[1 + MAX_PASSWORD_LENGTH]; //MAX_PASSWORD_LENGTH = 7
}stcUser;

typedef struct
{
	uint32_t InitFlag1;
	uint32_t u_n8DcMotorPwmDuty;
	uint32_t BarcodeBaudRate;
	uint32_t PrinterBaudRate;
	uint32_t u_n8FilterMotorRPM;
	uint32_t u16LampDelayTime;

	uint32_t u32BuzzerEnable : 1;
	uint32_t u32PrinterEnable : 1;
	uint32_t u32AutoWashEnable : 1;
	uint32_t reserved : 29;					 // 4 bytes

	uint16_t AngleFilterToFilter; 			 // 2 bytes
	uint16_t AngleLightPathOffset; 			 // 2 bytes

	uint16_t u16SavedFilterAdc[en_FilterMax]; // en_FilterMax = 8 // 16 Bytes
	uint16_t fTempControlOnOff[en_MaxTempCntrl];// Intercept of Temp sensor 4bytes

	uint16_t Elscion_Model; 		// 2 bytes	//Elscion model
	uint16_t OEM1_Model; 	 	// 2 bytes  //Labnova model

	uint16_t OEM2_Model; 	    // 2 bytes
	uint16_t OEM3_Model;     	// 2 bytes

	float fPumpConstant;
	float fTurbiAbsLimit;
	float fWashVoleume_uL;
	float fAirGapVol_uL;
	float WavelegthFilterAngles[en_FilterMax];//angles for each and every filter
	float fTargetTemp[en_MaxTempCntrl]; // en_MaxTempCntrl = 2
	float fOffsetTemp[en_MaxTempCntrl];  // this offset values will set by production/service
	float fTemperatureOffset[en_MaxTempCntrl]; // this offset values will set by user
	float fPID_kP[en_MaxTempCntrl];// kP of PID
	float fPID_kI[en_MaxTempCntrl];// kI of PID
	float fPID_kD[en_MaxTempCntrl];// kD of PID
	float fTempSensSlope[en_MaxTempCntrl];// Slope of Temp sensor
	float fTempSensIntercept[en_MaxTempCntrl];// Intercept of Temp sensor

	float fPreampGainResistanceVal[en_PreampGain_Max]; // en_PreampGain_Max = 16
	float fGainResCalculatorBit[enPreGainResValMax]; // enPreGainResValMax = 4

	enPreampGain FilterPreampGain[en_FilterMax];  // 8 bytes

	char arr_CLabName[MAX_LAB_NAME_LENGTH + 2];/*For printer*/ // MAX_LAB_NAME_LENGTH = 22
	char arr_CFirstHeader[MAX_LAB_NAME_LENGTH + 2];/*For printer*/
	char arr_CSecondHeader[MAX_LAB_NAME_LENGTH + 2];/*For printer*/
	char arr_CEndfooter[MAX_LAB_NAME_LENGTH + 2];/*For printer*/
	char arr_cAdminPassword[MAX_PASSWORD_LENGTH + 1]; // MAX_PASSWORD_LENGTH = 7
	char arr_cServicePassword[MAX_PASSWORD_LENGTH + 1];
	char arr_COEMEditPassword[MAX_PASSWORD_LENGTH + 1];

	stcUser NormalUser[MAX_NUM_OF_USER]; // MAX_NUM_OF_USER = 4
	uint32_t InitFlag2;

}stcSettings;

extern stcSettings objstcSettings;


typedef struct
{
	uint32_t InitFlag1;
	uint32_t u32LampWorkMinutes;
	uint32_t u32LampWorkHours;
	uint32_t InitFlag2;

}stcSecondarySettings;

extern stcSecondarySettings objstcSecondarySettings;

void InitSecondarySettingsBuffer(void);
bool ReadSecondarySettingsBuffer(void);
bool VerifySecondarySettingsIntegrity(stcSecondarySettings* settings);
uint32_t WriteSecondarySettingsBuffer(void);



void InitSettingsBuffer(void);
bool ReadSettingsBuffer(void);
bool VerifySettingsIntegrity(stcSettings* settings);
uint32_t WriteSettingsBuffer(void);
void ResetNVMFlash(void);
stcSettings* GetInstance_SettingsData(void);

#endif /* APPL_INC_NONVOLATILEMEMORY_H_ */
