# $Id$
# $Locker$

FLAGS = -static

LEX = flex
BISON = bison

all: cdu cdu.a cdu_pic.a dcltables.o

clean:
	rm *.o *.a cdu

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe

OBJS_CDU = parse.o lex.o cdumain.o tree.o stringpool.o hashtable.o cld-lang.o
OBJS = cli.o cli2.o cli3.o clilex.o elf.o
OBJS_CDU_PIC = $(addprefix pic/,$(OBJS_CDU))
OBJS_PIC = $(addprefix pic/,$(OBJS))

cdu:  cli.o elf.o cli.h main.o $(OBJS_CDU)
	gcc  -g main.o $(OBJS_CDU) -ldl -liberty -lelf $(VMSLIBS) -o cdu 

#cdu2.a: $(OBJS2)
#	ar r cdu2.a $(OBJS2)

cdu.a: $(OBJS) cli.h
	ar r cdu.a $(OBJS)

#cdu2_pic.a: $(OBJS2_PIC)
#	ar r cdu2_pic.a $(OBJS2_PIC)

cdu_pic.a: $(OBJS_PIC) cli.h
	ar r cdu_pic.a $(OBJS_PIC)

parse.o: parse.c
	$(CC) -g $(EXTRA_CFLAGS) -I../../cdu/src -c -o parse.o parse.c

clilex.o: clilex.c
	$(CC) -g $(EXTRA_CFLAGS) -I../../librtl/src -I../../starlet/src -c -o clilex.o clilex.c

%.o: %.c cli.h
	gcc -g -c $(CFLAGS) ${OLDINCLUDE} -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@ -include $(TOPDIR)/linux/include/linux/user_compiler_types.h

pic/%.o: %.c 
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

parse.h: parse.c parse.y
parse.c: parse.y
	if $(BISON) -v $(BISONFLAGS) -d -o parse.c parse.y; then \
		grep '^#define[       ]*YYEMPTY' parse.c >> parse.h ; \
	else \
		rm -f parse.c ; \
        fi

lex.c: lex.l parse.h
	$(LEX) $(LEXFLAGS) -olex.c lex.l

clilex.c: clilex.l
	$(LEX) $(LEXFLAGS) -oclilex.c -Pcli clilex.l

dcltables.c: cdu ../../cld/src/all.cld
	./cdu ../../cld/src/all.cld

include $(TOPDIR)/Rules.make.freevms
