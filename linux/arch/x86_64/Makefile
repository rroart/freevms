#
# x86_64/Makefile
#
# This file is included by the global makefile so that you can add your own
# architecture-specific flags and dependencies. Remember to do have actions
# for "archclean" and "archdep" for cleaning up and making dependencies for
# this architecture
#
# This file is subject to the terms and conditions of the GNU General Public
# License.  See the file "COPYING" in the main directory of this archive
# for more details.
#
# Copyright (C) 1994 by Linus Torvalds
#
# 19990713  Artur Skawina <skawina@geocities.com>
#           Added '-march' and '-mpreferred-stack-boundary' support
# 20000913  Pavel Machek <pavel@suse.cz>
#	    Converted for x86_64 architecture
# 20010105  Andi Kleen, add IA32 compiler.
#           ....and later removed it again....
#
# $Id$

#
# early bootup linking needs 32bit. You can either use real 32bit tools
# here or 64bit tools switch to 32bit mode.
# 
IA32_CC := $(CROSS_COMPILE)gcc -m32 -O2 -fomit-frame-pointer -nostdinc -I $(HPATH)
IA32_LD := $(CROSS_COMPILE)ld -m elf_i386
IA32_AS := $(CROSS_COMPILE)gcc -m32 -Wa,--32 -D__ASSEMBLY__ -traditional -c -nostdinc -I $(HPATH) 
IA32_OBJCOPY := $(CROSS_COMPILE)objcopy
IA32_CPP := $(CROSS_COMPILE)gcc -m32 -E
export IA32_CC IA32_LD IA32_AS IA32_OBJCOPY IA32_CPP


LD=$(CROSS_COMPILE)ld -m elf_x86_64
OBJCOPY=$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -S
LDFLAGS=-e stext
LINKFLAGS =-T $(TOPDIR)/linux/arch/x86_64/vmlinux.lds $(LDFLAGS)

# Linux 3.16 uses these
CFLAGS += $(call check_gcc,-funit-at-a-time,)
CFLAGS += $(call check_gcc,-mno-sse -mno-mmx -mno-sse2 -mno-3dnow,)
CFLAGS += $(call check_gcc,-mno-avx,)
CFLAGS += $(call check_gcc,-maccumulate-outgoing-args,)
CFLAGS += $(call check_gcc,-mno-80387,)
CFLAGS += $(call check_gcc,-mno-fp-ret-in-387,)
CFLAGS += $(call check_gcc,-mpreferred-stack-boundary=3,)
# Note that the line with no-sse etc was critical for gcc-4 usage

CFLAGS += -mno-red-zone
CFLAGS += -mcmodel=kernel
CFLAGS += -pipe
CFLAGS += -fno-pie
CFLAGS += -include $(TOPDIR)/linux/include/linux/compiler_types.h
#CFLAGS += -fno-reorder-blocks
# needed for later gcc 3.1
# not in 3.16
#CFLAGS += -finline-limit=2000
# needed for earlier gcc 3.1
# not in 3.16
#CFLAGS += -fno-strength-reduce
CFLAGS += -Wno-sign-compare
#CFLAGS += -g
ifneq ($(CONFIG_X86_REMOTE_DEBUG),y)
CFLAGS += -fno-asynchronous-unwind-tables
endif
# this is needed right now for the 32bit ioctl code (no-unit-at-a-time)
# Linux 3.16 uses these
CFLAGS += $(call check_gcc,-funit-at-a-time,)

ifdef CONFIG_MK8
CFLAGS += $(call check_gcc,-march=k8,)
endif
ifdef CONFIG_MPSC
CFLAGS += $(call check_gcc,-march=nocona,)
endif

HEAD := linux/arch/x86_64/kernel/head.o linux/arch/x86_64/kernel/head64.o linux/arch/x86_64/kernel/init_task.o

SUBDIRS := linux/arch/x86_64/tools $(SUBDIRS) linux/arch/x86_64/kernel linux/arch/x86_64/mm linux/arch/x86_64/lib
CORE_FILES := linux/arch/x86_64/kernel/kernel.o $(CORE_FILES)
CORE_FILES +=  linux/arch/x86_64/mm/mm.o
LIBS := $(TOPDIR)/linux/arch/x86_64/lib/lib.a $(LIBS)

ifdef CONFIG_IA32_EMULATION
SUBDIRS += linux/arch/x86_64/ia32
CORE_FILES += linux/arch/x86_64/ia32/ia32.o
endif

ifdef CONFIG_HOSTFS
SUBDIRS += linux/arch/x86_64/hostfs
core-$(CONFIG_HOSTFS) += linux/arch/x86_64/hostfs/hostfs.o
endif

CORE_FILES += $(core-y)

linux/arch/x86_64/tools: dummy
	$(MAKE) linuxsubdirs SUBDIRS=linux/arch/x86_64/tools 

linux/arch/x86_64/kernel: dummy 
	$(MAKE) linuxsubdirs SUBDIRS=linux/arch/x86_64/kernel

linux/arch/x86_64/mm: dummy
	$(MAKE) linuxsubdirs SUBDIRS=linux/arch/x86_64/mm

MAKEBOOT = $(MAKE) -C linux/arch/$(ARCH)/boot

linux/vmlinux: linux/arch/x86_64/vmlinux.lds

FORCE: ;

.PHONY: zImage bzImage compressed zlilo bzlilo zdisk bzdisk install \
		clean archclean archmrproper archdep checkoffset

checkoffset: FORCE
	make -C linux/arch/$(ARCH)/tools $(TOPDIR)/linux/include/asm-x86_64/offset.h

bzImage: checkoffset linux/vmlinux
	@$(MAKEBOOT) bzImage

bzImage-padded: checkoffset linux/vmlinux
	@$(MAKEBOOT) bzImage-padded

tmp:
	@$(MAKEBOOT) BOOTIMAGE=bzImage zlilo

bzlilo: checkoffset linux/vmlinux
	@$(MAKEBOOT) BOOTIMAGE=bzImage zlilo

bzdisk: checkoffset linux/vmlinux
	@$(MAKEBOOT) BOOTIMAGE=bzImage zdisk

install: checkoffset linux/vmlinux
	@$(MAKEBOOT) BOOTIMAGE=bzImage install

archclean:
	@$(MAKEBOOT) clean
	@$(MAKE) -C $(TOPDIR)/linux/arch/x86_64/tools clean

archmrproper:
	rm -f $(TOPDIR)/linux/arch/x86_64/tools/offset.h
	rm -f $(TOPDIR)/linux/arch/x86_64/tools/offset.tmp
	rm -f $(TOPDIR)/linux/include/asm-x86_64/offset.h

archdep:
	@$(MAKE) -C $(TOPDIR)/linux/arch/x86_64/tools all
	@$(MAKEBOOT) dep
