all:
	cc -c starlet.c

vmsopt.a:	vmsopt.o
		rm vmsopt.a; ar q vmsopt.a vmsopt.o; ranlib vmsopt.a


vmsopt.o:	vmsopt.c
		cc -O2 -o vmsopt.o -c vmsopt.c

