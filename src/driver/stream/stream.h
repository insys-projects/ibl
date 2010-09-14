/** 
 *   @file  stream.h
 *
 *   @brief   
 *      The file has data structures and API definitions for the
 *      STREAM Module 
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2008, Texas Instruments, Inc.
 *
 *  \par
 */
#ifndef __STREAM_H__
#define __STREAM_H__

/**********************************************************************
 **************************** Exported Functions **********************
 **********************************************************************/

extern void  stream_init   (void);
extern Int32 stream_open   (Uint32 chunk_size);
extern void  stream_close  (void);
extern Int32 stream_read   (Uint8* ptr_data, Int32 num_bytes);
extern Int32 stream_peek   (Uint8* ptr_data, Int32 num_bytes);
extern Int32 stream_write  (Uint8* ptr_data, Int32 num_bytes);
extern Bool  stream_isempty(void);
extern Int32 stream_level  (void);

#endif /* __STREAM_H__ */
