/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */


#include "spi_ae250.h"

#include "RTE_Device.h"

#define NDS_SPI_DRV_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,9)

// SPI clock for ae250 is 66MHz
#define SPI_CLK    66000000
// The delay time between two transfers in the master mode to wait slave ready. /ms
#define DelayTime  0
// Number of data transferred per block
#define BLOCK_SIZE 512


// SPI transfer operation
#define SPI_SEND                      0x0
#define SPI_RECEIVE                   0x1
#define SPI_TRANSFER                  0x2

#if ((!RTE_SPI0) && (!RTE_SPI1) && (!RTE_SPI2) && (!RTE_SPI3))
	#error "SPI not enabled in RTE_Device.h!"
#endif

// driver version
static const NDS_DRIVER_VERSION spi_driver_version = {NDS_SPI_API_VERSION, NDS_SPI_DRV_VERSION};

// SPI0
#if (RTE_SPI0)
static SPI_INFO spi0_info = {0};

#if (RTE_SPI0_DMA_TX_EN == 1)
void spi0_dma_tx_event(uint32_t event);
static SPI_DMA spi0_dma_tx = {RTE_SPI0_DMA_TX_CH, RTE_SPI0_DMA_TX_REQID, spi0_dma_tx_event};
#endif

#if (RTE_SPI0_DMA_RX_EN == 1)
void spi0_dma_rx_event(uint32_t event);
static SPI_DMA spi0_dma_rx = {RTE_SPI0_DMA_RX_CH, RTE_SPI0_DMA_RX_REQID, spi0_dma_rx_event};
#endif

static const SPI_RESOURCES spi0_resources = {
	{0},
	DEV_SPI1,
	IRQ_SPI1_SOURCE,
#if (RTE_SPI0_DMA_TX_EN == 1)
	&spi0_dma_tx,
#else
	NULL,
#endif
#if (RTE_SPI0_DMA_RX_EN == 1)
	&spi0_dma_rx,
#else
	NULL,
#endif
	&spi0_info
};
#endif

// SPI1
#if (RTE_SPI1)
static SPI_INFO spi1_info = {0};

#if (RTE_SPI1_DMA_TX_EN == 1)
void spi1_dma_tx_event(uint32_t event);
static SPI_DMA spi1_dma_tx = {RTE_SPI1_DMA_TX_CH, RTE_SPI1_DMA_TX_REQID, spi1_dma_tx_event};
#endif

#if (RTE_SPI1_DMA_RX_EN == 1)
void spi1_dma_rx_event(uint32_t event);
static SPI_DMA spi1_dma_rx = {RTE_SPI1_DMA_RX_CH, RTE_SPI1_DMA_RX_REQID, spi1_dma_rx_event};
#endif

static const SPI_RESOURCES spi1_resources = {
	{0},
	DEV_SPI2,
	IRQ_SPI2_SOURCE,
#if (RTE_SPI1_DMA_TX_EN == 1)
	&spi1_dma_tx,
#else
	NULL,
#endif
#if (RTE_SPI1_DMA_RX_EN == 1)
	&spi1_dma_rx,
#else
	NULL,
#endif
	&spi1_info
};
#endif

// SPI2
#if (RTE_SPI2)
static SPI_INFO spi2_info = {0};

#if (RTE_SPI2_DMA_TX_EN == 1)
void spi2_dma_tx_event(uint32_t event);
static SPI_DMA spi2_dma_tx = {RTE_SPI2_DMA_TX_CH, RTE_SPI2_DMA_TX_REQID, spi2_dma_tx_event};
#endif

#if (RTE_SPI2_DMA_RX_EN == 1)
void spi2_dma_rx_event(uint32_t event);
static SPI_DMA spi2_dma_rx = {RTE_SPI2_DMA_RX_CH, RTE_SPI2_DMA_RX_REQID, spi2_dma_rx_event};
#endif

static const SPI_RESOURCES spi2_resources = {
	{0},
	DEV_SPI3,
	IRQ_SPI3_SOURCE,
#if (RTE_SPI0_DMA_TX_EN == 1)
	&spi2_dma_tx,
#else
	NULL,
#endif
#if (RTE_SPI0_DMA_RX_EN == 1)
	&spi2_dma_rx,
#else
	NULL,
#endif
	&spi2_info
};
#endif

// SPI3
#if (RTE_SPI3)
static SPI_INFO spi3_info = {0};

#if (RTE_SPI3_DMA_TX_EN == 1)
void spi3_dma_tx_event(uint32_t event);
static SPI_DMA spi3_dma_tx = {RTE_SPI3_DMA_TX_CH, RTE_SPI3_DMA_TX_REQID, spi3_dma_tx_event};
#endif

#if (RTE_SPI3_DMA_RX_EN == 1)
void spi3_dma_rx_event(uint32_t event);
static SPI_DMA spi3_dma_rx = {RTE_SPI3_DMA_RX_CH, RTE_SPI3_DMA_RX_REQID, spi3_dma_rx_event};
#endif

static const SPI_RESOURCES spi3_resources = {
	{0},
	DEV_SPI4,
	IRQ_SPI4_SOURCE,
#if (RTE_SPI0_DMA_TX_EN == 1)
	&spi3_dma_tx,
#else
	NULL,
#endif
#if (RTE_SPI0_DMA_RX_EN == 1)
	&spi3_dma_rx,
#else
	NULL,
#endif
	&spi3_info
};
#endif

volatile uint8_t Delay_tmp[16] = {0};
void delay(uint64_t ms){
	uint64_t i;
	for (i = 0; i < (ms * CPUFREQ)/1000; ++i) {
		Delay_tmp[i % 16] = (i % 16);
	}
}


// local functions
static inline uint32_t spi_ip_revision_number(SPI_RESOURCES *spi) {
	return spi->reg->IDREV & (SPI_IDREV_MAJOR_MASK | SPI_IDREV_MINOR_MASK);
}

/* SPI_REVISION_NUM(major, minor): Generate revision number from major and minor number. */
#define SPI_REVISION_NUM(major, minor)  (((major << SPI_IDREV_MAJOR_BIT) & SPI_IDREV_MAJOR_MASK) | (minor & SPI_IDREV_MINOR_MASK))

/*
 * SPI200 v4.6 is original version
 * SPI200 v4.7 extends FIFO depth to 128, and add slave data-only transfer feature.
 */
#define SPI_IP_HAS_FIFO_DEPTH_128(spi)  (spi_ip_revision_number(spi) > SPI_REVISION_NUM(4, 6))

static uint32_t spi_get_txfifo_size(SPI_RESOURCES *spi) {
	if (SPI_IP_HAS_FIFO_DEPTH_128(spi)) {
		return 2 << ((spi->reg->CONFIG >> 4) & 0xf);
	}
	else {
		return 2 << ((spi->reg->CONFIG >> 4) & 0x3);
	}
}

static void spi_polling_spiactive(SPI_RESOURCES *spi) {
	while ((spi->reg->STATUS) & 0x1);
}

// SPI driver functions
static NDS_DRIVER_VERSION spi_get_version(void) {
	return spi_driver_version;
}

static NDS_SPI_CAPABILITIES spi_get_capabilities(SPI_RESOURCES *spi) {
	return spi->capabilities;
}

static int32_t spi_initialize(NDS_SPI_SignalEvent_t cb_event, SPI_RESOURCES *spi) {

	if (spi->info->flags & SPI_FLAG_INITIALIZED)
		return NDS_DRIVER_OK;

	// initialize SPI run-time resources
	spi->info->cb_event          = cb_event;

	spi->info->status.busy       = 0U;
	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->xfer.rx_buf       = 0U;
	spi->info->xfer.tx_buf       = 0U;
	spi->info->xfer.tx_buf_limit = 0U;
	spi->info->xfer.rx_cnt       = 0U;
	spi->info->xfer.tx_cnt       = 0U;

	spi->info->mode              = 0U;
	spi->info->txfifo_size = spi_get_txfifo_size(spi);

	spi->info->flags = SPI_FLAG_INITIALIZED;  // SPI is initialized

	spi->info->xfer.dma_tx_complete = 0;
	spi->info->xfer.dma_rx_complete = 0;

	return NDS_DRIVER_OK;
}

static int32_t spi_uninitialize(SPI_RESOURCES *spi) {

	spi->info->flags = 0U;                    // SPI is uninitialized

	return NDS_DRIVER_OK;
}

static int32_t spi_power_control(NDS_POWER_STATE state, SPI_RESOURCES *spi) {

	switch (state) {
		case NDS_POWER_OFF:
			// Disable PLIC interrupt SPI0 source
			__nds__plic_disable_interrupt(spi->irq_num);

			// DMA uninitialize
			if (spi->dma_tx || spi->dma_rx) {
				dma_uninitialize();

				if (spi->dma_tx && (spi->info->status.busy != 0))
					dma_channel_disable(spi->dma_tx->channel);

				if (spi->dma_rx && (spi->info->status.busy != 0))
					dma_channel_disable(spi->dma_rx->channel);
			}

			// reset SPI and TX/RX FIFOs
			spi->reg->CTRL = (TXFIFORST | RXFIFORST | SPIRST);

			// clear SPI run-time resources
			spi->info->status.busy       = 0U;
			spi->info->status.data_lost  = 0U;
			spi->info->status.mode_fault = 0U;

			spi->info->xfer.rx_buf       = 0U;
			spi->info->xfer.tx_buf       = 0U;
			spi->info->xfer.rx_cnt       = 0U;
			spi->info->xfer.tx_cnt       = 0U;

			spi->info->mode              = 0U;

			spi->info->flags &= ~SPI_FLAG_POWERED;

			break;

		case NDS_POWER_LOW:
			return NDS_DRIVER_ERROR_UNSUPPORTED;

		case NDS_POWER_FULL:
			if ((spi->info->flags & SPI_FLAG_INITIALIZED) == 0U)
				return NDS_DRIVER_ERROR;

			if ((spi->info->flags & SPI_FLAG_POWERED) != 0U)
				return NDS_DRIVER_OK;

			// DMA initialize
			if (spi->dma_tx || spi->dma_rx)
				dma_initialize();

			// reset SPI and TX/RX FIFOs
			spi->reg->CTRL = (TXFIFORST | RXFIFORST | SPIRST);

			// clear SPI run-time resources
			spi->info->status.busy       = 0U;
			spi->info->status.data_lost  = 0U;
			spi->info->status.mode_fault = 0U;

			spi->info->xfer.rx_buf       = 0U;
			spi->info->xfer.tx_buf       = 0U;
			spi->info->xfer.rx_cnt       = 0U;
			spi->info->xfer.tx_cnt       = 0U;

			spi->info->mode              = 0U;

			// Priority must be set > 0 to trigger the interrupt
			__nds__plic_set_priority(spi->irq_num, 1);

			// Enable PLIC interrupt SPI0 source
			__nds__plic_enable_interrupt(spi->irq_num);

			// Enable the Machine-External bit in MIE
			set_csr(NDS_MIE, MIP_MEIP);

			// Enable GIE
			set_csr(NDS_MSTATUS, MSTATUS_MIE);

			spi->info->flags |= SPI_FLAG_POWERED;   // SPI is powered

			break;

		default:
			return NDS_DRIVER_ERROR_UNSUPPORTED;
	}
	return NDS_DRIVER_OK;
}

//Sending the data one by one block
static uint32_t spi_block_send(SPI_RESOURCES *spi) {

	uint32_t Block_tx_num;
	int32_t stat;

	//Setting the delay time for waiting the slave ready
	if(spi->info->mode == NDS_SPI_MODE_MASTER){
		delay(DelayTime);
	}

	//Decide the transfer size of one block
	if (spi->info->block_num == 1){
		Block_tx_num = spi->info->data_num % BLOCK_SIZE;
	} else {
		Block_tx_num = BLOCK_SIZE;
	}

	if ((spi->info->mode & NDS_SPI_TRANSFER_FORMAT_Msk) != NDS_SPI_SLV_DATA_ONLY_TRANSFER) {
		// set transfer mode to write only and transfer count for write data
		spi->reg->TRANSCTRL = (SPI_TRANSMODE_WRONLY | WR_TRANCNT(Block_tx_num));
	} else {
		// SlvDataOnly mode should set TransMode to 0
		spi->reg->TRANSCTRL = (SPI_TRANSCTRL_SLV_DATA_ONLY | WR_TRANCNT(Block_tx_num));
	}

	if (spi->info->data_bits <= 8) { // data bits = 1....8)
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + Block_tx_num));
	} else if (spi->info->data_bits <= 16){ // data bits = 9....16
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + Block_tx_num * 2));
	} else { // data bits = 17....32
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + Block_tx_num * 4));
	}

	// DMA mode
	if (spi->dma_tx) {
		// enable TX DMA
		spi->reg->CTRL |= TXDMAEN;
		// configure DMA channel
		stat = dma_channel_configure(spi->dma_tx->channel,
					    (uint32_t)(long)spi->info->xfer.tx_buf,
					    (uint32_t)(long)(&(spi->reg->DATA)),
					    Block_tx_num,
					    DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)          |
					    DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)       |
					    DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)       |
					    DMA_CH_CTRL_DMODE_HANDSHAKE              |
					    DMA_CH_CTRL_SRCADDR_INC                  |
					    DMA_CH_CTRL_DSTADDR_FIX                  |
					    DMA_CH_CTRL_DSTREQ(spi->dma_tx->reqsel)  |
					    DMA_CH_CTRL_INTABT                       |
					    DMA_CH_CTRL_INTERR                       |
					    DMA_CH_CTRL_INTTC                        |
					    DMA_CH_CTRL_ENABLE,
					    spi->dma_tx->cb_event);
		if (stat == -1)
			return NDS_DRIVER_ERROR;

		// enable interrupts
		spi->reg->INTREN = SPI_ENDINT;

	} else { // interrupt mode

		// enable interrupts
		spi->reg->INTREN = (SPI_TXFIFOINT | SPI_ENDINT);

		// enable TX FIFO underrun interrupt when slave mode
		if ((spi->info->mode & NDS_SPI_CONTROL_Msk) == NDS_SPI_MODE_SLAVE)
			spi->reg->INTREN |= SPI_TXFIFOOURINT;

	}

	// trigger transfer when SPI master mode
	if ((spi->info->mode & NDS_SPI_CONTROL_Msk) == NDS_SPI_MODE_MASTER) {
		spi->reg->CMD = 0U;
	}

	return 0;
}

static int32_t spi_send(const void *data, uint32_t num, SPI_RESOURCES *spi) {

	if ((data == NULL) || (num == 0U))
		return NDS_DRIVER_ERROR_PARAMETER;

	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
		return NDS_DRIVER_ERROR;

	if (spi->info->status.busy)
		return NDS_DRIVER_ERROR_BUSY;

	// set busy flag
	spi->info->status.busy       = 1U;

	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->block_num = (uint32_t)(num/BLOCK_SIZE) + 1;
	spi->info->data_num = num;
	spi->info->xfer.tx_buf   = (uint8_t *)data;
	spi->info->xfer.tx_cnt   = 0U;

	spi->info->xfer.transfer_op = SPI_SEND;

	// wait prior transfer finish
	spi_polling_spiactive(spi);

	// set TX FIFO threshold to 2
	spi->reg->CTRL = TXTHRES(2);

	// prepare info that ISR needed
	spi->info->xfer.txfifo_refill = spi->info->txfifo_size - 2; // TX FIFO threshold = 2

	//call the block transfer
	spi_block_send(spi);

	return NDS_DRIVER_OK;
}

//Receiving the data one by one block
static uint32_t spi_block_receive(SPI_RESOURCES *spi) {

	uint32_t Block_rx_num;
	int32_t stat;

	//Setting the delay time for waiting the slave ready
	if(spi->info->mode == NDS_SPI_MODE_MASTER){
		delay(DelayTime);
	}

	//decide the transfer size of one block
	if (spi->info->block_num == 1) {
		Block_rx_num = spi->info->data_num % BLOCK_SIZE;
	} else {
		Block_rx_num = BLOCK_SIZE;
	}


	if ((spi->info->mode & NDS_SPI_TRANSFER_FORMAT_Msk) != NDS_SPI_SLV_DATA_ONLY_TRANSFER) {
		// set transfer mode to read only and transfer count for read data
		spi->reg->TRANSCTRL = (SPI_TRANSMODE_RDONLY | RD_TRANCNT(Block_rx_num));
	} else {
		// SlvDataOnly mode should set TransMode to 0
		spi->reg->TRANSCTRL = (SPI_TRANSCTRL_SLV_DATA_ONLY | RD_TRANCNT(Block_rx_num));
	}

	 // DMA mode
	 if (spi->dma_rx) {
		// enable RX DMA
		spi->reg->CTRL |= RXDMAEN;

		// configure DMA channel
		stat = dma_channel_configure(spi->dma_rx->channel,
					     (uint32_t)(long)(&(spi->reg->DATA)),
					     (uint32_t)(long)spi->info->xfer.rx_buf,
					     Block_rx_num,
					     DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)          |
					     DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)       |
					     DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)       |
					     DMA_CH_CTRL_SMODE_HANDSHAKE              |
					     DMA_CH_CTRL_SRCADDR_FIX                  |
					     DMA_CH_CTRL_DSTADDR_INC                  |
					     DMA_CH_CTRL_SRCREQ(spi->dma_rx->reqsel)  |
					     DMA_CH_CTRL_INTABT                       |
					     DMA_CH_CTRL_INTERR                       |
					     DMA_CH_CTRL_INTTC                        |
					     DMA_CH_CTRL_ENABLE,
					     spi->dma_rx->cb_event);

		if (stat == -1)
			return NDS_DRIVER_ERROR;

		// enable interrupts
		spi->reg->INTREN = SPI_ENDINT;
	} else {
		// enable interrupts
		spi->reg->INTREN = (SPI_RXFIFOINT | SPI_ENDINT);

		// enable RX FIFO overrun interrupt when slave mode
		if ((spi->info->mode & NDS_SPI_CONTROL_Msk) == NDS_SPI_MODE_SLAVE)
			spi->reg->INTREN |= SPI_RXFIFOOORINT;
	}

	// trigger transfer when SPI master mode
	if ((spi->info->mode & NDS_SPI_CONTROL_Msk) == NDS_SPI_MODE_MASTER) {
		spi->reg->CMD = 0U;
	}
	return 0;

}
static int32_t spi_receive(void *data, uint32_t num, SPI_RESOURCES *spi) {

	if ((data == NULL) || (num == 0U))
		return NDS_DRIVER_ERROR_PARAMETER;

	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
		return NDS_DRIVER_ERROR;

	if (spi->info->status.busy)
		return NDS_DRIVER_ERROR_BUSY;

	// set busy flag
	spi->info->status.busy       = 1U;

	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->block_num = (uint32_t)(num/BLOCK_SIZE) + 1;
	spi->info->data_num = num;
	spi->info->xfer.rx_buf = (uint8_t *)data;
	spi->info->xfer.rx_cnt = 0U;

	spi->info->xfer.transfer_op = SPI_RECEIVE;

	// wait prior transfer finish
	spi_polling_spiactive(spi);

	// set RX FIFO threshold to 2
	spi->reg->CTRL = RXTHRES(1);

	//call the block transfer
	spi_block_receive(spi);

	return NDS_DRIVER_OK;
}


//Transfering the data one by one block
static int32_t spi_block_transfer(SPI_RESOURCES *spi) {

	uint32_t Block_tx_num;
	int32_t stat;

	//Setting the delay time for waiting the slave ready
	if(spi->info->mode == NDS_SPI_MODE_MASTER){
		delay(DelayTime);
	}

	//Decide the transfer size of one block
	if (spi->info->block_num == 1){
		Block_tx_num = spi->info->data_num % BLOCK_SIZE;
	} else {
		Block_tx_num = BLOCK_SIZE;
	}

	if ((spi->info->mode & NDS_SPI_TRANSFER_FORMAT_Msk) != NDS_SPI_SLV_DATA_ONLY_TRANSFER) {
		// set transfer mode to write and read at the same time and transfer count for write/read data
		spi->reg->TRANSCTRL = (SPI_TRANSMODE_WRnRD | WR_TRANCNT(Block_tx_num) | RD_TRANCNT(Block_tx_num));
	} else {
		// SlvDataOnly mode should set TransMode to 0
		spi->reg->TRANSCTRL = (SPI_TRANSCTRL_SLV_DATA_ONLY | WR_TRANCNT(Block_tx_num) | RD_TRANCNT(Block_tx_num));
	}
	if (spi->info->data_bits <= 8) { // data bits = 1....8)

		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + Block_tx_num));
	} else if (spi->info->data_bits <= 16){ // data bits = 9....16
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + Block_tx_num * 2));
	} else { // data bits = 17....32
		spi->info->xfer.tx_buf_limit = (uint8_t *)((long)(spi->info->xfer.tx_buf + Block_tx_num * 4));
	}

	// DMA mode
	if (spi->dma_tx || spi->dma_rx) {
		if (spi->dma_tx) {
			// enable TX DMA
			spi->reg->CTRL |= TXDMAEN;

			// configure DMA channel
			stat = dma_channel_configure(spi->dma_tx->channel,
						      (uint32_t)(long)spi->info->xfer.tx_buf,
						      (uint32_t)(long)(&(spi->reg->DATA)),
						      Block_tx_num,
						      DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)          |
						      DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)       |
						      DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)       |
						      DMA_CH_CTRL_DMODE_HANDSHAKE              |
						      DMA_CH_CTRL_SRCADDR_INC                  |
						      DMA_CH_CTRL_DSTADDR_FIX                  |
						      DMA_CH_CTRL_DSTREQ(spi->dma_tx->reqsel)  |
						      DMA_CH_CTRL_INTABT                       |
						      DMA_CH_CTRL_INTERR                       |
						      DMA_CH_CTRL_INTTC                        |
						      DMA_CH_CTRL_ENABLE,
						      spi->dma_tx->cb_event);
			if (stat == -1)
				return NDS_DRIVER_ERROR;
		}

		if (spi->dma_rx) {
			// enable RX DMA
			spi->reg->CTRL |= RXDMAEN;

			// configure DMA channel
			stat = dma_channel_configure(spi->dma_rx->channel,
						     (uint32_t)(long)(&(spi->reg->DATA)),
						     (uint32_t)(long)spi->info->xfer.rx_buf,
						     Block_tx_num,
						     DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)          |
						     DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)       |
						     DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)       |
						     DMA_CH_CTRL_SMODE_HANDSHAKE              |
						     DMA_CH_CTRL_SRCADDR_FIX                  |
						     DMA_CH_CTRL_DSTADDR_INC                  |
						     DMA_CH_CTRL_SRCREQ(spi->dma_rx->reqsel)  |
						     DMA_CH_CTRL_INTABT                       |
						     DMA_CH_CTRL_INTERR                       |
						     DMA_CH_CTRL_INTTC                        |
						     DMA_CH_CTRL_ENABLE,
						     spi->dma_rx->cb_event);
			if (stat == -1)
				return NDS_DRIVER_ERROR;
		}

		// enable interrupts
		spi->reg->INTREN = SPI_ENDINT;

	} else { // interrupt mode

		// enable interrupts
		spi->reg->INTREN = (SPI_TXFIFOINT | SPI_RXFIFOINT | SPI_ENDINT);

		// enable TX FIFO underrun and RX FIFO overrun interrupt when slave mode
		if ((spi->info->mode & NDS_SPI_CONTROL_Msk) == NDS_SPI_MODE_SLAVE)
			spi->reg->INTREN |= (SPI_TXFIFOOURINT | SPI_RXFIFOOORINT);
	}

	// trigger transfer when SPI master mode
	if ((spi->info->mode & NDS_SPI_CONTROL_Msk) == NDS_SPI_MODE_MASTER) {
		spi->reg->CMD = 0U;
	}

	return 0;
}

static int32_t spi_transfer(const void *data_out, void *data_in, uint32_t num, SPI_RESOURCES *spi) {

	if ((data_out == NULL) || (data_in == NULL) || (num == 0U))
		return NDS_DRIVER_ERROR_PARAMETER;

	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
		return NDS_DRIVER_ERROR;

	if (spi->info->status.busy)
		return NDS_DRIVER_ERROR_BUSY;

	// set busy flag
	spi->info->status.busy       = 1U;

	spi->info->status.data_lost  = 0U;
	spi->info->status.mode_fault = 0U;

	spi->info->block_num = (uint32_t)(num/BLOCK_SIZE) + 1;
	spi->info->data_num = num;

	spi->info->xfer.rx_buf = (uint8_t *)data_out;
	spi->info->xfer.tx_buf = (uint8_t *)data_in;

	spi->info->xfer.rx_cnt = 0U;
	spi->info->xfer.tx_cnt = 0U;

	spi->info->xfer.transfer_op = SPI_TRANSFER;

	// wait prior transfer finish
	spi_polling_spiactive(spi);

	// set TX FIFO threshold and RX FIFO threshold to 2
	spi->reg->CTRL = (TXTHRES(2) | RXTHRES(1));

	// prepare info that ISR needed
	spi->info->xfer.txfifo_refill = spi->info->txfifo_size - 2; // TX FIFO threshold = 2

	spi_block_transfer(spi);

	return NDS_DRIVER_OK;
}

static int32_t spi_get_data_count(SPI_RESOURCES *spi) {

	if (!(spi->info->flags & SPI_FLAG_CONFIGURED))
		return 0U;

	switch (spi->info->xfer.transfer_op) {
		case SPI_SEND:
		case SPI_TRANSFER:
			if (spi->dma_tx)
				return (dma_channel_get_count(spi->dma_tx->channel));
			else
				return (spi->info->xfer.tx_cnt);
		case SPI_RECEIVE:
			if (spi->dma_rx)
				return (dma_channel_get_count(spi->dma_rx->channel));
			else
				return (spi->info->xfer.rx_cnt);
		default:
			return NDS_DRIVER_OK;
	}
}

static int32_t spi_control(uint32_t control, uint32_t arg, SPI_RESOURCES *spi) {
	uint32_t sclk_div;

	if (!(spi->info->flags & SPI_FLAG_POWERED))
		return NDS_DRIVER_ERROR;

	if ((control & NDS_SPI_CONTROL_Msk) == NDS_SPI_ABORT_TRANSFER) {    // abort SPI transfer
		// disable SPI interrupts
		spi->reg->INTREN = 0;

		// clear SPI run-time resources
		spi->info->status.busy = 0U;

		spi->info->xfer.rx_buf = 0U;
		spi->info->xfer.tx_buf = 0U;
		spi->info->xfer.rx_cnt = 0U;
		spi->info->xfer.tx_cnt = 0U;

		spi->info->mode        = 0U;

		return NDS_DRIVER_OK;
	}

	if (spi->info->status.busy)
		return NDS_DRIVER_ERROR_BUSY;

	switch (control & NDS_SPI_CONTROL_Msk) {
		default:
			return NDS_DRIVER_ERROR_UNSUPPORTED;

		case NDS_SPI_MODE_MASTER_SIMPLEX:  // SPI master (output/input on MOSI); arg = bus speed in bps
		case NDS_SPI_MODE_SLAVE_SIMPLEX:   // SPI slave (output/input on MISO
		case NDS_SPI_SET_DEFAULT_TX_VALUE: // set default Transmit value; arg = value
		case NDS_SPI_CONTROL_SS:           // control slave select; arg = 0:inactive, 1:active
			return NDS_SPI_ERROR_MODE;

		case NDS_SPI_MODE_INACTIVE: // SPI inactive
			spi->info->mode &= ~NDS_SPI_CONTROL_Msk;
			spi->info->mode |= NDS_SPI_MODE_INACTIVE;

			spi->info->flags &= ~SPI_FLAG_CONFIGURED;

			return NDS_DRIVER_OK;

		case NDS_SPI_MODE_MASTER: // SPI master (output on MOSI, input on MISO); arg = bus speed in bps
			// set master mode and disable data merge mode
			spi->reg->TRANSFMT &= ~(SPI_MERGE | SPI_SLAVE);

			spi->info->mode &= ~NDS_SPI_CONTROL_Msk;
			spi->info->mode |= NDS_SPI_MODE_MASTER;

			spi->info->flags |= SPI_FLAG_CONFIGURED;

			goto set_speed;

		case NDS_SPI_MODE_SLAVE: // SPI slave (output on MISO, input on MOSI)
			// set slave mode and disable data merge mode
			spi->reg->TRANSFMT &= ~SPI_MERGE;
			spi->reg->TRANSFMT |= SPI_SLAVE;

			spi->info->mode &= ~NDS_SPI_CONTROL_Msk;
			spi->info->mode |= NDS_SPI_MODE_SLAVE;

			spi->info->flags |= SPI_FLAG_CONFIGURED;

			break;

		case NDS_SPI_SET_BUS_SPEED: // set bus speed in bps; arg = value
			set_speed:
				if (arg == 0U)
					return NDS_DRIVER_ERROR;

				sclk_div = (SPI_CLK / (2 * arg)) - 1;

				spi->reg->TIMING &= ~0xff;
				spi->reg->TIMING |= sclk_div;

				if ((control & NDS_SPI_CONTROL_Msk) == NDS_SPI_SET_BUS_SPEED)
					return NDS_DRIVER_OK;

			break;

		case NDS_SPI_GET_BUS_SPEED: // get bus speed in bps
			sclk_div = spi->reg->TIMING & 0xff;
			return (SPI_CLK / ((sclk_div + 1) * 2));
	}

	// SPI slave select mode for master
	if ((spi->info->mode & NDS_SPI_CONTROL_Msk) ==  NDS_SPI_MODE_MASTER) {
		switch (control & NDS_SPI_SS_MASTER_MODE_Msk) {
			case NDS_SPI_SS_MASTER_HW_OUTPUT:
				break;
			case NDS_SPI_SS_MASTER_UNUSED:
			case NDS_SPI_SS_MASTER_SW:
			case NDS_SPI_SS_MASTER_HW_INPUT:
				return NDS_SPI_ERROR_SS_MODE;
			default:
				break;
		}
	}

	// SPI slave select mode for slave
	if ((spi->info->mode & NDS_SPI_CONTROL_Msk) == NDS_SPI_MODE_SLAVE) {
		switch (control & NDS_SPI_SS_SLAVE_MODE_Msk) {
			case NDS_SPI_SS_SLAVE_HW:
				break;
			case NDS_SPI_SS_SLAVE_SW:
				return NDS_SPI_ERROR_SS_MODE;
			default:
				break;
		}
	}

	// SPI data-only transfer mode for slave
	if ((spi->info->mode & NDS_SPI_CONTROL_Msk) == NDS_SPI_MODE_SLAVE) {
		if ((control & NDS_SPI_TRANSFER_FORMAT_Msk) == NDS_SPI_SLV_DATA_ONLY_TRANSFER) {
			spi->info->mode &= ~NDS_SPI_TRANSFER_FORMAT_Msk;
			spi->info->mode |= NDS_SPI_SLV_DATA_ONLY_TRANSFER;
		}
	}

	// set SPI frame format
	switch (control & NDS_SPI_FRAME_FORMAT_Msk) {
		case NDS_SPI_CPOL0_CPHA0:
			spi->reg->TRANSFMT &= ~(SPI_CPOL | SPI_CPHA);
			break;
		case NDS_SPI_CPOL0_CPHA1:
			spi->reg->TRANSFMT &= ~SPI_CPOL;
			spi->reg->TRANSFMT |= SPI_CPHA;
			break;
		case NDS_SPI_CPOL1_CPHA0:
			spi->reg->TRANSFMT |= SPI_CPOL;
			spi->reg->TRANSFMT &= ~SPI_CPHA;
			break;
		case NDS_SPI_CPOL1_CPHA1:
			spi->reg->TRANSFMT |= SPI_CPOL;
			spi->reg->TRANSFMT |= SPI_CPHA;
			break;
		case NDS_SPI_TI_SSI:
		case NDS_SPI_MICROWIRE:
			default:
			return NDS_SPI_ERROR_FRAME_FORMAT;
	}

	// set number of data bits
	spi->info->data_bits = ((control & NDS_SPI_DATA_BITS_Msk) >> NDS_SPI_DATA_BITS_Pos);

	if ((spi->info->data_bits < 1U) || (spi->info->data_bits > 32U))
		return NDS_SPI_ERROR_DATA_BITS;
	else
		spi->reg->TRANSFMT |= DATA_BITS(spi->info->data_bits);

	// set SPI bit order
	if ((control & NDS_SPI_BIT_ORDER_Msk) == NDS_SPI_LSB_MSB)
		spi->reg->TRANSFMT |= SPI_LSB;
	else
		spi->reg->TRANSFMT &= ~SPI_LSB;

	return NDS_DRIVER_OK;
}

static NDS_SPI_STATUS spi_get_status(SPI_RESOURCES *spi) {
	NDS_SPI_STATUS status;

	status.busy       = spi->info->status.busy;
	status.data_lost  = spi->info->status.data_lost;
	status.mode_fault = spi->info->status.mode_fault;

	return (status);
}

static void spi_irq_handler(SPI_RESOURCES *spi) {
	uint32_t i, j, status;
	uint32_t data = 0;
	uint32_t rx_num = 0;
	uint32_t event = 0;
	uint32_t have_clear_state = 0;


	// read status register
	status = spi->reg->INTRST;

	if ((status & SPI_RXFIFOOORINT) || (status & SPI_TXFIFOOURINT)) {
		// TX FIFO underrun or RX FIFO overrun interrupt status
		spi->info->status.data_lost = 1U;

		event |= NDS_SPI_EVENT_DATA_LOST;
	}

	if (status & SPI_TXFIFOINT) {
		for (i = 0; i < spi->info->xfer.txfifo_refill; i++) {
			data = 0;
			if (spi->info->xfer.tx_buf < spi->info->xfer.tx_buf_limit) {
				// handle the data frame format
				if (spi->info->data_bits <= 8) { // data bits = 1....8
					data = *spi->info->xfer.tx_buf;
					spi->info->xfer.tx_buf++;
				} else if (spi->info->data_bits <= 16) { // data bits = 9....16
					for (j = 0; j < 2; j++) {
						data |= *spi->info->xfer.tx_buf << j * 8;
						spi->info->xfer.tx_buf++;
					}
				} else { // data bits = 17....32
					for (j = 0; j < 4; j++) {
						data |= *spi->info->xfer.tx_buf << j * 8;
						spi->info->xfer.tx_buf++;
					}
				}
				spi->reg->DATA = data;
				spi->info->xfer.tx_cnt++;
			} else {
				spi->reg->INTREN &= ~SPI_TXFIFOINT;
			}
		}
	}

	if (status & SPI_RXFIFOINT) {

		// get number of valid entries in the RX FIFO
		if (SPI_IP_HAS_FIFO_DEPTH_128(spi)) {
			rx_num = (((spi->reg->STATUS >> 8) & 0x2f) | ((spi->reg->STATUS & (0x3 << 24)) >> 18));
		}
		else {
			rx_num = (spi->reg->STATUS >> 8) & 0x1f;
		}

		for (i = rx_num; i > 0; i--) {
			data = spi->reg->DATA;
			spi->info->xfer.rx_cnt++;

			// handle the data frame format
			if (spi->info->data_bits <= 8) {
				*spi->info->xfer.rx_buf = data & 0xff;
				spi->info->xfer.rx_buf++;
			} else if (spi->info->data_bits <= 16) {
				for (j = 0; j < 2; j++) {
					*spi->info->xfer.rx_buf = (data >> j * 8) & 0xff;
					spi->info->xfer.rx_buf++;
				}
			} else {
				for (j = 0; j < 4; j++) {
					*spi->info->xfer.rx_buf = (data >> j * 8) & 0xff;
					spi->info->xfer.rx_buf++;
				}
			}
		}
	}

	if (status & SPI_ENDINT) {
		// disable SPI interrupts
		spi->reg->INTREN = 0;
		spi->info->block_num--;

		if (spi->dma_tx) {
			if (!spi->info->xfer.dma_tx_complete)
				dma_channel_disable(spi->dma_tx->channel);

			if(spi->info->block_num){
				spi->info->xfer.tx_buf = spi->info->xfer.tx_buf + BLOCK_SIZE;
			}
			spi->info->xfer.dma_tx_complete = 0;
		}

		if (spi->dma_rx) {
			if (!spi->info->xfer.dma_rx_complete)
				dma_channel_disable(spi->dma_rx->channel);

			if(spi->info->block_num){
				spi->info->xfer.rx_buf = spi->info->xfer.rx_buf + BLOCK_SIZE;
			}
			spi->info->xfer.dma_rx_complete = 0;
		}

		// clear TX/RX FIFOs
		spi->reg->CTRL |= (TXFIFORST | RXFIFORST );

		//If there is no block need to be transfered, meaning the transfer completed and spi not busy.
		if (spi->info->block_num == 0){
			spi->info->status.busy = 0U;
			event |= NDS_SPI_EVENT_TRANSFER_COMPLETE;

		//If there is block need to be transfer,call the block transfer function again.
		} else if (spi->info->xfer.transfer_op == SPI_SEND){
			spi->reg->INTRST = status;
			spi->reg->INTRST;
			have_clear_state = 1;
			spi_block_send(spi);

		} else if (spi->info->xfer.transfer_op == SPI_RECEIVE){
			spi->reg->INTRST = status;
			spi->reg->INTRST;
			have_clear_state = 1;
			spi_block_receive(spi);
		} else if (spi->info->xfer.transfer_op == SPI_TRANSFER){
			spi->reg->INTRST = status;
			spi->reg->INTRST;
			have_clear_state = 1;
			spi_block_transfer(spi);
		}
	}

	if(have_clear_state == 0){
		// clear interrupt status
		spi->reg->INTRST = status;
		// make sure "write 1 clear" take effect before iret
		spi->reg->INTRST;
	}

	if ((spi->info->cb_event != NULL) && (event != 0)){
		spi->info->cb_event(event);
	}
}

#if (((RTE_SPI0) && (RTE_SPI0_DMA_TX_EN == 1)) || \
     ((RTE_SPI1) && (RTE_SPI1_DMA_TX_EN == 1)) || \
     ((RTE_SPI2) && (RTE_SPI2_DMA_TX_EN == 1)) || \
     ((RTE_SPI3) && (RTE_SPI3_DMA_TX_EN == 1)))
static void spi_dma_tx_event (uint32_t event, SPI_RESOURCES * spi) {
	switch (event) {
		case DMA_EVENT_TERMINAL_COUNT_REQUEST:
			spi->info->xfer.dma_tx_complete = 1;
			break;
		case DMA_EVENT_ERROR:
		default:
			break;
	}
}
#endif

#if (((RTE_SPI0) && (RTE_SPI0_DMA_RX_EN == 1)) || \
     ((RTE_SPI1) && (RTE_SPI1_DMA_TX_EN == 1)) || \
     ((RTE_SPI2) && (RTE_SPI2_DMA_TX_EN == 1)) || \
     ((RTE_SPI3) && (RTE_SPI3_DMA_TX_EN == 1)))
static void spi_dma_rx_event (uint32_t event, SPI_RESOURCES * spi) {
	switch (event) {
		case DMA_EVENT_TERMINAL_COUNT_REQUEST:
			spi->info->xfer.dma_rx_complete = 1;
			break;
		case DMA_EVENT_ERROR:
		default:
			break;
	}
}
#endif

#if (RTE_SPI0)
// SPI0 driver wrapper functions
static NDS_SPI_CAPABILITIES spi0_get_capabilities(void) {
	return spi_get_capabilities(&spi0_resources);
}

static int32_t spi0_initialize(NDS_SPI_SignalEvent_t cb_event) {
	return spi_initialize(cb_event, &spi0_resources);
}

static int32_t spi0_uninitialize (void) {
	return spi_uninitialize(&spi0_resources);
}

static int32_t spi0_power_control(NDS_POWER_STATE state) {
	return spi_power_control(state, &spi0_resources);
}

static int32_t spi0_send(const void *data, uint32_t num) {
	return spi_send(data, num, &spi0_resources);
}

static int32_t spi0_receive(void *data, uint32_t num) {
	return spi_receive(data, num, &spi0_resources);
}

static int32_t spi0_transfer(const void *data_out, void *data_in, uint32_t num) {
	return spi_transfer(data_out, data_in, num, &spi0_resources);
}

static uint32_t spi0_get_data_count(void) {
	return spi_get_data_count(&spi0_resources);
}

static int32_t spi0_control(uint32_t control, uint32_t arg) {
	return spi_control(control, arg, &spi0_resources);
}

static NDS_SPI_STATUS spi0_get_status(void) {
	return spi_get_status(&spi0_resources);
}

/*-----------*/
#define SPI0_irq_handler	spi1_irq_handler
void SPI0_irq_handler(void) {
	spi_irq_handler(&spi0_resources);
}

#if (RTE_SPI0_DMA_TX_EN == 1)
void spi0_dma_tx_event (uint32_t event) {
	spi_dma_tx_event(event, &spi0_resources);
}
#endif

#if (RTE_SPI0_DMA_RX_EN == 1)
void spi0_dma_rx_event (uint32_t event) {
	spi_dma_rx_event(event, &spi0_resources);
}
#endif

// SPI0 driver control block
NDS_DRIVER_SPI Driver_SPI0 = {
	spi_get_version,
	spi0_get_capabilities,
	spi0_initialize,
	spi0_uninitialize,
	spi0_power_control,
	spi0_send,
	spi0_receive,
	spi0_transfer,
	spi0_get_data_count,
	spi0_control,
	spi0_get_status
};
#endif

#if (RTE_SPI1)
// SPI1 driver wrapper functions
static NDS_SPI_CAPABILITIES spi1_get_capabilities(void) {
	return spi_get_capabilities(&spi1_resources);
}

static int32_t spi1_initialize(NDS_SPI_SignalEvent_t cb_event) {
	return spi_initialize(cb_event, &spi1_resources);
}

static int32_t spi1_uninitialize (void) {
	return spi_uninitialize(&spi1_resources);
}

static int32_t spi1_power_control(NDS_POWER_STATE state) {
	return spi_power_control(state, &spi1_resources);
}

static int32_t spi1_send(const void *data, uint32_t num) {
	return spi_send(data, num, &spi1_resources);
}

static int32_t spi1_receive(void *data, uint32_t num) {
	return spi_receive(data, num, &spi1_resources);
}

static int32_t spi1_transfer(const void *data_out, void *data_in, uint32_t num) {
	return spi_transfer(data_out, data_in, num, &spi1_resources);
}

static uint32_t spi1_get_data_count(void) {
	return spi_get_data_count(&spi1_resources);
}

static int32_t spi1_control(uint32_t control, uint32_t arg) {
	return spi_control(control, arg, &spi1_resources);
}

static NDS_SPI_STATUS spi1_get_status(void) {
	return spi_get_status(&spi1_resources);
}

/*----------*/
#define SPI1_irq_handler       spi2_irq_handler
void SPI1_irq_handler(void) {
	spi_irq_handler(&spi1_resources);
}

#if (RTE_SPI1_DMA_TX_EN == 1)
void spi1_dma_tx_event (uint32_t event) {
	spi_dma_tx_event(event, &spi1_resources);
}
#endif

#if (RTE_SPI1_DMA_RX_EN == 1)
void spi1_dma_rx_event (uint32_t event) {
	spi_dma_rx_event(event, &spi1_resources);
}
#endif

// SPI1 driver control block
NDS_DRIVER_SPI Driver_SPI1 = {
	spi_get_version,
	spi1_get_capabilities,
	spi1_initialize,
	spi1_uninitialize,
	spi1_power_control,
	spi1_send,
	spi1_receive,
	spi1_transfer,
	spi1_get_data_count,
	spi1_control,
	spi1_get_status
};
#endif

#if (RTE_SPI2)
// SPI2 driver wrapper functions
static NDS_SPI_CAPABILITIES spi2_get_capabilities(void) {
	return spi_get_capabilities(&spi0_resources);
}

static int32_t spi2_initialize(NDS_SPI_SignalEvent_t cb_event) {
	return spi_initialize(cb_event, &spi2_resources);
}

static int32_t spi2_uninitialize (void) {
	return spi_uninitialize(&spi2_resources);
}

static int32_t spi2_power_control(NDS_POWER_STATE state) {
	return spi_power_control(state, &spi2_resources);
}

static int32_t spi2_send(const void *data, uint32_t num) {
	return spi_send(data, num, &spi2_resources);
}

static int32_t spi2_receive(void *data, uint32_t num) {
	return spi_receive(data, num, &spi2_resources);
}

static int32_t spi2_transfer(const void *data_out, void *data_in, uint32_t num) {
	return spi_transfer(data_out, data_in, num, &spi2_resources);
}

static uint32_t spi2_get_data_count(void) {
	return spi_get_data_count(&spi2_resources);
}

static int32_t spi2_control(uint32_t control, uint32_t arg) {
	return spi_control(control, arg, &spi2_resources);
}

static NDS_SPI_STATUS spi2_get_status(void) {
	return spi_get_status(&spi2_resources);
}

/*-----------*/
#define SPI2_irq_handler	spi3_irq_handler
void SPI2_irq_handler(void) {
	spi_irq_handler(&spi2_resources);
}

#if (RTE_SPI2_DMA_TX_EN == 1)
void spi2_dma_tx_event (uint32_t event) {
	spi_dma_tx_event(event, &spi2_resources);
}
#endif

#if (RTE_SPI2_DMA_RX_EN == 1)
void spi2_dma_rx_event (uint32_t event) {
	spi_dma_rx_event(event, &spi2_resources);
}
#endif

// SPI2 driver control block
NDS_DRIVER_SPI Driver_SPI2 = {
	spi_get_version,
	spi2_get_capabilities,
	spi2_initialize,
	spi2_uninitialize,
	spi2_power_control,
	spi2_send,
	spi2_receive,
	spi2_transfer,
	spi2_get_data_count,
	spi2_control,
	spi2_get_status
};
#endif


#if (RTE_SPI3)
// SPI3 driver wrapper functions
static NDS_SPI_CAPABILITIES spi3_get_capabilities(void) {
	return spi_get_capabilities(&spi3_resources);
}

static int32_t spi3_initialize(NDS_SPI_SignalEvent_t cb_event) {
	return spi_initialize(cb_event, &spi3_resources);
}

static int32_t spi3_uninitialize (void) {
	return spi_uninitialize(&spi3_resources);
}

static int32_t spi3_power_control(NDS_POWER_STATE state) {
	return spi_power_control(state, &spi3_resources);
}

static int32_t spi3_send(const void *data, uint32_t num) {
	return spi_send(data, num, &spi3_resources);
}

static int32_t spi3_receive(void *data, uint32_t num) {
	return spi_receive(data, num, &spi3_resources);
}

static int32_t spi3_transfer(const void *data_out, void *data_in, uint32_t num) {
	return spi_transfer(data_out, data_in, num, &spi3_resources);
}

static uint32_t spi3_get_data_count(void) {
	return spi_get_data_count(&spi3_resources);
}

static int32_t spi3_control(uint32_t control, uint32_t arg) {
	return spi_control(control, arg, &spi3_resources);
}

static NDS_SPI_STATUS spi3_get_status(void) {
	return spi_get_status(&spi3_resources);
}

/*-----------*/
#define SPI3_irq_handler	spi4_irq_handler
void SPI3_irq_handler(void) {
	spi_irq_handler(&spi3_resources);
}

#if (RTE_SPI3_DMA_TX_EN == 1)
void spi3_dma_tx_event (uint32_t event) {
	spi_dma_tx_event(event, &spi3_resources);
}
#endif

#if (RTE_SPI3_DMA_RX_EN == 1)
void spi3_dma_rx_event (uint32_t event) {
	spi_dma_rx_event(event, &spi3_resources);
}
#endif

// SPI3 driver control block
NDS_DRIVER_SPI Driver_SPI3 = {
	spi_get_version,
	spi3_get_capabilities,
	spi3_initialize,
	spi3_uninitialize,
	spi3_power_control,
	spi3_send,
	spi3_receive,
	spi3_transfer,
	spi3_get_data_count,
	spi3_control,
	spi3_get_status
};
#endif
