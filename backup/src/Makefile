#
#
REMOTE=#-DREMOTE			# -DREMOTE  use remote tape
CFLAGS= $(REMOTE) -g
LFLAGS=
LIBS= #-lrmt   			# remote magtape library
OWNER=tar			# user for remote tape access
MODE=4755
BINDIR=/usr/local/bin
MANSEC=l
MANDIR=/usr/man/man$(MANSEC)
DISTFILES=README vmsbackup.1 Makefile vmsbackup.c match.c NEWS \
	build.com dclmain.c getoptmain.c vmsbackup.cld vmsbackup.h \
	sysdep.h

#
vmsbackup: vmsbackup.o match.o getoptmain.o
	cc $(LFLAGS) -o vmsbackup vmsbackup.o match.o getoptmain.o $(LIBS)
install:
	install -m $(MODE) -o $(OWNER) -s vmsbackup $(BINDIR)	
	cp vmsbackup.1 $(MANDIR)/vmsbackup.$(MANSEC)
clean:
	rm -f vmsbackup *.o core
shar:
	shar -a $(DISTFILES) > vmsbackup.shar
dist:
	rm -rf vmsbackup-dist
	mkdir vmsbackup-dist
	for i in $(DISTFILES); do \
	  ln $${i} vmsbackup-dist; \
	done
	tar chf vmsbackup.tar vmsbackup-dist
