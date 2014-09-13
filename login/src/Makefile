all: loginout

clean: 
	rm *.o loginout

loginout: login.o 
	gcc -g -static -o $@ $^

install: all
	install loginout ../../rooti386/vms"$$"common/sysexe

include $(TOPDIR)/Rules.make.freevms
