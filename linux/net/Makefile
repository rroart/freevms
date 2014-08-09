#
# Makefile for the linux networking.
#
# 2 Sep 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
#

O_TARGET :=	network.o

obj-y	:= socket.o $(join $(subdir-y), $(patsubst %,/%.o,$(notdir $(subdir-y))))

include $(TOPDIR)/Rules.make
