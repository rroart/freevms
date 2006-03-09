FLAGS = -static

all: i386/dfu${NOEXE} um/dfu${NOEXE}
# i386/dfu${EXE} um/dfu${EXE} 

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install um/* ../../rootum/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe

OBJS = dfu.o dfu_bitmap.o dfu_filedir.o dfu_ifscan.o dfu_tables.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ -I../../cdu/src $< -o $@

dfu_tables.c: ../../cdu/src/cdu dfu_tables.cld
	../../cdu/src/cdu dfu_tables.cld

i386/dfu${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2)
	install -d i386
	gcc -g -static $(OBJS) $(VMSLIBS) $(EXTERNAL) $(EXTERNAL2) -ldl -lelf -o i386/dfu${NOEXE}

i386/dfu${EXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(EXTERNAL2) 
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) $(EXTERNAL) $(EXTERNAL2) /usr/lib/libdl.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/dfu${EXE}

um/dfu${NOEXE}: $(VMSLIBSUM) $(OBJS) $(EXTERNAL) $(EXTERNAL2)
	install -d um
	gcc $(FLAGS) -g $(OBJS) $(VMSLIBSUM) $(EXTERNAL) $(EXTERNAL2) -ldl -lelf -o um/dfu${NOEXE}

um/dfu${EXE}: $(VMSLIBSUM) $(OBJS) $(EXTERNAL) $(EXTERNAL2) 
	install -d um
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBSUM) $(EXTERNAL) $(EXTERNAL2) /usr/lib/libdl.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o um/dfu${EXE}
