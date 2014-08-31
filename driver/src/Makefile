all:

install: 
#i386/random.exe i386/null.exe driver.o
	install -d ../../rooti386/vms"$$"common/sys"$$"ldr
#	install i386/*.exe ../../rooti386/vms"$$"common/sys"$$"ldr

#VMS_VPI = vms"$$"volatile_private_interfaces.a
VMS_VPI = driver_tables.o driver_table_init.o

O_TARGET := driver.o
# $(VMS_VPI) i386/null.exe i386/random.exe 

obj-y := file.o pedriver.o dudriver.o scs.o driver_table_init.o drivmisc.o

#$(VMS_VPI): driver_tables.o driver_table_init.o
#	ar -rv $(VMS_VPI) driver_tables.o driver_table_init.o

i386/%.exe: %.o $(VMS_VPI)
	install -d i386
	ld -r -o $@ $< $(VMS_VPI)

include $(TOPDIR)/Rules.make
