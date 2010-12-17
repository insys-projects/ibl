#ifndef _GMACSL_API_H
#define _GMACSL_API_H
/*******************************************************************************************
 * FILE PURPOSE: Mac sliver API
 *******************************************************************************************
 * FILE NAME: gmacsl_api.h
 *
 * DESCRIPTION: Defines the API for control of the CPGMAC sliver
 *
 *******************************************************************************************/
 
/*******************************************************************************************
 * Definition: The cpgmac sliver configuration structure
 *******************************************************************************************/
typedef struct hwGmacSlCfg_s  {

    UINT32 maxRxLen;    /* Maximum receive packet length. */
    
    UINT32 ctl;         /* Control bitfield */
    
} hwGmacSlCfg_t;


/*******************************************************************************************
 * Definition: Control bitfields used in the ctl field of hwGmacSlCfg_t
 *******************************************************************************************/
#define GMACSL_RX_ENABLE_RCV_CONTROL_FRAMES       (1 << 24)
#define GMACSL_RX_ENABLE_RCV_SHORT_FRAMES         (1 << 23)
#define GMACSL_RX_ENABLE_RCV_ERROR_FRAMES         (1 << 22)
#define GMACSL_RX_ENABLE_EXT_CTL                  (1 << 18)  /* duplex and gig read from input pins */
#define GMACSL_RX_ENABLE_GIG_FORCE                (1 << 17)
#define GMACSL_RX_ENABLE_IFCTL_B                  (1 << 16)
#define GMACSL_RX_ENABLE_IFCTL_A                  (1 << 15)
#define GMACSL_RX_ENABLE_CMD_IDLE                 (1 << 11)
#define GMACSL_TX_ENABLE_SHORT_GAP                (1 << 10)
#define GMACSL_ENABLE_GIG_MODE                    (1 <<  7)
#define GMACSL_TX_ENABLE_PACE                     (1 <<  6)
#define GMACSL_ENABLE                             (1 <<  5)
#define GMACSL_TX_ENABLE_FLOW_CTL                 (1 <<  4)
#define GMACSL_RX_ENABLE_FLOW_CTL                 (1 <<  3)
#define GMACSL_ENABLE_LOOPBACK                    (1 <<  1)
#define GMACSL_ENABLE_FULL_DUPLEX                 (1 <<  0)


/********************************************************************************************
 * DEFINTITION: function return values
 ********************************************************************************************/
#define GMACSL_RET_OK                        0
#define GMACSL_RET_INVALID_PORT             -1
#define GMACSL_RET_WARN_RESET_INCOMPLETE    -2
#define GMACSL_RET_WARN_MAXLEN_TOO_BIG      -3
#define GMACSL_RET_CONFIG_FAIL_RESET_ACTIVE -4


SINT16 hwGmacSlReset (UINT16 port);
SINT16 hwGmacSlConfig (UINT16 port, hwGmacSlCfg_t *cfg);





#endif /* _GMACSL_API_H */


