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
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  \par
*/


/**********************************************************************
 ************************** Local Structures **************************
 **********************************************************************/
#include "types.h"
#include "iblcfg.h"
#include "stream_osal.h"
#include <string.h>

/** 
 * @defgroup stream_op
 *
 * @ingroup stream_op
 * @{
 *
 * @brief
 *  Internal defintion to distinguish between read and peek
 */
/**
 *  @def STREAM_READ
 */
#define STREAM_READ     200   /**< Read from a stream */

/**
 *  @def STREAM_PEEK
 */
#define STREAM_PEEK     210   /**< Peek from a stream */

/* @} */



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
 *      The function is called to read/peek data from the stream module.
 *
 *  @param[in]  ptr_data
 *      Pointer to the data buffer where the data will be copied to.
 *  @param[in]  num_bytes
 *      Number of bytes to be read.
 *  @param[in]  op
 *      Distinguishes a read from a peek @ref stream_op
 *
 *  @retval
 *      Success -   Number of bytes actually read
 *  @retval
 *      Error   -   <0
 */
Int32 stream_read_peek (Uint8* ptr_data, Int32 num_bytes, Int32 op)
{
    Int32 index, read_index;
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
        if (ptr_data != NULL)
        {
            read_index = stream_mcb.read_idx + index;
            read_index = read_index % MAX_SIZE_STREAM_BUFFER;
            *(ptr_data + index) = *(stream_mcb.buffer + read_index);
        }
    }

    /* Increment the read index. 
    * Once data has been copied; increment the free size accordingly */
    if (op == STREAM_READ)  {
        stream_mcb.read_idx  = (stream_mcb.read_idx + num_bytes_to_read) % MAX_SIZE_STREAM_BUFFER;
        stream_mcb.free_size = stream_mcb.free_size + num_bytes_to_read;
    }


    /* Return the number of bytes read. */
    return num_bytes_to_read;
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
    return (stream_read_peek (ptr_data, num_bytes, STREAM_READ));
}

/**
 *  @b Description
 *  @n
 *      The function is called to peek data from the stream module.
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
Int32 stream_peek (Uint8* ptr_data, Int32 num_bytes)
{
    return (stream_read_peek (ptr_data, num_bytes, STREAM_PEEK));
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
    stream_mcb.is_open   = FALSE;
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
    streamMemset ((void *)&stream_mcb, 0, sizeof(STREAM_MCB));

    /* Make sure we initialize the free size correctly. */
    stream_mcb.free_size = MAX_SIZE_STREAM_BUFFER;
    return;
}


/**
 *  @b Description
 *  @n
 *      Returns the number of bytes currently available in the stream
 *
 *  @retval
 *      The number of bytes in the stream buffer
 *      -1 if the stream is closed AND empty
 */
Int32 stream_level (void)
{
    Int32 remain;

    remain = MAX_SIZE_STREAM_BUFFER - stream_mcb.free_size;

    if ((stream_mcb.is_open != TRUE) && (remain == 0))
        return (-1);

    return (remain);

}
