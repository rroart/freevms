#O_TARGET := ods2.o
O_TARGET := f11x.o

#obj-y := super.o inode.o vms.o dir.o name.o
#obj-m    := $(O_TARGET)

obj-y := dispatch.o

include $(TOPDIR)/Rules.make

myall:	testvms

testvms:
	gcc -Wall -o testvms testvms.c
