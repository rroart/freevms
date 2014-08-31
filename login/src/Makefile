all: loginout

clean: 
	rm *.o loginout

login.o: login.c
	gcc -g -c ${OLDINCLUDE} login.c

loginout: login.o 
	gcc -g -static -o $@ $^

install: all
	install loginout ../../rooti386/vms"$$"common/sysexe
