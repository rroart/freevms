all: alli386 allum

alli386: timer ast time eftim schdwk forcex ef wake synch enq lnm lnm2 qio1 qio2 cef cefw mem memf getjpi crec

timer: timer.c
	gcc -I../starlet/src -I../librtl/src/ timer.c ../starlet/src/starlet.a -o timer
time: gettim.c
	gcc -I../starlet/src -I../librtl/src/ gettim.c ../starlet/src/starlet.a -o time

ast: astdel.c
	gcc -I../starlet/src -I../librtl/src/ astdel.c ../starlet/src/starlet.a -o ast

eftim: eftim.c
	gcc -I../starlet/src -I../librtl/src/ eftim.c ../starlet/src/starlet.a -o eftim

schdwk: schdwk.c
	gcc -I../starlet/src -I../librtl/src/ schdwk.c ../starlet/src/starlet.a -o schdwk

forcex: forcex.c
	gcc -I../starlet/src -I../librtl/src/ forcex.c ../starlet/src/starlet.a -o forcex

ef: ef.c
	gcc -I../starlet/src -I../librtl/src/ ef.c ../starlet/src/starlet.a -o ef

wake: wake.c
	gcc -I../starlet/src -I../librtl/src/ wake.c ../starlet/src/starlet.a -o wake


synch: synch.c
	gcc -I../starlet/src -I../librtl/src/ synch.c ../starlet/src/starlet.a -o synch


enq: enq.c
	gcc -I../starlet/src -I../librtl/src/ enq.c ../starlet/src/starlet.a -o enq

lnm: lnm.c
	gcc -I../starlet/src -I../librtl/src/ lnm.c ../starlet/src/starlet.a -o lnm

lnm2: lnm2.c
	gcc -I../starlet/src -I../librtl/src/ lnm2.c ../starlet/src/starlet.a -o lnm2

qio1: qio1.c
	gcc -I../starlet/src -I../librtl/src/ qio1.c ../starlet/src/starlet.a -o qio1

qio2: qio2.c
	gcc -I../starlet/src -I../librtl/src/ qio2.c ../starlet/src/starlet.a -o qio2

cef: cef.c
	gcc -I../starlet/src -I../librtl/src/ cef.c ../starlet/src/starlet.a -o cef

cefw: cefwait.c
	gcc -I../starlet/src -I../librtl/src/ cefwait.c ../starlet/src/starlet.a -o cefw

mem: mem.c
	gcc -static -I../starlet/src -I../librtl/src/ mem.c ../starlet/src/starlet.a -o mem

memf: memf.c
	gcc -static -I../starlet/src -I../librtl/src/ memf.c ../starlet/src/starlet.a -o memf

getjpi: getjpi.c
	gcc -static -I../starlet/src -I../librtl/src/ getjpi.c ../starlet/src/starlet.a -o getjpi

crec: crec.c
	gcc -static -I../starlet/src -I../librtl/src/ crec.c ../starlet/src/starlet.a -o crec

allum: timerum astum timeum eftimum schdwkum forcexum efum wakeum synchum enqum lnmum lnm2um qio1um qio2um cefum cefwum memfum getjpium crecum

UMDEF=-Dcompile_um

timerum: timer.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ timer.c ../starlet/src/starletum.a -o timerum
timeum: gettim.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ gettim.c ../starlet/src/starletum.a -o timeum

astum: astdel.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ astdel.c ../starlet/src/starletum.a -o astum

eftimum: eftim.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ eftim.c ../starlet/src/starletum.a -o eftimum

schdwkum: schdwk.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ schdwk.c ../starlet/src/starletum.a -o schdwkum

forcexum: forcex.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ forcex.c ../starlet/src/starletum.a -o forcexum

efum: ef.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ ef.c ../starlet/src/starletum.a -o efum

wakeum: wake.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ wake.c ../starlet/src/starletum.a -o wakeum


synchum: synch.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ synch.c ../starlet/src/starletum.a -o synchum


enqum: enq.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ enq.c ../starlet/src/starletum.a -o enqum

lnmum: lnm.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ lnm.c ../starlet/src/starletum.a -o lnmum

lnm2um: lnm2.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ lnm2.c ../starlet/src/starletum.a -o lnm2um

qio1um: qio1.c
	gcc ${UMDEF} -I../starlet/src -I../librtl/src/ qio1.c ../starlet/src/starletum.a -o qio1um

qio2um: qio2.c
	gcc ${UMDEF} -I../starlet/src -I../librtl/src/ qio2.c ../starlet/src/starletum.a -o qio2um

cefum: cef.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ cef.c ../starlet/src/starletum.a -o cefum

cefwum: cefwait.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ cefwait.c ../starlet/src/starletum.a -o cefwum

memum: mem.c
	gcc -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ mem.c ../starlet/src/starletum.a -o memum

memfum: memf.c
	gcc -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ memf.c ../starlet/src/starletum.a -o memfum

getjpium: getjpi.c
	gcc -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ getjpi.c ../starlet/src/starletum.a -o getjpium


crecum: crec.c
	gcc -static -s $(UMDEF) -I../starlet/src -I../librtl/src/ crec.c ../starlet/src/starletum.a -o crecum

