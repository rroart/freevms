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
$ WRITE SYS$OUTPUT "	Linking IPACP"
$ LINK /EXE='P1'IPACP /NOTRACE/NODEB -
	'P1'IPACP.OLB/LIBRARY/INCLUDE=(IPACP),-
	'P1'SNMP.OLB/LIBRARY/INCLUDE=(SNMP_AGENT),-
	'P1'RPC.OLB/LIBRARY,-
	'P1'NETLIB.OLB/LIB,-
	'P1'NETERROR.OBJ,-
	'P1'IPACP.OPT/OPT, -
	SYS$SYSTEM:SYS.STB/SEL
$
