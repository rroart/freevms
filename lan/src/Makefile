O_TARGET := lan.o

export-objs := 8390.o

obj-y := common.o

obj-$(CONFIG_NE2000) += 8390.o ne.o
obj-$(CONFIG_VORTEX) += 3c59x.o mii.o

include $(TOPDIR)/Rules.make
