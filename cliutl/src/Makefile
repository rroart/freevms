# $Id$
# $Locker$

EXTERNAL2 = ../../cdu/src/cdu2.a

FLAGS = -static

all: set.a show.a i386/create${NOEXE} i386/create${EXE} i386/create${EXE_ELF} um/create${NOEXE} um/create${EXE} um/create${EXE_ELF} i386/type${NOEXE} i386/type${EXE} i386/type${EXE_ELF} um/type${NOEXE} um/type${EXE} um/type${EXE_ELF}

clean:
	rm *.o *.a i386/* um/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install um/*.* ../../rootum/vms"$$"common/sysexe/
	install i386/*.* ../../rooti386/vms"$$"common/sysexe

setproces: setproces.o
	gcc -o setproces setproces.o ../../starlet/src/vmsopt.a $(VMSLIBS)

set.a: setproces.o setwork.o
	ar r set.a setproces.o setwork.o

show.a: showsystem.o showdevice.o showwork.o showlog.o
	ar r show.a showsystem.o showdevice.o showwork.o showlog.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/create${NOEXE}: create.o $(VMSLIBS) $(EXTERNAL2)
	install -d i386
	gcc $(FLAGS) -g create.o $(VMSLIBS) $(EXTERNAL2) -o i386/create${NOEXE}

i386/create${EXE_ELF}: create.o $(VMSLIBS) $(EXTERNAL2)
	ld -Bshareable create.o $(VMSLIBS) $(EXTERNAL2) $(LIBC) $(LIBGCC) -o i386/create${EXE_ELF}

i386/create${EXE}: create.o $(VMSLIBS) $(EXTERNAL2)
	install -d i386
	$(LINKER) $(LINKPRE) create.o $(VMSLIBS) $(LIBGCC) $(EXTERNAL2) $(LINKPOST) $(LIBGCC) -o i386/create${EXE}

um/create${NOEXE}: create.o $(VMSLIBSUM) $(EXTERNAL2)
	install -d um
	gcc $(FLAGS) -g create.o $(VMSLIBSUM) $(EXTERNAL2) -o um/create${NOEXE}

um/create${EXE_ELF}: create.o $(VMSLIBSUM) $(EXTERNAL2)
	ld -Bshareable create.o $(VMSLIBSUM) $(EXTERNAL2) $(LIBC) $(LIBGCC) -o um/create${EXE_ELF}

um/create${EXE}: create.o $(VMSLIBSUM) $(EXTERNAL2)
	install -d um
	$(LINKER) $(LINKPRE) create.o $(VMSLIBSUM) $(LIBGCC) $(EXTERNAL2) $(LINKPOST) $(LIBGCC) -o um/create${EXE}

i386/type${NOEXE}: $(VMSLIBS) type.o $(EXTERNAL2)
	install -d i386
	gcc -g -static type.o $(VMSLIBS) $(EXTERNAL2) -o i386/type${NOEXE}

i386/type${EXE_ELF}: $(VMSLIBS) type.o $(EXTERNAL2)
	ld -Bshareable type.o $(VMSLIBS) $(EXTERNAL2) $(LIBC) $(LIBGCC) -o i386/type${EXE_ELF}

i386/type${EXE}: $(VMSLIBS) type.o $(EXTERNAL2)
	install -d i386
	$(LINKER) $(LINKPRE) type.o $(VMSLIBS) $(LIBGCC) $(EXTERNAL2) $(LINKPOST) $(LIBGCC) -o i386/type${EXE}

UMDEF=-Dcompile_um -I.

um/type${NOEXE}: $(VMSLIBSUM) type.o $(EXTERNAL2)
	install -d um
	gcc -g -static type.o $(VMSLIBSUM) $(EXTERNAL2) -o um/type${NOEXE}
um/type${EXE_ELF}: $(VMSLIBSUM) type.o $(EXTERNAL2)
	ld -Bshareable type.o $(VMSLIBSUM) $(EXTERNAL2) $(LIBC) $(LIBGCC) -o um/type${EXE_ELF}

um/type${EXE}: $(VMSLIBSUM) type.o
	install -d um
	$(LINKER) $(LINKPRE) type.o $(VMSLIBSUM) $(LIBGCC) $(EXTERNAL2) $(LINKPOST) $(LIBGCC) -o um/type${EXE}
