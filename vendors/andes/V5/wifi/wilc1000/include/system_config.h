/*******************************************************************************
  MPLAB Harmony System Configuration Header

  File Name:
    system_config.h

  Summary:
    Build-time configuration header for the system defined by this MPLAB Harmony
    project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    This configuration header must not define any prototypes or data
    definitions (or include any files that do).  It only provides macro
    definitions for build-time configuration options that are not instantiated
    until used by another MPLAB Harmony module or application.

    Created with MPLAB Harmony Version 2.05
*******************************************************************************/
// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright 2017 Microchip Technology Incorporated and its subsidiaries.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*******************************************************************************/
// DOM-IGNORE-END


#include "iot_wifi.h"
#include "aws_clientcredential.h"

#define WDRV_EXT_RTOS_TASK_SIZE (2048u)
#define WDRV_EXT_RTOS_TASK_PRIORITY (configMAX_PRIORITIES - 1)
#define WDRV_DEFAULT_CHANNEL 6


#define WDRV_DEFAULT_SSID clientcredentialWIFI_SSID                                                                                       
/* See wdrv_wilc1000_api.h for default key examples in the help definitions. */
#define WDRV_DEFAULT_SECURITY_MODE clientcredentialWIFI_SECURITY
#define WDRV_DEFAULT_WEP_KEYS_40 "5AFB6C8E77" // default WEP40 key
#define WDRV_DEFAULT_WEP_KEYS_104 "90E96780C739409DA50034FCAA" // default WEP104 key
#define WDRV_DEFAULT_PSK_PHRASE clientcredentialWIFI_PASSWORD
#define WDRV_DEFAULT_WPS_PIN "12390212" // default WPS PIN


#define WDRV_DEFAULT_NETWORK_TYPE WDRV_NETWORK_TYPE_INFRASTRUCTURE
/*
#define WDRV_DEFAULT_WEP_KEYS_104 "90E96780C739409DA50034FCAA"
#define WDRV_DEFAULT_WEP_KEYS_40 "5AFB6C8E77"

#define WDRV_DEFAULT_WPS_PIN "12390212"
#define WDRV_DEFAULT_SECURITY_MODE clientcredentialWIFI_SECURITY

#define WDRV_DEFAULT_PSK_PHRASE clientcredentialWIFI_PASSWORD

#define WDRV_DEFAULT_SSID clientcredentialWIFI_SSID
*/
/*
#define WILC1000_INT_SOURCE 2
#define SYS_INT_SourceEnable( source ) do { __nds__plic_set_priority(source, 1);__nds__plic_enable_interrupt(source); } while (0)
#define SYS_INT_SourceDisable( source ) __nds__plic_disable_interrupt( source )
#define SYS_INT_SourceStatusClear( source ) __nds__plic_complete_interrupt(source);
*/

#define SYS_ERROR_PRINT(level, fmt, ...)    do { printf(fmt, ##__VA_ARGS__); } while (0)
/*
#define SYS_CONSOLE_MESSAGE(message) printf(message)
*/
#define TCPIP_STACK_USE_EVENT_NOTIFICATION
#define OSAL_USE_RTOS          9
/*
typedef int              OSAL_SEM_HANDLE_TYPE;
typedef SemaphoreHandle_t              OSAL_SEM_HANDLE_TYPE;
typedef SemaphoreHandle_t              OSAL_MUTEX_HANDLE_TYPE;
*/
