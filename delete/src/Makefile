FLAGS = -static

all: i386/delete${NOEXE}
#i386/delete${EXE} 
#i386/delete${EXE_ELF}

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe

OBJS = delete.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

pic/%.o: %.c
	install -d pic
	gcc -fPIC -fpic -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/delete${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL)
	install -d i386
	gcc -g -static $(OBJS) -L$(OLDLIBCDIR) -L$(OLDLIBGCCDIR) $(VMSLIBS) $(EXTERNAL) -o i386/delete${NOEXE}

i386/delete${EXE_ELF}: $(VMSLIBS_PIC) $(OBJS_PIC) $(EXTERNAL_PIC)
	ld -Bshareable $(OBJS_PIC) $(VMSLIBS_PIC) $(EXTERNAL_PIC) -lc $(LIBGCC) -o i386/delete${EXE_ELF}

i386/delete${EXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) $(LIBGCC) $(EXTERNAL) --start-group $(LINKPOST) $(LIBGCC) --end-group -o i386/delete${EXE}

include $(TOPDIR)/Rules.make.freevms
