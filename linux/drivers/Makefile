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
ifneq ($(CONFIG_VMS),y)
subdir-$(CONFIG_MTD)		+= mtd
endif
subdir-$(CONFIG_VT)		+= video
subdir-$(CONFIG_IDE)		+= ide
#subdir-$(CONFIG_SCSI)		+= scsi

include $(TOPDIR)/Rules.make
