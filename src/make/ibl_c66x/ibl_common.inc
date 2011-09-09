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
-heap  0x8000


MEMORY
{
	TEXT_INIT :  origin = 0x800000, length = 0x4200
	TEXT      :  origin = 0x804200, length = 0xbe00
	STACK     :  origin = 0x810000, length = 0x0800
	HEAP      :  origin = 0x810800, length = 0x8000
	DATA_INIT :  origin = 0x818800, length = 0x0400
	DATA      :  origin = 0x818c00, length = 0x2c00
	CFG       :  origin = 0x81b800, length = 0x0300
	STAT      :  origin = 0x81bb00, length = 0x0200

	LINKRAM   :  origin = 0x1081be00, length = 0x0200
	CPPIRAM   :  origin = 0x1081c000, length = 0x0200
	PKTRAM    :  origin = 0x1081c200, length = 0x0800
}


SECTIONS
{
	.stack  > STACK
	.sysmem > HEAP

	.ibl_config_table > CFG
	.ibl_status_table > STAT

}

