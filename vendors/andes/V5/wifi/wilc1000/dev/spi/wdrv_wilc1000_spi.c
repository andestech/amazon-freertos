/*******************************************************************************
  WILC1000 Wireless Driver SPI Communication Support

  File Name:
    wdrv_wilc1000_spi.c

  Summary:
    Support SPI communications to the WILC1000 module.

  Description:
    Support SPI communications to the WILC1000 module.
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
#include "Driver_SPI.h"
#include "spi_ae250.h"

#include "wifi/wilc1000/include/wdrv_wilc1000_api.h"

static volatile char spi_event_transfer_complete = 0;

extern NDS_DRIVER_SPI Driver_SPI3;

#define TOTAL_TRANSFER_SIZE 256

static void wait_complete(void) {
	        while (!spi_event_transfer_complete);
		spi_event_transfer_complete = 0;
}

void spi_callback(uint32_t event) {
	switch (event) {
		case NDS_SPI_EVENT_TRANSFER_COMPLETE:
			spi_event_transfer_complete = 1;
			break;
		case NDS_SPI_EVENT_DATA_LOST:
			while(1);
			break;
	}
}




bool WDRV_STUB_SPI_Out(unsigned char *const buf, uint32_t size)
{
	NDS_DRIVER_SPI *SPI_Dri = &Driver_SPI3;
	uint32_t ret = -1;
	
	// write data to slave
	ret = SPI_Dri->Send((uint8_t *)buf, size);

	while ( ret == NDS_DRIVER_ERROR_BUSY){
		ret = SPI_Dri->Send((uint8_t *)buf, size);
	}

	wait_complete();
	
	if ( ret == NDS_DRIVER_OK ){
		return true;
	}
	else{
		return false;
	}
}

bool WDRV_STUB_SPI_In(unsigned char *const buf, uint32_t size)
{
	NDS_DRIVER_SPI *SPI_Dri = &Driver_SPI3;
	uint32_t ret = -1;
	uint8_t data_in[TOTAL_TRANSFER_SIZE] = {0};

	ret = SPI_Dri->Transfer(buf, (uint8_t *)data_in, size);

	while ( ret == NDS_DRIVER_ERROR_BUSY){
		ret = SPI_Dri->Transfer(buf, (uint8_t *)data_in, size);
	}

	wait_complete();

	if ( ret == NDS_DRIVER_OK ){
		return true;
	}
	else{
		return false;
	}
}

void WDRV_STUB_SPI_Initialize(void)
{
	NDS_DRIVER_SPI *SPI_Dri = &Driver_SPI3;

	// initialize SPI
	SPI_Dri->Initialize(spi_callback);

	// power up the SPI peripheral
	SPI_Dri->PowerControl(NDS_POWER_FULL);

	// configure the SPI to master, 8-bit data length and bus speed to 1MHz
	SPI_Dri->Control(NDS_SPI_MODE_MASTER |
			 NDS_SPI_CPOL0_CPHA0 |
			 NDS_SPI_MSB_LSB     |
			 NDS_SPI_DATA_BITS(8), 1000000);
}

void WDRV_STUB_SPI_Deinitialize(void)
{
	NDS_DRIVER_SPI *SPI_Dri = &Driver_SPI3;
	SPI_Dri->Uninitialize();
}

//DOM-IGNORE-END
