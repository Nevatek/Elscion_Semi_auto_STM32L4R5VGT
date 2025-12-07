/*
 * NVM_TestResultsHistory.cpp
 *
 *  Created on: Feb 12, 2024
 *      Author: Alvin
 */
#include "../../APPL/Inc/NonVolatileMemory.h"
#include "../APPL/Inc/NVM_TestResultsHistory.hpp"
#include "../APPL/Inc/NVM_TestParameters.hpp"
#include "../Screens/Inc/CommonDisplayFunctions.h"
#include <string.h>
#include <string>
#include "Buzzer.h"
#include <HandlerPheripherals.hpp>

extern enSystemMemoryPeripherals stcMemoryPeripherals;
ResHistoryDatabase g_ResHistoryDataBase;
SEARCH_RESHISTORY g_ResultHistSearchTag;

static int32_t Get_ResultFreeIndexPos(void);
static void ResetHistorySearch(void);
static void CopyResultsToUiBuffer(SEARCH_RESHISTORY *pHistData  , ResultHistory *p_Database , uint16_t u16IndexPos);

void DeleteAllestHistory(void)
{
	for(uint8_t u32Block = 0 ; u32Block < TOTAL_NUM_OF_FLASH_BLOCK_USED ; ++u32Block)
	{
		for(int32_t u32Idx = 0 ; u32Idx < MAX_RESULTS_IN_EACH_BLOCK ; ++u32Idx)
		{
			g_ResHistoryDataBase.m_DataBase[u32Block][u32Idx].m_ValidDataSaved = enDataBuffFree;
		}
	}
	SaveTestResults();
}
void DeleteTestResultHistory(uint32_t u32IndexPos)
{
	for(uint32_t u32nI = (u32IndexPos + 1) ; u32nI < TOTAL_NUM_OF_RESULTS_SAVED; ++u32nI)
	{
		/*Rearrange all slots*/
		memcpy(&g_ResHistoryDataBase.arrBuff[u32nI - 1] ,
				&g_ResHistoryDataBase.arrBuff[u32nI] , sizeof(ResultHistory));
	}
	/*Delete last slot*/
	g_ResHistoryDataBase.arrBuff[TOTAL_NUM_OF_RESULTS_SAVED - 1].m_ValidDataSaved = enDataBuffFree;
}

//void ReadAllTestResultHistory(void)
//{
//    uint32_t u32DataSize = MAX_RESULTS_IN_EACH_BLOCK * sizeof(ResultHistory);
//    uint32_t u32TotalSize = u32DataSize + 2 * sizeof(uint32_t); // Includes InitFlags
//
//    uint8_t *readBuffer = (uint8_t*)malloc(u32TotalSize);
//
//	stcMemoryPeripherals.TestResultMemCorrpt = false;
//	stcMemoryPeripherals.TestResultBackUpMemCorrpt = false;
//
//    for (uint8_t u8BlockNum = 0, u32BlockId = enFlashBlockAddressResHistory_B0_Start,
//                 u32BackupId = enFlashBlockAddressResHistory_B0_Start_Backup;
//         u32BlockId <= enFlashBlockAddressResHistory_End;
//         ++u8BlockNum, ++u32BlockId, ++u32BackupId)
//    {
//        // Step 1: Read from Main Block
//        W25qxx_ReadBlock(readBuffer, u32BlockId, 0, u32TotalSize);
//
//        uint32_t readInitFlag1, readInitFlag2;
//        memcpy(&readInitFlag1, readBuffer, sizeof(uint32_t));
//        memcpy(&readInitFlag2, readBuffer + sizeof(uint32_t) + u32DataSize, sizeof(uint32_t));
//
//        if (readInitFlag1 == NVM_INIT_OK_FLAG && readInitFlag2 == NVM_INIT_OK_FLAG)
//        {
//            // Step 2: Valid main block, use this data
//            memcpy(&g_ResHistoryDataBase.m_DataBase[u8BlockNum][0], readBuffer + sizeof(uint32_t), u32DataSize);
//        }
//        else
//        {
//        	stcMemoryPeripherals.TestResultMemCorrpt = true;
//            // Step 3: Main block is corrupted, read from backup
//            W25qxx_ReadBlock(readBuffer, u32BackupId, 0, u32TotalSize);
//            memcpy(&readInitFlag1, readBuffer, sizeof(uint32_t));
//            memcpy(&readInitFlag2, readBuffer + sizeof(uint32_t) + u32DataSize, sizeof(uint32_t));
//
//            if (readInitFlag1 == NVM_INIT_OK_FLAG && readInitFlag2 == NVM_INIT_OK_FLAG)
//            {
//                // Backup is valid, use this data
//                memcpy(&g_ResHistoryDataBase.m_DataBase[u8BlockNum][0], readBuffer + sizeof(uint32_t), u32DataSize);
//            }
//            else
//            {
//            	stcMemoryPeripherals.TestResultBackUpMemCorrpt = true;
//        		MemoryCorruptionBuzzer();
//            }
//        }
//    }
//
//    free(readBuffer);
//}


void ReadAllTestResultHistory(void)
{
    uint32_t u32DataSize = MAX_RESULTS_IN_EACH_BLOCK * sizeof(ResultHistory);
    uint32_t u32TotalSize = u32DataSize + 2 * sizeof(uint32_t); // Includes InitFlags

    uint8_t *readBuffer = (uint8_t *)malloc(u32TotalSize);

    stcMemoryPeripherals.TestResultMemCorrpt = false;
    stcMemoryPeripherals.TestResultBackUpMemCorrpt = false;

    uint32_t mainId = enFlashBlockAddressResHistory_B0_Start;
    uint32_t backupId = enFlashBlockAddressResHistory_B0_Start_Backup;

    for (uint8_t u8BlockNum = 0; mainId <= enFlashBlockAddressResHistory_End; ++u8BlockNum, ++mainId, ++backupId)
    {
        bool mainValid = false;

        // Step 1: Try reading from main
        W25qxx_ReadBlock(readBuffer, mainId, 0, u32TotalSize);

        uint32_t readInitFlag1, readInitFlag2;
        memcpy(&readInitFlag1, readBuffer, sizeof(uint32_t));
        memcpy(&readInitFlag2, readBuffer + sizeof(uint32_t) + u32DataSize, sizeof(uint32_t));

        if (readInitFlag1 == NVM_INIT_OK_FLAG && readInitFlag2 == NVM_INIT_OK_FLAG)
        {
            // Main is valid
            memcpy(&g_ResHistoryDataBase.m_DataBase[u8BlockNum][0], readBuffer + sizeof(uint32_t), u32DataSize);
            mainValid = true;
        }
        else
        {
            stcMemoryPeripherals.TestResultMemCorrpt = true;

            // Step 2: Try reading from backup
            W25qxx_ReadBlock(readBuffer, backupId, 0, u32TotalSize);
            memcpy(&readInitFlag1, readBuffer, sizeof(uint32_t));
            memcpy(&readInitFlag2, readBuffer + sizeof(uint32_t) + u32DataSize, sizeof(uint32_t));

            if (readInitFlag1 == NVM_INIT_OK_FLAG && readInitFlag2 == NVM_INIT_OK_FLAG)
            {
                // Backup is valid, copy to RAM and restore main
                stcMemoryPeripherals.TestResultMemCorrpt = false;
                memcpy(&g_ResHistoryDataBase.m_DataBase[u8BlockNum][0], readBuffer + sizeof(uint32_t), u32DataSize);
                W25qxx_WriteBlock(readBuffer, mainId, 0, u32TotalSize);
            }
            else
            {
                // Both corrupted
                stcMemoryPeripherals.TestResultBackUpMemCorrpt = true;
                MemoryCorruptionBuzzer();
            }
        }

        // Optional: If main is valid, also validate backup and restore if backup is bad
        if (mainValid)
        {
            uint8_t *pBackupCheckBuffer = (uint8_t *)malloc(u32TotalSize);
            W25qxx_ReadBlock(pBackupCheckBuffer, backupId, 0, u32TotalSize);

            memcpy(&readInitFlag1, pBackupCheckBuffer, sizeof(uint32_t));
            memcpy(&readInitFlag2, pBackupCheckBuffer + sizeof(uint32_t) + u32DataSize, sizeof(uint32_t));

            if (!(readInitFlag1 == NVM_INIT_OK_FLAG && readInitFlag2 == NVM_INIT_OK_FLAG))
            {
                // Restore from main to backup
                W25qxx_WriteBlock(readBuffer, backupId, 0, u32TotalSize);
            }

            free(pBackupCheckBuffer);
        }
    }

    free(readBuffer);
}


void SaveTestResultHistoryBlockWise(uint32_t u32BlockNumber)
{
    uint32_t u32MainBlock = u32BlockNumber + enFlashBlockAddressResHistory_B0_Start;
    uint32_t u32BackupBlock = u32BlockNumber + enFlashBlockAddressResHistory_B0_Start_Backup;

    uint32_t u32DataSize = MAX_RESULTS_IN_EACH_BLOCK * sizeof(ResultHistory);
    uint32_t u32TotalSize = u32DataSize + 2 * sizeof(uint32_t);  // Include InitFlags

    uint8_t *writeBuffer = (uint8_t*)malloc(u32TotalSize);
    uint8_t *verifyBuffer = (uint8_t*)malloc(u32TotalSize);

	stcMemoryPeripherals.TestResultBackUpWritefailed = false;

    // Step 1: Prepare Write Data
    uint32_t initFlag = (uint32_t)NVM_INIT_OK_FLAG;
    memcpy(writeBuffer, &initFlag, sizeof(uint32_t));  // Start flag
    memcpy(writeBuffer + sizeof(uint32_t), &g_ResHistoryDataBase.m_DataBase[u32BlockNumber][0], u32DataSize); // Data
    memcpy(writeBuffer + sizeof(uint32_t) + u32DataSize, &initFlag, sizeof(uint32_t));  // End flag

    // Step 2: Write to Backup Block First
    W25qxx_WriteBlock(writeBuffer, u32BackupBlock, 0, u32TotalSize);

    // Step 3: Verify Backup
    W25qxx_ReadBlock(verifyBuffer, u32BackupBlock, 0, u32TotalSize);

    uint32_t verifyInitFlag1, verifyInitFlag2;
    memcpy(&verifyInitFlag1, verifyBuffer, sizeof(uint32_t));
    memcpy(&verifyInitFlag2, verifyBuffer + sizeof(uint32_t) + u32DataSize, sizeof(uint32_t));

    if (verifyInitFlag1 == (uint32_t)NVM_INIT_OK_FLAG && verifyInitFlag2 == (uint32_t)NVM_INIT_OK_FLAG)
    {
        // Step 4: Write to Main Block Only If Backup is Valid
        W25qxx_WriteBlock(writeBuffer, u32MainBlock, 0, u32TotalSize);
    }
    else
    {
    	stcMemoryPeripherals.TestResultBackUpWritefailed = true;
		MemoryCorruptionBuzzer();
    }

    free(writeBuffer);
    free(verifyBuffer);

}


void SaveTestResults(void)
{
	for(uint8_t u32Block = 0 ; u32Block < TOTAL_NUM_OF_FLASH_BLOCK_USED ; ++u32Block)
	{
		SaveTestResultHistoryBlockWise(u32Block);
	}
}

int32_t Get_ResultFreeIndexPos(void)
{
	int32_t n32Index = (-1);/*(-1) if no index position is available as free*/
	for(uint8_t u32Block = 0 ; u32Block < TOTAL_NUM_OF_FLASH_BLOCK_USED ; ++u32Block)
	{
		for(int32_t u32Idx = 0 ; u32Idx < MAX_RESULTS_IN_EACH_BLOCK ; ++u32Idx)
		{
			/*Check wather any slot is free*/
			if(enDataBuffFree == g_ResHistoryDataBase.m_DataBase[u32Block][u32Idx].m_ValidDataSaved)
			{
				/*Calculate slot index*/
				n32Index = u32Idx + (u32Block * MAX_RESULTS_IN_EACH_BLOCK);
				return n32Index;
			}
		}
	}

	/*If no slot if free - delete first slot and re arrange - insert new data to last slot*/
	if((-1) == n32Index)
	{
		for(uint32_t u32nI = 1U/*Start from 2nd slot*/ ; u32nI < TOTAL_NUM_OF_RESULTS_SAVED; ++u32nI)
		{
			/*Rearrange all slots*/
			memcpy(&g_ResHistoryDataBase.arrBuff[u32nI - 1] ,
					&g_ResHistoryDataBase.arrBuff[u32nI] , sizeof(ResultHistory));
		}

		/*Delete last slot*/
		g_ResHistoryDataBase.arrBuff[TOTAL_NUM_OF_RESULTS_SAVED - 1].m_ValidDataSaved = enDataBuffFree;

		/*Next data saving index will be last slot*/
		n32Index = (int32_t)(TOTAL_NUM_OF_RESULTS_SAVED - 1);
	}
	return n32Index;
}
bool AddNewResultHistoryData(uint32_t u32TestId , TestData *pTestData , char *pUser)
{
	bool bStatus = false;
	int32_t n32FreeIdxPos = (-1);
	uint16_t u16Block = 0;
	uint16_t u16TargetIndexPos = 0;
	RTC_DateTypeDef m_CurrDate;
	RTC_TimeTypeDef m_CurrTime;
	char arr_CBuffer[32] = {0};

	n32FreeIdxPos = Get_ResultFreeIndexPos();
	if((-1) != n32FreeIdxPos)/*If we got some valid positions*/
	{
		u16Block = uint32_t(n32FreeIdxPos / MAX_RESULTS_IN_EACH_BLOCK);
		u16TargetIndexPos = n32FreeIdxPos - (u16Block * MAX_RESULTS_IN_EACH_BLOCK);
		strncpy(g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].arrPatientId , pTestData->arrPatientIDBuffer , MAX_PATIENT_ID_LENGTH);
		strncpy(g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].arrUserName , pUser , MAX_USERNAME_LENGTH);
		g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].fResult = pTestData->Result;

//		if(GetInstance_TestParams()[stcTestData.TestId].ReagentBlankDoneFlag)
//		{
//			g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].fAbs = pTestData->SampleOpticalDensity - GetInstance_TestParams()[stcTestData.TestId].ReagentBlankAbs;
//		}
//		else
//		{
//			g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].fAbs = pTestData->SampleOpticalDensity;
//		}

		g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].fAbs = pTestData->SampleOpticalDensity;

		strncpy(g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].arrTestName , GetInstance_TestParams()[stcTestData.TestId].arrTestName , MAX_TEST_NAME_CHAR_LENGTH);
		if(true == GetInstance_TestParams()[stcTestData.TestId].bCustomUNitEnable)
		{
			snprintf(arr_CBuffer , MAX_CUSTOM_UNITS_LENGTH , "%s" ,GetInstance_TestParams()[stcTestData.TestId].arrCustomUnit);
		}
		else
		{
			enUnits ResultUnit = GetInstance_TestParams()[stcTestData.TestId].Unit;
			snprintf(arr_CBuffer , 31 , "%s" ,&arr_cUnitBuffer[ResultUnit][0]);
		}
		strncpy(g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].Unit , arr_CBuffer , MAX_CUSTOM_UNITS_LENGTH);

		GetCurrentDate(&m_CurrDate);
		GetCurrentTime(&m_CurrTime);
		g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].u16Date_day = m_CurrDate.Date;
		g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].u16Date_Month = m_CurrDate.Month;
		g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].u16Date_Year = m_CurrDate.Year;
		g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].m_ValidDataSaved = enDataValid;
		g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].u16Hour = m_CurrTime.Hours;
		g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].u16Minute = m_CurrTime.Minutes;
		//g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].u16Second = m_CurrTime.Seconds;
		g_ResHistoryDataBase.m_DataBase[u16Block][u16TargetIndexPos].TestProcedure = GetInstance_TestParams()[stcTestData.TestId].TestProcedureType;

		SaveTestResultHistoryBlockWise(u16Block);/*Save results to flash*/

		bStatus = true;
	}
	return bStatus;
}

void ResetHistorySearch(void)
{
	for(uint16_t u16Idx = 0 ; u16Idx < TOTAL_NUM_OF_RESULTS_SAVED ; ++u16Idx)
	{
		g_ResultHistSearchTag.n16SearchItemIndex[u16Idx] = (-1);
	}
	g_ResultHistSearchTag.u16ItemsFound = 0;
}

void SetCurrSearchType(en_RESULT_HIST_SEARCH m_Val)
{
	g_ResultHistSearchTag.m_SearchType = m_Val;
}
en_RESULT_HIST_SEARCH GetCurrSearchType(void)
{
	return g_ResultHistSearchTag.m_SearchType;
}
SEARCH_RESHISTORY* GetInstance_SearchResultHistory(void)
{
	return &g_ResultHistSearchTag;
}

ResHistoryDatabase* GetInstance_ResultHistoryDatabase(void)
{
	return &g_ResHistoryDataBase;
}
void SearchResultHistDatabase(void)
{
	SEARCH_RESHISTORY *pHistData = GetInstance_SearchResultHistory();
	en_RESULT_HIST_SEARCH enSearchType = GetCurrSearchType();
	ResetHistorySearch();
	char arrTmp[64] = {0};
	uint32_t u32TargetDateStart = 0;
	uint32_t u32TargetDateEnd = 0;
	uint32_t u32ReadDate = 0;
	for(int16_t u16Idx = (TOTAL_NUM_OF_RESULTS_SAVED - 1) ; u16Idx >= 0 ; u16Idx--)
	{
		ResultHistory *p_Database = &GetInstance_ResultHistoryDatabase()->arrBuff[u16Idx];
		if(enDataValid == p_Database->m_ValidDataSaved)
		{
			/*Date search - Start*/
			if(enSearchDate == enSearchType)
			{
				snprintf(arrTmp , sizeof(arrTmp) , "%02d%02d%02d" , pHistData->m_DateStart.Year ,
						pHistData->m_DateStart.Month,
						pHistData->m_DateStart.Date);
				sscanf(arrTmp , "%u" , (unsigned int*)&u32TargetDateStart);/*Target start date*/

				snprintf(arrTmp , sizeof(arrTmp) , "%02d%02d%02d" , pHistData->m_DateEnd.Year ,
						pHistData->m_DateEnd.Month,
						pHistData->m_DateEnd.Date);
				sscanf(arrTmp , "%u" , (unsigned int*)&u32TargetDateEnd);/*Target end date*/

				snprintf(arrTmp , sizeof(arrTmp) , "%02d%02d%02d" , p_Database->u16Date_Year ,
						p_Database->u16Date_Month ,
						p_Database->u16Date_day);
				sscanf(arrTmp , "%u" , (unsigned int*)&u32ReadDate);/*Read date*/


				if((u32ReadDate == u32TargetDateStart) ||
						(u32ReadDate == u32TargetDateEnd) ||
						(u32ReadDate > u32TargetDateStart && u32ReadDate < u32TargetDateEnd))
				{
					CopyResultsToUiBuffer(pHistData , p_Database , u16Idx);/*Copy item to result history list*/
				}
//				if((p_Database->u16Date_Year == pHistData->m_DateStart.Year) ||
//						(p_Database->u16Date_Year >= pHistData->m_DateStart.Year &&
//						p_Database->u16Date_Year <= pHistData->m_DateEnd.Year))
//				{
//					if((p_Database->u16Date_Month == pHistData->m_DateStart.Month) ||
//							(p_Database->u16Date_Month >= pHistData->m_DateStart.Month &&
//							p_Database->u16Date_Month <= pHistData->m_DateEnd.Month))
//					{
//						if((p_Database->u16Date_day == pHistData->m_DateStart.Date) ||
//								(p_Database->u16Date_day >= pHistData->m_DateStart.Date &&
//								p_Database->u16Date_day <= pHistData->m_DateEnd.Date))
//						{
//							CopyResultsToUiBuffer(pHistData , p_Database , u16Idx);
//						}
//					}
//				}
			}
			/*Date search - end*/

			/*Test type search - Start*/
			else if(enSearchMethod == enSearchType)
			{
				if((enTestProcedureType)p_Database->TestProcedure == pHistData->m_TestType)
				{
					CopyResultsToUiBuffer(pHistData , p_Database , u16Idx);
				}
			}
			/*Test type search - end*/

			/*Test name search - Start*/
			else if(enSearchTestname== enSearchType)
			{
				if(0 == strncmp(p_Database->arrTestName , pHistData->arrBuff , sizeof(pHistData->arrBuff)))
				{
					CopyResultsToUiBuffer(pHistData , p_Database , u16Idx);
				}
			}
			/*Test name search - end*/

			/*User name search - Start*/
			else if(enSearchUser == enSearchType)
			{
				if(0 == strncmp(p_Database->arrUserName , pHistData->arrBuff , sizeof(p_Database->arrUserName)))
				{
					CopyResultsToUiBuffer(pHistData , p_Database , u16Idx);
				}
			}
			/*User name search - end*/

			/*User name search - Start*/
			else if(enSearchPatientId == enSearchType)
			{
				if(0 == strncmp(p_Database->arrPatientId , pHistData->arrBuff , sizeof(p_Database->arrPatientId)))
				{
					CopyResultsToUiBuffer(pHistData , p_Database , u16Idx);
				}
			}
			/*User name search - end*/
		}
	}
}

void CopyResultsToUiBuffer(SEARCH_RESHISTORY *pHistData  , ResultHistory *p_Database , uint16_t u16IndexPos)
{
	pHistData->n16SearchItemIndex[pHistData->u16ItemsFound] = u16IndexPos;
	pHistData->u16ItemsFound++;
	if(TOTAL_NUM_OF_RESULTS_SAVED <= pHistData->u16ItemsFound)
	{
		pHistData->u16ItemsFound = TOTAL_NUM_OF_RESULTS_SAVED;
	}
}

