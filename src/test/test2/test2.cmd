/* Linker command file for test2. Placement is for Tomahawk */

c0.obj
test2.obj
test2ini.obj


-c
-stack 0x400


MEMORY
{

	BLOB_ENTER  : origin = 0xe0000000,  length = 0x0040
	TEXT		: origin = 0xe0000040,  length = 0x04c0
	CONST		: origin = 0xe0000500,  length = 0x1100
	CINIT		: origin = 0xe0001600,  length = 0x0080

	STACK		: origin = 0x210000,  length = 0x400
	DATA		: origin = 0x210400,  length = 0x400

}

SECTIONS
{
	.enter  > BLOB_ENTER
	.text   > TEXT
	.dconst > CONST
	.const  > CONST
	.cinit  > CINIT

	.stack > STACK
	.data  > DATA
	.bss   > DATA
	.far   > DATA
	.data0 > DATA

}




