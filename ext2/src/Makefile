#
# Makefile for the linux ext2-filesystem routines.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

O_TARGET := exttwo.o

obj-y    := balloc.o bitmap.o dir.o fsync.o ialloc.o inode.o \
		super.o ext2vms.o ext2access.o ext2direct.o ext2update.o 
obj-m    := $(O_TARGET)

include $(TOPDIR)/Rules.make
