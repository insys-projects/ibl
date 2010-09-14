/******************************************************************************
 * FILE PURPOSE: Define the device timer API
 ******************************************************************************
 * FILE NAME: devtimer.h
 *
 * DESCRIPTION: The API for the all device timers
 *
 * @file devtimer.h
 *
 * @brief
 *	This file defines the API used by all device timers
 *
 *******************************************************************************/
#ifndef DEVTIMER_H
#define DEVTIMER_H

#include "types.h"
#include "devtimer.h"

Bool dev_check_timer (void);
Int32 dev_delete_timer (void);
Int32 dev_create_timer (void);


#endif /* DEVTIMER_H */








