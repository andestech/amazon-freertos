/*
 * Project:      USART (Universal Synchronous Asynchronous Receiver Transmitter)
 *               Driver definitions
 */

#ifndef __DRIVER_USART_H
#define __DRIVER_USART_H

#include "Driver_Common.h"

#define NDS_USART_API_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,02)  /* API version */


/****** USART Control Codes *****/

#define NDS_USART_CONTROL_Pos                0
#define NDS_USART_CONTROL_Msk               (0xFFUL << NDS_USART_CONTROL_Pos)

/*----- USART Control Codes: Mode -----*/
#define NDS_USART_MODE_ASYNCHRONOUS         (0x01UL << NDS_USART_CONTROL_Pos)   ///< UART (Asynchronous); arg = Baudrate
#define NDS_USART_MODE_SYNCHRONOUS_MASTER   (0x02UL << NDS_USART_CONTROL_Pos)   ///< Synchronous Master (generates clock signal); arg = Baudrate
#define NDS_USART_MODE_SYNCHRONOUS_SLAVE    (0x03UL << NDS_USART_CONTROL_Pos)   ///< Synchronous Slave (external clock signal)
#define NDS_USART_MODE_SINGLE_WIRE          (0x04UL << NDS_USART_CONTROL_Pos)   ///< UART Single-wire (half-duplex); arg = Baudrate
#define NDS_USART_MODE_IRDA                 (0x05UL << NDS_USART_CONTROL_Pos)   ///< UART IrDA; arg = Baudrate
#define NDS_USART_MODE_SMART_CARD           (0x06UL << NDS_USART_CONTROL_Pos)   ///< UART Smart Card; arg = Baudrate

/*----- USART Control Codes: Mode Parameters: Data Bits -----*/
#define NDS_USART_DATA_BITS_Pos              8
#define NDS_USART_DATA_BITS_Msk             (7UL << NDS_USART_DATA_BITS_Pos)
#define NDS_USART_DATA_BITS_5               (5UL << NDS_USART_DATA_BITS_Pos)    ///< 5 Data bits
#define NDS_USART_DATA_BITS_6               (6UL << NDS_USART_DATA_BITS_Pos)    ///< 6 Data bit
#define NDS_USART_DATA_BITS_7               (7UL << NDS_USART_DATA_BITS_Pos)    ///< 7 Data bits
#define NDS_USART_DATA_BITS_8               (0UL << NDS_USART_DATA_BITS_Pos)    ///< 8 Data bits (default)
#define NDS_USART_DATA_BITS_9               (1UL << NDS_USART_DATA_BITS_Pos)    ///< 9 Data bits

/*----- USART Control Codes: Mode Parameters: Parity -----*/
#define NDS_USART_PARITY_Pos                 12
#define NDS_USART_PARITY_Msk                (3UL << NDS_USART_PARITY_Pos)
#define NDS_USART_PARITY_NONE               (0UL << NDS_USART_PARITY_Pos)       ///< No Parity (default)
#define NDS_USART_PARITY_EVEN               (1UL << NDS_USART_PARITY_Pos)       ///< Even Parity
#define NDS_USART_PARITY_ODD                (2UL << NDS_USART_PARITY_Pos)       ///< Odd Parity

/*----- USART Control Codes: Mode Parameters: Stop Bits -----*/
#define NDS_USART_STOP_BITS_Pos              14
#define NDS_USART_STOP_BITS_Msk             (3UL << NDS_USART_STOP_BITS_Pos)
#define NDS_USART_STOP_BITS_1               (0UL << NDS_USART_STOP_BITS_Pos)    ///< 1 Stop bit (default)
#define NDS_USART_STOP_BITS_2               (1UL << NDS_USART_STOP_BITS_Pos)    ///< 2 Stop bits
#define NDS_USART_STOP_BITS_1_5             (2UL << NDS_USART_STOP_BITS_Pos)    ///< 1.5 Stop bits
#define NDS_USART_STOP_BITS_0_5             (3UL << NDS_USART_STOP_BITS_Pos)    ///< 0.5 Stop bits

/*----- USART Control Codes: Mode Parameters: Flow Control -----*/
#define NDS_USART_FLOW_CONTROL_Pos           16
#define NDS_USART_FLOW_CONTROL_Msk          (3UL << NDS_USART_FLOW_CONTROL_Pos)
#define NDS_USART_FLOW_CONTROL_NONE         (0UL << NDS_USART_FLOW_CONTROL_Pos) ///< No Flow Control (default)
#define NDS_USART_FLOW_CONTROL_RTS          (1UL << NDS_USART_FLOW_CONTROL_Pos) ///< RTS Flow Control
#define NDS_USART_FLOW_CONTROL_CTS          (2UL << NDS_USART_FLOW_CONTROL_Pos) ///< CTS Flow Control
#define NDS_USART_FLOW_CONTROL_RTS_CTS      (3UL << NDS_USART_FLOW_CONTROL_Pos) ///< RTS/CTS Flow Control

/*----- USART Control Codes: Mode Parameters: Clock Polarity (Synchronous mode) -----*/
#define NDS_USART_CPOL_Pos                   18
#define NDS_USART_CPOL_Msk                  (1UL << NDS_USART_CPOL_Pos)
#define NDS_USART_CPOL0                     (0UL << NDS_USART_CPOL_Pos)         ///< CPOL = 0 (default)
#define NDS_USART_CPOL1                     (1UL << NDS_USART_CPOL_Pos)         ///< CPOL = 1

/*----- USART Control Codes: Mode Parameters: Clock Phase (Synchronous mode) -----*/
#define NDS_USART_CPHA_Pos                   19
#define NDS_USART_CPHA_Msk                  (1UL << NDS_USART_CPHA_Pos)
#define NDS_USART_CPHA0                     (0UL << NDS_USART_CPHA_Pos)         ///< CPHA = 0 (default)
#define NDS_USART_CPHA1                     (1UL << NDS_USART_CPHA_Pos)         ///< CPHA = 1


/*----- USART Control Codes: Miscellaneous Controls  -----*/
#define NDS_USART_SET_DEFAULT_TX_VALUE      (0x10UL << NDS_USART_CONTROL_Pos)   ///< Set default Transmit value (Synchronous Receive only); arg = value
#define NDS_USART_SET_IRDA_PULSE            (0x11UL << NDS_USART_CONTROL_Pos)   ///< Set IrDA Pulse in ns; arg: 0=3/16 of bit period  
#define NDS_USART_SET_SMART_CARD_GUARD_TIME (0x12UL << NDS_USART_CONTROL_Pos)   ///< Set Smart Card Guard Time; arg = number of bit periods
#define NDS_USART_SET_SMART_CARD_CLOCK      (0x13UL << NDS_USART_CONTROL_Pos)   ///< Set Smart Card Clock in Hz; arg: 0=Clock not generated
#define NDS_USART_CONTROL_SMART_CARD_NACK   (0x14UL << NDS_USART_CONTROL_Pos)   ///< Smart Card NACK generation; arg: 0=disabled, 1=enabled
#define NDS_USART_CONTROL_TX                (0x15UL << NDS_USART_CONTROL_Pos)   ///< Transmitter; arg: 0=disabled, 1=enabled
#define NDS_USART_CONTROL_RX                (0x16UL << NDS_USART_CONTROL_Pos)   ///< Receiver; arg: 0=disabled, 1=enabled
#define NDS_USART_CONTROL_BREAK             (0x17UL << NDS_USART_CONTROL_Pos)   ///< Continuous Break transmission; arg: 0=disabled, 1=enabled
#define NDS_USART_ABORT_SEND                (0x18UL << NDS_USART_CONTROL_Pos)   ///< Abort \ref NDS_USART_Send
#define NDS_USART_ABORT_RECEIVE             (0x19UL << NDS_USART_CONTROL_Pos)   ///< Abort \ref NDS_USART_Receive
#define NDS_USART_ABORT_TRANSFER            (0x1AUL << NDS_USART_CONTROL_Pos)   ///< Abort \ref NDS_USART_Transfer



/****** USART specific error codes *****/
#define NDS_USART_ERROR_MODE                (NDS_DRIVER_ERROR_SPECIFIC - 1)     ///< Specified Mode not supported
#define NDS_USART_ERROR_BAUDRATE            (NDS_DRIVER_ERROR_SPECIFIC - 2)     ///< Specified baudrate not supported
#define NDS_USART_ERROR_DATA_BITS           (NDS_DRIVER_ERROR_SPECIFIC - 3)     ///< Specified number of Data bits not supported
#define NDS_USART_ERROR_PARITY              (NDS_DRIVER_ERROR_SPECIFIC - 4)     ///< Specified Parity not supported
#define NDS_USART_ERROR_STOP_BITS           (NDS_DRIVER_ERROR_SPECIFIC - 5)     ///< Specified number of Stop bits not supported
#define NDS_USART_ERROR_FLOW_CONTROL        (NDS_DRIVER_ERROR_SPECIFIC - 6)     ///< Specified Flow Control not supported
#define NDS_USART_ERROR_CPOL                (NDS_DRIVER_ERROR_SPECIFIC - 7)     ///< Specified Clock Polarity not supported
#define NDS_USART_ERROR_CPHA                (NDS_DRIVER_ERROR_SPECIFIC - 8)     ///< Specified Clock Phase not supported


/**
\brief USART Status
*/
typedef struct _NDS_USART_STATUS {
  uint32_t tx_busy          : 1;        ///< Transmitter busy flag
  uint32_t rx_busy          : 1;        ///< Receiver busy flag
  uint32_t tx_underflow     : 1;        ///< Transmit data underflow detected (cleared on start of next send operation)
  uint32_t rx_overflow      : 1;        ///< Receive data overflow detected (cleared on start of next receive operation)
  uint32_t rx_break         : 1;        ///< Break detected on receive (cleared on start of next receive operation)
  uint32_t rx_framing_error : 1;        ///< Framing error detected on receive (cleared on start of next receive operation)
  uint32_t rx_parity_error  : 1;        ///< Parity error detected on receive (cleared on start of next receive operation)
} NDS_USART_STATUS;

/**
\brief USART Modem Control
*/
typedef enum _NDS_USART_MODEM_CONTROL {
  NDS_USART_RTS_CLEAR,                  ///< Deactivate RTS
  NDS_USART_RTS_SET,                    ///< Activate RTS
  NDS_USART_DTR_CLEAR,                  ///< Deactivate DTR
  NDS_USART_DTR_SET                     ///< Activate DTR
} NDS_USART_MODEM_CONTROL;

/**
\brief USART Modem Status
*/
typedef struct _NDS_USART_MODEM_STATUS {
  uint32_t cts : 1;                     ///< CTS state: 1=Active, 0=Inactive
  uint32_t dsr : 1;                     ///< DSR state: 1=Active, 0=Inactive
  uint32_t dcd : 1;                     ///< DCD state: 1=Active, 0=Inactive
  uint32_t ri  : 1;                     ///< RI  state: 1=Active, 0=Inactive
} NDS_USART_MODEM_STATUS;


/****** USART Event *****/
#define NDS_USART_EVENT_SEND_COMPLETE       (1UL << 0)  ///< Send completed; however USART may still transmit data
#define NDS_USART_EVENT_RECEIVE_COMPLETE    (1UL << 1)  ///< Receive completed
#define NDS_USART_EVENT_TRANSFER_COMPLETE   (1UL << 2)  ///< Transfer completed
#define NDS_USART_EVENT_TX_COMPLETE         (1UL << 3)  ///< Transmit completed (optional)
#define NDS_USART_EVENT_TX_UNDERFLOW        (1UL << 4)  ///< Transmit data not available (Synchronous Slave)
#define NDS_USART_EVENT_RX_OVERFLOW         (1UL << 5)  ///< Receive data overflow
#define NDS_USART_EVENT_RX_TIMEOUT          (1UL << 6)  ///< Receive character timeout (optional)
#define NDS_USART_EVENT_RX_BREAK            (1UL << 7)  ///< Break detected on receive
#define NDS_USART_EVENT_RX_FRAMING_ERROR    (1UL << 8)  ///< Framing error detected on receive
#define NDS_USART_EVENT_RX_PARITY_ERROR     (1UL << 9)  ///< Parity error detected on receive
#define NDS_USART_EVENT_CTS                 (1UL << 10) ///< CTS state changed (optional)
#define NDS_USART_EVENT_DSR                 (1UL << 11) ///< DSR state changed (optional)
#define NDS_USART_EVENT_DCD                 (1UL << 12) ///< DCD state changed (optional)
#define NDS_USART_EVENT_RI                  (1UL << 13) ///< RI  state changed (optional)


// Function documentation
/**
  \fn          NDS_DRIVER_VERSION NDS_USART_GetVersion (void)
  \brief       Get driver version.
  \return      \ref NDS_DRIVER_VERSION

  \fn          NDS_USART_CAPABILITIES NDS_USART_GetCapabilities (void)
  \brief       Get driver capabilities
  \return      \ref NDS_USART_CAPABILITIES

  \fn          int32_t NDS_USART_Initialize (NDS_USART_SignalEvent_t cb_event)
  \brief       Initialize USART Interface.
  \param[in]   cb_event  Pointer to \ref NDS_USART_SignalEvent
  \return      \ref execution_status

  \fn          int32_t NDS_USART_Uninitialize (void)
  \brief       De-initialize USART Interface.
  \return      \ref execution_status

  \fn          int32_t NDS_USART_PowerControl (NDS_POWER_STATE state)
  \brief       Control USART Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status

  \fn          int32_t NDS_USART_Send (const void *data, uint32_t num)
  \brief       Start sending data to USART transmitter.
  \param[in]   data  Pointer to buffer with data to send to USART transmitter
  \param[in]   num   Number of data items to send
  \return      \ref execution_status

  \fn          int32_t NDS_USART_Receive (void *data, uint32_t num)
  \brief       Start receiving data from USART receiver.
  \param[out]  data  Pointer to buffer for data to receive from USART receiver
  \param[in]   num   Number of data items to receive
  \return      \ref execution_status

  \fn          int32_t NDS_USART_Transfer (const void *data_out,
                                                 void *data_in,
                                           uint32_t    num)
  \brief       Start sending/receiving data to/from USART transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to USART transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from USART receiver
  \param[in]   num       Number of data items to transfer
  \return      \ref execution_status

  \fn          uint32_t NDS_USART_GetTxCount (void)
  \brief       Get transmitted data count.
  \return      number of data items transmitted

  \fn          uint32_t NDS_USART_GetRxCount (void)
  \brief       Get received data count.
  \return      number of data items received

  \fn          int32_t NDS_USART_Control (uint32_t control, uint32_t arg)
  \brief       Control USART Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      common \ref execution_status and driver specific \ref usart_execution_status

  \fn          NDS_USART_STATUS NDS_USART_GetStatus (void)
  \brief       Get USART status.
  \return      USART status \ref NDS_USART_STATUS

  \fn          int32_t NDS_USART_SetModemControl (NDS_USART_MODEM_CONTROL control)
  \brief       Set USART Modem Control line state.
  \param[in]   control  \ref NDS_USART_MODEM_CONTROL
  \return      \ref execution_status 

  \fn          NDS_USART_MODEM_STATUS NDS_USART_GetModemStatus (void)
  \brief       Get USART Modem Status lines state.
  \return      modem status \ref NDS_USART_MODEM_STATUS

  \fn          void NDS_USART_SignalEvent (uint32_t event)
  \brief       Signal USART Events.
  \param[in]   event  \ref USART_events notification mask
  \return      none
*/

typedef void (*NDS_USART_SignalEvent_t) (uint32_t event);  ///< Pointer to \ref NDS_USART_SignalEvent : Signal USART Event.


/**
\brief USART Device Driver Capabilities.
*/
typedef struct _NDS_USART_CAPABILITIES {
  uint32_t asynchronous       : 1;      ///< supports UART (Asynchronous) mode 
  uint32_t synchronous_master : 1;      ///< supports Synchronous Master mode
  uint32_t synchronous_slave  : 1;      ///< supports Synchronous Slave mode
  uint32_t single_wire        : 1;      ///< supports UART Single-wire mode
  uint32_t irda               : 1;      ///< supports UART IrDA mode
  uint32_t smart_card         : 1;      ///< supports UART Smart Card mode
  uint32_t smart_card_clock   : 1;      ///< Smart Card Clock generator available
  uint32_t flow_control_rts   : 1;      ///< RTS Flow Control available
  uint32_t flow_control_cts   : 1;      ///< CTS Flow Control available
  uint32_t event_tx_complete  : 1;      ///< Transmit completed event: \ref NDS_USART_EVENT_TX_COMPLETE
  uint32_t event_rx_timeout   : 1;      ///< Signal receive character timeout event: \ref NDS_USART_EVENT_RX_TIMEOUT
  uint32_t rts                : 1;      ///< RTS Line: 0=not available, 1=available
  uint32_t cts                : 1;      ///< CTS Line: 0=not available, 1=available
  uint32_t dtr                : 1;      ///< DTR Line: 0=not available, 1=available
  uint32_t dsr                : 1;      ///< DSR Line: 0=not available, 1=available
  uint32_t dcd                : 1;      ///< DCD Line: 0=not available, 1=available
  uint32_t ri                 : 1;      ///< RI Line: 0=not available, 1=available
  uint32_t event_cts          : 1;      ///< Signal CTS change event: \ref NDS_USART_EVENT_CTS
  uint32_t event_dsr          : 1;      ///< Signal DSR change event: \ref NDS_USART_EVENT_DSR
  uint32_t event_dcd          : 1;      ///< Signal DCD change event: \ref NDS_USART_EVENT_DCD
  uint32_t event_ri           : 1;      ///< Signal RI change event: \ref NDS_USART_EVENT_RI
} NDS_USART_CAPABILITIES;


/**
\brief Access structure of the USART Driver.
*/
typedef struct _NDS_DRIVER_USART {
  NDS_DRIVER_VERSION     (*GetVersion)      (void);                              ///< Pointer to \ref NDS_USART_GetVersion : Get driver version.
  NDS_USART_CAPABILITIES (*GetCapabilities) (void);                              ///< Pointer to \ref NDS_USART_GetCapabilities : Get driver capabilities.
  int32_t                (*Initialize)      (NDS_USART_SignalEvent_t cb_event);  ///< Pointer to \ref NDS_USART_Initialize : Initialize USART Interface.
  int32_t                (*Uninitialize)    (void);                              ///< Pointer to \ref NDS_USART_Uninitialize : De-initialize USART Interface.
  int32_t                (*PowerControl)    (NDS_POWER_STATE state);             ///< Pointer to \ref NDS_USART_PowerControl : Control USART Interface Power.
  int32_t                (*Send)            (const void *data, uint32_t num);    ///< Pointer to \ref NDS_USART_Send : Start sending data to USART transmitter.
  int32_t                (*Receive)         (      void *data, uint32_t num);    ///< Pointer to \ref NDS_USART_Receive : Start receiving data from USART receiver.
  int32_t                (*Transfer)        (const void *data_out,
                                                   void *data_in,
                                             uint32_t    num);                   ///< Pointer to \ref NDS_USART_Transfer : Start sending/receiving data to/from USART.
  uint32_t               (*GetTxCount)      (void);                              ///< Pointer to \ref NDS_USART_GetTxCount : Get transmitted data count.
  uint32_t               (*GetRxCount)      (void);                              ///< Pointer to \ref NDS_USART_GetRxCount : Get received data count.
  int32_t                (*Control)         (uint32_t control, uint32_t arg);    ///< Pointer to \ref NDS_USART_Control : Control USART Interface.
  NDS_USART_STATUS       (*GetStatus)       (void);                              ///< Pointer to \ref NDS_USART_GetStatus : Get USART status.
  int32_t                (*SetModemControl) (NDS_USART_MODEM_CONTROL control);   ///< Pointer to \ref NDS_USART_SetModemControl : Set USART Modem Control line state.
  NDS_USART_MODEM_STATUS (*GetModemStatus)  (void);                              ///< Pointer to \ref NDS_USART_GetModemStatus : Get USART Modem Status lines state.
} const NDS_DRIVER_USART;

#endif /* __DRIVER_USART_H */
