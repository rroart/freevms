EXE = .exe
NOEXE = 

LIB = ../../starlet/src/starlet.a ../../librtl/src/starlet.a
LIBUM = ../../starlet/src/starletum.a ../../librtl/src/starlet.a

FLAGS = -static

all: i386/dfu${EXE} i386/dfu${NOEXE} um/dfu${EXE} um/dfu${NOEXE}

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms$$common.sysexe
	install -d ../../rooti386/vms$$common.sysexe
	install um/* ../../rootum/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe

OBJS = dfu.o dfu_bitmap.o dfu_filedir.o dfu_ifscan.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/dfu${NOEXE}: $(LIB) $(OBJS)
	install -d i386
	gcc -g -static $(OBJS) $(LIB) -o i386/dfu${NOEXE}

i386/dfu${EXE}: $(LIB) $(OBJS) 
	install -d i386
	../../linker/src/linker $(LINKPRE) $(OBJS) $(LIB) $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/dfu${EXE}

UMDEF=-Dcompile_um -I.

um/dfu${NOEXE}: dfu.c $(LIBUM) $(OBJS)
	install -d um
	gcc $(FLAGS) -g $(UMDEF) $(OBJS) $(LIBUM) -o um/dfu${NOEXE}

um/dfu${EXE}:
