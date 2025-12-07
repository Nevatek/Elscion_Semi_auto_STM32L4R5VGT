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
#include "../Screens/Inc/CommonDisplayFunctions.h"
#include "../APPL/Inc/Testrun.h"
#include "../../APPL/Inc/NVM_TestParameters.hpp"
#include "../APPL/Inc/NVM_QCSetup.hpp"
#include <string.h>
#include <string>
#include "Buzzer.h"
#include <HandlerPheripherals.hpp>

extern enSystemMemoryPeripherals stcMemoryPeripherals;
Testparams stcTestparams[MAX_NUM_OF_TEST_PARAMS_SAVED];
Testparams LastSavedTestParams;
QCSETUP p_QcSetup;

//bool ReadTestParameterBuffer(void)
//{
//    uint32_t u32Chunk = (MAX_NUM_OF_TEST_PARAMS_SAVED / 2);
//    uint32_t dataSize = (u32Chunk) * sizeof(stcTestparams[0]);
//    uint32_t totalSize = dataSize + 2 * sizeof(uint32_t); // Include InitFlags
//
//    uint32_t mainBlock1 = enFlashBlockAddressTestParams;
//    uint32_t mainBlock2 = enFlashBlockAddressTestParams + 1;
//    uint32_t backupBlock1 = enFlashBlockAddressTestParams_Backup;
//    uint32_t backupBlock2 = enFlashBlockAddressTestParams_Backup + 1;
//
//	stcMemoryPeripherals.TestParamsMemCorrpt = false;
//	stcMemoryPeripherals.TestParamsBackUpMemCorrpt = false;
//
//    uint8_t* readBuffer = (uint8_t*)malloc(totalSize);
//    uint32_t readInitFlag1, readInitFlag2;
//
//    // Step 1: Read first main block
//    W25qxx_ReadBlock(readBuffer, mainBlock1, 0, totalSize);
//    memcpy(&readInitFlag1, readBuffer, sizeof(uint32_t));
//    memcpy(&readInitFlag2, readBuffer + sizeof(uint32_t) + dataSize, sizeof(uint32_t));
//
//    if (readInitFlag1 == (uint32_t)NVM_INIT_OK_FLAG && readInitFlag2 == (uint32_t)NVM_INIT_OK_FLAG)
//    {
//        // First half is valid, copy to RAM
//        memcpy(&stcTestparams[0], readBuffer + sizeof(uint32_t), dataSize);
//    }
//    else
//    {
//    	stcMemoryPeripherals.TestParamsMemCorrpt = true;
//        // Read from backup if main block is corrupted
//        W25qxx_ReadBlock(readBuffer, backupBlock1, 0, totalSize);
//        memcpy(&readInitFlag1, readBuffer, sizeof(uint32_t));
//        memcpy(&readInitFlag2, readBuffer + sizeof(uint32_t) + dataSize, sizeof(uint32_t));
//
//        if (readInitFlag1 == (uint32_t)NVM_INIT_OK_FLAG && readInitFlag2 == (uint32_t)NVM_INIT_OK_FLAG)
//        {
//            // Backup is valid, restore to main
//            memcpy(&stcTestparams[0], readBuffer + sizeof(uint32_t), dataSize);
//            W25qxx_WriteBlock(readBuffer, mainBlock1, 0, totalSize);
//        }
//        else
//        {
//        	stcMemoryPeripherals.TestParamsBackUpMemCorrpt = true;
//    		MemoryCorruptionBuzzer();
//        }
//    }
//
//    // Step 2: Read second main block
//    W25qxx_ReadBlock(readBuffer, mainBlock2, 0, totalSize);
//    memcpy(&readInitFlag1, readBuffer, sizeof(uint32_t));
//    memcpy(&readInitFlag2, readBuffer + sizeof(uint32_t) + dataSize, sizeof(uint32_t));
//
//    if (readInitFlag1 == (uint32_t)NVM_INIT_OK_FLAG && readInitFlag2 == (uint32_t)NVM_INIT_OK_FLAG)
//    {
//        // Second half is valid, copy to RAM
//        memcpy(&stcTestparams[u32Chunk], readBuffer + sizeof(uint32_t), dataSize);
//    }
//    else
//    {
//    	stcMemoryPeripherals.TestParamsMemCorrpt = true;
//        // Read from backup if main block is corrupted
//        W25qxx_ReadBlock(readBuffer, backupBlock2, 0, totalSize);
//        memcpy(&readInitFlag1, readBuffer, sizeof(uint32_t));
//        memcpy(&readInitFlag2, readBuffer + sizeof(uint32_t) + dataSize, sizeof(uint32_t));
//
//        if (readInitFlag1 == (uint32_t)NVM_INIT_OK_FLAG && readInitFlag2 == (uint32_t)NVM_INIT_OK_FLAG)
//        {
//            // Backup is valid, restore to main
//            memcpy(&stcTestparams[u32Chunk], readBuffer + sizeof(uint32_t), dataSize);
//            W25qxx_WriteBlock(readBuffer, mainBlock2, 0, totalSize);
//        }
//        else
//        {
//        	stcMemoryPeripherals.TestParamsBackUpMemCorrpt = true;
//    		MemoryCorruptionBuzzer();
//        }
//    }
//    free(readBuffer);
//    return true;
//}

bool ReadTestParameterBuffer(void)
{
    uint32_t u32Chunk = (MAX_NUM_OF_TEST_PARAMS_SAVED / 2);
    uint32_t dataSize = (u32Chunk) * sizeof(stcTestparams[0]);
    uint32_t totalSize = dataSize + 2 * sizeof(uint32_t); // InitFlag1 + Data + InitFlag2

    uint32_t mainBlock1 = enFlashBlockAddressTestParams;
    uint32_t mainBlock2 = enFlashBlockAddressTestParams + 1;
    uint32_t backupBlock1 = enFlashBlockAddressTestParams_Backup;
    uint32_t backupBlock2 = enFlashBlockAddressTestParams_Backup + 1;

    stcMemoryPeripherals.TestParamsMemCorrpt = false;
    stcMemoryPeripherals.TestParamsBackUpMemCorrpt = false;

    uint8_t* readBuffer = (uint8_t*)malloc(totalSize);

    for (int i = 0; i < 2; i++)
    {
        uint32_t mainBlock = (i == 0) ? mainBlock1 : mainBlock2;
        uint32_t backupBlock = (i == 0) ? backupBlock1 : backupBlock2;
        uint32_t offset = (i == 0) ? 0 : u32Chunk;

        uint32_t flag1 = 0, flag2 = 0;

        // Read from main
        W25qxx_ReadBlock(readBuffer, mainBlock, 0, totalSize);
        memcpy(&flag1, readBuffer, sizeof(uint32_t));
        memcpy(&flag2, readBuffer + sizeof(uint32_t) + dataSize, sizeof(uint32_t));

        if (flag1 == NVM_INIT_OK_FLAG && flag2 == NVM_INIT_OK_FLAG)
        {
            memcpy(&stcTestparams[offset], readBuffer + sizeof(uint32_t), dataSize);

            // Verify backup
            uint8_t* backupBuffer = (uint8_t*)malloc(totalSize);
			uint32_t bFlag1 = 0, bFlag2 = 0;
			W25qxx_ReadBlock(backupBuffer, backupBlock, 0, totalSize);
			memcpy(&bFlag1, backupBuffer, sizeof(uint32_t));
			memcpy(&bFlag2, backupBuffer + sizeof(uint32_t) + dataSize, sizeof(uint32_t));

			if (bFlag1 != NVM_INIT_OK_FLAG || bFlag2 != NVM_INIT_OK_FLAG)
			{
				stcMemoryPeripherals.TestParamsBackUpMemCorrpt = true;
				W25qxx_WriteBlock(readBuffer, backupBlock, 0, totalSize);
			}
			free(backupBuffer);
        }
        else
        {
            stcMemoryPeripherals.TestParamsMemCorrpt = true;

            // Read from backup
            W25qxx_ReadBlock(readBuffer, backupBlock, 0, totalSize);
            memcpy(&flag1, readBuffer, sizeof(uint32_t));
            memcpy(&flag2, readBuffer + sizeof(uint32_t) + dataSize, sizeof(uint32_t));

            if (flag1 == NVM_INIT_OK_FLAG && flag2 == NVM_INIT_OK_FLAG)
            {
            	stcMemoryPeripherals.TestParamsMemCorrpt = false;
                memcpy(&stcTestparams[offset], readBuffer + sizeof(uint32_t), dataSize);
                W25qxx_WriteBlock(readBuffer, mainBlock, 0, totalSize);
            }
            else
            {
                stcMemoryPeripherals.TestParamsBackUpMemCorrpt = true;
                MemoryCorruptionBuzzer();
            }
        }
    }

    free(readBuffer);
    return true;
}


uint32_t WriteTestparameterBuffer(void)
{
    ArrangeTestparamMemory(); /* Arrange buffer before writing */

    uint32_t u32Chunk = (MAX_NUM_OF_TEST_PARAMS_SAVED / 2);
    uint32_t dataSize = (u32Chunk) * sizeof(stcTestparams[0]);
    uint32_t totalSize = dataSize + 2 * sizeof(uint32_t);

    uint32_t backupBlock1 = enFlashBlockAddressTestParams_Backup;
    uint32_t backupBlock2 = enFlashBlockAddressTestParams_Backup + 1;
    uint32_t mainBlock1 = enFlashBlockAddressTestParams;
    uint32_t mainBlock2 = enFlashBlockAddressTestParams + 1;

	stcMemoryPeripherals.TestParamsBackUpWritefaile = false;

    uint32_t InitFlag1 = (uint32_t)NVM_INIT_OK_FLAG;
    uint32_t InitFlag2 = (uint32_t)NVM_INIT_OK_FLAG;

    uint8_t* writeBuffer = (uint8_t*)malloc(totalSize);

    // Safe Copy: First Chunk
    memcpy(writeBuffer, &InitFlag1, sizeof(uint32_t));
    memcpy(writeBuffer + sizeof(uint32_t), &stcTestparams[0], dataSize);
    memcpy(writeBuffer + sizeof(uint32_t) + dataSize, &InitFlag2, sizeof(uint32_t));

    // Write to Backup First
    W25qxx_WriteBlock(writeBuffer, backupBlock1, 0, totalSize);

    // Verify Before Writing to Main Memory
    uint8_t* readBuffer = (uint8_t*)malloc(totalSize);
    W25qxx_ReadBlock(readBuffer, backupBlock1, 0, totalSize);

    uint32_t readInitFlag1, readInitFlag2;
    memcpy(&readInitFlag1, readBuffer, sizeof(uint32_t));
    memcpy(&readInitFlag2, readBuffer + sizeof(uint32_t) + dataSize, sizeof(uint32_t));

    if (readInitFlag1 == InitFlag1 && readInitFlag2 == InitFlag2)
    {
        W25qxx_WriteBlock(writeBuffer, mainBlock1, 0, totalSize);
    }
    else
    {
    	stcMemoryPeripherals.TestParamsBackUpWritefaile = true;
		MemoryCorruptionBuzzer();
    }

    // Repeat for Second Chunk
    memcpy(writeBuffer, &InitFlag1, sizeof(uint32_t));
    memcpy(writeBuffer + sizeof(uint32_t), &stcTestparams[u32Chunk], dataSize);
    memcpy(writeBuffer + sizeof(uint32_t) + dataSize, &InitFlag2, sizeof(uint32_t));

    W25qxx_WriteBlock(writeBuffer, backupBlock2, 0, totalSize);
    W25qxx_ReadBlock(readBuffer, backupBlock2, 0, totalSize);

    memcpy(&readInitFlag1, readBuffer, sizeof(uint32_t));
    memcpy(&readInitFlag2, readBuffer + sizeof(uint32_t) + dataSize, sizeof(uint32_t));

    if (readInitFlag1 == InitFlag1 && readInitFlag2 == InitFlag2)
    {
        W25qxx_WriteBlock(writeBuffer, mainBlock2, 0, totalSize);
    }
    else
    {
    	stcMemoryPeripherals.TestParamsBackUpWritefaile = true;
		MemoryCorruptionBuzzer();
    }

    free(writeBuffer);
    free(readBuffer);

    return true; // Return total bytes written
}

void InitALLTestParameterBuffer(void)
{
	for(int nI = 0 ; nI < MAX_NUM_OF_TEST_PARAMS_SAVED ; ++nI)
	{
		InitTestParameterBuffer(nI);
	}
}
void InitTestParameterBuffer(uint32_t nI)
{
	stcTestparams[nI].ValidTestParamSavedFlag = INVALID_TEST_PARAM_FLAG;
	stcTestparams[nI].AspVoleume = DEFAULT_ASP_VOL;
	stcTestparams[nI].ReagentBlankEnable = false;
	stcTestparams[nI].SampleBlankEnable = false;
	stcTestparams[nI].IncubationTimeSec = DEFAULT_LAG_TIME;/*Default Read Time*/
	stcTestparams[nI].MeasurementTimeSec = DEFAULT_READ_TIME;/*Default Lag time*/
	stcTestparams[nI].PrimWavelength = en_Filter340;
	stcTestparams[nI].SecWavelength = en_FilterHome;/*Default is OFF*/
	stcTestparams[nI].TestProcedureType = en_Endpoint;
	stcTestparams[nI].AbsorbanceLimit[en_RangeLow] = float(DEFAULT_ABS_LOW);
	stcTestparams[nI].AbsorbanceLimit[en_RangeHigh] = float(DEFAULT_ABS_HIGH);
	stcTestparams[nI].RefrenceRange[en_RangeLow] = float(DEFAULT_REF_LOW);
	stcTestparams[nI].RefrenceRange[en_RangeHigh] = float(DEFAULT_REF_HIGH);
	stcTestparams[nI].ReagentBlankLimit[en_RangeLow] = float(DEFAULT_REG_BLANK_LOW);
	stcTestparams[nI].ReagentBlankLimit[en_RangeHigh] = float(DEFAULT_REG_BLANK_HIGH);
	stcTestparams[nI].LinearityRange[en_RangeLow] = float(DEFAULT_LINEARITY_LOW);
	stcTestparams[nI].LinearityRange[en_RangeHigh] = float(DEFAULT_LINEARITY_HIGH);
	stcTestparams[nI].MaxDeltaAbsPerMin = float(MAX_DELTA_PER_ABS_VAL);
	stcTestparams[nI].Temperature = float(DEFAULT_TEMPERATURE);
	stcTestparams[nI].CuvetteType = en_Flowcell;
	stcTestparams[nI].KFactor = float(DEFAULT_K_FACTOR);
	stcTestparams[nI].TurbiAbsLimit = float(DEFAULT_TURBIABSLIMIT);
	stcTestparams[nI].Calib = en_Factor;/*default is factor*/

	enCalibrationType CalibType = stcTestparams[nI].Calib;
	uint8_t MinNumOfStd = MinNumOfStandardsForSelectedMethod(CalibType);
	stcTestparams[nI].NumOfStandardsSaved = MinNumOfStd;

	snprintf(stcTestparams[nI].arrTestName , (MAX_TEST_NAME_CHAR_LENGTH), "T%03d" , (int)nI);
	strcpy(stcTestparams[nI].arrFullTestName, "");

	for(uint8_t nJ = 0 ; nJ < MAX_NUM_OF_STANDARDS ; ++nJ)
	{
		stcTestparams[nI].StandardConc[nJ] = float(0);
		stcTestparams[nI].StandardOD[nJ] = float(0);
		stcTestparams[nI].StandardODBlank[nJ] = float(0);
		stcTestparams[nI].StandardDoneFlag[nJ] = false;
		stcTestparams[nI].StandardBlankDoneFlag[nJ] = false;
	}
	stcTestparams[nI].NumOfCalTraceSaved = 0;
	stcTestparams[nI].DiwaterDoneFlag = false;
	stcTestparams[nI].ReagentDoneFlag = false;
	stcTestparams[nI].ReagentBlankDoneFlag = false;
	stcTestparams[nI].DiwaterPriADC = float(0);
	stcTestparams[nI].DiwaterSecADC = float(0);
	stcTestparams[nI].DiwaterPriGain = float(0);
	stcTestparams[nI].DiwaterSecGain = float(0);
	stcTestparams[nI].ReagentBlankAbs = float(0);
	stcTestparams[nI].ReagentAbs = float(0);

	stcTestparams[nI].FourplA = float(0);
	stcTestparams[nI].FourplB = float(0);
	stcTestparams[nI].FourplC = float(0);
	stcTestparams[nI].FourplD = float(0);
	stcTestparams[nI].Corr_A = float(1);
	stcTestparams[nI].Corr_B = float(0);

	memset(stcTestparams[nI].arrCustomUnit , 0 , sizeof(stcTestparams[nI].arrCustomUnit));
	stcTestparams[nI].bCustomUNitEnable = false;

	for(uint8_t nIdx = 0 ; nIdx < MAX_NUM_OF_QC_CONTROL_PER_TEST ; ++nIdx)
	{
		p_QcSetup.m_QcTestSetup[nI][nIdx].u8QcIdx = NVM_INIT_NOT_OK_FLAG;
		p_QcSetup.m_QcTestSetup[nI][nIdx].fHighVal = 0;
		p_QcSetup.m_QcTestSetup[nI][nIdx].fLowVal = 0;
		p_QcSetup.m_QcTestSetup[nI][nIdx].fMeanVal = 0;
		p_QcSetup.m_QcTestSetup[nI][nIdx].u8NumOfResSaved = 0;
	}
}

void ArrangeTestparamMemory(void)
{
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	QCDATA *pQcData = Get_InstaneQcResult();
	QCDATA *pData = Get_InstaneQcResult();

	uint32_t LengthOfData = sizeof(stcTestparams[0]);
	for(int nI = 0 ; nI < MAX_NUM_OF_TEST_PARAMS_SAVED ; ++nI)
	{
		if(VALID_TEST_PARAM_FLAG != stcTestparams[nI].ValidTestParamSavedFlag)/*true only if calid test parameter is saved*/
		{
			for(int nJ = nI + 1; nJ < MAX_NUM_OF_TEST_PARAMS_SAVED ; ++nJ)
			{
				if(VALID_TEST_PARAM_FLAG == stcTestparams[nJ].ValidTestParamSavedFlag)
				{
					memcpy(&stcTestparams[nI] , &stcTestparams[nJ] , LengthOfData);//copy valid buffer to invalid buffer
					stcTestparams[nJ].ValidTestParamSavedFlag = INVALID_TEST_PARAM_FLAG;

                    // Check if QC data needs to be read and written
                    bool readWriteRequired = false;
                    for (uint8_t u8SlotIndex = 0; u8SlotIndex < MAX_NUM_OF_QC_CONTROL_PER_TEST; ++u8SlotIndex)
                    {
                        if (m_QcSetup->m_QcTestSetup[nJ][u8SlotIndex].u8QcIdx != NVM_INIT_NOT_OK_FLAG)
                        {
                            readWriteRequired = true;
                            break;
                        }
                    }

                    if (readWriteRequired)
                    {
						ReadQcResData(nI, pQcData);
						ReadQcResData(nJ, pData);

						// Rearrange all 4 QCs data
						for (uint8_t u8SlotIndex = 0; u8SlotIndex < MAX_NUM_OF_QC_CONTROL_PER_TEST; ++u8SlotIndex)
						{
							m_QcSetup->m_QcTestSetup[nI][u8SlotIndex] = m_QcSetup->m_QcTestSetup[nJ][u8SlotIndex];

							// Copy QC results and datetime in a single operation
							memcpy(pQcData->Res[u8SlotIndex], pData->Res[u8SlotIndex], sizeof(pData->Res[0]));
							memcpy(pQcData->m_DateTime[u8SlotIndex], pData->m_DateTime[u8SlotIndex], sizeof(pData->m_DateTime[0]));

							// Reset the invalidated entry
							memset(&m_QcSetup->m_QcTestSetup[nJ][u8SlotIndex], 0, sizeof(m_QcSetup->m_QcTestSetup[nJ][u8SlotIndex]));
							m_QcSetup->m_QcTestSetup[nJ][u8SlotIndex].u8QcIdx = NVM_INIT_NOT_OK_FLAG;
						}
						WriteQcResData(nI, pQcData);
						WriteQcResData(nJ, pData);
                    }
					break;
				}
			}
		}
	}
}
uint8_t GetSlotForSavingTestParam(void)
{
	uint8_t Slot = 0;
	for(Slot = 0 ; Slot < MAX_NUM_OF_TEST_PARAMS_SAVED ; ++Slot)
	{
		if(VALID_TEST_PARAM_FLAG != stcTestparams[Slot].ValidTestParamSavedFlag)
		{
			break;
		}
	}
	if(Slot == MAX_NUM_OF_TEST_PARAMS_SAVED)
	{
		Slot = (INVALID_TEST_PARAM_FLAG);/*No slot available*/
	}
	return Slot;
}
uint16_t GetTestParamsTotalItemSaved(void)
{
	ReadTestParameterBuffer();
	uint16_t ValidTestParameterAvailable = 0;
	for(int nI = 0 ; nI < MAX_NUM_OF_TEST_PARAMS_SAVED ; ++nI)
	{
		if(VALID_TEST_PARAM_FLAG == stcTestparams[nI].ValidTestParamSavedFlag)/*true only if calid test parameter is saved*/
		{
			ValidTestParameterAvailable++;
		}
	}
	return ValidTestParameterAvailable;
}

Testparams* GetInstance_TestParams(void)
{
	return &stcTestparams[0];
}
