/*  This section lists the other files that are included in this file.

*/
#include "FreeRTOSConfig.h"
#include "sys_tmr.h"


// for the following functions no lock is performed
// because they need to be really fast and
// they are info only anyway

uint32_t SYS_TMR_TickCountGet ( void )
{
    volatile uint32_t * const pullMachineTimerCompareRegister = ( volatile uint32_t * const ) ( configCLINT_BASE_ADDRESS + 0x0000 );
    return *pullMachineTimerCompareRegister;
}

