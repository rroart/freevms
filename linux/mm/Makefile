#
# Makefile for the linux memory manager.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definition is now in the main makefile...

O_TARGET := mm.o

obj-y	 := numa.o oom_kill.o

obj-$(CONFIG_HIGHMEM) += highmem.o

include $(TOPDIR)/Rules.make
