#ifndef _QM_API_H
#define _QM_API_H
/*******************************************************************************************
 * FILE PURPOSE: Boot loader queue manager API definition
 *******************************************************************************************
 * FILE NAME: qm_api.h
 *
 * DESCRIPTION: Defines the stripped down QM driver API to the boot loader.
 *
 ********************************************************************************************/
 
/* return values */
#define QM_OK                                0
#define QM_INVALID_LINKRAM_ALIGNMENT        -1
#define QM_INVALID_MEMREGION_ALIGNMENT      -2
#define QM_INVALID_LINKRAM_SIZE             -3
#define QM_INVALID_LINKRAM_RAM_SIZE         -4      /* Not enough link ram for the number of descriptors */


/* Memory alignment requirements (bytes) */
#define QM_LINKRAM_ALIGN        4 
#define QM_MEMR_ALIGN           16      /* Not specified in the doc */

/* The driver supports only a single descriptor size */
#define QM_DESC_SIZE_BYTES      64

/* QM setup configuration */
typedef struct qmConfig_s  {

    UINT32 linkRamBase;
    UINT32 linkRamSize;
    
    UINT32 memRegionBase;
    UINT32 memRegNumDescriptors; 
    
    UINT32 destQ;       /* Where the initialized descriptors are placed */
    
} qmConfig_t;

typedef struct qmHostDesc_s {
    /** Descriptor type, packet type, protocol specific region location, packet length */
    UINT32          descInfo;  
    /** Source tag, Destination tag */
    UINT32          tagInfo;
    /** EPIB present, PS valid word count, error flags, PS flags, return policy, return push policy, 
     * packet return QM number, packet return queue number */
    UINT32          packetInfo;
    /** Number of valid data bytes in the buffer */
    UINT32          buffLen;
    /** Byte aligned memory address of the buffer associated with this descriptor */
    UINT32          buffPtr;
    /** 32-bit word aligned memory address of the next buffer descriptor */
    UINT32          nextBDPtr;       
    /** Completion tag, original buffer size */
    UINT32          origBufferLen;
    /** Original buffer pointer */
    UINT32          origBuffPtr;
    /** Optional EPIB word0 */
    UINT32          timeStamp;
    /** Optional EPIB word1 */
    UINT32          softwareInfo0;
    /** Optional EPIB word2 */
    UINT32          softwareInfo1;
    /** Optional EPIB word3 */
    UINT32          softwareInfo2;
    /** Optional protocol specific data */
    UINT32          psData; 
} qmHostDesc_t;


/* Descriptor values */
/* Descriptor Info: Descriptor type is host with any protocol specific info in the descriptor */
#define QM_DESC_TYPE_HOST           0
#define QM_DESC_PSINFO_IN_DESCR     0
#define QM_DESC_DEFAULT_DESCINFO       (QM_DESC_TYPE_HOST << 30)    |  \
                                       (QM_DESC_PSINFO_IN_DESCR << 22)
#define QM_DESC_INFO_GET_PSINFO_LOC(x)  BOOT_READ_BITFIELD((x), 22, 22)
                                                                        
#define QM_DESC_DESCINFO_SET_PKT_LEN(x,v)  (x) = BOOT_SET_BITFIELD((x), (v), 21, 0)
#define QM_DESC_DESCINFO_GET_PKT_LEN(x)    BOOT_READ_BITFIELD((x), 21, 0)
                                        
                                        
/* Packet Info */
#define QM_DESC_PINFO_EPIB              1
#define QM_DESC_PINFO_RETURN_OWN        1
#define QM_DESC_DEFAULT_PINFO           (QM_DESC_PINFO_EPIB << 31)          |   \
                                        (QM_DESC_PINFO_RETURN_OWN << 15) 
#define QM_PKT_INFO_GET_EPIB(x)         BOOT_READ_BITFIELD((x), 31, 31)
#define QM_PKT_INFO_SET_PSINFO_SIZE(x,v)    (x) = BOOT_SET_BITFIELD((x), (v), 29, 24)                                      
                                        
                                                                             
                                        
#define QM_DESC_PINFO_SET_QM(x,v)       (x) = BOOT_SET_BITFIELD((x), (v), 13, 12)
#define QM_DESC_PINFO_SET_QUEUE(x,v)    (x) = BOOT_SET_BITFIELD((x), (v), 11,  0)
                                        
                                        
/*  prototypes */
qmHostDesc_t *hwQmQueuePop (UINT32 qnum);
void hwQmQueuePush (qmHostDesc_t *hd, UINT32 qnum, UINT32 descSize);
SINT16 hwQmSetup (qmConfig_t *cfg);
UINT32  hwQmQueueCount (UINT32 qnum);
void hwQmTeardown (void);
SINT16 hwQmInitThreshold (UINT32 qnum);




#endif /* _QM_API_H */

