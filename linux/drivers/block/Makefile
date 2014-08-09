#
# Makefile for the kernel block device drivers.
#
# 12 June 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
# 
# Note : at this point, these files are compiled on all systems.
# In the future, some of these should be built conditionally.
#

O_TARGET := block.o

export-objs	:= blkpg.o genhd.o

obj-y	:= blkpg.o genhd.o vmsblock.o

ifeq ($(ARCH),i386)
#obj-y				+= pvdriver.o
endif

include $(TOPDIR)/Rules.make
