# Makefile for the Linux video drivers.
# 5 Aug 1999, James Simmons, <mailto:jsimmons@edgeglobal.com>
# Rewritten to use lists instead of if-statements.

O_TARGET := video.o

# All of the (potential) objects that export symbols.
# This list comes from 'grep -l EXPORT_SYMBOL *.[hc]'.

export-objs    := 

# Each configuration option enables a list of files.

obj-$(CONFIG_VGA_CONSOLE)         += vgacon.o

include $(TOPDIR)/Rules.make

clean:
	rm -f core *.o *.a *.s

../conmakehash: ../conmakehash.c
	$(HOSTCC) $(HOSTCFLAGS) -o ../conmakehash ../conmakehash.c
