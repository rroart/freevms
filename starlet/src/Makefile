all: starlet.a starletum.a

starlet.a: vmsopt.a starlet.c starlet.h
	gcc -c starlet.c
	gcc -Dcompile_um -c starlet.c -o starletum.o
	gcc -c -I../../starlet/src rms.c
	ar r starlet.a starlet.o rms.o
	ar r starletum.a starletum.o rms.o
	ranlib starlet.a
	ranlib starletum.a

vmsopt.a:	vmsopt.o
		rm vmsopt.a; ar q vmsopt.a vmsopt.o; ranlib vmsopt.a


vmsopt.o:	vmsopt.c
		gcc -O2 -o vmsopt.o -c vmsopt.c

