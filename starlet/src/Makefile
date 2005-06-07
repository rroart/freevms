all: starlet.a i386/starlet${EXE_ELF} i386/starlet${EXE} starletum.a

install: all
	install -d ../../rooti386/vms"$$"common/syslib
	install i386/*.* ../../rooti386/vms"$$"common/syslib

i386/starlet${EXE_ELF}: starlet.o rms.o errno.o
	install -d i386
	ld -Bshareable starlet.o rms.o errno.o -o i386/starlet${EXE_ELF}

i386/starlet${EXE}: starlet.o rms.o errno.o
	install -d i386
	$(LINKER) -Bshareable starlet.o rms.o errno.o -o i386/starlet${EXE}

starlet.a: vmsopt.a starlet.o starlet.h
	gcc -c starlet.c
	gcc -Dcompile_um -c starlet.c -o starletum.o
	gcc -c -I../../starlet/src rms.c
	ar r starlet.a starlet.o rms.o
	ar r starletum.a starletum.o rms.o
	ranlib starlet.a
	ranlib starletum.a

vmsopt.a:	vmsopt.o
		rm vmsopt.a; ar q vmsopt.a vmsopt.o; ranlib vmsopt.a

vmsopt.o:	vmsopt.c
		gcc -O2 -o vmsopt.o -c vmsopt.c

errno.o: 	errno.c
		gcc -c errno.c