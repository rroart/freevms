EXTERNAL = ../../cliutl/src/show.a ../../cliutl/src/set.a ../../cdu/src/cdu.a ../../cdu/src/cdu2.a ../../cdu/src/dcltables.o

#all: i386/dcl${NOEXE} i386/dcl${EXE} um/dcl${NOEXE} um/dcl${EXE}
all: i386/dcl${NOEXE} um/dcl${NOEXE} startup.com install.com

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms"$$"common/sysexe
	install -d ../../rooti386/vms"$$"common/sysexe
	install um/* ../../rootum/vms"$$"common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe
	install *.com ../../rooti386/vms"$$"common/sysexe

OBJS = cli.o clicall.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@


i386/dcl${NOEXE}: $(OBJS) $(VMSLIBS) $(EXTERNAL)
	install -d i386
ifeq ($(ARCH),i386)
	gcc -s -static -Wl,-Telf_i386.x $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcl${NOEXE}
	gcc -g -static -Wl,-Telf_i386.x $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcld${NOEXE}
endif
ifeq ($(ARCH),x86_64)
	gcc -s -static -Wl,-Telf_x86_64.x $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcl${NOEXE}
	gcc -g -static -Wl,-Telf_x86_64.x $(OBJS) $(EXTERNAL) $(VMSLIBS) -ldl -lelf -o i386/dcld${NOEXE}
endif

i386/dcl${EXE}: $(OBJS) $(VMSLIBS) $(EXTERNAL)
	$(LINKER) $(LINKPRE) $(OBJS) $(EXTERNAL) $(VMSLIBS) /usr/lib/libdl.a /usr/lib/libiberty.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/dcl${EXE}
# DCL linked like this is just for testing (DCL is really special)

um/dcl${NOEXE}: $(OBJS) $(VMSLIBSUM) $(EXTERNAL)
	install -d um
	gcc -s -static $(OBJS) $(EXTERNAL) $(VMSLIBSUM) -ldl -lelf -o um/dcl${NOEXE}
	gcc -static $(OBJS) $(EXTERNAL) $(VMSLIBSUM) -ldl -lelf -o um/dcld${NOEXE}

um/dcl${EXE}: $(OBJS) $(VMSLIBSUM) $(EXTERNAL)
	$(LINKER) $(LINKPRE) $(OBJS) $(EXTERNAL) $(VMSLIBSUM) /usr/lib/libdl.a /usr/lib/libiberty.a $(LIBGCC) $(LINKPOST) $(LIBGCC) -o um/dcl${EXE}
# DCL linked like this is just for testing (DCL is really special)

