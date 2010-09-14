/** 
 *   @file  timer.h
 *
 *   @brief   
 *      The file has data structures and API definitions for the
 *      TIMER Module 
 *
 *  \par
 *  NOTE:
 *      (C) Copyright 2008, Texas Instruments, Inc.
 *
 *  \par
 */
#ifndef __TIMER_H__
#define __TIMER_H__


/**********************************************************************
 **************************** Exported Functions **********************
 **********************************************************************/

extern void  timer_init(void);
extern Int32 timer_add (Uint32 timeout, void (*expiry)(void));
extern void  timer_delete(Int32 handle);
extern void  timer_run (void);

#endif /* __TIMER_H__ */

