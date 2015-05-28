all: starlet.a starlet_pic.a
#i386/starlet${EXE} 
#i386/starlet${EXE_ELF} 

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/syslib
#	install i386/*.* ../../rooti386/vms"$$"common/syslib

i386/starlet${EXE_ELF}: pic/starlet.o pic/rms.o
	install -d i386
	ld -Bshareable pic/starlet.o pic/rms.o -o i386/starlet${EXE_ELF}

i386/starlet${EXE}: starlet.o rms.o
	install -d i386
	$(LINKER) -Bshareable starlet.o rms.o -o i386/starlet${EXE}

OBJS = starlet.o rms.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

starlet.a: $(OBJS)
	ar rv starlet.a $(OBJS)
	ranlib starlet.a

starlet_pic.a: $(OBJS_PIC)
	ar rv starlet_pic.a $(OBJS_PIC)
	ranlib starlet_pic.a

vmsopt.a:	vmsopt.o
		rm vmsopt.a; ar q vmsopt.a vmsopt.o; ranlib vmsopt.a

vmsopt.o:	vmsopt.c
		gcc $(CFLAGS) ${OLDINCLUDE} -O2 -o vmsopt.o -c vmsopt.c

pic/%.o: %.c
		install -d pic
		gcc -fpic -fPIC -c -I. $< -o $@

include $(TOPDIR)/Rules.make.freevms
