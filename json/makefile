a.out: driver.c y.tab.o lex.yy.o json.o
	cc -g driver.o y.tab.o lex.yy.o json.o

driver.o: driver.c y.tab.h json.h
	cc -g -c driver.c

y.tab.o: y.tab.c
	cc -g -c y.tab.c

y.tab.c: json.y
	yacc -dv -Wnone json.y

y.tab.h: y.tab.c
	echo "nono"

lex.yy.o: lex.yy.c y.tab.h json.h
	cc -g -c lex.yy.c

lex.yy.c: json.lex
	lex json.lex

json.o: json.c json.h
	cc -g -c json.c

clean:
	rm -f a.out json.o lex.yy.c lex.yy.o y.tab.c y.tab.h y.tab.o driver.o *~ y.output
