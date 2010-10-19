
i2cRead.oc
../../hw/c64x/make/i2c.oc

-c
-a
-stack 0x400


MEMORY
{
	I2C_DAT  :  origin = 0x800000  length = 0x10000
	L2       :  origin = 0x810000  length = 0x10000

}

SECTIONS
{
	.i2cData > I2C_DAT
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







