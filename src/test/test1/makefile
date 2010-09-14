# makefile for test1

test1.btbl.bin: test1.btbl.ccs
	..\util\ccs2bin $< $@

test1.btbl.ccs: test1.btbl
	..\util\b2ccs $< $@

test1.btbl: test1.out
	hex6x -a -order L -boot -e=_c_int00 -bootorg=0x0400 -memwidth32 -romwidth32 -o=test1.btbl test1.out

test1.out: test1.obj test1.cmd
	cl6x -z -o test1.out -m test1.map test1.cmd $(TOOLSC6X)/cgtools/lib/rts64plus.lib


test1.obj: test1.c
	cl6x -g -c -mv 6400+ test1.c


