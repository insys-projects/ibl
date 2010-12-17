#ifndef _CPSW_API_H
#define _CPSW_API_H
/*************************************************************************************************
 * FILE PURPOSE: Common Platform ethernet SWitch driver API
 *************************************************************************************************
 * FILE NAME: cpsw_api.h
 *
 * DESCRIPTION: Defines the driver interface for the switch
 *
 *************************************************************************************************/
 
/* Control bitfields */
#define CPSW_CTL_P2_PASS_PRI_TAGGED     (1 << 5)
#define CPSW_CTL_P1_PASS_PRI_TAGGED     (1 << 4)
#define CPSW_CTL_P0_PASS_PRI_TAGGED     (1 << 3)
#define CPSW_CTL_P0_ENABLE              (1 << 2)
#define CPSW_CTL_VLAN_AWARE             (1 << 1)
#define CPSW_CTL_FIFO_LOOPBACK          (1 << 0)

/* API */
SINT16 hwCpswConfig (UINT32 ctl, UINT32 maxPktSize);
SINT16 hwCpswEnableP0 (void);
BOOL hwCpswIsEnabled (void);





#endif /* _CPSW_API_H */

