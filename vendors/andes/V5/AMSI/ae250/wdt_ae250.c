/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "wdt_ae250.h"

#include "RTE_Device.h"

#define NDS_WDT_DRV_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,9)

#if (!RTE_WDT)
	#error "WDT not enabled in RTE_Device.h!"
#endif

// Driver Version
static const NDS_DRIVER_VERSION wdt_driver_version = { NDS_WDT_API_VERSION, NDS_WDT_DRV_VERSION };

// WDT
#if (RTE_WDT)
static WDT_INFO wdt_info = {0};
#endif

static const WDT_RESOURCES wdt = {
	{
	1,
	1,
	},
	AE250_WDT,
	&wdt_info
};

/**
  \fn          NDS_DRIVER_VERSION wdt_get_version (void)
  \brief       Get driver version.
  \return      \ref NDS_DRIVER_VERSION
*/
static NDS_DRIVER_VERSION wdt_get_version(void) {
	return wdt_driver_version;
}

/**
  \fn          NDS_WDT_CAPABILITIES wdt_get_capabilities (void)
  \brief       Get driver capabilities
  \param[in]   wdt     Pointer to WDT resources
  \return      \ref NDS_WDT_CAPABILITIES
*/
static NDS_WDT_CAPABILITIES wdt_get_capabilities() {
	return wdt.capabilities;
}

/**
  \fn          int32_t wdt_initialize (NDS_WDT_SignalEvent_t cb_event)
  \brief       Initialize WDT Interface.
  \param[in]   cb_event  Pointer to \ref NDS_WDT_SignalEvent
  \return      \ref execution_status
*/
static int32_t wdt_initialize(NDS_WDT_SignalEvent_t cb_event) {
	if (wdt.info->flags & WDT_FLAG_INITIALIZED) {
		// Driver is already initialized
		return NDS_DRIVER_OK;
	}

	wdt.info->cb_event = cb_event;
	wdt.info->flags = WDT_FLAG_INITIALIZED;

	return NDS_DRIVER_OK;
}

/**
  \fn          int32_t wdt_uninitialize (void)
  \brief       De-initialize WDT Interface.
  \return      \ref execution_status
*/
static int32_t wdt_uninitialize(void) {
	// Reset WDT status flags
	wdt.info->flags = 0U;
	wdt.info->cb_event = NULL;

	return NDS_DRIVER_OK;
}

/**
  \fn          void NDS_WDT_RestartTimer (void) 
  \brief       Restart WDT Timer
  \return      none
*/
static void wdt_restart_timer(void) {
	wdt.reg->WREN = WDT_WREN_NUM;
	wdt.reg->RESTART = WDT_RESTART_NUM;
}

/**
  \fn          void NDS_WDT_ClearIrq (void) 
  \brief       Clear WDT IRQ Expired status
  \return      none
*/
static void wdt_clear_irq_status(void) {
	wdt.reg->ST = WDT_ST_INTEXPIRED_CLR;
}

/**
  \fn          int32_t wdt_control (uint32_t control, uint32_t arg)
  \brief       Control WDT Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \return      common \ref execution_status and driver specific \ref wdt_execution_status
*/
static int32_t wdt_control(uint32_t control, uint32_t arg) {
	if ((wdt.info->flags & WDT_FLAG_INITIALIZED) == 0U) {
		// WDT not initialized
		return NDS_DRIVER_ERROR;
	}

	uint32_t time_period = arg;
	uint32_t ctrl = 0;

	// enable WDT RST/INT stages.
	ctrl |= (WDT_CTRL_RSTEN | WDT_CTRL_INTEN);
	// set RST time to minimum.
	ctrl |= WDT_CTRL_RSTTIME_POW_2_7;

	switch (control & NDS_WDT_CLKSRC_Msk) {
		case NDS_WDT_CLKSRC_APB:      ctrl |= WDT_CTRL_APBCLK; break;
		case NDS_WDT_CLKSRC_EXTERNAL: ctrl |= WDT_CTRL_EXTCLK; break;
	}

	switch (time_period) {
		case NDS_WDT_TIME_POW_2_6:  ctrl |= WDT_CTRL_INTTIME_POW_2_6;  break;
		case NDS_WDT_TIME_POW_2_8:  ctrl |= WDT_CTRL_INTTIME_POW_2_8;  break;
		case NDS_WDT_TIME_POW_2_10: ctrl |= WDT_CTRL_INTTIME_POW_2_10; break;
		case NDS_WDT_TIME_POW_2_11: ctrl |= WDT_CTRL_INTTIME_POW_2_11; break;
		case NDS_WDT_TIME_POW_2_12: ctrl |= WDT_CTRL_INTTIME_POW_2_12; break;
		case NDS_WDT_TIME_POW_2_13: ctrl |= WDT_CTRL_INTTIME_POW_2_13; break;
		case NDS_WDT_TIME_POW_2_14: ctrl |= WDT_CTRL_INTTIME_POW_2_14; break;
		case NDS_WDT_TIME_POW_2_15: ctrl |= WDT_CTRL_INTTIME_POW_2_15; break;
		default: return NDS_WDT_ERROR_TIME_PREIOD;
	}

	wdt_restart_timer();
	wdt_clear_irq_status();

	wdt.reg->WREN = WDT_WREN_NUM;
	wdt.reg->CTRL = ctrl;

	return NDS_DRIVER_OK;
}

/**
  \fn          void NDS_WDT_Enable (void)
  \brief       Enable WDT Timer
  \return      none
*/
static void wdt_enable(void) {
	uint32_t ctrl = wdt.reg->CTRL;
	ctrl |= WDT_CTRL_EN;
	wdt.reg->WREN = WDT_WREN_NUM;
	wdt.reg->CTRL = ctrl;
}

/**
  \fn          void NDS_WDT_Disable (void)
  \brief       Disable WDT Timer
  \return      none
*/
static void wdt_disable(void) {
	uint32_t ctrl = wdt.reg->CTRL;
	ctrl &= ~WDT_CTRL_EN;
	wdt.reg->WREN = WDT_WREN_NUM;
	wdt.reg->CTRL = ctrl;
}

/**
  \fn          NDS_WDT_STATUS wdt_get_status (void)
  \brief       Get WDT status.
  \return      WDT status \ref NDS_WDT_STATUS
*/
static NDS_WDT_STATUS wdt_get_status(void) {
	NDS_WDT_STATUS status = {0};

	uint32_t status_reg = wdt.reg->ST;
	if( status_reg & WDT_ST_INTEXPIRED ){
		status.irq_expired = 1;
	}

	return status;
}

NDS_DRIVER_WDT Driver_WDT = {
	wdt_get_version,
	wdt_get_capabilities,
	wdt_initialize,
	wdt_uninitialize,
	wdt_control,
	wdt_enable,
	wdt_disable,
	wdt_restart_timer,
	wdt_clear_irq_status,
	wdt_get_status,
};

/* Interrupt handlers */
void nmi_handler(void) {
	NDS_WDT_STATUS status = wdt_get_status();

	if( status.irq_expired ){
		wdt.info->cb_event(0);
	}

	// wait WDT reset signal to reset system.
	while(1);
}
