FLAGS = -static

all: i386/directory${EXE} i386/directory${NOEXE} um/directory${EXE} um/directory${NOEXE}

clean:
	rm *.o um/* i386/*

install: all
	install -d ../../rootum/vms$$common/sysexe
	install -d ../../rooti386/vms$$common/sysexe
	install i386/* ../../rooti386/vms"$$"common/sysexe
	install um/* ../../rootum/vms"$$"common/sysexe
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} copy${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} import${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} export${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} delete${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} difference${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} extend${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} help${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} show${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} search${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} type${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); ln -f directory${NOEXE} mount${NOEXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} copy${EXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} import${EXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} export${EXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} delete${EXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} difference${EXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} extend${EXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} help${EXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} show${EXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} search${EXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} type${EXE}
	cd $(ROOTI386_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} mount${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} copy${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} import${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} export${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} delete${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} difference${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} extend${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} help${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} show${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} search${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} type${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); ln -f directory${NOEXE} mount${NOEXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} copy${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} import${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} export${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} delete${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} difference${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} extend${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} help${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} show${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} search${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} type${EXE}
	cd $(ROOTUM_COMMON_SYSEXE); [ -f directory${EXE} ] && ln -f directory${EXE} mount${EXE}

OBJS = directory.o

%.o: %.c
	gcc -g -c -I../../starlet/src -I../../librtl/src -I../../lib/src/ $< -o $@

i386/directory${NOEXE}: $(VMSLIBS) $(OBJS)
	install -d i386
	gcc -g -static $(OBJS) $(VMSLIBS) -o i386/directory${NOEXE}

i386/directory${EXE}: $(VMSLIBS) $(OBJS)
	install -d i386
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBS) $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/directory${EXE}

UMDEF=-Dcompile_um -I.

um/directory${NOEXE}: $(VMSLIBSUM) $(OBJS)
	install -d um
	gcc -g -static $(OBJS) $(VMSLIBSUM) -o um/directory${NOEXE}

um/directory${EXE}: $(VMSLIBSUM) $(OBJS)
	install -d um
	$(LINKER) $(LINKPRE) $(OBJS) $(VMSLIBSUM) $(LIBGCC) $(LINKPOST) $(LIBGCC) -o um/directory${EXE}
