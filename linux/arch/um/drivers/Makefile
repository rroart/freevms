# 
# Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
# Licensed under the GPL
#

OBJ := um_drivers.o

CHAN_OBJS = chan_kern.o chan_user.o line.o 

OBJS-y = 
OBJS-$(CONFIG_SSL) += ssl.o 
OBJS-$(CONFIG_UML_NET_SLIP) += slip_kern.o slip_user.o
OBJS-$(CONFIG_UML_NET_ETHERTAP) += ethertap_kern.o ethertap_user.o 
OBJS-$(CONFIG_UML_NET_TUNTAP) += tuntap_kern.o tuntap_user.o
OBJS-$(CONFIG_UML_NET_DAEMON) += daemon_kern.o daemon_user.o 
OBJS-$(CONFIG_UML_NET_MCAST) += mcast_user.o mcast_kern.o 
OBJS-$(CONFIG_UML_NET) += net_kern.o net_user.o 
OBJS-$(CONFIG_MCONSOLE) += mconsole_kern.o mconsole_user.o
OBJS-$(CONFIG_MMAPPER) += mmapper_kern.o 
ifneq ($(CONFIG_VMS),y)
OBJS-y += ubd.o ubd_user.o
endif
OBJS-$(CONFIG_VMS) += ../../../../driver/src/ubdriver.o ubd_user.o vmsstdio_console.o
OBJS-$(CONFIG_HOSTAUDIO) += hostaudio_kern.o hostaudio_user.o
OBJS-$(CONFIG_FD_CHAN) += fd.o 
OBJS-$(CONFIG_NULL_CHAN) += null.o 
OBJS-$(CONFIG_PORT_CHAN) += port.o port_kern.o
OBJS-$(CONFIG_PTY_CHAN) += pty.o
OBJS-$(CONFIG_TTY_CHAN) += tty.o 
OBJS-$(CONFIG_XTERM_CHAN) += xterm.o 

OBJS = stdio_console.o $(OBJS-y) $(CHAN_OBJS)

USER_OBJS = $(filter %_user.o,$(OBJS)) fd.o null.o port.o pty.o socket.o \
	tty.o xterm.o

all : $(OBJ)

$(USER_OBJS) : %.o: %.c
	$(CC) $(CFLAGS_$@) $(USER_CFLAGS) -c -o $@ $<

$(OBJ): $(OBJS) $(export-objs)
	rm -f $@
	$(LD) $(LINKFLAGS) --start-group $^ --end-group -o $@

clean:
	rm -f $(OBJS) $(export-objs)

modules:

fastdep:

include $(TOPDIR)/Rules.make
