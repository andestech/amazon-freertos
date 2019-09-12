#ifndef __RTE_DEVICE_H
#define __RTE_DEVICE_H


// USB0 Controller [Driver_USBD0 and Driver_USBH0]
// Configuration settings for Driver_USBD0 in component ::Drivers:USB Device
// Configuration settings for Driver_USBH0 in component ::Drivers:USB Host
#define RTE_USB_USB0                    0

// ENET (Ethernet Interface) [Driver_ETH_MAC0]
// Configuration settings for Driver_ETH_MAC0 in component ::Drivers:Ethernet MAC
#define RTE_ENET                        0

// SD/MMC Interface [Driver_MCI0]
// Configuration settings for Driver_MCI0 in component ::Drivers:MCI
#define RTE_SDMMC                       0

// I2C0 (Inter-integrated Circuit Interface) [Driver_I2C0]
// Configuration settings for Driver_I2C0 in component ::Drivers:I2C
#define RTE_I2C0                        1

// DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_I2C0_DMA_TX_EN              0
#define RTE_I2C0_DMA_TX_CH              2
#define RTE_I2C0_DMA_TX_REQID           8
//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_I2C0_DMA_RX_EN              0
#define RTE_I2C0_DMA_RX_CH              3
#define RTE_I2C0_DMA_RX_REQID           8

// UART0 (Universal asynchronous receiver transmitter) [Driver_USART0]
// Configuration settings for Driver_USART0 in component ::Drivers:USART
#define RTE_UART0                       0

// DMA
//  Tx
//    Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_UART0_DMA_TX_EN             0
#define RTE_UART0_DMA_TX_CH             0
#define RTE_UART0_DMA_TX_REQID          4
//  Rx
//    Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_UART0_DMA_RX_EN             0
#define RTE_UART0_DMA_RX_CH             1
#define RTE_UART0_DMA_RX_REQID          5

// UART1 (Universal asynchronous receiver transmitter) [Driver_USART1]
// Configuration settings for Driver_USART1 in component ::Drivers:USART
#define RTE_UART1                       1

// DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_UART1_DMA_TX_EN             1
#define RTE_UART1_DMA_TX_CH             0
#define RTE_UART1_DMA_TX_REQID          6
//   Rx
//     Channel    <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_UART1_DMA_RX_EN             1
#define RTE_UART1_DMA_RX_CH             1
#define RTE_UART1_DMA_RX_REQID          7

// SPI0 (Serial Peripheral Interface) [Driver_SPI0]
// Configuration settings for Driver_SPI0 in component ::Drivers:SPI
#define RTE_SPI0                        1

// DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_SPI0_DMA_TX_EN              0
#define RTE_SPI0_DMA_TX_CH              0
#define RTE_SPI0_DMA_TX_REQID           0

//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_SPI0_DMA_RX_EN              0
#define RTE_SPI0_DMA_RX_CH              1
#define RTE_SPI0_DMA_RX_REQID           1

// SPI1 (Serial Peripheral Interface) [Driver_SPI1]
// Configuration settings for Driver_SPI1 in component ::Drivers:SPI
#define RTE_SPI1                        1

// DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_SPI1_DMA_TX_EN              0
#define RTE_SPI1_DMA_TX_CH              0
#define RTE_SPI1_DMA_TX_REQID           2

//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_SPI1_DMA_RX_EN              0
#define RTE_SPI1_DMA_RX_CH              1
#define RTE_SPI1_DMA_RX_REQID           3

// SPI2 (Serial Peripheral Interface) [Driver_SPI2]
// Configuration settings for Driver_SPI2 in component ::Drivers:SPI
#define RTE_SPI2                        0

// DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_SPI2_DMA_TX_EN              0
#define RTE_SPI2_DMA_TX_CH              0
#define RTE_SPI2_DMA_TX_REQID           9

//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_SPI2_DMA_RX_EN              0
#define RTE_SPI2_DMA_RX_CH              1
#define RTE_SPI2_DMA_RX_REQID           10

// SPI3 (Serial Peripheral Interface) [Driver_SPI3]
// Configuration settings for Driver_SPI3 in component ::Drivers:SPI
#define RTE_SPI3                        1

// DMA
//   Tx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_SPI3_DMA_TX_EN              0
#define RTE_SPI3_DMA_TX_CH              0
#define RTE_SPI3_DMA_TX_REQID           11

//   Rx
//     Channel     <0=>0 <1=>1 <2=>2 <3=>3 <4=>4 <5=>5 <6=>6 <7=>7
#define RTE_SPI3_DMA_RX_EN              0
#define RTE_SPI3_DMA_RX_CH              1
#define RTE_SPI3_DMA_RX_REQID           12

// I2S0 (Integrated Interchip Sound 0) [Driver_SAI0]
// Configuration settings for Driver_SAI0 in component ::Drivers:SAI
#define RTE_I2S0                        0

// CAN0 Controller [Driver_CAN0]
// Configuration settings for Driver_CAN0 in component ::Drivers:CAN
#define RTE_CAN_CAN0                    0

// PWM0 Controller [Driver_PWM0]
// Configuration settings for Driver_PWM0 in component ::Drivers:PWM
#define RTE_PWM                         1

// RTC (Real-Time Clock)
#define RTE_RTC				0

// WDT0 Controller [Driver_WDT0]
// Configuration settings for Driver_WDT0 in component ::Drivers:WDT
#define RTE_WDT                         1

#endif  /* __RTE_DEVICE_H */
