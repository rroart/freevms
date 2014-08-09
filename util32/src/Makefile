FLAGS = -static

all: i386/search${EXE} i386/search${NOEXE}
#i386/search${EXE_ELF}

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/*.* ../../rooti386/vms"$$"common/sysexe

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
