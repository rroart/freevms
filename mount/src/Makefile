O_TARGET := mount.o

obj-y := mountdsp.o

include $(TOPDIR)/Rules.make

EXTERNAL2 = ../../cdu/src/cdu2.a

FLAGS = -static

all: i386/vmount${NOEXE} i386/vmount${EXE} i386/vmount${EXE_ELF} um/vmount${NOEXE} um/vmount${EXE} um/vmount${EXE_ELF}

clean:
	rm *.o *.a i386/* um/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install um/* ../../rootum/vms"$$"common/sysexe/
	install i386/* ../../rooti386/vms"$$"common/sysexe

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/vmount${NOEXE}: vmount.o $(VMSLIBS) $(EXTERNAL2)
	install -d i386
	gcc $(FLAGS) -g vmount.o $(VMSLIBS) $(EXTERNAL2) -o i386/vmount${NOEXE}

i386/vmount${EXE_ELF}: vmount.o $(VMSLIBS) $(EXTERNAL2)
	ld -Bshareable vmount.o $(VMSLIBS) $(EXTERNAL2) $(LIBC) $(LIBGCC) -o i386/vmount${EXE_ELF}

i386/vmount${EXE}: vmount.o $(VMSLIBS) $(EXTERNAL2)
	install -d i386
	$(LINKER) $(LINKPRE) vmount.o $(VMSLIBS) $(LIBGCC) $(EXTERNAL2) $(LINKPOST) $(LIBGCC) -o i386/vmount${EXE}

um/vmount${NOEXE}: vmount.o $(VMSLIBSUM) $(EXTERNAL2)
	install -d um
	gcc $(FLAGS) -g vmount.o $(VMSLIBSUM) $(EXTERNAL2) -o um/vmount${NOEXE}

um/vmount${EXE_ELF}: vmount.o $(VMSLIBSUM) $(EXTERNAL2)
	ld -Bshareable vmount.o $(VMSLIBSUM) $(EXTERNAL2) $(LIBC) $(LIBGCC) -o um/vmount${EXE_ELF}

um/vmount${EXE}: vmount.o $(VMSLIBSUM) $(EXTERNAL2)
	install -d um
	$(LINKER) $(LINKPRE) vmount.o $(VMSLIBSUM) $(LIBGCC) $(EXTERNAL2) $(LINKPOST) $(LIBGCC) -o um/vmount${EXE}
