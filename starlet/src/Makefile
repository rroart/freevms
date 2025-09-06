all: starlet.a starletum.a starlet_pic.a
#i386/starlet${EXE} 
#i386/starlet${EXE_ELF}

MYCFLAGS = -Wno-error=declaration-missing-parameter-type -Wno-error=implicit-int -Wno-error=int-conversion

clean:
	rm *.o i386/*

install: all
	install -d ../../rooti386/vms"$$"common/syslib
#	install i386/*.* ../../rooti386/vms"$$"common/syslib

i386/starlet${EXE_ELF}: pic/starlet.o pic/rms.o pic/errno.o
	install -d i386
	ld -Bshareable pic/starlet.o pic/rms.o pic/errno.o -o i386/starlet${EXE_ELF}

i386/starlet${EXE}: starlet.o rms.o errno.o
	install -d i386
	$(LINKER) -Bshareable starlet.o rms.o errno.o -o i386/starlet${EXE}

OBJS = starlet.o rms.o
OBJS_PIC = $(addprefix pic/,$(OBJS))

starletum.a: starlet.a
ifneq ($(ARCH),x86_64)
	gcc $(MYCFLAGS) -Dcompile_um -I. -c starlet.c -o starletum.o
	gcc $(MYCFLAGS) -fPIC -fpic -Dcompile_um -I. -c starlet.c -o starletum_pic.o
else
	gcc $(MYCFLAGS) -I. -c starlet.c -o starletum.o
	gcc $(MYCFLAGS) -fPIC -fpic -I. -c starlet.c -o starletum_pic.o
endif
	gcc -fPIC -fpic -c -I. rms.c -o rms_pic.o
	ar r starletum.a starletum.o rms.o
	ranlib starletum.a
	ar r starletum_pic.a starletum_pic.o rms_pic.o
	ranlib starletum_pic.a

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

errno.o: 	errno.c
		gcc $(CFLAGS) ${OLDINCLUDE} -c errno.c

pic/%.o: %.c
		install -d pic
		gcc $(MYCFLAGS) -fpic -fPIC -c -I. $< -o $@

include $(TOPDIR)/Rules.make.freevms
