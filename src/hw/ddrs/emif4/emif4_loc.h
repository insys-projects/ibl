#ifndef _EMIF4_LOC_H
#define _EMIF4_LOC_H
/************************************************************************************************
 * FILE PURPOSE: EMIF4 peripheral definitions
 ************************************************************************************************
 * FILE NAME: emif4_loc.h
 *
 * DESCRIPTION: Local definitions for the emif4 driver
 *
 ************************************************************************************************/
#include "types.h"


#define EMIF4_INIT_SEQ_MASK     ibl_EMIF4_ENABLE_sdRamConfig       |  \
                                ibl_EMIF4_ENABLE_sdRamConfig2      |  \
                                ibl_EMIF4_ENABLE_sdRamRefreshCtl
                                
/* Register offsets */                                
#define EMIF_REG_SD_RAM_CFG                 0x008
#define EMIF_REG_SD_RAM_CFG2                0x00c
#define EMIF_REG_SDRAM_REF_CTL              0x010
#define EMIF_REG_TIMING1                    0x018
#define EMIF_REG_TIMING2                    0x020
#define EMIF_REG_TIMING3                    0x028
#define EMIF_REG_NVM_TIMING                 0x030
#define EMIF_REG_PWR_MNG_CTL                0x038
#define EMIF_REG_IODFT_TST_LOGIC            0x060
#define EMIF_REG_PERFORM_CNT_CFG            0x080
#define EMIF_REG_PERFORM_CNT_MST_REG_SEL    0x08c
#define EMIF_REG_IDLE_CTL                   0x098
#define EMIF_REG_INT_EN_SET                 0x0b4
#define EMIF_REG_OUT_IMP_CAL_CFG            0x0c8
#define EMIF_REG_TEMP_ALTER_CFG             0x0cc
#define EMIF_REG_PHY_CTL1                   0x0e4
#define EMIF_REG_PHY_CLT2                   0x0ec
#define EMIF_REG_PRI_CLASS_SVC_MAP          0x100
#define EMIF_REG_ID2CLS_SVC_1MAP            0x104
#define EMIF_REG_ID2CLS_SVC_2MAP            0x108
#define EMIF_REG_ECC_CTL                    0x110
#define EMIF_REG_ECC_RANGE1                 0x114
#define EMIF_REG_ECC_RANGE2                 0x118
#define EMIF_REG_RD_WRT_EXC_THRESH          0x120

#define EMIF_REG_VAL_SDRAM_REF_CTL_SET_INITREF_DIS(x,v)     (x) = BOOT_SET_BITFIELD((x),(v),31,31)
#define EMIF_REG_VAL_SDRAM_REF_CTL_SET_SELF_REFRESH(x,v)    (x) = BOOT_SET_BITFIELD((x),(v),28,28)


#endif /* _EMIF4_LOC_H */




