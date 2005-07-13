# $Id$
# $Locker$

FLAGS = -static

LEX = flex
BISON = bison

all: message 

clean:
	rm *.o *.a message

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install um/* ../../rootum/vms"$$"common/sysexe/
	install i386/* ../../rooti386/vms"$$"common/sysexe

message: parse.o lex.o main.o msgmain.o tree.o stringpool.o hashtable.o msg-lang.o msg.o msg.h
	gcc  -g -static parse.o lex.o main.o msgmain.o tree.o stringpool.o hashtable.o msg-lang.o msg.o -ldl -liberty $(VMSLIBS) -o message

message2.a: msg2.o
	ar r msg2.a msg2.o

msg.a: parse.o lex.o msgmain.o tree.o stringpool.o hashtable.o msg-lang.o msg.o msg.h
	ar r msg.a parse.o lex.o msgmain.o tree.o stringpool.o hashtable.o msg-lang.o msg.o

%.o: %.c msg.h
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

parse.h: parse.c parse.y
parse.c: parse.y
	if $(BISON) -v $(BISONFLAGS) -d -o p$$$$.c parse.y; then \
		grep '^#define[       ]*YYEMPTY' p$$$$.c >> p$$$$.h ; \
		test -f p$$$$.output && mv -f p$$$$.output parse.output ; \
		mv -f p$$$$.c parse.c ; mv -f p$$$$.h parse.h ; \
	else \
		rm -f p$$$$.* ; \
		false ; \
        fi

lex.c: lex.l parse.h
	$(LEX) $(LEXFLAGS) -olex.c lex.l

msgtables.c: message all.msg
	./message all.msg
