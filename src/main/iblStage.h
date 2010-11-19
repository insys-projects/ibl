#ifndef _IBL_STAGE_H
#define _IBL_STAGE_H
/**
 * @file iblStage.h
 *
 * @brief
 *	  This file contains global declarations used by both stages of the IBL
 */

/**
 * @brief The ibl table is declared.
 *
 * @details
 *   The ibl table is declared uninitialized by this ibl program. An external
 *   initialization can be performed if the default operation of the ibl is
 *   not desired.
 */
#pragma DATA_SECTION(ibl, ".ibl_config_table")
ibl_t ibl;


/**
 * @brief The ibl status table is declared.
 *  
 * @details
 *   The ibl status table is declared. It is initialized at run time
 *   in function main.
 */
#pragma DATA_SECTION(iblStatus, ".ibl_status_table")
iblStatus_t iblStatus;




#endif  /* _IBL_STAGE_H */
