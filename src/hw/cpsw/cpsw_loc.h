#ifndef _CPSW_LOC_H
#define _CPSW_LOC_H
/**********************************************************************************************
 * FILE PURPOSE: Local definitions for the Common Platform Ethernet Switch
 **********************************************************************************************
 * FILE NAME: cpsw_loc.h
 *
 * DESCRIPTION: Defines the peripheral registers used to configure the switch
 *
 **********************************************************************************************/

/* Register offsets */
#define CPSW_REG_CTL                0x004
#define CPSW_REG_STAT_PORT_EN       0x00c
#define CPSW_REG_MAXLEN             0x040
#define CPSW_REG_ALE_CONTROL        0x608
#define CPSW_REG_ALE_PORTCTL(x)     (0x640 + (x)*4)


/* Register values */
#define CPSW_REG_VAL_STAT_ENABLE_ALL             0xf
#define CPSW_REG_VAL_ALE_CTL_RESET_AND_ENABLE   ((UINT32)0xc0000000)
#define CPSW_REG_VAL_PORTCTL_FORWARD_MODE        0x3


#endif /* _CPSW_LOC_H */
