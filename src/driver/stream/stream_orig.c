/**
 *   @file  stream.c
 *
 *   @brief   
 *      The file implements the STREAM module.
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2008, Texas Instruments, Inc.
 *
 *  \par
 */


/**********************************************************************
 ************************** Local Structures **************************
 **********************************************************************/
#include "types.h"
#include "iblcfg.h"
#include <string.h>

/**
 * @brief 
 *  The structure describes the Stream Master Control block.
 *
 * @details
 *  The structure stores information about the stream module
 *  internal buffers and state information.
 */
typedef struct STREAM_MCB
{
    /**
     * @brief   Flag which indicates if the stream buffer is open or not?
     */
    Bool       is_open;
        
    /**
     * @brief   This is the *internal* stream buffer.
     */
    Uint8      buffer[MAX_SIZE_STREAM_BUFFER];

    /**
     * @brief   This is the read index from where data is read.
     */
    Int32      read_idx;

    /**
     * @brief   This is the write index to which data is written.
     */
    Int32      write_idx;

    /**
     * @brief   This is the free size available in the internal buffer.
     */
    Int32      free_size;
}STREAM_MCB;

/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/

/**
 * @brief   This is the STREAM Master control block which keeps track
 * of all the stream module information.
 */
STREAM_MCB  stream_mcb;

/**********************************************************************
 ************************** Stream Functions **************************
 **********************************************************************/

/**
 *  @b Description
 *  @n
 *      The function is called to open the stream module
 *
 *  @param[in]  chunk_size
 *      Maximum amount of data that can be received at any
 *      instant by the boot module.
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
Int32 stream_open (Uint32 chunk_size)
{
    /* Basic Validations: Ensure that the chunk size is not greater
     * than the internal buffer size. */
    if (chunk_size > MAX_SIZE_STREAM_BUFFER)
        return -1;

    /* Initialize the Master control block. */
    stream_mcb.is_open   = TRUE;
    stream_mcb.read_idx  = 0;
    stream_mcb.write_idx = 0;
    stream_mcb.free_size = MAX_SIZE_STREAM_BUFFER;

    /* Module has been initialized. */
    return 0;
}

/**
 *  @b Description
 *  @n
 *      The function is called to read data from the stream module.
 *
 *  @param[in]  ptr_data
 *      Pointer to the data buffer where the data will be copied to.
 *  @param[in]  num_bytes
 *      Number of bytes to be read.
 *
 *  @retval
 *      Success -   Number of bytes actually read
 *  @retval
 *      Error   -   <0
 */
Int32 stream_read (Uint8* ptr_data, Int32 num_bytes)
{
    Int32 index;
    Int32 num_bytes_to_read;
    
    /* Determine the number of bytes which can be read. */
    if (num_bytes > (MAX_SIZE_STREAM_BUFFER - stream_mcb.free_size))
    {
        /* User has requested more data than what is available. In this case we 
         * can return only what we have. */
        num_bytes_to_read = (MAX_SIZE_STREAM_BUFFER - stream_mcb.free_size);
    }
    else
    {
        /* User has requested less data than what is available. In this case we 
         * return only what the user wants. */            
        num_bytes_to_read = num_bytes;
    }

    /* There is data available copy it from the internal to the user supplied buffer.  */
    for (index = 0; index < num_bytes_to_read; index++)
    {
        /* Copy the data to the "write" index. */
        *(ptr_data + index) = *(stream_mcb.buffer + stream_mcb.read_idx);

        /* Increment the read index. */
        stream_mcb.read_idx = (stream_mcb.read_idx + 1) % MAX_SIZE_STREAM_BUFFER;
    }

    /* Once data has been copied; increment the free size accordingly */
    stream_mcb.free_size = stream_mcb.free_size + num_bytes_to_read;

    /* Return the number of bytes read. */
    return num_bytes_to_read;
}

/**
 *  @b Description
 *  @n
 *      The function is called to write data to the stream
 *      module.
 *
 *  @param[in]  ptr_data
 *      Pointer to the data buffer which contains the data to be copied.
 *  @param[in]  num_bytes
 *      Number of bytes being written
 *
 *  @retval
 *      Success -   0
 *  @retval
 *      Error   -   <0
 */
Int32 stream_write (Uint8* ptr_data, Int32 num_bytes)
{
    Int32 index;

    /* Basic Validations: Ensure there is sufficient space to copy the data. */
    if (num_bytes > stream_mcb.free_size)
        return -1;

    /* Basic Validations: Make sure the pointers are valid. */
    if (ptr_data == NULL)
        return -1;

    /* There was sufficient space to copy the data lets do so but we copy byte by byte
     * since the internal buffer is circular and we can wrap around... */
    for (index = 0; index < num_bytes; index++)
    {
        /* Copy the data to the "write" index. */
        *(stream_mcb.buffer + stream_mcb.write_idx) = *(ptr_data + index);

        /* Increment the write index. */
        stream_mcb.write_idx = (stream_mcb.write_idx + 1) % MAX_SIZE_STREAM_BUFFER;
    }

    /* Once data has been copied; decrement the free size accordingly */
    stream_mcb.free_size = stream_mcb.free_size - num_bytes;
    return 0;
}

/**
 *  @b Description
 *  @n
 *      The function is used to check if the stream buffers are empty or
 *      not?
 *
 *  @retval
 *      Empty       -   TRUE
 *  @retval
 *      Not Empty   -   FALSE
 */
Bool stream_isempty (void)
{
    /* Check the number of free bytes available? */
    if (stream_mcb.free_size == MAX_SIZE_STREAM_BUFFER)
        return TRUE;

    /* There is data in the stream buffer; so its not empty. */
    return FALSE;
}

/**
 *  @b Description
 *  @n
 *      The function closes the stream module.
 *
 *  @retval
 *      Not Applicable.
 */
void stream_close (void)
{
    /* The stream module is no longer open... */
    stream_mcb.is_open = FALSE;
    return;
}

/**
 *  @b Description
 *  @n
 *      The function initializes the stream module.
 *
 *  @retval
 *      Not Applicable.
 */
void stream_init (void)
{
    /* Reset the memory contents. */
    memset ((void *)&stream_mcb, 0, sizeof(STREAM_MCB));

    /* Make sure we initialize the free size correctly. */
    stream_mcb.free_size = MAX_SIZE_STREAM_BUFFER;
    return;
}

