
EXTRA_CFLAGS	+= -I$(TOPDIR)/drivers/scsi


O_TARGET	:= dummy.o

list-multi	:= aacraid.o
aacraid-objs	:= linit.o aachba.o commctrl.o comminit.o commsup.o \
		   dpcsup.o rx.o sap1sup.o

obj-$(CONFIG_SCSI_AACRAID) += aacraid.o

include $(TOPDIR)/Rules.make

aacraid.o: $(aacraid-objs)
	$(LD) -r -o $@ $(aacraid-objs)
