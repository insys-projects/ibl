/**
 *  @file nandwriter_c6472.cmd
 *
 *  @brief
 *		Linker command file for the c6472 nand flash writer
 *
 */

nandwriter.oc
../../ecc/c64x/make/3byte_ecc.oc
../../hw/c64x/make/nandgpio.oc
../../hw/c64x/make/nandwrgpio.oc
../../hw/c64x/make/gpio.oc
../../hw/c64x/make/pll.oc



-c
-a
-stack 0x400
-heap  0x2000


MEMORY
{
	TEXT   :   origin = 0x801000   length = 0xa000
	DATA   :   origin = 0x80b000   length = 0x4000

}


SECTIONS
{

	.text  > TEXT
	.const > TEXT
	.cinit > TEXT


	.stack  > DATA
	.sysmem > DATA
	.heap   > DATA
	.far    > DATA
	.cio    > DATA

}
	











