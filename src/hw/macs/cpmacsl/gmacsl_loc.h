#ifndef _GMACSL_LOC_H
#define _GMACSL_LOC_H
/*****************************************************************************************
 * FILE PURPOSE: Local definitions for the cpgmac sliver
 *****************************************************************************************
 * FILE NAME: gmacsl_loc.h
 *
 * DESCRIPTION: Defines the peripheral registers used to configure the cpgmac sliver
 *
 ******************************************************************************************/

/* Register offsets */
#define CPGMACSL_REG_ID         0x00
#define CPGMACSL_REG_CTL        0x04
#define CPGMACSL_REG_STATUS     0x08
#define CPGMACSL_REG_RESET      0x0c
#define CPGMACSL_REG_MAXLEN     0x10
#define CPGMACSL_REG_BOFF       0x14
#define CPGMACSL_REG_RX_PAUSE   0x18
#define CPGMACSL_REG_TX_PAURSE  0x1c
#define CPGMACSL_REG_EM_CTL     0x20
#define CPGMACSL_REG_PRI        0x24


/* Soft reset register values */
#define CPGMAC_REG_RESET_VAL_RESET_MASK      (1 << 0)
#define CPGMAC_REG_RESET_VAL_RESET           (1 << 0)

/* Maxlen register values */
#define CPGMAC_REG_MAXLEN_LEN                0x3fff




#endif /* _GMACSL_LOC_H */

