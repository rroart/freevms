# Makefile for the Linux video drivers.
# 5 Aug 1999, James Simmons, <mailto:jsimmons@edgeglobal.com>
# Rewritten to use lists instead of if-statements.

O_TARGET := video.o

mod-subdirs	:= matrox

# All of the (potential) objects that export symbols.
# This list comes from 'grep -l EXPORT_SYMBOL *.[hc]'.

export-objs    := 

# Each configuration option enables a list of files.

obj-$(CONFIG_DUMMY_CONSOLE)       += dummycon.o
obj-$(CONFIG_SGI_NEWPORT_CONSOLE) += newport_con.o
obj-$(CONFIG_PROM_CONSOLE)        += promcon.o promcon_tbl.o
obj-$(CONFIG_STI_CONSOLE)         += sticon.o sticon-bmode.o sticore.o
obj-$(CONFIG_VGA_CONSOLE)         += vgacon.o

obj-$(CONFIG_FONT_SUN8x16)        += font_sun8x16.o
obj-$(CONFIG_FONT_SUN12x22)       += font_sun12x22.o
obj-$(CONFIG_FONT_8x8)            += font_8x8.o
obj-$(CONFIG_FONT_8x16)           += font_8x16.o
obj-$(CONFIG_FONT_6x11)           += font_6x11.o
obj-$(CONFIG_FONT_PEARL_8x8)      += font_pearl_8x8.o
obj-$(CONFIG_FONT_ACORN_8x8)      += font_acorn_8x8.o

include $(TOPDIR)/Rules.make

clean:
	rm -f core *.o *.a *.s

../conmakehash: ../conmakehash.c
	$(HOSTCC) $(HOSTCFLAGS) -o ../conmakehash ../conmakehash.c

promcon_tbl.c: prom.uni ../char/conmakehash
	../char/conmakehash prom.uni | \
	sed -e '/#include <[^>]*>/p' -e 's/types/init/' \
	    -e 's/dfont\(_uni.*\]\)/promfont\1 __initdata/' > promcon_tbl.c

promcon_tbl.o: promcon_tbl.c $(TOPDIR)/include/linux/types.h

