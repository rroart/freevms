#
#	****************************************************************
#
#		Copyright (c) 1992, Carnegie Mellon University
#
#		All Rights Reserved
#
#	Permission  is  hereby  granted   to  use,  copy,  modify,  and
#	distribute  this software  provided  that the  above  copyright
#	notice appears in  all copies and that  any distribution be for
#	noncommercial purposes.
#
#	Carnegie Mellon University disclaims all warranties with regard
#	to this software.  In no event shall Carnegie Mellon University
#	be liable for  any special, indirect,  or consequential damages
#	or any damages whatsoever  resulting from loss of use, data, or
#	profits  arising  out of  or in  connection  with  the  use  or
#	performance of this software.
#
#	****************************************************************
#
#++
# Descrip.Mms	Copyright (c) 1986	Carnegie Mellon University
#
# Description:
#
#	Descrip.Mms for IP/TCP Telnet program.
#
# Written By:	Dale Moore	02-MAY-1986	CMU-CS/RI
#
# Modifications:
#
#	40-Sep-1990	Bruce R. Miller		CMU NetDev
#	Modified to work with 6.5 kitinstal.com
#	Include .OLB files in saveset
#
#	10-MAR-1989	Dale Moore	CMU-CS
#	Removed PRINT.L32 from EXE dependencies.
#
#	09-JAN-1989	Dale Moore	CMU-CS
#	Added a few commas to be consistent.
#	Modified the module name here to correspond to the source
#	Added TELNET.HLB to the B saveset.
#
#	20-DEC-1988	Bruce Miller	CMU Network Development
#	Added module Telnet_Term to contain terminal-specific code.
#
#	12-DEC-1988	Bruce Miller	CMU Network Development
#	Added module OPTIONS to contain option-specific code.
#
#	06-MAY-1986	Dale Moore	CMU-CS/RI
#	Change the object files to be in a library.
#--

CC		= $(BLISS)

DEBUG		= /NODEBUG /NOTRACE
LDEBUG		= /NODEBUG /NOTRACE

BFLAGS		= /OBJECT=$(MMS$TARGET_NAME) $(DEBUG) /TERM=STATISTICS /LIST
LINKFLAGS	= /EXEC=$(MMS$TARGET_NAME) $(LDEBUG) /MAP/SYM

all: links telnet

telnet	: telnet.exe
	echo "$ ! Done building Telnet"

both	: telnet.hlb telnet.exe
	echo "$ ! Done building Telnet"

telnet_server.a		: telnet_server.o telnet_common.o print.o telnetmsg.o
	ar -rv telnet_server.o telnet_common.o print.o telnetmsg.o

telnet_server.exe	: telnet_server.a
	$(link) $(linkflags) \
		telnet_server.a/lib/include=telnet_server \
		telnet_server.opt/opt \
		cmuip_src:[central]neterror.o \
		cmuip_src:[central]netlib.a/lib

telnet.a	: telnet.o telnetmsg.o telnet_common.o options.o telnet_parse.o routines.o key.o port.o print.o telnet_term.o condition.o log_util.o
	ar -rv telnet.o telnetmsg.o telnet_common.o options.o telnet_parse.o routines.o key.o port.o print.o telnet_term.o condition.o log_util.o

telnet.exe	: telnet.a
	$(link) $(linkflags) telnet.a/lib/include=telnet \
		telnet.opt/opt\
		cmuip_src:[central]neterror.o \
		cmuip_src:[central]netlib.a/lib

telnet_server.o	: telnet_server.b32 telnet.req option.req

telnet.o	: telnet.b32 telnet.req print.req option.req cli.req

telnet_term.o	: telnet_term.b32 print.req

telnet_common.o	: telnet_common.b32 option.req print.req

telnet_parse.o	: telnet_parse.cld

telnet.hlb	: telnetcmd.hlp
	library /create /help telnet.hlb telnetcmd.hlp

ttyloc.o	: ttyloc.b32 ttyloc.req

options.o	: options.b32 telnet.req option.req

option.req	: option.req telnet.req

routines.o	: routines.b32 telnet.req print.req

condition.o	: condition.b32 telnet.req print.req

log_util.o	: log_util.b32

kit		: telnet032.b telnet.src
	continue

telnet032.b	: telnet_link.com \
		telnet.opt \
		telnet.a \
		telnet.hlp telnet.cld telnet.hlb
	set prot=(w:re) $(mms$source_list)
	purge $(mms$source_list)
	backup $(mms$source_list) $(mms$target)/save/block=8192/inter

telnet.src	: \
	telnet_link.com \
	telnet.opt \
	telnet_exe.com \
	descrip.mms \
	telnet.cld \
	telnet_parse.cld \
	telnet.hlp \
	telnetcmd.hlp \
	telnetmsg.msg \
	cli.req \
	option.req \
	print.req \
	telnet.req \
	ttyloc.req \
	condition.b32 \
	key.b32 \
	options.b32 \
	port.b32 \
	print.b32 \
	routines.b32 \
	telnet.b32 \
	telnet_common.b32 \
	telnet_term.b32 \
	ttyloc.b32 \
	log_util.b32
	purge $(mms$source_list)
	backup $(mms$source_list) $(mms$target)/save/block=8192/inter

#  DEC/CMS REPLACEMENT HISTORY, Element DESCRIP.MMS
#  *2     5-MAR-1991 19:49:00 SYNFUL "Included checking for TELNET.OPT and TELNET_SERVER.OPT"
#  *1     5-MAR-1991 18:57:06 SYNFUL "Descrip.Mms for IP/TCP Telnet program"
#  DEC/CMS REPLACEMENT HISTORY, Element DESCRIP.MMS

%.req: %.r32
	ln -s $< $@

telnetmsg.o:
	touch telnetmsg.o

%.o: %.b32
	$(CC) -c $< -o $@

starlet.req:
	ln -s ../../../starlet/src/starlet.req .

tpamac.req:
	ln -s ../../../starlet/src/tpamac.req .

netaux.req:
	ln -s ../../central/netaux.req .

netcommon.req:
	ln -s ../../central/netcommon.req .

neterror.req:
	ln -s ../../central/neterror.req .

network.req:
	ln -s ../../central/network.req .

netxport.req:
	ln -s ../../central/netxport.req .

netconfig.req:
	ln -s ../../central/netconfig.req .

links: starlet.req netaux.req netcommon.req neterror.req network.req netxport.req netconfig.req
