# $Id$
# $Locker$

VMLINUX = ../../linux/vmlinux 
LINKVMLINUX = -Wl,--just-symbols=../../linux/vmlinux 

FLAGS = -static

all: set.a show.a i386/create${NOEXE} i386/create${EXE} i386/type${NOEXE} i386/type${EXE} i386/show${NOEXE} i386/set${NOEXE}
#i386/create${EXE_ELF} i386/type${EXE_ELF} 

clean:
	rm *.o *.a i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/*.* ../../rooti386/vms"$$"common/sysexe
	install i386/show ../../rooti386/vms"$$"common/sysexe
	install i386/set ../../rooti386/vms"$$"common/sysexe

ifeq ($(ARCH),x86_64)
MODEL = -mcmodel=large
MODELGCC = gcc
else
MODEL =
MODELGCC = gcc
endif

setproces: setproces.o
	gcc -o setproces setproces.o ../../starlet/src/vmsopt.a $(VMSLIBS)

set.a: setproces.o setwork.o setsec.o
	ar r set.a setproces.o setwork.o setsec.o

show.a: showsystem.o showdevice.o showwork.o showlog.o showmemory.o misc.o
	ar r show.a showsystem.o showdevice.o showwork.o showlog.o showmemory.o misc.o

showmemory.o: showmemory.c
	$(MODELGCC) $(MODEL) -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

showsystem.o: showsystem.c ../../linux/include/linux/version.h
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/%.o: %.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/create${NOEXE}: create.o $(VMSLIBS) $(EXTERNAL)
	install -d i386
	gcc $(FLAGS) -g create.o $(VMSLIBS) $(EXTERNAL) -o i386/create${NOEXE}

i386/create${EXE_ELF}: pic/create.o $(VMSLIBS_PIC) $(EXTERNAL_PIC)
	ld -Bshareable pic/create.o $(VMSLIBS_PIC) $(EXTERNAL_PIC) -lc $(LIBGCC) -o i386/create${EXE_ELF}

i386/create${EXE}: create.o $(VMSLIBS) $(EXTERNAL)
	install -d i386
	$(LINKER) $(LINKPRE) create.o $(VMSLIBS) $(LIBGCC) $(EXTERNAL) --start-group $(LINKPOST) $(LIBGCC) --end-group -o i386/create${EXE}

i386/type${NOEXE}: $(VMSLIBS) type.o $(EXTERNAL)
	install -d i386
	gcc -g -static type.o $(VMSLIBS) $(EXTERNAL) -o i386/type${NOEXE}

i386/type${EXE_ELF}: $(VMSLIBS_PIC) pic/type.o $(EXTERNAL_PIC)
	ld -Bshareable pic/type.o $(VMSLIBS_PIC) $(EXTERNAL_PIC) -lc $(LIBGCC) -o i386/type${EXE_ELF}

i386/type${EXE}: $(VMSLIBS) type.o $(EXTERNAL)
	install -d i386
	$(LINKER) $(LINKPRE) type.o $(VMSLIBS) $(LIBGCC) $(EXTERNAL) --start-group $(LINKPOST) $(LIBGCC) --end-group -o i386/type${EXE}

i386/show${NOEXE}: show.a showmain.o $(VMLINUX)
	install -d i386
	gcc -g -static showmain.o show.a -lc $(VMSLIBS) $(EXTERNAL) $(LINKVMLINUX) -o i386/show${NOEXE}

i386/set${NOEXE}: set.a setmain.o $(VMLINUX)
	install -d i386
	gcc -g -static setmain.o set.a -lc $(VMSLIBS) $(EXTERNAL) $(LINKVMLINUX) -o i386/set${NOEXE}
