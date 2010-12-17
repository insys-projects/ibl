#ifndef _SERDES_LOC_H
#define _SERDES_LOC_H
/*************************************************************************************************
 * FILE PURPOSE: Local definitions for SERDES
 *************************************************************************************************
 * FILE NAME: serdesloc.h
 *
 * DESCRIPTION: Local serdes definitions
 *
 *************************************************************************************************/
#include "target.h"

/* Offsets */
#define SERDES_REG_CFG      0
#define SERDES_REG_RX(x)    (4 + 8*(x))
#define SERDES_REG_TX(x)    (8 + 8*(x))


/* Cfg register */
#define SERDES_SET_CFG_SLEEP(x,v)  BOOT_SET_BITFIELD((x),(v),10,10)
#define SERDES_GET_ENABLE(x)       BOOT_READ_BITFIELD((x),0,0)
#define SERDES_SET_ENABLE(x,v)     BOOT_SET_BITFIELD((x),(v),0,0)
#define SERDES_SET_MULT(x,v)       BOOT_SET_BITFIELD((x),(v),7,1)
#define SERDES_SET_VRANGE(x,v)     BOOT_SET_BITFIELD((x),(v),9,9)


/* Rx Cfg register */
#define SERDES_RX_CFG_SET_ENABLE(x,v)   BOOT_SET_BITFIELD((x),(v),0,0)
#define SERDES_RX_CFG_SET_RATE(x,v)     BOOT_SET_BITFIELD((x),(v),5,4)

/* Tx Cfg register */
#define SERDES_TX_CFG_SET_ENABLE(x,v)   BOOT_SET_BITFIELD((x),(v),0,0)
#define SERDES_TX_CFG_SET_RATE(x,v)     BOOT_SET_BITFIELD((x),(v),5,4)


#endif /* _SERDES_LOC_H */

