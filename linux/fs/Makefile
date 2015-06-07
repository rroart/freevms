#
# Makefile for the Linux filesystems.
#
# 14 Sep 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
# 

O_TARGET := fs.o

export-objs :=	open.o dcache.o

mod-subdirs :=	

obj-y :=	open.o read_write.o file_table.o \
		super.o block_dev.o stat.o exec.o namei.o \
		fcntl.o ioctl.o readdir.o select.o \
		dcache.o file.o iobuf.o \
		namespace.o seq_file.o

obj-y += noquot.o

subdir-y			+= partitions

# Do not add any filesystems before this line

obj-$(CONFIG_BINFMT_ELF)	+= binfmt_elf.o

# persistent filesystems
obj-y += $(join $(subdir-y),$(subdir-y:%=/%.o))

include $(TOPDIR)/Rules.make
