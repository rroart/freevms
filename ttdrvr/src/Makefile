O_TARGET := ttdrvr.o

obj-y := ttyfdt.o ttychari.o ttycharo.o ttydrvdat.o ttysub.o ttystrstp.o

export-objs := console.o tty_io.o tty_ioctl.o keyboard.o 
obj-y += tty_io.o
obj-y += keyboard.o

KEYMAP   =../../linux/drivers/char/defkeymap.o
KEYBD    =pc_keyb.o
CONSOLE  =console.o
SERIAL   =serial.o

obj-y += $(CONSOLE) $(KEYBD)
obj-y += vmsconsole.o $(KEYMAP) 

include $(TOPDIR)/Rules.make
