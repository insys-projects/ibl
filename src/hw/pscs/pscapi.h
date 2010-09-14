#ifndef _PSCAPI_H
#define _PSCAPI_H
/**********************************************************************************
 * FILE PURPOSE: Define the power save controller api
 **********************************************************************************
 * FILE NAME: pscapi.h
 *
 * DESCRIPTION: Defines the driver interface for the psc controller driver.
 *
 * Copyright (C) 2006, Texas Instruments, Inc.
 *
 * @file pscapi.h
 *
 * @brief
 *  Defines the API used by all power controllers
 *
 **********************************************************************************/
#include "types.h"

int16 pscEnableModule (uint32 modNum);
int16 pscDisableModule (uint32 modNum);
BOOL pscModuleIsEnabled (uint32 modNum);
int16 pscSetResetIso (uint32 modNum);
int16 pscDisableDomain (uint32 domainNum);
void pscDisableModAndDomain (uint32 modNum, uint32 domainNum);





#endif /* _PSCAPI_H */
