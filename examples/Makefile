all: dirs alli386 allum

clean:
	rm *.o um/* i386/*

install: all
	install -d ../rootum/vms"$$"common/syshlp/examples
	install -d ../rooti386/vms"$$"common/syshlp/examples
	install *.com net net2 INET"$$"CONFIG um/* ../rootum/vms"$$"common/syshlp/examples
	install *.com net net2 INET"$$"CONFIG i386/* ../rooti386/vms"$$"common/syshlp/examples

alli386: i386/timer i386/ast i386/time i386/eftim i386/schdwk i386/forcex i386/ef i386/wake i386/synch i386/enq i386/enq2 i386/lnm i386/lnm2 i386/qio1 i386/qio2 i386/cef i386/cefw i386/mem i386/memf i386/getjpi i386/crec i386/getdvi i386/inout i386/mbx i386/creprc i386/hi i386/hi.exe

dirs:
	install -d um
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

i386/hi: hi.o $(VMSLIBS)
	../linker/src/linker -L../linker/src/build/ld -melf_i386 -Bstatic hi.o $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/hi
	ld -Bshareable hi.o $(LIBC) -o i386/hi.exe2

i386/hi.exe: hi.o $(VMSLIBS)
	../linker/src/linker $(LINKPRE) hi.o $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/hi.exe

allum: um/timer um/ast um/time um/eftim um/schdwk um/forcex um/ef um/wake um/synch um/enq um/enq2 um/lnm um/lnm2 um/qio1 um/qio2 um/cef um/cefw um/mem um/memf um/getjpi um/crec um/getdvi um/inout um/mbx um/creprc

UMDEF=-Dcompile_um

um/timer: timer.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ timer.c $(VMSLIBSUM) -o um/timer
um/time: gettim.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ gettim.c $(VMSLIBSUM) -o um/time

um/ast: astdel.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ astdel.c $(VMSLIBSUM) -o um/ast

um/eftim: eftim.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ eftim.c $(VMSLIBSUM) -o um/eftim

um/schdwk: schdwk.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ schdwk.c $(VMSLIBSUM) -o um/schdwk

um/forcex: forcex.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ forcex.c $(VMSLIBSUM) -o um/forcex

um/ef: ef.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ ef.c $(VMSLIBSUM) -o um/ef

um/wake: wake.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ wake.c $(VMSLIBSUM) -o um/wake


um/synch: synch.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ synch.c $(VMSLIBSUM) -o um/synch


um/enq: enq.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ enq.c $(VMSLIBSUM) -o um/enq

um/enq2: enq2.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ enq2.c $(VMSLIBSUM) -o um/enq2

um/lnm: lnm.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ lnm.c $(VMSLIBSUM) -o um/lnm

um/lnm2: lnm2.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ lnm2.c $(VMSLIBSUM) -o um/lnm2

um/qio1: qio1.c $(VMSLIBSUM)
	gcc -$(FLAGS) ${UMDEF} -I../starlet/src -I../librtl/src/ qio1.c $(VMSLIBSUM) -o um/qio1

um/qio2: qio2.c $(VMSLIBSUM)
	gcc -$(FLAGS) ${UMDEF} -I../starlet/src -I../librtl/src/ qio2.c $(VMSLIBSUM) -o um/qio2

um/cef: cef.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ cef.c $(VMSLIBSUM) -o um/cef

um/cefw: cefwait.c $(VMSLIBSUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ cefwait.c $(VMSLIBSUM) -o um/cefw

um/mem: mem.c $(VMSLIBSUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ mem.c $(VMSLIBSUM) -o um/mem

um/memf: memf.c $(VMSLIBSUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ memf.c $(VMSLIBSUM) -o um/memf

um/getjpi: getjpi.c $(VMSLIBSUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ getjpi.c $(VMSLIBSUM) -o um/getjpi

um/crec: crec.c $(VMSLIBSUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ crec.c $(VMSLIBSUM) -o um/crec

um/getdvi: getdvi.c $(VMSLIBSUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ getdvi.c $(VMSLIBSUM) -o um/getdvi

um/inout: inout.c $(VMSLIBSUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ inout.c $(VMSLIBSUM) -o um/inout

um/mbx: mbx.c $(VMSLIBSUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ mbx.c $(VMSLIBSUM) -o um/mbx

um/creprc: creprc.c $(VMSLIBSUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ creprc.c $(VMSLIBSUM) -o um/creprc
