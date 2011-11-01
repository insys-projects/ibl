/*************************************************************************
 * @file  i2cparam_c6472.cmd
 *
 * @brief  Places the i2c parameter writer program into memory
 *
 *************************************************************************/

/* Object files included in linker invokation */

-c
-a
-stack 0x400

MEMORY
{
   STACK   :  origin = 0x810000 length = 0x0400
   TEXT    :  origin = 0x810400 length = 0x8f00
   DATA    :  origin = 0x819300 length = 0x0900
   SYSMEM  :  origin = 0x80a000 length = 0x0800
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

	.sysmem > SYSMEM

}
	
