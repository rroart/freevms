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

i386/dfu${NOEXE}: $(VMSLIBS) $(OBJS)
	install -d i386
	gcc -g -static $(OBJS) $(VMSLIBS) -o i386/dfu${NOEXE}

i386/dfu${EXE}: $(VMSLIBS) $(OBJS) 
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/dfu${EXE}

um/dfu${NOEXE}: dfu.c $(VMSLIBSUM) $(OBJS)
	install -d um
	gcc $(FLAGS) -g $(OBJS) $(VMSLIBSUM) -o um/dfu${NOEXE}

um/dfu${EXE}:
	install -d um
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBSUM) $(LIBGCC) $(LINKPOST) $(LIBGCC) -o um/dfu${EXE}
