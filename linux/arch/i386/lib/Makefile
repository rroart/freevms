#
# Makefile for i386-specific library files..
#

.S.o:
	$(CC) $(AFLAGS) -c $< -o $*.o

L_TARGET = lib.a

obj-y = checksum.o old-checksum.o delay.o \
	usercopy.o getuser.o \
	memcpy.o strstr.o

include $(TOPDIR)/Rules.make
