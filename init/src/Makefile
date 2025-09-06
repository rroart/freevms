INITINCLUDES = -I../../lib/src -I../../sys/src -I../../starlet/src

CFLAGS_OTHER := -Wno-error=incompatible-pointer-types -Wno-error=int-conversion \
		  -Wno-error=implicit-int -Wno-error=declaration-missing-parameter-type \
		  -Wno-error=return-mismatch -Wno-error=implicit-function-declaration

all: init

clean:
	rm *.o init

init: initialize.o utils.o utils2.o
	gcc -o init initialize.o utils.o utils2.o

utils.o: utils.c
	gcc $(CFLAGS_OTHER) $(INITINCLUDES) -c utils.c

utils2.o: utils2.c
	gcc $(CFLAGS_OTHER) $(INITINCLUDES) -c utils2.c

initialize.o: initialize.c
	gcc $(CFLAGS_OTHER) $(INITINCLUDES) -c initialize.c

install: all
	install init ../../rooti386/vms"$$"common/sysexe
