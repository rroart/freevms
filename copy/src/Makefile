FLAGS = -static

all: i386/copy${EXE} i386/copy${NOEXE} i386/copy${EXE_ELF} um/copy${EXE} um/copy${NOEXE} um/copy${EXE_ELF}

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/*.* ../../rooti386/vms"$$"common/sysexe
	install um/*.* ../../rootum/vms"$$"common/sysexe

OBJS = copy.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/%.o: %.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/copy${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL2)
	install -d i386
	gcc -g -static $(OBJS) $(VMSLIBS) $(EXTERNAL2) -o i386/copy${NOEXE}

i386/copy${EXE_ELF}: $(VMSLIBS_PIC) $(OBJS_PIC) $(EXTERNAL2_PIC)
	ld -Bshareable $(OBJS_PIC) $(VMSLIBS_PIC) $(EXTERNAL2_PIC) -lc $(LIBGCC) -o i386/copy${EXE_ELF}

i386/copy${EXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL2)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) $(LIBGCC) $(EXTERNAL2) --start-group $(LINKPOST) $(LIBGCC) --end-group -o i386/copy${EXE}

UMDEF=-Dcompile_um -I.

um/copy${NOEXE}: $(VMSLIBSUM) $(OBJS) $(EXTERNAL2)
	install -d um
	gcc -g -static $(OBJS) $(VMSLIBSUM) $(EXTERNAL2) -o um/copy${NOEXE}
um/copy${EXE_ELF}: $(VMSLIBSUM_PIC) $(OBJS_PIC) $(EXTERNAL2_PIC)
	ld -Bshareable $(OBJS_PIC) $(VMSLIBSUM_PIC) $(EXTERNAL2_PIC) -lc $(LIBGCC) -o um/copy${EXE_ELF}

um/copy${EXE}: $(VMSLIBSUM) $(OBJS)
	install -d um
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBSUM) $(LIBGCC) $(EXTERNAL2) --start-group $(LINKPOST) $(LIBGCC) --end-group -o um/copy${EXE}
