/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include <stdio.h>
#include "platform.h"

typedef void (*isr_func)(void);

void default_irq_handler(void)
{
	printf("Default interrupt handler\n");
}


void wdt_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void rtc_period_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void rtc_alarm_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void pit_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void spi1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void spi2_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void spi4_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void i2c_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void gpio_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void uart1_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void uart2_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void dma_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void bmc_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void swint_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void sdc_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void mac_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void standby_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));
void wakeup_irq_handler(void) __attribute__((weak, alias("default_irq_handler")));

const isr_func irq_handler[] = {
	wdt_irq_handler,
	rtc_period_irq_handler,
	rtc_alarm_irq_handler,
	pit_irq_handler,
	spi1_irq_handler,
	spi2_irq_handler,
	i2c_irq_handler,
	gpio_irq_handler,
	uart1_irq_handler,
	uart2_irq_handler,
	dma_irq_handler,
	bmc_irq_handler,
	swint_irq_handler,
	default_irq_handler,
	default_irq_handler,
	default_irq_handler,
	spi4_irq_handler,
	default_irq_handler,
	sdc_irq_handler,
	mac_irq_handler,
	default_irq_handler,
	default_irq_handler,
	default_irq_handler,
	default_irq_handler,
	default_irq_handler,
	default_irq_handler,
	standby_irq_handler,
	wakeup_irq_handler,
	default_irq_handler,
	default_irq_handler,
	default_irq_handler,
	default_irq_handler
};

/* At the time of writing, interrupt nesting is not supported, so do not use
the default mext_interrupt() implementation as that enables interrupts.  A
version that does not enable interrupts is provided below.  THIS INTERRUPT
HANDLER IS SPECIFIC TO FREERTOS WHICH USES PLIC! */
void mext_interrupt(void)
{
	unsigned int irq_source = __nds__plic_claim_interrupt();

	/* Do interrupt handler */
	irq_handler[irq_source]();

	__nds__plic_complete_interrupt(irq_source);
}
