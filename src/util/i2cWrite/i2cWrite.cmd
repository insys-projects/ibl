
/* Object file list provided in linker invokation */

-c
-a
-stack 0x400


MEMORY
{
	I2C_DAT  :  origin = 0x800000  length = 0x20000
	I2C_READ :  origin = 0x820000  length = 0x20000
	L2       :  origin = 0x840000  length = 0x10000

}

SECTIONS
{
	.i2cData > I2C_DAT
	.i2cRead > I2C_READ
	.stack   > L2
	.heap    > L2
	.text    > L2
	.cinit   > L2
	.const   > L2
	.far     > L2
	.sysmem  > L2
	.cio     > L2
	.switch  > L2

}







