/* Linker command file for test1 */

test1.obj

-c
-stack 0x200

/* Memory must not conflict with the IBL */
MEMORY
{
	TEXT   :  origin = 0x840000,  length = 0x1000
	STACK  :  origin = 0x841000,  length = 0x200
	CONST  :  origin = 0x841200,  length = 0x100
	FAR    :  origin = 0x841300,  length = 0xd00
	CINIT  :  origin = 0x842000,  length = 0x400
	SWITCH :  origin = 0x842400,  length = 0x400

}

SECTIONS
{
	.text   > TEXT
	.stack  > STACK
	.const  > CONST
	.far    > FAR
	.bss    > FAR
	.cinit  > CINIT
	.switch > SWITCH

}


