O_TARGET := ip4acp.o

override CFLAGS += -I$(TOPDIR)/cmuip/central/include -I$(TOPDIR)

obj-y := main.o tcp.o tcp_user.o tcp_segin.o tcp_telnet.o tcp_telnet_misc.o udp.o ip.o mem.o memgr.o xedrv.o xedrv_arp.o config.o user.o ip_user.o icmp.o icmp_user.o snmp_hook.o ioutil.o nmlook.o maclib.o

include $(TOPDIR)/Rules.make
