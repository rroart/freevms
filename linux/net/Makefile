#
# Makefile for the linux networking.
#
# 2 Sep 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
#

O_TARGET :=	network.o

ifneq ($(CONFIG_VMS),y)

mod-subdirs :=	ipv4/netfilter netlink sched
export-objs :=	netsyms.o

subdir-y :=	core ethernet
subdir-m :=	ipv4 # hum?

subdir-$(CONFIG_NET)		+= 802 sched netlink
subdir-$(CONFIG_INET)		+= ipv4
subdir-$(CONFIG_UNIX)		+= unix

subdir-$(CONFIG_PACKET)		+= packet

obj-y	:= socket.o $(join $(subdir-y), $(patsubst %,/%.o,$(notdir $(subdir-y))))
ifeq ($(CONFIG_NET),y)
obj-$(CONFIG_MODULES)		+= netsyms.o
obj-$(CONFIG_SYSCTL)		+= sysctl_net.o
endif

else

obj-y	:= socket.o $(join $(subdir-y), $(patsubst %,/%.o,$(notdir $(subdir-y))))

endif

include $(TOPDIR)/Rules.make
