# $Id$
# $Makefile$

all: libqueue.a

libqueue.a: queue.o
	ar cr $@ queue.o
	ranlib $@ 

queue.o: queue.c
	cc -c queue.c
