#
# Makefile for the Linux filesystems.
#
# 14 Sep 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
# 

O_TARGET := fs.o

export-objs :=	filesystems.o open.o dcache.o

ifneq ($(CONFIG_IO_VMS),y)
 export-objs += buffer.o
endif

mod-subdirs :=	

obj-y :=	open.o read_write.o devices.o file_table.o \
		super.o block_dev.o char_dev.o stat.o exec.o pipe.o namei.o \
		fcntl.o ioctl.o readdir.o select.o fifo.o locks.o \
		dcache.o inode.o attr.o bad_inode.o file.o iobuf.o dnotify.o \
		filesystems.o namespace.o seq_file.o

ifneq ($(CONFIG_IO_VMS),y)
 obj-y += buffer.o
endif

ifeq ($(CONFIG_QUOTA),y)
obj-y += dquot.o
else
obj-y += noquot.o
endif

subdir-$(CONFIG_PROC_FS)	+= proc
subdir-y			+= partitions

# Do not add any filesystems before this line
ifneq ($(CONFIG_VMS),y)
subdir-$(CONFIG_EXT2_FS)	+= ext2
endif
subdir-$(CONFIG_DEVFS_FS)	+= devfs
subdir-$(CONFIG_DEVPTS_FS)	+= devpts

ifneq ($(CONFIG_VMS),y)
#subdir-y			+= ods2
endif

obj-$(CONFIG_BINFMT_AOUT)	+= binfmt_aout.o
obj-$(CONFIG_BINFMT_EM86)	+= binfmt_em86.o
obj-$(CONFIG_BINFMT_MISC)	+= binfmt_misc.o

# binfmt_script is always there
obj-y				+= binfmt_script.o

obj-$(CONFIG_BINFMT_ELF)	+= binfmt_elf.o

# persistent filesystems
obj-y += $(join $(subdir-y),$(subdir-y:%=/%.o))


include $(TOPDIR)/Rules.make
