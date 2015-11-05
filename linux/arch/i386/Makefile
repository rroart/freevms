#
# i386/Makefile
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
#

LD=$(CROSS_COMPILE)ld -m elf_i386
OBJCOPY=$(CROSS_COMPILE)objcopy -O binary -R .note -R .comment -R p1pp -S
LDFLAGS=-e stext
LINKFLAGS =-T $(TOPDIR)/linux/arch/i386/vmlinux.lds $(LDFLAGS)

CFLAGS += -pipe

# prevent gcc from keeping the stack 16 byte aligned
CFLAGS += $(shell if $(CC) -mpreferred-stack-boundary=2 -S -o /dev/null -xc /dev/null >/dev/null 2>&1; then echo "-mpreferred-stack-boundary=2"; fi)

ifdef CONFIG_M386
CFLAGS += -march=i386
endif

HEAD := linux/arch/i386/kernel/head.o linux/arch/i386/kernel/init_task.o

SUBDIRS += linux/arch/i386/tools linux/arch/i386/kernel linux/arch/i386/mm linux/arch/i386/lib

CORE_FILES := linux/arch/i386/kernel/kernel.o linux/arch/i386/mm/mm.o $(CORE_FILES)
LIBS := $(TOPDIR)/linux/arch/i386/lib/lib.a $(LIBS) $(TOPDIR)/linux/arch/i386/lib/lib.a

linux/arch/i386/tools: dummy
	$(MAKE) linuxsubdirs SUBDIRS=linux/arch/i386/tools 

linux/arch/i386/kernel: dummy
	$(MAKE) linuxsubdirs SUBDIRS=linux/arch/i386/kernel

linux/arch/i386/mm: dummy
	$(MAKE) linuxsubdirs SUBDIRS=linux/arch/i386/mm

MAKEBOOT = $(MAKE) -C linux/arch/$(ARCH)/boot

linux/vmlinux: linux/arch/i386/vmlinux.lds

FORCE: ;

.PHONY: zImage bzImage compressed zlilo bzlilo zdisk bzdisk install \
		clean archclean archmrproper archdep checkoffset

checkoffset: FORCE
	make -C linux/arch/$(ARCH)/tools $(TOPDIR)/linux/include/asm-i386/offset.h

zImage: checkoffset linux/vmlinux
	@$(MAKEBOOT) zImage

bzImage: checkoffset linux/vmlinux
	@$(MAKEBOOT) bzImage

compressed: zImage

zlilo: checkoffset linux/vmlinux
	@$(MAKEBOOT) BOOTIMAGE=zImage zlilo

tmp:
	@$(MAKEBOOT) BOOTIMAGE=bzImage zlilo
bzlilo: checkoffset linux/vmlinux
	@$(MAKEBOOT) BOOTIMAGE=bzImage zlilo

zdisk: checkoffset linux/vmlinux
	@$(MAKEBOOT) BOOTIMAGE=zImage zdisk

bzdisk: checkoffset linux/vmlinux
	@$(MAKEBOOT) BOOTIMAGE=bzImage zdisk

install: checkoffset linux/vmlinux
	@$(MAKEBOOT) BOOTIMAGE=bzImage install

archclean:
	@$(MAKEBOOT) clean
	@$(MAKE) -C $(TOPDIR)/linux/arch/i386/tools clean

archmrproper:
	rm -f $(TOPDIR)/linux/arch/i386/tools/offset.h
	rm -f $(TOPDIR)/linux/arch/i386/tools/offset.tmp
	rm -f $(TOPDIR)/linux/include/asm-i386/offset.h

archdep:
	@$(MAKE) -C $(TOPDIR)/linux/arch/i386/tools all
	@$(MAKEBOOT) dep
