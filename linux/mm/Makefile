#
# Makefile for the linux memory manager.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definition is now in the main makefile...

O_TARGET := mm.o

ifneq ($(CONFIG_MM_VMS),y)
export-objs := shmem.o filemap.o
endif

ifeq ($(CONFIG_MM_VMS),y)
export-objs := vmsshmem.o vmsfilemap.o
endif

obj-y	 := page_io.o \
	    numa.o oom_kill.o \
	    swap.o swap_state.o

obj-$(CONFIG_HIGHMEM) += highmem.o

ifneq ($(CONFIG_MM_VMS),y)
obj-y += page_alloc.o bootmem.o slab.o vmscan.o mmap.o \
	 filemap.o memory.o mremap.o \
	 mprotect.o mlock.o vmalloc.o shmem.o swapfile.o
endif

obj-$(CONFIG_MM_VMS) += memoryalc.o vmsbootmem.o allocpfn.o ptalloc.o \
			vmsslab.o swapper.o wrtmfypag.o \
			vmsmmap.o vmsfilemap.o vmsmemory.o vmsmremap.o \
	 		vmsmprotect.o vmsmlock.o vmsvmalloc.o \
			vmsshmem.o vmsswapfile.o

include $(TOPDIR)/Rules.make
