FLAGS = -static

all: i386/delete${EXE} i386/delete${NOEXE} i386/delete${EXE_ELF} um/delete${EXE} um/delete${NOEXE} um/delete${EXE_ELF}

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/*.* ../../rooti386/vms"$$"common/sysexe
	install um/*.* ../../rootum/vms"$$"common/sysexe

OBJS = delete.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/%.o: %.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/delete${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL2)
	install -d i386
	gcc -g -static $(OBJS) $(VMSLIBS) $(EXTERNAL2) -o i386/delete${NOEXE}

i386/delete${EXE_ELF}: $(VMSLIBS_PIC) $(OBJS_PIC) $(EXTERNAL2_PIC)
	ld -Bshareable $(OBJS_PIC) $(VMSLIBS_PIC) $(EXTERNAL2_PIC) -lc $(LIBGCC) -o i386/delete${EXE_ELF}

i386/delete${EXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL2)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) $(LIBGCC) $(EXTERNAL2) --start-group $(LINKPOST) $(LIBGCC) --end-group -o i386/delete${EXE}

UMDEF=-Dcompile_um -I.

um/delete${NOEXE}: $(VMSLIBSUM) $(OBJS) $(EXTERNAL2)
	install -d um
	gcc -g -static $(OBJS) $(VMSLIBSUM) $(EXTERNAL2) -o um/delete${NOEXE}
um/delete${EXE_ELF}: $(VMSLIBSUM_PIC) $(OBJS_PIC) $(EXTERNAL2_PIC)
	ld -Bshareable $(OBJS_PIC) $(VMSLIBSUM_PIC) $(EXTERNAL2_PIC) -lc $(LIBGCC) -o um/delete${EXE_ELF}

um/delete${EXE}: $(VMSLIBSUM) $(OBJS)
	install -d um
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBSUM) $(LIBGCC) $(EXTERNAL2) --start-group $(LINKPOST) $(LIBGCC) --end-group -o um/delete${EXE}
