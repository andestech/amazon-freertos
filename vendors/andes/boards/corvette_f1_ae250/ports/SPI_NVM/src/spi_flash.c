#include "spi_flash.h"

extern NDS_DRIVER_SPI Driver_SPI0;

/*flash cmd*/

#define OPCODE_WREN     6L      /* Write enable */
#define OPCODE_RDSR     5L      /* Read status register */
#define OPCODE_READ     3L      /* Read data bytes */
#define OPCODE_PP       2L      /* Page program */
#define OPCODE_SE       0x20L   /* Sector erase */
#define OPCODE_RES      0xabL   /* Read Electronic Signature */
#define OPCODE_RDID     0x9fL   /* Read JEDEC ID */
#define DUMMY		0x00

/*flash information*/
#define SIZE_1K			1024
#define MAX_SIZE		(6 * SIZE_1K)
#define SECTOR_SIZE		(4 * SIZE_1K)
#define MAX_READY_WAIT_COUNT    0x100000

enum ADDRLEN { addrlen_1byte = 0, addrlen_2byte, addrlen_3byte, addrlen_none };

static int wait_till_flash_ready(void);

static volatile char spi_event_transfer_complete = 0;

static void wait_complete(void) {

	while (!spi_event_transfer_complete);
	spi_event_transfer_complete = 0;

}


static void spi_callback(uint32_t event) {
	switch (event) {
		case NDS_SPI_EVENT_TRANSFER_COMPLETE:
			spi_event_transfer_complete = 1;
			break;
		case NDS_SPI_EVENT_DATA_LOST:
			while(1);
			break;
	}
}

void Flash_SpiInit(void){

	NDS_DRIVER_SPI *SPI_Dri = &Driver_SPI0;

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

void Flash_Read(uint32_t * Flash_Address, uint8_t * Data,  uint32_t Data_Size){

	int Cmd_Size = 4;
	int SEND_SIZE;
	uint8_t data_receive[512];
	uint8_t data_send[512];
	int loop = 0;
	NDS_DRIVER_SPI *SPI_Dri = &Driver_SPI0;

	while(Data_Size > 0){

		if(Data_Size > 256){
			SEND_SIZE = 256 + Cmd_Size;
		}
		else{
			SEND_SIZE = Data_Size + Cmd_Size;
		}

		if(!wait_till_flash_ready()) {
			while(1);
		}

		data_send[0] = (uint8_t)OPCODE_READ;
		data_send[1] = (uint8_t)(((uint32_t)Flash_Address >> 16 ) & 0xff); 
		data_send[2] = (uint8_t)(((uint32_t)Flash_Address >>  8 ) & 0xff);
		data_send[3] = (uint8_t)(((uint32_t)Flash_Address ) & 0xff);

		SPI_Dri->Transfer(data_receive, data_send, SEND_SIZE );
		wait_complete();

		for(int i = 0; i < (SEND_SIZE - Cmd_Size); i++){
			Data[loop*256 + i] = data_receive[Cmd_Size + i];
		}
		
		Flash_Address = (uint32_t *)((uint32_t)Flash_Address + (uint32_t)(SEND_SIZE - Cmd_Size));

		Data_Size = Data_Size - (SEND_SIZE - Cmd_Size);
		loop += 1;
	}
}

void Flash_Write(uint32_t * Flash_Address, uint8_t * Data,  uint32_t Data_Size){

	int Cmd_Size = 4;
	int SEND_SIZE;
	uint8_t data_send[ 512 ];
	int count = 0;
	int loop = 0;
	NDS_DRIVER_SPI *SPI_Dri = &Driver_SPI0;

	while (Data_Size > 0){

		Flash_Enable();

		if(Data_Size > 256){
			SEND_SIZE = 256 + Cmd_Size;
		}
		else{
			SEND_SIZE = Data_Size + Cmd_Size;
		}

		if(!wait_till_flash_ready()) {
			while(1);
		}

		data_send[0] = (uint8_t)OPCODE_PP;
		data_send[1] = (uint8_t)(((uint32_t)Flash_Address >> 16 ) & 0xff); 
		data_send[2] = (uint8_t)(((uint32_t)Flash_Address >>  8 ) & 0xff);
		data_send[3] = (uint8_t)(((uint32_t)Flash_Address ) & 0xff);

		for(count = 0; count < (SEND_SIZE -Cmd_Size ); count++){
			data_send[Cmd_Size + count] = Data[count + loop*256];
		}

		SPI_Dri->Send( data_send, SEND_SIZE);
		wait_complete();

		Flash_Address = (uint32_t *)((uint32_t)Flash_Address + (uint32_t)(SEND_SIZE - Cmd_Size));

		Data_Size = Data_Size - (SEND_SIZE - Cmd_Size);
		loop += 1;
	}
}

void Flash_Enable(void){

	int SEND_SIZE = 1;
	uint8_t data_send[1] = {0};/*equal to SEND_SIZE*/

	if(!wait_till_flash_ready()) {
		while(1);
	}

	data_send[0] = OPCODE_WREN;

	NDS_DRIVER_SPI *SPI_Dri = &Driver_SPI0;
	SPI_Dri->Send( data_send, SEND_SIZE);
	wait_complete();
}

void Flash_Erase( uint32_t * Flash_Address, int Data_Size){

	int Cmd_Size = 4;
	int Erase_Size = SECTOR_SIZE;
	uint8_t data_send[4] = {0};
	uint8_t data_receive[MAX_SIZE] = {0};

	uint32_t * Erase_Address = Flash_Address;

	NDS_DRIVER_SPI *SPI_Dri = &Driver_SPI0;

	while(Data_Size > 0){
		Flash_Enable();

		if(!wait_till_flash_ready()) {
			while(1);
		}

		/*Set the flash command & Address*/
		data_send[0] = (uint8_t)OPCODE_SE;
		data_send[1] = (uint8_t)(((uint32_t)Erase_Address >> 16 ) & 0xff); 
		data_send[2] = (uint8_t)(((uint32_t)Erase_Address >>  8 ) & 0xff);
		data_send[3] = (uint8_t)(((uint32_t)Erase_Address ) & 0xff);


		SPI_Dri->Send(data_send, Cmd_Size);
		wait_complete();

		/* check if erase success*/
		if(!wait_till_flash_ready()) {
			while(1);
		}

		Flash_Read(Erase_Address, data_receive, Erase_Size);

		for (int i = 0; i < Erase_Size; i++) {
			if(data_receive[i] != 0xff) {
				while(1);
			}
		}

		Data_Size = Data_Size - Erase_Size;
		Erase_Address = (uint32_t *)((uint32_t)Erase_Address + Erase_Size);
	}
}

char Flash_ReadStatus(void){

	int SEND_SIZE = 2;
	uint8_t data_send[2] = {0};/*equal to SEND_SIZE*/
        uint8_t data_receive[2] = {0};

	NDS_DRIVER_SPI *SPI_Dri = &Driver_SPI0;
	/*Set the flash command & Address*/
	data_send[0] = (uint8_t)OPCODE_RDSR;
	data_send[1] = (uint8_t)DUMMY;

	SPI_Dri->Transfer(data_receive, data_send, SEND_SIZE);
	wait_complete();

	return data_receive[1];
}

static int wait_till_flash_ready(void){

	unsigned char sr;
	unsigned int count;

	for(count = 0; count < MAX_READY_WAIT_COUNT; count++) {
		sr = Flash_ReadStatus();
		if(!(sr & SR_BUSY))
			return 1;
	}
	return 0;
}
