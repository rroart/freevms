all: loginout

clean: 
	rm *.o loginout

login.o: login.c
	gcc -g -c login.c

loginout: login.o 
	gcc -g -static -o $@ $^

install: all
	install loginout ../../rootum/vms"$$"common/sysexe
	install loginout ../../rooti386/vms"$$"common/sysexe
