all: dirs alli386

clean:
	rm *.o i386/*

install: all
	install -d ../rooti386/vms"$$"common/syshlp/examples
	install i386/cef.exe ../rooti386/vms"$$"common/syshlp/examples
#	install *.com net net2 INET"$$"CONFIG i386/* ../rooti386/vms"$$"common/syshlp/examples

alli386: i386/timer i386/ast i386/time i386/eftim i386/schdwk i386/forcex i386/ef i386/wake i386/synch i386/enq i386/enq2 i386/lnm i386/lnm2 i386/qio1 i386/qio2 i386/cef i386/cefw i386/mem i386/memf i386/getjpi i386/crec i386/getdvi i386/inout i386/mbx i386/creprc i386/hi i386/hi.exe i386/modes i386/cef.exe i386/cm

dirs:
	install -d i386

FLAGS = -static

i386/timer: timer.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ timer.c $(VMSLIBS) -o i386/timer
i386/time: gettim.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ gettim.c $(VMSLIBS) -o i386/time

i386/ast: astdel.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ astdel.c $(VMSLIBS) -o i386/ast

i386/eftim: eftim.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ eftim.c $(VMSLIBS) -o i386/eftim

i386/schdwk: schdwk.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ schdwk.c $(VMSLIBS) -o i386/schdwk

i386/forcex: forcex.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ forcex.c $(VMSLIBS) -o i386/forcex

i386/ef: ef.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ ef.c $(VMSLIBS) -o i386/ef

i386/wake: wake.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ wake.c $(VMSLIBS) -o i386/wake


i386/synch: synch.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ synch.c $(VMSLIBS) -o i386/synch


i386/enq: enq.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ enq.c $(VMSLIBS) -o i386/enq

i386/enq2: enq2.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ enq2.c $(VMSLIBS) -o i386/enq2

i386/lnm: lnm.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ lnm.c $(VMSLIBS) -o i386/lnm

i386/lnm2: lnm2.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ lnm2.c $(VMSLIBS) -o i386/lnm2

i386/qio1: qio1.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ qio1.c $(VMSLIBS) -o i386/qio1

i386/qio2: qio2.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ qio2.c $(VMSLIBS) -o i386/qio2

i386/cef: cef.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ cef.c $(VMSLIBS) -o i386/cef

i386/cef$(EXE): cefq.c $(VMSLIBS)
	gcc -c -$(FLAGS) -I../starlet/src -I../librtl/src/ cefq.c $(VMSLIBS)
	$(LINKER) cefq.o ../starlet/src/i386/starlet.exe -o i386/cef$(EXE)

i386/cefw: cefwait.c $(VMSLIBS)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ cefwait.c $(VMSLIBS) -o i386/cefw

i386/mem: mem.c $(VMSLIBS)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ mem.c $(VMSLIBS) -o i386/mem

i386/memf: memf.c $(VMSLIBS)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ memf.c $(VMSLIBS) -o i386/memf

i386/getjpi: getjpi.c $(VMSLIBS)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ getjpi.c $(VMSLIBS) -o i386/getjpi

i386/crec: crec.c $(VMSLIBS)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ crec.c $(VMSLIBS) -o i386/crec

i386/getdvi: getdvi.c $(VMSLIBS)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ getdvi.c $(VMSLIBS) -o i386/getdvi

i386/inout: inout.c $(VMSLIBS)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ inout.c $(VMSLIBS) -o i386/inout

i386/mbx: mbx.c $(VMSLIBS)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ mbx.c $(VMSLIBS) -o i386/mbx

i386/creprc: creprc.c $(VMSLIBS)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ creprc.c $(VMSLIBS) -o i386/creprc

hi.o: hi.c
	gcc -c hi.c

ifeq ($(ARCH),i386)
i386/hi: hi.o $(VMSLIBS)
	../linker/src/linker -L../linker/src/build/ld -melf_i386 -Bstatic hi.o $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/hi
	ld -Bshareable hi.o $(LIBC) -o i386/hi.exe2
else
i386/hi:
	touch i386/hi
endif

ifeq ($(ARCH),i386)
i386/hi.exe: hi.o $(VMSLIBS)
	../linker/src/linker $(LINKPRE) hi.o $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/hi.exe
else
i386/hi.exe:
	touch i386/hi.exe
endif

i386/modes: modes.o $(VMSLIBS)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ modes.c $(VMSLIBS) -o i386/modes

i386/cm: cm.o $(VMSLIBS) ../linux/vmlinux
	gcc -$(FLAGS) -static -s -I../starlet/src -I../librtl/src/ -lc -Wl,--just-symbols=../linux/vmlinux  cm.c $(VMSLIBS) -o i386/cm
