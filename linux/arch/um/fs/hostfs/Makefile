# 
# Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
# Licensed under the GPL
#

# struct stat64 changed the inode field name between 2.2 and 2.4 from st_ino
# to __st_ino.  It stayed in the same place, so as long as the correct name
# is used, hostfs compiled on 2.2 should work on 2.4 and vice versa.

STAT64_INO_FIELD := $(shell grep -q __st_ino /usr/include/bits/stat.h && \
				echo __)st_ino

USER_CFLAGS := $(USER_CFLAGS) -DSTAT64_INO_FIELD=$(STAT64_INO_FIELD)

O_TARGET :=
obj-y = 
obj-m =

CFLAGS_hostfs_kern.o := $(CFLAGS)
CFLAGS_hostfs_user.o := $(USER_CFLAGS)

ifneq ($(CONFIG_HOSTFS), n)
  O_TARGET := hostfs.o
endif

obj-y += hostfs_kern.o hostfs_user.o
obj-m += $(O_TARGET)

override CFLAGS =  

include $(TOPDIR)/Rules.make
