#ifndef _PLLLOC_H
#define _PLLLOC_H
/**************************************************************************************
 * FILE PURPOSE: Local pll definitions
 **************************************************************************************
 * FILE NAME: pllloc.h
 *
 * DESCRIPTION: Local definitions for the pll driver
 **************************************************************************************/

/* Register offsets */
#define PLL_REG_RSTYPE	  0x00e4
#define PLL_REG_RSCTL     0x00e8
#define PLL_REG_RSCFG     0x00ec
#define PLL_REG_CTL       0x0100
#define PLL_REG_SECCTL    0x0108
#define PLL_REG_PLLM      0x0110
#define PLL_REG_PREDIV    0x0114
#define PLL_REG_POSTDIV   0x0128
#define PLL_REG_CMD       0x0138
#define PLL_REG_PLLSTAT   0x013c
#define PLL_REG_CKCTL     0x0154
#define PLL_REG_DIV2	  0x011C
#define PLL_REG_DIV5	  0x0164
#define PLL_REG_DIV8	  0x0170
#define PLL_REG_ALNCTL	  0x0140

/* PLL constants */

/* Bit fields */
/* Reset type register */
#define PLL_REG_RSTYPE_FIELD_POWER_ON_RESET	(1<<0)
#define PLL_REG_RSTYPE_FIELD_RESET_PI       (1<<1)
#define PLL_REG_RSTYPE_FIELD_PLL_CTRL       (1<<2)
#define PLL_REG_RSTYPE_FIELD_RESET_REQ      (0xff00)  /* Bits 8-15 */
#define PLL_REG_RSTYPE_FIELD_EMU_0          (1<<28)
#define PLL_REG_RSTYPE_FIELD_EMU_1          (1<<29)

#define PLL_REG_RSTYPE_EXTRACT_RESET_REQ(x)  (((x) & PLL_REG_RSTYPE_FIELD_RESET_REQ) >> 8)

/* Reset cfg register */
#define PLL_REG_RSCFG_FIELD_RESET_REQ_TYPE       (0x00ff)
#define PLL_REG_RSCFG_FIELD_RESET_PI_TYPE        (1<<12)
#define PLL_REG_RSCFG_FIELD_RESET_PLL_CTRL_TYPE  (1<<13)
#define PLL_REG_RSCFG_FIELD_POWER_ON_RESET       (1<<29)


/* Reset ctrl register */
#define PLL_REG_RSCTL_VALUE_SOFTRST_DISABLED (1<<16)
#define PLL_REG_RSCTL_VALUE_KEY              (0x5A69)
 
 
/* Ctl register */
#define PLL_REG_CTL_FIELD_PLLEN             (1<<0)
#define PLL_REG_CTL_FIELD_PLLRST            (1<<3)
#define PLL_REG_CTL_FIELD_PLLDIS            (1<<4)
#define PLL_REG_CTL_FIELD_PLLENSRC          (1<<5)

/* Secondary control register */
#define PLL_REG_SECCTL_FIELD_BYPASS         (1<<23)
#define PLL_REG_SECCTL_SET_POSTDIV(x,v)     BOOT_SET_BITFIELD((x),(v),16,16)
#define PLL_REG_SECCTL_ENABLE_POSTDIV(x)    BOOT_SET_BITFIELD((x),1,19,19)


/* Pll multiplier register */
#define PLL_REG_PLLM_FIELD_MULTm1           (0x3f<<0)

/* Prediv register */
#define PLL_REG_PREDIV_FIELD_RATIOm1        (0x1f<<0)
#define PLL_REG_PREDIV_FIELD_ENABLE         (1<<15)

/* Postdiv register */
#define PLL_REG_POSTDIV_FIELD_RATIO         (0x3f <<0)
#define PLL_REG_POSTDIV_FIELD_ENABLE        (1<<15)
#define PLL_REG_POSTDIV_VALUE_DISABLED      0

/* Status register */
#define PLL_REG_STATUS_FIELD_GOSTAT         (1<<0)
#define PLL_REG_STATUS_FIELD_LOCK           (1<<1)

/* CKCTL register */
#define PLL_REG_CKCTL_ALNBYP                (1<<0)

/* Cmd register */
#define PLL_REG_CMD_GOSET                   (1<<0)

#endif /* _PLLLOC_H */



