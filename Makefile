SUBDIRS = starlet librtl cliutl backup dir pal
SUBDIRS2 = examples dcl

all:
	for X in $(SUBDIRS); do \
		cd $$X/src; \
		make; \
		cd ../..; \
	done; \
	for X in $(SUBDIRS2); do \
		cd $$X; \
		make; \
		cd ..; \
	done; 
