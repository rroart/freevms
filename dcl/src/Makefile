EXE =
# EXE = .exe

LIB = ../../starlet/src/starlet.a
LIBUM = ../../starlet/src/starletum.a

EXTERNAL = ../../cliutl/src/show.a ../../cliutl/src/set.a

all: i386/dcl${EXE} um/dcl${EXE}

install: all
	[ ! -d ../../rootum/vms$$common.sysexe ] && mkdir -p ../../rootum/vms"$$"common/sysexe
	[ ! -d ../../rooti386/vms$$common.sysexe ] && mkdir -p ../../rooti386/vms"$$"common/sysexe
	cp -p um/* ../../rootum/vms"$$"common/sysexe
	cp -p i386/* ../../rooti386/vms"$$"common/sysexe


dcl.o: cli.c $(LIB) $(EXTERNAL)
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ cli.c -o dcl.o

i386/dcl${EXE}: dcl.o $(LIB) $(EXTERNAL)
	mkdir i386 || echo i386 exists
	gcc -s -static dcl.o $(EXTERNAL) ../../starlet/src/starlet.a ../../librtl/src/starlet.a -ldl -o i386/dcl${EXE}
	gcc -static dcl.o $(EXTERNAL) ../../starlet/src/starlet.a ../../librtl/src/starlet.a -ldl -o i386/dcld${EXE}
#	../../linker/src/linker $(LINKPRE) dcl.o $(EXTERNAL) ../../starlet/src/starlet.a /usr/lib/libdl.a $(LINKPOST) -o i386/dcl.exe
# DCL linked like this is just for testing (DCL is really special)

UMDEF=-Dcompile_um -I.

um/dcl${EXE}: dcl.o $(LIBUM) $(EXTERNAL)
	mkdir um || echo um exists
	gcc -s -static dcl.o $(EXTERNAL) ../../starlet/src/starletum.a ../../librtl/src/starlet.a -ldl -o um/dcl${EXE}
	gcc -static dcl.o $(EXTERNAL) ../../starlet/src/starletum.a ../../librtl/src/starlet.a -ldl -o um/dcld${EXE}
