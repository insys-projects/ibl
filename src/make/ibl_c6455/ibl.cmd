/************************************************************************************
 * FILE PURPOSE: Define the memory usage of the ibl module for the c6455
 ************************************************************************************
 * FILE NAME: ibl.cmd
 *
 * DESCRIPTION: The memory placement for the IBL is defined
 *
 ************************************************************************************/


../main/c64x/make/iblmain.oc
../device/c64x/make/c6455.oc
../ethboot/c64x/make/ethboot.oc
../driver/c64x/make/net.oc
../driver/c64x/make/arp.oc
../driver/c64x/make/ip.oc
../driver/c64x/make/udp.oc
../driver/c64x/make/stream.oc
../driver/c64x/make/timer.oc
../driver/c64x/make/bootp.oc
../driver/c64x/make/tftp.oc
../hw/c64x/make/t64.oc
../hw/c64x/make/cpmacdrv.oc
../interp/c64x/make/bis.oc

-c
-stack 0x400



MEMORY
{
	L2   :  origin = 0x800000, length = 0x10000
	CFG  :  origin = 0x810000, length = 0x0200
	STAT :  origin = 0x810200, length = 0x0200
}

SECTIONS
{
	.stack  > L2
	.cinit  > L2
	.const  > L2
	.text   > L2
	.far    > L2
	.switch > L2
	.bss    > L2

    .ibl_config_table > CFG
	.ibl_status_table > STAT

}

