

all: gen_cdefdep romparse

romparse: cdefdep rparse.tab.o lex.yy.o romparse.c
	gcc -o romparse romparse.c rparse.tab.o lex.yy.o -I../.. -I. -D$(TARGET)


rparse.tab.o: rparse.y
	bison -d rparse.y
	gcc -c rparse.tab.c

lex.yy.o: rparse.flex
	flex rparse.flex
	gcc -c lex.yy.c


clean:
	rm -f romparse.exe rparse.tab.o lex.yy.o lex.yy.c rparse.tab.c rparse.tab.h

gen_cdefdep:
	@echo Checking command line dependencies
	@echo $(TARGET) > cdefdep.tmp
	@sh -c 'if diff -q cdefdep.tmp cdefdep ; then echo same ; else cp cdefdep.tmp cdefdep ; fi '
	





