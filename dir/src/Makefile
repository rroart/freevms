FLAGS = -static

EXTERNAL = ../../cdu/src/cdu.a
EXTERNAL2 = ../../cdu/src/cdu2.a

all: i386/directory${EXE} i386/directory${NOEXE} i386/directory${EXE_ELF} um/directory${EXE} um/directory${NOEXE} um/directory${EXE_ELF}

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms$$common/sysexe
	install -d ../../rooti386/vms$$common/sysexe
	install i386/*.* ../../rooti386/vms"$$"common/sysexe
	install um/*.* ../../rootum/vms"$$"common/sysexe

OBJS = directory.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/directory${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL2)
	install -d i386
	gcc -g -static $(OBJS) $(VMSLIBS) $(EXTERNAL2) -o i386/directory${NOEXE}

i386/directory${EXE_ELF}: $(VMSLIBS) $(OBJS) $(EXTERNAL2)
	ld -Bshareable $(OBJS) $(VMSLIBS) $(EXTERNAL2) $(LIBC) $(LIBGCC) -o i386/directory${EXE_ELF}

i386/directory${EXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL2)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) $(LIBGCC) $(EXTERNAL2) $(LINKPOST) $(LIBGCC) -o i386/directory${EXE}

UMDEF=-Dcompile_um -I.

um/directory${NOEXE}: $(VMSLIBSUM) $(OBJS) $(EXTERNAL2)
	install -d um
	gcc -g -static $(OBJS) $(VMSLIBSUM) $(EXTERNAL2) -o um/directory${NOEXE}
um/directory${EXE_ELF}: $(VMSLIBSUM) $(OBJS) $(EXTERNAL2)
	ld -Bshareable $(OBJS) $(VMSLIBSUM) $(EXTERNAL2) $(LIBC) $(LIBGCC) -o um/directory${EXE_ELF}

um/directory${EXE}: $(VMSLIBSUM) $(OBJS)
	install -d um
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBSUM) $(LIBGCC) $(EXTERNAL2) $(LINKPOST) $(LIBGCC) -o um/directory${EXE}
