/*
 * NonVolatileMemory.c
 *
 *  Created on: Aug 31, 2022
 *      Author: Alvin
 */
#include "../../APPL/Inc/NonVolatileMemory.h"
#include "../Libraries/json/cJSON_Utils.h"
#include "../Libraries/json/cJSON.h"
#include "../../APPL/Inc/ApplUsbDevice.h"
#include "../APPL/Inc/NVM_QCSetup.hpp"
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "../APPL/Inc/NVM_TestResultsHistory.hpp"
#include <string.h>
#include "Buzzer.h"
#include <HandlerPheripherals.hpp>

stcSettings objstcSettings;
stcSecondarySettings objstcSecondarySettings;
extern enSystemMemoryPeripherals stcMemoryPeripherals;

///*Total Resistance (Mega ohm) values arranged on oder of each preamp gain*/
///*Change here if resistance of each gain is changed*/
//const float g_DefaultPreampGainResVal[en_PreampGain_Max] = {float(0),//0
//															float(6.9),//1
//															float(20),//2
//															float(5.130111524),//3
//															float(100),//4
//															float(6.45463049579046),//5
//															float(16.6666666666667),//6
//															float(4.87977369165488),//7
//															float(300),//8
//															float(6.74486803519062),//9
//															float(18.75),//10
//															float(5.04385964912281),//11
//															float(75),//12
//															float(6.31868131868132),//13
//															float(15.7894736842105),//14
//															float(4.80167014613779)};//15


void InitSettingsBuffer(void)
{
	objstcSettings.InitFlag1 = (uint32_t)NVM_INIT_OK_FLAG;
	objstcSettings.InitFlag2 = (uint32_t)NVM_INIT_OK_FLAG;
	objstcSettings.u32BuzzerEnable = true;
	objstcSettings.u32PrinterEnable = true;
	objstcSettings.BarcodeBaudRate = 9600;
	objstcSettings.PrinterBaudRate = 9600;

	objstcSettings.FilterPreampGain[en_FilterHome] = en_PreampGain_OFF;
	objstcSettings.FilterPreampGain[en_Filter340] = en_PreampGain_2;//Default gain for filter
	objstcSettings.FilterPreampGain[en_Filter405] = en_PreampGain_1;//Default gain for filter
//	objstcSettings.FilterPreampGain[en_Filter450] = en_PreampGain_1;//Default gain for filter
	objstcSettings.FilterPreampGain[en_Filter505] = en_PreampGain_3;//Default gain for filter
	objstcSettings.FilterPreampGain[en_Filter546] = en_PreampGain_3;//Default gain for filter
	objstcSettings.FilterPreampGain[en_Filter578] = en_PreampGain_3;//Default gain for filter
	objstcSettings.FilterPreampGain[en_Filter630] = en_PreampGain_3;//Default gain for filter
//	objstcSettings.FilterPreampGain[en_Filter670] = en_PreampGain_3;//Default gain for filter
//	objstcSettings.FilterPreampGain[en_Filter700] = en_PreampGain_15;//Default gain for filter
	objstcSettings.FilterPreampGain[en_FilterBlack] = en_PreampGain_15;//Default gain for filter

	objstcSettings.AngleFilterToFilter = FILTER_ANGLE_OFFSET_FROM_HOME; // filter to filter angle
	objstcSettings.AngleLightPathOffset = OFFSET_FILTER_HOME_TO_LIGHT_PATH; // filter to home angle
	objstcSettings.WavelegthFilterAngles[en_FilterHome] = (0);
	objstcSettings.WavelegthFilterAngles[en_Filter340] = ((objstcSettings.AngleFilterToFilter * 0) + 1) + objstcSettings.AngleLightPathOffset;
	objstcSettings.WavelegthFilterAngles[en_Filter405] = ((objstcSettings.AngleFilterToFilter * 1) + 1) + objstcSettings.AngleLightPathOffset;
//	objstcSettings.WavelegthFilterAngles[en_Filter450] = ((objstcSettings.AngleFilterToFilter * 2) + 1) + objstcSettings.AngleLightPathOffset;
	objstcSettings.WavelegthFilterAngles[en_Filter505] = ((objstcSettings.AngleFilterToFilter * 3) + 1) + objstcSettings.AngleLightPathOffset;
	objstcSettings.WavelegthFilterAngles[en_Filter546] = ((objstcSettings.AngleFilterToFilter * 4) + 1) + objstcSettings.AngleLightPathOffset;
	objstcSettings.WavelegthFilterAngles[en_Filter578] = ((objstcSettings.AngleFilterToFilter * 5) + 1) + objstcSettings.AngleLightPathOffset;
	objstcSettings.WavelegthFilterAngles[en_Filter630] = ((objstcSettings.AngleFilterToFilter * 6) + 1) + objstcSettings.AngleLightPathOffset;
//	objstcSettings.WavelegthFilterAngles[en_Filter670] = ((objstcSettings.AngleFilterToFilter * 7) + 1) + objstcSettings.AngleLightPathOffset;
//	objstcSettings.WavelegthFilterAngles[en_Filter700] = ((objstcSettings.AngleFilterToFilter * 9) + 1) + objstcSettings.AngleLightPathOffset;
	objstcSettings.WavelegthFilterAngles[en_FilterBlack] = objstcSettings.AngleFilterToFilter + 1;

	objstcSettings.u_n8DcMotorPwmDuty = (DEFAULT_DC_MOTOR_DUTY_ON);
	objstcSettings.u_n8FilterMotorRPM = (DEFAULT_FILTER_MOTOR_RPM);
	objstcSettings.u16SavedFilterAdc[en_FilterHome] = 0;
	objstcSettings.u16SavedFilterAdc[en_Filter340] = 0;
	objstcSettings.u16SavedFilterAdc[en_Filter405] = 0;
//	objstcSettings.u16SavedFilterAdc[en_Filter450] = 0;
	objstcSettings.u16SavedFilterAdc[en_Filter505] = 0;
	objstcSettings.u16SavedFilterAdc[en_Filter546] = 0;
	objstcSettings.u16SavedFilterAdc[en_Filter578] = 0;
	objstcSettings.u16SavedFilterAdc[en_Filter630] = 0;
//	objstcSettings.u16SavedFilterAdc[en_Filter670] = 0;
	objstcSettings.u16SavedFilterAdc[en_FilterBlack] = 0;
//	for(uint16_t NoOfGain = en_PreampGain_OFF; NoOfGain < en_PreampGain_Max; ++NoOfGain)
//	{
//		objstcSettings.fPreampDarkADCVal[NoOfGain] = 0;
//	}
	objstcSettings.fTemperatureOffset[en_PhotometerTemp] = IDEAL_TEMP_TOLERANCE;
	objstcSettings.fTemperatureOffset[en_ChamperTemp] = IDEAL_TEMP_TOLERANCE;
	objstcSettings.fTempSensSlope[en_PhotometerTemp] = (float)(1.0);
	objstcSettings.fTempSensSlope[en_ChamperTemp] = (float)(1.0);
	objstcSettings.fTempSensIntercept[en_PhotometerTemp] = (float)(0);
	objstcSettings.fTempSensIntercept[en_ChamperTemp] = (float)(0);

	objstcSettings.fTargetTemp[en_PhotometerTemp] = (float)(37.0);
	objstcSettings.fTargetTemp[en_ChamperTemp] = (float)(37.0);

	objstcSettings.fOffsetTemp[en_PhotometerTemp] =(float)(0);
	objstcSettings.fOffsetTemp[en_ChamperTemp] =(float)(0);

	objstcSettings.fPID_kP[en_PhotometerTemp] = (float)(50);
	objstcSettings.fPID_kI[en_PhotometerTemp] = (float)(5);
	objstcSettings.fPID_kD[en_PhotometerTemp] = (float)(20);

	objstcSettings.fPID_kP[en_ChamperTemp] = (float)(50);
	objstcSettings.fPID_kI[en_ChamperTemp] = (float)(5);
	objstcSettings.fPID_kD[en_ChamperTemp] = (float)(20);
	objstcSettings.fTempControlOnOff[en_PhotometerTemp] = true;
	objstcSettings.fTempControlOnOff[en_ChamperTemp] = false;

	strncpy(objstcSettings.arr_CLabName,DEFAULT_LAB_NAME,(MAX_LAB_NAME_LENGTH - 2));
	memset(objstcSettings.arr_CFirstHeader,0,sizeof(objstcSettings.arr_CFirstHeader));
	memset(objstcSettings.arr_CSecondHeader,0,sizeof(objstcSettings.arr_CSecondHeader));
	memset(objstcSettings.arr_CEndfooter,0,sizeof(objstcSettings.arr_CEndfooter));

	strncpy(objstcSettings.arr_cAdminPassword , DEFAULT_ADMIN_PASSWORD , MAX_PASSWORD_LENGTH);
	strncpy(objstcSettings.arr_cServicePassword , DEFAULT_SERVICE_PASSWORD , MAX_PASSWORD_LENGTH);
	strncpy(objstcSettings.arr_COEMEditPassword , DEFAULT_OEMEDIT_PASSWORD , MAX_PASSWORD_LENGTH);

	for(uint8_t nI = 0 ; nI < MAX_NUM_OF_USER ; ++nI)
	{
		objstcSettings.NormalUser[nI].Enable = NVM_INIT_NOT_OK_FLAG;
	}

	objstcSettings.u16LampDelayTime = DEFAULT_LAMP_DELAY_MIN;
	objstcSettings.fWashVoleume_uL = DEFAULT_WASH_VOLUME_uL;
	objstcSettings.fPumpConstant = DEFAULT_PUMP_CONSTANT;
	objstcSettings.fAirGapVol_uL = DEFAULT_AIRGAP_VOLEUME;
	objstcSettings.fTurbiAbsLimit = DEFAULT_TURBI_ABS;

	objstcSettings.fGainResCalculatorBit[enPreGainResValR1] = DEF_PRE_GAIN_RES_B0;
	objstcSettings.fGainResCalculatorBit[enPreGainResValR2] = DEF_PRE_GAIN_RES_B1;
	objstcSettings.fGainResCalculatorBit[enPreGainResValR3] = DEF_PRE_GAIN_RES_B2;
	objstcSettings.fGainResCalculatorBit[enPreGainResValR4] = DEF_PRE_GAIN_RES_B3;
	CalculatePreampGainResistance();
}

void InitSecondarySettingsBuffer(void)
{
	objstcSecondarySettings.InitFlag1 = (uint32_t)NVM_INIT_OK_FLAG;
	objstcSecondarySettings.InitFlag2 = (uint32_t)NVM_INIT_OK_FLAG;
	objstcSecondarySettings.u32LampWorkMinutes = 0;
	objstcSecondarySettings.u32LampWorkHours = 0;
}
//uint32_t WriteSettingsBuffer(void)
//{
//	uint32_t LengthOfData = sizeof(objstcSettings);
//	uint32_t blockAddress = enFlashBlockAddressSettings;// * w25qxx.BlockSize;
//	W25qxx_WriteBlock((uint8_t*)&objstcSettings , blockAddress , 0 , LengthOfData);
//	return LengthOfData;
//}
//
//bool ReadSettingsBuffer(void)
//{
//	uint32_t blockAddress = enFlashBlockAddressSettings;// * w25qxx.BlockSize;
//	uint32_t LengthOfData = sizeof(objstcSettings);
//	memset(&objstcSettings , 0 , LengthOfData);
//	W25qxx_ReadBlock((uint8_t*)&objstcSettings , blockAddress , 0 , LengthOfData);
//	return true;
//}

// Assuming objstcSettings is a globally defined variable of type stcSettings

uint32_t WriteSettingsBuffer(void)
{
    uint32_t LengthOfData = sizeof(objstcSettings);
    uint32_t blockAddressMain = enFlashBlockAddressSettings;  // Main block address
    uint32_t blockAddressBackup = enFlashBlockAddressSettings_Backup;  // Backup block address
	stcMemoryPeripherals.SettingsBackUpWritefailed = false;
    // Write to backup sector first
    W25qxx_WriteBlock((uint8_t*)&objstcSettings, blockAddressBackup, 0, LengthOfData);

    // Verify backup data integrity
    W25qxx_ReadBlock((uint8_t*)&objstcSettings, blockAddressBackup, 0, LengthOfData);

    // Check if the backup data is valid
    if (VerifySettingsIntegrity(&objstcSettings))
    {
        // If backup is valid, write to the main sector
        W25qxx_WriteBlock((uint8_t*)&objstcSettings, blockAddressMain, 0, LengthOfData);
        return LengthOfData;
    }
    else
    {
    	stcMemoryPeripherals.SettingsBackUpWritefailed = true;
		MemoryCorruptionBuzzer();
    }
    return 0;
}

bool ReadSettingsBuffer(void)
{
    uint32_t blockAddressMain = enFlashBlockAddressSettings;        // Main block
    uint32_t blockAddressBackup = enFlashBlockAddressSettings_Backup; // Backup block
    uint32_t LengthOfData = sizeof(objstcSettings);

    stcMemoryPeripherals.SettingsMemCorrpt = false;
    stcMemoryPeripherals.SettingsBackUpMemCorrpt = false;

    memset(&objstcSettings, 0, LengthOfData);

    // Read from main block
    W25qxx_ReadBlock((uint8_t*)&objstcSettings, blockAddressMain, 0, LengthOfData);

    if (!VerifySettingsIntegrity(&objstcSettings))
    {
        stcMemoryPeripherals.SettingsMemCorrpt = true;

        // Try reading from backup
        W25qxx_ReadBlock((uint8_t*)&objstcSettings, blockAddressBackup, 0, LengthOfData);

        if (!VerifySettingsIntegrity(&objstcSettings))
        {
            stcMemoryPeripherals.SettingsBackUpMemCorrpt = true;
            MemoryCorruptionBuzzer();
        }
        else
        {
            // Restore valid backup to main
            stcMemoryPeripherals.SettingsMemCorrpt = false;
            W25qxx_WriteBlock((uint8_t*)&objstcSettings, blockAddressMain, 0, LengthOfData);
        }
    }
    else
    {
        // Main is valid, now check backup
        uint8_t *pBackupBuffer = (uint8_t *)malloc(LengthOfData);

		W25qxx_ReadBlock(pBackupBuffer, blockAddressBackup, 0, LengthOfData);

		if (!VerifySettingsIntegrity((stcSettings*)pBackupBuffer))
		{
			// Restore main data to backup
			W25qxx_WriteBlock((uint8_t*)&objstcSettings, blockAddressBackup, 0, LengthOfData);
		}

		free(pBackupBuffer);
    }

    return true;
}



//bool ReadSettingsBuffer(void)
//{
//    uint32_t blockAddressMain = enFlashBlockAddressSettings;  // Main block address
//    uint32_t blockAddressBackup = enFlashBlockAddressSettings_Backup;  // Backup block address
//    uint32_t LengthOfData = sizeof(objstcSettings);
//
//	stcMemoryPeripherals.SettingsMemCorrpt = false;
//	stcMemoryPeripherals.SettingsBackUpMemCorrpt = false;
//
//	memset(&objstcSettings , 0 , LengthOfData);
//    // First, attempt to read from the main sector
//    W25qxx_ReadBlock((uint8_t*)&objstcSettings, blockAddressMain, 0, LengthOfData);
//
//    // Check integrity of data from the main sector
//    if (!VerifySettingsIntegrity(&objstcSettings))
//    {
//    	stcMemoryPeripherals.SettingsMemCorrpt = true;
//        // If main sector is corrupted, attempt to read from the backup sector
//		TestTimerCompleteBuzzer();
//        W25qxx_ReadBlock((uint8_t*)&objstcSettings, blockAddressBackup, 0, LengthOfData);
//
//        // Check integrity of the backup data
//        if (!VerifySettingsIntegrity(&objstcSettings))
//        {
//            // If both main and backup are corrupted, return false
//        	stcMemoryPeripherals.SettingsBackUpMemCorrpt = true;
//    		MemoryCorruptionBuzzer();
//        }
//        else
//        {
//            // If backup is valid, restore from backup and write back to main sector
//            W25qxx_WriteBlock((uint8_t*)&objstcSettings, blockAddressMain, 0, LengthOfData);
//        }
//    }
//
//    return true;  // Data read successfully
//}

// Function to verify the integrity of settings data using InitFlag1 and InitFlag2
bool VerifySettingsIntegrity(stcSettings* settings)
{
    if (settings->InitFlag1 != (uint32_t)NVM_INIT_OK_FLAG || settings->InitFlag2 != (uint32_t)NVM_INIT_OK_FLAG)
    {
        return false; // Data is corrupted
    }
    return true; // Data is valid
}


uint32_t WriteSecondarySettingsBuffer(void)
{
    uint32_t LengthOfData = sizeof(objstcSecondarySettings);
    uint32_t blockAddressMain = enFlashBlockAddressSecondarySettings;  // Main block address
    uint32_t blockAddressBackup = enFlashBlockAddressSecondarySettings_Backup;  // Backup block address
	stcMemoryPeripherals.SecondarySettingsBackUpWritefailed = false;
    // Write to backup sector first
    W25qxx_WriteBlock((uint8_t*)&objstcSecondarySettings, blockAddressBackup, 0, LengthOfData);

    // Verify backup data integrity
    W25qxx_ReadBlock((uint8_t*)&objstcSecondarySettings, blockAddressBackup, 0, LengthOfData);

    // Check if the backup data is valid
    if (VerifySecondarySettingsIntegrity(&objstcSecondarySettings))
    {
        // If backup is valid, write to the main sector
        W25qxx_WriteBlock((uint8_t*)&objstcSecondarySettings, blockAddressMain, 0, LengthOfData);
        return LengthOfData;
    }
    else
    {
    	stcMemoryPeripherals.SecondarySettingsBackUpWritefailed = true;
		MemoryCorruptionBuzzer();
    }
    return 0;
}

bool ReadSecondarySettingsBuffer(void)
{
    uint32_t blockAddressMain = enFlashBlockAddressSecondarySettings;  // Main block address
    uint32_t blockAddressBackup = enFlashBlockAddressSecondarySettings_Backup;  // Backup block address
    uint32_t LengthOfData = sizeof(objstcSecondarySettings);

	stcMemoryPeripherals.SecondarySettingsMemCorrpt = false;
	stcMemoryPeripherals.SecondarySettingsBackUpMemCorrpt = false;

	memset(&objstcSecondarySettings , 0 , LengthOfData);
    // First, attempt to read from the main sector
    W25qxx_ReadBlock((uint8_t*)&objstcSecondarySettings, blockAddressMain, 0, LengthOfData);

    // Check integrity of data from the main sector
    if (!VerifySecondarySettingsIntegrity(&objstcSecondarySettings))
    {
    	stcMemoryPeripherals.SecondarySettingsMemCorrpt = true;
        // If main sector is corrupted, attempt to read from the backup sector
		TestTimerCompleteBuzzer();
        W25qxx_ReadBlock((uint8_t*)&objstcSecondarySettings, blockAddressBackup, 0, LengthOfData);

        // Check integrity of the backup data
        if (!VerifySecondarySettingsIntegrity(&objstcSecondarySettings))
        {
            // If both main and backup are corrupted, return false
        	stcMemoryPeripherals.SecondarySettingsBackUpMemCorrpt = true;
    		MemoryCorruptionBuzzer();
        }
        else
        {
            // If backup is valid, restore from backup and write back to main sector
            W25qxx_WriteBlock((uint8_t*)&objstcSecondarySettings, blockAddressMain, 0, LengthOfData);
        }
    }
    else
    {
        // Main is valid, now check backup
        uint8_t *pBackupBuffer = (uint8_t *)malloc(LengthOfData);

		W25qxx_ReadBlock(pBackupBuffer, blockAddressBackup, 0, LengthOfData);

		if (!VerifySecondarySettingsIntegrity((stcSecondarySettings*)pBackupBuffer))
		{
			// Restore main data to backup
			W25qxx_WriteBlock((uint8_t*)&objstcSecondarySettings, blockAddressBackup, 0, LengthOfData);
		}

		free(pBackupBuffer);
    }

    return true;  // Data read successfully
}

// Function to verify the integrity of settings data using InitFlag1 and InitFlag2
bool VerifySecondarySettingsIntegrity(stcSecondarySettings* settings)
{
    if (settings->InitFlag1 != (uint32_t)NVM_INIT_OK_FLAG || settings->InitFlag2 != (uint32_t)NVM_INIT_OK_FLAG)
    {
        return false; // Data is corrupted
    }
    return true; // Data is valid
}

void ResetNVMFlash(void)
{
	InitSettingsBuffer();
	InitSecondarySettingsBuffer();
	InitALLTestParameterBuffer();
	DeleteAllestHistory();
	InitQcSetup();
//	InitQcData();
	Delete_QcSetup();

	WriteSettingsBuffer();
	WriteSecondarySettingsBuffer();
	WriteTestparameterBuffer();/*Writing tets params to memory*/
	NVM_WriteQcSetup();
}

stcSettings* GetInstance_SettingsData(void)
{
	return (&objstcSettings);
}
