LEX = flex

EXTERNAL = ../../cliutl/src/show.a ../../cliutl/src/set.a ../../cdu/src/cdu.a ../../cdu/src/dcltables.o

#all: i386/dcl${NOEXE} i386/dcl${EXE}
all: i386/dcl${NOEXE} startup.com install.com test1.com test2.com

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe
	install *.com ../../rooti386/vms"$$"common/sysexe

OBJS = cli.o clicall.o cliexpr.o
#clilex.o

%.o: %.c
	gcc -g -c ${OLDINCLUDE} -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

#clilex.c: clilex.l
#	$(LEX) $(LEXFLAGS) -oclilex.c clilex.l

i386/dcl${NOEXE}: $(OBJS) $(VMSLIBS) $(EXTERNAL)
	install -d i386
ifeq ($(ARCH),i386)
	gcc -s -static -Wl,-Telf_i386.x -L$(OLDLIBCDIR_I386) -L$(OLDLIBELFDIR_I386) $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcl${NOEXE}
	gcc -g -static -Wl,-Telf_i386.x -L$(OLDLIBCDIR_I386) -L$(OLDLIBELFDIR_I386) $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcld${NOEXE}
#	gcc -s -nostdlib -static -Wl,-Telf_i386.x $(OBJS) $(EXTERNAL) $(VMSLIBS) $(OLDLIBDL_I386) $(OLDLIBELF_I386) -o i386/dcl${NOEXE} $(OLDCRT1_I386) $(OLDCRTI_I386) /usr/lib/gcc/i486-linux-gnu/4.4.5/crtbeginT.o $(OLDLIBC_I386) -lgcc_eh -lgcc $(OLDLIBC_I386) /usr/lib/gcc/i486-linux-gnu/4.4.5/crtend.o $(OLDCRTN_I386) 
#	gcc -g -nostdlib -static -Wl,-Telf_i386.x $(OBJS) $(EXTERNAL) $(VMSLIBS) $(OLDLIBDL_I386) $(OLDLIBELF_I386) -o i386/dcld${NOEXE} $(OLDCRT1_I386) $(OLDCRTI_I386) $(OLDLIBC_I386) -lgcc_eh -lgcc $(OLDLIBC_I386) $(OLDCRTN_I386) 
endif
ifeq ($(ARCH),x86_64)
	gcc -s -static -Wl,-Telf_x86_64.x -L$(OLDLIBCDIR_AMD64) -L$(OLDLIBELFDIR_AMD64) $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcl${NOEXE}
	gcc -g -static -Wl,-Telf_x86_64.x -L$(OLDLIBCDIR_AMD64) -L$(OLDLIBELFDIR_AMD64) $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcld${NOEXE}
#	gcc -s -static -Wl,-Telf_x86_64.x $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcl${NOEXE}
#	gcc -g -static -Wl,-Telf_x86_64.x $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcld${NOEXE}
#	gcc -s -nostdlib -static -Wl,-Telf_x86_64.x $(OBJS) $(EXTERNAL) $(VMSLIBS) $(OLDLIBDL_AMD64) $(OLDLIBELF_AMD64) -o i386/dcl${NOEXE} $(OLDCRT1_AMD64) $(OLDCRTI_AMD64) $(OLDLIBC_AMD64) -lgcc_eh -lgcc $(OLDLIBC_AMD64) $(OLDCRTN_AMD64)
#	gcc -g -nostdlib -static -Wl,-Telf_x86_64.x $(OBJS) $(EXTERNAL) $(VMSLIBS) $(OLDLIBDL_AMD64) $(OLDLIBELF_AMD64) -o i386/dcld${NOEXE} $(OLDCRT1_AMD64) $(OLDCRTI_AMD64) $(OLDLIBC_AMD64) -lgcc_eh -lgcc $(OLDLIBC_AMD64) $(OLDCRTN_AMD64) 
endif

i386/dcl${EXE}: $(OBJS) $(VMSLIBS) $(EXTERNAL)
	$(LINKER) $(LINKPRE) $(OBJS) $(EXTERNAL) $(VMSLIBS) /usr/lib/libdl.a /usr/lib/libiberty.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/dcl${EXE}
# DCL linked like this is just for testing (DCL is really special)

