
romparse: rparse.tab.o lex.yy.o romparse.c
	gcc -o romparse romparse.c rparse.tab.o lex.yy.o -I../.. -I.


rparse.tab.o: rparse.y
	bison -d -S bison.simple rparse.y
	gcc -c rparse.tab.c

lex.yy.o: rparse.flex
	flex rparse.flex
	gcc -c lex.yy.c









