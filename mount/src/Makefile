O_TARGET := mount.o

obj-y := mountdsp.o

include $(TOPDIR)/Rules.make

FLAGS = -static

all: i386/vmount${NOEXE}
#i386/vmount${EXE}
#i386/vmount${EXE_ELF}

clean:
	rm *.o *.a i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe

vmount.o: vmount.c
	gcc -g -c $(CFLAGS) -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/vmount.o: vmount.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/vmount${NOEXE}: vmount.o $(VMSLIBS) $(EXTERNAL)
	install -d i386
	gcc $(FLAGS) -g vmount.o -L$(OLDLIBCDIR) $(VMSLIBS) $(EXTERNAL) -o i386/vmount${NOEXE}

i386/vmount${EXE_ELF}: pic/vmount.o $(VMSLIBS_PIC) $(EXTERNAL_PIC)
	ld -Bshareable pic/vmount.o $(VMSLIBS_PIC) $(EXTERNAL_PIC) -lc $(LIBGCC) -o i386/vmount${EXE_ELF}

i386/vmount${EXE}: vmount.o $(VMSLIBS) $(EXTERNAL)
	install -d i386
	$(LINKER) $(LINKPRE) vmount.o $(VMSLIBS) $(LIBGCC) $(EXTERNAL) --start-group $(LINKPOST) $(LIBGCC) --end-group -o i386/vmount${EXE}
