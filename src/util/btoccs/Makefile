all: b2ccs.exe b2i2c.exe ccs2b.exe bfaddsect.exe bfmerge.exe ccs2bin.exe


b2ccs.exe: b2ccs.c
	gcc -o b2ccs b2ccs.c

b2i2c.exe: b2i2c.c
	gcc -o b2i2c b2i2c.c

ccs2b.exe: ccs2b.c
	gcc -o ccs2b ccs2b.c

bfaddsect.exe: bfaddsect.c
	gcc -o bfaddsect bfaddsect.c

bfmerge.exe: bfmerge.c
	gcc -o bfmerge bfmerge.c


ccs2bin.exe: ccs2bin.c
	gcc -o ccs2bin ccs2bin.c


clean:
	-rm -f b2ccs.exe b2i2c.exe ccs2b.exe bfaddsect.exe bfmerge.exe ccs2bin.exe
