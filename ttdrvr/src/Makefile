O_TARGET := ttdrvr.o

obj-y := ttyfdt.o ttychari.o ttycharo.o ttydrvdat.o ttysub.o ttystrstp.o

ifdef CONFIG_VMS
export-objs := tty_io.o tty_ioctl.o 
obj-y += tty_io.o tty_ioctl.o n_tty.o
endif

include $(TOPDIR)/Rules.make
