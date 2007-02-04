FLAGS = -static

all: i386/search${EXE} i386/search${NOEXE} i386/search${EXE_ELF} um/search${EXE} um/search${NOEXE} um/search${EXE_ELF}

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/*.* ../../rooti386/vms"$$"common/sysexe
	install um/*.* ../../rootum/vms"$$"common/sysexe

OBJS = search.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

pic/%.o: %.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/search${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL)
	install -d i386
	gcc -g -static $(OBJS) $(VMSLIBS) $(EXTERNAL) -o i386/search${NOEXE}

i386/search${EXE_ELF}: $(VMSLIBS_PIC) $(OBJS_PIC) $(EXTERNAL_PIC)
	ld -Bshareable $(OBJS_PIC) $(VMSLIBS_PIC) $(EXTERNAL_PIC) -lc $(LIBGCC) -o i386/search${EXE_ELF}

i386/search${EXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) $(LIBGCC) $(EXTERNAL) --start-group $(LINKPOST) $(LIBGCC) --end-group -o i386/search${EXE}

UMDEF=-Dcompile_um -I.

um/search${NOEXE}: $(VMSLIBSUM) $(OBJS) $(EXTERNAL)
	install -d um
	gcc -g -static $(OBJS) $(VMSLIBSUM) $(EXTERNAL) -o um/search${NOEXE}
um/search${EXE_ELF}: $(VMSLIBSUM_PIC) $(OBJS_PIC) $(EXTERNAL_PIC)
	ld -Bshareable $(OBJS_PIC) $(VMSLIBSUM_PIC) $(EXTERNAL_PIC) -lc $(LIBGCC) -o um/search${EXE_ELF}

um/search${EXE}: $(VMSLIBSUM) $(OBJS)
	install -d um
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBSUM) $(LIBGCC) $(EXTERNAL) --start-group $(LINKPOST) $(LIBGCC) --end-group -o um/search${EXE}
