/*******************************************************************************
  Timer System Service Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    sys_tmr.h

  Summary:
    Timer System Service interface definition.

  Description:
    This file contains the interface definition for the Timer System
    Service.  It provides a way to interact with the Timer subsystem to
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

#ifndef _SYS_TMR_H
#define _SYS_TMR_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// Section: SYS TMR Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* SYS Timer Module Index Numbers

  Summary:
    Timer System Service index definitions.

  Description:
    These constants provide Timer System Service index definitions.

  Remarks:
    These constants should be used in place of hard-coded numeric literals.

*/

#define SYS_TMR_INDEX_0                 0


// *****************************************************************************
/* SYS Timer invalid handle macro definition

  Summary:
    Identifies the invalid handle of the system timer.

  Description:
    This enumeration identifies the invalid handle of the system timer.

  Remarks:
    None.
*/

#define SYS_TMR_HANDLE_INVALID          ( ( uintptr_t ) -1 )


// *****************************************************************************
/* SYS TMR Handle

  Summary
    Identifies a client timer object.

  Description
    This handle identifies a registered instance of a client timer object.
    Access to a timer object parameters and functionality has to use
    a valid client timer handle.

  Remarks:
    None.
*/

uint32_t SYS_TMR_TickCountGet( void );

// *****************************************************************************
/* Function:
    uint64_t SYS_TMR_TickCountGetLong( void )

  Summary:
    Provides the current counter value with 64-bit resolution.

  Description:
    This function provides the current tick counter value as a 64-bit value.

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system timer tick count value.

  Example:
    <code>
    uint64_t lCount;
    lCount = SYS_TMR_TickCountGetLong ();
    </code>

  Remarks:
    None.
*/

uint64_t SYS_TMR_TickCountGetLong( void );

// *****************************************************************************
/* Function:
    uint32_t SYS_TMR_TickCounterFrequencyGet ( void )

  Summary:
    Provides the number of ticks per second

  Description:
    This function provides the system tick resolution,
    the number of ticks per second

  Precondition:
    The SYS_TMR_Initialize function should have been called before calling this
    function.

  Parameters:
    None.

  Returns:
    The current system timer ticks per second.

  Example:
    <code>
    uint32_t tickFrequencyHz;
 
    tickFrequencyHz = SYS_TMR_TickCounterFrequencyGet ( );
    </code>

  Remarks:
    None.
*/

#endif //_SYS_TMR_H

/*******************************************************************************
 End of File
*/
