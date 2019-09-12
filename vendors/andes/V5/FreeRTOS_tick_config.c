/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Platform includes. */
#include "platform.h"

extern uint64_t ullNextTime;
extern const size_t uxTimerIncrementsForOneTick;

volatile uint64_t * const pullMtimecmpRegister = ( volatile uint64_t * const ) ( configMTIME_BASE_ADDRESS + 8 );

/*-----------------------------------------------------------*/

/*
 * Create implementation of vPortSetupTimerInterrupt() if the CLINT is not
 * available, but make sure the configCLINT_BASE_ADDRESS constant is still
 * defined.
 */
void vPortSetupTimerInterrupt( void )
{
uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
volatile uint32_t * const pulTimeHigh = ( volatile uint32_t * const ) ( configMTIME_BASE_ADDRESS + 4 );
volatile uint32_t * const pulTimeLow = ( volatile uint32_t * const ) ( configMTIME_BASE_ADDRESS );

	do
	{
		ulCurrentTimeHigh = *pulTimeHigh;
		ulCurrentTimeLow = *pulTimeLow;
	} while( ulCurrentTimeHigh != *pulTimeHigh );

	ullNextTime = ( uint64_t ) ulCurrentTimeHigh;
	ullNextTime <<= 32ULL;
	ullNextTime |= ( uint64_t ) ulCurrentTimeLow;
	ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;
	*pullMtimecmpRegister = ullNextTime;

	/* Prepare the time to use after the next tick interrupt. */
	ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;

	/* Enable mtime interrupt */
	__asm volatile( "csrs mie,%0" :: "r"(0x80) );
}
/*-----------------------------------------------------------*/

void FreeRTOS_tick_handler( void )
{
	/* Update the mtimer compare match value. */
	*pullMtimecmpRegister = ullNextTime;

	/* Add ullNextTime to the timer increments for one tick. */
	ullNextTime += ( uint64_t ) uxTimerIncrementsForOneTick;

	if( xTaskIncrementTick() != pdFALSE )
	{
		vTaskSwitchContext();
	}
}
/*-----------------------------------------------------------*/
