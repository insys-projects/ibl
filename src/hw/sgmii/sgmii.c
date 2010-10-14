/**
 *  @file sgmii.c
 *
 *  @brief
 *		The sgmii driver for the ibl
 *
 */

#include "types.h"
#include "ibl.h"
#include "sgmii.h"
#include "target.h"

#define SGMII_ACCESS(port,reg)   *((volatile unsigned int *)(sgmiis[port] + reg))

/**
 *  @brief
 *      Configure the sgmii
 */
int32 hwSgmiiConfig (int32 port, iblSgmii_t *iblSgmii)
{
    unsigned int sgmiis[] = TARGET_SGMII_BASE_ADDRESSES;
    int32 v;

    if (port >= TARGET_EMAC_N_PORTS)
        return (-1);


    SGMII_ACCESS(port, TARGET_SGMII_SOFT_RESET) = 1;

    do  {

        v = SGMII_ACCESS(port, TARGET_SGMII_SOFT_RESET);

    } while (v & 1);

    /* Bit 1 holds tx/rx in reset. Used during loopback state change in 
     * the control register */
    SGMII_ACCESS(port, TARGET_SGMII_SOFT_RESET) = 2;


    SGMII_ACCESS(port, TARGET_SGMII_CONTROL) = iblSgmii->control;
    SGMII_ACCESS(port, TARGET_SGMII_SOFT_RESET) = 0;

    SGMII_ACCESS(port, TARGET_SGMII_MR_ADV_ABILITY) = iblSgmii->adviseAbility;

    /* Serdes configuration */
    SGMII_ACCESS(port, TARGET_SGMII_TX_CFG) = iblSgmii->txConfig;
    SGMII_ACCESS(port, TARGET_SGMII_RX_CFG) = iblSgmii->rxConfig;
    SGMII_ACCESS(port, TARGET_SGMII_AUX_CFG) = iblSgmii->auxConfig;

    return (0);

}

    





