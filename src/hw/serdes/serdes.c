/***********************************************************************************
 * FILE PURPOSE: Serdes configuration
 ***********************************************************************************
 * FILE NAME: serdes.c
 *
 * DESCRIPTION: Performs serdes configurations
 *
 ***********************************************************************************/
#include "types.h"
#include "serdes_api.h"
#include "serdesloc.h"
#include "device.h"


/************************************************************************************
 * FUNCTION PURPOSE: Enable and configure serdes
 ************************************************************************************
 * DESCRIPTION: The serdes is configured and enabled
 ************************************************************************************/
SINT16 hwSerdesConfig (UINT32 sBase, serdesConfig_t *scfg)
{
    UINT32 reg;
    UINT32 regb;
    SINT32 i;


    /* If the serdes is already enabled and the new value does not match
     * the current value, the serdes is first disabled. Dont compare 
     * the sleep value in the register, which can be toggled dynamically */
    reg  = DEVICE_REG32_R (sBase + SERDES_REG_CFG);
    reg  = SERDES_SET_CFG_SLEEP(reg, 0);
    regb = SERDES_SET_CFG_SLEEP(scfg->cfg, 0);

    chipKickOpenSerdes(sBase);

    if ( (SERDES_GET_ENABLE(reg) == 1)         && 
         (SERDES_GET_ENABLE(scfg->cfg) == 1)   &&
         (reg != regb)                           )   {

        reg = SERDES_SET_ENABLE(reg, 0);
        DEVICE_REG32_W (sBase + SERDES_REG_CFG, reg);
        chipDelay32 (100);
    }


    /* Config register. After enable it takes upt to 350ns, or 200 cycles to 
     * stabalize. Although these are serdes clock cycles the delay here is
     * in cpu cycles. The PLL status will be checked by the peripheral
     * using the PLL */


    DEVICE_REG32_W (sBase + SERDES_REG_CFG, regb);
    chipDelay32 (200);

    /* Some devices have unreliable lock status bits. Add an extra delay
     * to allow the serdes to lock */
    chipDelay32 (TARGET_SERDES_LOCK_DELAY);


    /* rx and tx config registers */
    for (i = 0; i < scfg->nLanes; i++)  {

        DEVICE_REG32_W (sBase + SERDES_REG_RX(i), scfg->rxCfg[i]);
        DEVICE_REG32_W (sBase + SERDES_REG_TX(i), scfg->txCfg[i]);

    }

    chipKickClosedSerdes(sBase);

    return (0);

} /* hwSerdesConfig */



/*******************************************************************************************
 * FUNCTION PURPOSE: Wait for a serdes lock on lane 0
 *******************************************************************************************
 * DESCRIPTION: Waits for a lock on lane 0. Doesn't trap if the lock is not found
 *******************************************************************************************/
SINT16 hwSerdesWaitLock (UINT32 statusBase)
{
    UINT32 reg;
    UINT32 i;

    for (i = 0; i < 100; i++)  {
        reg = DEVICE_REG32_R (statusBase);
        if (reg & 1)
            return (0);

        chipDelay32 (1000);
    }

    return (-1);

} /* hwSerdesWaitLock */




