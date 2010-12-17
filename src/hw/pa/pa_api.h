#ifndef _PA_API_H
#define _PA_API_H
/*****************************************************************************************************
 * FILE PURPOSE: Define the Packet Accelerator API 
 *****************************************************************************************************
 * FILE NAME: pa_api.h
 *
 * DESCRIPTION: The boot loader driver API to the packet accelerator is defined
 *
 *****************************************************************************************************/
 
typedef struct paConfig_s  {

    UINT32  mac0ms;     /* 32 most significant bits of the mac address */
    UINT32  mac0ls;     /* 32 least significant bits of the mac address, in the 16msbs of this word */
    UINT32  mac1ms;     /* 32 most significant bits of the mac address */
    UINT32  mac1ls;     /* 32 least significant bits of the mac address, in the 16 msbs of this word */
    UINT32  rxQnum;     /* Receive packet queue number */
    UINT8   *cmdBuf;    /* Buffer used to create PA command */

} paConfig_t;

/* API */
SINT16 hwPaEnable (const paConfig_t *cfg);
SINT16 hwPaDisable (void);

#include "hwpafwsw.h"



#endif /* _PA_API_H */

