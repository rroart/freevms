INITINCLUDES = -I../../lib/src -I../../sys/src -I../../starlet/src

all: init

clean:
	rm *.o init

init: initialize.o utils.o utils2.o
	gcc -o init initialize.o utils.o utils2.o

utils.o: utils.c
	gcc $(INITINCLUDES) -c utils.c

utils2.o: utils2.c
	gcc $(INITINCLUDES) -c utils2.c

initialize.o: initialize.c
	gcc $(INITINCLUDES) -c initialize.c

install: all
	install init ../../rooti386/vms"$$"common/sysexe
