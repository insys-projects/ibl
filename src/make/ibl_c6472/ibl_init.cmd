/************************************************************************************
 * FILE PURPOSE: Define the memory usage of the ibl module for the c6455
 ************************************************************************************
 * FILE NAME: ibl.cmd
 *
 * DESCRIPTION: The memory placement for the IBL is defined
 *
 ************************************************************************************/


../main/c64x/make/iblinit.oc
../device/c64x/make/c6472init.oc
../hw/c64x/make/pll.oc
../hw/c64x/make/i2c.oc
../interp/c64x/make/btblwrap.oc
../interp/c64x/make/btblpr.oc
../interp/c64x/make/gem.oc

-c
-stack 0x800
-heap  0x6000



MEMORY
{
	TEXT   :  origin = 0x801000, length = 0x20000
	STACK  :  origin = 0x821000, length = 0x0800
	HEAP   :  origin = 0x821800, length = 0x6000
	DATA   :  origin = 0x827800, length = 0x3000
	CFG    :  origin = 0x831800, length = 0x0300
	STAT :    origin = 0x831b00, length = 0x0200
}

SECTIONS
{
	.stack  > STACK
	.sysmem > HEAP
	.cinit  > TEXT
	.const  > TEXT
	.text   > TEXT
	.switch > TEXT
	.far    > DATA
	.bss    > DATA

    .ibl_config_table > CFG
	.ibl_status_table > STAT

}

