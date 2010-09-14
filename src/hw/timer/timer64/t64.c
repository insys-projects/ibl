/*************************************************************************
 * FILE PURPOSE: Timer64 configuration
 *************************************************************************
 * FILE NAME: t64.c
 *
 * DESCRIPTION: The low level timer 64 driver
 *
 * @file t64.c
 *
 * @brief
 *  This file is used to control timer64s at the device level. Only timer 0
 *  is supported, and only in single shot mode.
 *
 **************************************************************************/
#include "types.h"
#include "iblcfg.h"
#include "ibl.h"
#include "iblloc.h"
#include "devtimer.h"
#include "t64hw.h"


/**
 * @brief
 *  A global variable is used to track the timer enable/disable status
 */
Int32 timer_created = 0;


/**
 *  @b Description
 *  @n
 *
 *  A timer is disabled
 *
 *  @retval
 *      0 - Timer deleted
 *     -1 - Timer was not active 
 */
Int32 dev_delete_timer (void) 
{ 
    Int32 retval;
    
    /* Go ahead and disable the timer, even if it is not tracked as active */
    t64_tcr   = TIMER64_TCR_DISABLE;
    t64_tgcr  = TIMER64_TGCR_DISABLE;

    /* Set the counters to 0 */
    t64_tim12 = 0;
    t64_tim34 = 0;

    /* Set the timer period to 0 */
    t64_prd12 = 0;
    t64_prd34 = 0;

    if (timer_created == 0)
        retval = -1;
    else
        retval = 0;

    timer_created = 0;

    return (retval);

}

/**
 * @b  Description
 * @n
 * 
 *  Starts the timer in one shot mode
 */
void timer_go (void)
{
    long long timdiv;

    /* Remove reset, leave timer disabled */
    t64_tgcr = TIMER64_TGCR_64;

    /* Write the timer period. 
     * The desired period is 100mus. One cycle is 1us per MHz of the device */
    timdiv = (long long)ibl.pllConfig[ibl_MAIN_PLL].pllOutFreqMhz / TIMER_INPUT_DIVIDER;
    timdiv = timdiv * 100 * 1000;
    t64_prd34 = (timdiv >> 32);
    t64_prd12 = (timdiv & 0xffffffff);

    /* Start the timer in 1 shot mode */
    t64_tcr = TIMER64_TCR_ONE_SHOT;

}


/**
 *  @b Description
 *  @n
 *
 *  A very simple hardware driver to configure the timer as a one shot 100ms timer.
 *
 *  @retval
 *      0  - Timer setup
 *     -1  - Timer setup failed, timer already enabled
 *
 */
Int32 dev_create_timer (void)
{ 
    volatile Int32 i;
    
    if (timer_created != 0)
        return (-1);

    /* Close out the timer, just to be safe */
    dev_delete_timer ();

    /* Need some delay before reprogramming the timer */
    for (i = 0; i < 500; i++);


    timer_go();

    timer_created = 1;

    return (0);

}


/**
 *  @b  Description
 *  @n
 *  
 *   The status of the timer is returned
 *
 *  @retval
 *     TRUE - The timer has expired
 *     FALSE - The timer is active
 *
 */
Bool dev_check_timer (void)
{ 
    volatile unsigned int i;

    if (timer_created == 0)
        return (FALSE);

    if ((t64_tim12 != 0) || (t64_tim34 != 0))
        return (FALSE);

    /* The timer has expired. Restart the timer */
    dev_delete_timer ();

    /* Need some delay before reprogramming the timer */
    for (i = 0; i < 500; i++);

    timer_go ();

    timer_created = 1;

    return (TRUE);

}






