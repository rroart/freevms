OBJ = um.o

OBJS = exec_kern.o exec_user.o frame_kern.o frame.o init_task.o irq.o \
	irq_user.o mem.o mem_user.o process.o ptrace.o reboot.o resource.o \
	setup.o signal_user.o smp.o syscall_kern.o syscall_user.o sysrq.o \
	sys_call_table.o time.o time_kern.o tlb.o trap_kern.o trap_user.o \
	uaccess_user.o um_arch.o umid.o user_util.o

ifeq ($(CONFIG_BLK_DEV_INITRD), y)
  OBJS += initrd_kern.o initrd_user.o
endif

# user_syms.o not included here because Rules.make has its own ideas about
# building anything in export-objs

USER_OBJS = $(filter %_user.o,$(OBJS)) process.o time.o umid.o user_util.o

export-objs = ksyms.o process_kern.o signal_kern.o user_syms.o

UNMAP_CFLAGS := $(patsubst -pg -DPROFILING,,$(USER_CFLAGS))
UNMAP_CFLAGS := $(patsubst -fprofile-arcs -ftest-coverage,,$(UNMAP_CFLAGS))

ifeq ($(CONFIG_MODULES), y)
  DMODULES = -D__CONFIG_MODULES__
endif

ifeq ($(CONFIG_MODVERSIONS), y)
  DMODVERSIONS = -D__CONFIG_MODVERSIONS__
endif

ifeq ($(CONFIG_GPROF), y)
    OBJS += gprof_syms.o
    export-objs += gprof_syms.o
endif

ifeq ($(CONFIG_GCOV), y)
    OBJS += gmon_syms.o
    export-objs += gmon_syms.o
endif

CFLAGS_user_syms.o = -D__AUTOCONF_INCLUDED__ $(DMODULES) $(DMODVERSIONS) -I- \
	-I../include -I$(TOPDIR)/../freevms/sys/src

CFLAGS_frame.o := $(patsubst -fomit-frame-pointer,,$(USER_CFLAGS))

all: $(OBJ) unmap_fin.o

$(USER_OBJS) : %.o: %.c
	$(CC) $(CFLAGS_$@) $(USER_CFLAGS) -c -o $@ $<

unmap.o: unmap.c
	$(CC) $(UNMAP_CFLAGS) -c -o $@ $<

frame.o: frame.c
	$(CC) $(CFLAGS_$@) -c -o $@ $<

unmap_fin.o : unmap.o
	ld -r -o $@ $< -lc -L/usr/lib

$(OBJ): $(OBJS) $(export-objs)
	rm -f $@
	$(LD) $(LINKFLAGS) --start-group $^ --end-group -o $@

clean:
	rm -f $(OBJS) $(export-objs)

modules:

fastdep:

include $(TOPDIR)/Rules.make
