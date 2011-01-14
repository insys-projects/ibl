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
   TEXT    :  origin = 0x810400 length = 0x9000
   DATA    :  origin = 0x819400 length = 0x2000
   SYSMEM  :  origin = 0x829400 length = 0x0800
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
	