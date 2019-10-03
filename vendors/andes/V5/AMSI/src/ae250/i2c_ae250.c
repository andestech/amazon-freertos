/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "i2c_ae250.h"
#include "RTE_Device.h"

#define NDS_I2C_DRV_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if ((!RTE_I2C0))
	#error "I2C not enabled in RTE_Device.h!"
#endif

// Driver Version
static const NDS_DRIVER_VERSION DriverVersion = {
	NDS_I2C_API_VERSION,
	NDS_I2C_DRV_VERSION
};

// Driver Capabilities
static const NDS_I2C_CAPABILITIES DriverCapabilities = {
	1
};

#if (RTE_I2C0)
// I2C0 Control Information
static I2C_INFO I2C0_Info = {0};

#if (RTE_I2C0_DMA_TX_EN == 1)
void i2c0_dma_tx_event (uint32_t event);
static I2C_DMA i2c0_dma_tx = {
	RTE_I2C0_DMA_TX_CH,
	RTE_I2C0_DMA_TX_REQID,
	i2c0_dma_tx_event
};
#endif

#if (RTE_I2C0_DMA_RX_EN == 1)
void i2c0_dma_rx_event (uint32_t event);
static I2C_DMA i2c0_dma_rx = {
	RTE_I2C0_DMA_RX_CH,
	RTE_I2C0_DMA_RX_REQID,
	i2c0_dma_rx_event
};
#endif

// I2C0 Resources
static I2C_RESOURCES I2C0_Resources = {
	AE250_I2C,

#if (RTE_I2C0_DMA_TX_EN == 1)
	&i2c0_dma_tx,
#else
	NULL,
#endif

#if (RTE_I2C0_DMA_RX_EN == 1)
	&i2c0_dma_rx,
#else
	NULL,
#endif

	&I2C0_Info
};
#endif

NDS_DRIVER_VERSION i2c_get_version(void) {
	return DriverVersion;
}

NDS_I2C_CAPABILITIES i2c_get_capabilities(void) {
	return DriverCapabilities;
}

int32_t i2cx_initialize(NDS_I2C_SignalEvent_t cb_event, I2C_RESOURCES* i2c) {
	if (i2c->info->Driver_State & I2C_DRV_INIT) {
		return NDS_DRIVER_OK;
	}

	// Priority must be set > 0 to trigger the interrupt
	__nds__plic_set_priority(IRQ_I2C_SOURCE, 1);

	// Enable PLIC interrupt I2C source
	__nds__plic_enable_interrupt(IRQ_I2C_SOURCE);

	// Enable the Machine-External bit in MIE
	set_csr(NDS_MIE, MIP_MEIP);

	// Enable GIE
	set_csr(NDS_MSTATUS, MSTATUS_MIE);

	i2c->info->Sig_Evt_CB = cb_event;
	i2c->info->Pwr_State = NDS_POWER_FULL;

	// DMA Initialize
	if (i2c->dma_tx || i2c->dma_rx)	{
		dma_initialize();
	}

	i2c->info->Driver_State |= I2C_DRV_INIT;
	return NDS_DRIVER_OK;
}

int32_t i2cx_uninitialize(I2C_RESOURCES* i2c) {
	// DMA Uninitialize
	if (i2c->dma_tx || i2c->dma_rx) {
		dma_uninitialize();

		if (i2c->dma_tx && (i2c->info->Status.busy != 0))
			dma_channel_disable(i2c->dma_tx->channel);

		if (i2c->dma_rx && (i2c->info->Status.busy != 0))
			dma_channel_disable(i2c->dma_rx->channel);
	}

	// Disable and Clear DMA IRQ
	__nds__plic_disable_interrupt(IRQ_I2C_SOURCE);

	i2c->info->Sig_Evt_CB = NULL;
	i2c->info->Pwr_State = NDS_POWER_OFF;

	// clear & set driver state to none
	i2c->info->Driver_State = I2C_DRV_NONE;

	return NDS_DRIVER_OK;
}

int32_t i2cx_power_control(NDS_POWER_STATE state, I2C_RESOURCES* i2c) {
	uint32_t Tmp_C = 0, Tmp_S1 = 0;

	i2c->info->Pwr_State = state;

	switch (state) {
	case NDS_POWER_OFF:
		// DMA uninitialize
		if (i2c->dma_tx || i2c->dma_rx) {
			dma_uninitialize();

			if (i2c->dma_tx && (i2c->info->Status.busy != 0))
				dma_channel_disable(i2c->dma_tx->channel);

			if (i2c->dma_rx && (i2c->info->Status.busy != 0))
				dma_channel_disable(i2c->dma_rx->channel);
		}

		// Disable and Clear DMA IRQ
		__nds__plic_disable_interrupt(IRQ_I2C_SOURCE);

		// I2C reset controller, including disable all I2C interrupts & clear fifo
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_RESET_I2C);
		i2c->reg->CMD = Tmp_C;

		// I2C Ctrl disable
		Tmp_C = i2c->reg->SETUP;
		if (i2c->dma_tx || i2c->dma_rx)	{
			Tmp_C &= (~ SETUP_DMA_EN);
		}
		Tmp_C &= (~ SETUP_I2C_EN);
		i2c->reg->SETUP = Tmp_C;

		i2c->info->Driver_State &= (~I2C_DRV_POWER);
		break;

	case NDS_POWER_LOW:
		break;

	case NDS_POWER_FULL:
		// I2C query FIFO depth
		// read only FIFO size config
		Tmp_S1 = i2c->reg->CFG;
		switch (Tmp_S1 & FIFO_SZ_MSK) {
		case FIFO_SZ_2:
		    i2c->info->FIFO_Depth = 2;
		    break;
		case FIFO_SZ_4:
		    i2c->info->FIFO_Depth = 4;
		    break;
		case FIFO_SZ_8:
		    i2c->info->FIFO_Depth = 8;
		    break;
		case FIFO_SZ_16:
		    i2c->info->FIFO_Depth = 16;
		    break;
		}

		// I2C reset controller, including disable all I2C interrupts & clear fifo
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_RESET_I2C);
		i2c->reg->CMD = Tmp_C;

		// I2C setting: speed standard, slave mode(default)
		// FIFO(CPU) mode, 7-bit slave address, Ctrl enable
		i2c->reg->SETUP = 0x0;
		Tmp_C = i2c->reg->SETUP;
		if (i2c->dma_tx || i2c->dma_rx)	{
			Tmp_C |= SETUP_DMA_EN;
		}
		Tmp_C |= ((SETUP_T_SUDAT_STD << 24) |
		          (SETUP_T_SP_STD  << 21) |
		          (SETUP_T_HDDAT_STD << 16) |
		          (SETUP_T_SCL_RATIO_STD << 13) |
		          (SETUP_T_SCLHI_STD << 4) |
		          SETUP_I2C_EN);
		i2c->reg->SETUP = Tmp_C;

		// I2C setting: enable completion interrupt & address hit interrupt
		Tmp_C = i2c->reg->INTEN;
		// dma-slave mode still need complete interrupt
		Tmp_C |= (IEN_CMPL | IEN_ADDR_HIT);
		i2c->reg->INTEN = Tmp_C;

		// clear status
		i2c->info->Status.busy = 0;
		// define mode => 0:slave / 1:master
		i2c->info->Status.mode = 0;
		// define direction => 0:tx / 1:rx
		i2c->info->Status.direction = 0;
		i2c->info->Status.arbitration_lost = 0;
		i2c->info->Status.bus_error = 0;

		i2c->info->Driver_State = I2C_DRV_POWER;
		break;

	default:
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	return NDS_DRIVER_OK;
}

// master mode know how many bytes to tx
int32_t i2cx_master_transmit(uint32_t addr, const uint8_t* data, uint32_t num, bool xfer_pending, I2C_RESOURCES* i2c) {
	uint32_t Tmp_C = 0;
	int32_t stat = 0;

	// max 10-bit address(0x3FF), null data or num is no payload for acknowledge polling
	// If no I2C payload, set Phase_data=0x0
	if (addr > 0x3FF) {
		return NDS_DRIVER_ERROR_PARAMETER;
	}

	if (i2c->info->Pwr_State != NDS_POWER_FULL) {
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	// Transfer operation in progress, or slave stalled
	if (i2c->info->Status.busy) {
		return NDS_DRIVER_ERROR_BUSY;
	}

	i2c->info->Status.busy = 1;
	// define mode => 0:slave / 1:master
	i2c->info->Status.mode = 1;
	// define direction => 0:tx / 1:rx
	i2c->info->Status.direction = 0;
	i2c->info->Status.arbitration_lost = 0;
	i2c->info->Status.bus_error = 0;

	// clear & set driver state to master tx before issue transaction
	i2c->info->Driver_State = I2C_DRV_MASTER_TX;

	// Step0
	// I2C reset controller, including disable all I2C interrupts & clear fifo
	Tmp_C = i2c->reg->CMD;
	Tmp_C &= (~ CMD_MSK);
	Tmp_C |= (CMD_RESET_I2C);
	i2c->reg->CMD = Tmp_C;

	// Step1
	// I2C master, FIFO(CPU) mode, Ctrl enable
	Tmp_C = i2c->reg->SETUP;
	if (i2c->dma_tx) {
		Tmp_C |= SETUP_DMA_EN;
	} else {
		Tmp_C &= (~ SETUP_DMA_EN);
	}
	Tmp_C |= (SETUP_MASTER | SETUP_I2C_EN);
	i2c->reg->SETUP = Tmp_C;

	// I2C timing parameter is configed by middleware w/ I2C_BUS_SPEED control code

	// Step2
	// I2C phase start enable, phase addr enable, phase data enable, phase stop enable.
	// If I2C data transaction w/o I2C payload, remember to clear data bit.
	// xfer_pending: Transfer operation is pending - Stop condition will not be generated.
	// The bus is busy when a START condition is on bus and it ends when a STOP condition is seen.
	// 10-bit slave address must set STOP bit.
	// I2C direction : master tx, set xfer data count.
	Tmp_C = i2c->reg->CTRL;
	Tmp_C &= (~ (CTRL_PHASE_STOP | CTRL_DIR | CTRL_DATA_COUNT));
	Tmp_C |= (CTRL_PHASE_START | CTRL_PHASE_ADDR | (!xfer_pending << 9) | (num & CTRL_DATA_COUNT));
	if (!num) {
		// clear bit
		Tmp_C &= (~ CTRL_PHASE_DATA);
	} else {
		Tmp_C |= CTRL_PHASE_DATA;
	}
	i2c->reg->CTRL = Tmp_C;

	// Step3
	i2c->info->Slave_Addr = addr;
	i2c->info->Xfered_Data_Wt_Ptr = 0;
	i2c->info->Xfer_Wt_Num = num;
	i2c->info->Xfer_Cmpl_Count = 0;
	i2c->info->middleware_tx_buf = (uint8_t*)data;

	// I2C slave address, general call address = 0x0(7-bit or 10-bit)
	Tmp_C = i2c->reg->ADDR;
	Tmp_C &= (~ SLAVE_ADDR_MSK);
	Tmp_C |= (i2c->info->Slave_Addr & (SLAVE_ADDR_MSK));
	i2c->reg->ADDR = Tmp_C;

	// Step4
	// I2C Enable the Completion Interrupt, Enable/Disable the FIFO Empty Interrupt
	// I2C Enable the Arbitration Lose Interrupt, master mode only
	Tmp_C = i2c->reg->INTEN;
	if (i2c->dma_tx) {
		// dma mode still need complete interrupt for status.busy timing
		// but no need fifo empty interrupt
		Tmp_C |= (IEN_CMPL | IEN_ARB_LOSE);
	} else {
		// I2C write a patch of data(FIFO_Depth) to FIFO,
		// it will be consumed empty if data is actually issued on I2C bus,
		// currently FIFO is not empty, will not trigger FIFO_EMPTY interrupt
		i2cx_master_fifo_write(i2c, 1);

		Tmp_C |= (IEN_CMPL | IEN_ARB_LOSE);
		if (num > 0) {
			// enable
			Tmp_C |= IEN_FIFO_EMPTY;
		} else {
			// disable
			Tmp_C &= (~ IEN_FIFO_EMPTY);
		}
	}
	i2c->reg->INTEN = Tmp_C;

	// Step5
	// I2C Write 0x1 to the Command register to issue the transaction
	Tmp_C = i2c->reg->CMD;
	Tmp_C &= (~ CMD_MSK);
	Tmp_C |= (CMD_ISSUE_TRANSACTION);
	i2c->reg->CMD = Tmp_C;

	// DMA mode
	if (i2c->dma_tx) {
		// Configure DMA channel
		stat = dma_channel_configure(i2c->dma_tx->channel,
		                               (uint32_t)(long)(&i2c->info->middleware_tx_buf[0]),
		                               (uint32_t)(long)(&(i2c->reg->DATA)),
		                               num,
		                               DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)             |
 		                               DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)          |
 		                               DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)          |
		                               DMA_CH_CTRL_DMODE_HANDSHAKE                 |
		                               DMA_CH_CTRL_SRCADDR_INC                     |
		                               DMA_CH_CTRL_DSTADDR_FIX                     |
		                               DMA_CH_CTRL_DSTREQ(i2c->dma_tx->reqsel)     |
		                               DMA_CH_CTRL_INTABT                          |
		                               DMA_CH_CTRL_INTERR                          |
		                               DMA_CH_CTRL_INTTC                           |
		                               DMA_CH_CTRL_ENABLE,
		                               i2c->dma_tx->cb_event);

		if (stat == -1)	{
			return NDS_DRIVER_ERROR;
		}
	}

	// master tx no blocking operation, blocking operation is owned by middleware

	return NDS_DRIVER_OK;
}

// master mode know how many bytes to rx
int32_t i2cx_master_receive(uint32_t addr, uint8_t* data, uint32_t num, bool xfer_pending, I2C_RESOURCES* i2c) {
	uint32_t Tmp_C = 0;
	int32_t stat = 0;

	// max 10-bit address(0x3FF), null data or num is no payload for acknowledge polling
	// If no I2C payload, set Phase_data=0x0
	if (addr > 0x3FF) {
		return NDS_DRIVER_ERROR_PARAMETER;
	}

	if (i2c->info->Pwr_State != NDS_POWER_FULL)
	{
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	// Transfer operation in progress, or slave stalled
	if (i2c->info->Status.busy) {
		return NDS_DRIVER_ERROR_BUSY;
	}

	i2c->info->Status.busy = 1;
	// define mode => 0:slave / 1:master
	i2c->info->Status.mode = 1;
	// define direction => 0:tx / 1:rx
	i2c->info->Status.direction = 1;
	i2c->info->Status.arbitration_lost = 0;
	i2c->info->Status.bus_error = 0;

	//clear & set driver state to master rx before issue transaction
	i2c->info->Driver_State = I2C_DRV_MASTER_RX;

	// Step0
	// I2C reset controller, including disable all I2C interrupts & clear fifo
	Tmp_C = i2c->reg->CMD;
	Tmp_C &= (~ CMD_MSK);
	Tmp_C |= (CMD_RESET_I2C);
	i2c->reg->CMD = Tmp_C;

	// Step1
	// I2C master, FIFO(CPU) mode, Ctrl enable
	Tmp_C = i2c->reg->SETUP;
	if (i2c->dma_rx) {
		Tmp_C |= SETUP_DMA_EN;
	} else {
		Tmp_C &= (~ SETUP_DMA_EN);
	}
	Tmp_C |= (SETUP_MASTER | SETUP_I2C_EN);
	i2c->reg->SETUP = Tmp_C;

	// I2C timing parameter is configed by middleware w/ I2C_BUS_SPEED control code

	// Step2 */
	// I2C phase start enable, phase addr enable, phase data enable, phase stop enable.
	// If I2C data transaction w/o I2C payload, remember to clear data bit.
	// xfer_pending: Transfer operation is pending - Stop condition will not be generated.
	// The bus is busy when a START condition is on bus and it ends when a STOP condition is seen.
	// 10-bit slave address must set STOP bit.
	// I2C direction : master rx, set xfer data count.
	Tmp_C = i2c->reg->CTRL;
	Tmp_C &= (~ (CTRL_PHASE_STOP | CTRL_DATA_COUNT));
	Tmp_C |= (CTRL_PHASE_START | CTRL_PHASE_ADDR | (!xfer_pending << 9) | (num & CTRL_DATA_COUNT) | CTRL_DIR);
	if (!num) {
		// clear bit
		Tmp_C &= (~ CTRL_PHASE_DATA);
	} else {
		Tmp_C |= CTRL_PHASE_DATA;
	}
	i2c->reg->CTRL = Tmp_C;

	// Step3
	i2c->info->Slave_Addr = addr;
	i2c->info->Xfered_Data_Rd_Ptr = 0;
	i2c->info->Xfer_Rd_Num = num;
	i2c->info->Xfer_Cmpl_Count = 0;
	i2c->info->middleware_rx_buf = data;

	// I2C slave address, general call address = 0x0(7-bit or 10-bit)
	Tmp_C = i2c->reg->ADDR;
	Tmp_C &= (~ SLAVE_ADDR_MSK);
	Tmp_C |= (i2c->info->Slave_Addr & (SLAVE_ADDR_MSK));
	i2c->reg->ADDR = Tmp_C;

	// Step4
	// I2C Enable the Completion Interrupt, Enable the FIFO Full Interrupt
	// I2C Enable the Arbitration Lose Interrupt, master mode only
	Tmp_C = i2c->reg->INTEN;
	if (i2c->dma_rx) {
		// dma mode still need complete interrupt for status.busy timing
		// but no need fifo empty interrupt
		Tmp_C |= (IEN_CMPL | IEN_ARB_LOSE);
	} else {
		Tmp_C |= (IEN_CMPL | IEN_FIFO_FULL | IEN_ARB_LOSE);
	}
	i2c->reg->INTEN = Tmp_C;

	// Step5
	// I2C Write 0x1 to the Command register to issue the transaction
	Tmp_C = i2c->reg->CMD;
	Tmp_C &= (~ CMD_MSK);
	Tmp_C |= (CMD_ISSUE_TRANSACTION);
	i2c->reg->CMD = Tmp_C;

	// DMA mode
	if (i2c->dma_rx) {
		// Configure DMA channel
		stat = dma_channel_configure(i2c->dma_rx->channel,
		                               (uint32_t)(long)(&(i2c->reg->DATA)),
		                               (uint32_t)(long)(&i2c->info->middleware_rx_buf[0]),
		                               num,
		                               DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)               |
		                               DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)            |
		                               DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)            |
		                               DMA_CH_CTRL_SMODE_HANDSHAKE                   |
		                               DMA_CH_CTRL_SRCADDR_FIX                       |
		                               DMA_CH_CTRL_DSTADDR_INC                       |
		                               DMA_CH_CTRL_SRCREQ(i2c->dma_rx->reqsel)       |
		                               DMA_CH_CTRL_INTABT                            |
		                               DMA_CH_CTRL_INTERR                            |
		                               DMA_CH_CTRL_INTTC                             |
		                               DMA_CH_CTRL_ENABLE,
		                               i2c->dma_rx->cb_event);

		if (stat == -1)	{
			return NDS_DRIVER_ERROR;
		}
	// Interrupt mode
	}

	// master rx no blocking operation, blocking operation is owned by middleware

	return NDS_DRIVER_OK;
}

// slave mode unknow how many bytes to tx, so tx 1 byte each time until complete or nack_assert
int32_t i2cx_slave_transmit(const uint8_t* data, uint32_t num, I2C_RESOURCES* i2c) {
	if (i2c->info->Pwr_State != NDS_POWER_FULL) {
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	// Transfer operation in progress, or master stalled
	if (i2c->info->Status.busy) {
		return NDS_DRIVER_ERROR_BUSY;
	}

	i2c->info->Status.busy = 1;
	// define mode => 0:slave / 1:master
	i2c->info->Status.mode = 0;
	// define direction => 0:tx / 1:rx
	i2c->info->Status.direction = 0;
	i2c->info->Status.bus_error = 0;

	// I2C xfer data count
	// If DMA is not enabled, DataCnt is the number of
	// bytes transmitted/received from the bus master.
	// It is reset to 0 when the controller is addressed
	// and then increased by one for each byte of data
	// transmitted/received

	// no I2C reset controller, no I2C clear fifo
	// w/ minimal change I2C HW setting

	i2c->info->Xfered_Data_Wt_Ptr = 0;
	i2c->info->Xfer_Wt_Num = num;
	i2c->info->Xfer_Cmpl_Count = 0;
	i2c->info->middleware_tx_buf = (uint8_t*)data;

	i2cx_slave_fifo_write(i2c);

	return NDS_DRIVER_OK;
}

// slave mode unknow how many bytes to rx, so rx fifo-full byte each time until complete
int32_t i2cx_slave_receive(uint8_t* data, uint32_t num, I2C_RESOURCES* i2c) {
	if (i2c->info->Pwr_State != NDS_POWER_FULL) {
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	// since middleware just read data from Xfer_Data_Rd_Buf,
	// no set busy flag, driver slave rx is independent of
	// middleware slave rx, if set busy flag will affect
	// slave tx behavior
	// define mode => 0:slave / 1:master
	i2c->info->Status.mode = 0;
	// define direction => 0:tx / 1:rx
	i2c->info->Status.direction = 1;
	i2c->info->Status.bus_error = 0;

	// I2C xfer data count
	// If DMA is not enabled, DataCnt is the number of
	// bytes transmitted/received from the bus master.
	// It is reset to 0 when the controller is addressed
	// and then increased by one for each byte of data
	// transmitted/received

	// no I2C reset controller, no I2C clear fifo since middleware just read data from Xfer_Data_Rd_Buf
	// w/ minimal change I2C HW setting

	// Xfer_Data_Rd_Buf already read the data from hw fifo and keep,
	// currently middleware able to take from the buffer */
	// error hit
	if (num > MAX_XFER_SZ) {
		return NDS_DRIVER_ERROR;
	}
	MEMCPY(data, &i2c->info->Xfer_Data_Rd_Buf[i2c->info->middleware_rx_ptr], num);

	i2c->info->Xfer_Rd_Num = num;
	i2c->info->Xfer_Cmpl_Count = 0;
	i2c->info->middleware_rx_ptr += num;

	// slave rx no blocking operation, blocking operation is owned by middleware

	return NDS_DRIVER_OK;
}

int32_t i2cx_get_datacount(I2C_RESOURCES* i2c) {
	return (i2c->info->Xfer_Cmpl_Count);
}

int32_t i2cx_control(uint32_t control, uint32_t arg0, uint32_t arg1, I2C_RESOURCES* i2c) {
	uint32_t Tmp_C = 0;
	int32_t status = NDS_DRIVER_OK;

	if (i2c->info->Pwr_State != NDS_POWER_FULL) {
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	switch (control) {
	// middleware use control code
	case NDS_I2C_OWN_ADDRESS:
		// set slave addressing
		Tmp_C = i2c->reg->SETUP;
		if (arg0 & NDS_I2C_ADDRESS_10BIT) {
			// I2C 10-bit slave address
			Tmp_C |= SETUP_ADDRESSING;
		} else {
			// I2C 7-bit slave address
			Tmp_C &= (~ SETUP_ADDRESSING);
		}
		i2c->reg->SETUP = Tmp_C;

		// I2C slave address, general call address = 0x0(7-bit or 10-bit)
		Tmp_C = i2c->reg->ADDR;
		Tmp_C &= (~ SLAVE_ADDR_MSK);
		Tmp_C |= (arg0 & (SLAVE_ADDR_MSK));
		i2c->reg->ADDR = Tmp_C;
		break;
	case NDS_I2C_BUS_SPEED:
		Tmp_C = i2c->reg->SETUP;
		// clear previous setting
   		Tmp_C &= (~ (SETUP_T_SUDAT | SETUP_T_SP | SETUP_T_HDDAT | SETUP_T_SCL_RATIO | SETUP_T_SCLHI));

		switch (arg0) {
		case NDS_I2C_BUS_SPEED_STANDARD:
			// I2C speed standard
			// apply current setting
			Tmp_C |= ((SETUP_T_SUDAT_STD << 24) |
			(SETUP_T_SP_STD  << 21) |
			(SETUP_T_HDDAT_STD << 16) |
			(SETUP_T_SCL_RATIO_STD << 13) |
			(SETUP_T_SCLHI_STD << 4));

			break;
		case NDS_I2C_BUS_SPEED_FAST:
			// I2C speed fast
			// apply current setting
			Tmp_C |= ((SETUP_T_SUDAT_FAST << 24) |
			(SETUP_T_SP_FAST  << 21) |
			(SETUP_T_HDDAT_FAST << 16) |
			(SETUP_T_SCL_RATIO_FAST << 13) |
			(SETUP_T_SCLHI_FAST << 4));
			break;
		case NDS_I2C_BUS_SPEED_FAST_PLUS:
			// I2C speed fast plus
			// apply current setting
			Tmp_C |= ((SETUP_T_SUDAT_FAST_P << 24) |
			(SETUP_T_SP_FAST_P  << 21) |
			(SETUP_T_HDDAT_FAST_P << 16) |
			(SETUP_T_SCL_RATIO_FAST_P << 13) |
			(SETUP_T_SCLHI_FAST_P << 4));
			break;
		default:
			return NDS_DRIVER_ERROR_UNSUPPORTED;
		}

		// apply
		i2c->reg->SETUP = Tmp_C;
		break;
	case NDS_I2C_BUS_CLEAR:
		// I2C reset controller, including disable all I2C interrupts & clear fifo
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_RESET_I2C);
		i2c->reg->CMD = Tmp_C;
		break;
	case NDS_I2C_ABORT_TRANSFER:
		// I2C reset controller ??
		// I2C reset controller, including disable all I2C interrupts & clear fifo
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_RESET_I2C);
		i2c->reg->CMD = Tmp_C;
		break;
	default:
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	i2c->info->Driver_State |= I2C_DRV_CFG_PARAM;

	return status;
}

NDS_I2C_STATUS i2cx_getstatus(I2C_RESOURCES* i2c) {
	return i2c->info->Status;
}

void i2cx_signalevent(uint32_t event, I2C_RESOURCES* i2c) {
	// invoke callback function
	if (i2c->info->Sig_Evt_CB) {
		i2c->info->Sig_Evt_CB(event);
	}
}

uint32_t check = 0;
void i2c_cmpl_handler(I2C_RESOURCES* i2c) {
	uint32_t Tmp_S = 0, Tmp_S1 = 0, Tmp_C = 0;

	Tmp_S = i2c->reg->SETUP;

	// master mode
	if (Tmp_S & SETUP_MASTER) {
		// I2C disable all Interrupts in the Interrupt Enable Register
		Tmp_C = i2c->reg->INTEN;
		Tmp_C &= (~ IEN_ALL);
		i2c->reg->INTEN = Tmp_C;
	} else {
		// I2C no disable all Interrupts in the Interrupt Enable Register,
		// keep previous setting for slave tx */
	}

	// check the DataCnt field of the Control Register
	// to know if all data are successfully transmitted.
	// -> Master: The number of bytes to transmit/receive.
	// 0 means 256 bytes. DataCnt will be decreased by one
	// for each byte transmitted/received.
	if ((i2c->info->Driver_State & I2C_DRV_MASTER_TX) || (i2c->info->Driver_State & I2C_DRV_MASTER_RX)) {
		Tmp_S = i2c->reg->CTRL;

		if (i2c->info->Driver_State & I2C_DRV_MASTER_TX) {
			if (i2c->dma_tx) {
				i2c->info->Xfered_Data_Wt_Ptr = dma_channel_get_count(RTE_I2C0_DMA_TX_CH);
				i2c->info->Xfer_Cmpl_Count = dma_channel_get_count(RTE_I2C0_DMA_TX_CH);
			} else {
				i2c->info->Xfer_Cmpl_Count = i2c->info->Xfer_Wt_Num - (Tmp_S & CTRL_DATA_COUNT);
			}

			// clear & set driver state to master tx complete
			i2c->info->Driver_State = I2C_DRV_MASTER_TX_CMPL;

			// clear busy bit on i2c complete event as master dma/cpu tx
			i2c->info->Status.busy = 0;
		}

		if (i2c->info->Driver_State & I2C_DRV_MASTER_RX) {
			if (i2c->dma_rx) {
				i2c->info->Xfered_Data_Rd_Ptr = dma_channel_get_count(RTE_I2C0_DMA_RX_CH);
				i2c->info->Xfer_Cmpl_Count = dma_channel_get_count(RTE_I2C0_DMA_RX_CH);

				// clear & set driver state to master rx complete
				i2c->info->Driver_State = I2C_DRV_MASTER_RX_CMPL;

				// clear busy bit on dma complete event as master dma rx
				// i2c->info->Status.busy = 0;
			} else {
				i2cx_master_fifo_read(i2c);

				i2c->info->Xfer_Cmpl_Count = i2c->info->Xfer_Rd_Num - (Tmp_S & CTRL_DATA_COUNT);

				// clear & set driver state to master rx complete
				i2c->info->Driver_State = I2C_DRV_MASTER_RX_CMPL;

				// clear busy bit on i2c complete event as master cpu rx
				i2c->info->Status.busy = 0;
			}
		}
	}

	// check the DataCnt field of the Control Register
	// to know if all data are successfully transmitted.
	// -> Slave: the meaning of DataCnt depends on the
	// DMA mode:
	// If DMA is not enabled, DataCnt is the number of
	// bytes transmitted/received from the bus master.
	// It is reset to 0 when the controller is addressed
	// and then increased by one for each byte of data
	// transmitted/received.
	// If DMA is enabled, DataCnt is the number of
	// bytes to transmit/receive. It will not be reset to 0
	// when the slave is addressed and it will be
	// decreased by one for each byte of data
	// transmitted/received..
	if ((i2c->info->Driver_State & I2C_DRV_SLAVE_TX) || (i2c->info->Driver_State & I2C_DRV_SLAVE_RX)) {
		Tmp_S1 = i2c->reg->SETUP;
		Tmp_S = i2c->reg->CTRL;

		// I2C_FIFO mode
		if (!(Tmp_S1 & SETUP_DMA_EN)) {
			if (i2c->info->Driver_State & I2C_DRV_SLAVE_TX) {
				// I2C Disable the Byte Transmit Interrupt in the Interrupt Enable Register
				Tmp_C = i2c->reg->INTEN;
				Tmp_C &= (~ IEN_BYTE_TRANS);
				i2c->reg->INTEN = Tmp_C;

				// clear & set driver state to slave tx complete
				i2c->info->Driver_State = I2C_DRV_SLAVE_TX_CMPL;
			}

			if (i2c->info->Driver_State & I2C_DRV_SLAVE_RX) {
				i2cx_slave_fifo_read(i2c, 0);

				// I2C Disable the FIFO Full Interrupt in the Interrupt Enable Register
				Tmp_C = i2c->reg->INTEN;
				Tmp_C &= (~ IEN_FIFO_FULL);
				i2c->reg->INTEN = Tmp_C;

				// keypoint for middleware to query
				i2c->info->Xfer_Cmpl_Count = i2c->info->Xfered_Data_Rd_Ptr - i2c->info->middleware_rx_ptr;

				// clear & set driver state to slave rx complete
				i2c->info->Driver_State = I2C_DRV_SLAVE_RX_CMPL;
			}

			// clear busy bit on i2c complete event as slave cpu tx/rx
			i2c->info->Status.busy = 0;
		// I2C_DMA mode
		}else {
			if (i2c->info->Driver_State & I2C_DRV_SLAVE_TX) {
				// I2C Disable the Byte Transmit Interrupt in the Interrupt Enable Register
				Tmp_C = i2c->reg->INTEN;
				Tmp_C &= (~ IEN_BYTE_TRANS);
				i2c->reg->INTEN = Tmp_C;

		                // keypoint for middleware to query
				i2c->info->Xfered_Data_Wt_Ptr = dma_channel_get_count(RTE_I2C0_DMA_TX_CH);
				i2c->info->Xfer_Cmpl_Count = dma_channel_get_count(RTE_I2C0_DMA_TX_CH);

				// clear & set driver state to slave tx complete
				i2c->info->Driver_State = I2C_DRV_SLAVE_TX_CMPL;

				// clear busy bit on i2c complete event as slave dma tx
				i2c->info->Status.busy = 0;
			}

			if (i2c->info->Driver_State & I2C_DRV_SLAVE_RX) {
				// keypoint for middleware to query
				i2c->info->Xfer_Cmpl_Count = dma_channel_get_count(RTE_I2C0_DMA_RX_CH);

				// abort dma channel since MAX_XFER_SZ-read and expect complete in cmpl_handler
				dma_channel_abort(RTE_I2C0_DMA_RX_CH);

				check = (Tmp_S & CTRL_DATA_COUNT);
				// clear & set driver state to slave rx complete
				i2c->info->Driver_State = I2C_DRV_SLAVE_RX_CMPL;

				// clear busy bit on i2c complete event as slave dma rx since read MAX_XFER_SZ
				i2c->info->Status.busy = 0;
			}
		}

		// I2C clear fifo first to prevent mistake i2cx_slave_fifo_read()
		// if the Completion Interrupt asserts, clear the FIFO and go next transaction.
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_CLEAR_FIFO);
		i2c->reg->CMD = Tmp_C;
	}

	// i2c->info->Status.busy = 0;
}

// basic fifo write function
void i2cx_master_fifo_write(I2C_RESOURCES* i2c, uint8_t is_flash_addr) {
	uint32_t i = 0, write_fifo_count = 0, Tmp_C = 0;

	write_fifo_count = ((i2c->info->Xfer_Wt_Num - i2c->info->Xfered_Data_Wt_Ptr) >= i2c->info->FIFO_Depth) ?
				i2c->info->FIFO_Depth : (i2c->info->Xfer_Wt_Num - i2c->info->Xfered_Data_Wt_Ptr);

	if (is_flash_addr) {
		write_fifo_count = 2;
	}

	// I2C write a patch of data(FIFO_Depth) to FIFO,
	// it will be consumed empty if data is actually issued on I2C bus */
	for (i = 0; i < write_fifo_count; i++) {
		// I2C write data to FIFO through data port register
		i2c->reg->DATA = (i2c->info->middleware_tx_buf[i2c->info->Xfered_Data_Wt_Ptr] & (DATA_MSK));

		i2c->info->Xfered_Data_Wt_Ptr++;

		// If all data are pushed into the FIFO, disable the FIFO Empty Interrupt. Otherwise, repeat
		if (i2c->info->Xfered_Data_Wt_Ptr == i2c->info->Xfer_Wt_Num) {
			// I2C disable the FIFO Empty Interrupt in the Interrupt Enable Register
			Tmp_C = i2c->reg->INTEN;
			Tmp_C &= (~ IEN_FIFO_EMPTY);
			i2c->reg->INTEN = Tmp_C;
		}
	}
}

// basic fifo write function
void i2cx_slave_fifo_write(I2C_RESOURCES* i2c) {
	uint32_t i = 0, write_fifo_count = 0, Tmp_C = 0;
	int32_t stat = 0;

	// slave TX 1 byte each time, since no information got
	// about how many bytes of master rx should be,
	// check nack_assert to complete slave tx */
	write_fifo_count = 1;

	// DMA mode
	if (i2c->dma_tx) {
		// If DMA is enabled, DataCnt is the number of
		// bytes to transmit/receive. It will not be reset to 0
		// when the slave is addressed and it will be
		// decreased by one for each byte of data
		// transmitted/received.*/
		// fix bug of dma-slave 10bit address test(rx 3 bytes flash addr)
		Tmp_C = i2c->reg->CTRL;
		Tmp_C &= (~ CTRL_DATA_COUNT);
		Tmp_C |= (write_fifo_count & CTRL_DATA_COUNT);
		i2c->reg->CTRL = Tmp_C;

		// Configure DMA channel
		stat = dma_channel_configure(i2c->dma_tx->channel,
		                               (uint32_t)(long)(&i2c->info->middleware_tx_buf[0]),
		                               (uint32_t)(long)(&(i2c->reg->DATA)),
		                               write_fifo_count,
		                               DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)               |
		                               DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)            |
		                               DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)            |
		                               DMA_CH_CTRL_DMODE_HANDSHAKE                   |
		                               DMA_CH_CTRL_SRCADDR_INC                       |
		                               DMA_CH_CTRL_DSTADDR_FIX                       |
		                               DMA_CH_CTRL_DSTREQ(i2c->dma_tx->reqsel)       |
		                               DMA_CH_CTRL_INTABT                            |
		                               DMA_CH_CTRL_INTERR                            |
		                               DMA_CH_CTRL_INTTC                             |
		                               DMA_CH_CTRL_ENABLE,
		                               i2c->dma_tx->cb_event);

		if (stat == -1) {
			return;
		}
	// Interrupt mode
	} else {
		// I2C write a patch of data(FIFO_Depth) to FIFO,
		// it will be consumed empty if data is actually issued on I2C bus
		for (i = 0; i < write_fifo_count; i++) {
			// I2C write data to FIFO through data port register
			i2c->reg->DATA = (i2c->info->middleware_tx_buf[i2c->info->Xfered_Data_Wt_Ptr] & (DATA_MSK));

			i2c->info->Xfered_Data_Wt_Ptr++;
			i2c->info->Xfer_Cmpl_Count++;
		}
	}
}

void i2cx_fifo_empty_handler(I2C_RESOURCES* i2c) {
	if (i2c->info->Pwr_State != NDS_POWER_FULL) {
		return;
	}

	if (i2c->info->Driver_State & I2C_DRV_MASTER_TX) {
		i2cx_master_fifo_write(i2c, 0);
	} else if (i2c->info->Driver_State & I2C_DRV_SLAVE_TX) {
		// i2cx_slave_fifo_write(prI2C_Info);
	}
}

// basic fifo read function
void i2cx_master_fifo_read(I2C_RESOURCES* i2c) {
	uint32_t i = 0, read_fifo_count = 0, Tmp_C = 0;

	read_fifo_count = ((i2c->info->Xfer_Rd_Num - i2c->info->Xfered_Data_Rd_Ptr) >= i2c->info->FIFO_Depth) ?
				i2c->info->FIFO_Depth : (i2c->info->Xfer_Rd_Num - i2c->info->Xfered_Data_Rd_Ptr);

	// I2C read a patch of data(FIFO_Depth) from FIFO,
	// it will be consumed empty if data is actually read out by driver
	for (i = 0; i < read_fifo_count; i++) {
		// I2C read data from FIFO through data port register
		i2c->info->middleware_rx_buf[i2c->info->Xfered_Data_Rd_Ptr] = (i2c->reg->DATA & (DATA_MSK));

		i2c->info->Xfered_Data_Rd_Ptr++;

		// If all data are read from the FIFO, disable the FIFO Full Interrupt. Otherwise, repeat
		if (i2c->info->Xfered_Data_Rd_Ptr == i2c->info->Xfer_Rd_Num) {
			// I2C disable the FIFO Full Interrupt in the Interrupt Enable Register
			Tmp_C = i2c->reg->INTEN;
			Tmp_C &= (~ IEN_FIFO_FULL);
			i2c->reg->INTEN = Tmp_C;
		}
	}
}

// basic fifo read function
void i2cx_slave_fifo_read(I2C_RESOURCES* i2c, uint8_t is_fifo_full) {
	uint32_t i = 0, read_fifo_count = 0, curr_rx_data_count = 0;

	// slave rx data count is accumulated and depend on
	// master tx data count of one transaction(start-addr-data-stop),
	// possible larger than fifo length(4 bytes)
	// slave: If DMA is not enabled(FIFO mode), DataCnt is the number of
	// bytes transmitted/received from the bus master.
	// It is reset to 0 when the controller is addressed
	// and then increased by one for each byte of data
	// transmitted/received
	// curr_rx_data_count = (i2c->reg->CTRL & CTRL_DATA_COUNT);
	curr_rx_data_count = (i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val & CTRL_DATA_COUNT);

	// error hit
	if (curr_rx_data_count > MAX_XFER_SZ) {
		while(1);
	}

	if (is_fifo_full) {
		read_fifo_count = i2c->info->FIFO_Depth;
	// just only read fifo in complete interrut handler if fifo is not full
	} else {
		read_fifo_count = curr_rx_data_count - i2c->info->last_rx_data_count;
	}

	// error hit
	if (read_fifo_count > MAX_XFER_SZ) {
		while(1);
	}

	// I2C read a patch of data(FIFO_Depth) from FIFO,
	// it will be consumed empty if data is actually read out by driver */
	for (i = 0; i < read_fifo_count; i++) {
		// I2C read data from FIFO through data port register
		i2c->info->Xfer_Data_Rd_Buf[i2c->info->Xfered_Data_Rd_Ptr] = (i2c->reg->DATA & (DATA_MSK));

		i2c->info->Xfered_Data_Rd_Ptr++;

		if (i2c->info->Xfered_Data_Rd_Ptr == MAX_XFER_SZ) {
			// slave rx buffer overwrite
			i2c->info->Xfered_Data_Rd_Ptr_Ow++;
			i2c->info->Xfered_Data_Rd_Ptr = 0;
		}
	}

	i2c->info->last_rx_data_count = curr_rx_data_count;
}

void i2cx_fifo_full_handler(I2C_RESOURCES* i2c) {
	if (i2c->info->Pwr_State != NDS_POWER_FULL) {
		return;
	}

	if (i2c->info->Driver_State & I2C_DRV_MASTER_RX) {
		i2cx_master_fifo_read(i2c);
	} else if (i2c->info->Driver_State & I2C_DRV_SLAVE_RX) {
		i2cx_slave_fifo_read(i2c, 1);
	}
}

void i2cx_slave_addr_hit_handler(I2C_RESOURCES* i2c) {
	uint32_t Tmp_C = 0;
	int32_t stat = 0;

	if (i2c->info->Pwr_State != NDS_POWER_FULL) {
		return;
	}

	// I2C clear fifo first to prevent mistake i2cx_slave_fifo_read()
	// if the Completion Interrupt asserts, clear the FIFO and go next transaction.
	Tmp_C = i2c->reg->CMD;
	Tmp_C &= (~ CMD_MSK);
	Tmp_C |= (CMD_CLEAR_FIFO);
	i2c->reg->CMD = Tmp_C;

	// slave mode Rx: if address hit, fifo may not be full state
	if (i2c->info->Driver_State & I2C_DRV_SLAVE_RX) {
		// A new I2C data transaction(start-addr-data-stop)
		i2c->info->Xfered_Data_Rd_Ptr_Ow = 0;
		i2c->info->Xfered_Data_Rd_Ptr = 0;
		// keypoint to reset
		i2c->info->middleware_rx_ptr = 0;
		MEMSET(i2c->info->Xfer_Data_Rd_Buf, 0, sizeof(i2c->info->Xfer_Data_Rd_Buf));

		if (i2c->dma_rx) {
			i2c->info->last_rx_data_count = (i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val & CTRL_DATA_COUNT);

			// If DMA is enabled, DataCnt is the number of
			// bytes to transmit/receive. It will not be reset to 0
			// when the slave is addressed and it will be
			// decreased by one for each byte of data
			// transmitted/received.
			// fix bug of dma-slave 10bit address test(rx 3 bytes flash addr)
			Tmp_C = i2c->reg->CTRL;
			Tmp_C &= (~ CTRL_DATA_COUNT);
			Tmp_C |= (MAX_XFER_SZ & CTRL_DATA_COUNT);
			i2c->reg->CTRL = Tmp_C;

			// Configure DMA channel w/ MAX_XFER_SZ-read and expect complete in cmpl_handler
			stat = dma_channel_configure(i2c->dma_rx->channel,
			                               (uint32_t)(long)(&(i2c->reg->DATA)),
			                               (uint32_t)(long)(&i2c->info->Xfer_Data_Rd_Buf[i2c->info->Xfered_Data_Rd_Ptr]),
			                               MAX_XFER_SZ,
			                               DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)               |
			                               DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)            |
			                               DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)            |
			                               DMA_CH_CTRL_SMODE_HANDSHAKE                   |
			                               DMA_CH_CTRL_SRCADDR_FIX                       |
			                               DMA_CH_CTRL_DSTADDR_INC                       |
			                               DMA_CH_CTRL_SRCREQ(i2c->dma_rx->reqsel)       |
			                               DMA_CH_CTRL_INTABT                            |
			                               DMA_CH_CTRL_INTERR                            |
			                               DMA_CH_CTRL_INTTC                             |
			                               DMA_CH_CTRL_ENABLE,
			                               i2c->dma_rx->cb_event);

			if (stat == -1) {
				return;
			}

		} else {
			i2c->info->last_rx_data_count = 0;

			// I2C Enable the FIFO Full Interrupt in the Interrupt Enable Register
			Tmp_C = i2c->reg->INTEN;
			Tmp_C |= IEN_FIFO_FULL;
			i2c->reg->INTEN = Tmp_C;
		}
		// slave mode Tx: if address hit, fifo may not be empty state
	} else if (i2c->info->Driver_State & I2C_DRV_SLAVE_TX) {
		// master will gen nack_assert to notify slave TX complete
		i2c->info->nack_assert = 0;

		// I2C Enable the Byte Transmit Interrupt in the Interrupt Enable Register
		// for status.busy flag support
		Tmp_C = i2c->reg->INTEN;
		Tmp_C |= IEN_BYTE_TRANS;
		i2c->reg->INTEN = Tmp_C;
	}
}

void i2c_irq_handler() {
	I2C_RESOURCES* i2c = &I2C0_Resources;
	uint32_t Tmp_S = 0, Tmp_S1 = 0, Tmp_W = 0, Tmp_C = 0, Evt_Id = 0;

	Tmp_W = Tmp_S = i2c->reg->STATUS;
	Tmp_S1 = i2c->reg->SETUP;
	Tmp_C = i2c->reg->CTRL;

	// write 1 clear for those interrupts be able to W1C
	i2c->reg->STATUS = (Tmp_W & STATUS_W1C_ALL);

	if (Tmp_S & STATUS_CMPL) {
		// as i2c irq hit, quick store hw already-tranfered data count
		i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val = Tmp_C;

		i2c_cmpl_handler(i2c);

		Evt_Id |= NDS_I2C_EVENT_TRANSFER_DONE;
	} else {
		Evt_Id |= NDS_I2C_EVENT_TRANSFER_INCOMPLETE;
	}

	if (Tmp_S & STATUS_FIFO_EMPTY) {
		i2cx_fifo_empty_handler(i2c);
	}

	if (Tmp_S & STATUS_FIFO_FULL) {
		// as i2c irq hit, quick store hw already-tranfered data count
		i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val = Tmp_C;

		i2cx_fifo_full_handler(i2c);
	}

	// Here is the entry for slave mode driver to detect
	// slave RX/TX action depend on master TX/RX action.
	// Addr hit is W1C bit, so during payload transaction,
	// it is not set again.
	// A new I2C data transaction(start-addr-data-stop)
	if (Tmp_S & STATUS_ADDR_HIT) {
		// slave mode
		if (!(Tmp_S1 & SETUP_MASTER)) {
			// cleared on start of next Slave operation
			i2c->info->Status.general_call = 0;

			if (((Tmp_C & CTRL_DIR) >> 8) == I2C_SLAVE_RX) {
			    i2c->info->Slave_Rx_Cmpl_Ctrl_Reg_Val = Tmp_C;

				// notify middleware to do slave rx action
				Evt_Id |= NDS_I2C_EVENT_SLAVE_RECEIVE;

				//clear & set driver state to slave rx before data transaction
				i2c->info->Driver_State = I2C_DRV_SLAVE_RX;
			} else if (((Tmp_C & CTRL_DIR) >> 8) == I2C_SLAVE_TX) {
				// notify middleware to do slave tx action
				Evt_Id |= NDS_I2C_EVENT_SLAVE_TRANSMIT;

				//clear & set driver state to slave tx before data transaction
				i2c->info->Driver_State = I2C_DRV_SLAVE_TX;
			}

			// A new I2C data transaction(start-addr-data-stop)
			i2cx_slave_addr_hit_handler(i2c);
		}
	}

	// for slave mode to complete slave tx
	if (!(Tmp_S & STATUS_ACK)) {
		//slave mode
		if (!(Tmp_S1 & SETUP_MASTER)) {
			if (Tmp_S & STATUS_BYTE_RECV) {

			} else if (Tmp_S & STATUS_BYTE_TRANS) {
				// notify middleware to do slave tx complete
				Evt_Id |= NDS_I2C_EVENT_ADDRESS_NACK;

				i2c->info->nack_assert = 1;
			}
		}
	}

	// Indicates that the address of the current
	// transaction is a general call address.
	// This status is only valid in slave mode.
	// A new I2C data transaction(start-addr-data-stop)
	// slave mode
	if ((Tmp_S & STATUS_ADDR_HIT) && (Tmp_S & STATUS_GEN_CALL) && !(Tmp_S1 & SETUP_MASTER)) {
		// cleared on start of next Slave operation
		i2c->info->Status.general_call = 1;

		// I2C General Call slave address
		Evt_Id |= NDS_I2C_EVENT_GENERAL_CALL;
	}

	// master mode
	if ((Tmp_S & STATUS_ARB_LOSE) && (Tmp_S1 & SETUP_MASTER)) {
		// cleared on start of next Master operation
		i2c->info->Status.arbitration_lost = 1;

		Evt_Id |= NDS_I2C_EVENT_ARBITRATION_LOST;
	}

	// slave mode
	if ((Tmp_S & STATUS_BYTE_TRANS) && !(Tmp_S1 & SETUP_MASTER)) {
		// I2C clear fifo first to prevent mistake i2cx_slave_fifo_read()
		// if the Completion Interrupt asserts, clear the FIFO and go next transaction.
		Tmp_C = i2c->reg->CMD;
		Tmp_C &= (~ CMD_MSK);
		Tmp_C |= (CMD_CLEAR_FIFO);
		i2c->reg->CMD = Tmp_C;

		// set on start of next slave operation,
		// cleared on slave tx 1 byte done or data transaction complete.
		i2c->info->Status.busy = 0;
	}

	i2cx_signalevent(Evt_Id, i2c);
}

#if ((RTE_I2C0) && (RTE_I2C0_DMA_TX_EN == 1))
void i2cx_dma_tx_event (uint32_t event, I2C_RESOURCES* i2c) {
	switch (event) {
	case DMA_EVENT_TERMINAL_COUNT_REQUEST:
		break;
	case DMA_EVENT_ERROR:
	default:
		break;
	}
}
#endif

#if ((RTE_I2C0) && (RTE_I2C0_DMA_RX_EN == 1))
static void i2cx_dma_rx_event (uint32_t event, I2C_RESOURCES* i2c) {
	switch (event) {
	case DMA_EVENT_TERMINAL_COUNT_REQUEST:
		// clear busy bit on dma complete event as master dma rx
		i2c->info->Status.busy = 0;
		break;
	case DMA_EVENT_ERROR:
	default:
		break;
	}
}
#endif

#if (RTE_I2C0)
int32_t i2c0_initialize(NDS_I2C_SignalEvent_t cb_event) {
	return  i2cx_initialize(cb_event, &I2C0_Resources);
}

int32_t i2c0_uninitialize() {
	return  i2cx_uninitialize(&I2C0_Resources);
}

int32_t i2c0_power_control(NDS_POWER_STATE state) {
	return  i2cx_power_control(state, &I2C0_Resources);
}

int32_t i2c0_master_transmit(uint32_t addr, const uint8_t* data, uint32_t num, bool xfer_pending) {
	return  i2cx_master_transmit(addr, data, num, xfer_pending, &I2C0_Resources);
}

int32_t i2c0_master_receive(uint32_t addr, uint8_t* data, uint32_t num, bool xfer_pending) {
	return  i2cx_master_receive(addr, data, num, xfer_pending, &I2C0_Resources);
}

int32_t i2c0_slave_transmit(const uint8_t* data, uint32_t num) {
	return  i2cx_slave_transmit(data, num, &I2C0_Resources);
}

int32_t i2c0_slave_receive(uint8_t* data, uint32_t num) {
	return  i2cx_slave_receive(data, num, &I2C0_Resources);
}

int32_t i2c0_get_datacount() {
	return  i2cx_get_datacount(&I2C0_Resources);
}

int32_t i2c0_control(uint32_t control, uint32_t arg) {
	return  i2cx_control(control, arg, 0, &I2C0_Resources);
}

NDS_I2C_STATUS i2c0_get_status() {
	return  i2cx_getstatus(&I2C0_Resources);
}

#if (RTE_I2C0_DMA_TX_EN == 1)
void i2c0_dma_tx_event (uint32_t event) {
	i2cx_dma_tx_event(event, &I2C0_Resources);
}
#endif

#if (RTE_I2C0_DMA_RX_EN == 1)
void i2c0_dma_rx_event (uint32_t event) {
	i2cx_dma_rx_event(event, &I2C0_Resources);
}
#endif

//End I2C Interface
NDS_DRIVER_I2C Driver_I2C0 = {
	i2c_get_version,
	i2c_get_capabilities,
	i2c0_initialize,
	i2c0_uninitialize,
	i2c0_power_control,
	i2c0_master_transmit,
	i2c0_master_receive,
	i2c0_slave_transmit,
	i2c0_slave_receive,
	i2c0_get_datacount,
	i2c0_control,
	i2c0_get_status,
};
#endif
