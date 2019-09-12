#include <nds32_intrinsic.h>

#include "Driver_SPI.h"
#include "spi_ae250.h"

/* Flash Status Register bits. */
#define SR_BUSY         1       /* Erase/Write in progress */
#define SR_WEL          2       /* Write enable latch */
#define SR_BP0          4       /* Block protect 0 */
#define SR_BP1          8       /* Block protect 1 */
#define SR_BP2          0x10    /* Block protect 2 */
#define SR_BP3          0x20    /* Block protect 3 */
#define SR_QE           0x40    /* Quad Enable */
#define SR_SRWD         0x80    /* SR write protect */


void Flash_SpiInit(void);
void Flash_Write(uint32_t * Flash_Address, uint8_t * Data,  uint32_t Data_Size);
void Flash_Read(uint32_t * Flash_Address, uint8_t * Data,  uint32_t Data_Size);
void Flash_Enable(void);
void Flash_Erase( uint32_t * Flash_Address, int Data_Size);
char Flash_ReadStatus(void);
