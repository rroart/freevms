all: dirs alli386 allum

install: all
	[ ! -d ../rootum/vms$$common/syshlp/examples ] && mkdir -p ../rootum/vms"$$"common/syshlp/examples
	[ ! -d ../rooti386/vms$$common/syshlp/examples ] && mkdir -p ../rooti386/vms"$$"common/syshlp/examples
	cp -p *.com net net2 INET"$$"CONFIG um/* ../rootum/vms"$$"common/syshlp/examples
	cp -p *.com net net2 INET"$$"CONFIG i386/* ../rooti386/vms"$$"common/syshlp/examples

alli386: i386/timer i386/ast i386/time i386/eftim i386/schdwk i386/forcex i386/ef i386/wake i386/synch i386/enq i386/enq2 i386/lnm i386/lnm2 i386/qio1 i386/qio2 i386/cef i386/cefw i386/mem i386/memf i386/getjpi i386/crec i386/getdvi i386/inout i386/mbx i386/creprc i386/hi i386/hi.exe

dirs:
	mkdir um || echo um exists
	mkdir i386 || echo i386 exists

LIB = ../starlet/src/starlet.a
LIBUM = ../starlet/src/starletum.a
LIBRTL = ../librtl/src/starlet.a

FLAGS = -static

i386/timer: timer.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ timer.c ../starlet/src/starlet.a -o i386/timer
i386/time: gettim.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ gettim.c ../starlet/src/starlet.a -o i386/time

i386/ast: astdel.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ astdel.c ../starlet/src/starlet.a -o i386/ast

i386/eftim: eftim.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ eftim.c ../starlet/src/starlet.a -o i386/eftim

i386/schdwk: schdwk.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ schdwk.c ../starlet/src/starlet.a -o i386/schdwk

i386/forcex: forcex.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ forcex.c ../starlet/src/starlet.a -o i386/forcex

i386/ef: ef.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ ef.c ../starlet/src/starlet.a -o i386/ef

i386/wake: wake.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ wake.c ../starlet/src/starlet.a -o i386/wake


i386/synch: synch.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ synch.c ../starlet/src/starlet.a -o i386/synch


i386/enq: enq.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ enq.c ../starlet/src/starlet.a -o i386/enq

i386/enq2: enq2.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ enq2.c ../starlet/src/starlet.a -o i386/enq2

i386/lnm: lnm.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ lnm.c ../starlet/src/starlet.a -o i386/lnm

i386/lnm2: lnm2.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ lnm2.c ../starlet/src/starlet.a -o i386/lnm2

i386/qio1: qio1.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ qio1.c ../starlet/src/starlet.a -o i386/qio1

i386/qio2: qio2.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ qio2.c ../starlet/src/starlet.a -o i386/qio2

i386/cef: cef.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ cef.c ../starlet/src/starlet.a -o i386/cef

i386/cefw: cefwait.c $(LIB)
	gcc -$(FLAGS) -I../starlet/src -I../librtl/src/ cefwait.c ../starlet/src/starlet.a -o i386/cefw

i386/mem: mem.c $(LIB)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ mem.c ../starlet/src/starlet.a -o i386/mem

i386/memf: memf.c $(LIB)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ memf.c ../starlet/src/starlet.a -o i386/memf

i386/getjpi: getjpi.c $(LIB)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ getjpi.c ../starlet/src/starlet.a -o i386/getjpi

i386/crec: crec.c $(LIB)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ crec.c ../starlet/src/starlet.a -o i386/crec

i386/getdvi: getdvi.c $(LIB)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ getdvi.c ../starlet/src/starlet.a -o i386/getdvi

i386/inout: inout.c $(LIB) $(LIBRTL)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ inout.c $(LIBRTL) ../starlet/src/starlet.a -o i386/inout

i386/mbx: mbx.c $(LIB) $(LIBRTL)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ mbx.c $(LIBRTL) ../starlet/src/starlet.a -o i386/mbx

i386/creprc: creprc.c $(LIB) $(LIBRTL)
	gcc -$(FLAGS) -static -I../starlet/src -I../librtl/src/ creprc.c $(LIBRTL) ../starlet/src/starlet.a -o i386/creprc

hi.o: hi.c
	gcc -c hi.c

i386/hi: hi.o $(LIB) $(LIBRTL)
	../linker/src/linker -L../linker/src/build/ld -melf_i386 -Bstatic hi.o $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/hi
	ld -Bshareable hi.o $(LIBC) -o i386/hi.exe2

i386/hi.exe: hi.o $(LIB) $(LIBRTL)
	../linker/src/linker $(LINKPRE) hi.o $(LIBGCC) $(LINKPOST) $(LIBGCC) -o i386/hi.exe

allum: um/timer um/ast um/time um/eftim um/schdwk um/forcex um/ef um/wake um/synch um/enq um/enq2 um/lnm um/lnm2 um/qio1 um/qio2 um/cef um/cefw um/mem um/memf um/getjpi um/crec um/getdvi um/inout um/mbx um/creprc

UMDEF=-Dcompile_um

um/timer: timer.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ timer.c ../starlet/src/starletum.a -o um/timer
um/time: gettim.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ gettim.c ../starlet/src/starletum.a -o um/time

um/ast: astdel.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ astdel.c ../starlet/src/starletum.a -o um/ast

um/eftim: eftim.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ eftim.c ../starlet/src/starletum.a -o um/eftim

um/schdwk: schdwk.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ schdwk.c ../starlet/src/starletum.a -o um/schdwk

um/forcex: forcex.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ forcex.c ../starlet/src/starletum.a -o um/forcex

um/ef: ef.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ ef.c ../starlet/src/starletum.a -o um/ef

um/wake: wake.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ wake.c ../starlet/src/starletum.a -o um/wake


um/synch: synch.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ synch.c ../starlet/src/starletum.a -o um/synch


um/enq: enq.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ enq.c ../starlet/src/starletum.a -o um/enq

um/enq2: enq2.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ enq2.c ../starlet/src/starletum.a -o um/enq2

um/lnm: lnm.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ lnm.c ../starlet/src/starletum.a -o um/lnm

um/lnm2: lnm2.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ lnm2.c ../starlet/src/starletum.a -o um/lnm2

um/qio1: qio1.c $(LIBUM)
	gcc -$(FLAGS) ${UMDEF} -I../starlet/src -I../librtl/src/ qio1.c ../starlet/src/starletum.a -o um/qio1

um/qio2: qio2.c $(LIBUM)
	gcc -$(FLAGS) ${UMDEF} -I../starlet/src -I../librtl/src/ qio2.c ../starlet/src/starletum.a -o um/qio2

um/cef: cef.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ cef.c ../starlet/src/starletum.a -o um/cef

um/cefw: cefwait.c $(LIBUM)
	gcc -$(FLAGS) $(UMDEF) -I../starlet/src -I../librtl/src/ cefwait.c ../starlet/src/starletum.a -o um/cefw

um/mem: mem.c $(LIBUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ mem.c ../starlet/src/starletum.a -o um/mem

um/memf: memf.c $(LIBUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ memf.c ../starlet/src/starletum.a -o um/memf

um/getjpi: getjpi.c $(LIBUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ getjpi.c ../starlet/src/starletum.a -o um/getjpi

um/crec: crec.c $(LIBUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ crec.c ../starlet/src/starletum.a -o um/crec

um/getdvi: getdvi.c $(LIBUM)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ getdvi.c ../starlet/src/starletum.a -o um/getdvi

um/inout: inout.c $(LIBUM) $(LIBRTL)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ inout.c $(LIBRTL) ../starlet/src/starletum.a -o um/inout

um/mbx: mbx.c $(LIBUM) $(LIBRTL)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ mbx.c $(LIBRTL) ../starlet/src/starletum.a -o um/mbx

um/creprc: creprc.c $(LIBUM) $(LIBRTL)
	gcc -$(FLAGS) -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ creprc.c $(LIBRTL) ../starlet/src/starletum.a -o um/creprc
