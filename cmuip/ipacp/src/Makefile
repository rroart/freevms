O_TARGET := ipacp.o

override CFLAGS += -I$(TOPDIR)/cmuip/central/include -I$(TOPDIR)

obj-y := maclib.o vmslib.o 
#rpc.o xdr.o

ifneq ($(ARCH),x86_64)
CHECKSUM = ../../../linux/arch/i386/lib/checksum.o
else
CHECKSUM = ../../../linux/arch/x86_64/lib/csum-partial.o
endif

obj-y2 := main.o tcp.o tcp_user.o tcp_segin.o tcp_telnet.o tcp_telnet_misc.o udp.o ip.o mem.o memgr.o xedrv.o xedrv_arp.o config.o user.o ip_user.o icmp.o icmp_user.o snmp_hook.o ioutil.o nmlook.o maclib.o vmslib.o tcp_mech.o snmp_agent.o snmp_auth.o snmp.o asn1.o

all: i386/ipacp${NOEXE}

install: all
	install -d ../../../rooti386/vms"$$"common/sysexe
	install i386/ipacp ../../../rooti386/vms"$$"common/sysexe

CFLAGS_NOKERNEL = -I$(TOPDIR)/cmuip/central/include -I$(TOPDIR)/linux/include -I$(TOPDIR) -DNOKERNEL -D__KERNEL__

nokernel_objs := $(addprefix nokernel/,$(obj-y2))
nokernel/%.o: %.c 
	install -d nokernel
	$(CC) -c -I../../../starlet/src -I../../../librtl/src -I../../../lib/src/ -I../../../sys/src $(CFLAGS_NOKERNEL) -o $@ $<

ifeq ($(ARCH),x86_64)
nokernel/maclib.o: maclib.c 
	install -d nokernel
	$(CC) -mcmodel=large -c -I../../../starlet/src -I../../../librtl/src -I../../../lib/src/ -I../../../sys/src $(CFLAGS_NOKERNEL) -o $@ $<
endif

OBJS = $(nokernel_objs)

VMLINUX = ../../../linux/vmlinux 
LINKVMLINUX = -Wl,--just-symbols=../../../linux/vmlinux 

i386/ipacp${NOEXE}: $(OBJS) $(VMSLIBS) $(EXTERNAL) $(VMLINUX)
	install -d i386
	gcc -s -static $(OBJS) $(EXTERNAL) $(VMSLIBS) $(CHECKSUM) -lc $(LINKVMLINUX) -o i386/ipacp${NOEXE}
	gcc -g -static $(OBJS) $(EXTERNAL) $(VMSLIBS) $(CHECKSUM) -lc $(LINKVMLINUX) -o i386/ipacpd${NOEXE}
