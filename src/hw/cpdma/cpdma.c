/********************************************************************************************
 * FILE PURPOSE: The cpdma driver
 ********************************************************************************************
 * FILE NAME: cpdma.c
 *
 * DESCRIPTION: The boot loader cpdma driver
 *
 ********************************************************************************************/
#include "types.h"
#include "cpdma_loc.h"
#include "cpdma_api.h"
#include "device.h"


/*********************************************************************************************
 * FUNCTION PURPOSE: Disable all rx channels and clear all the flow registers
 *********************************************************************************************
 * DESCRIPTION: The teardown is initiated and polled for completion. The function will
 *              return an error if the teardown is never complete, but will not stay
 *              in the function forever.
 *********************************************************************************************/
SINT16 hwCpdmaRxDisable (const cpdmaRxCfg_t *cfg)
{
    UINT32 i;
    UINT32 v;
    BOOL   done;

    for (i = 0; i < cfg->nRxChans; i++)  {

        /* If enabled, set the teardown bit */
        v = DEVICE_REG32_R (cfg->rxBase + CPDMA_REG_RCHAN_CFG_REG_A(i));
        if ( (v & CPDMA_REG_VAL_RCHAN_A_RX_ENABLE) == CPDMA_REG_VAL_RCHAN_A_RX_ENABLE )  {
            v = v | CPDMA_REG_VAL_RCHAN_A_RX_TDOWN;
            DEVICE_REG32_W (cfg->rxBase + CPDMA_REG_RCHAN_CFG_REG_A(i), v);
        }

    }

    /* Poll for completion */
    for (i = 0, done = FALSE; ( (i < cfg->tdownPollCount) && (done == FALSE) ); i++)  {

        chipDelay32 (100);

        done = TRUE;
        v = DEVICE_REG32_R (cfg->rxBase + CPDMA_REG_RCHAN_CFG_REG_A(i));
        if ( (v & CPDMA_REG_VAL_RCHAN_A_RX_ENABLE) == CPDMA_REG_VAL_RCHAN_A_RX_ENABLE )
            done = FALSE;

    }

    if (done == FALSE)
        return (-1);


    /* Clear all of the flow registers */
    for (i = 0; i < cfg->nRxFlows; i++)  {
        DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_A, i), 0);
        DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_B, i), 0);
        DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_C, i), 0);
        DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_D, i), 0);
        DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_E, i), 0);
        DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_F, i), 0);
        DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_G, i), 0);
        DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_H, i), 0);

    }

    return (0);

} /* hwCpdmaRxDisable */
        


/*********************************************************************************************
 * FUNCTION PURPOSE: Configure the cpdma receive direction for boot loader
 *********************************************************************************************
 * DESCRIPTION: The receive configuration for boot consists of a single flow configuration
 *              which is stored as flow configuration 0. All extended info and psinfo
 *              is stripped.
 *********************************************************************************************/
SINT16 hwCpdmaRxConfig (const cpdmaRxCfg_t *cfg)
{
    UINT32 v;
    UINT32 i;
    SINT16 ret = 0;


    /* Rx dma channels are disabled during flow configurations.
     * Even if disable fails attempt to configure the receive so the boot can work */
    if (hwCpdmaRxDisable (cfg) != 0)
        ret = -1;

    /* Configure the flow
     * The flow is configured to not pass extended info or psinfo, with descriptor
     * type host */

    v = CPDMA_REG_VAL_MAKE_RX_FLOW_A(  1,                       /* extended info passed */
                                       1,                       /* psinfo passed */
                                       0,                       /* Retry on failure to transmit */
                                       CPDMA_DESC_TYPE_HOST,    /* Host type descriptor */
                                       0,                       /* PS located in descriptor */
                                       0,                       /* SOP offset */
                                       cfg->qmNumRx,            /* Rx packet destination QM number */
                                       cfg->queueRx );          /* Rx packet destination queue */


    DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_A, 0), v);

    DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_B, 0), CPDMA_REG_VAL_RX_FLOW_B_DEFAULT);
    DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_C, 0), CPDMA_REG_VAL_RX_FLOW_C_DEFAULT);

    v = CPDMA_REG_VAL_MAKE_RX_FLOW_D (  cfg->qmNumFreeBuf,      /* Rx packet destination QM number, 1st descriptor */
                                        cfg->queueFreeBuf,      /* Rx packet destination queue, 1st descriptor */
                                        cfg->qmNumFreeBuf,      /* Rx packet destination QM number, subsequent descriptors */
                                        cfg->queueFreeBuf );    /* Rx packet destination queue, subsequent descriptors */
    DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_D, 0), v);
  
    
    /* Register E uses the same setup as D */
    DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_E, 0), v);



    DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_F, 0), CPDMA_REG_VAL_RX_FLOW_F_DEFAULT);
    DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_G, 0), CPDMA_REG_VAL_RX_FLOW_G_DEFAULT);
    DEVICE_REG32_W (cfg->flowBase + CPDMA_RX_FLOW_CFG(CPDMA_RX_FLOW_REG_H, 0), CPDMA_REG_VAL_RX_FLOW_H_DEFAULT);


    /* Enable the rx channels */
    for (i = 0; i < cfg->nRxChans; i++) 
        DEVICE_REG32_W (cfg->rxBase + CPDMA_REG_RCHAN_CFG_REG_A(i), CPDMA_REG_VAL_RCHAN_A_RX_ENABLE);


    return (ret);

} /* hwCpdmaRxConfig */


/*************************************************************************************************
 * FUNCTION PURPOSE: Configure the cpdma transmit direction for boot loader
 *************************************************************************************************
 * DESCRIPTION: The transmit channels are enabled
 *************************************************************************************************/
SINT16 hwCpdmaTxConfig (const cpdmaTxCfg_t *cfg)
{
    UINT32 i;

    /* Disable loopback in the tx direction */
    DEVICE_REG32_W (cfg->gblCtlBase + CPDMA_REG_EMU_CTL, CPDMA_REG_VAL_EMU_CTL_NO_LOOPBACK);

    /* Enable all channels. The current state isn't important */
    for (i = 0; i < cfg->nTxChans; i++)  {
        DEVICE_REG32_W (cfg->txBase + CPDMA_REG_TCHAN_CFG_REG_B(i), 0);  /* Priority */
        DEVICE_REG32_W (cfg->txBase + CPDMA_REG_TCHAN_CFG_REG_A(i), CPDMA_REG_VAL_TCHAN_A_TX_ENABLE);
    }

    return (0);

} /* hwCpdmaTxConfig */


/***************************************************************************************************
 * FUNCTION PURPOSE: Disable the transmit channels
 ***************************************************************************************************
 * DESCRIPTION: The transmit channels are disabled
 ***************************************************************************************************/
SINT16 hwCpdmaTxDisable (const cpdmaTxCfg_t *cfg)
{
    UINT32 i;
    UINT32 v;

    for (i = 0; i < cfg->nTxChans; i++)  {

        v = DEVICE_REG32_R (cfg->txBase + CPDMA_REG_TCHAN_CFG_REG_A(i));

        if ( (v & CPDMA_REG_VAL_TCHAN_A_TX_ENABLE) == CPDMA_REG_VAL_TCHAN_A_TX_ENABLE)  {
            v = v | CPDMA_REG_VAL_TCHAN_A_TX_TDOWN;
            DEVICE_REG32_W (cfg->txBase + CPDMA_REG_TCHAN_CFG_REG_A(i), v);
        }
    }


    return (0);

} /* hwCpdmaTxDisable */
                        





