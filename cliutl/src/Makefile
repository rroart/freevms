# $Id$
# $Locker$

FLAGS = -static

all: set.a show.a i386/create${NOEXE} i386/create${EXE} um/create${NOEXE} um/create${EXE}

clean:
	rm *.o *.a i386/* um/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install um/* ../../rootum/vms"$$"common/sysexe/
	install i386/* ../../rooti386/vms"$$"common/sysexe

setproces: setproces.o
	gcc -o setproces setproces.o ../../starlet/src/vmsopt.a $(VMSLIBS)

set.a: setproces.o setwork.o
	ar r set.a setproces.o setwork.o

show.a: showsystem.o showdevice.o showwork.o showlog.o
	ar r show.a showsystem.o showdevice.o showwork.o showlog.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/create${NOEXE}: create.o $(VMSLIBS)
	install -d i386
	gcc $(FLAGS) -g create.o $(VMSLIBS) -o i386/create${NOEXE}

i386/create${EXE}: create.o $(VMSLIBS)
	install -d i386
	$(LINKER) $(LINKPRE) create.o $(VMSLIBS) $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/dfu${EXE}

um/create${NOEXE}: create.o $(VMSLIBSUM)
	install -d um
	gcc $(FLAGS) -g create.o $(VMSLIBSUM) -o um/create${NOEXE}

um/create${EXE}: create.o $(VMSLIBSUM)
	install -d um
	$(LINKER) $(LINKPRE) create.o $(VMSLIBSUM) $(LIBGCC) $(LINKPOST) $(LIBGCC) -o um/dfu${EXE}
