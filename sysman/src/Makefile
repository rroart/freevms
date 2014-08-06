FLAGS = -static

VMLINUX = ../../linux/vmlinux 
LINKVMLINUX = -Wl,--just-symbols=../../linux/vmlinux 


ifneq ($(ARCH),x86_64)
all: i386/sysman${NOEXE} i386/sysman${EXE_ELF}
else
all: i386/sysman${NOEXE}
endif
# i386/sysman${EXE} 

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/sysman ../../rooti386/vms"$$"common/sysexe

ifeq ($(ARCH),x86_64)
MODEL = -mcmodel=medium
MODELGCC = gcc
else
MODEL =
MODELGCC = gcc
endif

OBJS = sm_load.o sm_main.o sysman_parse.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ -I../../sys/src/ -I../../linux/include -I. $< -o $@

pic/%.o: %.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ -I../../sys/src/ -I../../linux/include -I. $< -o $@

device.o: device.c
	$(MODELGCC) $(MODEL) -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/device.o: device.c
	install -d pic
	$(MODELGCC) $(MODEL) -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/sysman_parse.o: sysman_parse.c
	install -d pic
	$(MODELGCC) $(MODEL) -fPIC -fpic -g -c -I$(TOPDIR)/cdu/src -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

sda_spin.o: sda_spin.c
	$(MODELGCC) $(MODEL) -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ -I../../sys/src/ $< -o $@

pic/sda_spin.o: sda_spin.c
	install -d pic
	$(MODELGCC) $(MODEL) -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ -I../../sys/src/ $< -o $@

i386/sysman$(EXE): $(VMSLIBS) $(OBJS) $(EXTERNAL) $(VMLINUX)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(LIBGCC) $(EXTERNAL) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKPOST) --just-symbols=$(VMLINUX) $(LIBGCC) -o i386/sysman{EXE}

i386/sysman${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(VMLINUX)
	install -d i386
	gcc -g -static $(OBJS) -lc $(EXTERNAL) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKVMLINUX) -o i386/sysman${NOEXE}

ifneq ($(ARCH),x86_64)
i386/sysman${EXE_ELF}: $(VMSLIBS_PIC) $(OBJS_PIC) $(EXTERNAL_PIC) $(VMLINUX)
	ld -Bshareable $(OBJS_PIC) $(EXTERNAL_PIC) $(VMSLIBS_PIC) /usr/lib/libelf.a /usr/lib/libdl.a $(LIBC) $(LIBGCC) --just-symbols=$(VMLINUX) -o i386/sysman${EXE_ELF}
endif

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

librtl$(EXE_ELF): $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)
	ln -s $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)

sysman_parse.o: sysman_parse.c
	gcc -I$(TOPDIR)/cdu/src -c sysman_parse.c

sysman_parse.c: sysman_parse.cld
	../../cdu/src/cdu sysman_parse.cld
