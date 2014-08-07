FLAGS = -static

VMLINUX = ../../linux/vmlinux 
LINKVMLINUX = -Wl,--just-symbols=../../linux/vmlinux 

ifneq ($(ARCH),x86_64)
all: i386/authorize${NOEXE} uafgen i386/sysuaf.dat
#i386/authorize${EXE_ELF} 
else
all: i386/authorize${NOEXE} uafgen i386/sysuaf.dat
endif
# i386/authorize${EXE} 

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/authorize ../../rooti386/vms"$$"common/sysexe
	install i386/sysuaf.dat ../../rooti386/vms"$$"common/sysexe

ifeq ($(ARCH),x86_64)
MODEL = -mcmodel=medium
MODELGCC = gcc-3.4
else
MODEL =
MODELGCC = gcc
endif

OBJS = authmain.o auth_parse.o authshow.o authmisc.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/%.o: %.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/authorize$(EXE): $(VMSLIBS) $(OBJS) $(EXTERNAL) $(VMLINUX)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(LIBGCC) $(EXTERNAL) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKPOST) --just-symbols=$(VMLINUX) $(LIBGCC) -o i386/authorize{EXE}

i386/authorize${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(VMLINUX)
	install -d i386
	gcc -g -static $(OBJS) -lc $(EXTERNAL) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKVMLINUX) -o i386/authorize${NOEXE}

ifneq ($(ARCH),x86_64)
i386/authorize${EXE_ELF}: $(VMSLIBS_PIC) $(OBJS_PIC) $(EXTERNAL_PIC) $(VMLINUX)
	ld -Bshareable $(OBJS_PIC) $(EXTERNAL_PIC) $(VMSLIBS_PIC) /usr/lib/libelf.a /usr/lib/libdl.a $(LIBC) $(LIBGCC) --just-symbols=$(VMLINUX) -o i386/authorize${EXE_ELF}
endif

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

librtl$(EXE_ELF): $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)
	ln -s $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)

auth_parse.o: auth_parse.c
	gcc -I$(TOPDIR)/cdu/src -c auth_parse.c

pic/auth_parse.o: auth_parse.c
	gcc -I$(TOPDIR)/cdu/src -c auth_parse.c -o pic/auth_parse.o

auth_parse.c: auth_parse.cld
	../../cdu/src/cdu auth_parse.cld

uafgen: uafgen.o sysuaf.dat
	gcc uafgen.o -o uafgen

i386/sysuaf.dat: uafgen sysuaf.dat
	install -d i386
	./uafgen
