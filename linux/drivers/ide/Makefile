#
# Makefile for the kernel ata, atapi, and ide block device drivers.
#
# 12 September 2000, Bartlomiej Zolnierkiewicz <bkz@linux-ide.org>
# Rewritten to use lists instead of if-statements.
#
# Note : at this point, these files are compiled on all systems.
# In the future, some of these should be built conditionally.
#

O_TARGET := idedriver.o

export-objs             := myide.o ide-features.o

list-multi		:= ide-mod.o ide-probe-mod.o

obj-y		:=
obj-m		:=
ide-obj-y	:=

obj-y += myide.o myide-disk.o

# this one is not enabled in config, difficult to integrate yet
obj-$(CONFIG_BLK_DEV_IDE_VMS)       += dqdriver.o enable-ide.o 

obj-$(CONFIG_BLK_DEV_IDE)       += ide-mod.o ide-probe-mod.o

ide-obj-$(CONFIG_BLK_DEV_IDEDMA_PCI)	+= myide-dma.o
ide-obj-$(CONFIG_BLK_DEV_IDEPCI)	+= ide-pci.o
ide-obj-$(CONFIG_BLK_DEV_PIIX)		+= piix.o

ide-mod-objs            := ide-features.o $(ide-obj-y)
ide-probe-mod-objs	:= ide-probe.o ide-geometry.o

include $(TOPDIR)/Rules.make

ide-mod.o: $(ide-mod-objs)
	$(LD) -r -o $@ $(ide-mod-objs)

ide-probe-mod.o: $(ide-probe-mod-objs)
	$(LD) -r -o $@ $(ide-probe-mod-objs)
