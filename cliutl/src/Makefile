# $Id$
# $Locker$

all: setproces

setproces: setproces.o
	cc -o setproces setproces.o ../../starlet/src/vmsopt.a ../../starlet/src/starlet.o

setproces.o: setproces.c
	cc -c setproces.c

userland:
	cc -I. -I.. lnmsub.c syslnm.c system_data_cells.c ../../pal/src/libqueue.a -o test -g -DUSERLAND

