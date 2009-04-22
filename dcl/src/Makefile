LEX = flex

EXTERNAL = ../../cliutl/src/show.a ../../cliutl/src/set.a ../../cdu/src/cdu.a ../../cdu/src/dcltables.o

#all: i386/dcl${NOEXE} i386/dcl${EXE}
all: i386/dcl${NOEXE} startup.com install.com

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe
	install *.com ../../rooti386/vms"$$"common/sysexe

OBJS = cli.o clicall.o cliexpr.o
#clilex.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

#clilex.c: clilex.l
#	$(LEX) $(LEXFLAGS) -oclilex.c clilex.l

i386/dcl${NOEXE}: $(OBJS) $(VMSLIBS) $(EXTERNAL)
	install -d i386
ifeq ($(ARCH),i386)
	gcc -s -static -Wl,-Telf_i386.x $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcl${NOEXE}
	gcc -g -static -Wl,-Telf_i386.x $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcld${NOEXE}
endif
ifeq ($(ARCH),x86_64)
	gcc -s -static -Wl,-Telf_x86_64.x $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcl${NOEXE}
	gcc -g -static -Wl,-Telf_x86_64.x $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcld${NOEXE}
endif

i386/dcl${EXE}: $(OBJS) $(VMSLIBS) $(EXTERNAL)
	$(LINKER) $(LINKPRE) $(OBJS) $(EXTERNAL) $(VMSLIBS) /usr/lib/libdl.a /usr/lib/libiberty.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/dcl${EXE}
# DCL linked like this is just for testing (DCL is really special)

