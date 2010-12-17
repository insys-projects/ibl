#ifndef _PA_LOC_H
#define _PA_LOC_H
/******************************************************************************************
 * FILE PURPOSE: Provide local Packet Accelerator definitions
 ******************************************************************************************
 * FILE NAME: pa_loc.h
 *
 * DESCRIPTION: Defines the packet accelerator sub-system and hardware interface
 *
 ******************************************************************************************/


#define PA_REG_MAILBOX_SLOT(pdsp, slot)		(0x00 + ((pdsp)*0x10) + ((slot)*0x04))
#define PA_REG_PDSP_CTL(pdsp)               (0x1000 + ((pdsp)*0x100))
#define PA_MEM_PDSP_IRAM(pdsp)				(0x10000 + ((pdsp)*0x8000))


/* The pdsp control register */
#define PA_REG_VAL_PDSP_CTL_ENABLE_PDSP(pcVal)   (((pcVal) << 16) | 0x3)
#define PA_REG_VAL_PDSP_CTL_DISABLE_PDSP         0

/* Number of mailbox slots for each PDPS */
#define PA_NUM_MAILBOX_SLOTS                4




#endif /* _PA_LOC_H */

