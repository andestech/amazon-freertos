/*******************************************************************************
  WILC1000 Wireless Driver GPIO Interface

  File Name:
    wdrv_wilc1000_gpio.c

  Summary:
    GPIO interface for WILC1000 wireless driver.

  Description:
    GPIO interface for WILC1000 wireless driver.
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/

#include "wdrv_wilc1000_api.h"

#include "Driver_GPIO.h"
#include "gpio_ae250.h"

// GPIO driver
extern NDS_DRIVER_GPIO Driver_GPIO;

#define WDRV_RESET_N_PORT_CHANNEL 0/*no channel in F1*/
#define WDRV_CHIP_EN_PORT_CHANNEL 0
#define WDRV_RESET_N_BIT_POS            11 /*v1:20*/
#define WDRV_CHIP_EN_BIT_POS		10 /*v1:18*/

extern void gpio_callback(uint32_t event);

static void __GPIO_OutLow(uint32_t channel, uint32_t bit_pos)
{
	NDS_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;
	GPIO_Dri->Write( 1 << bit_pos, 0);
}

static void __GPIO_OutHigh(uint32_t channel, uint32_t bit_pos)
{
	NDS_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;
	GPIO_Dri->Write( 1 << bit_pos, 1);
}

void WDRV_STUB_GPIO_ModuleReset(void)
{
	__GPIO_OutLow(WDRV_RESET_N_PORT_CHANNEL, WDRV_RESET_N_BIT_POS);
}

void WDRV_STUB_GPIO_ModuleUnreset(void)
{
	__GPIO_OutHigh(WDRV_RESET_N_PORT_CHANNEL, WDRV_RESET_N_BIT_POS);
}

void WDRV_STUB_GPIO_ChipEnable(void)
{
	__GPIO_OutHigh(WDRV_CHIP_EN_PORT_CHANNEL, WDRV_CHIP_EN_BIT_POS);
}

void WDRV_STUB_GPIO_ChipDisable(void)
{
	__GPIO_OutLow(WDRV_CHIP_EN_PORT_CHANNEL, WDRV_CHIP_EN_BIT_POS);
}

void WDRV_STUB_GPIO_Initialize(void)
{
	NDS_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;

	GPIO_Dri->Initialize(gpio_callback);
	
	// set GPIO direction
	GPIO_Dri->SetDir( 1 << WDRV_RESET_N_BIT_POS, NDS_GPIO_DIR_OUTPUT);
	GPIO_Dri->SetDir( 1 << WDRV_CHIP_EN_BIT_POS, NDS_GPIO_DIR_OUTPUT);
}

void WDRV_STUB_GPIO_DeInitialize(void)
{
	NDS_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;
	GPIO_Dri->Uninitialize();
}

//DOM-IGNORE-END
