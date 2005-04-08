$!
$!	****************************************************************
$!
$!		Copyright (c) 1992, Carnegie Mellon University
$!
$!		All Rights Reserved
$!
$!	Permission  is  hereby  granted   to  use,  copy,  modify,  and
$!	distribute  this software  provided  that the  above  copyright
$!	notice appears in  all copies and that  any distribution be for
$!	noncommercial purposes.
$!
$!	Carnegie Mellon University disclaims all warranties with regard
$!	to this software.  In no event shall Carnegie Mellon University
$!	be liable for  any special, indirect,  or consequential damages
$!	or any damages whatsoever  resulting from loss of use, data, or
$!	profits  arising  out of  or in  connection  with  the  use  or
$!	performance of this software.
$!
$!	****************************************************************
$!
$ WRITE SYS$OUTPUT "	Linking Telnet client"
$ LINK /NODEB/NOTRACE /EXE='P1'Telnet -
	'P1'Telnet.OLB/lib/include=(Telnet),-
	'P1'Telnet.opt/OPT,-
	'P1'NETERROR.OBJ, -
	'P1'NETLIB.OLB/lib
$
$!  DEC/CMS REPLACEMENT HISTORY, Element TELNET_LINK.COM
$!  *1     5-MAR-1991 19:05:58 SYNFUL "VMSINSTAL command procedure to LINK the Telnet images"
$!  DEC/CMS REPLACEMENT HISTORY, Element TELNET_LINK.COM
