

/* Object file list provided in linker invokation */

-c
-a
-stack 0x400

MEMORY
{
	L2  :  origin = 0x800000  length = 0x10000

	TX  :  origin = 0x0c000000 length = 0x80000
	RX  :  origin = 0x0c080000 length = 0x80000

}

SECTIONS
{
	.text   > L2
	.heap   > L2
	.stack  > L2
	.cinit  > L2
	.const  > L2
	.far    > L2
	.sysmem > L2
	.cio    > L2
	.switch > L2

	.tx_data > TX
	.rx_data > RX

}


