#ifndef _SERDES_API_H
#define _SERDES_API_H
/**************************************************************************************************
 * FILE PURPOSE: SERDES API
 **************************************************************************************************
 * FILE NAME: serdes_api.h
 *
 * DESCRIPTION: Defines the serdes configuration functions
 **************************************************************************************************/

#define SERDES_MAX_LANES    4

typedef struct serdesConfig_s  {

    UINT32 cfg;
    UINT32 nLanes;
    UINT32 rxCfg[SERDES_MAX_LANES];
    UINT32 txCfg[SERDES_MAX_LANES];

} serdesConfig_t;


SINT16 hwSerdesConfig (UINT32 sBase, serdesConfig_t *scfg);
UINT32 hwSerdesInsertMult (UINT32 reg, UINT32 multX4, BOOL *matchFound);
UINT32 hwSerdesFormCfg (UINT32 vin, UINT32 refClkMhzx100, UINT32 linkRateMhz, UINT32 *dataRate, UINT32 dataRateIdx);
UINT32 hwSerdesFormRxCfg (UINT32 v, UINT32 rate, UINT32 enable);
UINT32 hwSerdesFormTxCfg (UINT32 v, UINT32 rate, UINT32 enable);
BOOL hwSerdesIsEnabled (UINT32 sBase);
SINT16 hwSerdesWaitLock (UINT32 statusBase);


#endif /* _SERDES_API_H */
