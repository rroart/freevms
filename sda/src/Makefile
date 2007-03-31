FLAGS = -static

VMLINUX = ../../linux/vmlinux 
LINKVMLINUX = -Wl,--just-symbols=../../linux/vmlinux 


ifneq ($(ARCH),x86_64)
all: i386/sda${NOEXE} i386/sda${EXE_ELF}
else
all: i386/sda${NOEXE}
endif
# i386/sda${EXE} 

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/sda ../../rooti386/vms"$$"common/sysexe

ifeq ($(ARCH),x86_64)
MODEL = -mcmodel=medium
else
MODEL =
endif

OBJS = sdamain.o sda_parse.o device.o misc.o sda_lock.o sda_dump.o sda_exam.o \
	sda_proc.o sda_spin.o sda_addr.o System_map.o sdamap.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ -I../../sys/src/ -I../../linux/include -I. $< -o $@

pic/%.o: %.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ -I../../sys/src/ -I../../linux/include -I. $< -o $@

device.o: device.c
	gcc $(MODEL) -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/device.o: device.c
	install -d pic
	gcc $(MODEL) -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/sda_parse.o: sda_parse.c
	install -d pic
	gcc $(MODEL) -fPIC -fpic -g -c -I$(TOPDIR)/cdu/src -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/sda$(EXE): $(VMSLIBS) $(OBJS) $(EXTERNAL) $(VMLINUX)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(LIBGCC) $(EXTERNAL) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKPOST) --just-symbols=$(VMLINUX) $(LIBGCC) -o i386/sda{EXE}

i386/sda${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(VMLINUX)
	install -d i386
	gcc -g -static $(OBJS) -lc $(EXTERNAL) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKVMLINUX) -o i386/sda${NOEXE}

ifneq ($(ARCH),x86_64)
i386/sda${EXE_ELF}: $(VMSLIBS_PIC) $(OBJS_PIC) $(EXTERNAL_PIC) $(VMLINUX)
	ld -Bshareable $(OBJS_PIC) $(EXTERNAL_PIC) $(VMSLIBS_PIC) /usr/lib/libelf.a /usr/lib/libdl.a $(LIBC) $(LIBGCC) --just-symbols=$(VMLINUX) -o i386/sda${EXE_ELF}
endif

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

librtl$(EXE_ELF): $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)
	ln -s $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)

sda_parse.o: sda_parse.c
	gcc -I$(TOPDIR)/cdu/src -c sda_parse.c

sda_parse.c: sda_parse.cld
	../../cdu/src/cdu sda_parse.cld

System_map.c: ../../linux/vmlinux make_map.sh
	./make_map.sh