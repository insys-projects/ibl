/******************************************************************************************
 * FILE PURPOSE: Boot queue manager driver
 ******************************************************************************************
 * FILE NAME: qm.c
 *
 * DESCRIPTION: The boot loader queue manager driver. This driver uses a very simple
 *				setup on the queue manager, with a single link ram and a single
 *				memory region. The descriptors are configured as host descriptors
 *				and have the minimum 32 byte size.
 *
 *              For the boot driver queue register C is not written. It is important
 *              to the boot loader if packets are pushed/popped from the head or
 *              tail of a queue.
 *
 ******************************************************************************************/
#include "types.h"
#include "qm_loc.h"
#include "qm_api.h"
#include "device.h"
#include <string.h>

/*******************************************************************************************
 * FUNCTION PURPOSE: Pop a descriptor off of a queue
 *******************************************************************************************
 * DESCRIPTION: The descriptor is read from queue register D.
 *******************************************************************************************/
qmHostDesc_t *hwQmQueuePop (UINT32 qnum)
{
    qmHostDesc_t *hd;
    UINT32        uhd;

    /* Strip the descriptor size info */
    uhd = DEVICE_REG32_R (DEVICE_QM_MANAGER_QUEUES_BASE + QM_REG_QUEUE_REGD(qnum));
    uhd = uhd & ~0xf;
    hd  = (qmHostDesc_t *)uhd;

    return (hd);
} /* hwQmQueuePop */

/*******************************************************************************************
 * FUNCTION PURPOSE: Return the number of descriptors on a queue
 *******************************************************************************************
 * DESCRIPTION: The queue count is returned
 *******************************************************************************************/
UINT32  hwQmQueueCount (UINT32 qnum)
{
    UINT32 rega;

    rega = DEVICE_REG32_R (DEVICE_QM_QUEUE_STATUS_BASE + QM_REG_QUEUE_REGA(qnum));
    rega = BOOT_READ_BITFIELD (rega, QM_QA_ENTRY_COUNT_MSB, QM_QA_ENTRY_COUNT_LSB);
    return (rega);

} /* hwQmQueueCount */

/*******************************************************************************************
 * FUNCTION PURPOSE: Set a queue threshold
 *******************************************************************************************
 * DESCRIPTION: The queue threshold is enabled. Only high threshold is set, with a threshold
 * 				value of 1.
 *******************************************************************************************/
SINT16 hwQmInitThreshold (UINT32 qnum)
{
	DEVICE_REG32_W (DEVICE_QM_QUEUE_STATUS_BASE + QM_REG_STAT_CFG_REGD(qnum), 0x81);
	
	return (0);
	
} /* hwQmInitThreshold */	


/*******************************************************************************************
 * FUNCTION PURPOSE: Push a descriptor onto a queue
 *******************************************************************************************
 * DESCRIPTION: The descriptor is written to queue register D. No check is made to see
 *              if the queue number is valid.
 *******************************************************************************************/
void hwQmQueuePush (qmHostDesc_t *hd, UINT32 qnum, UINT32 descrSize)
{
	UINT32 regd;
	
	regd = ((UINT32) hd | ((descrSize >> 4) - 1));
	
    /* Push the descriptor onto the queue */
    DEVICE_REG32_W (DEVICE_QM_MANAGER_QUEUES_BASE + QM_REG_QUEUE_REGD(qnum), regd);

} /* hwQmQueuePush */

/*******************************************************************************************
 * FILE PURPOSE: Setup the queue manager
 *******************************************************************************************
 * DESCRIPTION: The queue manager is setup using a single linking ram and memory region,
 *              with fixed descriptor initialization.
 *
 *              Since the linking ram and descriptor regions are configured, it is assumed
 *              that all queues are currently empty.
 *
 ********************************************************************************************/
SINT16 hwQmSetup (qmConfig_t *cfg)
{
    UINT32 v;
    UINT32 w;
    UINT32 x;
    UINT32 i;
    qmHostDesc_t *hd;

    /* Verify that alignment requirements */
    if ( (cfg->linkRamBase & (QM_LINKRAM_ALIGN-1))  !=  0 )
        return (QM_INVALID_LINKRAM_ALIGNMENT);

    if ( (cfg->memRegionBase & (QM_MEMR_ALIGN-1))   !=  0 )
        return (QM_INVALID_MEMREGION_ALIGNMENT);

    /* Verify linkram sizing is in range */
    if ( (cfg->linkRamSize & ~QM_REG_LINKRAM_SIZE_MAX_MASK)  != 0 )
        return (QM_INVALID_LINKRAM_SIZE);

    /* Verify there is enough linkram to cover the single memory region */ 
    if (cfg->linkRamSize < cfg->memRegNumDescriptors)
        return (QM_INVALID_LINKRAM_RAM_SIZE);


    /* Linking RAM info */
    DEVICE_REG32_W (DEVICE_QM_MANAGER_BASE + QM_REG_LINKRAM_BASE(0), cfg->linkRamBase);
    DEVICE_REG32_W (DEVICE_QM_MANAGER_BASE + QM_REG_LINKRAM_SIZE(0), cfg->linkRamSize);
    DEVICE_REG32_W (DEVICE_QM_MANAGER_BASE + QM_REG_LINKRAM_BASE(1), 0);

    /* Memory region 0 info */
    DEVICE_REG32_W (DEVICE_QM_DESC_SETUP_BASE + QM_REG_MEMR_BASE_ADDR(0), cfg->memRegionBase);
    DEVICE_REG32_W (DEVICE_QM_DESC_SETUP_BASE + QM_REG_MEMR_START_IDX(0), 0);

    /* Calculate the 2 fields in the descriptor setup register. Bits 0-3 specifiy
     * the total memory size rounded up to the next higher power of two, and
     * is expresses as 2^(n - 5). So for example if you have 20 descriptors
     * The next higher power of 2 that exceeds this is 32, which is 2^5, so the value 0 (5-5)
     * is placed in this field */
    v = (31 - chipLmbd (1, cfg->memRegNumDescriptors));
    if (v >= 4)
      v = v - 4;
    else
      v = 0;

    /* Add the descriptor size field */
    QM_REG_VAL_DESC_SETUP_SET_DESC_SIZE(v, QM_DESC_SIZE_BYTES);

    DEVICE_REG32_W (DEVICE_QM_DESC_SETUP_BASE + QM_REG_MEMR_DESC_SETUP(0), v);


    /* Now format the descriptors and put them in a queue */
    for (i = 0, v = cfg->memRegionBase; i < cfg->memRegNumDescriptors; i++, v += QM_DESC_SIZE_BYTES)  {

        hd = (qmHostDesc_t *)v;
        memset (hd, 0, sizeof(qmHostDesc_t));

        hd->descInfo   = QM_DESC_DEFAULT_DESCINFO;
        hd->packetInfo = QM_DESC_DEFAULT_PINFO;
        
        if (QM_DESC_INFO_GET_PSINFO_LOC(hd->descInfo) == QM_DESC_PSINFO_IN_DESCR)  {
        	if (QM_PKT_INFO_GET_EPIB(hd->packetInfo) == QM_DESC_PINFO_EPIB)
        		w = QM_DESC_SIZE_BYTES - 32 - 16;  /* 32 bytes min descriptor size, 16 bytes extended info */
        	else
        		w = QM_DESC_SIZE_BYTES - 32;
        }  else
        	w = 0;
        	
       	QM_PKT_INFO_SET_PSINFO_SIZE(hd->packetInfo, (w >> 2));
 

        /* Push the descriptor onto the queue */
        x = deviceLocalAddrToGlobal (v);

        DEVICE_REG32_W (DEVICE_QM_MANAGER_QUEUES_BASE + QM_REG_QUEUE_REGD(cfg->destQ), x);

    }

    return (QM_OK);

} /* hwQmSetup */


/****************************************************************************************
 * FUNCTION PURPOSE: Disable the QM
 ****************************************************************************************
 * DESCRIPTION: The QM is reset by clearing the linking ram and region 0 information
 ****************************************************************************************/
void hwQmTeardown (void)
{
    UINT32 i;

    /* Linking RAM info */
    for (i = 0; i < DEVICE_QM_NUM_LINKRAMS; i++)  {
        DEVICE_REG32_W (DEVICE_QM_MANAGER_BASE + QM_REG_LINKRAM_BASE(i), 0);
        DEVICE_REG32_W (DEVICE_QM_MANAGER_BASE + QM_REG_LINKRAM_SIZE(i), 0);
    }

    /* Memory region info */
    for (i = 0; i < DEVICE_QM_NUM_MEMREGIONS; i++)  {
        DEVICE_REG32_W (DEVICE_QM_DESC_SETUP_BASE + QM_REG_MEMR_BASE_ADDR(i),  0);
        DEVICE_REG32_W (DEVICE_QM_DESC_SETUP_BASE + QM_REG_MEMR_START_IDX(i),  0);
        DEVICE_REG32_W (DEVICE_QM_DESC_SETUP_BASE + QM_REG_MEMR_DESC_SETUP(i), 0);
    }

} /* hwQmTeardown */












