# $Id$
# $Locker$

INC= -I. -I../../pal/src -I../../lib/src -I../../librtl/src -I../../starlet/src

all: userland

set_process: set_process.o
	cc -o set_process set_process.o ../../starlet/src/vmsopt.a ../../starlet/src/starlet.o

set_process.o: set_process.c
	cc -c set_process.c $(INC)

userland:
	cc $(INC) lnmsub.c syslnm.c ../../pal/src/libqueue.a -o test -g -DUSERLAND

