/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include <stdio.h>
#include "platform.h"

typedef struct {
	union {
		struct {
			long x1;
			long x4;
			long x5;
			long x6;
			long x7;
			long x10;
			long x11;
			long x12;
			long x13;
			long x14;
			long x15;
			long x16;
			long x17;
			long x28;
			long x29;
			long x30;
			long x31;
		};
		long caller_regs[17];
	};
	long mepc;
	long mstatus;
	long mxstatus;
#ifdef __riscv_flen
#if __riscv_flen == 64
	long long fp_caller_regs[20];
#else
	long fp_caller_regs[20];
#endif
	int fcsr;
#endif
} SAVED_CONTEXT;

extern void mext_interrupt(void);

__attribute__((weak)) void mtime_handler(void)
{
	clear_csr(NDS_MIE, MIP_MTIP);
}

__attribute__((weak)) void mswi_handler(void)
{
	clear_csr(NDS_MIE, MIP_MSIP);
}

__attribute__((weak)) void syscall_handler(long n, long a0, long a1, long a2, long a3)
{
	printf("syscall #%ld (a0:0x%lx,a1:0x%lx, a2:0x%lx, a3:0x%lx)\n", n, a0, a1, a2, a3);
}

__attribute__((weak)) long except_handler(long cause, long epc, long *reg)
{
	/* Unhandled Trap */
	printf("Unhandled Trap : mcause = 0x%x, mepc = 0x%x\n", (unsigned int)cause, (unsigned int)epc);
	while(1);
	return epc;
}

void trap_handler(unsigned long mcause, SAVED_CONTEXT *context)
{
	/* Do your trap handling */
	if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_EXT)) {
		/* Machine-level interrupt from PLIC */
		mext_interrupt();
	} else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)) {
		/* Machine timer interrupt */
		mtime_handler();
	} else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_SOFT)) {
		/* Machine SWI interrupt */
		mswi_handler();
	} else if (!(mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == TRAP_M_ECALL)) {
		/* Machine Syscal call */
#ifdef __riscv_32e
		syscall_handler(context->x5, context->x10, context->x11, context->x12, context->x13);
#else
		syscall_handler(context->x17, context->x10, context->x11, context->x12, context->x13);
#endif
		context->mepc += 4;
	}else {
		/* Unhandled Trap */
		context->mepc = except_handler(mcause, context->mepc, context->caller_regs);
	}
}

/*
 * The portasmHANDLE_INTERRUPT is defined to FreeRTOS_IRQ_handler() to provide handling
 * external interrupts. Since we don't have CLINT, we also handle machine timer and software
 * interrupts.
 */
__attribute__((naked)) void FreeRTOS_IRQ_handler(unsigned long mcause)
{
	if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_TIMER)) {
		__asm volatile( "tail FreeRTOS_tick_handler" );
	} else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_EXT)) {
		__asm volatile( "tail mext_interrupt" );
	} else if ((mcause & MCAUSE_INT) && ((mcause & MCAUSE_CAUSE) == IRQ_M_SOFT)) {
		__asm volatile( "tail mswi_handler" );
	}

	while (1) {
		__asm volatile( "ebreak" );
	}
}
