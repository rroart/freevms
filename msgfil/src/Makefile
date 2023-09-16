# $Id$
# $Locker$

FLAGS = -static

LEX = flex
BISON = bison

all: message 

clean:
	rm *.o *.a message

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe

message: parse.o lex.o main.o msgmain.o tree.o stringpool.o hashtable.o msg-lang.o msg.o msg.h
	gcc  -g parse.o lex.o main.o msgmain.o tree.o stringpool.o hashtable.o msg-lang.o msg.o -ldl -liberty $(VMSLIBS) -o message

message2.a: msg2.o
	ar r msg2.a msg2.o

msg.a: parse.o lex.o msgmain.o tree.o stringpool.o hashtable.o msg-lang.o msg.o msg.h
	ar r msg.a parse.o lex.o msgmain.o tree.o stringpool.o hashtable.o msg-lang.o msg.o

parse.o: parse.c
	$(CC) -g $(EXTRA_CFLAGS) -c -o parse.o parse.c

%.o: %.c msg.h
	gcc -g -c $(CFLAGS) ${OLDINCLUDE} -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@ -include $(TOPDIR)/linux/include/linux/user_compiler_types.h

parse.h: parse.c parse.y
parse.c: parse.y
	if $(BISON) -v $(BISONFLAGS) -d -o parse.c parse.y; then \
		grep '^#define[       ]*YYEMPTY' parse.c >> parse.h ; \
	else \
		rm -f parse.c ; \
        fi

lex.c: lex.l parse.h
	$(LEX) $(LEXFLAGS) -olex.c lex.l

msgtables.c: message all.msg
	./message all.msg

include $(TOPDIR)/Rules.make.freevms
