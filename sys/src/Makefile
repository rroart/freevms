# $Id$
# $Locker$

all: set_process

set_process: set_process.o
	cc -o set_process set_process.o ../../lbr/src/vmsopt.a ../../starlet/src/starlet.o

set_process.o: set_process.c
	cc -c set_process.c

userland:
	cc -I. -I.. lnmsub.c syslnm.c system_data_cells.c ../../lbr/src/libqueue.a -o test -g -DUSERLAND

