all: timer ast time

timer: timer.c
	gcc -I../starlet/src -I../librtl/src/ timer.c ../starlet/src/starlet.o -o timer
time: gettim.c
	gcc -I../starlet/src -I../librtl/src/ gettim.c ../starlet/src/starlet.o -o time

ast: astdel.c
	gcc -I../starlet/src -I../librtl/src/ astdel.c ../starlet/src/starlet.o -o ast

