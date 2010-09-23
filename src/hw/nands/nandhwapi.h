#ifndef _NAND_HW_API_H
#define _NAND_HW_API_H
/****************************************************************************************
 * FILE PURPOSE: Define the nand api which applies to all nand drivers
 ****************************************************************************************
 * FILE NAME: nandhwapi.h
 *
 * DESCRIPTION: Defines the API used by all the low level nand drivers
 *
 *  @file nandhwapi.h
 *
 *  @brief
 *		Defines the API used by all the low level nand drivers
 *
 ****************************************************************************************/
 
/* Return values */
#define NAND_INVALID_ADDR_SIZE      -810
#define NAND_NULL_ARG               -811
#define NAND_INVALID_ADDR           -812
#define NAND_ECC_FAILURE            -813


/* Information used only for programming flash */
typedef struct nandProgramInfo_s
{
    uint8   blockEraseCommandPre;  /**< The command used to erase a block (sent before the address) */
    uint8   blockEraseCommandPost; /**< The command used to erase a block (sent after the address) */
    uint8   blockEraseNaddrBytes;  /**< The number of address bytes sent */
    bool    blockErasePost;        /**< If TRUE the post command is sent */

    uint8   pageWriteCommandPre;   /**< The command used to program a page (sent before the address) */
    uint8   pageWriteCommandPost;  /**< The command used to program a page (sent after the address) */
    bool    pageWritePost;         /**< If TRUE the post command is sent */
    
}  nandProgramInfo_t; 
    



/* Driver functions */
Int32 nandHwDriverInit (nandDevInfo_t *devInfo);
Int32 nandHwDriverReadBytes (Uint32 block, Uint32 page, Uint32 byte, Uint32 nbytes, Uint8 *data);
Int32 nandHwDriverReadPage(Uint32 block, Uint32 page, Uint8 *data);
Int32 nandHwDriverClose (void);

Int32 nandHwDriverWritePage  (Uint32 block, Uint32 page, Uint8 *data, nandProgramInfo_t *winfo);
Int32 nandHwDriverBlockErase (Uint32 uiBlockNumber, nandProgramInfo_t *winfo);




#endif /* _NAND_HW_API_H */
