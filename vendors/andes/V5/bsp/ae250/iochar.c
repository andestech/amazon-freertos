/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#include "platform.h"
#include "uart.h"

int outbyte(int c)
{
	uart_send_byte(AE250_UART2, c);
	if (c =='\n')
		uart_send_byte(AE250_UART2, '\r');
	return c;
}

