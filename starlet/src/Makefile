all: vmsopt.a
	cc -c starlet.c
	cc -c -I../../starlet/src rms.c
	ar q starlet.a starlet.o rms.o
	ranlib starlet.a

vmsopt.a:	vmsopt.o
		rm vmsopt.a; ar q vmsopt.a vmsopt.o; ranlib vmsopt.a


vmsopt.o:	vmsopt.c
		cc -O2 -o vmsopt.o -c vmsopt.c

