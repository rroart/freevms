# $Id$
# $Makefile$

all: libqueue.a

libqueue.a: queue.o
	ar cr libqueue.a queue.o
	ranlib libqueue.a

queue.o:
	cc -c queue.c

vmsopt.a:	vmsopt.o
		rm vmsopt.a; ar q vmsopt.a vmsopt.o; ranlib vmsopt.a


vmsopt.o:	vmsopt.c
		cc -O2 -o vmsopt.o -c vmsopt.c

