O_TARGET := ttdrvr.o

obj-y := ttyfdt.o ttychari.o ttycharo.o ttydrvdat.o ttysub.o ttystrstp.o

ifdef CONFIG_VMS
export-objs := console.o tty_io.o tty_ioctl.o 
obj-y += tty_io.o tty_ioctl.o n_tty.o
endif

KEYMAP   =defkeymap.o
ifdef CONFIG_VMS
KEYBD    =pc_keyb.o
CONSOLE  =console.o
else
KEYBD    =
CONSOLE  =
endif
SERIAL   =serial.o

ifeq ($(ARCH),um)
  KEYMAP   =
  KEYBD    =
  CONSOLE  =
endif

obj-$(CONFIG_VT) += $(CONSOLE) $(KEYBD)
ifdef CONFIG_VMS
obj-$(CONFIG_VT) += vmspc_keybd.o vmsconsole.o
endif

include $(TOPDIR)/Rules.make
