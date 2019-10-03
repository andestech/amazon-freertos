/*
 * Project:      I2C (Inter-Integrated Circuit) Driver definitions
 */

#ifndef __DRIVER_I2C_H
#define __DRIVER_I2C_H

#include "Driver_Common.h"

#define NDS_I2C_API_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,02)  /* API version */


/****** I2C Control Codes *****/

#define NDS_I2C_OWN_ADDRESS             (0x01)      ///< Set Own Slave Address; arg = address 
#define NDS_I2C_BUS_SPEED               (0x02)      ///< Set Bus Speed; arg = speed
#define NDS_I2C_BUS_CLEAR               (0x03)      ///< Execute Bus clear: send nine clock pulses
#define NDS_I2C_ABORT_TRANSFER          (0x04)      ///< Abort Master/Slave Transmit/Receive

/*----- I2C Bus Speed -----*/
#define NDS_I2C_BUS_SPEED_STANDARD      (0x01)      ///< Standard Speed (100kHz)
#define NDS_I2C_BUS_SPEED_FAST          (0x02)      ///< Fast Speed     (400kHz)
#define NDS_I2C_BUS_SPEED_FAST_PLUS     (0x03)      ///< Fast+ Speed    (  1MHz)
#define NDS_I2C_BUS_SPEED_HIGH          (0x04)      ///< High Speed     (3.4MHz)


/****** I2C Address Flags *****/

#define NDS_I2C_ADDRESS_10BIT           0x0400      ///< 10-bit address flag
#define NDS_I2C_ADDRESS_GC              0x8000      ///< General Call flag


/**
\brief I2C Status
*/
typedef struct _NDS_I2C_STATUS {
  uint32_t busy             : 1;        ///< Busy flag
  uint32_t mode             : 1;        ///< Mode: 0=Slave, 1=Master
  uint32_t direction        : 1;        ///< Direction: 0=Transmitter, 1=Receiver
  uint32_t general_call     : 1;        ///< General Call indication (cleared on start of next Slave operation)
  uint32_t arbitration_lost : 1;        ///< Master lost arbitration (cleared on start of next Master operation)
  uint32_t bus_error        : 1;        ///< Bus error detected (cleared on start of next Master/Slave operation)
} NDS_I2C_STATUS;


/****** I2C Event *****/
#define NDS_I2C_EVENT_TRANSFER_DONE       (1UL << 0)  ///< Master/Slave Transmit/Receive finished
#define NDS_I2C_EVENT_TRANSFER_INCOMPLETE (1UL << 1)  ///< Master/Slave Transmit/Receive incomplete transfer
#define NDS_I2C_EVENT_SLAVE_TRANSMIT      (1UL << 2)  ///< Slave Transmit operation requested
#define NDS_I2C_EVENT_SLAVE_RECEIVE       (1UL << 3)  ///< Slave Receive operation requested
#define NDS_I2C_EVENT_ADDRESS_NACK        (1UL << 4)  ///< Address not acknowledged from Slave
#define NDS_I2C_EVENT_GENERAL_CALL        (1UL << 5)  ///< General Call indication
#define NDS_I2C_EVENT_ARBITRATION_LOST    (1UL << 6)  ///< Master lost arbitration
#define NDS_I2C_EVENT_BUS_ERROR           (1UL << 7)  ///< Bus error detected (START/STOP at illegal position)
#define NDS_I2C_EVENT_BUS_CLEAR           (1UL << 8)  ///< Bus clear finished


// Function documentation
/**
  \fn          NDS_DRIVER_VERSION NDS_I2C_GetVersion (void)
  \brief       Get driver version.
  \return      \ref NDS_DRIVER_VERSION

  \fn          NDS_I2C_CAPABILITIES NDS_I2C_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref NDS_I2C_CAPABILITIES

  \fn          int32_t NDS_I2C_Initialize (NDS_I2C_SignalEvent_t cb_event)
  \brief       Initialize I2C Interface.
  \param[in]   cb_event  Pointer to \ref NDS_I2C_SignalEvent
  \return      \ref execution_status

  \fn          int32_t NDS_I2C_Uninitialize (void)
  \brief       De-initialize I2C Interface.
  \return      \ref execution_status

  \fn          int32_t NDS_I2C_PowerControl (NDS_POWER_STATE state)
  \brief       Control I2C Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status

  \fn          int32_t NDS_I2C_MasterTransmit (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending)
  \brief       Start transmitting data as I2C Master.
  \param[in]   addr          Slave address (7-bit or 10-bit)
  \param[in]   data          Pointer to buffer with data to transmit to I2C Slave
  \param[in]   num           Number of data bytes to transmit
  \param[in]   xfer_pending  Transfer operation is pending - Stop condition will not be generated
  \return      \ref execution_status

  \fn          int32_t NDS_I2C_MasterReceive (uint32_t addr, uint8_t *data, uint32_t num, bool xfer_pending)
  \brief       Start receiving data as I2C Master.
  \param[in]   addr          Slave address (7-bit or 10-bit)
  \param[out]  data          Pointer to buffer for data to receive from I2C Slave
  \param[in]   num           Number of data bytes to receive
  \param[in]   xfer_pending  Transfer operation is pending - Stop condition will not be generated
  \return      \ref execution_status

  \fn          int32_t NDS_I2C_SlaveTransmit (const uint8_t *data, uint32_t num)
  \brief       Start transmitting data as I2C Slave.
  \param[in]   data  Pointer to buffer with data to transmit to I2C Master
  \param[in]   num   Number of data bytes to transmit
  \return      \ref execution_status

  \fn          int32_t NDS_I2C_SlaveReceive (uint8_t *data, uint32_t num)
  \brief       Start receiving data as I2C Slave.
  \param[out]  data  Pointer to buffer for data to receive from I2C Master
  \param[in]   num   Number of data bytes to receive
  \return      \ref execution_status

  \fn          int32_t NDS_I2C_GetDataCount (void)
  \brief       Get transferred data count.
  \return      number of data bytes transferred; -1 when Slave is not addressed by Master

  \fn          int32_t NDS_I2C_Control (uint32_t control, uint32_t arg)
  \brief       Control I2C Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      \ref execution_status

  \fn          NDS_I2C_STATUS NDS_I2C_GetStatus (void)
  \brief       Get I2C status.
  \return      I2C status \ref NDS_I2C_STATUS

  \fn          void NDS_I2C_SignalEvent (uint32_t event)
  \brief       Signal I2C Events.
  \param[in]   event  \ref I2C_events notification mask
*/

typedef void (*NDS_I2C_SignalEvent_t) (uint32_t event);  ///< Pointer to \ref NDS_I2C_SignalEvent : Signal I2C Event.


/**
\brief I2C Driver Capabilities.
*/
typedef struct _NDS_I2C_CAPABILITIES {
  uint32_t address_10_bit : 1;          ///< supports 10-bit addressing
} NDS_I2C_CAPABILITIES;


/**
\brief Access structure of the I2C Driver.
*/
typedef struct _NDS_DRIVER_I2C {
  NDS_DRIVER_VERSION   (*GetVersion)     (void);                                                                ///< Pointer to \ref NDS_I2C_GetVersion : Get driver version.
  NDS_I2C_CAPABILITIES (*GetCapabilities)(void);                                                                ///< Pointer to \ref NDS_I2C_GetCapabilities : Get driver capabilities.
  int32_t              (*Initialize)     (NDS_I2C_SignalEvent_t cb_event);                                      ///< Pointer to \ref NDS_I2C_Initialize : Initialize I2C Interface.
  int32_t              (*Uninitialize)   (void);                                                                ///< Pointer to \ref NDS_I2C_Uninitialize : De-initialize I2C Interface.
  int32_t              (*PowerControl)   (NDS_POWER_STATE state);                                               ///< Pointer to \ref NDS_I2C_PowerControl : Control I2C Interface Power.
  int32_t              (*MasterTransmit) (uint32_t addr, const uint8_t *data, uint32_t num, bool xfer_pending); ///< Pointer to \ref NDS_I2C_MasterTransmit : Start transmitting data as I2C Master.
  int32_t              (*MasterReceive)  (uint32_t addr,       uint8_t *data, uint32_t num, bool xfer_pending); ///< Pointer to \ref NDS_I2C_MasterReceive : Start receiving data as I2C Master.
  int32_t              (*SlaveTransmit)  (               const uint8_t *data, uint32_t num);                    ///< Pointer to \ref NDS_I2C_SlaveTransmit : Start transmitting data as I2C Slave.
  int32_t              (*SlaveReceive)   (                     uint8_t *data, uint32_t num);                    ///< Pointer to \ref NDS_I2C_SlaveReceive : Start receiving data as I2C Slave.
  int32_t              (*GetDataCount)   (void);                                                                ///< Pointer to \ref NDS_I2C_GetDataCount : Get transferred data count.
  int32_t              (*Control)        (uint32_t control, uint32_t arg);                                      ///< Pointer to \ref NDS_I2C_Control : Control I2C Interface.
  NDS_I2C_STATUS       (*GetStatus)      (void);                                                                ///< Pointer to \ref NDS_I2C_GetStatus : Get I2C status.
} const NDS_DRIVER_I2C;

#endif /* __DRIVER_I2C_H */
