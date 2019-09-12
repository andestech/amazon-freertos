/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifndef __UART_H__
#define __UART_H__

#include "platform.h"

/* IER Register (+0x24) */
#define UARTC_IER_RDR                   0x01 /* Data Ready Enable */
#define UARTC_IER_THRE                  0x02 /* THR Empty Enable */
#define UARTC_IER_RLS                   0x04 /* Receive Line Status Enable */
#define UARTC_IER_MS                    0x08 /* Modem Staus Enable */

/* IIR Register (+0x28) */
#define UARTC_IIR_NONE                  0x01 /* No interrupt pending */
#define UARTC_IIR_RLS                   0x06 /* Receive Line Status */
#define UARTC_IIR_RDA                   0x04 /* Receive Data Available */
#define UARTC_IIR_RTO                   0x0c /* Receive Time Out */
#define UARTC_IIR_THRE                  0x02 /* THR Empty */
#define UARTC_IIR_MODEM                 0x00 /* Modem Status */
#define UARTC_IIR_INT_MASK              0x0f /* Initerrupt Status Bits Mask */

#define UARTC_IIR_TFIFO_FULL            0x10 /* TX FIFO full */
#define UARTC_IIR_FIFO_EN               0xc0 /* FIFO mode is enabled, set when FCR[0] is 1 */

/* FCR Register (+0x28) */
#define UARTC_FCR_FIFO_EN               0x01 /* FIFO Enable */
#define UARTC_FCR_RFIFO_RESET           0x02 /* Rx FIFO Reset */
#define UARTC_FCR_TFIFO_RESET           0x04 /* Tx FIFO Reset */
#define UARTC_FCR_DMA_EN                0x08 /* Select UART DMA mode */

#define UARTC_FCR_TFIFO16_TRGL1         0x00 /* TX 16-byte FIFO int trigger level - 1 char */
#define UARTC_FCR_TFIFO16_TRGL3         0x10 /* TX 16-byte FIFO int trigger level - 3 char */
#define UARTC_FCR_TFIFO16_TRGL9         0x20 /* TX 16-byte FIFO int trigger level - 9 char */
#define UARTC_FCR_TFIFO16_TRGL13        0x30 /* TX 16-byte FIFO int trigger level - 13 char */

#define UARTC_FCR_RFIFO16_TRGL1         0x00 /* RX 16-byte FIFO int trigger level - 1 char */
#define UARTC_FCR_RFIFO16_TRGL4         0x40 /* RX 16-byte FIFO int trigger level - 4 char */
#define UARTC_FCR_RFIFO16_TRGL8         0x80 /* RX 16-byte FIFO int trigger level - 8 char */
#define UARTC_FCR_RFIFO16_TRGL14        0xc0 /* RX 16-byte FIFO int trigger level - 14 char */

#define UARTC_FCR_FIFO_EN_MASK          0x01 /* FIFO Enable */
#define UARTC_FCR_FIFO_EN_BIT           0
#define UARTC_FCR_RFIFO_RESET_MASK      0x02 /* Rx FIFO Reset */
#define UARTC_FCR_RFIFO_RESET_BIT       1
#define UARTC_FCR_TFIFO_RESET_MASK      0x04 /* Tx FIFO Reset */
#define UARTC_FCR_TFIFO_RESET_BIT       2
#define UARTC_FCR_DMA_EN_MASK           0x08 /* Select UART DMA mode */
#define UARTC_FCR_DMA_EN_BIT            3

#define UARTC_FCR_TXFIFO_TRGL_MASK      0x30 /* TX FIFO int trigger level */
#define UARTC_FCR_TXFIFO_TRGL_SHIFT     4
#define UARTC_FCR_RXFIFO_TRGL_MASK      0xc0 /* RX FIFO int trigger level */
#define UARTC_FCR_RXFIFO_TRGL_SHIFT     6

/* LCR Register (+0x2c) */
#define UARTC_LCR_BITS5                 0x00
#define UARTC_LCR_BITS6                 0x01
#define UARTC_LCR_BITS7                 0x02
#define UARTC_LCR_BITS8                 0x03
#define UARTC_LCR_STOP1                 0x00
#define UARTC_LCR_STOP2                 0x04

#define UARTC_LCR_PARITY_EN             0x08 /* Parity Enable */
#define UARTC_LCR_PARITY_NONE           0x00 /* No Parity Check */
#define UARTC_LCR_PARITY_EVEN           0x18 /* Even Parity */
#define UARTC_LCR_PARITY_ODD            0x08 /* Odd Parity */
#if 0
#define UARTC_LCR_PARITY_1              0x21 /* 1 Parity Bit */
#define UARTC_LCR_PARITY_0              0x31 /* 0 Parity Bit */
#endif
#define UARTC_LCR_SETBREAK              0x40 /* Set Break condition */
#define UARTC_LCR_DLAB                  0x80 /* Divisor Latch Access Bit */

/* MCR Register (+0x30) */
#define UARTC_MCR_DTR                   0x01 /* Data Terminal Ready */
#define UARTC_MCR_RTS                   0x02 /* Request to Send */
#define UARTC_MCR_OUT1                  0x04 /* output1 */
#define UARTC_MCR_OUT2                  0x08 /* output2 or global interrupt enable */
#define UARTC_MCR_LPBK                  0x10 /* loopback mode */
#define UARTC_MCR_AFE                   0x20 /* Auto flow control */

/* LSR Register (+0x34) */
#define UARTC_LSR_RDR                   0x1 /* Data Ready */
#define UARTC_LSR_OE                    0x2 /* Overrun Error */
#define UARTC_LSR_PE                    0x4 /* Parity Error */
#define UARTC_LSR_FE                    0x8 /* Framing Error */
#define UARTC_LSR_BI                    0x10 /* Break Interrupt */
#define UARTC_LSR_THRE                  0x20 /* THR/FIFO Empty */
#define UARTC_LSR_TEMT                  0x40 /* THR/FIFO and TFR Empty */
#define UARTC_LSR_ERRF                  0x80 /* FIFO Data Error */

/* MSR Register (+0x38) */
#define UARTC_MSR_DCTS                  0x1 /* Delta CTS */
#define UARTC_MSR_DDSR                  0x2 /* Delta DSR */
#define UARTC_MSR_TERI                  0x4 /* Trailing Edge RI */
#define UARTC_MSR_DDCD                  0x8 /* Delta CD */
#define UARTC_MSR_CTS                   0x10 /* Clear To Send */
#define UARTC_MSR_DSR                   0x20 /* Data Set Ready */
#define UARTC_MSR_RI                    0x40 /* Ring Indicator */
#define UARTC_MSR_DCD                   0x80 /* Data Carrier Detect */

extern void uart_init(UART_RegDef *UARTx);
extern void uart_set_baudrate(UART_RegDef *UARTx, unsigned int baudrate);
extern int uart_receive_byte(UART_RegDef *UARTx);
extern void uart_send_byte(UART_RegDef *UARTx, int ch);

#endif /* __UART_H__ */
