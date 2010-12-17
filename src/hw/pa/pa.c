/****************************************************************************************************
 * FILE PURPOSE: The boot loader packet accelerator driver
 ****************************************************************************************************
 * FILE NAME: pa.c
 *
 * DESCRIPTION: The driver for the packet accelerator during boot
 *
 ****************************************************************************************************/
#include "types.h"
#include "pa_loc.h"
#include "pa_api.h"
#include "device.h"
#include "hwpafw_bin.h"
#include <string.h>

/****************************************************************************************************
 * FUNCTION PURPOSE: Initialize the PA sub-system
 ****************************************************************************************************
 * DESCRIPTION: Only PDSP 0 is used. All other PDSPs are put into reset. PDSP0 is downloaded
 *              and started, then provided with the mac address configuration.
 ****************************************************************************************************/
SINT16 hwPaEnable (const paConfig_t *cfg)
{
    UINT32 i;
    UINT32 v;
    BOOL   done;

    /* Disable all PDSPs */
    for (i = 0; i < DEVICE_PA_NUM_PDSPS; i++)
        DEVICE_REG32_W (DEVICE_PA_BASE + PA_REG_PDSP_CTL(i), PA_REG_VAL_PDSP_CTL_DISABLE_PDSP);

    /* Clear the mailbox registers for PDSP 0 */
    for (i = 0; i < PA_NUM_MAILBOX_SLOTS; i++)
        DEVICE_REG32_W (DEVICE_PA_BASE + PA_REG_MAILBOX_SLOT(0, i), 0);


    /* Give a few cycles for the disable */
    chipDelay32 (100);

    /* download the firmware */
    memcpy ((UINT32 *)(DEVICE_PA_BASE + PA_MEM_PDSP_IRAM(0)), PDSPcode, sizeof(PDSPcode));

    /* Reset the PC and enable PDSP0 */
    DEVICE_REG32_W (DEVICE_PA_BASE + PA_REG_PDSP_CTL(0), PA_REG_VAL_PDSP_CTL_ENABLE_PDSP(0));


    /* Copy the two destination mac addresses to the mail box slots.
     * Mailbox 4 must be written last since this write triggers the firmware to
     * update the match information */
    cfg->cmdBuf[0] = BOOT_READ_BITFIELD(cfg->mac0ms, 31, 24);
    cfg->cmdBuf[1] = BOOT_READ_BITFIELD(cfg->mac0ms, 23, 16);
    cfg->cmdBuf[2] = BOOT_READ_BITFIELD(cfg->mac0ms, 15,  8);
    /* Cant use BOOT_READ_BITFIELD for 8 LSBs because it compiles with endian dependency */
    cfg->cmdBuf[3] = chipLower8 (cfg->mac0ms);
    cfg->cmdBuf[4] = BOOT_READ_BITFIELD(cfg->mac0ls, 31, 24);
    cfg->cmdBuf[5] = BOOT_READ_BITFIELD(cfg->mac0ls, 23, 16);
    cfg->cmdBuf[6] = cfg->cmdBuf[7] = 0;

    cfg->cmdBuf[8]  = BOOT_READ_BITFIELD(cfg->mac1ms, 31, 24);
    cfg->cmdBuf[9]  = BOOT_READ_BITFIELD(cfg->mac1ms, 23, 16);
    cfg->cmdBuf[10] = BOOT_READ_BITFIELD(cfg->mac1ms, 15,  8);
    /* Cant use BOOT_READ_BITFIELD for 8 LSBs because it compiles with endian dependency */
    cfg->cmdBuf[11] = chipLower8 (cfg->mac1ms);
    cfg->cmdBuf[12] = BOOT_READ_BITFIELD(cfg->mac1ls, 31, 24);
    cfg->cmdBuf[13] = BOOT_READ_BITFIELD(cfg->mac1ls, 23, 16);

    cfg->cmdBuf[14] = BOOT_READ_BITFIELD(cfg->rxQnum, 15, 8);
    cfg->cmdBuf[15] = chipLower8 (cfg->rxQnum);
    

    /* Give some delay then verify that the mailboxes have been cleared */
    for (i = 0, done = FALSE; ((i < DEVICE_PA_RUN_CHECK_COUNT) && (done == FALSE)); i++)  {
        chipDelay32 (100);
        v = DEVICE_REG32_R (DEVICE_PA_BASE + PA_REG_MAILBOX_SLOT(0, 3));
        if (v == 0)
            done = TRUE;
    }

    if (done == FALSE)
        return (-1);

    return (0);

} /* hwPaEnable */
    

/***********************************************************************************************
 * FUNCTION PURPOSE: Disable the PA firmware
 ***********************************************************************************************
 * DESCRIPTION: All PDSPs are disabled and all mailbox slots cleared
 ***********************************************************************************************/
SINT16 hwPaDisable (void)
{
    UINT32 i, j;

    /* Disable all pdsps, clear all mailboxes */
    for (i = 0; i < DEVICE_PA_NUM_PDSPS; i++)  {

        DEVICE_REG32_W (DEVICE_PA_BASE + PA_REG_PDSP_CTL(i), PA_REG_VAL_PDSP_CTL_DISABLE_PDSP);

        for (j = 0; j < PA_NUM_MAILBOX_SLOTS; j++)
            DEVICE_REG32_W (DEVICE_PA_BASE + PA_REG_MAILBOX_SLOT(i, j), 0);

    }

    return (0);

} /* hwPaDisable */




    











