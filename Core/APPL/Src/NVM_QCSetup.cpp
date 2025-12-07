/*
 * NVM_QCSetup.cpp
 *
 *  Created on: Apr 4, 2024
 *      Author: Alvin
 */
#include "../../APPL/Inc/NonVolatileMemory.h"
#include "../APPL/Inc/Testrun.h"
#include "../APPL/Inc/NVM_TestParameters.hpp"
#include "../APPL/Inc/NVM_QCSetup.hpp"
#include <string.h>
#include <string>
#include "Buzzer.h"
#include <HandlerPheripherals.hpp>

extern enSystemMemoryPeripherals stcMemoryPeripherals;
QCSETUP g_QcSetup;
QCDATA  g_QcResultData;

void InitQcSetup(void)
{
	memset(&g_QcSetup , 0 , sizeof(QCSETUP));
	g_QcSetup.InitFlag1 = (uint32_t)NVM_INIT_OK_FLAG;
	g_QcSetup.InitFlag2 = (uint32_t)NVM_INIT_OK_FLAG;
}

void NVM_ReadQcSetup(void)
{
    uint32_t blockAddressMain = enFlashBlockQcSetup;  // Main block address
    uint32_t blockAddressBackup = enFlashBlockQcSetup_Backup;  // Backup block address
	uint32_t LengthOfData = sizeof(g_QcSetup);

	stcMemoryPeripherals.QCSetupMemCorrpt = false;
	stcMemoryPeripherals.QCSetupBackUpMemCorrpt = false;

	memset(&g_QcSetup , 0 , LengthOfData);
    // First, attempt to read from the main sector
    W25qxx_ReadBlock((uint8_t*)&g_QcSetup, blockAddressMain, 0, LengthOfData);
    bool mainValid = VerifybackupQCSETUPIntegrity(&g_QcSetup);

    // Check integrity of data from the main sector
    if (!mainValid)
    {
    	stcMemoryPeripherals.QCSetupMemCorrpt = true;
        // If main sector is corrupted, attempt to read from the backup sector
        MemoryCorruptionBuzzer();
        W25qxx_ReadBlock((uint8_t*)&g_QcSetup, blockAddressBackup, 0, LengthOfData);

        // Check integrity of the backup data
        if (VerifybackupQCSETUPIntegrity(&g_QcSetup))
        {
        	stcMemoryPeripherals.QCSetupMemCorrpt = false;
        	 W25qxx_WriteBlock((uint8_t*)&g_QcSetup, blockAddressMain, 0, LengthOfData);
        }
        else
        {
        	stcMemoryPeripherals.QCSetupBackUpMemCorrpt = true;
    		MemoryCorruptionBuzzer();
        }
    }
    else
    {
        // Main is valid, now check backup
        uint8_t* readBuffer = (uint8_t*)malloc(LengthOfData);

		W25qxx_ReadBlock(readBuffer, blockAddressBackup, 0, LengthOfData);

		if (!VerifybackupQCSETUPIntegrity((QCSETUP*)readBuffer))
		{
			// Backup is corrupted -> restore from main
			W25qxx_WriteBlock((uint8_t*)&g_QcSetup, blockAddressBackup, 0, LengthOfData);
		}
		free(readBuffer);
    }
}

void NVM_WriteQcSetup(void)
{
	uint32_t LengthOfData = sizeof(g_QcSetup);
	uint32_t blockAddressMain = enFlashBlockQcSetup;
    uint32_t blockAddressBackup = enFlashBlockQcSetup_Backup;  // Backup block address

	stcMemoryPeripherals.QCSetupBackUpWritefailed = false;

    // Write to backup sector first
    W25qxx_WriteBlock((uint8_t*)&g_QcSetup, blockAddressBackup, 0, LengthOfData);

    // Verify backup data integrity
    W25qxx_ReadBlock((uint8_t*)&g_QcSetup, blockAddressBackup, 0, LengthOfData);

    // Check if the backup data is valid
    if (VerifybackupQCSETUPIntegrity(&g_QcSetup))
    {
        // If backup is valid, write to the main sector
        W25qxx_WriteBlock((uint8_t*)&g_QcSetup, blockAddressMain, 0, LengthOfData);
    }
    else
    {
    	stcMemoryPeripherals.QCSetupBackUpWritefailed = true;
		MemoryCorruptionBuzzer();
    }
}

bool VerifybackupQCSETUPIntegrity(QCSETUP* g_QcSetup)
{
    if (g_QcSetup->InitFlag1 != (uint32_t)NVM_INIT_OK_FLAG || g_QcSetup->InitFlag2 != (uint32_t)NVM_INIT_OK_FLAG)
    {
        return false; // Data is corrupted
    }
    return true; // Data is valid
}

void Delete_QcSetup(void)
{
	for(uint8_t nI = enQcCntrl_0 ; nI < enQcCntrl_Max ; ++nI)
	{
		g_QcSetup.m_QcControls[nI].u8ValidFlag = NVM_INIT_NOT_OK_FLAG;
	}
	for(uint8_t nI = 0 ; nI < MAX_NUM_OF_TEST_PARAMS_SAVED ; ++nI)
	{
		for(uint8_t nIdx = 0 ; nIdx < MAX_NUM_OF_QC_CONTROL_PER_TEST ; ++nIdx)
		{
			g_QcSetup.m_QcTestSetup[nI][nIdx].u8QcIdx = NVM_INIT_NOT_OK_FLAG;
			g_QcSetup.m_QcTestSetup[nI][nIdx].fHighVal = 0;
			g_QcSetup.m_QcTestSetup[nI][nIdx].fLowVal = 0;
			g_QcSetup.m_QcTestSetup[nI][nIdx].fMeanVal = 0;
			g_QcSetup.m_QcTestSetup[nI][nIdx].u8NumOfResSaved = 0;
		}
	}
}

QCSETUP* Get_InstaneQcSetup(void)
{
	return (&g_QcSetup);
}

QCDATA* Get_InstaneQcResult(void)
{
	return (&g_QcResultData);
}

void AppendQcResData(uint8_t u8TestId , uint8_t u8QcTestIdx ,float fRes)
{
	QCDATA *pData = Get_InstaneQcResult();
	QCSETUP *m_QcSetup = Get_InstaneQcSetup();
	uint8_t u8QcDataAvail = 0;
	uint8_t u8SaveLocIdx = 0;
	uint8_t u8DataIdx = 0;
	/*Read exisitng data to buffer*/
	ReadQcResData(u8TestId , pData);

	u8QcDataAvail = m_QcSetup->m_QcTestSetup[u8TestId][u8QcTestIdx].u8NumOfResSaved;
	if(MAX_NUM_OF_QC_PER_TEST <= u8QcDataAvail)
	{
		/*Re arrange all qc result datas FIFO*/
		for(u8DataIdx = 0; u8DataIdx < MAX_NUM_OF_QC_PER_TEST ; ++u8DataIdx)
		{
			pData->Res[u8QcTestIdx][u8DataIdx] =
					pData->Res[u8QcTestIdx][u8DataIdx + 1];

			memcpy(&pData->m_DateTime[u8QcTestIdx][u8DataIdx] ,
					&pData->m_DateTime[u8QcTestIdx][u8DataIdx + 1] , sizeof(DATETIME));
		}
		u8SaveLocIdx = MAX_NUM_OF_QC_PER_TEST - 1;
	}
	else
	{
		u8SaveLocIdx = u8QcDataAvail;
	}

	/*Append datas*/
	RTC_DateTypeDef m_Date;
	RTC_TimeTypeDef m_Time;

	GetCurrentDate(&m_Date);
	GetCurrentTime(&m_Time);
	pData->Res[u8QcTestIdx][u8SaveLocIdx] = fRes;
	pData->m_DateTime[u8QcTestIdx][u8SaveLocIdx].u16Date_day = m_Date.Date;
	pData->m_DateTime[u8QcTestIdx][u8SaveLocIdx].u16Date_Month = m_Date.Month;
	pData->m_DateTime[u8QcTestIdx][u8SaveLocIdx].u16Date_Year = m_Date.Year;

	pData->m_DateTime[u8QcTestIdx][u8SaveLocIdx].u16Date_Hour = m_Time.Hours;
	pData->m_DateTime[u8QcTestIdx][u8SaveLocIdx].u16Date_min = m_Time.Minutes;

	m_QcSetup->m_QcTestSetup[u8TestId][u8QcTestIdx].u8NumOfResSaved++;
	if((MAX_NUM_OF_QC_PER_TEST) <= m_QcSetup->m_QcTestSetup[u8TestId][u8QcTestIdx].u8NumOfResSaved)
	{
		m_QcSetup->m_QcTestSetup[u8TestId][u8QcTestIdx].u8NumOfResSaved = MAX_NUM_OF_QC_PER_TEST;
	}

	/*Save data*/
	WriteQcResData(u8TestId , pData);
	NVM_WriteQcSetup();
}

void WriteQcResData(uint8_t u8TestId, QCDATA *pQcData)
{
    uint32_t u32BlockSize = W25qxx_GetBlockSize();  /* 65536 bytes (64KB) */
    uint32_t u32SizeOfStructure = sizeof(QCDATA);  /* 1032 bytes */
    uint32_t u32MaxQcDataPerBlock = u32BlockSize / u32SizeOfStructure; /* 63 per block */
    uint32_t u32TargetBlock = enFlashBlockAddsQcData_Start + (u8TestId / u32MaxQcDataPerBlock);
    uint32_t u32BackupBlock = enFlashBlockAddsQcData_Start_Backup + (u8TestId / u32MaxQcDataPerBlock);

	stcMemoryPeripherals.QCResultBackUpWritefailed = false;

    /* Allocate memory for sector buffer */
    uint8_t *pSectorBuffer = (uint8_t*)malloc(u32BlockSize);
    uint8_t *pVerifyBuffer = (uint8_t*)malloc(u32BlockSize);

    /* Set the initialization flags */
    uint32_t initFlag = NVM_INIT_OK_FLAG;
    pQcData->InitFlag1 = initFlag;
    pQcData->InitFlag2 = initFlag;

    /* Read the entire sector from backup */
    W25qxx_ReadBlock(pSectorBuffer, u32BackupBlock, 0, u32BlockSize);

    /* Modify data in the backup sector */
    uint32_t u32AddressOffset = u32SizeOfStructure * (u8TestId % u32MaxQcDataPerBlock);
    memcpy(&pSectorBuffer[u32AddressOffset], pQcData, sizeof(QCDATA));

    /* Write to backup sector first */
    W25qxx_WriteBlock(pSectorBuffer, u32BackupBlock, 0, u32BlockSize);

    /* Read back and verify */
    W25qxx_ReadBlock(pVerifyBuffer, u32BackupBlock, 0, u32BlockSize);

    QCDATA *pBackupData = (QCDATA*)pVerifyBuffer;

    if (pBackupData->InitFlag1 == NVM_INIT_OK_FLAG && pBackupData->InitFlag2 == NVM_INIT_OK_FLAG)
    {
        /* Backup verified, now write to main memory */
        W25qxx_WriteBlock(pSectorBuffer, u32TargetBlock, 0, u32BlockSize);
    }
    else
    {
    	stcMemoryPeripherals.QCResultBackUpWritefailed = true;
		MemoryCorruptionBuzzer();
    }

    /* Free allocated memory */
    free(pSectorBuffer);
    free(pVerifyBuffer);
}

void ReadQcResData(uint8_t u8TestId, QCDATA *pQcData)
{
    uint32_t u32BlockSize = W25qxx_GetBlockSize();
    uint32_t u32SizeOfStructure = sizeof(QCDATA);
    uint32_t u32MaxQcDataPerBlock = u32BlockSize / u32SizeOfStructure;

    uint32_t u32TargetBlock = enFlashBlockAddsQcData_Start + (u8TestId / u32MaxQcDataPerBlock);
    uint32_t u32BackupBlock = enFlashBlockAddsQcData_Start_Backup + (u8TestId / u32MaxQcDataPerBlock);
    uint32_t u32AddressOffset = u32SizeOfStructure * (u8TestId % u32MaxQcDataPerBlock);

    stcMemoryPeripherals.QCResultMemCorrpt = false;
    stcMemoryPeripherals.QCResultBackUpMemCorrpt = false;

    uint8_t *pSectorBuffer = (uint8_t *)malloc(u32BlockSize);

    // Read main block
    W25qxx_ReadBlock(pSectorBuffer, u32TargetBlock, 0, u32BlockSize);
    memcpy(pQcData, &pSectorBuffer[u32AddressOffset], sizeof(QCDATA));

    if (pQcData->InitFlag1 != NVM_INIT_OK_FLAG || pQcData->InitFlag2 != NVM_INIT_OK_FLAG)
    {
        stcMemoryPeripherals.QCResultMemCorrpt = true;

        // Try reading backup block
        W25qxx_ReadBlock(pSectorBuffer, u32BackupBlock, 0, u32BlockSize);
        memcpy(pQcData, &pSectorBuffer[u32AddressOffset], sizeof(QCDATA));

        if (pQcData->InitFlag1 == NVM_INIT_OK_FLAG && pQcData->InitFlag2 == NVM_INIT_OK_FLAG)
        {
            // Restore backup to main
            W25qxx_WriteBlock(pSectorBuffer, u32TargetBlock, 0, u32BlockSize);
            stcMemoryPeripherals.QCResultMemCorrpt = false;
        }
        else
        {
            stcMemoryPeripherals.QCResultBackUpMemCorrpt = true;
            MemoryCorruptionBuzzer();
        }
    }
    else
    {
        // Main valid, now check backup integrity
        uint8_t *pBackupBuffer = (uint8_t *)malloc(u32BlockSize);

		W25qxx_ReadBlock(pBackupBuffer, u32BackupBlock, 0, u32BlockSize);
		QCDATA *pBackupQc = (QCDATA *)&pBackupBuffer[u32AddressOffset];

		if (pBackupQc->InitFlag1 != NVM_INIT_OK_FLAG || pBackupQc->InitFlag2 != NVM_INIT_OK_FLAG)
		{
			// Restore main data to backup
			W25qxx_WriteBlock(pSectorBuffer, u32BackupBlock, 0, u32BlockSize);
		}
		free(pBackupBuffer);
    }
    free(pSectorBuffer);
}

void InitializeQcResData(uint8_t u8TestId, QCDATA *pQcData)
{
    uint32_t u32BlockSize = W25qxx_GetBlockSize();
    uint32_t u32SizeOfStructure = sizeof(QCDATA);
    uint32_t u32MaxQcDataPerBlock = u32BlockSize / u32SizeOfStructure;

    uint32_t u32TargetBlock = enFlashBlockAddsQcData_Start + (u8TestId / u32MaxQcDataPerBlock);
//    uint32_t u32BackupBlock = enFlashBlockAddsQcData_Start_Backup + (u8TestId / u32MaxQcDataPerBlock);
    uint32_t u32AddressOffset = u32SizeOfStructure * (u8TestId % u32MaxQcDataPerBlock);

    uint8_t *pSectorBuffer = (uint8_t *)malloc(u32BlockSize);

    // Read main block
    W25qxx_ReadBlock(pSectorBuffer, u32TargetBlock, 0, u32BlockSize);
    memcpy(pQcData, &pSectorBuffer[u32AddressOffset], sizeof(QCDATA));

//    if (pQcData->InitFlag1 != NVM_INIT_OK_FLAG || pQcData->InitFlag2 != NVM_INIT_OK_FLAG)
//    {
//        stcMemoryPeripherals.QCResultMemCorrpt = true;
//
//        // Try reading backup block
//        W25qxx_ReadBlock(pSectorBuffer, u32BackupBlock, 0, u32BlockSize);
//        memcpy(pQcData, &pSectorBuffer[u32AddressOffset], sizeof(QCDATA));
//
//        if (pQcData->InitFlag1 == NVM_INIT_OK_FLAG && pQcData->InitFlag2 == NVM_INIT_OK_FLAG)
//        {
//            // Restore backup to main
//            W25qxx_WriteBlock(pSectorBuffer, u32TargetBlock, 0, u32BlockSize);
//            stcMemoryPeripherals.QCResultMemCorrpt = false;
//        }
//        else
//        {
//            stcMemoryPeripherals.QCResultBackUpMemCorrpt = true;
//            MemoryCorruptionBuzzer();
//        }
//    }
//    else
//    {
//        // Main valid, now check backup integrity
//        uint8_t *pBackupBuffer = (uint8_t *)malloc(u32BlockSize);
//
//		W25qxx_ReadBlock(pBackupBuffer, u32BackupBlock, 0, u32BlockSize);
//		QCDATA *pBackupQc = (QCDATA *)&pBackupBuffer[u32AddressOffset];
//
//		if (pBackupQc->InitFlag1 != NVM_INIT_OK_FLAG || pBackupQc->InitFlag2 != NVM_INIT_OK_FLAG)
//		{
//			// Restore main data to backup
//			W25qxx_WriteBlock(pSectorBuffer, u32BackupBlock, 0, u32BlockSize);
//		}
//		free(pBackupBuffer);
//    }
    free(pSectorBuffer);
}

void InitQcData(uint8_t u8TestId)
{
	QCDATA *pData = Get_InstaneQcResult();
	InitializeQcResData(u8TestId, pData);
	g_QcResultData.InitFlag1 = (uint32_t)NVM_INIT_OK_FLAG;
	g_QcResultData.InitFlag2 = (uint32_t)NVM_INIT_OK_FLAG;
	WriteQcResData(u8TestId , pData);
}

//void ReadQcResData(uint8_t u8TestId, QCDATA *pQcData)
//{
//    uint32_t u32BlockSize = W25qxx_GetBlockSize();
//    uint32_t u32SizeOfStructure = sizeof(QCDATA);
//    uint32_t u32MaxQcDataPerBlock = u32BlockSize / u32SizeOfStructure;
//    uint32_t u32TargetBlock = enFlashBlockAddsQcData_Start + (u8TestId / u32MaxQcDataPerBlock);
//    uint32_t u32BackupBlock = enFlashBlockAddsQcData_Start_Backup + (u8TestId / u32MaxQcDataPerBlock);
//
//	stcMemoryPeripherals.QCResultMemCorrpt = false;
//	stcMemoryPeripherals.QCResultBackUpMemCorrpt = false;
//
//    /* Allocate memory */
//    uint8_t *pSectorBuffer = (uint8_t*)malloc(u32BlockSize);
//
//    /* Read sector from main memory */
//    W25qxx_ReadBlock(pSectorBuffer, u32TargetBlock, 0, u32BlockSize);
//
//    /* Extract data */
//    uint32_t u32AddressOffset = u32SizeOfStructure * (u8TestId % u32MaxQcDataPerBlock);
//    memcpy(pQcData, &pSectorBuffer[u32AddressOffset], sizeof(QCDATA));
//
//    /* Verify Init Flags */
//    if (pQcData->InitFlag1 != NVM_INIT_OK_FLAG || pQcData->InitFlag2 != NVM_INIT_OK_FLAG)
//    {
//    	stcMemoryPeripherals.QCResultMemCorrpt = true;
//        /* Read from backup */
//        W25qxx_ReadBlock(pSectorBuffer, u32BackupBlock, 0, u32BlockSize);
//        memcpy(pQcData, &pSectorBuffer[u32AddressOffset], sizeof(QCDATA));
//
//        /* If backup is valid, restore it to main memory */
//        if (pQcData->InitFlag1 == NVM_INIT_OK_FLAG && pQcData->InitFlag2 == NVM_INIT_OK_FLAG)
//        {
//            W25qxx_WriteBlock(pSectorBuffer, u32TargetBlock, 0, u32BlockSize);
//        }
//        else
//        {
//        	stcMemoryPeripherals.QCResultBackUpMemCorrpt = true;
//    		MemoryCorruptionBuzzer();
//        }
//    }
//    free(pSectorBuffer);
//}


void ResetQcMemory(void)
{

}


