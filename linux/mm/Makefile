#
# Makefile for the linux memory manager.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definition is now in the main makefile...

O_TARGET := mm.o

export-objs := shmem.o filemap.o

obj-y	 := memory.o mmap.o filemap.o mprotect.o mlock.o mremap.o \
	    vmalloc.o slab.o swap.o vmscan.o page_io.o \
	    swap_state.o swapfile.o numa.o oom_kill.o \
	    shmem.o

obj-$(CONFIG_HIGHMEM) += highmem.o

ifneq ($(CONFIG_MM_VMS),y)
obj-y += page_alloc.o bootmem.o 
endif

obj-$(CONFIG_MM_VMS) += memoryalc.o vmsbootmem.o allocpfn.o ptalloc.o

include $(TOPDIR)/Rules.make
