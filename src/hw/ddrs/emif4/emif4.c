/*************************************************************************************************
 * FILE PURPOSE: EMIF4 driver
 *************************************************************************************************
 * FILE NAME: emif4.c
 *
 * DESCRIPTION: The boot emif4 driver
 *
 *************************************************************************************************/
#include "types.h"
#include "ibl.h"
#include "emif4_api.h"
#include "emif4_loc.h"
#include "device.h"

/*************************************************************************************************
 * FUNCTION PUROPSE: Initial EMIF4 setup
 *************************************************************************************************
 * DESCRIPTION: Emif configuration
 *************************************************************************************************/
SINT16 hwEmif4p0Enable (iblEmif4p0_t *cfg)
{
    UINT32 v;

    /* If the config registers or refresh control registers are being written
     * disable the initialization sequence until they are all setup */
    if ((cfg->registerMask & EMIF4_INIT_SEQ_MASK) != 0)  {

        v = DEVICE_REG32_R (DEVICE_EMIF4_BASE + EMIF_REG_SDRAM_REF_CTL);
        EMIF_REG_VAL_SDRAM_REF_CTL_SET_INITREF_DIS(v,1);
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_SDRAM_REF_CTL, v);
    }

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamTiming1) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_TIMING1, cfg->sdRamTiming1);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamTiming2) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_TIMING2, cfg->sdRamTiming2);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamTiming3) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_TIMING3, cfg->sdRamTiming3);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_lpDdrNvmTiming) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_NVM_TIMING, cfg->lpDdrNvmTiming);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_powerManageCtl) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PWR_MNG_CTL, cfg->powerManageCtl);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_iODFTTestLogic) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_IODFT_TST_LOGIC, cfg->iODFTTestLogic);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_performCountCfg) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PERFORM_CNT_CFG, cfg->performCountCfg);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_performCountMstRegSel) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PERFORM_CNT_MST_REG_SEL, cfg->performCountMstRegSel);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_readIdleCtl) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_IDLE_CTL, cfg->readIdleCtl);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_sysVbusmIntEnSet) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_INT_EN_SET, cfg->sysVbusmIntEnSet);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamOutImpdedCalCfg) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_OUT_IMP_CAL_CFG, cfg->sdRamOutImpdedCalCfg);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_tempAlterCfg) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_TEMP_ALTER_CFG, cfg->tempAlterCfg);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_ddrPhyCtl1) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PHY_CTL1, cfg->ddrPhyCtl1);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_ddrPhyCtl2) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PHY_CLT2, cfg->ddrPhyCtl2);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_priClassSvceMap) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_PRI_CLASS_SVC_MAP, cfg->priClassSvceMap);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_mstId2ClsSvce1Map) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_ID2CLS_SVC_1MAP, cfg->mstId2ClsSvce1Map);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_mstId2ClsSvce2Map) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_ID2CLS_SVC_2MAP, cfg->mstId2ClsSvce2Map);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_eccCtl) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_ECC_CTL, cfg->eccCtl);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_eccRange1) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_ECC_RANGE1, cfg->eccRange1);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_eccRange2) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_ECC_RANGE2, cfg->eccRange2);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_rdWrtExcThresh) != 0)
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_RD_WRT_EXC_THRESH, cfg->rdWrtExcThresh);

    /* Allow the configuration to occur */
    v = DEVICE_REG32_R (DEVICE_EMIF4_BASE + EMIF_REG_SDRAM_REF_CTL);
    EMIF_REG_VAL_SDRAM_REF_CTL_SET_INITREF_DIS(v,0);
    DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_SDRAM_REF_CTL, v);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamConfig) != 0) 
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_SD_RAM_CFG, cfg->sdRamConfig);

    if ((cfg->registerMask & ibl_EMIF4_ENABLE_sdRamConfig2) != 0) 
        DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_SD_RAM_CFG2, cfg->sdRamConfig2);

    v = cfg->sdRamRefreshCtl;
    EMIF_REG_VAL_SDRAM_REF_CTL_SET_INITREF_DIS(v,0);
    DEVICE_REG32_W (DEVICE_EMIF4_BASE + EMIF_REG_SDRAM_REF_CTL, v);

    return (0);

} /* hwEmif4p0Enable */

    
