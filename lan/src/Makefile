O_TARGET := lan.o

export-objs := 8390.o mii.o

obj-y := common.o

obj-$(CONFIG_NE2000) += 8390.o ne.o
obj-$(CONFIG_NE2000) += ne2k-pci.o
#workaround for adding pci ne2k
obj-$(CONFIG_VORTEX) += 3c59x.o mii.o

ifeq ($(CONFIG_E1000),y)
  obj-y += e1000/e1000.o
endif

subdir-$(CONFIG_E1000) += e1000

include $(TOPDIR)/Rules.make
