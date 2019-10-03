/*
 * Project:      PWM (Pulse Width Modulation)
 *               Driver definitions
 */

#ifndef __DRIVER_PWM_H
#define __DRIVER_PWM_H

#include "Driver_Common.h"

#define NDS_PWM_API_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,02)  /* API version */


/****** PWM Control Codes *****/

#define NDS_PWM_CONTROL_Msk                 (0xFFUL)

#define NDS_PWM_ACTIVE_CONFIGURE            (0x01UL)   ///< Configure PWM ; arg1 = PWM channel number

/*----- PWM Control Codes: Configure Parameters: PARK Mode -----*/
#define NDS_PWM_PARK_BITS_Pos               8
#define NDS_PWM_PARK_BITS_Msk               (1UL << NDS_PWM_PARK_BITS_Pos)
#define NDS_PWM_PARK_LOW                    (0UL << NDS_PWM_PARK_BITS_Pos)    ///< Park LOW (default)
#define NDS_PWM_PARK_HIGH                   (1UL << NDS_PWM_PARK_BITS_Pos)    ///< Park HIGH

/*----- PWM Control Codes: Configure Parameters: Clock Source -----*/
#define NDS_PWM_CLKSRC_Pos                  9
#define NDS_PWM_CLKSRC_Msk                  (1UL << NDS_PWM_CLKSRC_Pos)
#define NDS_PWM_CLKSRC_SYSTEM               (0UL << NDS_PWM_CLKSRC_Pos)       ///< System clock (default)
#define NDS_PWM_CLKSRC_EXTERNAL             (1UL << NDS_PWM_CLKSRC_Pos)       ///< External clock


/**
\brief PWM Status
*/
typedef struct _NDS_PWM_STATUS {
  uint32_t configured    : 8;               ///< PWM channel state: 1=Configured, 0=Unconfigured
  uint32_t output        : 8;               ///< PWM channel output: 1=Enable, 0=Disable
} NDS_PWM_STATUS;


/****** PWM Event *****/
#define NDS_PWM_EVENT_ONESTEP_COMPLETE      (1UL << 0)  ///< One step complete


// Function documentation
/**
  \fn          NDS_DRIVER_VERSION NDS_PWM_GetVersion (void)
  \brief       Get driver version.
  \return      \ref NDS_DRIVER_VERSION

  \fn          NDS_PWM_CAPABILITIES NDS_PWM_GetCapabilities (void)
  \brief       Get driver capabilities
  \return      \ref NDS_PWM_CAPABILITIES

  \fn          int32_t NDS_PWM_Initialize (NDS_PWM_SignalEvent_t cb_event)
  \brief       Initialize PWM Interface.
  \param[in]   cb_event  Pointer to \ref NDS_PWM_SignalEvent
  \return      \ref execution_status

  \fn          int32_t NDS_PWM_Uninitialize (void)
  \brief       De-initialize PWM Interface.
  \return      \ref execution_status

  \fn          int32_t NDS_PWM_PowerControl (NDS_POWER_STATE state)
  \brief       Control PWM Interface Power.
  \param[in]   state  Power state
  \return      \ref execution_status

  \fn          int32_t NDS_PWM_Control (uint32_t control, uint32_t arg)
  \brief       Control PWM Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      common \ref execution_status and driver specific \ref PWM_execution_status

  \fn          int32_t NDS_PWM_SetFreq (uint8_t ch_num, uint32_t freq)
  \brief       Set PWM Interface frequency
  \param[in]   ch_num  PWM channel number
  \param[in]   freq    Frequency
  \return      \ref execution_status

  \fn          int32_t NDS_PWM_Output (uint8_t ch_num, uint8_t duty)
  \brief       Output PWM pulse
  \param[in]   ch_num  PWM channel number
  \param[in]   duty    Duty length (0 ~ 255)
  \return      \ref execution_status

  \fn          NDS_PWM_STATUS NDS_PWM_GetStatus (void)
  \brief       Get PWM status.
  \return      PWM status \ref NDS_PWM_STATUS

  \fn          void NDS_PWM_SignalEvent (uint32_t event)
  \brief       Signal PWM Events.
  \param[in]   event  \ref PWM_events notification mask
  \return      none
*/

typedef void (*NDS_PWM_SignalEvent_t) (uint32_t ch_num, uint32_t event);  ///< Pointer to \ref NDS_PWM_SignalEvent : Signal PWM Event.


/**
\brief PWM Device Driver Capabilities.
*/
typedef struct _NDS_PWM_CAPABILITIES {
  uint32_t channels               : 4;  ///< supports PWM channel numbers
  uint32_t event_onestep_complete : 1;  ///< Signal one step complete event: \ref NDS_PWM_EVENT_ONESTEP_COMPLETE
} NDS_PWM_CAPABILITIES;


/**
\brief Access structure of the PWM Driver.
*/
typedef struct _NDS_DRIVER_PWM {
  NDS_DRIVER_VERSION     (*GetVersion)      (void);                              ///< Pointer to \ref NDS_PWM_GetVersion : Get driver version.
  NDS_PWM_CAPABILITIES   (*GetCapabilities) (void);                              ///< Pointer to \ref NDS_PWM_GetCapabilities : Get driver capabilities.
  int32_t                (*Initialize)      (NDS_PWM_SignalEvent_t cb_event);    ///< Pointer to \ref NDS_PWM_Initialize : Initialize PWM Interface.
  int32_t                (*Uninitialize)    (void);                              ///< Pointer to \ref NDS_PWM_Uninitialize : De-initialize PWM Interface.
  int32_t                (*PowerControl)    (NDS_POWER_STATE state);             ///< Pointer to \ref NDS_PWM_PowerControl : Control PWM Interface Power.
  int32_t                (*Control)         (uint32_t control, uint32_t arg);    ///< Pointer to \ref NDS_PWM_Control : Control PWM Interface.
  int32_t                (*SetFreq)         (uint8_t pwm, uint32_t freq);        ///< Pointer to \ref NDS_PWM_SetFreq : Set the
  int32_t                (*Output)          (uint8_t pwm, uint8_t duty);         ///< Pointer to \ref NDS_PWM_Output : Start PWM output
  NDS_PWM_STATUS         (*GetStatus)       (void);                              ///< Pointer to \ref NDS_PWM_GetStatus : Get PWM status.
} const NDS_DRIVER_PWM;

#endif /* __DRIVER_PWM_H */
