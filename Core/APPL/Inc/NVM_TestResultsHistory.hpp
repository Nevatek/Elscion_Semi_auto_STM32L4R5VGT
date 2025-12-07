/*
 * NVM_TestResultsHistory.hpp
 *
 *  Created on: Feb 12, 2024
 *      Author: Alvin
 */
#ifndef APPL_INC_NVM_TESTRESULTSHISTORY_HPP_
#define APPL_INC_NVM_TESTRESULTSHISTORY_HPP_

#include "../APPL/Inc/Testrun.h"
#include "../APPL/Inc/NVM_TestParameters.hpp"

#define MAX_RESULT_PER_WINDOW (5)
#define MAX_RESULTS_IN_EACH_BLOCK (1250)
#define TOTAL_NUM_OF_FLASH_BLOCK_USED (4)
#define TOTAL_NUM_OF_RESULTS_SAVED (TOTAL_NUM_OF_FLASH_BLOCK_USED * MAX_RESULTS_IN_EACH_BLOCK)

typedef enum
{
	enSearchDate = 0,
	enSearchMethod,
	enSearchTestname,
	enSearchPatientId,
	enSearchUser,
	enSearchMax
}en_RESULT_HIST_SEARCH;

typedef enum
{
	enDataValid = 0,
	enDataBuffFree = 1,
}enDataValidity;

typedef enum
{
	enSearchForword = 0,
	enSearchReverse,
}EnSEARCHDIRECTION;

typedef struct
{
    char arrPatientId[1 + MAX_PATIENT_ID_LENGTH];       /* 16 Bytes */
    char arrUserName[1 + MAX_USERNAME_LENGTH];          /* 8 Bytes */

    float fResult;
    float fAbs;  /* 4 Bytes */
    char arrTestName[MAX_TEST_NAME_CHAR_LENGTH + 2];    /* 8 Bytes */
    char Unit[MAX_CUSTOM_UNITS_LENGTH];                 /* 8 Bytes */

    uint16_t u16Date_day : 5;
    uint16_t u16Date_Month : 4;
    uint16_t u16Date_Year : 7;

    uint16_t m_ValidDataSaved : 1;                      /* 1 bit */
    uint16_t u16Hour : 5;                               /* 5 bits */
    uint16_t u16Minute : 6;                             /* 6 bits */
    uint16_t TestProcedure : 4;
} ResultHistory; /*Total size is 52 bytes*/

typedef union
{
	ResultHistory m_DataBase[TOTAL_NUM_OF_FLASH_BLOCK_USED][MAX_RESULTS_IN_EACH_BLOCK];
	ResultHistory arrBuff[TOTAL_NUM_OF_RESULTS_SAVED];

}ResHistoryDatabase;

typedef struct
{
	en_RESULT_HIST_SEARCH m_SearchType;
	uint16_t u16ItemsFound;
	int16_t n16SearchItemIndex[TOTAL_NUM_OF_RESULTS_SAVED];
	bool bItemSelectionStatus[MAX_RESULT_PER_WINDOW];
	RTC_DateTypeDef m_DateStart;
	RTC_DateTypeDef m_DateEnd;
	char arrBuff[1 + 15];/*4 Bytes*/
	enTestProcedureType m_TestType;/*1 Bytes*/
}SEARCH_RESHISTORY;

void SaveTestResults(void);
void ReadAllTestResultHistory(void);
void SaveTestResultHistoryBlockWise(uint32_t u32BlockNumber);
void DeleteAllestHistory(void);
void DeleteTestResultHistory(uint32_t u32IndexPos);
bool AddNewResultHistoryData(uint32_t u32TestId , TestData *pTestData , char *pUser);
void SearchResultHistDatabase(void);
void SetCurrSearchType(en_RESULT_HIST_SEARCH m_Val);
en_RESULT_HIST_SEARCH GetCurrSearchType(void);
SEARCH_RESHISTORY* GetInstance_SearchResultHistory(void);
ResHistoryDatabase* GetInstance_ResultHistoryDatabase(void);
#endif /* APPL_INC_NVM_TESTRESULTSHISTORY_HPP_ */
