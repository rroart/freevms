EXTERNAL = ../../cliutl/src/show.a ../../cliutl/src/set.a ../../cdu/src/cdu.a ../../cdu/src/cdu2.a ../../cdu/src/dcltables.o

#all: i386/dcl${NOEXE} i386/dcl${EXE} um/dcl${NOEXE} um/dcl${EXE}
all: i386/dcl${NOEXE} um/dcl${NOEXE}

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install um/* ../../rootum/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe

dcl.o: cli.c $(VMSLIBS) $(EXTERNAL)
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ cli.c -o dcl.o

i386/dcl${NOEXE}: dcl.o $(VMSLIBS) $(EXTERNAL)
	install -d i386
	gcc -s -static dcl.o $(EXTERNAL) $(VMSLIBS) -ldl -o i386/dcl${NOEXE}
	gcc -static dcl.o $(EXTERNAL) $(VMSLIBS) -ldl -o i386/dcld${NOEXE}

i386/dcl${EXE}: dcl.o $(VMSLIBS) $(EXTERNAL)
	$(LINKER) $(LINKPRE) dcl.o $(EXTERNAL) $(VMSLIBS) /usr/lib/libdl.a /usr/lib/libiberty.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/dcl${EXE}
# DCL linked like this is just for testing (DCL is really special)

um/dcl${NOEXE}: dcl.o $(VMSLIBSUM) $(EXTERNAL)
	install -d um
	gcc -s -static dcl.o $(EXTERNAL) $(VMSLIBSUM) -ldl -o um/dcl${NOEXE}
	gcc -static dcl.o $(EXTERNAL) $(VMSLIBSUM) -ldl -o um/dcld${NOEXE}

um/dcl${EXE}: dcl.o $(VMSLIBSUM) $(EXTERNAL)
	$(LINKER) $(LINKPRE) dcl.o $(EXTERNAL) $(VMSLIBSUM) /usr/lib/libdl.a /usr/lib/libiberty.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o um/dcl${EXE}
# DCL linked like this is just for testing (DCL is really special)

