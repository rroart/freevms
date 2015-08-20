#
# Makefile for the linux kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

O_TARGET := kernel.o

export-objs = signal.o sys.o kmod.o ksyms.o exec_domain.o printk.o

obj-y     = dma.o fork.o exec_domain.o panic.o printk.o \
	    module6.o exit.o itimer.o info.o time.o resource.o \
	    sysctl.o acct.o capability.o ptrace.o timer.o user.o \
	    signal.o sys.o kmod.o

obj-$(CONFIG_UID16) += uid16.o
obj-$(CONFIG_MODULES) += ksyms.o

CFLAGS_sched.o := $(PROFILING) -fno-omit-frame-pointer

include $(TOPDIR)/Rules.make
