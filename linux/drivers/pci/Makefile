#
# Makefile for the PCI bus specific drivers.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definition is now inherited from the
# parent makefile.
#

O_TARGET := driver.o

export-objs := pci.o

obj-$(CONFIG_PCI) += pci.o quirks.o compat.o names.o setup-res.o

include $(TOPDIR)/Rules.make

names.o: names.c devlist.h classlist.h

devlist.h classlist.h: pci.ids gen-devlist
	./gen-devlist <pci.ids

gen-devlist: gen-devlist.c
	$(HOSTCC) $(HOSTCFLAGS) -o gen-devlist gen-devlist.c
