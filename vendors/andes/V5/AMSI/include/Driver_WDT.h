/*
 * Project:      WDT (Watchdog Timer)
 *               Driver definitions
 */

#ifndef __DRIVER_WDT_H
#define __DRIVER_WDT_H

#include "Driver_Common.h"

#define NDS_WDT_API_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,02)  /* API version */

/****** WDT Control Codes *****/

#define NDS_WDT_CONTROL_Pos                0
#define NDS_WDT_CONTROL_Msk               (0xFFUL << NDS_WDT_CONTROL_Pos)

/*----- WDT Control Codes: Clock Source -----*/
#define NDS_WDT_CLKSRC_Pos                 0
#define NDS_WDT_CLKSRC_Msk                (0x1UL << NDS_WDT_CLKSRC_Pos)
#define NDS_WDT_CLKSRC_APB                (0x1UL << NDS_WDT_CLKSRC_Pos)     ///< System clock
#define NDS_WDT_CLKSRC_EXTERNAL           (0x0UL << NDS_WDT_CLKSRC_Pos)     ///< External clock (default)

/*----- WDT Time Period -----*/
#define NDS_WDT_TIME_POW_2_6              (0x0UL)                           ///< WDT timer interval    64 clock periods.
#define NDS_WDT_TIME_POW_2_8              (0x1UL)                           ///< WDT timer interval   256 clock periods.
#define NDS_WDT_TIME_POW_2_10             (0x2UL)                           ///< WDT timer interval  1024 clock periods.
#define NDS_WDT_TIME_POW_2_11             (0x3UL)                           ///< WDT timer interval  2048 clock periods.
#define NDS_WDT_TIME_POW_2_12             (0x4UL)                           ///< WDT timer interval  4096 clock periods.
#define NDS_WDT_TIME_POW_2_13             (0x5UL)                           ///< WDT timer interval  8192 clock periods.
#define NDS_WDT_TIME_POW_2_14             (0x6UL)                           ///< WDT timer interval 16384 clock periods.
#define NDS_WDT_TIME_POW_2_15             (0x7UL)                           ///< WDT timer interval 32768 clock periods.

/****** WDT specific error codes *****/
#define NDS_WDT_ERROR_TIME_PREIOD         (NDS_DRIVER_ERROR_SPECIFIC - 1)   ///< Specified time period not supported

/**
\brief WDT Status
*/
typedef struct _NDS_WDT_STATUS {
  uint32_t irq_expired  : 1;        ///< WDT IRQ stage time expired (in WDT Interrupt)
} NDS_WDT_STATUS;

/****** WDT Event *****/
#define NDS_WDT_EVENT_INT_STAGE (1UL << 0)  ///< WDT Interrupt event by IRQ stage time expired

// Function documentation
/**
  \fn          NDS_DRIVER_VERSION NDS_WDT_GetVersion (void)
  \brief       Get driver version.
  \return      \ref NDS_DRIVER_VERSION

  \fn          NDS_WDT_CAPABILITIES wdt_get_capabilities (WDT_RESOURCES *wdt)
  \brief       Get driver capabilities
  \param[in]   wdt     Pointer to WDT resources
  \return      \ref NDS_WDT_CAPABILITIES

  \fn          int32_t NDS_WDT_Initialize (NDS_WDT_SignalEvent_t cb_event)
  \brief       Initialize WDT Interface.
  \param[in]   cb_event  Pointer to \ref NDS_WDT_SignalEvent
  \return      \ref execution_status

  \fn          int32_t NDS_WDT_Uninitialize (void)
  \brief       De-initialize WDT Interface.
  \return      \ref execution_status

  \fn          int32_t NDS_WDT_Control (uint32_t control, uint32_t arg)
  \brief       Control WDT Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      common \ref execution_status and driver specific \ref usart_execution_status

  \fn          void NDS_WDT_Enable (void)
  \brief       Enable WDT Timer
  \return      none

  \fn          void NDS_WDT_Disable (void)
  \brief       Disable WDT Timer
  \return      none

  \fn          void NDS_WDT_RestartTimer (void) 
  \brief       Restart WDT Timer
  \return      none

  \fn          void NDS_WDT_ClearIrqStatus (void) 
  \brief       Clear WDT IRQ Expired status
  \return      none

  \fn          NDS_WDT_STATUS NDS_WDT_GetStatus (void)
  \brief       Get WDT status.
  \return      WDT status \ref NDS_WDT_STATUS

  \fn          void NDS_WDT_SignalEvent (uint32_t event)
  \brief       Signal WDT Events.
  \param[in]   event  \ref WDT_events notification mask
  \return      none
*/

typedef void (*NDS_WDT_SignalEvent_t) (uint32_t event);  ///< Pointer to \ref NDS_WDT_SignalEvent : Signal WDT Event.

/**
\brief WDT Device Driver Capabilities
*/

typedef struct _NDS_WDT_CAPABILITIES {
  uint32_t external_timer : 1; ///< support using external timer as clock source.
  uint32_t irq_stage      : 1; ///< support interrupt stage before WDT reset.
} NDS_WDT_CAPABILITIES;

/*
\brief Access structure of the WDT Driver.
*/
typedef struct _NDS_DRIVER_WDT {
  NDS_DRIVER_VERSION     (*GetVersion)      (void);                              ///< Pointer to \ref NDS_WDT_GetVersion : Get driver version.
  NDS_WDT_CAPABILITIES   (*GetCapabilities) (void);                              ///< Pointer to \ref NDS_WDT_GetCapabilities : Get driver capabilities.
  int32_t                (*Initialize)      (NDS_WDT_SignalEvent_t cb_event);    ///< Pointer to \ref NDS_WDT_Initialize : Initialize WDT Interface.
  int32_t                (*Uninitialize)    (void);                              ///< Pointer to \ref NDS_WDT_Uninitialize : De-initialize WDT Interface.
  int32_t                (*Control)         (uint32_t control, uint32_t arg);    ///< Pointer to \ref NDS_WDT_Control : Control WDT Interface.
  void                   (*Enable)          (void);                              ///< Pointer to \ref NDS_WDT_Enable : Enable WDT Timer.
  void                   (*Disable)         (void);                              ///< Pointer to \ref NDS_WDT_Disable : Disable WDT Timer.
  void                   (*RestartTimer)    (void);                              ///< Pointer to \ref NDS_WDT_RestartTimer : Restart WDT Timer
  void                   (*ClearIrqStatus)  (void);                              ///< Pointer to \ref NDS_WDT_ClearIrqStatus : Clear WDT IRQ Expired status.
  NDS_WDT_STATUS         (*GetStatus)       (void);                              ///< Pointer to \ref NDS_WDT_GetStatus : Get WDT status.
} const NDS_DRIVER_WDT;

#endif /* __DRIVER_WDT_H */
