/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifndef __RTC_AE250_H
#define __RTC_AE250_H

#include "ae250.h"
#include "Driver_RTC.h"
#include "platform.h"

#ifndef BIT
#define BIT(n)                      ((unsigned int) 1 << (n))
#define BITS2(m,n)                  (BIT(m) | BIT(n) )

/* bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                   (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */

#define ATCRTC100_HALF_SECOND_SUPPORT   (0)
#define ATCRTC100_DAY_BITS              (5)        // for counting up to 2^5 = 32 days

// id rev register bit define
#define ATCRTC100_ID                BITS(12,31)    // ID number for ATCRTC100
#define ATCRTC100_MAJOR             BITS(4,11)     // Enable alarm wakeup signal
#define ATCRTC100_MINOR             BITS(0,3)      // Enable alarm interrupt

// counter register bit define
// Day passed after RTC enabled, BITS(17,21), counting up to 2^5 = 32 days
#define CNTR_DAY                    BITS(17,(17 + (ATCRTC100_DAY_BITS - 1)))
#define CNTR_HOUR                   BITS(12,16)    // Hour field of current time; range: 0 ~ 23
#define CNTR_MIN                    BITS(6,11)     // Minute field of current time; range: 0 ~ 59
#define CNTR_SEC                    BITS(0,5)      // Second field of current time; range: 0 ~ 59

// alarm register bit define
#define ALARM_HOUR                  BITS(12,16)    // Hour field of current time; range: 0 ~ 23
#define ALARM_MIN                   BITS(6,11)     // Minute field of current time; range: 0 ~ 59
#define ALARM_SEC                   BITS(0,5)      // Second field of current time; range: 0 ~ 59

// ctrl register bit define
#define CTRL_RTC_EN                 BIT(0)         // Enable RTC
#define CTRL_ALARM_WAKEUP           BIT(1)         // Enable alarm wakeup signal
#define CTRL_ALARM_INT              BIT(2)         // Enable alarm interrupt
#define CTRL_DAY                    BIT(3)         // Enable day interrupt
#define CTRL_HOUR                   BIT(4)         // Enable hour interrupt
#define CTRL_MIN                    BIT(5)         // Enable minute interrupt
#define CTRL_SEC                    BIT(6)         // Enable second interrupt
#define CTRL_HSEC                   BIT(7)         // Enable half-second interrupt

// status register bit define
#define STATUS_ALARM_INT            BIT(2)         // Alarm interrupt status
#define STATUS_DAY                  BIT(3)         // Day interrupt status
#define STATUS_HOUR                 BIT(4)         // Hour interrupt status
#define STATUS_MIN                  BIT(5)         // Minute interrupt status
#define STATUS_SEC                  BIT(6)         // Second interrupt status
#define STATUS_HSEC                 BIT(7)         // Half-second interrupt status
#define STATUS_WRITEDONE            BIT(16)        // the synchronization progress of RTC register updates

typedef struct _RTC_INFO
{
	NDS_RTC_SignalEvent_t   Sig_Evt_CB;
	NDS_RTC_TIME            Set_Rtc_Time;
	NDS_RTC_ALARM           Set_Rtc_Alarm;
	NDS_POWER_STATE         Pwr_State;
} RTC_INFO;

extern RTC_INFO g_rRtc_Info;
#endif /* __RTC_AE250_H */
