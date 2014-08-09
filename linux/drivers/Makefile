#
# Makefile for the Linux kernel device drivers.
#
# 15 Sep 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
#


mod-subdirs :=	mtd video input ide

subdir-y :=	char block net
subdir-m :=	$(subdir-y)


subdir-$(CONFIG_PCI)		+= pci
subdir-$(CONFIG_VT)		+= video
subdir-$(CONFIG_IDE)		+= ide

include $(TOPDIR)/Rules.make
