/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "usart_ae250.h"

#include "RTE_Device.h"

#define NDS_USART_DRV_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if ((!RTE_UART0) && (!RTE_UART1))
	#error "UART not enabled in RTE_Device.h!"
#endif

// Driver Version
static const NDS_DRIVER_VERSION usart_driver_version = { NDS_USART_API_VERSION, NDS_USART_DRV_VERSION };

// Trigger level definitions
// Can be user defined by C preprocessor
#ifndef UART0_TRIG_LVL
#define UART0_TRIG_LVL            USART_TRIG_LVL_1
#endif
#ifndef UART1_TRIG_LVL
#define UART1_TRIG_LVL            USART_TRIG_LVL_1
#endif

// UART0
#if (RTE_UART0)
static USART_INFO usart0_info = {0};

#if (RTE_UART0_DMA_TX_EN == 1)
void usart0_dma_tx_event (uint32_t event);
static USART_DMA usart0_dma_tx = {RTE_UART0_DMA_TX_CH,
                                  RTE_UART0_DMA_TX_REQID,
                                  usart0_dma_tx_event};
#endif
#if (RTE_UART0_DMA_RX_EN == 1)
void usart0_dma_rx_event (uint32_t event);
static USART_DMA usart0_dma_rx = {RTE_UART0_DMA_RX_CH,
                                  RTE_UART0_DMA_RX_REQID,
                                  usart0_dma_rx_event};
#endif

static const USART_RESOURCES usart0_resources = {
	{   // Capabilities
	1,  // supports UART (Asynchronous) mode
	0,  // supports Synchronous Master mode
	0,  // supports Synchronous Slave mode
	0,  // supports UART Single-wire mode
	0,  // supports UART IrDA mode
	0,  // supports UART Smart Card mode
	0,
	0,  // RTS Flow Control available
	0,  // CTS Flow Control available
	0,  // Transmit completed event: \ref NDS_USART_EVENT_TX_COMPLETE
#if ((RTE_UART0_DMA_RX_EN == 1) || (UART0_TRIG_LVL == USART_TRIG_LVL_1))
	0,  // Signal receive character timeout event: \ref NDS_USART_EVENT_RX_TIMEOUT
#else
	1,  // Signal receive character timeout event: \ref NDS_USART_EVENT_RX_TIMEOUT
#endif
	0,  // RTS Line: 0=not available, 1=available
	0,  // CTS Line: 0=not available, 1=available
	0,  // DTR Line: 0=not available, 1=available
	0,  // DSR Line: 0=not available, 1=available
	0,  // DCD Line: 0=not available, 1=available
	0,  // RI Line: 0=not available, 1=available
	0,  // Signal CTS change event: \ref NDS_USART_EVENT_CTS
	0,  // Signal DSR change event: \ref NDS_USART_EVENT_DSR
	0,  // Signal DCD change event: \ref NDS_USART_EVENT_DCD
	0,  // Signal RI change event: \ref NDS_USART_EVENT_RI
	},
	DEV_UART1,
	IRQ_UART1_SOURCE,
	UART0_TRIG_LVL,
#if (RTE_UART0_DMA_TX_EN == 1)
	&usart0_dma_tx,
#else
	NULL,
#endif
#if (RTE_UART0_DMA_RX_EN == 1)
	&usart0_dma_rx,
#else
	NULL,
#endif
	&usart0_info
};
#endif

// UART1
#if (RTE_UART1)
static USART_INFO usart1_info = {0};

#if (RTE_UART1_DMA_TX_EN == 1)
void usart1_dma_tx_event (uint32_t event);
static USART_DMA usart1_dma_tx = {RTE_UART1_DMA_TX_CH,
                                  RTE_UART1_DMA_TX_REQID,
                                  usart1_dma_tx_event};
#endif
#if (RTE_UART1_DMA_RX_EN == 1)
void usart1_dma_rx_event (uint32_t event);
static USART_DMA usart1_dma_rx = {RTE_UART1_DMA_RX_CH,
                                  RTE_UART1_DMA_RX_REQID,
                                  usart1_dma_rx_event};
#endif

static const USART_RESOURCES usart1_resources = {
	{   // Capabilities
	1,  // supports UART (Asynchronous) mode
	0,  // supports Synchronous Master mode
	0,  // supports Synchronous Slave mode
	0,  // supports UART Single-wire mode
	0,  // supports UART IrDA mode
	0,  // supports UART Smart Card mode
	0,  // Smart Card Clock generator
	0,  // RTS Flow Control available
	0,  // CTS Flow Control available
	0,  // Transmit completed event: \ref NDS_USART_EVENT_TX_COMPLETE
#if ((RTE_UART1_DMA_RX_EN == 1) || (UART1_TRIG_LVL == USART_TRIG_LVL_1))
	0,  // Signal receive character timeout event: \ref NDS_USART_EVENT_RX_TIMEOUT
#else
	1,  // Signal receive character timeout event: \ref NDS_USART_EVENT_RX_TIMEOUT
#endif
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	},
	DEV_UART2,
	IRQ_UART2_SOURCE,
	UART1_TRIG_LVL,
#if (RTE_UART1_DMA_TX_EN == 1)
	&usart1_dma_tx,
#else
	NULL,
#endif
#if (RTE_UART1_DMA_RX_EN == 1)
	&usart1_dma_rx,
#else
	NULL,
#endif
	&usart1_info
};
#endif


// Local Function
/**
  \fn          int32_t usart_set_baudrate (uint32_t         baudrate,
                                           USART_RESOURCES *usart)
  \brief       Set baudrate dividers
  \param[in]   baudrate  Usart baudrate
  \param[in]   usart     Pointer to USART resources)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t usart_set_baudrate (	uint32_t         baudrate,
				USART_RESOURCES *usart) {

	uint32_t div;

	// Oversampling is fixed to 16
	// divider = frequency of uclk / (desired baud rate ? oversampling)
	div = (UCLKFREQ / baudrate / 16);

	usart->reg->LCR = 0;
	usart->reg->LCR = 3;
	usart->reg->LCR |= UARTC_LCR_DLAB;
	usart->reg->DLL  = (div >> 0) & 0xff;
	usart->reg->DLM  = (div >> 8) & 0xff;
	// Reset DLAB bit
	usart->reg->LCR &= (~UARTC_LCR_DLAB);

	usart->info->baudrate = baudrate;

	return 0;
}

/**
  \fn          uint32_t usart_rxline_irq_handler (USART_RESOURCES *usart)
  \brief       Receive line interrupt handler
  \param[in]   usart     Pointer to USART resources
  \return      Rx Line event mask
*/
static uint32_t usart_rxline_irq_handler (USART_RESOURCES *usart) {

	uint32_t lsr, event;

	#define UARTC_LSR_LINE_INT      (UARTC_LSR_OE | UARTC_LSR_PE | UARTC_LSR_FE | UARTC_LSR_BI)
	event = 0U;
	lsr   = usart->reg->LSR  & UARTC_LSR_LINE_INT;

	// OverRun error
	if (lsr & UARTC_LSR_OE) {
		usart->info->rx_status.rx_overflow = 1U;
		event |= NDS_USART_EVENT_RX_OVERFLOW;
	}

	// Parity error
	if (lsr & UARTC_LSR_PE) {
		usart->info->rx_status.rx_parity_error = 1U;
		event |= NDS_USART_EVENT_RX_PARITY_ERROR;
	}

	// Break detected
	if (lsr & UARTC_LSR_BI) {
		usart->info->rx_status.rx_break = 1U;
		event |= NDS_USART_EVENT_RX_BREAK;
	// Framing error
	} else {
		if (lsr & UARTC_LSR_FE) {
			usart->info->rx_status.rx_framing_error = 1U;
			event |= NDS_USART_EVENT_RX_FRAMING_ERROR;
		}
	}

	return event;
}

// Function Prototypes
static int32_t usart_receive (	void            *data,
				uint32_t         num,
				USART_RESOURCES *usart);


// USART Driver functions

/**
  \fn          NDS_DRIVER_VERSION uart_get_version (void)
  \brief       Get driver version.
  \return      \ref NDS_DRIVER_VERSION
*/
static NDS_DRIVER_VERSION usart_get_version (void) {

	return usart_driver_version;
}

/**
  \fn          NDS_USART_CAPABILITIES usart_get_capabilities (USART_RESOURCES *usart)
  \brief       Get driver capabilities
  \param[in]   usart     Pointer to USART resources
  \return      \ref NDS_USART_CAPABILITIES
*/
static NDS_USART_CAPABILITIES usart_get_capabilities (USART_RESOURCES *usart) {

	return usart->capabilities;
}

/**
  \fn          int32_t usart_initialize (NDS_USART_SignalEvent_t  cb_event
                                         USART_RESOURCES         *usart)
  \brief       Initialize USART Interface.
  \param[in]   cb_event  Pointer to \ref NDS_USART_SignalEvent
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t usart_initialize (	NDS_USART_SignalEvent_t  cb_event,
					USART_RESOURCES         *usart) {

	if (usart->info->flags & USART_FLAG_INITIALIZED) {
		// Driver is already initialized
		return NDS_DRIVER_OK;
	}

	// Initialize USART Run-time Resources
	usart->info->cb_event = cb_event;

	usart->info->rx_status.rx_busy          = 0U;
	usart->info->rx_status.rx_overflow      = 0U;
	usart->info->rx_status.rx_break         = 0U;
	usart->info->rx_status.rx_framing_error = 0U;
	usart->info->rx_status.rx_parity_error  = 0U;

	usart->info->xfer.send_active           = 0U;
	usart->info->xfer.tx_def_val            = 0U;

	// DMA Initialize
	if (usart->dma_tx || usart->dma_rx) {
		dma_initialize ();
	}

	usart->info->flags = USART_FLAG_INITIALIZED;

	return NDS_DRIVER_OK;
}

/**
  \fn          int32_t usart_uninitialize (USART_RESOURCES *usart)
  \brief       De-initialize USART Interface.
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t usart_uninitialize (USART_RESOURCES *usart) {

	// DMA Uninitialize
	if (usart->dma_tx || usart->dma_rx) {
		dma_uninitialize ();
	}

	// Reset USART status flags
	usart->info->flags = 0U;

	return NDS_DRIVER_OK;
}

/**
  \fn          int32_t usart_power_control (NDS_POWER_STATE state)
  \brief       Control USART Interface Power.
  \param[in]   state  Power state
  \param[in]   usart  Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t usart_power_control (	NDS_POWER_STATE  state,
					USART_RESOURCES *usart) {

	uint32_t val;

	switch (state) {
	case NDS_POWER_OFF:
		// Disable USART IRQ
		__nds__plic_disable_interrupt(usart->irq_num);

		// If DMA mode - disable TX DMA channel
		if ((usart->dma_tx) && (usart->info->xfer.send_active != 0U)) {
			dma_channel_disable (usart->dma_tx->channel);
		}

		// If DMA mode - disable DMA channel
		if ((usart->dma_rx) && (usart->info->rx_status.rx_busy)) {
			dma_channel_disable (usart->dma_rx->channel);
		}

		// Clear driver variables
		usart->info->rx_status.rx_busy          = 0U;
		usart->info->rx_status.rx_overflow      = 0U;
		usart->info->rx_status.rx_break         = 0U;
		usart->info->rx_status.rx_framing_error = 0U;
		usart->info->rx_status.rx_parity_error  = 0U;
		usart->info->xfer.send_active           = 0U;

		usart->info->flags &= ~USART_FLAG_POWERED;
		break;

	case NDS_POWER_LOW:
		return NDS_DRIVER_ERROR_UNSUPPORTED;

	case NDS_POWER_FULL:
		if ((usart->info->flags & USART_FLAG_INITIALIZED) == 0U) {
			return NDS_DRIVER_ERROR;
		}

		if ((usart->info->flags & USART_FLAG_POWERED) != 0U) {
			return NDS_DRIVER_OK;
		}

		// Disable interrupts
		usart->reg->IER = 0U;

		// Configure FIFO Control register
		// Set trigger level
		val = (usart->trig_lvl & UARTC_FCR_RXFIFO_TRGL_MASK) | UARTC_FCR_FIFO_EN;

		if (usart->dma_rx || usart->dma_tx) {
			val |= UARTC_FCR_DMA_EN;
		}

		usart->reg->FCR = val;

		// Enable modem lines status interrupts (only UART1)
		if (	usart->capabilities.cts || usart->capabilities.dcd ||
			usart->capabilities.dsr || usart->capabilities.ri) {
			usart->reg->IER |= UARTC_IER_MS;
		}

		// Clear driver variables
		usart->info->rx_status.rx_busy          = 0U;
		usart->info->rx_status.rx_overflow      = 0U;
		usart->info->rx_status.rx_break         = 0U;
		usart->info->rx_status.rx_framing_error = 0U;
		usart->info->rx_status.rx_parity_error  = 0U;

		usart->info->mode                       = 0U;
		usart->info->flags                      = 0U;
		usart->info->xfer.send_active           = 0U;

		usart->info->flags = USART_FLAG_POWERED | USART_FLAG_INITIALIZED;

		// Priority must be set > 0 to trigger the interrupt
		__nds__plic_set_priority(usart->irq_num, 1);

		// Enable PLIC interrupt PIT source
		__nds__plic_enable_interrupt(usart->irq_num);

		// Enable the Machine-External bit in MIE
		set_csr(NDS_MIE, MIP_MEIP);

		// Enable GIE
		set_csr(NDS_MSTATUS, MSTATUS_MIE);

		break;

	default:
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	return NDS_DRIVER_OK;
}

/**
  \fn          int32_t usart_send (const void            *data,
                                         uint32_t         num,
                                         USART_RESOURCES *usart)
  \brief       Start sending data to USART transmitter.
  \param[in]   data  Pointer to buffer with data to send to USART transmitter
  \param[in]   num   Number of data items to send
  \param[in]   usart Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t usart_send (	const void	*data,
				uint32_t         num,
				USART_RESOURCES *usart) {

	int32_t stat, val;

	if ((data == NULL) || (num == 0U)) {
		// Invalid parameters
		return NDS_DRIVER_ERROR_PARAMETER;
	}

	if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
		// USART is not configured (mode not selected)
		return NDS_DRIVER_ERROR;
	}

	if (usart->info->xfer.send_active != 0U) {
		// Send is not completed yet
		return NDS_DRIVER_ERROR_BUSY;
	}

	// Set Send active flag
	usart->info->xfer.send_active = 1U;

	// Save transmit buffer info
	usart->info->xfer.tx_buf = (uint8_t *)data;
	usart->info->xfer.tx_num = num;
	usart->info->xfer.tx_cnt = 0U;

	// DMA mode
	if (usart->dma_tx) {

		// Configure DMA channel
		stat = dma_channel_configure (	usart->dma_tx->channel,
						(uint32_t)(long)data,
						(uint32_t)(long)(&(usart->reg->THR)),
						num,
						DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)			|
						DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)		|
						DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)		|
						DMA_CH_CTRL_DMODE_HANDSHAKE			|
						DMA_CH_CTRL_SRCADDR_INC				|
						DMA_CH_CTRL_DSTADDR_FIX				|
						DMA_CH_CTRL_DSTREQ(usart->dma_tx->reqsel)	|
						DMA_CH_CTRL_INTABT				|
						DMA_CH_CTRL_INTERR				|
						DMA_CH_CTRL_INTTC				|
						DMA_CH_CTRL_ENABLE,
						usart->dma_tx->cb_event);

		if (stat == -1) {
			return NDS_DRIVER_ERROR;
		}

	// Interrupt mode
	} else {
		// Fill TX FIFO
		if (usart->reg->LSR & UARTC_LSR_THRE) {
			val = 16U;
			while ((val--) && (usart->info->xfer.tx_cnt != usart->info->xfer.tx_num)) {
				usart->reg->THR = usart->info->xfer.tx_buf[usart->info->xfer.tx_cnt++];
			}
		}

		// Enable transmit holding register empty interrupt
		usart->reg->IER |= UARTC_IER_THRE;
	}

	return NDS_DRIVER_OK;
}

/**
  \fn          int32_t usart_receive (void            *data,
                                      uint32_t         num,
                                      USART_RESOURCES *usart)
  \brief       Start receiving data from USART receiver.
  \param[out]  data  Pointer to buffer for data to receive from USART receiver
  \param[in]   num   Number of data items to receive
  \param[in]   usart Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t usart_receive (	void            *data,
				uint32_t         num,
				USART_RESOURCES *usart) {

	int32_t stat;

	if ((data == NULL) || (num == 0U)) {
		// Invalid parameters
		return NDS_DRIVER_ERROR_PARAMETER;
	}

	if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
		// USART is not configured (mode not selected)
		return NDS_DRIVER_ERROR;
	}

	// Check if receiver is busy
	if (usart->info->rx_status.rx_busy == 1U) {
		return NDS_DRIVER_ERROR_BUSY;
	}

	// Set RX busy flag
	usart->info->rx_status.rx_busy = 1U;

	// Save number of data to be received
	usart->info->xfer.rx_num = num;

	// Clear RX statuses
	usart->info->rx_status.rx_break          = 0U;
	usart->info->rx_status.rx_framing_error  = 0U;
	usart->info->rx_status.rx_overflow       = 0U;
	usart->info->rx_status.rx_parity_error   = 0U;

	// Save receive buffer info
	usart->info->xfer.rx_buf = (uint8_t *)data;
	usart->info->xfer.rx_cnt =              0U;

	// DMA mode
	if (usart->dma_rx) {

		stat = dma_channel_configure (	usart->dma_rx->channel,
						(uint32_t)(long)&usart->reg->RBR,
						(uint32_t)(long)data,
						num,
						DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)			|
						DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)		|
						DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)		|
						DMA_CH_CTRL_SMODE_HANDSHAKE			|
						DMA_CH_CTRL_SRCADDR_FIX				|
						DMA_CH_CTRL_DSTADDR_INC				|
						DMA_CH_CTRL_SRCREQ(usart->dma_rx->reqsel)	|
						DMA_CH_CTRL_INTABT				|
						DMA_CH_CTRL_INTERR				|
						DMA_CH_CTRL_INTTC				|
						DMA_CH_CTRL_ENABLE,
						usart->dma_rx->cb_event);

		if (stat == -1) {
			return NDS_DRIVER_ERROR;
		}

	// Interrupt mode
	} else {
		// Enable receive data available interrupt
		usart->reg->IER |= UARTC_IER_RDR;
	}

	return NDS_DRIVER_OK;
}

/**
  \fn          int32_t usart_transfer (const void             *data_out,
                                             void             *data_in,
                                             uint32_t          num,
                                             USART_RESOURCES  *usart)
  \brief       Start sending/receiving data to/from USART transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to USART transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from USART receiver
  \param[in]   num       Number of data items to transfer
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t usart_transfer (	const void	*data_out,
				void		*data_in,
				uint32_t	num,
				USART_RESOURCES	*usart) {

	if ((data_out == NULL) || (data_in == NULL) || (num == 0U)) {
		// Invalid parameters
		return NDS_DRIVER_ERROR_PARAMETER;
	}

	if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
		// USART is not configured
		return NDS_DRIVER_ERROR;
	}

	// Only in synchronous mode
	return NDS_DRIVER_ERROR;
}

/**
  \fn          uint32_t usart_get_txcount (USART_RESOURCES *usart)
  \brief       Get transmitted data count.
  \param[in]   usart     Pointer to USART resources
  \return      number of data items transmitted
*/
static uint32_t usart_get_txcount (USART_RESOURCES *usart) {

	uint32_t cnt;

	if (usart->dma_tx) {
		cnt = dma_channel_get_count (usart->dma_tx->channel);
	} else {
		cnt = usart->info->xfer.tx_cnt;
	}

	return cnt;
}

/**
  \fn          uint32_t usart_get_rxcount (USART_RESOURCES *usart)
  \brief       Get received data count.
  \param[in]   usart     Pointer to USART resources
  \return      number of data items received
*/
static uint32_t usart_get_rxcount (USART_RESOURCES *usart) {

	uint32_t cnt;

	if (usart->dma_rx) {
		cnt = dma_channel_get_count (usart->dma_rx->channel);
	} else {
		cnt = usart->info->xfer.rx_cnt;
	}

	return cnt;
}

/**
  \fn          int32_t usart_control (uint32_t          control,
                                      uint32_t          arg,
                                      USART_RESOURCES  *usart)
  \brief       Control USART Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \param[in]   usart    Pointer to USART resources
  \return      common \ref execution_status and driver specific \ref usart_execution_status
*/
static int32_t usart_control (	uint32_t          control,
				uint32_t          arg,
				USART_RESOURCES  *usart) {

	uint32_t val, mode;
	uint32_t lcr, mcr;

	if ((usart->info->flags & USART_FLAG_POWERED) == 0U) {
		// USART not powered
		return NDS_DRIVER_ERROR;
	}

	lcr      = 0U;
	mcr      = 0U;

	switch (control & NDS_USART_CONTROL_Msk) {
	// Control TX
	case NDS_USART_CONTROL_TX:
		// Check if TX line available
		if (arg) {
			usart->info->flags |= USART_FLAG_TX_ENABLED;
		} else {
			usart->info->flags &= ~USART_FLAG_TX_ENABLED;
		}
		return NDS_DRIVER_OK;

	// Control RX
	case NDS_USART_CONTROL_RX:
		// RX Line interrupt enable (overrun, framing, parity error, break)
		if (arg) {
			usart->info->flags |= USART_FLAG_RX_ENABLED;
			usart->reg->IER |= UARTC_IER_RLS;
		} else {
			usart->info->flags &= ~USART_FLAG_RX_ENABLED;
			usart->reg->IER &= ~UARTC_IER_RLS;
		}
		return NDS_DRIVER_OK;

	// Control break
	case NDS_USART_CONTROL_BREAK:
		if (arg) {
			if (usart->info->xfer.send_active != 0U) {
				return NDS_DRIVER_ERROR_BUSY;
			}

			usart->reg->LCR |= UARTC_LCR_SETBREAK;

			// Set Send active flag
			usart->info->xfer.send_active = 1U;
		} else {
			usart->reg->LCR &= ~UARTC_LCR_SETBREAK;

			// Clear Send active flag
			usart->info->xfer.send_active = 0U;
		}
		return NDS_DRIVER_OK;

	// Abort Send
	case NDS_USART_ABORT_SEND:
		// Disable transmit holding register empty interrupt
		usart->reg->IER &= ~UARTC_IER_THRE;

		// Set trigger level
		val  = (usart->trig_lvl & UARTC_FCR_RXFIFO_TRGL_MASK) |	UARTC_FCR_FIFO_EN;
		if (usart->dma_rx || usart->dma_tx) {
			val |= UARTC_FCR_DMA_EN;
		}

		// Transmit FIFO reset
		val |= UARTC_FCR_TFIFO_RESET;
		usart->reg->FCR = val;

		// If DMA mode - disable DMA channel
		if ((usart->dma_tx) && (usart->info->xfer.send_active != 0U)) {
			dma_channel_disable (usart->dma_tx->channel);
		}

		// Clear Send active flag
		usart->info->xfer.send_active = 0U;
		return NDS_DRIVER_OK;

		// Abort receive
		case NDS_USART_ABORT_RECEIVE:
		// Disable receive data available interrupt
		usart->reg->IER &= ~UARTC_IER_RDR;

		// Set trigger level
		val  = (usart->trig_lvl & UARTC_FCR_RXFIFO_TRGL_MASK) |	UARTC_FCR_FIFO_EN;
		if (usart->dma_rx || usart->dma_tx) {
			val |= UARTC_FCR_DMA_EN;
		}

		// Receive FIFO reset
		val |= UARTC_FCR_RFIFO_RESET;
		usart->reg->FCR = val;

		// If DMA mode - disable DMA channel
		if ((usart->dma_rx) && (usart->info->rx_status.rx_busy)) {
			dma_channel_disable (usart->dma_rx->channel);
		}

		// Clear RX busy status
		usart->info->rx_status.rx_busy = 0U;
		return NDS_DRIVER_OK;

	// Abort transfer
	case NDS_USART_ABORT_TRANSFER:
		// Disable transmit holding register empty and
		// receive data available interrupts
		usart->reg->IER &= ~(UARTC_IER_THRE | UARTC_IER_RDR);

		// If DMA mode - disable DMA channel
		if ((usart->dma_tx) && (usart->info->xfer.send_active != 0U)) {
			dma_channel_disable (usart->dma_tx->channel);
		}

		if ((usart->dma_rx) && (usart->info->rx_status.rx_busy)) {
			dma_channel_disable (usart->dma_rx->channel);
		}

		// Set trigger level
		val  = (usart->trig_lvl & UARTC_FCR_RXFIFO_TRGL_MASK) | UARTC_FCR_FIFO_EN;
		if (usart->dma_rx || usart->dma_tx) {
			val |= UARTC_FCR_DMA_EN;
		}

		// Transmit and receive FIFO reset
		val |= UARTC_FCR_TFIFO_RESET | UARTC_FCR_RFIFO_RESET;
		usart->reg->FCR = val;

		// Clear busy statuses
		usart->info->rx_status.rx_busy = 0U;
		usart->info->xfer.send_active  = 0U;
		return NDS_DRIVER_OK;

	default:
		break;
	}

	switch (control & NDS_USART_CONTROL_Msk) {
	case NDS_USART_MODE_ASYNCHRONOUS:
		mode = NDS_USART_MODE_ASYNCHRONOUS;
		break;
	case NDS_USART_MODE_SYNCHRONOUS_MASTER:
	case NDS_USART_MODE_SYNCHRONOUS_SLAVE:
	case NDS_USART_MODE_IRDA:
	case NDS_USART_MODE_SMART_CARD:
	case NDS_USART_MODE_SINGLE_WIRE:
		return NDS_USART_ERROR_MODE;

	// Default TX value
	case NDS_USART_SET_DEFAULT_TX_VALUE:
		usart->info->xfer.tx_def_val = arg;
		return NDS_DRIVER_OK;

	// Unsupported command
	default:
		return NDS_DRIVER_ERROR_UNSUPPORTED;
	}

	// Check if Receiver/Transmitter is busy
	if (	usart->info->rx_status.rx_busy ||
		(usart->info->xfer.send_active != 0U)) {
		return NDS_DRIVER_ERROR_BUSY;
	}

	// USART Data bits
	switch (control & NDS_USART_DATA_BITS_Msk) {
	case NDS_USART_DATA_BITS_5: lcr |= UARTC_LCR_BITS5; break;
	case NDS_USART_DATA_BITS_6: lcr |= UARTC_LCR_BITS6; break;
	case NDS_USART_DATA_BITS_7: lcr |= UARTC_LCR_BITS7; break;
	case NDS_USART_DATA_BITS_8: lcr |= UARTC_LCR_BITS8; break;
	default: return NDS_USART_ERROR_DATA_BITS;
	}

	// USART Parity
	switch (control & NDS_USART_PARITY_Msk) {
	case NDS_USART_PARITY_NONE:                                  break;
	case NDS_USART_PARITY_EVEN: lcr |= UARTC_LCR_PARITY_EVEN;    break;
	case NDS_USART_PARITY_ODD:  lcr |= UARTC_LCR_PARITY_ODD;     break;
	default: return (NDS_USART_ERROR_PARITY);
	}

	// USART Stop bits
	switch (control & NDS_USART_STOP_BITS_Msk) {
	case NDS_USART_STOP_BITS_1:                         break;
	case NDS_USART_STOP_BITS_2: lcr |= UARTC_LCR_STOP2; break;
	default: return NDS_USART_ERROR_STOP_BITS;
	}

	mcr = usart->reg->MCR & ~UARTC_MCR_AFE;

	switch (control & NDS_USART_FLOW_CONTROL_Msk) {
	case NDS_USART_FLOW_CONTROL_NONE:
		break;
	case NDS_USART_FLOW_CONTROL_RTS:
		if (usart->capabilities.flow_control_rts) {
			mcr |= UARTC_MCR_AFE;
		} else {
			return NDS_USART_ERROR_FLOW_CONTROL;
		}
		break;
	case NDS_USART_FLOW_CONTROL_CTS:
		if (usart->capabilities.flow_control_cts) {
			mcr |= UARTC_MCR_AFE;
		} else {
			return NDS_USART_ERROR_FLOW_CONTROL;
		}
		break;
	case NDS_USART_FLOW_CONTROL_RTS_CTS:
		if (	usart->capabilities.flow_control_rts &&
			usart->capabilities.flow_control_cts) {
			mcr |= UARTC_MCR_AFE;
		} else {
			return NDS_USART_ERROR_FLOW_CONTROL;
		}
		break;
	default:
		return NDS_USART_ERROR_FLOW_CONTROL;
	}

	// USART Baudrate
	if (usart_set_baudrate (arg, usart) == -1) {
		return NDS_USART_ERROR_BAUDRATE;
	}

	// Configuration is OK - Mode is valid
	usart->info->mode = mode;

	// Configure MCR register
	usart->reg->MCR = (usart->reg->MCR & ~UARTC_MCR_AFE) | mcr;

	// Configure Line control register
	usart->reg->LCR = ((usart->reg->LCR & (UARTC_LCR_SETBREAK | UARTC_LCR_DLAB)) | lcr);

	// Set configured flag
	usart->info->flags |= USART_FLAG_CONFIGURED;

	return NDS_DRIVER_OK;
}

/**
  \fn          NDS_USART_STATUS usart_get_status (USART_RESOURCES *usart)
  \brief       Get USART status.
  \param[in]   usart     Pointer to USART resources
  \return      USART status \ref NDS_USART_STATUS
*/
static NDS_USART_STATUS usart_get_status (USART_RESOURCES *usart) {

	NDS_USART_STATUS stat;

	stat.tx_busy          = (usart->reg->LSR & UARTC_LSR_TEMT ? (0U) : (1U));
	stat.rx_busy          = usart->info->rx_status.rx_busy;
	stat.tx_underflow     = 0U;
	stat.rx_overflow      = usart->info->rx_status.rx_overflow;
	stat.rx_break         = usart->info->rx_status.rx_break;
	stat.rx_framing_error = usart->info->rx_status.rx_framing_error;
	stat.rx_parity_error  = usart->info->rx_status.rx_parity_error;
	return stat;
}

/**
  \fn          int32_t usart_set_modem_control (NDS_USART_MODEM_CONTROL  control,
                                                USART_RESOURCES         *usart)
  \brief       Set USART Modem Control line state.
  \param[in]   control   \ref NDS_USART_MODEM_CONTROL
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t usart_set_modem_control (	NDS_USART_MODEM_CONTROL  control,
						USART_RESOURCES         *usart) {

	if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
		// USART is not configured
		return NDS_DRIVER_ERROR;
	}

	if (control == NDS_USART_RTS_CLEAR) {
		if (usart->capabilities.rts) {
			usart->reg->MCR &= ~UARTC_MCR_RTS;
		} else {
			return NDS_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	if (control == NDS_USART_RTS_SET) {
		if (usart->capabilities.rts) {
			usart->reg->MCR |=  UARTC_MCR_RTS;
		} else {
			return NDS_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	if (control == NDS_USART_DTR_CLEAR) {
		if (usart->capabilities.dtr) {
			usart->reg->MCR &= ~UARTC_MCR_DTR;
		} else {
			return NDS_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	if (control == NDS_USART_DTR_SET) {
		if (usart->capabilities.dtr) {
			usart->reg->MCR |=  UARTC_MCR_DTR;
		} else {
			return NDS_DRIVER_ERROR_UNSUPPORTED;
		}
	}

	return NDS_DRIVER_OK;
}

/**
  \fn          NDS_USART_MODEM_STATUS usart_get_modem_status (USART_RESOURCES *usart)
  \brief       Get USART Modem Status lines state.
  \param[in]   usart     Pointer to USART resources
  \return      modem status \ref NDS_USART_MODEM_STATUS
*/
static NDS_USART_MODEM_STATUS usart_get_modem_status (USART_RESOURCES *usart) {

	NDS_USART_MODEM_STATUS modem_status;
	uint32_t msr;

	if (usart->info->flags & USART_FLAG_CONFIGURED) {

		msr = usart->reg->MSR;

		if (usart->capabilities.cts) {
			modem_status.cts = (msr & UARTC_MSR_CTS ? (1U) : (0U));
		} else {
			modem_status.cts = 0U;
		}

		if (usart->capabilities.dsr) {
			modem_status.dsr = (msr & UARTC_MSR_DSR ? (1U) : (0U));
		} else {
			modem_status.dsr = 0U;
		}

		if (usart->capabilities.ri ) {
			modem_status.ri  = (msr & UARTC_MSR_RI  ? (1U) : (0U));
		} else {
			modem_status.ri  = 0U;
		}

		if (usart->capabilities.dcd) {
			modem_status.dcd = (msr & UARTC_MSR_DCD ? (1U) : (0U));
		} else {
			modem_status.dcd = 0U;
		}

	} else {
		modem_status.cts = 0U;
		modem_status.dsr = 0U;
		modem_status.ri  = 0U;
		modem_status.dcd = 0U;
	}

	return modem_status;
}

/**
  \fn          void usart_irq_handler (UART_RESOURCES *usart)
  \brief       USART Interrupt handler.
  \param[in]   usart     Pointer to USART resources
*/
static void usart_irq_handler (USART_RESOURCES *usart) {

	uint32_t iir, event, val, i;

	event = 0U;
	iir   = usart->reg->IIR;

	if ((iir & UARTC_IIR_NONE) == 0U) {
		// Transmit holding register empty
		if ((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_THRE) {
			val = 16U;
			while ((val --) && (usart->info->xfer.tx_num != usart->info->xfer.tx_cnt)) {
				// Write data to Tx FIFO
				usart->reg->THR = usart->info->xfer.tx_buf[usart->info->xfer.tx_cnt];
				usart->info->xfer.tx_cnt++;
			}

			// Check if all data is transmitted
			if (usart->info->xfer.tx_num == usart->info->xfer.tx_cnt) {
				// Disable THRE interrupt
				usart->reg->IER &= ~UARTC_IER_THRE;

				// Clear TX busy flag
				usart->info->xfer.send_active = 0U;

				// Set send complete event
				event |= NDS_USART_EVENT_SEND_COMPLETE;
			}
		}

		// Receive line status
		if ((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_RLS) {
			event |= usart_rxline_irq_handler(usart);
		}

		// Receive data available and Character time-out indicator interrupt
		if (	((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_RDA)  ||
			((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_RTO)) {

			switch (usart->trig_lvl) {
			case USART_TRIG_LVL_1:  i = 1U;  break;
			case USART_TRIG_LVL_4:  i = 3U;  break;
			case USART_TRIG_LVL_8:  i = 7U;  break;
			case USART_TRIG_LVL_14: i = 13U; break;
			default: i = 1U;
			}

			// Get available data from RX FIFO
			while ((usart->reg->LSR & UARTC_LSR_RDR) && (i--)) {
				// Check RX line interrupt for errors
				event |= usart_rxline_irq_handler(usart);

				// Read data from RX FIFO into receive buffer
				usart->info->xfer.rx_buf[usart->info->xfer.rx_cnt] = usart->reg->RBR;

				usart->info->xfer.rx_cnt++;

				// Check if requested amount of data is received
				if (usart->info->xfer.rx_cnt == usart->info->xfer.rx_num) {
					// Disable RDA interrupt
					usart->reg->IER &= ~UARTC_IER_RDR;

					// Clear RX busy flag and set receive transfer complete event
					usart->info->rx_status.rx_busy = 0U;
					event |= NDS_USART_EVENT_RECEIVE_COMPLETE;

					break;
				}
			}
		}

		// Character time-out indicator
		if ((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_RTO) {
			// Signal RX Time-out event, if not all requested data received
			if (usart->info->xfer.rx_cnt != usart->info->xfer.rx_num) {
				event |= NDS_USART_EVENT_RX_TIMEOUT;
			}
		}

		// Modem interrupt
		if ((iir & UARTC_IIR_INT_MASK) == UARTC_IIR_MODEM) {
			// Save modem status register
			val = usart->reg->MSR;

			// CTS state changed
			if ((usart->capabilities.cts) && (val & UARTC_MSR_DCTS)) {
			event |= NDS_USART_EVENT_CTS;
			}

			// DSR state changed
			if ((usart->capabilities.dsr) && (val & UARTC_MSR_DDSR)) {
			event |= NDS_USART_EVENT_DSR;
			}

			// Ring indicator
			if ((usart->capabilities.ri)  && (val & UARTC_MSR_TERI)) {
			event |= NDS_USART_EVENT_RI;
			}

			// DCD state changed
			if ((usart->capabilities.dcd) && (val & UARTC_MSR_DDCD)) {
			event |= NDS_USART_EVENT_DCD;
			}

		}
	}

	if ((usart->info->cb_event != NULL) && (event != 0U)) {
		usart->info->cb_event (event);
	}
}

#if (((RTE_UART0) && (RTE_UART0_DMA_TX_EN == 1)) || \
     ((RTE_UART1) && (RTE_UART1_DMA_TX_EN == 1)))
/**
  \fn          void usart_dma_tx_event (uint32_t event, USART_RESOURCES *usart)
  \brief       UART Interrupt handler.
  \param[in]   usart     Pointer to USART resources
  \param[in]   event     DMA_EVENT_TERMINAL_COUNT_REQUEST / DMA_EVENT_ERROR
*/
static void usart_dma_tx_event (uint32_t event, USART_RESOURCES *usart) {

	switch (event) {
	case DMA_EVENT_TERMINAL_COUNT_REQUEST:
		usart->info->xfer.tx_cnt = usart->info->xfer.tx_num;
		// Clear TX busy flag
		usart->info->xfer.send_active = 0U;

		// Set Send Complete event for asynchronous transfers
		if (usart->info->cb_event) {
			usart->info->cb_event (NDS_USART_EVENT_SEND_COMPLETE);
		}
		break;
	case DMA_EVENT_ERROR:
	default:
		break;
	}
}
#endif

#if (((RTE_UART0) && (RTE_UART0_DMA_RX_EN == 1)) || \
     ((RTE_UART1) && (RTE_UART1_DMA_RX_EN == 1)))
/**
  \fn          void usart_dma_rx_event (uint32_t event, USART_RESOURCES *usart)
  \brief       UART Interrupt handler.
  \param[in]   event     DMA_EVENT_TERMINAL_COUNT_REQUEST / DMA_EVENT_ERROR
  \param[in]   usart     Pointer to USART resources
*/
static void usart_dma_rx_event (uint32_t event, USART_RESOURCES *usart) {

	switch (event) {
	case DMA_EVENT_TERMINAL_COUNT_REQUEST:
		usart->info->xfer.rx_cnt    = usart->info->xfer.rx_num;
		usart->info->rx_status.rx_busy = 0U;

		if (usart->info->cb_event) {
			usart->info->cb_event (NDS_USART_EVENT_RECEIVE_COMPLETE);
		}
		break;
	case DMA_EVENT_ERROR:
	default:
		break;
	}
}
#endif


#if (RTE_UART0)
// usart0 Driver Wrapper functions
static NDS_USART_CAPABILITIES usart0_get_capabilities (void) {
	return usart_get_capabilities (&usart0_resources);
}
static int32_t usart0_initialize (NDS_USART_SignalEvent_t cb_event) {
	return usart_initialize (cb_event, &usart0_resources);
}
static int32_t usart0_uninitialize (void) {
	return usart_uninitialize(&usart0_resources);
}
static int32_t usart0_power_control (NDS_POWER_STATE state) {
	return usart_power_control (state, &usart0_resources);
}
static int32_t usart0_send (const void *data, uint32_t num) {
	return usart_send (data, num, &usart0_resources);
}
static int32_t usart0_receive (void *data, uint32_t num) {
	return usart_receive (data, num, &usart0_resources);
}
static int32_t usart0_transfer (	const void	*data_out,
					void		*data_in,
					uint32_t	num) {
	return usart_transfer (data_out, data_in, num, &usart0_resources);
}
static uint32_t usart0_get_txcount (void) {
	return usart_get_txcount (&usart0_resources);
}
static uint32_t usart0_get_rxcount (void) {
	return usart_get_rxcount (&usart0_resources);
}
static int32_t usart0_control (uint32_t control, uint32_t arg) {
	return usart_control (control, arg, &usart0_resources);
}
static NDS_USART_STATUS usart0_get_status (void) {
	return usart_get_status (&usart0_resources);
}
static int32_t usart0_set_modem_control (NDS_USART_MODEM_CONTROL control) {
	return usart_set_modem_control (control, &usart0_resources);
}
static NDS_USART_MODEM_STATUS usart0_get_modem_status (void) {
	return usart_get_modem_status (&usart0_resources);
}

/*--------*/
#define UART0_irq_handler	uart1_irq_handler
void UART0_irq_handler (void) {
	usart_irq_handler (&usart0_resources);
}

#if (RTE_UART0_DMA_TX_EN == 1)
void usart0_dma_tx_event (uint32_t event) {
	usart_dma_tx_event(event, &usart0_resources);
}
#endif

#if (RTE_UART0_DMA_RX_EN == 1)
void usart0_dma_rx_event (uint32_t event) {
	usart_dma_rx_event(event, &usart0_resources);
}
#endif

// USART0 Driver Control Block
NDS_DRIVER_USART Driver_USART0 = {
	usart_get_version,
	usart0_get_capabilities,
	usart0_initialize,
	usart0_uninitialize,
	usart0_power_control,
	usart0_send,
	usart0_receive,
	usart0_transfer,
	usart0_get_txcount,
	usart0_get_rxcount,
	usart0_control,
	usart0_get_status,
	usart0_set_modem_control,
	usart0_get_modem_status
};
#endif

#if (RTE_UART1)
// USART1 Driver Wrapper functions
static NDS_USART_CAPABILITIES usart1_get_capabilities (void) {
	return usart_get_capabilities (&usart1_resources);
}
static int32_t usart1_initialize (NDS_USART_SignalEvent_t cb_event) {
	return usart_initialize (cb_event, &usart1_resources);
}
static int32_t usart1_uninitialize (void) {
	return usart_uninitialize(&usart1_resources);
}
static int32_t usart1_power_control (NDS_POWER_STATE state) {
	return usart_power_control (state, &usart1_resources);
}
static int32_t usart1_send (const void *data, uint32_t num) {
	return usart_send (data, num, &usart1_resources);
}
static int32_t usart1_receive (void *data, uint32_t num) {
	return usart_receive (data, num, &usart1_resources);
}
static int32_t usart1_transfer (	const void	*data_out,
					void      	*data_in,
					uint32_t	num) {
	return usart_transfer (data_out, data_in, num, &usart1_resources);
}
static uint32_t usart1_get_txcount (void) {
	return usart_get_txcount (&usart1_resources);
}
static uint32_t usart1_get_rxcount (void) {
	return usart_get_rxcount (&usart1_resources);
}
static int32_t usart1_control (uint32_t control, uint32_t arg) {
	return usart_control (control, arg, &usart1_resources);
}
static NDS_USART_STATUS usart1_get_status (void) {
	return usart_get_status (&usart1_resources);
}
static int32_t usart1_set_modem_control (NDS_USART_MODEM_CONTROL control) {
	return usart_set_modem_control (control, &usart1_resources);
}
static NDS_USART_MODEM_STATUS usart1_get_modem_status (void) {
	return usart_get_modem_status (&usart1_resources);
}

/*--------*/
#define UART1_irq_handler	uart2_irq_handler
void UART1_irq_handler (void) {
	usart_irq_handler (&usart1_resources);
}

#if (RTE_UART1_DMA_TX_EN == 1)
void usart1_dma_tx_event (uint32_t event) {
	usart_dma_tx_event(event, &usart1_resources);
}
#endif

#if (RTE_UART1_DMA_RX_EN == 1)
void usart1_dma_rx_event (uint32_t event) {
	usart_dma_rx_event(event, &usart1_resources);
}
#endif

// USART1 Driver Control Block
NDS_DRIVER_USART Driver_USART1 = {
	usart_get_version,
	usart1_get_capabilities,
	usart1_initialize,
	usart1_uninitialize,
	usart1_power_control,
	usart1_send,
	usart1_receive,
	usart1_transfer,
	usart1_get_txcount,
	usart1_get_rxcount,
	usart1_control,
	usart1_get_status,
	usart1_set_modem_control,
	usart1_get_modem_status
};
#endif
