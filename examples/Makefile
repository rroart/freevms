all: dirs alli386 allum

alli386: i386/timer i386/ast i386/time i386/eftim i386/schdwk i386/forcex i386/ef i386/wake i386/synch i386/enq i386/lnm i386/lnm2 i386/qio1 i386/qio2 i386/cef i386/cefw i386/mem i386/memf i386/getjpi i386/crec

dirs:
	mkdir um || echo um exists
	mkdir i386 || echo i386 exists

LIB = ../starlet/src/starlet.a
LIBUM = ../starlet/src/starletum.a

i386/timer: timer.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ timer.c ../starlet/src/starlet.a -o i386/timer
i386/time: gettim.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ gettim.c ../starlet/src/starlet.a -o i386/time

i386/ast: astdel.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ astdel.c ../starlet/src/starlet.a -o i386/ast

i386/eftim: eftim.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ eftim.c ../starlet/src/starlet.a -o i386/eftim

i386/schdwk: schdwk.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ schdwk.c ../starlet/src/starlet.a -o i386/schdwk

i386/forcex: forcex.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ forcex.c ../starlet/src/starlet.a -o i386/forcex

i386/ef: ef.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ ef.c ../starlet/src/starlet.a -o i386/ef

i386/wake: wake.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ wake.c ../starlet/src/starlet.a -o i386/wake


i386/synch: synch.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ synch.c ../starlet/src/starlet.a -o i386/synch


i386/enq: enq.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ enq.c ../starlet/src/starlet.a -o i386/enq

i386/lnm: lnm.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ lnm.c ../starlet/src/starlet.a -o i386/lnm

i386/lnm2: lnm2.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ lnm2.c ../starlet/src/starlet.a -o i386/lnm2

i386/qio1: qio1.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ qio1.c ../starlet/src/starlet.a -o i386/qio1

i386/qio2: qio2.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ qio2.c ../starlet/src/starlet.a -o i386/qio2

i386/cef: cef.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ cef.c ../starlet/src/starlet.a -o i386/cef

i386/cefw: cefwait.c $(LIB)
	gcc -I../starlet/src -I../librtl/src/ cefwait.c ../starlet/src/starlet.a -o i386/cefw

i386/mem: mem.c $(LIB)
	gcc -static -I../starlet/src -I../librtl/src/ mem.c ../starlet/src/starlet.a -o i386/mem

i386/memf: memf.c $(LIB)
	gcc -static -I../starlet/src -I../librtl/src/ memf.c ../starlet/src/starlet.a -o i386/memf

i386/getjpi: getjpi.c $(LIB)
	gcc -static -I../starlet/src -I../librtl/src/ getjpi.c ../starlet/src/starlet.a -o i386/getjpi

i386/crec: crec.c $(LIB)
	gcc -static -I../starlet/src -I../librtl/src/ crec.c ../starlet/src/starlet.a -o i386/crec

allum: um/timer um/ast um/time um/eftim um/schdwk um/forcex um/ef um/wake um/synch um/enq um/lnm um/lnm2 um/qio1 um/qio2 um/cef um/cefw um/mem um/memf um/getjpi um/crec

UMDEF=-Dcompile_um

um/timer: timer.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ timer.c ../starlet/src/starletum.a -o um/timer
um/time: gettim.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ gettim.c ../starlet/src/starletum.a -o um/time

um/ast: astdel.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ astdel.c ../starlet/src/starletum.a -o um/ast

um/eftim: eftim.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ eftim.c ../starlet/src/starletum.a -o um/eftim

um/schdwk: schdwk.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ schdwk.c ../starlet/src/starletum.a -o um/schdwk

um/forcex: forcex.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ forcex.c ../starlet/src/starletum.a -o um/forcex

um/ef: ef.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ ef.c ../starlet/src/starletum.a -o um/ef

um/wake: wake.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ wake.c ../starlet/src/starletum.a -o um/wake


um/synch: synch.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ synch.c ../starlet/src/starletum.a -o um/synch


um/enq: enq.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ enq.c ../starlet/src/starletum.a -o um/enq

um/lnm: lnm.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ lnm.c ../starlet/src/starletum.a -o um/lnm

um/lnm2: lnm2.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ lnm2.c ../starlet/src/starletum.a -o um/lnm2

um/qio1: qio1.c $(LIBUM)
	gcc ${UMDEF} -I../starlet/src -I../librtl/src/ qio1.c ../starlet/src/starletum.a -o um/qio1

um/qio2: qio2.c $(LIBUM)
	gcc ${UMDEF} -I../starlet/src -I../librtl/src/ qio2.c ../starlet/src/starletum.a -o um/qio2

um/cef: cef.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ cef.c ../starlet/src/starletum.a -o um/cef

um/cefw: cefwait.c $(LIBUM)
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ cefwait.c ../starlet/src/starletum.a -o um/cefw

um/mem: mem.c $(LIBUM)
	gcc -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ mem.c ../starlet/src/starletum.a -o um/mem

um/memf: memf.c $(LIBUM)
	gcc -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ memf.c ../starlet/src/starletum.a -o um/memf

um/getjpi: getjpi.c $(LIBUM)
	gcc -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ getjpi.c ../starlet/src/starletum.a -o um/getjpi


um/crec: crec.c $(LIBUM)
	gcc -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ crec.c ../starlet/src/starletum.a -o um/crec

