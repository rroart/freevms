# $Id$
# $Makefile$

all: libqueue.a

libqueue.a: queue.o
	ar cr lib.a queue.o
	ranlib lib.a

queue.o:
	cc -c queue.c
