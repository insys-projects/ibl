/*************************************************************************
 * @file  i2cparam_c6472.cmd
 *
 * @brief  Places the i2c parameter writer program into memory
 *
 *************************************************************************/

i2cparam.oc
../../hw/c64x/make/i2c.oc
../../hw/c64x/make/pll.oc

-c
-a
-stack 0x400

MEMORY
{
   STACK   :  origin = 0x810000 length = 0x0400
   TEXT    :  origin = 0x810400 length = 0x8000
   DATA    :  origin = 0x818400 length = 0x0800
}

SECTIONS
{
	.stack > STACK

	.text   > TEXT
	.const  > TEXT
	.switch > TEXT
	.cinit  > TEXT

	.far    > DATA
	.bss    > DATA
	.data   > DATA

}
	