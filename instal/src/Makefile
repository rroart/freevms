FLAGS = -static

VMLINUX = ../../linux/vmlinux 
LINKVMLINUX = -Wl,--just-symbols=../../linux/vmlinux 

ifneq ($(ARCH),x86_64)
all: i386/install${NOEXE} i386/install${EXE_ELF}
else
all: i386/install${NOEXE}
endif

# i386/install${EXE} 

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/install ../../rooti386/vms"$$"common/sysexe

ifeq ($(ARCH),x86_64)
MODEL = -mcmodel=medium
MODELGCC = gcc-3.4
else
MODEL =
MODELGCC = gcc
endif

OBJS = instmain.o inst_parse.o instadd.o instmisc.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/%.o: %.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

instadd.o: instadd.c
	$(MODELGCC) $(MODEL) -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

instmisc.o: instmisc.c
	$(MODELGCC) $(MODEL) -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/instadd.o: instadd.c
	install -d pic
	$(MODELGCC) $(MODEL) -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/inst_parse.o: inst_parse.c
	install -d pic
	$(MODELGCC) $(MODEL) -fPIC -fpic -g -c -I$(TOPDIR)/cdu/src -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/install$(EXE): $(VMSLIBS) $(OBJS) $(EXTERNAL) $(VMLINUX)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(LIBGCC) $(EXTERNAL) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKPOST) --just-symbols=$(VMLINUX) $(LIBGCC) -o i386/install{EXE}

i386/install${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(VMLINUX)
	install -d i386
	gcc -g -static $(OBJS) -lc $(EXTERNAL) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKVMLINUX) -o i386/install${NOEXE}

ifneq ($(ARCH),x86_64)
i386/install${EXE_ELF}: $(VMSLIBS_PIC) $(OBJS_PIC) $(EXTERNAL_PIC) $(EXTERNAL_PIC) $(VMLINUX)
	ld -Bshareable $(OBJS_PIC) $(EXTERNAL_PIC) $(EXTERNAL_PIC) $(VMSLIBS_PIC) /usr/lib/libelf.a /usr/lib/libdl.a $(LIBC) $(LIBGCC) --just-symbols=$(VMLINUX) -o i386/install${EXE_ELF}
endif

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

librtl$(EXE_ELF): $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)
	ln -s $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)

inst_parse.o: inst_parse.c
	gcc -I$(TOPDIR)/cdu/src -c inst_parse.c

inst_parse.c: inst_parse.cld
	../../cdu/src/cdu inst_parse.cld

