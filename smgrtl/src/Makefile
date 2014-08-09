CFLAGS = -Wall -Wmissing-prototypes -Wcast-qual -Wconversion -I. -I../../starlet/src -I../../librtl/src -DNOKERNEL

SMGOBJ = \
	smgchar.o smgpaste.o smgread.o smgview.o smgvirdis.o smgvirkey.o

SMGOBJ_PIC = $(addprefix pic/,$(SMGOBJ))

SMGLIB = smgrtl.a
SMGLIB_PIC = smgrtl_pic.a
#ELFLIB = i386/smgrtl$(EXE_ELF)
#EXELIB = i386/smgrtl$(EXE)

all : $(SMGLIB) $(SMGLIB_PIC) $(EXELIB) tests
#$(ELFLIB)

install: all
	install -d ../../rooti386/vms"$$"common/syslib
#	install i386/*.ele ../../rooti386/vms"$$"common/syslib
	install i386/*.exe ../../rooti386/vms"$$"common/syslib

clean :
	- rm *.o
	- rm *.a

tests:

$(SMGLIB) : $(SMGOBJ)
	ar -rv $(SMGLIB) $?

$(SMGLIB_PIC) : $(SMGOBJ_PIC)
	ar -rv $(SMGLIB_PIC) $?

$(EXELIB) : $(SMGOBJ)
	install -d i386
	$(LINKER) -Bshareable $? ../../starlet/src/i386/starlet$(EXE) -o $(EXELIB)

starlet$(EXE_ELF): $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)
	ln -s $(TOPDIR)/starlet/src/i386/starlet$(EXE_ELF)

$(ELFLIB) : $(SMGOBJ_PIC) starlet$(EXE_ELF)
	install -d i386
	ld -Bshareable $(VMS_LD_RPATH) $? starlet$(EXE_ELF) $(LIBGCC) -o $(ELFLIB)

pic/%.o: %.c 
	install -d pic
	$(CC) -fpic -fPIC $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<

%.o: %.c 
	install -d pic
	$(CC) -g $(CFLAGS) $(EXTRA_CFLAGS) $(CFLAGS_$@) -c -o $@ $<

