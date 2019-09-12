/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "FreeRTOSConfig.h"

.extern FreeRTOS_tick_handler
.extern mswi_handler
.extern mext_interrupt

	.global FreeRTOS_IRQ_handler
	.type FreeRTOS_IRQ_handler,@function

FreeRTOS_IRQ_handler:
	addi t0, x0, 1

	slli t0, t0, __riscv_xlen - 1   /* LSB is already set, shift into MSB.  Shift 31 on 32-bit or 63 on 64-bit cores. */

	test_if_mtimer:
		addi t1, t0, 7          /* 0x8000[]0007 == machine timer interrupt. */
		bne a0, t1, test_if_software_interrupt
		j FreeRTOS_tick_handler

	test_if_software_interrupt:
		addi t1, t0, 3          /* 0x8000[]0003 == machine software interrupt. */
		bne a0, t1, test_if_external_interrupt
		j mswi_handler

	test_if_external_interrupt:
		addi t1, t0, 11          /* 0x8000[]000b == machine external interrupt. */
		bne a0, t1, IRQ_unhandled    /* Something as yet unhandled. */
		j mext_interrupt

IRQ_unhandled:
	ebreak
	j IRQ_unhandled
