#all: i386/crtl${EXE_ELF} i386/crtl${EXE}
all: i386/crtl${EXE}

install: all
	#install -d ../../rooti386/vms"$$"common/syslib
	#install i386/*.exe ../../rooti386/vms"$$"common/syslib
	rm -f ../../rooti386/vms"$$"common/syslib/crtl.exe
	touch ../../rooti386/vms"$$"common/syslib/crtl.exe

libc-start.o: /usr/lib/libc.a
	ar x /usr/lib/libc.a

i386/crtl${EXE_ELF}: libc-start.o
	install -d i386
	ld -Bshareable *.o -o i386/crtl${EXE_ELF}

i386/crtl${EXE}: libc-start.o
	install -d i386
	$(LINKER) -Bshareable *.o -o i386/crtl${EXE}
