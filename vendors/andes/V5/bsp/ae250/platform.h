/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "core_v5.h"
#include "ae250.h"

/*
 * Define 'NDS_PLIC_BASE' and 'NDS_PLIC_SW_BASE' before include platform
 * intrinsic header file to active PLIC/PLIC_SW related intrinsic functions.
 */
#define NDS_PLIC_BASE        PLIC_BASE
#define NDS_PLIC_SW_BASE     PLIC_SW_BASE
#include "nds_v5_platform.h"

/*****************************************************************************
 * Peripheral device HAL declaration
 ****************************************************************************/
#define DEV_PLMT             AE250_PLMT
#define DEV_DMA              AE250_DMA
#define DEV_SMU              AE250_SMU
#define DEV_UART1            AE250_UART1
#define DEV_UART2            AE250_UART2
#define DEV_PIT              AE250_PIT
#define DEV_RTC              AE250_RTC
#define DEV_GPIO             AE250_GPIO
#define DEV_I2C              AE250_I2C
#define DEV_SPI1             AE250_SPI1
#define DEV_SPI2             AE250_SPI2
#define DEV_SPI3             AE250_SPI3
#define DEV_SPI4             AE250_SPI4

/*****************************************************************************
 * Board specified
 ****************************************************************************/
#define GPIO_USED_MASK       0x7F    /* Which GPIOs to use */

#ifdef __cplusplus
}
#endif

#endif	/* __PLATFORM_H__ */
