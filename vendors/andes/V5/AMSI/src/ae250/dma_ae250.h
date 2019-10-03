/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifndef __DMA_AE250_H
#define __DMA_AE250_H

#include <stdint.h>

#include "platform.h"

// Number of DMA channels
#define DMA_NUMBER_OF_CHANNELS           ((uint8_t) 8)

// GPDMA Events
#define DMA_EVENT_TERMINAL_COUNT_REQUEST (1)
#define DMA_EVENT_ERROR                  (2)
#define DMA_EVENT_ABORT                  (3)

// Source burst size in Source and Destination definitions
#define DMA_BSIZE_1                      (0)    // Burst size = 1
#define DMA_BSIZE_2                      (1)    // Burst size = 2
#define DMA_BSIZE_4                      (2)    // Burst size = 4
#define DMA_BSIZE_8                      (3)    // Burst size = 8
#define DMA_BSIZE_16                     (4)    // Burst size = 16
#define DMA_BSIZE_32                     (5)    // Burst size = 32
#define DMA_BSIZE_64                     (6)    // Burst size = 64
#define DMA_BSIZE_128                    (7)    // Burst size = 128

// Width in Source transfer width and Destination transfer width definitions
#define DMA_WIDTH_BYTE                   (0)    // Width = 1 byte
#define DMA_WIDTH_HALFWORD               (1)    // Width = 2 bytes
#define DMA_WIDTH_WORD                   (2)    // Width = 4 bytes

// DMA Channel Control register definition
#define DMA_CH_CTRL_PRIORITY_HIGH        (   1 << 29)
#define DMA_CH_CTRL_SBSIZE_POS           (        22)
#define DMA_CH_CTRL_SBSIZE_MASK          (0x07 << DMA_CH_CTRL_SBSIZE_POS)
#define DMA_CH_CTRL_SBSIZE(n)            (((n) << DMA_CH_CTRL_SBSIZE_POS) & DMA_CH_CTRL_SBSIZE_MASK)
#define DMA_CH_CTRL_SWIDTH_POS           (        20)
#define DMA_CH_CTRL_SWIDTH_MASK          (0x03 << DMA_CH_CTRL_SWIDTH_POS)
#define DMA_CH_CTRL_SWIDTH(n)            (((n) << DMA_CH_CTRL_SWIDTH_POS) & DMA_CH_CTRL_SWIDTH_MASK)
#define DMA_CH_CTRL_DWIDTH_POS           (        18)
#define DMA_CH_CTRL_DWIDTH_MASK          (0x03 << DMA_CH_CTRL_DWIDTH_POS)
#define DMA_CH_CTRL_DWIDTH(n)            (((n) << DMA_CH_CTRL_DWIDTH_POS) & DMA_CH_CTRL_DWIDTH_MASK)
#define DMA_CH_CTRL_SMODE_HANDSHAKE      (   1 << 17)
#define DMA_CH_CTRL_DMODE_HANDSHAKE      (   1 << 16)
#define DMA_CH_CTRL_SRCADDRCTRL_POS      (        14)
#define DMA_CH_CTRL_SRCADDRCTRL_MASK     (0x03 << DMA_CH_CTRL_SRCADDRCTRL_POS)
#define DMA_CH_CTRL_SRCADDR_INC          (   0 << DMA_CH_CTRL_SRCADDRCTRL_POS)
#define DMA_CH_CTRL_SRCADDR_DEC          (   1 << DMA_CH_CTRL_SRCADDRCTRL_POS)
#define DMA_CH_CTRL_SRCADDR_FIX          (   2 << DMA_CH_CTRL_SRCADDRCTRL_POS)
#define DMA_CH_CTRL_DSTADDRCTRL_POS      (        12)
#define DMA_CH_CTRL_DSTADDRCTRL_MASK     (0x03 << DMA_CH_CTRL_DSTADDRCTRL_POS)
#define DMA_CH_CTRL_DSTADDR_INC          (   0 << DMA_CH_CTRL_DSTADDRCTRL_POS)
#define DMA_CH_CTRL_DSTADDR_DEC          (   1 << DMA_CH_CTRL_DSTADDRCTRL_POS)
#define DMA_CH_CTRL_DSTADDR_FIX          (   2 << DMA_CH_CTRL_DSTADDRCTRL_POS)
#define DMA_CH_CTRL_SRCREQ_POS           (        8)
#define DMA_CH_CTRL_SRCREQ_MASK          (0x0F << DMA_CH_CTRL_SRCREQ_POS)
#define DMA_CH_CTRL_SRCREQ(n)            (((n) << DMA_CH_CTRL_SRCREQ_POS) & DMA_CH_CTRL_SRCREQ_MASK)
#define DMA_CH_CTRL_DSTREQ_POS           (        4)
#define DMA_CH_CTRL_DSTREQ_MASK          (0x0F << DMA_CH_CTRL_DSTREQ_POS)
#define DMA_CH_CTRL_DSTREQ(n)            (((n) << DMA_CH_CTRL_DSTREQ_POS) & DMA_CH_CTRL_DSTREQ_MASK)
#define DMA_CH_CTRL_INTABT               (   0 << 3)
#define DMA_CH_CTRL_INTERR               (   0 << 2)
#define DMA_CH_CTRL_INTTC                (   0 << 1)
#define DMA_CH_CTRL_ENABLE               (   1 << 0)

/**
  \fn          void DMA_SignalEvent_t (uint32_t event)
  \brief       Signal DMA Events.
  \param[in]   event  DMA Event mask
  \return      none
*/
typedef void (*DMA_SignalEvent_t) (uint32_t event);

/**
  \fn          int32_t dma_initialize (void)
  \brief       Initialize DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_initialize (void);

/**
  \fn          int32_t dma_uninitialize (void)
  \brief       De-initialize DMA peripheral
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_uninitialize (void);

/**
  \fn          int32_t dma_channel_configure (uint8_t            ch,
                                              uint32_t           src_addr,
                                              uint32_t           dst_addr,
                                              uint32_t           size,
                                              uint32_t           control,
                                              uint32_t           config,
                                              DMA_SignalEvent_t  cb_event)
  \brief       Configure DMA channel for next transfer
  \param[in]   ch        Channel number (0..7)
  \param[in]   src_addr  Source address
  \param[in]   dest_addr Destination address
  \param[in]   size      Amount of data to transfer
  \param[in]   control   Channel control
  \param[in]   config    Channel configuration
  \param[in]   cb_event  Channel callback pointer
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_channel_configure (	uint8_t			ch,
					uint32_t		src_addr,
					uint32_t		dst_addr,
					uint32_t		size,
					uint32_t		control,
					DMA_SignalEvent_t	cb_event);

/**
  \fn          int32_t dma_channel_enable (uint8_t ch)
  \brief       Enable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_channel_enable (uint8_t ch);

/**
  \fn          int32_t dma_channel_disable (uint8_t ch)
  \brief       Disable DMA channel
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_channel_disable (uint8_t ch);

/**
  \fn          uint32_t dma_channel_get_status (uint8_t ch)
  \brief       Check if DMA channel is enabled or disabled
  \param[in]   ch Channel number (0..7)
  \returns     Channel status
   - \b  1: channel enabled
   - \b  0: channel disabled
*/
extern uint32_t dma_channel_get_status (uint8_t ch);

/**
  \fn          uint32_t dma_channel_get_count (uint8_t ch)
  \brief       Get number of transferred data
  \param[in]   ch Channel number (0..7)
  \returns     Number of transferred data
*/
extern uint32_t dma_channel_get_count (uint8_t ch);

/**
  \fn          uint32_t dma_channel_abort (uint8_t ch)
  \brief       Abort ch transfer
  \param[in]   ch Channel number (0..7)
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
extern int32_t dma_channel_abort (uint8_t ch);
#endif /* __DMA_AE250_H */
