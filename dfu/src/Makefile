EXE = .exe
NOEXE = 

export LIBGCC = `gcc -print-libgcc-file-name`

export LIBC = /usr/lib/libc.a

LIB = ../../starlet/src/starlet.a
LIBUM = ../../starlet/src/starletum.a

FLAGS = -static

all: i386/dfu${EXE} i386/dfu${NOEXE} um/dfu${EXE} um/dfu${NOEXE}

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms$$common.sysexe
	install -d ../../rooti386/vms$$common.sysexe
	install um/* ../../rootum/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/dfu${NOEXE}: dfu.o $(LIB)
	install -d i386
	gcc -s -static dfu.o ../../starlet/src/starlet.a -o i386/dfu${NOEXE}
i386/dfu${EXE}: dfu.o $(LIB)
	../../linker/src/linker $(LINKPRE) dfu.o ../../starlet/src/starlet.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o dfu${EXE}

UMDEF=-Dcompile_um -I.

um/dfu${NOEXE}: dfu.c $(LIBUM)
	install -d um
	gcc $(FLAGS) -g $(UMDEF) -I../../starlet/src -I../../librtl/src/ -I../../lib/src dfu.c ../../starlet/src/starletum.a -o dfuum${NOEXE}

um/dfu${EXE}:
