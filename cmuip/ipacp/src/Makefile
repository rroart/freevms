O_TARGET := ipacp.o

override CFLAGS += -I$(TOPDIR)/cmuip/central/include -I$(TOPDIR)

obj-y := main.o tcp.o tcp_user.o tcp_segin.o tcp_telnet.o tcp_telnet_misc.o udp.o ip.o mem.o memgr.o xedrv.o xedrv_arp.o config.o user.o ip_user.o icmp.o icmp_user.o snmp_hook.o ioutil.o nmlook.o maclib.o vmslib.o tcp_mech.o snmp_agent.o snmp_auth.o snmp.o asn1.o
#rpc.o xdr.o

include $(TOPDIR)/Rules.make
