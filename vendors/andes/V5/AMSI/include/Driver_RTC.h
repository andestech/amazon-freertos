/*
 * Project:      RTC (Real-Time Clock) Driver definitions
 */

#ifndef __DRIVER_RTC_H
#define __DRIVER_RTC_H

#include "Driver_Common.h"

// API version
#define NDS_RTC_API_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,02)

// RTC Status
typedef struct _NDS_RTC_STATUS
{
    unsigned int Status;
} NDS_RTC_STATUS;

// RTC control ID
typedef enum _NDS_RTC_CTRL_ID
{
    NDS_RTC_CTRL_NONE,
    NDS_RTC_CTRL_EN,
    NDS_RTC_CTRL_ALARM_WAKEUP,
    NDS_RTC_CTRL_ALARM_INT,
    NDS_RTC_CTRL_DAY_INT,
    NDS_RTC_CTRL_HOUR_INT,
    NDS_RTC_CTRL_MIN_INT,
    NDS_RTC_CTRL_SEC_INT,
    NDS_RTC_CTRL_HSEC_INT,
}NDS_RTC_CTRL_ID;

// RTC event ID
#define NDS_RTC_EVENT_ALARM_INT     (1UL << 0)
#define NDS_RTC_EVENT_DAY_INT       (1UL << 1)
#define NDS_RTC_EVENT_HOUR_INT      (1UL << 2)
#define NDS_RTC_EVENT_MIN_INT       (1UL << 3)
#define NDS_RTC_EVENT_SEC_INT       (1UL << 4)
#define NDS_RTC_EVENT_HSEC_INT      (1UL << 5)

// callback function
typedef void (*NDS_RTC_SignalEvent_t) (uint32_t event);

// RTC Driver Time Format.
typedef struct _NDS_RTC_TIME
{
    uint32_t day;
    uint32_t hour;
    uint32_t min;
    uint32_t sec;
} NDS_RTC_TIME;

// RTC Driver Date Format.
typedef struct _NDS_RTC_ALARM
{
    uint32_t hour;
    uint32_t min;
    uint32_t sec;
} NDS_RTC_ALARM;

// Access structure of the RTC Driver.
typedef struct _NDS_DRIVER_RTC
{
    NDS_DRIVER_VERSION   (*GetVersion) (void);
    int32_t              (*Initialize) (NDS_RTC_SignalEvent_t cb_event);
    int32_t              (*Uninitialize) (void);
    int32_t              (*PowerControl) (NDS_POWER_STATE state);
    int32_t              (*SetTime) (NDS_RTC_TIME* stime, uint32_t format);
    int32_t              (*GetTime) (NDS_RTC_TIME* stime, uint32_t format);
    int32_t              (*SetAlarm) (NDS_RTC_ALARM* salarm, uint32_t format);
    int32_t              (*GetAlarm) (NDS_RTC_ALARM* salarm, uint32_t format);
    int32_t              (*Control) (uint32_t control, uint32_t arg);
    NDS_RTC_STATUS       (*GetStatus) (void);
} const NDS_DRIVER_RTC;
#endif /* __DRIVER_RTC_H */
