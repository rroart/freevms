# $Id$
# $Locker$

all: setproces show.a

setproces: setproces.o
	gcc -o setproces setproces.o ../../starlet/src/vmsopt.a ../../starlet/src/starlet.a

setproces.o: setproces.c
	gcc -c setproces.c

show.a: showsystem.o showdevice.o
	ar q show.a showsystem.o showdevice.o

showsystem.o: showsystem.c
	gcc -I../../starlet/src -I../../librtl/src/ -c showsystem.c

showdevice.o: showdevice.c
	gcc -I../../starlet/src -I../../librtl/src/ -c showdevice.c
