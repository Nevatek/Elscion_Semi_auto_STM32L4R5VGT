/*
 * NVM_QCSetup.hpp
 *
 *  Created on: Apr 4, 2024
 *      Author: Alvin
 */

#ifndef APPL_INC_NVM_QCSETUP_HPP_
#define APPL_INC_NVM_QCSETUP_HPP_

#define MAX_QC_LOT_NUM_LEN (8)
#define MAX_QC_CONTROL_NAME_LEN (7)
#define MAX_NUM_OF_QC_CONTROL_PER_TEST (4)
#define MAX_NUM_OF_QC_PER_TEST (30)
#define MAX_NUM_OF_QC_RES_BLOCK (63)

typedef enum
{
	enQcCntrl_0 = 0,
	enQcCntrl_1,
	enQcCntrl_2,
	enQcCntrl_3,
	enQcCntrl_4,
	enQcCntrl_5,
	enQcCntrl_6,
	enQcCntrl_7,
	enQcCntrl_8,
	enQcCntrl_9,
	enQcCntrl_10,
	enQcCntrl_11,
	enQcCntrl_12,
	enQcCntrl_13,
	enQcCntrl_14,
	enQcCntrl_Max
}QcCntrlIndex;

typedef struct
{
	uint16_t u8ValidFlag : 8; 			// 1 byte
	uint16_t reserved : 8;            // 1 bytes
	char arRLotNum[2 + MAX_QC_LOT_NUM_LEN]; // 10 bytes
	char arrControlName[1 + MAX_QC_CONTROL_NAME_LEN]; // 8 bytes
	RTC_DateTypeDef m_ExpiryDate;	// 4 bytes

} QCCONTROL;

typedef struct
{
	float fHighVal;
	float fMeanVal;
	float fLowVal;
	uint16_t u8NumOfResSaved;
	uint16_t u8QcIdx;  // Total 16Bytes
}QCTEST;

typedef struct
{
	uint32_t u16Date_day : 5;
	uint32_t u16Date_Month : 5;
	uint32_t u16Date_Year : 7;
	uint32_t u16Date_Hour : 7;
	uint32_t u16Date_min : 8;  // 4 bytes
}DATETIME;

typedef struct
{
	uint32_t InitFlag1; // 4 bytes
	QCCONTROL m_QcControls[enQcCntrl_Max]; 	// 15 * 24 = 360 Bytes
	QCTEST m_QcTestSetup[MAX_NUM_OF_TEST_PARAMS_SAVED][MAX_NUM_OF_QC_CONTROL_PER_TEST]; // 200 * 4 * 16 bytes = 12800
	uint32_t InitFlag2; // 4 bytes

}QCSETUP;

//typedef struct/*Size : 1024 bytes per test -> 64 Test QC data per block can be saved*/
//{
//	float Res[MAX_NUM_OF_QC_CONTROL_PER_TEST][MAX_NUM_OF_QC_PER_TEST + 2]; // 512 bytes
//	DATETIME m_DateTime[MAX_NUM_OF_QC_CONTROL_PER_TEST][MAX_NUM_OF_QC_PER_TEST + 2]; // 512 bytes
//
//}QCDATA;/*Make sure size of this structure as multiple of 8*/

typedef struct/*Size : 1032 bytes per test -> 63 Test QC data per block can be saved*/
{
	uint32_t InitFlag1; // 4 bytes
	float Res[MAX_NUM_OF_QC_CONTROL_PER_TEST][MAX_NUM_OF_QC_PER_TEST + 2]; // 512 bytes
	DATETIME m_DateTime[MAX_NUM_OF_QC_CONTROL_PER_TEST][MAX_NUM_OF_QC_PER_TEST + 2]; // 512 bytes
	uint32_t InitFlag2; // 4 bytes

}QCDATA;/*Make sure size of this structure as multiple of 8*/


void InitQcSetup(void);
void InitQcData(uint8_t u8TestId);
void NVM_ReadQcSetup(void);
void NVM_WriteQcSetup(void);
bool VerifybackupQCSETUPIntegrity(QCSETUP* g_QcSetup);
void Delete_QcSetup(void);
QCSETUP* Get_InstaneQcSetup(void);
QCDATA* Get_InstaneQcResult(void);
void WriteQcResData(uint8_t u8TestId , QCDATA *pQcData);
void ReadQcResData(uint8_t u8TestId , QCDATA *pQcData);
void InitializeQcResData(uint8_t u8TestId, QCDATA *pQcData);
void AppendQcResData(uint8_t u8TestId , uint8_t u8QcTestIdx ,float fRes);
#endif /* APPL_INC_NVM_QCSETUP_HPP_ */
