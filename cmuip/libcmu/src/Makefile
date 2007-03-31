O_TARGET := libcmu2.o

override CFLAGS += -DVMS -I. -I$(TOPDIR)/cmuip/central/include -I$(TOPDIR)

#obj-y := maclib.o vmslib.o 
#rpc.o xdr.o

ifneq ($(ARCH),x86_64)
CHECKSUM = ../../../linux/arch/i386/lib/checksum.o
else
CHECKSUM = ../../../linux/arch/x86_64/lib/csum-partial.o
endif

obj-y := inet.o libcmu.o ntoh_hton.o

nokernel_objs := $(addprefix nokernel/,$(obj-y2))
nokernel/%.o: %.c 
	install -d nokernel
	$(CC) -c -I../../../starlet/src -I../../../librtl/src -I../../../lib/src/ -I../../../sys/src $(CFLAGS_NOKERNEL) -o $@ $<

ifeq ($(ARCH),x86_64)
nokernel/maclib.o: maclib.c 
	install -d nokernel
	$(CC) -mcmodel=medium -c -I../../../starlet/src -I../../../librtl/src -I../../../lib/src/ -I../../../sys/src $(CFLAGS_NOKERNEL) -o $@ $<
endif

OBJS = $(nokernel_objs)

include $(TOPDIR)/Rules.make
