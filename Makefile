VMSSUBDIRS = starlet librtl cliutl backup dir init dcl
VMSSUBDIRSINST = dir dcl init
VMSSUBDIRS2 = examples

vmsall:
	for X in $(VMSSUBDIRS); do \
		cd $$X/src; \
		make; \
		cd ../..; \
	done; \
	for X in $(VMSSUBDIRS2); do \
		cd $$X; \
		make; \
		cd ..; \
	done;

rootinstall:
	for X in $(VMSSUBDIRSINST); do \
		cd $$X/src; \
		make install; \
		cd ../..; \
	done; \
	for X in $(VMSSUBDIRS2); do \
		cd $$X; \
		make install; \
		cd ..; \
	done; 

include Makefile.kernel
