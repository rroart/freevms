FLAGS = -static

EXTERNAL = ../../cdu/src/cdu.a
EXTERNAL2 = ../../cdu/src/cdu2.a
VMLINUX = ../../linux/vmlinux 
LINKVMLINUX = -Wl,--just-symbols=../../linux/vmlinux 

all: i386/install${NOEXE} i386/install${EXE_ELF}
# i386/install${EXE} 

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms$$common/sysexe
	install -d ../../rooti386/vms$$common/sysexe
	install i386/install ../../rooti386/vms"$$"common/sysexe

OBJS = instmain.o inst_parse.o instadd.o instmisc.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/install$(EXE): $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMLINUX)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(LIBGCC) $(EXTERNAL) $(EXTERNAL2) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKPOST) --just-symbols=$(VMLINUX) $(LIBGCC) -o i386/install{EXE}

i386/install${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMLINUX)
	install -d i386
	gcc -g -static $(OBJS) -lc $(EXTERNAL) $(EXTERNAL2) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKVMLINUX) -o i386/install${NOEXE}

i386/install${EXE_ELF}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMLINUX)
	ld -Bshareable $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LIBC) $(LIBGCC) --just-symbols=$(VMLINUX) -o i386/install${EXE_ELF}

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

librtl$(EXE_ELF): $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)
	ln -s $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)

inst_parse.o: inst_parse.c
	gcc -I$(TOPDIR)/cdu/src -c inst_parse.c

inst_parse.c: inst_parse.cld
	../../cdu/src/cdu inst_parse.cld

