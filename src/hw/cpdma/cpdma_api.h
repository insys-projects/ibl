#ifndef _CPDMA_API_H
#define _CPDMA_API_H
/********************************************************************************************************
 * FILE PURPOSE: Provide the cpdma API
 ********************************************************************************************************
 * FILE NAME: cpdma_api.h
 *
 * DESCRIPTION: The public API is defined
 *
 ********************************************************************************************************/


typedef struct cpdmaRxCfg_s  {

    UINT32  rxBase;             /* Base address of rx registers */
    UINT32  nRxChans;           /* The number of rx channels */
    UINT32  flowBase;           /* Add address of flow registers */
    UINT32  nRxFlows;           /* Number of rx flows */
    UINT32  qmNumFreeBuf;       /* Queue manager for descriptors/buffers for received packets */
    UINT32  queueFreeBuf;       /* Queue that holds descriptors/buffers for received packets */
    UINT32  qmNumRx;            /* Queue manager for received packets */
    UINT32  queueRx;            /* Default Rx queue for received packets */
    UINT32  tdownPollCount;     /* Number of loop iterations to wait for teardown */
    
} cpdmaRxCfg_t; 


typedef struct cpdmaTxCfg_s  {

    UINT32 gblCtlBase;          /* Base address of global control registers */
    UINT32 txBase;              /* Base address of the tx registers */
    UINT32 nTxChans;            /* The number of tx channels */
    
} cpdmaTxCfg_t;
    

/* Prototypes */
SINT16 hwCpdmaRxDisable (const cpdmaRxCfg_t *cfg);
SINT16 hwCpdmaRxConfig (const cpdmaRxCfg_t *cfg);
SINT16 hwCpdmaTxConfig (const cpdmaTxCfg_t *cfg);
SINT16 hwCpdmaTxDisable (const cpdmaTxCfg_t *cfg);

#endif /* _CPDMA_API_H */
