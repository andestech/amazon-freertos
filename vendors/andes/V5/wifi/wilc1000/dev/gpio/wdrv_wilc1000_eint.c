/*******************************************************************************
  WILC1000 Wireless Driver External Interrupt Handler

  File Name:
    wilc1000_eint.c

  Summary:
    External interrupt handler for WILC1000 wireless driver.

  Description:
    External interrupt handler for WILC1000 wireless driver.
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
#include <nds32_intrinsic.h>
#include <stdio.h>
#include "wdrv_wilc1000_api.h"
#include "Driver_GPIO.h"
#include "gpio_ae250.h"

#define GPIO_WIFI_PIN_MASK   0x200/*v1:19*/

extern NDS_DRIVER_GPIO Driver_GPIO;

static void (*s_isr)(void) = NULL;

void gpio_callback(uint32_t event){
	printf("i am cb 1\n");
        switch (event) {
		case NDS_GPIO_EVENT_PIN9:
			printf("i am cb 2\n");
			s_isr();
			break;
		default:
			break;

	}
}


void WDRV_STUB_INTR_SourceEnable(void)
{
	// Enable PLIC interrupt GPIO source

	__nds__plic_enable_interrupt(IRQ_GPIO_SOURCE);

	// Enable the Machine-External bit in MIE
	set_csr(NDS_MIE, MIP_MEIP);
	
	// Enable GIE
	set_csr(NDS_MSTATUS, MSTATUS_MIE);
}

void WDRV_STUB_INTR_SourceDisable(void)
{
	// Disable PLIC interrupt GPIO source
	__nds__plic_disable_interrupt(IRQ_GPIO_SOURCE);
}

void WDRV_STUB_INTR_Init(void (*isr)(void))
{
	// Disable PLIC interrupt GPIO source
	__nds__plic_disable_interrupt(IRQ_GPIO_SOURCE);

	NDS_DRIVER_GPIO *GPIO_Dri = &Driver_GPIO;
	
	GPIO_Dri->Initialize(gpio_callback);

	GPIO_Dri->Control(NDS_GPIO_SET_INTR_NEGATIVE_EDGE | NDS_GPIO_INTR_ENABLE, GPIO_WIFI_PIN_MASK);

	__nds__plic_enable_interrupt(IRQ_GPIO_SOURCE);

	s_isr = isr;
}

void WDRV_STUB_INTR_Deinit(void)
{
	__nds__plic_disable_interrupt(IRQ_GPIO_SOURCE);
	s_isr = NULL;
}
