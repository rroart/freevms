#
# Makefile for the linux memory manager.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definition is now in the main makefile...

O_TARGET := mm.o

ifneq ($(CONFIG_VMS),y)
export-objs := shmem.o filemap.o
endif

obj-y	 := numa.o oom_kill.o

obj-$(CONFIG_HIGHMEM) += highmem.o

ifneq ($(CONFIG_VMS),y)
obj-y += page_alloc.o bootmem.o slab.o vmscan.o mmap.o \
	 filemap.o memory.o mremap.o page_io.o \
	 mprotect.o mlock.o vmalloc.o shmem.o swapfile.o swap.o swap_state.o
endif

include $(TOPDIR)/Rules.make
