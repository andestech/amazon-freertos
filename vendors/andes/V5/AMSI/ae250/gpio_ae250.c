/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "gpio_ae250.h"

#define NDS_GPIO_DRV_VERSION NDS_DRIVER_VERSION_MAJOR_MINOR(2,9)

// driver version
static const NDS_DRIVER_VERSION gpio_driver_version = {NDS_GPIO_API_VERSION, NDS_GPIO_DRV_VERSION};

static GPIO_INFO gpio_info = {0};

static void gpio_set_intr_mode(uint32_t intr_mode, uint32_t mask) {
	uint32_t i;

	for (i = 0; i < GPIO_MAX_PIN_NUM; i++) {
		if (mask & (0x1 << i)) {
			if ((i >= 0) & (i < 8)) {
				AE250_GPIO->INTRMODE0 &= ~(7 << (i * 4));
				AE250_GPIO->INTRMODE0 |= (intr_mode << (i * 4));
			} else if ((i >= 8) & (i < 16)) {
				AE250_GPIO->INTRMODE1 &= ~(7 << ((i-8) * 4));
				AE250_GPIO->INTRMODE1 |= (intr_mode << ((i-8) * 4));
			} else if ((i >= 16) & (i < 24)) {
				AE250_GPIO->INTRMODE2 &= ~(7 << ((i-16) * 4));
				AE250_GPIO->INTRMODE2 |= (intr_mode << ((i-16) * 4));
			} else if ((i >= 24) & (i < 32)) {
				AE250_GPIO->INTRMODE3 &= ~(7 << ((i-24) * 4));
				AE250_GPIO->INTRMODE3 |= (intr_mode << ((i-24) * 4));
			}
		}
	}
}

void gpio_irq_handler() {
	uint32_t status;
	uint32_t i;

	status = AE250_GPIO->INTRSTATUS;

	// write 1 to clear interrupt status
	AE250_GPIO->INTRSTATUS = status;

	for (i = 0; i < GPIO_MAX_PIN_NUM; i++) {
		if (status & (0x1 << i)) {
			gpio_info.cb_event((1UL << i));
		}
	}
}

NDS_DRIVER_VERSION gpio_get_version(void) {
	return gpio_driver_version;
}

int32_t gpio_initialize(NDS_GPIO_SignalEvent_t cb_event) {

        // disable all interrupts
	AE250_GPIO->INTREN = 0;

        // Write 1 to clear interrupt status
	AE250_GPIO->INTRSTATUS = 0xffffffff;

        // Priority must be set > 0 to trigger the interrupt
	__nds__plic_set_priority(IRQ_GPIO_SOURCE, 1);

	// Enable PLIC interrupt GPIO source
	__nds__plic_enable_interrupt(IRQ_GPIO_SOURCE);

	// Enable the Machine-External bit in MIE
	set_csr(NDS_MIE, MIP_MEIP);

	// Enable GIE
	set_csr(NDS_MSTATUS, MSTATUS_MIE);

	gpio_info.cb_event = cb_event;

	return NDS_DRIVER_OK;
}

int32_t gpio_uninitialize(void) {

	// disable all interrupts
	AE250_GPIO->INTREN = 0;

        // write 1 to clear interrupt status
	AE250_GPIO->INTRSTATUS = 0xffffffff;

	// Disable PLIC interrupt GPIO source
	__nds__plic_disable_interrupt(IRQ_GPIO_SOURCE);

	gpio_info.cb_event = NULL;

	return NDS_DRIVER_OK;
}

void gpio_pinwrite(uint32_t pin_num, int32_t val) {
	val ? (AE250_GPIO->DOUTSET = 1UL << pin_num) : (AE250_GPIO->DOUTCLEAR = (1UL << pin_num));
}

uint8_t gpio_pinread(uint32_t pin_num) {
	return ((AE250_GPIO->DATAIN & (1UL << pin_num)) ? (1) : (0));
}

void gpio_write(uint32_t mask, uint32_t val) {
	val ? (AE250_GPIO->DOUTSET = mask) : (AE250_GPIO->DOUTCLEAR = mask);
}

uint32_t gpio_read(void) {
	return (AE250_GPIO->DATAIN);
}

void gpio_setdir(uint32_t mask, int32_t dir) {
	dir ? (AE250_GPIO->CHANNELDIR |= mask) : (AE250_GPIO->CHANNELDIR &= ~mask);
}

int32_t gpio_control(uint32_t mode, uint32_t val) {

	// set interrupt mode
	if (mode & NDS_GPIO_SET_INTR_HIGH_LEVEL) {
		gpio_set_intr_mode(GPIO_INTR_HIGH_LEVEL, val);
	} else if (mode & NDS_GPIO_SET_INTR_LOW_LEVEL) {
		gpio_set_intr_mode(GPIO_INTR_LOW_LEVEL, val);
	} else if (mode & NDS_GPIO_SET_INTR_NEGATIVE_EDGE) {
		gpio_set_intr_mode(GPIO_INTR_NEGATIVE_EDGE, val);
	} else if (mode & NDS_GPIO_SET_INTR_POSITIVE_EDGE) {
		gpio_set_intr_mode(GPIO_INTR_POSITIVE_EDGE, val);
	} else if (mode & NDS_GPIO_SET_INTR_DUAL_EDGE) {
		gpio_set_intr_mode(GPIO_INTR_DUAL_EDGE, val);
	}

	// interrupt disable/enable
	if (mode & NDS_GPIO_INTR_DISABLE) {
		AE250_GPIO->INTREN &= ~val;
	} else if (mode & NDS_GPIO_INTR_ENABLE) {
		AE250_GPIO->INTREN |= val;
	}

	return NDS_DRIVER_OK;
}

// GPIO driver control block
NDS_DRIVER_GPIO Driver_GPIO = {
	gpio_get_version,
	gpio_initialize,
	gpio_uninitialize,
	gpio_pinwrite,
	gpio_pinread,
	gpio_write,
	gpio_read,
	gpio_setdir,
	gpio_control
};
