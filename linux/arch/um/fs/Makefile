# 
# Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
# Licensed under the GPL
#

O_TARGET := fs.o

subdir-$(CONFIG_HOSTFS) = hostfs

MOD_SUB_DIRS := $(subdir-m)
SUB_DIRS := $(subdir-y)

obj-y += $(join $(subdir-y),$(subdir-y:%=/%.o))
obj-m += $(join $(subdir-m),$(subdir-m:%=/%.o))

include $(TOPDIR)/Rules.make
