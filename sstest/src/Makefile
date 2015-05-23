#all: i386/sstest${NOEXE} i386/sstest${EXE}
all: i386/sstest${NOEXE}

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/systest
	install i386/* ../../rooti386/vms"$$"common/systest

OBJS = main.o devicetest.o logicaltest.o processtest.o systemtest.o timetest.o

i386/sstest${NOEXE}: $(OBJS) $(VMSLIBS)
	install -d i386
ifeq ($(ARCH),i386)
	gcc -s -static -Wl,-Telf_i386.x -L$(OLDLIBCDIR_I386) -L$(OLDLIBELFDIR_I386) $(OBJS) $(VMSLIBS) -ldl -lelf -o i386/sstest${NOEXE}
#	gcc -s -nostdlib -static -Wl,-Telf_i386.x $(OBJS) $(VMSLIBS) $(OLDLIBDL_I386) $(OLDLIBELF_I386) -o i386/sstest${NOEXE} $(OLDCRT1_I386) $(OLDCRTI_I386) /usr/lib/gcc/i486-linux-gnu/4.4.5/crtbeginT.o $(OLDLIBC_I386) -lgcc_eh -lgcc $(OLDLIBC_I386) /usr/lib/gcc/i486-linux-gnu/4.4.5/crtend.o $(OLDCRTN_I386) 
endif
ifeq ($(ARCH),x86_64)
	gcc -s -static -Wl,-Telf_x86_64.x -L$(OLDLIBCDIR_AMD64) -L$(OLDLIBELFDIR_AMD64) $(OBJS) $(VMSLIBS) -ldl -lelf -o i386/sstest${NOEXE}
#	gcc -s -static -Wl,-Telf_x86_64.x $(OBJS) $(VMSLIBS) -ldl -lelf -o i386/sstest${NOEXE}
#	gcc -s -nostdlib -static -Wl,-Telf_x86_64.x $(OBJS) $(VMSLIBS) $(OLDLIBDL_AMD64) $(OLDLIBELF_AMD64) -o i386/sstest${NOEXE} $(OLDCRT1_AMD64) $(OLDCRTI_AMD64) $(OLDLIBC_AMD64) -lgcc_eh -lgcc $(OLDLIBC_AMD64) $(OLDCRTN_AMD64)
endif

i386/sstest${EXE}: $(OBJS) $(VMSLIBS)
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) /usr/lib/libdl.a /usr/lib/libiberty.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/sstest${EXE}
# sstest linked like this is just for testing (sstest is really special)

include $(TOPDIR)/Rules.make.freevms
