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

ifdef CONFIG_M486
CFLAGS += -march=i486
endif

ifdef CONFIG_M586
CFLAGS += -march=i586
endif

ifdef CONFIG_M586TSC
CFLAGS += -march=i586
endif

ifdef CONFIG_M586MMX
CFLAGS += -march=i586
endif

ifdef CONFIG_M686
CFLAGS += -march=i686
endif

ifdef CONFIG_MPENTIUMIII
CFLAGS += -march=i686
endif

ifdef CONFIG_MPENTIUM4
CFLAGS += -march=i686
endif

ifdef CONFIG_MK6
CFLAGS += $(shell if $(CC) -march=k6 -S -o /dev/null -xc /dev/null >/dev/null 2>&1; then echo "-march=k6"; else echo "-march=i586"; fi)
endif

ifdef CONFIG_MK7
CFLAGS += $(shell if $(CC) -march=athlon -S -o /dev/null -xc /dev/null >/dev/null 2>&1; then echo "-march=athlon"; else echo "-march=i686 -malign-functions=4"; fi) 
endif

ifdef CONFIG_MCRUSOE
CFLAGS += -march=i686 -malign-functions=0 -malign-jumps=0 -malign-loops=0
endif

ifdef CONFIG_MWINCHIPC6
CFLAGS += -march=i586
endif

ifdef CONFIG_MWINCHIP2
CFLAGS += -march=i586
endif

ifdef CONFIG_MWINCHIP3D
CFLAGS += -march=i586
endif

ifdef CONFIG_MCYRIXIII
CFLAGS += -march=i586
endif

HEAD := linux/arch/i386/kernel/head.o linux/arch/i386/kernel/init_task.o

SUBDIRS += linux/arch/i386/tools linux/arch/i386/kernel linux/arch/i386/mm linux/arch/i386/lib

CORE_FILES := linux/arch/i386/kernel/kernel.o linux/arch/i386/mm/mm.o $(CORE_FILES)
LIBS := $(TOPDIR)/linux/arch/i386/lib/lib.a $(LIBS) $(TOPDIR)/linux/arch/i386/lib/lib.a

ifdef CONFIG_MATH_EMULATION
SUBDIRS += linux/arch/i386/math-emu
DRIVERS += linux/arch/i386/math-emu/math.o
endif

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
