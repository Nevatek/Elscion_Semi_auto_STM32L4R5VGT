/*
 * RTC.c
 *
 *  Created on: Jul 3, 2022
 *      Author: Alvin
 */
#include "main.h"
#include "cmsis_os.h"
#include "RTC.h"
#include <stdio.h>
#include <stdlib.h>
extern RTC_HandleTypeDef hrtc;
extern osMutexId MutexIntRtcHandle;
#define MAX_MUTEX_WAIT_DELAY_INT_RTC 20
uint32_t GetCurrentDate(RTC_DateTypeDef *sDate)
{
	osMutexWait(MutexIntRtcHandle,MAX_MUTEX_WAIT_DELAY_INT_RTC);
	#define MAX_DATE_TIME_STRING 32
	char strDate[MAX_DATE_TIME_STRING] = {0};
	HAL_RTC_GetDate(&hrtc, sDate, RTC_FORMAT_BIN);

    if (sDate->Year == 0 || sDate->Month == 0 || sDate->Date == 0)
    {
        sDate->Year = 0x18; // 24 in hex
        sDate->Month = 0x01; // January
        sDate->Date = 0x01;  // 1st day
        HAL_RTC_SetDate(&hrtc, sDate, RTC_FORMAT_BIN);
    }
	snprintf(strDate,MAX_DATE_TIME_STRING,"%02d%02d%02d",sDate->Year,sDate->Month,sDate->Date);
	uint32_t u_n32Date = (uint32_t)atoi(strDate);
	osMutexRelease(MutexIntRtcHandle);
	return u_n32Date;
}
uint32_t GetCurrentTime(RTC_TimeTypeDef *sTime)
{
	osMutexWait(MutexIntRtcHandle,MAX_MUTEX_WAIT_DELAY_INT_RTC);
	#define MAX_DATE_TIME_STRING 32
	char strTime[MAX_DATE_TIME_STRING] = {0};
	HAL_RTC_GetTime(&hrtc, sTime, RTC_FORMAT_BIN);
    if (sTime->Hours == 0 && sTime->Minutes == 0 && sTime->Seconds == 0)
    {
        sTime->Hours = 0x01;  // 1 hour
        sTime->Minutes = 0x01; // 1 minute
        sTime->Seconds = 0x00; // 0 seconds
        HAL_RTC_SetTime(&hrtc, sTime, RTC_FORMAT_BIN);
    }
	snprintf(strTime,MAX_DATE_TIME_STRING,"%02d%02d%02d",sTime->Hours,sTime->Minutes,sTime->Seconds);
	uint32_t u_n32Time = (uint32_t)atoi(strTime);
	osMutexRelease(MutexIntRtcHandle);
	return u_n32Time;
}

HAL_StatusTypeDef SetCurrentDate(uint8_t u_n8Year,uint8_t u_n8Month,uint8_t u_n8Date)
{
   osMutexWait(MutexIntRtcHandle,MAX_MUTEX_WAIT_DELAY_INT_RTC);
   HAL_StatusTypeDef Status = HAL_ERROR;
   RTC_DateTypeDef sDate;
   sDate.WeekDay = RTC_WEEKDAY_MONDAY;
   sDate.Month = u_n8Month;
   sDate.Date = u_n8Date;
   sDate.Year = u_n8Year;
   Status = HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
   osMutexRelease(MutexIntRtcHandle);
   return Status;
}
HAL_StatusTypeDef SetCurrentTime(uint8_t u_n8Hour,uint8_t u_n8Min)
{
	osMutexWait(MutexIntRtcHandle,MAX_MUTEX_WAIT_DELAY_INT_RTC);
	HAL_StatusTypeDef Status = HAL_ERROR;
	RTC_TimeTypeDef sTime;
	sTime.Hours = u_n8Hour;
	sTime.Minutes = u_n8Min;
	sTime.Seconds = 0x00;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	osMutexRelease(MutexIntRtcHandle);
	return Status = HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
}
