# $Id$
# $Locker$

VMLINUX = ../../linux/vmlinux 
LINKVMLINUX = -Wl,--just-symbols=../../linux/vmlinux 

FLAGS = -static

all: set.a show.a i386/create${NOEXE} i386/create${EXE} i386/create${EXE_ELF} um/create${NOEXE} um/create${EXE} um/create${EXE_ELF} i386/type${NOEXE} i386/type${EXE} i386/type${EXE_ELF} um/type${NOEXE} um/type${EXE} um/type${EXE_ELF} i386/show${NOEXE}

clean:
	rm *.o *.a i386/* um/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install um/*.* ../../rootum/vms"$$"common/sysexe/
	install i386/*.* ../../rooti386/vms"$$"common/sysexe
	install i386/show ../../rooti386/vms"$$"common/sysexe

ifeq ($(ARCH),x86_64)
MODEL = -mcmodel=medium
else
MODEL =
endif

setproces: setproces.o
	gcc -o setproces setproces.o ../../starlet/src/vmsopt.a $(VMSLIBS)

set.a: setproces.o setwork.o
	ar r set.a setproces.o setwork.o

show.a: showsystem.o showdevice.o showwork.o showlog.o showmemory.o misc.o
	ar r show.a showsystem.o showdevice.o showwork.o showlog.o showmemory.o misc.o

showmemory.o: showmemory.c
	gcc $(MODEL) -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/%.o: %.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/create${NOEXE}: create.o $(VMSLIBS) $(EXTERNAL2)
	install -d i386
	gcc $(FLAGS) -g create.o $(VMSLIBS) $(EXTERNAL2) -o i386/create${NOEXE}

i386/create${EXE_ELF}: pic/create.o $(VMSLIBS_PIC) $(EXTERNAL2_PIC)
	ld -Bshareable pic/create.o $(VMSLIBS_PIC) $(EXTERNAL2_PIC) -lc $(LIBGCC) -o i386/create${EXE_ELF}

i386/create${EXE}: create.o $(VMSLIBS) $(EXTERNAL2)
	install -d i386
	$(LINKER) $(LINKPRE) create.o $(VMSLIBS) $(LIBGCC) $(EXTERNAL2) --start-group $(LINKPOST) $(LIBGCC) --end-group -o i386/create${EXE}

um/create${NOEXE}: create.o $(VMSLIBSUM) $(EXTERNAL2)
	install -d um
	gcc $(FLAGS) -g create.o $(VMSLIBSUM) $(EXTERNAL2) -o um/create${NOEXE}

um/create${EXE_ELF}: pic/create.o $(VMSLIBSUM_PIC) $(EXTERNAL2_PIC)
	ld -Bshareable pic/create.o $(VMSLIBSUM_PIC) $(EXTERNAL2_PIC) -lc $(LIBGCC) -o um/create${EXE_ELF}

um/create${EXE}: create.o $(VMSLIBSUM) $(EXTERNAL2)
	install -d um
	$(LINKER) $(LINKPRE) create.o $(VMSLIBSUM) $(LIBGCC) $(EXTERNAL2) --start-group $(LINKPOST) $(LIBGCC) --end-group -o um/create${EXE}

i386/type${NOEXE}: $(VMSLIBS) type.o $(EXTERNAL2)
	install -d i386
	gcc -g -static type.o $(VMSLIBS) $(EXTERNAL2) -o i386/type${NOEXE}

i386/type${EXE_ELF}: $(VMSLIBS_PIC) pic/type.o $(EXTERNAL2_PIC)
	ld -Bshareable pic/type.o $(VMSLIBS_PIC) $(EXTERNAL2_PIC) -lc $(LIBGCC) -o i386/type${EXE_ELF}

i386/type${EXE}: $(VMSLIBS) type.o $(EXTERNAL2)
	install -d i386
	$(LINKER) $(LINKPRE) type.o $(VMSLIBS) $(LIBGCC) $(EXTERNAL2) --start-group $(LINKPOST) $(LIBGCC) --end-group -o i386/type${EXE}

UMDEF=-Dcompile_um -I.

um/type${NOEXE}: $(VMSLIBSUM) type.o $(EXTERNAL2)
	install -d um
	gcc -g -static type.o $(VMSLIBSUM) $(EXTERNAL2) -o um/type${NOEXE}
um/type${EXE_ELF}: $(VMSLIBSUM) pic/type.o $(EXTERNAL2_PIC)
	ld -Bshareable pic/type.o $(VMSLIBSUM_PIC) $(EXTERNAL2_PIC) -lc $(LIBGCC) -o um/type${EXE_ELF}

um/type${EXE}: $(VMSLIBSUM) type.o
	install -d um
	$(LINKER) $(LINKPRE) type.o $(VMSLIBSUM) $(LIBGCC) $(EXTERNAL2) --start-group $(LINKPOST) $(LIBGCC) --end-group -o um/type${EXE}

i386/show${NOEXE}: show.a showmain.o $(VMLINUX)
	install -d i386
	gcc -g -static showmain.o show.a -lc $(VMSLIBS) $(EXTERNAL2) $(LINKVMLINUX) -o i386/show${NOEXE}
