
all: init

init: initialize.o utils.o utils2.o
	gcc -o init initialize.o utils.o utils2.o

utils.o: utils.c
	gcc -c utils.c

utils2.o: utils2.c
	gcc -c utils2.c

initialize.o: initialize.c
	gcc -c initialize.c

install: all
	cp -p init ../../rootum/vms"$$"common/sysexe
	cp -p init ../../rooti386/vms"$$"common/sysexe
