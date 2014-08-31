FLAGS = -static

all: i386/directory${NOEXE}
#i386/directory${EXE} 
#i386/ddirectory${EXE} i386/ddirectory${NOEXE}
#i386/ddirectory${EXE_ELF} i386/directory${EXE_ELF}

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe
#	install i386/directory ../../rooti386/vms"$$"common/sysexe

OBJS = directory.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

%.o: %.c
	gcc -g -c ${OLDINCLUDE} -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/%.o: %.c 
	install -d pic
	gcc -fpic -fPIC -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/directory${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL)
	install -d i386
	gcc -g -static $(OBJS) -L$(OLDLIBCDIR) $(VMSLIBS) $(EXTERNAL) -o i386/directory${NOEXE}

i386/directory${EXE_ELF}: $(VMSLIBS_PIC) $(OBJS_PIC) $(EXTERNAL_PIC)
	ld -Bshareable $(OBJS_PIC) $(VMSLIBS_PIC) $(EXTERNAL_PIC) -lc $(LIBGCC) -o i386/directory${EXE_ELF}

i386/directory${EXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) $(LIBGCC) $(EXTERNAL) --start-group $(LINKPOST) $(LIBGCC) --end-group -o i386/directory${EXE}

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

librtl$(EXE_ELF): $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)
	ln -s $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)

ifeq ($(ARCH),x86_64)
i386/ddirectory${NOEXE}:
	touch i386/ddirectory${NOEXE}
else
i386/ddirectory${NOEXE}: $(VMSLIBS_EXE_ELF) $(OBJS) $(EXTERNAL) starlet$(EXE_ELF) librtl$(EXE_ELF)
	install -d i386
	gcc -g $(OBJS) starlet$(EXE_ELF) librtl$(EXE_ELF) $(EXTERNAL) -o i386/ddirectory${NOEXE}
endif

ifeq ($(ARCH),x86_64)
i386/ddirectory${EXE_ELF}:
	touch i386/ddirectory${EXE_ELF}
else
i386/ddirectory${EXE_ELF}: $(VMSLIBS_EXE_ELF) $(OBJS) $(EXTERNAL) starlet$(EXE_ELF) librtl$(EXE_ELF)
	ld -Bshareable $(VMS_LD_RPATH) $(OBJS) starlet$(EXE_ELF) librtl$(EXE_ELF) $(EXTERNAL) -lc $(LIBGCC) -o i386/ddirectory${EXE_ELF}
endif

ifeq ($(ARCH),x86_64)
i386/ddirectory${EXE}:
	touch i386/ddirectory${EXE}
else
i386/ddirectory${EXE}: $(VMSDLIBS_EXE) $(OBJS) $(EXTERNAL)
	install -d i386
	$(LINKER) $(OBJS) $(TOPDIR)/starlet/src/i386/starlet.exe $(TOPDIR)/librtl/src/i386/librtl.exe $(LIBGCC) $(EXTERNAL) /usr/lib/crt1.o /usr/lib/crti.o $(LIBGCC) -o i386/ddirectory${EXE}
endif
