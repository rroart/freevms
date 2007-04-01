O_TARGET := libcmu2.o

override CFLAGS += -DVMS -I. -I$(TOPDIR)/cmuip/central/include -I$(TOPDIR)

#obj-y := maclib.o vmslib.o 
#rpc.o xdr.o

obj-y := inet.o libcmu.o ntoh_hton.o

nokernel_objs := $(addprefix nokernel/,$(obj-y2))
nokernel/%.o: %.c 
	install -d nokernel
	$(CC) -c -I../../../starlet/src -I../../../librtl/src -I../../../lib/src/ -I../../../sys/src $(CFLAGS_NOKERNEL) -o $@ $<

OBJS = $(nokernel_objs)

include $(TOPDIR)/Rules.make
