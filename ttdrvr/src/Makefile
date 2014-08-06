O_TARGET := ttdrvr.o

obj-y := ttyfdt.o ttychari.o ttycharo.o ttydrvdat.o ttysub.o ttystrstp.o

ifdef CONFIG_VMS
export-objs := console.o tty_io.o tty_ioctl.o keyboard.o 
obj-y += tty_io.o
obj-y += keyboard.o
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

obj-y += $(CONSOLE) $(KEYBD)
obj-y += vmsconsole.o $(KEYMAP) 

include $(TOPDIR)/Rules.make
