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
	TEXT_INIT :  origin = 0x801000, length = 0x2800
	TEXT      :  origin = 0x803800, length = 0xd800
	STACK     :  origin = 0x811000, length = 0x0800
	HEAP      :  origin = 0x811800, length = 0x6000
	DATA_INIT :  origin = 0x817800, length = 0x0400
	DATA      :  origin = 0x817c00, length = 0x2c00
	CFG       :  origin = 0x821800, length = 0x0300
	STAT      :  origin = 0x821b00, length = 0x0200
}


SECTIONS
{
	.stack  > STACK
	.sysmem > HEAP

	.ibl_config_table > CFG
	.ibl_status_table > STAT

}

