CFLAGS = -Wall -Wmissing-prototypes -Wcast-qual -Wconversion -I. -I../../starlet/src -I../../librtl/src -DNOKERNEL

SORTOBJ = \
	sort.o

SORTOBJ_PIC = $(addprefix pic/,$(SORTOBJ))

SORTLIB = sortrtl.a
SORTLIB_PIC = sortrtl_pic.a
#ELFLIB = i386/sortrtl$(EXE_ELF)
#EXELIB = i386/sortrtl$(EXE)

all : $(SORTLIB) $(SORTLIB_PIC) $(EXELIB) tests
#$(ELFLIB)

install: all
	install -d ../../rooti386/vms"$$"common/syslib
	install i386/*.ele ../../rooti386/vms"$$"common/syslib
	install i386/*.exe ../../rooti386/vms"$$"common/syslib

clean :
	- rm *.o
	- rm *.a

tests:

$(SORTLIB) : $(SORTOBJ)
	ar -rv $(SORTLIB) $?

$(SORTLIB_PIC) : $(SORTOBJ_PIC)
	ar -rv $(SORTLIB_PIC) $?

$(EXELIB) : $(SORTOBJ)
	install -d i386
	$(LINKER) -Bshareable $? ../../starlet/src/i386/starlet$(EXE) -o $(EXELIB)

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

$(ELFLIB) : $(SORTOBJ_PIC) starlet$(EXE_ELF)
	install -d i386
	ld -Bshareable $(VMS_LD_RPATH) $? starlet$(EXE_ELF) $(LIBGCC) -o $(ELFLIB)

pic/%.o: %.c 
	install -d pic
	$(CC) -fpic -fPIC $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<

%.o: %.c 
	install -d pic
	$(CC) -g $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<

