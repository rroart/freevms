FLAGS = -static

EXTERNAL = ../../cdu/src/cdu.a
EXTERNAL2 = ../../cdu/src/cdu2.a
VMLINUX = ../../linux/vmlinux 
LINKVMLINUX = -Wl,--just-symbols=../../linux/vmlinux 

all: i386/authorize${NOEXE} i386/authorize${EXE_ELF} uafgen
# i386/authorize${EXE} 

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms$$common/sysexe
	install -d ../../rooti386/vms$$common/sysexe
	install i386/authorize ../../rooti386/vms"$$"common/sysexe
	install i386/sysuaf.dat ../../rooti386/vms"$$"common/sysexe

OBJS = authmain.o auth_parse.o authshow.o authmisc.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/authorize$(EXE): $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMLINUX)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(LIBGCC) $(EXTERNAL) $(EXTERNAL2) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKPOST) --just-symbols=$(VMLINUX) $(LIBGCC) -o i386/authorize{EXE}

i386/authorize${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMLINUX)
	install -d i386
	gcc -g -static $(OBJS) -lc $(EXTERNAL) $(EXTERNAL2) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKVMLINUX) -o i386/authorize${NOEXE}

i386/authorize${EXE_ELF}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMLINUX)
	ld -Bshareable $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LIBC) $(LIBGCC) --just-symbols=$(VMLINUX) -o i386/authorize${EXE_ELF}

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

librtl$(EXE_ELF): $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)
	ln -s $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)

auth_parse.o: auth_parse.c
	gcc -I$(TOPDIR)/cdu/src -c auth_parse.c

auth_parse.c: auth_parse.cld
	../../cdu/src/cdu auth_parse.cld

uafgen: uafgen.o sysuaf.dat
	gcc uafgen.o -o uafgen

i386/sysuaf.dat: uafgen sysuaf.dat
	install -d i386
	./uafgen
