all: timer ast time eftim schdwk forcex ef wake synch enq lnm

timer: timer.c
	gcc -I../starlet/src -I../librtl/src/ timer.c ../starlet/src/starlet.o -o timer
time: gettim.c
	gcc -I../starlet/src -I../librtl/src/ gettim.c ../starlet/src/starlet.o -o time

ast: astdel.c
	gcc -I../starlet/src -I../librtl/src/ astdel.c ../starlet/src/starlet.o -o ast

eftim: eftim.c
	gcc -I../starlet/src -I../librtl/src/ eftim.c ../starlet/src/starlet.o -o eftim

schdwk: schdwk.c
	gcc -I../starlet/src -I../librtl/src/ schdwk.c ../starlet/src/starlet.o -o schdwk

forcex: forcex.c
	gcc -I../starlet/src -I../librtl/src/ forcex.c ../starlet/src/starlet.o -o forcex

ef: ef.c
	gcc -I../starlet/src -I../librtl/src/ ef.c ../starlet/src/starlet.o -o ef

wake: wake.c
	gcc -I../starlet/src -I../librtl/src/ wake.c ../starlet/src/starlet.o -o wake


synch: synch.c
	gcc -I../starlet/src -I../librtl/src/ synch.c ../starlet/src/starlet.o -o synch


enq: enq.c
	gcc -I../starlet/src -I../librtl/src/ enq.c ../starlet/src/starlet.o -o enq

lnm: lnm.c
	gcc -I../starlet/src -I../librtl/src/ lnm.c ../starlet/src/starlet.o -o lnm

allum: timerum astum timeum eftimum schdwkum forcexum efum wakeum synchum enqum lnmum

UMDEF=-Dcompile_um -I.

timerum: timer.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ timer.c ../starlet/src/starlet.c -o timerum
timeum: gettim.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ gettim.c ../starlet/src/starlet.c -o timeum

astum: astdel.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ astdel.c ../starlet/src/starlet.c -o astum

eftimum: eftim.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ eftim.c ../starlet/src/starlet.c -o eftimum

schdwkum: schdwk.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ schdwk.c ../starlet/src/starlet.c -o schdwkum

forcexum: forcex.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ forcex.c ../starlet/src/starlet.c -o forcexum

efum: ef.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ ef.c ../starlet/src/starlet.c -o efum

wakeum: wake.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ wake.c ../starlet/src/starlet.c -o wakeum


synchum: synch.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ synch.c ../starlet/src/starlet.c -o synchum


enqum: enq.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ enq.c ../starlet/src/starlet.c -o enqum

lnmum: lnm.c
	gcc $(UMDEF) -I../starlet/src -I../librtl/src/ lnm.c ../starlet/src/starlet.c -o lnmum

