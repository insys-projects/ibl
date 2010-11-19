/**
 *   @file  timer.c
 *
 *   @brief   
 *      The file implements the TIMER module. 
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
 #include "types.h"
 #include "iblloc.h"
 #include "timer.h"
 #include "devtimer.h"
 #include "iblcfg.h"
 #include "timer_osal.h"
 #include <string.h>


/**********************************************************************
 *************************** LOCAL Structures *************************
 **********************************************************************/

/**
 * @brief 
 *  The structure describes the Timer blocks.
 *
 * @details
 *  Each timer in the system is associated with a timer
 *  block.  
 */
typedef struct TIMER_BLOCK
{
    /**
     * @brief  Expiration Routine which is called once the timer
     * expires.
     */
    void        (*expiry)(void);

    /**
     * @brief  This is the original expiration time in milli-seconds.
     */
    Uint32      org_timeout;

    /**
     * @brief  This is the expiration time in milli-seconds. This is
     * decremented by the timer module.
     */
    Uint32      timeout;
}TIMER_BLOCK;

/**
 * @brief 
 *  The structure describes the Timer Master Control Block.
 *
 * @details
 *  The structure contains information about the Timer Master Control
 *  block which stores information about the Timer Module.
 */
typedef struct TIMER_MCB
{
    /**
     * @brief  This keeps track of all timers which can exist in the
     * System.
     */
    TIMER_BLOCK   timer[MAX_TIMER_BLOCKS];

    /**
     * @brief  This is the number of active timers in the system.
     */
    Uint32        num_active_timers;
}TIMER_MCB;

/**********************************************************************
 *************************** GLOBAL Variables *************************
 **********************************************************************/

/**
 * @brief   This global variable keeps track of all the timer which are
 * currently active in the system.
 */
TIMER_MCB   timermcb;

/**********************************************************************
 *************************** TIMER Functions **************************
 **********************************************************************/

/**
 *  @b Description
 *  @n  
 *      The function initializes the Timer module.
 *
 *  @retval
 *      Not Applicable.
 */
void timer_init (void)
{
    /* Initialize the Timer Master Control Block. */
    timerMemset (&timermcb, 0, sizeof(TIMER_MCB));
    return;
}

/**
 *  @b Description
 *  @n  
 *      The function creates a timer.
 *
 *  @param[in]  timeout
 *      This is the timeout specified in milli-seconds after which the timer
 *      will expire.
 *  @param[in]  expiry
 *      Expiration routine which is called to indicate that the timer block
 *      has expired.
 *
 *  @retval
 *      Success  -  Handle to the timer block
 *  @retval
 *      Error    -  <0
 */
Int32 timer_add (Uint32 timeout, void (*expiry)(void))
{
    Uint16  index;

    /* Basic Validations: Ensure parameters passed are valid. */
    if ((timeout == 0) || (expiry == NULL))
        return -1;

    /* Cycle through and find a free timer block. */
    for (index = 0; index < MAX_TIMER_BLOCKS; index++)
    {
        /* Free timers can be identified using the expiry routine. NULL indicates
         * that the timer block is free. */
        if (timermcb.timer[index].expiry == NULL)
        {
            /* Found a free slot. Is this the first timer being created? */
            if (timermcb.num_active_timers == 0)
            {
                /* YES. We need to start the device timer. */
                if (dev_create_timer() < 0)
                {
                    /* Device layer timer creation failed. We will not be able
                     * to get the timers operational without this API. So we 
                     * might as well fail also. */
                    return -1;
                }
            }

            /* Populate the timer block structure. */
            timermcb.timer[index].expiry      = expiry;
            timermcb.timer[index].timeout     = timeout;
            timermcb.timer[index].org_timeout = timeout;

            /* Increment the number of timers in the system */
            timermcb.num_active_timers++;

            /* Return the handle to the timer block. */
            return index;
        }
    }

    /* Control comes here indicating that there were no free timer slots. */
    return -1;
}

/**
 *  @b Description
 *  @n  
 *      The function deletes a timer which was previously created.
 *
 *  @param[in]  handle
 *      This is the handle to the timer block to be deleted.
 *
 *  @retval
 *      Not Applicable.
 */
void timer_delete (Int32 handle)
{
    /* Basic Validations: Ensure paramter is valid. */
    if ((handle < 0) || (handle > MAX_TIMER_BLOCKS))
        return;

    /* Make sure there is at least one timer which is active; */
    if (timermcb.num_active_timers > 0)
    {
        /* Simply reset the memory contents */
        timerMemset ((void *)&timermcb.timer[handle], 0, sizeof(TIMER_BLOCK));

        /* Decrement the number of active timers in the system */
        timermcb.num_active_timers--;

        /* Check if there are any active timers in the system? */
        if (timermcb.num_active_timers == 0)
        {
            /* No more active timers; we can delete the timer in the device layer. */
            dev_delete_timer ();
        }
    }
    return;
}

/**
 *  @b Description
 *  @n  
 *      The function runs the timer scheduler. This API is required to be 
 *      called by the boot modules and this in turn will decrement the various
 *      timer registered in the system and will call the appropriate
 *      expiration routines. If the boot modules fail to call this API then 
 *      the timers will never expire.
 *
 *  @retval
 *      Not Applicable.
 */
void timer_run (void)
{
    Uint16  index;

    /* Check if there are any active timers in the System or not? 
     * If none are present; then we dont need to run the scheduler. */
    if (timermcb.num_active_timers == 0)
        return;

    /* Check if the device timer has expired or not? 
     *  If the timer has not expired there is no management to be done. */
    if (dev_check_timer() == FALSE)
        return;

    /* Cycle through all the timer blocks and update them. */
    for (index = 0; index < MAX_TIMER_BLOCKS; index++)
    {
        /* Is this an active timer? */
        if (timermcb.timer[index].expiry != NULL)
        {
            /* YES. Decrement the timeout. */
            timermcb.timer[index].timeout = timermcb.timer[index].timeout - 100;

            /* Has the timer expired? */
            if (timermcb.timer[index].timeout == 0)
            {
                /* Reset the timeouts */
                timermcb.timer[index].timeout = timermcb.timer[index].org_timeout;

                /* Call the expiration routine. */   
                timermcb.timer[index].expiry();
            }

        }
    }
    return;
}


