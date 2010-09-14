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
#define PLL_REG_CTL       0x0100
#define PLL_REG_PLLM      0x0110
#define PLL_REG_PREDIV    0x0114
#define PLL_REG_POSTDIV   0x0128
#define PLL_REG_PLLSTAT   0x013c


/* Bit fields */
/* Reset type register */
#define PLL_REG_RSTYPE_FIELD_POWER_ON_RESET	(1<<0)

/* Ctl register */
#define PLL_REG_CTL_FIELD_PLLEN             (1<<0)
#define PLL_REG_CTL_FIELD_PLLRST            (1<<3)
#define PLL_REG_CTL_FIELD_PLLENSRC          (1<<5)


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
#define PLL_REG_STATUS_FIELD_LOCK           (1<<1)



#endif /* _PLLLOC_H */



