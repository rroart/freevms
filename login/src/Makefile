all: loginout

login.o: login.c
	gcc -g -c login.c

loginout: login.o 
	gcc -g -static -o $@ $^

install: all
	cp -p loginout ../../rootum/vms"$$"common/sysexe
	cp -p loginout ../../rooti386/vms"$$"common/sysexe
