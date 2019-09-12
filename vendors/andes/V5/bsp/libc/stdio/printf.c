/*
	putchar is the only external dependency for this file,
	if you have a working putchar, leave it commented out.
	If not, uncomment the define below and
	replace outbyte(c) by your own function call.
*/

#define putchar(c) outbyte(c)

#include <stdarg.h>	/* va_list, va_arg() */

#include "do_printf.h"

/*****************************************************************************
 * PRINTF You must write your own putchar()
 *****************************************************************************/
static int vprintf_help(unsigned c, void **ptr){

	extern int putchar(int c);

	ptr = ptr; /* to avoid unused varible warning */
	putchar(c);

	return 0;
}

static int vsprintf_help(unsigned int c, void **ptr){

	char *dst = *ptr;
	*dst++ = c;
	*ptr = dst;

	return 0 ;
}

int vsprintf(char *buffer, const char *fmt, va_list args){

	int ret_val = do_printf(fmt, args, vsprintf_help, (void *)buffer);
	buffer[ret_val] = '\0';

	return ret_val;
}

int sprintf(char *buffer, const char *fmt, ...){

	va_list args;
	int ret_val;

	va_start(args, fmt);
	ret_val = vsprintf(buffer, fmt, args);
	va_end(args);

	return ret_val;
}

int vprintf(const char *fmt, va_list args){

	return do_printf(fmt, args, vprintf_help, (void *)0);
}

int printf(const char *fmt, ...){

	va_list args;
	int ret_val;

	va_start(args, fmt);
	ret_val = vprintf(fmt, args);
	va_end(args);

	return ret_val;
}
