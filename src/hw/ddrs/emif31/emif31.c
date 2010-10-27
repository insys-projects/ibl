/************************************************************************************
 * FILE PURPOSE: The emif 3.1 driver
 ************************************************************************************
 * FILE NAME: emif31.c
 *
 * DESCRIPTION: The emif controller is setup
 *
 * @file emif31.c
 *
 * @brief
 *		The emif controller is setup
 *
 **************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "emif31api.h"
#include "emif31loc.h"
#include "target.h"

#define DEVICE_REG32_W(x,y)   *(volatile unsigned int *)(x)=(y)
#define DEVICE_REG32_R(x)    (*(volatile unsigned int *)(x))

void hwEmif3p1Enable (iblEmif3p1_t *cfg)
{
    uint32 v;

    v = cfg->sdcfg | (1 << 15) | (1 << 23);  /* Timing unlock (bit 15), boot unlock (bit 23) set */
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDCFG, v);

    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDRFC,  cfg->sdrfc);
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDTIM1, cfg->sdtim1);
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDTIM2, cfg->sdtim2);
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_DMCCTL, cfg->dmcctl);

    v = v & (~(1 << 23));  /* Clear boot lock */
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDCFG, v);


    v = v & (~(1 << 15));  /* Clear timing lock */
    DEVICE_REG32_W (DEVICE_DDR_BASE + DDR_REG_SDCFG, v);
    
}
    





