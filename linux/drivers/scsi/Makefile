#
# Makefile for linux/drivers/scsi
#
# 30 May 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
#
# 20 Sep 2000, Torben Mathiasen <tmm@image.dk>
# Changed link order to reflect new scsi initialization.
#
# *!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!
# The link order must be, SCSI Core, SCSI HBA drivers, and
# lastly SCSI peripheral drivers (disk/tape/cdrom/etc.) to
# satisfy certain initialization assumptions in the SCSI layer.
# *!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!*!


CFLAGS_aha152x.o =   -DAHA152X_STAT -DAUTOCONF
CFLAGS_seagate.o =   -DARBITRATE -DPARITY -DSEAGATE_USE_ASM


O_TARGET := scsidrv.o

export-objs	:= scsi_syms.o 53c700.o


subdir-$(CONFIG_SCSI_AACRAID)	+= aacraid
subdir-$(CONFIG_SCSI_AIC7XXX)	+= aic7xxx


obj-$(CONFIG_SCSI)		+= scsi_mod.o

obj-$(CONFIG_SCSI_PCI2000)	+= pci2000.o
obj-$(CONFIG_SCSI_PCI2220I)	+= pci2220i.o
obj-$(CONFIG_SCSI_U14_34F)	+= u14-34f.o
obj-$(CONFIG_SCSI_ULTRASTOR)	+= ultrastor.o
obj-$(CONFIG_SCSI_AHA152X)	+= aha152x.o
obj-$(CONFIG_SCSI_AHA1542)	+= aha1542.o
obj-$(CONFIG_SCSI_AHA1740)	+= aha1740.o
ifeq ($(CONFIG_SCSI_AACRAID),y)
  obj-$(CONFIG_SCSI_AACRAID)	+= aacraid/aacraid.o
endif
ifeq ($(CONFIG_SCSI_AIC7XXX),y)
obj-$(CONFIG_SCSI_AIC7XXX)	+= aic7xxx/aic7xxx_drv.o
endif
obj-$(CONFIG_SCSI_AIC7XXX_OLD)	+= aic7xxx_old.o
obj-$(CONFIG_SCSI_FD_MCS)	+= fd_mcs.o
obj-$(CONFIG_SCSI_IN2000)	+= in2000.o
obj-$(CONFIG_SCSI_GENERIC_NCR5380) += g_NCR5380.o
obj-$(CONFIG_SCSI_NCR53C406A)	+= NCR53c406a.o
obj-$(CONFIG_SCSI_NCR_D700)	+= NCR_D700.o 53c700.o
obj-$(CONFIG_SCSI_SYM53C416)	+= sym53c416.o
obj-$(CONFIG_SCSI_PAS16)	+= pas16.o
obj-$(CONFIG_SCSI_SEAGATE)	+= seagate.o
obj-$(CONFIG_SCSI_FD_8xx)	+= seagate.o
obj-$(CONFIG_SCSI_T128)		+= t128.o
obj-$(CONFIG_SCSI_DMX3191D)	+= dmx3191d.o
obj-$(CONFIG_SCSI_DTC3280)	+= dtc.o
obj-$(CONFIG_SCSI_NCR53C7xx)	+= 53c7,8xx.o 
subdir-$(CONFIG_SCSI_SYM53C8XX_2)	+= sym53c8xx_2
ifeq ($(CONFIG_SCSI_SYM53C8XX_2),y)
  obj-$(CONFIG_SCSI_SYM53C8XX_2)	+= sym53c8xx_2/sym53c8xx.o
endif
obj-$(CONFIG_SCSI_SYM53C8XX)	+= sym53c8xx.o 
obj-$(CONFIG_SCSI_NCR53C8XX)	+= ncr53c8xx.o 
obj-$(CONFIG_SCSI_7000FASST)	+= wd7000.o
obj-$(CONFIG_SCSI_DC390T)	+= tmscsim.o
obj-$(CONFIG_SCSI_AM53C974)	+= AM53C974.o
obj-$(CONFIG_SCSI_MEGARAID)	+= megaraid.o
obj-$(CONFIG_SCSI_ACARD)	+= atp870u.o
obj-$(CONFIG_BLK_DEV_IDESCSI)	+= ide-scsi.o
obj-$(CONFIG_SCSI_MESH)		+= mesh.o
obj-$(CONFIG_SCSI_PLUTO)	+= pluto.o
obj-$(CONFIG_BLK_DEV_3W_XXXX_RAID) += 3w-xxxx.o
obj-$(CONFIG_SCSI_PPA)		+= ppa.o
obj-$(CONFIG_SCSI_IMM)		+= imm.o
obj-$(CONFIG_SCSI_DEBUG)	+= scsi_debug.o
obj-$(CONFIG_SCSI_FCAL)		+= fcal.o
obj-$(CONFIG_SCSI_LASI700)	+= lasi700.o 53c700.o

obj-$(CONFIG_CHR_DEV_ST)	+= st.o
obj-$(CONFIG_CHR_DEV_OSST)	+= osst.o
obj-$(CONFIG_BLK_DEV_SD)	+= sd_mod.o
obj-$(CONFIG_BLK_DEV_SR)	+= sr_mod.o
obj-$(CONFIG_CHR_DEV_SG)	+= sg.o

list-multi	:= scsi_mod.o sd_mod.o sr_mod.o
scsi_mod-objs	:= scsi.o hosts.o scsi_ioctl.o constants.o \
			scsicam.o scsi_proc.o scsi_error.o \
			scsi_obsolete.o scsi_queue.o scsi_lib.o \
			scsi_merge.o scsi_dma.o scsi_scan.o \
			scsi_syms.o
sd_mod-objs	:= sd.o
sr_mod-objs	:= sr.o sr_ioctl.o sr_vendor.o
initio-objs	:= ini9100u.o i91uscsi.o

include $(TOPDIR)/Rules.make


scsi_mod.o: $(scsi_mod-objs)
	$(LD) -r -o $@ $(scsi_mod-objs)

sd_mod.o: $(sd_mod-objs)
	$(LD) -r -o $@ $(sd_mod-objs)

sr_mod.o: $(sr_mod-objs)
	$(LD) -r -o $@ $(sr_mod-objs)

initio.o: $(initio-objs)
	$(LD) -r -o $@ $(initio-objs)

53c8xx_d.h: 53c7,8xx.scr script_asm.pl
	ln -sf 53c7,8xx.scr fake8.c
	$(CPP) $(CPPFLAGS) -traditional -DCHIP=810 fake8.c | grep -v '^#' | $(PERL) script_asm.pl 
	mv script.h 53c8xx_d.h
	mv scriptu.h 53c8xx_u.h
	rm fake8.c

53c8xx_u.h: 53c8xx_d.h

53c7,8xx.o: 53c8xx_u.h

53c7xx_d.h: 53c7xx.scr script_asm.pl
	ln -sf 53c7xx.scr fake7.c
	$(CPP) $(CPPFLAGS) -traditional -DCHIP=710 fake7.c | grep -v '^#' | $(PERL) -s script_asm.pl -ncr7x0_family
	mv script.h 53c7xx_d.h
	mv scriptu.h 53c7xx_u.h
	rm fake7.c

53c7xx_u.h: 53c7xx_d.h

53c7xx.o : 53c7xx_d.h

sim710_d.h: sim710.scr script_asm.pl
	ln -sf sim710.scr fake7.c
	$(CPP) $(CPPFLAGS) -traditional -DCHIP=710 fake7.c | grep -v '^#' | $(PERL) -s script_asm.pl -ncr7x0_family
	mv script.h sim710_d.h
	mv scriptu.h sim710_u.h
	rm fake7.c

sim710_u.h: sim710_d.h

sim710.o : sim710_d.h

53c700_d.h: 53c700.scr script_asm.pl
	$(PERL) -s script_asm.pl -ncr7x0_family < 53c700.scr
	rm -f scriptu.h
	mv script.h 53c700_d.h

53c700.o: 53c700_d.h
