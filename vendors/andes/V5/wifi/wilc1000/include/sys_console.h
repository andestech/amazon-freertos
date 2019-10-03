/*******************************************************************************
  Console System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_console.h

  Summary:
    Console System Service interface definitions.

  Description:
    This file contains the interface definition for the Console system
    service.  It provides a way to interact with the Console subsystem to
    manage the timing requests supported by the system.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
//DOM-IGNORE-END

#ifndef _SYS_CONSOLE_H
#define _SYS_CONSOLE_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "system_config.h"
#include "sys_module.h"


// DOM-IGNORE-BEGIN
#ifdef __cplusplus  

    extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: SYS CONSOLE Data Types
// *****************************************************************************
// *****************************************************************************

/* SYS Console Module Index Numbers

  Summary:
    Console System Service index definitions.

  Description:
    These constants provide Console System Service index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.

*/

//#ifdef SYS_CONSOLE_REMOVE_APIS
	   /* 
#ifndef SYS_CONSOLE_INCLUDE_APIS

#ifndef SYS_CONSOLE_MESSAGE
    #define SYS_CONSOLE_MESSAGE(message)
#endif

#ifndef SYS_CONSOLE_PRINT
    #define SYS_CONSOLE_PRINT(fmt, ...)
#endif
#endif
*/
#ifdef __cplusplus
}
#endif
// DOM-IGNORE-END

#endif //SYS_CONSOLE_H

/*******************************************************************************
 End of File
*/
