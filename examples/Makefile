all: timer ast time eftim schdwk forcex ef wake synch enq

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

