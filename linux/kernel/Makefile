#
# Makefile for the linux kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

O_TARGET := kernel.o

export-objs = signal.o sys.o kmod.o context.o ksyms.o pm.o exec_domain.o printk.o

obj-y     = sched.o dma.o fork.o exec_domain.o panic.o printk.o \
	    module.o exit.o itimer.o info.o time.o softirq.o resource.o \
	    sysctl.o acct.o capability.o ptrace.o timer.o user.o \
	    signal.o sys.o kmod.o context.o ast.o \
	    syssetpri.o system_data_cells.o ipl.o syspcntrl.o timeschdl.o \
	    exception_init.o syswait.o sysevtsrv.o rse.o exsubrout.o \
	    iociopost.o postef.o astdel.o forkcntrl.o iosubnpag.o sysastcon.o \
	    sysgettim.o syssetime.o syscvrtim.o syscanevt.o sysschevt.o \
	    sysdelprc.o sysforcex.o sysexit.o syssetmod.o sysenqdeq.o \
	    sysqioreq.o sysqiofdt.o mscp.o dudriver.o sys_scs.o sysacpfdt.o \
	    sysassign.o sysdassgn.o iosubpagd.o sysgetlki.o \
	    syslnm.o lnmsub.o random.o null.o vmsinit.o mutex.o \
	    queue.o sysascefc.o deadlock.o sysmailbx.o mbdriver.o \
	    rms.o rms2.o access.o update.o cache.o direct.o \
	    device.o phyunix.o phyvms.o mountdsp.o ucbcredel.o dispatch.o \
	    file.o iosubrams.o pedriver.o scs.o pagefault.o syscredel.o \
	    sysadjwsl.o syslkwset.o syscrmpsc.o phdutl.o sysdgblsc.o \
	    sysdclexh.o sysimgfix.o sysimgsta.o sysfao.o sysgetdvi.o \
	    exceptmsg.o sysputmsg.o sysgetmsg.o syssndjbc.o \
	    sysgetsyi.o sysrundwn.o syscreprc.o procstrt.o sysimgact.o \
	    syssetprt.o syssetprv.o syspurgws.o uai.o sysparprc.o \
	    sysupdsec.o sysgetjpi.o syssetexv.o process_scan.o \
	    rm3get.o pagefile.o

obj-$(CONFIG_UID16) += uid16.o
obj-$(CONFIG_MODULES) += ksyms.o
obj-$(CONFIG_PM) += pm.o

ifneq ($(CONFIG_IA64),y)
# According to Alan Modra <alan@linuxcare.com.au>, the -fno-omit-frame-pointer is
# needed for x86 only.  Why this used to be enabled for all architectures is beyond
# me.  I suspect most platforms don't need this, but until we know that for sure
# I turn this off for IA-64 only.  Andreas Schwab says it's also needed on m68k
# to get a correct value for the wait-channel (WCHAN in ps). --davidm
CFLAGS_sched.o := $(PROFILING) -fno-omit-frame-pointer
endif

include $(TOPDIR)/Rules.make
