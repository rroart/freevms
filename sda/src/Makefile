FLAGS = -static

EXTERNAL = ../../cdu/src/cdu.a
EXTERNAL2 = ../../cdu/src/cdu2.a
VMLINUX = ../../linux/vmlinux 
LINKVMLINUX = -Wl,--just-symbols=../../linux/vmlinux 

all: i386/sda${NOEXE} i386/sda${EXE_ELF}
# i386/sda${EXE} 

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms$$common/sysexe
	install -d ../../rooti386/vms$$common/sysexe
	install i386/sda ../../rooti386/vms"$$"common/sysexe

OBJS = sdamain.o sda_parse.o device.o misc.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/sda$(EXE): $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMLINUX)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(LIBGCC) $(EXTERNAL) $(EXTERNAL2) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKPOST) --just-symbols=$(VMLINUX) $(LIBGCC) -o i386/sda{EXE}

i386/sda${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMLINUX)
	install -d i386
	gcc -g -static $(OBJS) -lc $(EXTERNAL) $(EXTERNAL2) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LINKVMLINUX) -o i386/sda${NOEXE}

i386/sda${EXE_ELF}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMLINUX)
	ld -Bshareable $(OBJS) $(EXTERNAL) $(EXTERNAL2) $(VMSLIBS) /usr/lib/libelf.a /usr/lib/libdl.a $(LIBC) $(LIBGCC) --just-symbols=$(VMLINUX) -o i386/sda${EXE_ELF}

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

librtl$(EXE_ELF): $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)
	ln -s $(TOPDIR)/librtl/src/i386/librtl$(EXE_ELF)

sda_parse.o: sda_parse.c
	gcc -I$(TOPDIR)/cdu/src -c sda_parse.c

sda_parse.c: sda_parse.cld
	../../cdu/src/cdu sda_parse.cld

