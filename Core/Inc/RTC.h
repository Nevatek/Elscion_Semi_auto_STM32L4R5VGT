/*
 * RTC.h
 *
 *  Created on: Jul 3, 2022
 *      Author: Alvin
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_

typedef struct
{
	uint32_t  u8Day : 5;/*5 bit required to access 0 - 32 but required is 0 - 31*/
	uint32_t  u8Month : 4;/*4 bit required to access 0 - 16 but required is 0 - 12*/
	uint32_t u8Year : 8;/*8 bit required to access 0 - 255 but required is 0 - 100*/
	uint32_t  u8Hour : 5;/*4 bit required to access 0 - 32 but required is 0 - 23*/
	uint32_t  u8Min : 6;/*6 bit required to access 0 - 64 but required is 0 - 59*/
}stcNvmDateTime;

HAL_StatusTypeDef SetCurrentTime(uint8_t u_n8Hour,uint8_t u_n8Min);
HAL_StatusTypeDef SetCurrentDate(uint8_t u_n32Year,uint8_t u_n8Month,uint8_t u_n8Date);
uint32_t GetCurrentTime(RTC_TimeTypeDef *sTime);
uint32_t GetCurrentDate(RTC_DateTypeDef *sDate);
#endif /* INC_RTC_H_ */
