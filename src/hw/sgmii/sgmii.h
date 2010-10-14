#ifndef _SGMII_H
#define _SGMII_H
/**
 *  @file sgmii.h
 *
 *  @brief
 *		Defines the sgmii api
 */


#include "types.h"
#include "device.h"


/**
 * @brief
 *   The function prototype. If sgmii is not present in the device then hwSgmiiConfig will
 *   have been defined to a void statement in the device configuration header file 
 */
#ifndef hwSgmiiConfig
int32 hwSgmiiConfig (int32 port, iblSgmii_t *iblSgmii);
#endif

 



#endif /* _SGMII_H */
