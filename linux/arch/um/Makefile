ARCH_DIR = arch/um

include arch/$(ARCH)/Makefile-$(SUBARCH)

EXTRAVERSION := $(EXTRAVERSION)-11um
include/linux/version.h: arch/$(ARCH)/Makefile

# Recalculate MODLIB to reflect the EXTRAVERSION changes (via KERNELRELEASE)
# The way the toplevel Makefile is written EXTRAVERSION is not supposed
# to be changed outside the toplevel Makefile, but recalculating MODLIB is
# a sufficient workaround until we no longer need architecture dependent
# EXTRAVERSION...
MODLIB := $(INSTALL_MOD_PATH)/lib/modules/$(KERNELRELEASE)

MAKEBOOT = $(MAKE) -C $(ARCH_DIR)/boot

ifeq ($(CONFIG_DEBUGSYM),y)
DEBUG = -g
CFLAGS := $(subst -fomit-frame-pointer,,$(CFLAGS))
endif

ifeq ($(CONFIG_GCOV),y)
CFLAGS += -fprofile-arcs -ftest-coverage
endif

ifeq ($(CONFIG_GPROF), y)
PROFILE += -pg -DPROFILING
LINK_PROFILE = $(PROFILE) -Wl,--wrap,__monstartup
endif

SUBDIRS += $(ARCH_DIR)/fs $(ARCH_DIR)/drivers \
	$(ARCH_DIR)/kernel $(ARCH_DIR)/sys-$(SUBARCH)

LIBS += $(shell [ -e $(ARCH_DIR)/fs/fs.o ] && echo $(ARCH_DIR)/fs/fs.o) \
	$(ARCH_DIR)/kernel/um.o $(ARCH_DIR)/drivers/um_drivers.o \
	$(ARCH_DIR)/sys-$(SUBARCH)/sys.o 

ifeq ($(CONFIG_PT_PROXY), y)
SUBDIRS += $(ARCH_DIR)/ptproxy
LIBS += $(ARCH_DIR)/ptproxy/ptproxy.a
endif

NESTING = 0

ARCH_INCLUDE = $(TOPDIR)/$(ARCH_DIR)/include

# -Derrno=kernel_errno - This turns all kernel references to errno into
# kernel_errno to separate them from the libc errno.  This allows -fno-common
# in CFLAGS.  Otherwise, it would cause ld to complain about the two different
# errnos.

CFLAGS += $(DEBUG) $(PROFILE) $(ARCH_CFLAGS) -D__arch_um__ \
	-DSUBARCH=\"$(SUBARCH)\" -DNESTING=$(NESTING) -D_LARGEFILE64_SOURCE \
	-I$(ARCH_INCLUDE) -I$(TOPDIR)/../freevms/sys/src -I$(TOPDIR)/../freevms/lib/src -I$(TOPDIR)/../freevms/librtl/src -I$(TOPDIR)/../freevms/starlet/src -I$(TOPDIR)/../freevms/pal/src -Derrno=kernel_errno

LINKFLAGS += -r

LINK_WRAPS = -Wl,--wrap,malloc -Wl,--wrap,free -Wl,--wrap,calloc

$(ARCH_DIR)/link.ld: $(ARCH_DIR)/link.ld.in
	m4 -DSTART=$(START_ADDR) -DSUBARCH=$(SUBARCH) \
		-DELF_SUBARCH=$(ELF_SUBARCH) $< > $@

SYMLINK_HEADERS = include/asm-um/archparam.h include/asm-um/system.h \
	include/asm-um/sigcontext.h include/asm-um/processor.h \
	include/asm-um/ptrace.h 

ARCH_SYMLINKS = include/asm-um/arch arch/um/include/sysdep $(SYMLINK_HEADERS)

GEN_HEADERS = $(ARCH_DIR)/include/task.h

linux: $(ARCH_SYMLINKS) $(SYS_HEADERS) $(GEN_HEADERS) $(ARCH_DIR)/main.o \
	vmlinux $(ARCH_DIR)/link.ld
	mv vmlinux vmlinux.o
	$(CC) -Wl,-T,$(ARCH_DIR)/link.ld $(LINK_PROFILE) $(LINK_WRAPS) \
		-o linux -static $(ARCH_DIR)/main.o vmlinux.o -L/usr/lib

USER_CFLAGS := $(patsubst -I%,,$(CFLAGS)) -I$(TOPDIR)/../freevms/sys/src -I$(TOPDIR)/../freevms/lib/src
USER_CFLAGS := $(patsubst -Derrno=kernel_errno,,$(USER_CFLAGS))
USER_CFLAGS := $(patsubst -D__KERNEL__,,$(USER_CFLAGS)) -I$(ARCH_INCLUDE)

# To get a definition of F_SETSIG
USER_CFLAGS += -D_GNU_SOURCE

$(ARCH_DIR)/main.o: $(ARCH_DIR)/main.c
	$(CC) $(USER_CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

archmrproper:
	$(MAKE) -C $(ARCH_DIR)/sys-$(SUBARCH) archmrproper
	rm -f $(SYMLINK_HEADERS) $(ARCH_SYMLINKS) include/asm \
		$(ARCH_DIR)/link.ld \
		$(addprefix $(ARCH_DIR)/kernel/,$(KERN_SYMLINKS))

archclean: sysclean
	$(MAKE) $(MFLAGS) -C $(ARCH_DIR)/util clean
	find . \( -name '*.bb' -o -name '*.bbg' -o -name '*.da' \
		-o -name '*.gcov' \) -type f -print | xargs rm -f
	rm -f linux x.i gmon.out $(ARCH_DIR)/link.ld $(GEN_HEADERS)
	@$(MAKEBOOT) clean

archdep: 
	@$(MAKEBOOT) dep

$(SYMLINK_HEADERS):
	echo $@
	cd $(TOPDIR)/$(dir $@) ; \
	ln -sf $(basename $(notdir $@))-$(SUBARCH)$(suffix $@) $(notdir $@)

include/asm-um/arch:
	cd $(TOPDIR)/include/asm-um && ln -sf ../asm-$(SUBARCH) arch

arch/um/include/sysdep:
	cd $(TOPDIR)/arch/um/include && ln -sf sysdep-$(SUBARCH) sysdep

$(ARCH_DIR)/include/task.h : $(ARCH_DIR)/util/mk_task
	$< > $@

$(ARCH_DIR)/util/mk_task : $(ARCH_DIR)/util/mk_task_user.c \
	$(ARCH_DIR)/util/mk_task_kern.c 
	$(MAKE) $(MFLAGS) -C $(ARCH_DIR)/util all

export SUBARCH USER_CFLAGS
