FLAGS = -static

SMGRTL = ../../smgrtl/src/smgrtl.a
SORRTL = ../../sort/src/sortrtl.a

all: i386/dfu${NOEXE}
# i386/dfu${EXE}

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe

OBJS = dfu.o dfu_bitmap.o dfu_filedir.o dfu_ifscan.o dfu_tables.o

dfu_tables.o: dfu_tables.c
	$(CC) -g $(EXTRA_CFLAGS) -I../../cdu/src -c -o dfu_tables.o dfu_tables.c

dfu_tables.c: ../../cdu/src/cdu dfu_tables.cld
	../../cdu/src/cdu dfu_tables.cld

i386/dfu${NOEXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(SMGRTL) $(SORRTL)
	install -d i386
	gcc -g -static $(OBJS) -L$(OLDLIBCDIR) -L$(OLDLIBELFDIR) -L$(OLDLIBGCCDIR) $(EXTERNAL) $(SMGRTL) $(SORRTL) $(VMSLIBS) -ldl -lelf -o i386/dfu${NOEXE} -v

i386/dfu${EXE}: $(VMSLIBS) $(OBJS) $(EXTERNAL) $(SMGRTL) $(SORRTL)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(EXTERNAL) $(SMGRTL) $(SORRTL) $(VMSLIBS) /usr/lib/libdl.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/dfu${EXE}

include $(TOPDIR)/Rules.make.freevms
