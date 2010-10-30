/************************************************************************************
 * FILE PURPOSE: Define the memory usage of the ibl module for the c6474
 ************************************************************************************
 * FILE NAME: ibl.cmd
 *
 * DESCRIPTION: The memory placement for the IBL is defined
 *
 ************************************************************************************/


../main/c64x/make/iblmain.oc
../device/c64x/make/c6457.oc
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
../hw/c64x/make/pll.oc
../hw/c64x/make/psc.oc
../hw/c64x/make/emif31.oc
../hw/c64x/make/mdio.oc
../hw/c64x/make/i2c.oc
../hw/c64x/make/sgmii.oc
../interp/c64x/make/bis.oc
../interp/c64x/make/cload.oc
../interp/c64x/make/osal.oc
../interp/c64x/make/btblwrap.oc
../interp/c64x/make/btblpr.oc
../interp/c64x/make/gem.oc
../interp/c64x/make/blob.oc

/* ../interp/c64x/make/dload.oc */
/* ../interp/c64x/make/elfwrap.oc */
/* ../interp/c64x/make/dlw_client.oc */
/* ../interp/c64x/make/dload_endian.oc */
/* ../interp/c64x/make/ArrayList.oc */

/* ../nandboot/c64x/make/nandboot.oc */
/* ../driver/c64x/make/nand.oc */
/* ../ecc/c64x/make/3byte_ecc.oc */
/* ../hw/c64x/make/gpio.oc */
/* ../hw/c64x/make/nandgpio.oc */

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

