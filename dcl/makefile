#===============================================================================
# DCL/2 version 4.00
#
#   Tous droits réservés à l'auteur, Joël BERTRAND
#	All rights reserved worldwide
#===============================================================================

#===============================================================================
# Copyright (C) 2001 BERTRAND Joël
#
# This file is part of DCL/2.
#
# DCL/2 is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2, or (at your option) any
# later version.
#
# DCL/2 is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with Octave; see the file COPYING.  If not, write to the Free
# Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#===============================================================================

VERSION_DCL=0.00

SRCC=\
	builtins_c.c \
	builtins_d.c \
	builtins_h.c \
	builtins_l.c \
	builtins_r.c \
	builtins_s.c \
	call.c \
	dcl.c \
	init.c \
	loop.c \
	parsing.c \
	readcommand.c

SRCH=\
	dcl.h

SRCD= $(SRCC:.c=.d)
OBJC=$(SRCC:.c=.o)
DEPENDC=$(OBJC:.o=.d)

PREFIX=/usr/local

default:
	make install

include $(DEPENDC)


#-------------------------------------------------------------------------------
# Compilateurs et options générales
#-------------------------------------------------------------------------------

CC=g77
F77=g77
LD=g77

GCC_FLAGS=-O2 -g -Wall
LD_FLAGS=-O2 -g


#-------------------------------------------------------------------------------
# Dépendances normales
#-------------------------------------------------------------------------------

%.d: %.c
	set -e; $(CC) -M $< -I. | sed 's/\($*\)\.o[:]*/\1.o $@ : /g' > $@


#-------------------------------------------------------------------------------
# Création des fichiers objet
#-------------------------------------------------------------------------------

%.o: %.c makefile
	$(CC) -c $(GCC_FLAGS) $< -I. -D DCL\$$VERSION="\"$(VERSION_DCL)\""

objects: $(SRCD) $(OBJC)

tags: $(SRCC)
	ctags *.[cf]


#-------------------------------------------------------------------------------
# Création de l'exécutable
#-------------------------------------------------------------------------------

dcl: $(OBJC)
	$(LD) $(LD_FLAGS) -o $@ $+ -lreadline -lncurses


#-------------------------------------------------------------------------------
# Date de compilation
#-------------------------------------------------------------------------------

.PHONY: dcl.o

dcl.o: dcl.c makefile
	$(CC) -c $(GCC_FLAGS) $< -I. \
			-D DCL\$$DATE="\"$(shell LANG=en date +'%A %x, %X %Z')\"" \
			-D DCL\$$VERSION="\"$(VERSION_DCL)\""
	

#-------------------------------------------------------------------------------
# Effacement des fichiers objets, intermédiaires, modules, dépendances et core
#-------------------------------------------------------------------------------

clean:
	rm -f $(OBJC) $(DEPENDC) core dcl

install: dcl
	if [ ! -d $(PREFIX)/bin ]; then mkdir -p $(PREFIX)/bin; fi
	chmod 755 dcl; cp -fu dcl $(PREFIX)/bin/dcl
