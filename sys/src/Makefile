# $Id$
# $Locker$

all: set_process

set_process: set_process.o
	cc -o set_process set_process.o ../../starlet/src/vmsopt.a ../../starlet/src/starlet.o

set_process.o: set_process.c
	cc -c set_process.c

userland:
	cc -I. -I.. lnmsub.c syslnm.c system_data_cells.c ../../pal/src/libqueue.a -o test -g -DUSERLAND

