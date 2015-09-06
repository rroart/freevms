#
# Makefile for x86_64-specific library files..
#

.S.o:
	$(CC) $(AFLAGS) -c $< -o $*.o

CFLAGS_csum-partial.o := $(PROFILING) -funroll-loops

export-objs := csum-wrappers.o

L_TARGET = lib.a
obj-y  = csum-partial.o csum-copy.o csum-wrappers.o delay.o \
	usercopy.o getuser.o putuser.o  \
	thunk.o io.o clear_page.o copy_page.o copy_user.o \
	memcpy.o memset.o memmove.o bitstr.o

include $(TOPDIR)/Rules.make
