/***********************************************************************************************
 * FILE PURPOSE: Configure the cpsw
 ***********************************************************************************************
 * FILE NAME: cpsw.c
 *
 * DESCRIPTION: The common platform ethernet switch driver
 *
 ************************************************************************************************/
#include "types.h"
#include "cpsw_loc.h"
#include "cpsw_api.h"
#include "device.h"



/************************************************************************************************
 * FUNCTION PURPOSE: Configure the switch
 ************************************************************************************************
 * DESCRIPTION: The switch is setup without respect to its current state. The ALE 
 *              is reset during the configuration.
 ************************************************************************************************/
SINT16 hwCpswConfig (UINT32 ctl, UINT32 maxPktSize)
{
    UINT32 i;

    /* Max length register */
    DEVICE_REG32_W (DEVICE_CPSW_BASE + CPSW_REG_MAXLEN, maxPktSize);

    /* Control register */
    DEVICE_REG32_W (DEVICE_CPSW_BASE + CPSW_REG_CTL, ctl);

    /* All statistics enabled by default */
    DEVICE_REG32_W (DEVICE_CPSW_BASE + CPSW_REG_STAT_PORT_EN, CPSW_REG_VAL_STAT_ENABLE_ALL);

    /* Reset and enable the ALE */
    DEVICE_REG32_W (DEVICE_CPSW_BASE + CPSW_REG_ALE_CONTROL, CPSW_REG_VAL_ALE_CTL_RESET_AND_ENABLE);

    /* All ports put into forward mode */
    for (i = 0; i < DEVICE_CPSW_NUM_PORTS; i++)
        DEVICE_REG32_W (DEVICE_CPSW_BASE + CPSW_REG_ALE_PORTCTL(i), CPSW_REG_VAL_PORTCTL_FORWARD_MODE);


    return (0);

} /* hwCpswConfig */





