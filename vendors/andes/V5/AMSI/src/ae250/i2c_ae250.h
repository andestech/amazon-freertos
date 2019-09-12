/*
 * Copyright (c) 2012-2017 Andes Technology Corporation
 * All rights reserved.
 *
 */

#ifndef __I2C_AE250_H
#define __I2C_AE250_H

#include "ae250.h"
#include "Driver_I2C.h"

#include "dma_ae250.h"
#include "platform.h"

#ifndef BIT
#define BIT(n)                      ((unsigned int) 1 << (n))
#define BITS2(m,n)                  (BIT(m) | BIT(n) )

/* bits range: for example BITS(16,23) = 0xFF0000
 *   ==>  (BIT(m)-1)   = 0x0000FFFF     ~(BIT(m)-1)   => 0xFFFF0000
 *   ==>  (BIT(n+1)-1) = 0x00FFFFFF
 */
#define BITS(m,n)                   (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /* BIT */

//selet APB clock
#define APB_CLK_30MHZ			0
#define APB_CLK_40MHZ			0
#define APB_CLK_60MHZ			1

// n-byte FIFO, where n is the number 2, 4, 8 or 16.
#define ATCIIC100_FIFO_DEPTH_N      (16)

// id rev register bit define(RO)
#define ATCIIC100_ID                BITS(12,31)     // ID number for ATCRTC100
#define ATCIIC100_MAJOR             BITS(4,11)      // Enable alarm wakeup signal
#define ATCIIC100_MINOR             BITS(0,3)       // Enable alarm interrupt

// Configuration Register(RO)
#define FIFO_SZ_MSK                 BITS(0,1)       // FIFO size mask
#define FIFO_SZ_2                   (0x0)           // 2 bytes
#define FIFO_SZ_4                   (0x1)           // 4 bytes
#define FIFO_SZ_8                   (0x2)           // 8 bytes
#define FIFO_SZ_16                  (0x3)           // 16 bytes
#define MAX_XFER_SZ                 (256)           // 256 bytes

// Interrupt Enable Register(RW)
#define IEN_ALL                     BITS(0,9)       // All Interrupts mask.
#define IEN_CMPL                    BIT(9)          // Completion Interrupt.
#define IEN_BYTE_RECV               BIT(8)          // Byte Receive Interrupt.
#define IEN_BYTE_TRANS              BIT(7)          // Byte Transmit Interrupt.
#define IEN_START                   BIT(6)          // START Condition Interrupt.
#define IEN_STOP                    BIT(5)          // STOP Condition Interrupt.
#define IEN_ARB_LOSE                BIT(4)          // Arbitration Lose Interrupt.
#define IEN_ADDR_HIT                BIT(3)          // Address Hit Interrupt.
#define IEN_FIFO_HALF               BIT(2)          // FIFO Half Interrupt.
#define IEN_FIFO_FULL               BIT(1)          // FIFO Full Interrupt.
#define IEN_FIFO_EMPTY              BIT(0)          // FIFO Empty Interrupt.

// Status Register(RW)
#define STATUS_W1C_ALL              BITS(3,9)       // All Interrupts status write 1 clear mask.
#define STATUS_LINE_SDA             BIT(14)         // current status of the SDA line on the bus.
#define STATUS_LINE_SCL             BIT(13)         // current status of the SCL line on the bus.
#define STATUS_GEN_CALL             BIT(12)         // the address of the current transaction is a general call address.
#define STATUS_BUS_BUSY             BIT(11)         // the bus is busy.
#define STATUS_ACK                  BIT(10)         // the type of the last received/transmitted acknowledgement bit.
#define STATUS_CMPL                 BIT(9)          // Transaction Completion
#define STATUS_BYTE_RECV            BIT(8)          // a byte of data has been received
#define STATUS_BYTE_TRANS           BIT(7)          // a byte of data has been transmitted.
#define STATUS_START                BIT(6)          // a START Condition or a repeated TART condition has been transmitted/received.
#define STATUS_STOP                 BIT(5)          // a STOP Condition has been transmitted/received.
#define STATUS_ARB_LOSE             BIT(4)          // the controller has lost the bus arbitration (master mode only).
#define STATUS_ADDR_HIT             BIT(3)          // Master: indicates that a slave has responded to the transaction, Slave: indicates that a transaction is targeting the controller (including the General Call).
#define STATUS_FIFO_HALF            BIT(2)          // Indicates that the FIFO is half-full or half-empty.
#define STATUS_FIFO_FULL            BIT(1)          // the FIFO is full.
#define STATUS_FIFO_EMPTY           BIT(0)          // the FIFO is empty.

// Address Register(RW)
#define SLAVE_ADDR_MSK              BITS(0,9)       // The slave address.

// Data Register(RW)
#define DATA_MSK                    BITS(0,7)       // Write this register to put one byte of data to the FIFO, Read this register to get one byte of data from the FIFO.

// Control Register(RW)
#define CTRL_PHASE_START            BIT(12)         // Enable this bit to send a START condition at the beginning of transaction, master mode only.
#define CTRL_PHASE_ADDR             BIT(11)         // Enable this bit to send the address after START condition, master mode only.
#define CTRL_PHASE_DATA             BIT(10)         // Enable this bit to send the data after Address phase, master mode only.
#define CTRL_PHASE_STOP             BIT(9)          // Enable this bit to send a STOP condition at the end of a transaction, master mode only.
#define CTRL_DIR                    BIT(8)          // Transaction direction
#define CTRL_DATA_COUNT             BITS(0,7)       // Data counts in bytes.

// Command Register(RW)
#define CMD_MSK                     BITS(0,2)       // action command mask
#define CMD_NO_ACT                  (0x0)           // no action
#define CMD_ISSUE_TRANSACTION       (0x1)           // issue a data transaction (Master only)
#define CMD_ACK                     (0x2)           // respond with an ACK to the received byte
#define CMD_NACK                    (0x3)           // respond with a NACK to the received byte
#define CMD_CLEAR_FIFO              (0x4)           // clear the FIFO
#define CMD_RESET_I2C               (0x5)           /* reset the I2C controller(abort current
                                                       ransaction, set the SDA and SCL line to the
                                                       open-drain mode, reset the Status Register and
                                                       the Interrupt Enable Register, and empty the
                                                       FIFO) */

// Setup Register(RW)
#define SETUP_T_SUDAT               BITS(24,28)     // T_SUDAT defines the data setup time before releasing the SCL.
#define SETUP_T_SP                  BITS(21,23)     // T_SP defines the pulse width of spikes that must be suppressed by the input filter.
#define SETUP_T_HDDAT               BITS(16,20)     // T_SUDAT defines the data hold time after SCL goes LOW.
#define SETUP_T_SCL_RATIO           BIT(13)         // The LOW period of the generated SCL clock is defined by the combination of T_SCLRatio and T_SCLHi values
#define SETUP_T_SCLHI               BITS(4,12)      // The HIGH period of generated SCL clock is defined by T_SCLHi.
#define SETUP_DMA_EN                BIT(3)          // Enable the direct memory access mode data transfer.
#define SETUP_MASTER                BIT(2)          // Configure this device as a master or a slave.
#define SETUP_ADDRESSING            BIT(1)          // I2C addressing mode: 10-bit or 7-bit addressing mode
#define SETUP_I2C_EN                BIT(0)          // Enable the ATCIIC100 I2C controller.

#if APB_CLK_30MHZ
// I2C mode: standard-mode, fast-mode, fast-mode-plus, all based on APB-clk(30MHZ)
#define SETUP_T_SUDAT_STD           (0x3)           // setup time = (4 + 1 + 3) * 33ns = 264ns, Min 250ns
#define SETUP_T_SP_STD              (0x1)           // spikes time = 1 * 33ns = 33ns, Max 50ns
#define SETUP_T_HDDAT_STD           (5)             // hold time = (4 + 1 + 5) * 33ns = 330ns, Min 300ns
#define SETUP_T_SCL_RATIO_STD       (0x0)           // ratio=1, (4 + 1 + 138) * 33ns >= 4000ns, for I2C SCL clock, SCLHI Min 4000ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_STD           (138)           // (4 + 1 + 138 * 1) * 33ns >= 4700ns, SCLLOW Min 4700ns

#define SETUP_T_SUDAT_FAST          (0x0)           // setup time = (4 + 1 + 0) * 33ns = 165ns, Min 100ns
#define SETUP_T_SP_FAST             (0x1)           // spikes time = 1 * 33ns =  33ns, Max 50ns
#define SETUP_T_HDDAT_FAST          (5)             // hold time = (4 + 1 + 5) * 33ns = 330ns, Min 300ns
#define SETUP_T_SCL_RATIO_FAST      (0x1)           // ratio=2, (4 + 1 + 18) * 33ns >= 600ns, for I2C SCL clock, SCLHI Min 600ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST          (18)            // (4 + 1 + 18 * 2) * 33ns >= 1300ns, SCLLOW Min 1300ns

#define SETUP_T_SUDAT_FAST_P        (0x0)           // setup time = (4 + 1 + 0) * 33ns = 165s, Min 50ns
#define SETUP_T_SP_FAST_P           (0x1)           // spikes time = 1 * 33ns = 33ns, Max 50ns
#define SETUP_T_HDDAT_FAST_P        (0x0)           // hold time = (4 + 1 + 0) * 33ns = 165ns, Min 0ns
#define SETUP_T_SCL_RATIO_FAST_P    (0x1)           // ratio=2, (4 + 1 + 6) * 33ns >= 260ns, for I2C SCL clock, SCLHI Min 260ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST_P        (6)             // (4 + 1 + 6 * 2) * 33ns >= 500ns, SCLLOW Min 500ns

#elif APB_CLK_40MHZ
// I2C mode: standard-mode, fast-mode, fast-mode-plus, all based on APB-clk(40MHZ)
#define SETUP_T_SUDAT_STD           (0x4)           // setup time = (4 + 2 + 4) * 25ns = 250ns, Min 250ns
#define SETUP_T_SP_STD              (0x2)           // spikes time = 2 * 25ns = 50ns, Max 50ns
#define SETUP_T_HDDAT_STD           (0x6)           // hold time = (4 + 2 + 6) * 25ns = 300ns, Min 300ns
#define SETUP_T_SCL_RATIO_STD       (0x0)           // ratio=1, (4 + 2 + 182) * 25ns >= 4000ns, for I2C SCL clock, SCLHI Min 4000ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_STD           (182)           // (4 + 2 + 182 * 1) * 25ns >= 4700ns, SCLLOW Min 4700ns

#define SETUP_T_SUDAT_FAST          (0x0)           // setup time = (4 + 2 + 0) * 25ns = 150ns, Min 100ns
#define SETUP_T_SP_FAST             (0x2)           // spikes time = 2 * 25ns = 50ns, Max 50ns
#define SETUP_T_HDDAT_FAST          (0x6)           // hold time = (4 + 2 + 6) * 25ns = 300ns, Min 300ns
#define SETUP_T_SCL_RATIO_FAST      (0x1)           // ratio=2, (4 + 2 + 23) * 25ns >= 600ns, for I2C SCL clock, SCLHI Min 600ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST          (23)            // (4 + 2 + 23 * 2) * 25ns >= 1300ns, SCLLOW Min 1300ns

#define SETUP_T_SUDAT_FAST_P        (0x0)           // setup time = (4 + 2 + 0) * 25ns = 150ns, Min 50ns
#define SETUP_T_SP_FAST_P           (0x2)           // spikes time = 2 * 25ns = 50ns, Max 50ns
#define SETUP_T_HDDAT_FAST_P        (0x0)           // hold time = (4 + 2 + 0) * 25ns = 150ns, Min 0ns
#define SETUP_T_SCL_RATIO_FAST_P    (0x1)           // ratio=2, (4 + 2 + 7) * 25ns >= 260ns, for I2C SCL clock, SCLHI Min 260ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST_P        (7)             // (4 + 2 + 7 * 2) * 25ns >= 500ns, SCLLOW Min 500ns

#elif APB_CLK_60MHZ
// I2C mode: standard-mode, fast-mode, fast-mode-plus, all based on APB-clk(60MHZ)
#define SETUP_T_SUDAT_STD           (0x9)           // setup time = (4 + 3 + 9) * 16ns = 256ns, Min 250ns
#define SETUP_T_SP_STD              (0x3)           // spikes time = 3 * 16ns = 48ns, Max 50ns
#define SETUP_T_HDDAT_STD           (12)            // hold time = (4 + 3 + 12) * 16ns = 304ns, Min 300ns
#define SETUP_T_SCL_RATIO_STD       (0x0)           // ratio=1, (4 + 3 + 287) * 16ns >= 4000ns, for I2C SCL clock, SCLHI Min 4000ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_STD           (287)           // (4 + 3 + 287 * 1) * 16ns >= 4700ns, SCLLOW Min 4700ns

#define SETUP_T_SUDAT_FAST          (0x0)           // setup time = (4 + 3 + 0) * 16ns = 112ns, Min 100ns
#define SETUP_T_SP_FAST             (0x3)           // spikes time = 3 * 16ns =  48ns, Max 50ns
#define SETUP_T_HDDAT_FAST          (12)           // hold time = (4 + 3 + 12) * 16ns = 304ns, Min 300ns
#define SETUP_T_SCL_RATIO_FAST      (0x1)           // ratio=2, (4 + 3 + 38) * 16ns >= 600ns, for I2C SCL clock, SCLHI Min 600ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST          (38)            // (4 + 3 + 38 * 2) * 16ns >= 1300ns, SCLLOW Min 1300ns

#define SETUP_T_SUDAT_FAST_P        (0x0)           // setup time = (4 + 3 + 0) * 16ns = 112ns, Min 50ns
#define SETUP_T_SP_FAST_P           (0x3)           // spikes time = 3 * 16ns = 48ns, Max 50ns
#define SETUP_T_HDDAT_FAST_P        (0x0)           // hold time = (4 + 3 + 0) * 16ns = 112ns, Min 0ns
#define SETUP_T_SCL_RATIO_FAST_P    (0x1)           // ratio=2, (4 + 3 + 13) * 16ns >= 260ns, for I2C SCL clock, SCLHI Min 260ns
// The T_SCLHi value must be greater than T_SP and T_HDDAT values.
#define SETUP_T_SCLHI_FAST_P        (13)             // (4 + 3 + 13 * 2) * 16ns >= 500ns, SCLLOW Min 500ns
#endif

#define PARA_IGNORE                 (0)

typedef enum _I2C_CTRL_REG_ITEM_DIR
{
	I2C_MASTER_TX = 0x0,
	I2C_MASTER_RX = 0x1,
	I2C_SLAVE_TX = 0x1,
	I2C_SLAVE_RX = 0x0,
}I2C_CTRL_REG_ITEM_DIR;

// I2C driver running state
typedef enum _I2C_DRIVER_STATE
{
	I2C_DRV_NONE = 0x0,
	I2C_DRV_INIT = BIT(0),
	I2C_DRV_POWER = BIT(1),
	I2C_DRV_CFG_PARAM = BIT(2),
	I2C_DRV_MASTER_TX = BIT(3),
	I2C_DRV_MASTER_RX = BIT(4),
	I2C_DRV_SLAVE_TX = BIT(5),
	I2C_DRV_SLAVE_RX = BIT(6),
	I2C_DRV_MASTER_TX_CMPL = BIT(7),
	I2C_DRV_MASTER_RX_CMPL = BIT(8),
	I2C_DRV_SLAVE_TX_CMPL = BIT(9),
	I2C_DRV_SLAVE_RX_CMPL = BIT(10),
}I2C_DRIVER_STATE;

#define MEMSET(s, c, n)         __builtin_memset ((s), (c), (n))
#define MEMCPY(des, src, n)     __builtin_memcpy((des), (src), (n))

typedef struct _I2C_INFO
{
	NDS_I2C_SignalEvent_t           Sig_Evt_CB;
	NDS_I2C_CAPABILITIES            I2C_Cap;
	volatile I2C_DRIVER_STATE       Driver_State;
	NDS_POWER_STATE                 Pwr_State;
	uint8_t*                        middleware_rx_buf;
	uint8_t*                        middleware_tx_buf;
	/* flags for flow control */
	// salve mode only
	uint32_t                        last_rx_data_count;
	// Xfer_Data_Rd_Buf[] for middleware read out, salve mode only
	uint32_t                        middleware_rx_ptr;
	// salve mode only
	uint8_t                         nack_assert;
	uint32_t                        FIFO_Depth;
	uint32_t                        Slave_Addr;
	uint32_t                        Xfer_Wt_Num;
	uint32_t                        Xfer_Rd_Num;
	// write pointer
	uint32_t                        Xfered_Data_Wt_Ptr;
	// read pointer
	uint32_t                        Xfered_Data_Rd_Ptr;
	// read pointer overwrite
	uint32_t                        Xfered_Data_Rd_Ptr_Ow;
	uint8_t                         Xfer_Data_Rd_Buf[MAX_XFER_SZ];
	uint32_t                        Slave_Rx_Cmpl_Ctrl_Reg_Val;
	volatile NDS_I2C_STATUS         Status;
	uint32_t                        Xfer_Cmpl_Count;
}I2C_INFO;

// I2C DMA
typedef const struct _I2C_DMA
{
	uint8_t                 channel;          // DMA Channel
	uint8_t                 reqsel;           // DMA request selection
	DMA_SignalEvent_t       cb_event;         // DMA Event callback
}I2C_DMA;

// I2C Resource Configuration
typedef struct
{
	I2C_RegDef*		reg;                // I2C register interface
	I2C_DMA*		dma_tx;
	I2C_DMA*		dma_rx;
	I2C_INFO*		info;               // Run-Time control information
}const I2C_RESOURCES;

extern I2C_INFO I2C_Info;
extern int32_t i2cx_control(uint32_t control, uint32_t arg0, uint32_t arg1, I2C_RESOURCES* i2c);
extern void i2cx_signalevent(uint32_t event, I2C_RESOURCES* i2c);
extern void i2cx_master_fifo_write(I2C_RESOURCES* i2c, uint8_t is_flash_addr);
extern void i2cx_slave_fifo_write(I2C_RESOURCES* i2c);
extern void i2cx_master_fifo_read(I2C_RESOURCES* i2c);
extern void i2cx_slave_fifo_read(I2C_RESOURCES* i2c, uint8_t is_fifo_full);
#endif /* __I2C_AE250_H */
