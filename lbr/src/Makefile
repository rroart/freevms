# $Id$
# $Makefile$

all: libqueue.a

libqueue.a: queue.o
	ar cr libqueue.a queue.o
	ranlib libqueue.a

queue.o:
	cc -c queue.c
