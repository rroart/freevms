# $Id$
# $Locker$

FLAGS = -static

LEX = flex
BISON = bison

all: cdu cdu.a cdu2.a dcltables.o

clean:
	rm *.o *.a cdu

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install um/* ../../rootum/vms"$$"common/sysexe/
	install i386/* ../../rooti386/vms"$$"common/sysexe

cdu: parse.o lex.o main.o cdumain.o tree.o stringpool.o hashtable.o cld-lang.o cli.o cli.h
	gcc  -g -static parse.o lex.o main.o cdumain.o tree.o stringpool.o hashtable.o cld-lang.o cli.o -ldl -liberty $(VMSLIBS) -o cdu

cdu2.a: cli2.o
	ar r cdu2.a cli2.o

cdu.a: parse.o lex.o cdumain.o tree.o stringpool.o hashtable.o cld-lang.o cli.o cli.h
	ar r cdu.a parse.o lex.o cdumain.o tree.o stringpool.o hashtable.o cld-lang.o cli.o

%.o: %.c cli.h
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

parse.h: parse.c parse.y
parse.c: parse.y
	if $(BISON) $(BISONFLAGS) -d -o p$$$$.c parse.y; then \
		grep '^#define[       ]*YYEMPTY' p$$$$.c >> p$$$$.h ; \
		test -f p$$$$.output && mv -f p$$$$.output parse.output ; \
		mv -f p$$$$.c parse.c ; mv -f p$$$$.h parse.h ; \
	else \
		rm -f p$$$$.* ; \
		false ; \
        fi

lex.c: lex.l parse.h
	$(LEX) $(LEXFLAGS) -olex.c lex.l

dcltables.c: cdu ../../cld/src/all.cld
	./cdu ../../cld/src/all.cld