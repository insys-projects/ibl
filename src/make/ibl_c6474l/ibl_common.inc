/************************************************************************************
 * FILE PURPOSE: Provide common memory and sections definitions for ibl_init and ibl
 ************************************************************************************
 * FILE NAME: ibl_common.inc
 *
 * DESCRIPTION: Defines the common memory map and section placement required
 *			    to get ibl and ibl_init to work together in a two stage load
 *				process.
 *************************************************************************************/

-c
-stack 0x800
-heap  0x6000


MEMORY
{
	TEXT_INIT :  origin = 0x801000, length = 0x2a00
	TEXT      :  origin = 0x803a00, length = 0xd600
	STACK     :  origin = 0x811000, length = 0x0800
	HEAP      :  origin = 0x811800, length = 0x6000
	DATA_INIT :  origin = 0x817800, length = 0x0400
	DATA      :  origin = 0x817c00, length = 0x2c00
	CFG       :  origin = 0x821800, length = 0x0300
	STAT      :  origin = 0x821b00, length = 0x0200
	CORE_1	  :  origin = 0x11800000, length = 4
	CORE_2	  :  origin = 0x12800000, length = 4
}


SECTIONS
{
	.stack  > STACK
	.sysmem > HEAP

	.ibl_config_table > CFG
	.ibl_status_table > STAT

	.idle_c1 > CORE_1
	.idle_c2 > CORE_2
}

