!
!	****************************************************************
!
!		Copyright (c) 1992, Carnegie Mellon University
!
!		All Rights Reserved
!
!	Permission  is  hereby  granted   to  use,  copy,  modify,  and
!	distribute  this software  provided  that the  above  copyright
!	notice appears in  all copies and that  any distribution be for
!	noncommercial purposes.
!
!	Carnegie Mellon University disclaims all warranties with regard
!	to this software.  In no event shall Carnegie Mellon University
!	be liable for  any special, indirect,  or consequential damages
!	or any damages whatsoever  resulting from loss of use, data, or
!	profits  arising  out of  or in  connection  with  the  use  or
!	performance of this software.
!
!	****************************************************************
!
!
!	Error messages for network functions
!
!	Version 1-0
!
! Author: Vince Fuller, CMU-CSD, May 1986
!

	.TITLE		Network error messages
	.FACILITY	IPACP, 99/PREFIX=NET$_

.SEVERITY	ERROR
	IR	<Insufficient system resources>
	IFC	<Invalid network function code>
	IPC	<Invalid network protocol code>
	UCT	<Unable to create TCB>
	IFS	<Invalid foreign port>
	ILP	<Invalid local port>
	NUC	<Non-unique connection specified>
	CSE	<Connection table space exhausted>
	NOPRV	<No privilege for access to well-known port>
	CIP	<Connection illegal for this process>
	CDE	<Connection does not exist>
	CR	<Connection RESET by remote host>
	FSU	<Foreign host unspecified>
	UNN	<Unknown network node>
	VTF	<Valid-TCB table full (all connections in use)>
	CREF	<Connection refused by remote host>
	CCAN	<Connection cancelled by process abort>
	FIP	<IO Function in Progress>
	BTS	<User specified buffer is Too small>
	IHI	<Invalid known host index>
	BDI	<Bad device index>
	EPD	<Error processing device dump>
	URC	<Destination Unreachable>
	IGF	<Invalid GTHST function>
	UNA	<Unknown host name>
	UNU	<Unknown host number>
	CC	<Connection is closing - operation invalid>
	CTO	<Connection TimeOut>
	TWT	<Time-Wait TimeOut>
	TE	<Net ACP is Exiting>
	FTO	<User function timeout. Network event didn't happen>
	NYI	<User function not yet implemented>
	NOPN	<UDP wildcard connection not yet open>
	NOINA	<Internet access not allowed>
	NOANA	<ARPANET access not allowed>
	NOADR	<Name lookup not yet complete - no host address>
	GTHFUL	<GTHST queue full - can't queue name/address lookup>
	DAE	<NET$DUMP argument error>
	NMLTO	<Host name lookup time-out>
	NSEXIT	<Name lookup failure: name resolver is exiting>
	NONS	<Name lookup failure: name resolver is not running>
	NSQFULL	<Name lookup failure: name resolver queue is full>
	DSDOWN	<Name not found in host table (domain service unavailable)>
	DSNODS	<Domain service: no domain servers could be contacted>
	DSINCOMP  <Domain service: received incomplete domain server reply>
	DSNOADDR  <Domain service: no addresses found for host name>
	DSNONAME  <Domain service: no name found for IP address>
	DSFMTERR  <Domain service: domain server returned format error>
	DSSRVERR  <Domain service: domain server error>
	DSNAMERR  <Domain service: name error (no such name)>
	DSNOTIMP  <Domain service: domain server returned not implemented>
	DSREFUSD  <Domain service: domain server refused to resolve name>
	DSNONSRV  <Domain service: no usable servers returned by referral>
	DSUNKERR  <Domain service: unknown server error>
	DSREFEXC  <Domain service: maximum referral limit exceeded>
	GREENERR  <Name lookup failure, unknown error code>
	GP_INVREQ <Name lookup: invalid request (internal error)>
	GP_INVINF <Name lookup: invalid host info item (internal error)>
	GP_INVNAM <Name lookup: invalid domain name syntax (internal error)>
	GP_INVADR <Name lookup: invalid address syntax (internal error)>
	GP_INVMBX <Name lookup: invalid mailbox syntax (internal error)>
	GP_INVCLS <Name lookup: invalid domain class (internal error)>
	GP_RSBUSY <Name lookup: resolver to busy to handle query>
	GP_NONMSR <Name lookup: no name server could be found>
	GP_NOHINF <Name lookup: request host info item does not exist>
	GP_NOTFND <Name lookup: host name not found>
	GP_UNKMBX <Name lookup: mailbox not found>
	GP_NOTIMP <Name lookup: unimplemented request (internal error)>
	GP_TOOBIG <Name lookup: request data too big for UDP>
	GP_NSDOWN <Name lookup: all name servers declared down>
	NRT	<No route found for destination>
	KILL	<Connection killed>
