SUBDIRS = starlet librtl cliutl backup dir pal dcl
SUBDIRSINST = dir dcl
SUBDIRS2 = examples

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

install:
	for X in $(SUBDIRSINST); do \
		cd $$X/src; \
		make install; \
		cd ../..; \
	done; \
	for X in $(SUBDIRS2); do \
		cd $$X; \
		make install; \
		cd ..; \
	done; 
