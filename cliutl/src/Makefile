# $Id$
# $Locker$

EXE =
# EXE = .exe

LIB = ../../starlet/src/starlet.a ../../librtl/src/starlet.a
LIBUM = ../../starlet/src/starletum.a ../../librtl/src/starlet.a

FLAGS = -static

all: set.a show.a create${EXE} createum${EXE}

install: all
	[ ! -d ../../rootum/vms$$common.sysexe ] && mkdir -p ../../rootum/vms"$$"common/sysexe
	[ ! -d ../../rooti386/vms$$common.sysexe ] && mkdir -p ../../rooti386/vms"$$"common/sysexe
	cp -p createum${EXE} ../../rootum/vms"$$"common/sysexe/create${EXE}
	cp -p create${EXE} ../../rooti386/vms"$$"common/sysexe/create${EXE}

setproces: setproces.o
	gcc -o setproces setproces.o ../../starlet/src/vmsopt.a ../../starlet/src/starlet.a

set.a: setproces.o setwork.o
	ar r set.a setproces.o setwork.o

show.a: showsystem.o showdevice.o showwork.o
	ar r show.a showsystem.o showdevice.o showwork.o

setproces.o: setproces.c
	gcc -I../../starlet/src -I../../librtl/src/ -c setproces.c

setwork.o: setwork.c
	gcc -I../../starlet/src -I../../librtl/src/ -c setwork.c

showsystem.o: showsystem.c
	gcc -I../../starlet/src -I../../librtl/src/ -c showsystem.c

showdevice.o: showdevice.c
	gcc -I../../starlet/src -I../../librtl/src/ -c showdevice.c

showwork.o: showwork.c
	gcc -I../../starlet/src -I../../librtl/src/ -c showwork.c

create${EXE}: create.c $(LIB)
	gcc $(FLAGS) -g -I../../starlet/src -I../../librtl/src/ -I../../lib/src create.c ../../librtl/src/starlet.a ../../starlet/src/starlet.a -o create${EXE}

UMDEF=-Dcompile_um -I.

createum${EXE}: create.c $(LIBUM)
	gcc $(FLAGS) -g $(UMDEF) -I../../starlet/src -I../../librtl/src/ -I../../lib/src create.c ../../librtl/src/starlet.a ../../starlet/src/starletum.a -o createum${EXE}
