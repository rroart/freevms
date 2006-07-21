#
# Makefile for the linux kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#
# Note 2! The CFLAGS definitions are now in the main makefile...

O_TARGET := sys.o

export-objs = 

obj-y     = sched.o \
	    syssetpri.o system_data_cells.o syspcntrl.o timeschdl.o \
	    exception_init.o syswait.o sysevtsrv.o rse.o exsubrout.o \
	    iociopost.o postef.o astdel.o forkcntrl.o iosubnpag.o sysastcon.o \
	    sysgettim.o syssetime.o syscvrtim.o syscanevt.o sysschevt.o \
	    sysdelprc.o sysforcex.o sysexit.o syssetmod.o sysenqdeq.o \
	    sysqioreq.o sysqiofdt.o sysacpfdt.o \
	    sysassign.o sysdassgn.o iosubpagd.o sysgetlki.o \
	    syslnm.o lnmsub.o init.o mutex.o \
	    sysascefc.o deadlock.o sysmailbx.o mbdriver.o \
	    ucbcredel.o \
	    iosubrams.o pagefault.o syscredel.o \
	    sysadjwsl.o syslkwset.o syscrmpsc.o phdutl.o sysdgblsc.o \
	    sysdclexh.o sysimgfix.o sysimgsta.o sysfao.o sysgetdvi.o \
	    exceptmsg.o sysputmsg.o sysgetmsg.o syssndjbc.o \
	    sysgetsyi.o sysrundwn.o syscreprc.o procstrt.o sysimgact.o \
	    syssetprt.o syssetprv.o syspurgws.o uai.o sysparprc.o \
	    sysupdsec.o sysgetjpi.o syssetexv.o process_scan.o \
	    pagefile.o comdrvsub.o syscancel.o swapper.o syssndmsg.o \
	    shell.o memoryalc.o look_aside_list.o memoryalc_dyn.o \
	    memoryalc_pool.o syscli.o syschgmod.o \
	    system_service_dispatcher.o vmslib.o spinlocks.o smprout.o \
	    capability.o

ifeq ($(CONFIG_VMS),y)
export-objs := vmsfilemap.o vmsbuffer.o
endif

obj-$(CONFIG_VMS) += vmsbootmem.o ptalloc.o \
			vmsslab.o wrtmfypag.o \
			vmsmmap.o vmsfilemap.o vmsmemory.o vmsmremap.o \
	 		vmsmprotect.o vmsmlock.o vmsvmalloc.o \
			vmsswapfile.o allocpfn.o

obj-$(CONFIG_VMS) += vmsbuffer.o

include $(TOPDIR)/Rules.make
