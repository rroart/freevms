# $Id$
# $Locker$

all: set.a show.a

setproces: setproces.o
	gcc -o setproces setproces.o ../../starlet/src/vmsopt.a ../../starlet/src/starlet.a

set.a: setproces.o setwork.o
	ar r set.a setproces.o setwork.o

show.a: showsystem.o showdevice.o showwork.o
	ar r show.a showsystem.o showdevice.o showwork.o

setproces.o: setproces.c
	gcc -I../../starlet/src -I../../librtl/src/ -c setproces.c

setwork.o: setwork.c
	gcc -I../../starlet/src -I../../librtl/src/ -c setwork.c

showsystem.o: showsystem.c
	gcc -I../../starlet/src -I../../librtl/src/ -c showsystem.c

showdevice.o: showdevice.c
	gcc -I../../starlet/src -I../../librtl/src/ -c showdevice.c

showwork.o: showwork.c
	gcc -I../../starlet/src -I../../librtl/src/ -c showwork.c
