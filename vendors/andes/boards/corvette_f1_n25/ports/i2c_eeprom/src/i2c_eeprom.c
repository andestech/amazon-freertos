#include "Driver_I2C.h"
#include "i2c_eeprom.h"
#include "ae250.h"

extern NDS_DRIVER_I2C Driver_I2C0;
static NDS_DRIVER_I2C* pDrv_I2C = &Driver_I2C0;

/*Both of I2c EEPROM's A0 & A1 are 0, so device address is b1010000*/
#define EEPROM_I2C_ADDR			0x50
#define OWN_ADDRESS			0x51

#define EEPROM_MAX_WRITE		64

#define EEPROM_WRITE_TIME		1//(ms)

/*Compiler macro*/
#define MEMSET(s, c, n)              __builtin_memset ((s), (c), (n))
#define MEMCPY(des, src, n)          __builtin_memcpy((des), (src), (n))

volatile uint8_t tmp[16] = {0};
void Wait_Write_Complete(uint64_t ms){
	uint64_t i;
	for (i = 0; i < (ms * CPUFREQ)/1000; ++i) {
		tmp[i % 16] = (i % 16);
	}
}

void EEPROM_Uninitial(void){
	pDrv_I2C->Uninitialize();
}

int32_t EEPROM_Initial(void){

	int32_t Status = NDS_DRIVER_OK;

	// default slave mode
	Status = pDrv_I2C->Initialize(NULL);
	if(Status != NDS_DRIVER_OK){
                return Status;
        }

	Status = pDrv_I2C->Control(NDS_I2C_BUS_CLEAR, 0);
	if(Status != NDS_DRIVER_OK){
		return Status;
	}

	Status = pDrv_I2C->PowerControl(NDS_POWER_FULL);
	if(Status != NDS_DRIVER_OK){
	return Status;
	}

	Status = pDrv_I2C->Control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_STANDARD);
	if(Status != NDS_DRIVER_OK){
	return Status;
	}

	Status = pDrv_I2C->Control(NDS_I2C_OWN_ADDRESS, OWN_ADDRESS & (~NDS_I2C_ADDRESS_10BIT));

	//Status = pDrv_I2C->Control(NDS_I2C_OWN_ADDRESS, (EEPROM_I2C_ADDR));
	if(Status != NDS_DRIVER_OK){
		return Status;
	}

	return Status;
}

int32_t EEPROM_Write(uint32_t addr, uint8_t* buf, uint32_t len){

	uint8_t Wr_Buf[ EEPROM_MAX_WRITE + 2];//+2 is the address space
	int Send_Size = len;
	uint16_t Device_Addr = EEPROM_I2C_ADDR;

	while(len > 0){

		if(len > 32){
			Send_Size = 32;
		}
		else{
			Send_Size = len;
		}

		// 2 bytes for I2C flash memory address 0x0 ~ 0x3fff(16K)
		Wr_Buf[0] = (uint8_t)((addr >> 8) & 0xFF);
		Wr_Buf[1] = (uint8_t)(addr & 0xFF);

		MEMCPY(&Wr_Buf[2], buf, Send_Size);

		pDrv_I2C->MasterTransmit(Device_Addr, Wr_Buf, Send_Size + 2, false);
		while(pDrv_I2C->GetStatus().busy);

		if(pDrv_I2C->GetDataCount() != (Send_Size + 2)){
			return -1;
		}

		Wait_Write_Complete(EEPROM_WRITE_TIME);

		len = len - Send_Size;
		addr = addr + Send_Size;
		buf = buf + Send_Size;
	}

        return 0;
}

int32_t EEPROM_Read(uint32_t addr, uint8_t* buf, uint32_t len){

	uint8_t Cmd_Read[2] = {0};
	uint16_t Device_Addr = EEPROM_I2C_ADDR;
	int Receive_Size = len;

	while( len > 0 ){

		if(len > 256){
			Receive_Size = 256;
		}
		else{
			Receive_Size = len;
		}

		Cmd_Read[0] = (uint8_t)((addr >> 8) & 0xFF);
		Cmd_Read[1] = (uint8_t)(addr & 0xFF);
		// xfer_pending is true => no stop condition, still busy
		// since xfer_pending is true, no stop condition and busy bit will always be pull high
		// 10-bit slave address must set STOP bit
		// tell EEPROM which ROM address will be read

		pDrv_I2C->MasterTransmit(Device_Addr, Cmd_Read, 2, false);
		while(pDrv_I2C->GetStatus().busy);

		if(pDrv_I2C->GetDataCount() != 2){
			return -1;
		}

		// read-request to read the data from the ROM address
		pDrv_I2C->MasterReceive(Device_Addr, buf, Receive_Size, false);
		while(pDrv_I2C->GetStatus().busy);

		if(pDrv_I2C->GetDataCount() != Receive_Size){
			return -1;
		}

		len = len - Receive_Size;
		addr = addr + Receive_Size;
		buf = buf + Receive_Size;
	}

	return 0;
}
