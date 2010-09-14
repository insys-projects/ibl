/**********************************************************************************
 * FILE PURPOSE: A very simple mdio driver
 **********************************************************************************
 * FILE NAME: mdio.c
 *
 * DESCRIPTION: Enables mdio and performs blind access
 *
 *  @file mdio.c
 *
 * @brief
 *	Enables mdio and performs blind access
 *
 **********************************************************************************/
#include "types.h"
#include "mdio.h"
#include "device.h"

/***********************************************************************************
 * FUNCTION PURPOSE: Provide an approximate delay
 ***********************************************************************************
 * DESCRIPTION: A delay in units of CPU cycles is executed
 ***********************************************************************************/
void mdio_delay (uint32 del)
{
    volatile unsigned int i;

    for (i = 0; i < del/8; i++);

}

/***********************************************************************************
 * FUNCTION PURPOSE: Perform blind MDIO access
 ***********************************************************************************
 * DESCRIPTION: Pre-configured mdio access is performed.
 ***********************************************************************************/
void hwMdio (int16 nAccess, uint32 *access, uint16 clkdiv, uint32 delayCpuCycles)
{
    int16 i;

    /* Enable MDIO, set the divider. A divider value of 0 is not allowed */
    if (clkdiv == 0)
        clkdiv = 1;

    MDIOR->CONTROL = (0x40000000 | clkdiv);


    for (i = 0; i < nAccess; i++)  {

        /* Set the Go bit */
        MDIOR->USERACCESS0 = (((uint32)1 << 31) | access[i]);
        while (MDIOR->USERACCESS0 & 0x80000000);

        mdio_delay (delayCpuCycles);

    }

    /* A big delay after the last configure */
    mdio_delay (1000000);

}






