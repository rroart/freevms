O_TARGET := ttdrvr.o

obj-y := ttyfdt.o ttychari.o ttycharo.o ttydrvdat.o ttysub.o ttystrstp.o

ifdef CONFIG_VMS
export-objs := console.o tty_io.o tty_ioctl.o keyboard.o 
obj-y += tty_io.o
ifneq ($(ARCH),um)
obj-y += keyboard.o
endif
endif

KEYMAP   =../../linux/drivers/char/defkeymap.o
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

obj-y += $(CONSOLE) $(KEYBD)
ifneq ($(ARCH),um)
obj-y += vmsconsole.o $(KEYMAP) 
endif

include $(TOPDIR)/Rules.make
