FLAGS = -static

EXTERNAL = ../../cdu/src/cdu.a
EXTERNAL2 = ../../cdu/src/cdu2.a

all: i386/delete${EXE} i386/delete${NOEXE} i386/delete${EXE_ELF} um/delete${EXE} um/delete${NOEXE} um/delete${EXE_ELF}

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms$$common/sysexe
	install -d ../../rooti386/vms$$common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe
	install um/* ../../rootum/vms"$$"common/sysexe

OBJS = delete.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/delete${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL2)
	install -d i386
	gcc -g -static $(OBJS) $(VMSLIBS) $(EXTERNAL2) -o i386/delete${NOEXE}

i386/delete${EXE_ELF}: $(VMSLIBS) $(OBJS) $(EXTERNAL2)
	ld -Bshareable $(OBJS) $(VMSLIBS) $(EXTERNAL2) $(LIBC) $(LIBGCC) -o i386/delete${EXE_ELF}

i386/delete${EXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL2)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) $(LIBGCC) $(EXTERNAL2) $(LINKPOST) $(LIBGCC) -o i386/delete${EXE}

UMDEF=-Dcompile_um -I.

um/delete${NOEXE}: $(VMSLIBSUM) $(OBJS) $(EXTERNAL2)
	install -d um
	gcc -g -static $(OBJS) $(VMSLIBSUM) $(EXTERNAL2) -o um/delete${NOEXE}
um/delete${EXE_ELF}: $(VMSLIBSUM) $(OBJS) $(EXTERNAL2)
	ld -Bshareable $(OBJS) $(VMSLIBSUM) $(EXTERNAL2) $(LIBC) $(LIBGCC) -o i386/delete${EXE_ELF}

um/delete${EXE}: $(VMSLIBSUM) $(OBJS)
	install -d um
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBSUM) $(LIBGCC) $(EXTERNAL2) $(LINKPOST) $(LIBGCC) -o um/delete${EXE}
