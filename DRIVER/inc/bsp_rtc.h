/*****************************************************************************
 * Copyright (c) 2019, Nations Technologies Inc.
 *
 * All rights reserved.
 * ****************************************************************************
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Nations' name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY NATIONS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL NATIONS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ****************************************************************************/

/**
 * @file User_RTC_Config.h
 * @author Nations 
 * @version v1.0.1
 *
 * @copyright Copyright (c) 2019, Nations Technologies Inc. All rights reserved.
 */


#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__
#include "main.h"
#ifdef __cplusplus
extern "C" {
#endif


#define    RTC_ALARM_TEST_TYPE_IRQ        0x01
#define    RTC_ALARM_TEST_TYPE_OUTPUT     0x02
#define    RTC_ALARM_TEST_TYPE            RTC_ALARM_TEST_TYPE_IRQ

#define    RTC_ALARM_OUT_TYPE_OPEN_DRAIN  0x01
#define    RTC_ALARM_OUT_TYPE_PULL_PUSH   0x02
#define    RTC_ALARM_OUT_TYPE             RTC_ALARM_OUT_TYPE_PULL_PUSH

#define    USER_WRITE_BKP_DAT1_DATA       0xA5A5
#define    RTC_LSE_TRY_COUNT              250

#define		 DEFAULT_YEAR										2025-1970
#define		 DEFAULT_MONTH									2
#define		 DEFAULT_DATE										17
#define		 DEFAULT_WEEKDAY								1
#define		 DEFAULT_HOUR										9
#define		 DEFAULT_MIN										39
#define		 DEFAULT_SEC										0

typedef enum {
    RTC_CLK_SRC_TYPE_HSE128=0x01,
    RTC_CLK_SRC_TYPE_LSE=0x02,
    RTC_CLK_SRC_TYPE_LSI=0x03,
}RTC_CLK_SRC_TYPE;


extern ErrorStatus RTC_CLKSourceConfig(RTC_CLK_SRC_TYPE Clk_Src_Type, bool Is_First_Cfg_RCC, bool Is_Rst_Bkp);
extern ErrorStatus RTC_TimeRegulate(void);
extern ErrorStatus RTC_DateRegulate(void);
extern ErrorStatus RTC_AlarmRegulate(uint32_t RTC_Alarm);
extern void RTC_DateAndTimeDefaultVale(void);
extern  void RTC_PrescalerConfig(void);

extern void RTC_TimeShow(void);
extern void RTC_DateShow(void);
extern void RTC_AlarmShow(uint8_t AlarmX);
extern void EXTI17_RTCAlarm_Configuration(FunctionalState Cmd);
extern void EXTI_PA7_Configuration(void);
extern void EXTI20_RTCWKUP_Configuration(FunctionalState Cmd);

void SetRtcAlarmWakeup(u8 data);

void rtc_Init(void);
void Get_Rtc_Time(void);
void RTC_Date_Time_Write(u16 year,u8 month,u8 day,u8 hour,u8 minute,u8 second);
void Date_Time_Print(void);
#ifdef __cplusplus
}
#endif


#endif/*__USER_RTC_CONFIG_H__*/
