/*
	****************************************************************

		Copyright (c) 1992, Carnegie Mellon University

		All Rights Reserved

	Permission  is  hereby  granted   to  use,  copy,  modify,  and
	distribute  this software  provided  that the  above  copyright
	notice appears in  all copies and that  any distribution be for
	noncommercial purposes.

	Carnegie Mellon University disclaims all warranties with regard
	to this software.  In no event shall Carnegie Mellon University
	be liable for  any special, indirect,  or consequential damages
	or any damages whatsoever  resulting from loss of use, data, or
	profits  arising  out of  or in  connection  with  the  use  or
	performance of this software.

	****************************************************************
*/
//TITLE "SNMP_HOOK - Simple Network Management service"
/*
Facility:

	SNMP_HOOK.BLI - Provide SMNP service under UDP (RFC 1098)

Abstract:

	Supports the Simple Network Management Protocol (SMNP) for
	the IPACP.  Provides the ability to read and write MIB variables.

	There are four entry points into this module:

	SNMP$INIT :
		Initialize the module.
	SNMP$NET_INPUT :
		Where UDP/SNMP packets are delivered.
	SNMP$NET_INPUT :
		Provides the IPACP access to SNMP.
	getStatPtr :
		Entry point from the generic SNMP processing code.

Author:

	Bruce R. Miller, CMU NetDev, 8am Thursday morn. 2/22/90
	Copyright (c) 1990, Bruce R. Miller and Carnegie-Mellon University

Module Modification History:

07/18/90 Bruce R. Miller	CMU Network Development
	 Merged in SNMP_vars.bli

Copywrite:

***********************************************************
	Copyright 1990 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY && FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT || CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA || PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE || OTHER TORTIOUS ACTION,
ARISING OUT OF || IN CONNECTION WITH THE USE || PERFORMANCE OF THIS
SOFTWARE.
******************************************************************

*/

MODULE SNMP_HOOK(IDENT="1.0",LANGUAGE(BLISS32),
	      ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			      NONEXTERNAL=LONG_RELATIVE),
	      LIST(REQUIRE,ASSEMBLY,OBJECT,BINARY),
	      OPTIMIZE,OPTLEVEL=3,ZIP)=
{

#include "SYS$LIBRARY:STARLET";	// VMS system definitions
#include "CMUIP_SRC:[CENTRAL]NETXPORT";	// BLISS common definitions
!LIBRARY "CMUIP_SRC:[CENTRAL]NETERROR";	// Network error codes
#include "CMUIP_SRC:[CENTRAL]NETCONFIG";			// VMS-specific definitions
#include "TCPMACROS";		// System-wide Macro definitions
!LIBRARY "STRUCTURE";		// System-wide structure definitions
#include "ASN1";			// ASN defs
#include "SNMP";			// SNMP definitions
#include "TCP";			// TCP definitions

signed long
    SNMP_SERVICE : INITIAL (0);



SNMP$INIT = 
    {
    SS$_NORMAL
    };

extern
    SNMP_INPUT;

SNMP$NET_INPUT ( SrcAddr , DstAdr , SrcPrt , DstPrt ,
			    Size , Buff , out_buff, out_len)
    {
    signed long
	RC;

    out_len = 512;

    NOINT;
    RC = snmp_input(buff,Size,out_buff,out_len);
    OKINT;

    SS$_NORMAL
    };

SNMP$USER_INPUT (in_buff, in_len, out_buff, out_len)
    {
    signed long
	RC;

    NOINT;
    RC = snmp_input(in_buff,in_len,out_buff,out_len);
    OKINT;

    SS$_NORMAL
    };


!
// The rest of the module Provides access to the IPACP's MIB variables
!


static signed long
    long_return;


/*
 * Each variable name is placed in the variable table, without the terminating
 * substring that determines the instance of  the variable.   When  a string is
 * found that is lexicographicly preceded by the input string, the function for
 * that entry is called to find  the method  of access  of the  instance of the
 * named variable.  If that variable is not found, NULL is returned, and the
 * search through the table continues (it should stop at the next entry).  If
 * it is found, the function returns a character pointer and a length or a
 * function pointer.  The former is the address of the operand, the latter is
 * an access routine for the variable.  
 *
!!!HACK!!// Update these comments
 * u_char *
 * findVar(name, length, exact, var_len, access_method)
 * oid	    *name;	    IN/OUT - input name requested, output name found
 * int	    length;	    IN/OUT - number of sub-ids in the in and out oid's
 * int	    exact;	    IN - TRUE if an exact match was requested.
 * int	    len;	    OUT - length of variable or 0 if function returned.
 * int	    access_method; OUT - 1 if function, 0 if char pointer.
 *
 * accessVar(rw, var, varLen)
 * int	    rw;	    IN - request to READ or WRITE the variable
 * u_char   *var;   IN/OUT - input or output buffer space
 * int	    *varLen;IN/OUT - input and output buffer len
*/

LITERAL
    SNMP$K_OIDsize = 4;

$FIELD variable_fields (void)
    SET
    var$name	= [$Bytes(24*SNMP$K_OIDsize)], // object identifier of variable
    var$namelen	= [$Byte],	// /* length of above */
    var$type	= [$Byte],	// type of variable, INTEGER or (octet) STRING
    var$magic	= [$Byte],	// passed to function as a hint
    var$acl	= [$UWord],	// access control list for variable
    var$findVar	= [$Address]	// function that finds variable
    TES;

LITERAL
    MIBmax_size	= 3+21+19+4+26+12+5+4+2,	// Add 2 as a fudge factor...
    VAR_Block_Size = $Field_Set_Size;

MACRO
    variable_struct = BLOCK->VAR_Block_Size FIELD(variable_fields)%;

COMPILETIME
    $mibcnt = -1,
    $mibtmp = 0;

MACRO

    $MIBstart (void)
	%ASSIGN( $mibcnt, -1 )
	PRESET (
	%,

    $MIBoid->oid =
	%IF NOT %NULL(oid) %THEN
	[$MIBcnt,$mibtmp,0,8*SNMP$K_OIDsize,0] = oid
	%FI
	%ASSIGN($mibtmp,$mibtmp+1)
	%,

    $MIBent(rtn,nlen,type,magic,acl)
	%ASSIGN($mibcnt,$mibcnt+1)
	%ASSIGN($mibtmp,0)
	$MIBoid(%REMAINING),
	[$MIBcnt,var$namelen]	= nlen,
	[$MIBcnt,var$type]	= type,
	[$MIBcnt,var$magic]	= magic,
	[$MIBcnt,var$acl]	= acl,
	[$MIBcnt,var$findVar]	= rtn
	%,

    $MIBend (void)
	)%;

MACRO
    MIB = 1,3,6,1,2,1%;


// Declare the variable access routine which are used in the MIB
FORWARD ROUTINE
    VAR_SYSTEM,
    VAR_IFENTRY,
    VAR_IP,
    VAR_IPADDRENTRY,
    VAR_ICMP,
    VAR_UDP,
    VAR_TCP,
    VAR_TCPCONN;

// Declare the MIB dispatch block
!!!HACK!!// should we make this global
signed long
    variables : BLOCKVECTOR[MIBmax_size,VAR_Block_Size] FIELD(variable_fields)

    // Define the contents of the MIB blockvector
$MIBstart 
// these must be lexicographly ordered by the name field

// System Table
$MIBent( var_system,  9, STRING,   VERSION_DESCR, RONLY,
						MIB, 1, 1, 0),
$MIBent( var_system,  9, OBJID,    VERSION_ID, RONLY,
						MIB, 1, 2, 0),
$MIBent( var_system,  9, TIMETICKS,UPTIME, RONLY,
						MIB, 1, 3, 0),

// Interfaces Table
$MIBent( var_system,  9, INTEGER,  CFG_NNETS, RONLY,
						MIB, 2, 1, 0),
$MIBent( var_ifEntry, 11, INTEGER, IFINDEX, RONLY, MIB, 2, 2, 1, 1, %x"FF"),
$MIBent( var_ifEntry, 11, STRING,  IFDESCR, RONLY,
						MIB, 2, 2, 1, 2, %x"FF"),
$MIBent( var_ifEntry, 11, INTEGER, IFTYPE, RONLY,
						MIB, 2, 2, 1, 3, %x"FF"),
$MIBent( var_ifEntry, 11, INTEGER, IFMTU, RONLY,
						MIB, 2, 2, 1, 4, %x"FF"),
$MIBent( var_ifEntry, 11, GAUGE,   IFSPEED, RONLY,
						MIB, 2, 2, 1, 5, %x"FF"),
$MIBent( var_ifEntry, 11, STRING,  IFPHYSADDRESS, RONLY,
						MIB, 2, 2, 1, 6, %x"FF"),
$MIBent( var_ifEntry, 11, INTEGER, IFADMINSTATUS, RWRITE,
						MIB, 2, 2, 1, 7, %x"FF"),
$MIBent( var_ifEntry, 11, INTEGER, IFOPERSTATUS, RONLY,
						MIB, 2, 2, 1, 8, %x"FF"),
$MIBent( var_ifEntry, 11, TIMETICKS, IFLASTSTATE, RONLY,
						MIB, 2, 2, 1, 9, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFINOCTETS, RONLY,
						MIB, 2, 2, 1, 10, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFINUCASTPKTS, RONLY,
						MIB, 2, 2, 1, 11, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFINNUCASTPKTS, RONLY,
						MIB, 2, 2, 1, 12, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFINDISCARDS, RONLY,
						MIB, 2, 2, 1, 13, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFINERRORS, RONLY,
						MIB, 2, 2, 1, 14, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFINUNKNOWNPROTOS, RONLY,
						MIB, 2, 2, 1, 15, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFOUTOCTETS, RONLY,
						MIB, 2, 2, 1, 16, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFOUTUCASTPKTS, RONLY,
						MIB, 2, 2, 1, 17, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFOUTNUCASTPKTS, RONLY,
						MIB, 2, 2, 1, 18, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFOUTDISCARDS, RONLY,
						MIB, 2, 2, 1, 19, %x"FF"),
$MIBent( var_ifEntry, 11, COUNTER, IFOUTERRORS, RONLY,
						MIB, 2, 2, 1, 20, %x"FF"),
$MIBent( var_ifEntry, 11, GAUGE,   IFOUTQLEN, RONLY,
						MIB, 2, 2, 1, 21, %x"FF"),
$MIBent( var_ip,  9, INTEGER, IPFORWARDING, RONLY,	MIB, 4, 1, 0),
$MIBent( var_ip,  9, INTEGER, IPDEFAULTTTL, RONLY,	MIB, 4, 2, 0),
$MIBent( var_ip,  9, COUNTER, IPINRECEIVES, RONLY,	MIB, 4, 3, 0),
$MIBent( var_ip,  9, COUNTER, IPINHDRERRORS, RONLY,	MIB, 4, 4, 0),
$MIBent( var_ip,  9, COUNTER, IPINADDRERRORS, RONLY,	MIB, 4, 5, 0),
$MIBent( var_ip,  9, COUNTER, IPFORWDATAGRAMS, RONLY,	MIB, 4, 6, 0),
$MIBent( var_ip,  9, COUNTER, IPINUNKNOWNPROTOS, RONLY,	MIB, 4, 7, 0),
$MIBent( var_ip,  9, COUNTER, IPINDISCARDS, RONLY,	MIB, 4, 8, 0),
$MIBent( var_ip,  9, COUNTER, IPINDELIVERS, RONLY,	MIB, 4, 9, 0),
$MIBent( var_ip,  9, COUNTER, IPOUTREQUESTS, RONLY,	MIB, 4, 10, 0),
$MIBent( var_ip,  9, COUNTER, IPOUTDISCARDS, RONLY,	MIB, 4, 11, 0),
$MIBent( var_ip,  9, COUNTER, IPOUTNOROUTES, RONLY,	MIB, 4, 12, 0),
$MIBent( var_ip,  9, INTEGER, IPREASMTIMEOUT, RONLY,	MIB, 4, 13, 0),
$MIBent( var_ip,  9, COUNTER, IPREASMREQDS, RONLY,	MIB, 4, 14, 0),
$MIBent( var_ip,  9, COUNTER, IPREASMOKS, RONLY,	MIB, 4, 15, 0),
$MIBent( var_ip,  9, COUNTER, IPREASMFAILS, RONLY,	MIB, 4, 16, 0),
$MIBent( var_ip,  9, COUNTER, IPFRAGOKS, RONLY,		MIB, 4, 17, 0),
$MIBent( var_ip,  9, COUNTER, IPFRAGFAILS, RONLY,	MIB, 4, 18, 0),
$MIBent( var_ip,  9, COUNTER, IPFRAGCREATES, RONLY,	MIB, 4, 19, 0),

// IP Address Table
$MIBent( var_ipAddrEntry, 14, IPADDRESS, IPADADDR, RONLY,
			MIB, 4, 20, 1, 1, %x"FF', %x'FF', %x'FF', %x'FF"),
$MIBent( var_ipAddrEntry, 14, INTEGER,	IPADIFINDEX, RONLY,
			MIB, 4, 20, 1, 2, %x"FF', %x'FF', %x'FF', %x'FF"),
$MIBent( var_ipAddrEntry, 14, IPADDRESS, IPADNETMASK, RONLY,
			MIB, 4, 20, 1, 3, %x"FF', %x'FF', %x'FF', %x'FF"),
$MIBent( var_ipAddrEntry, 14, INTEGER,	IPADBCASTADDR, RONLY,
			MIB, 4, 20, 1, 4, %x"FF', %x'FF', %x'FF', %x'FF"),

// ICMP group
$MIBent( var_icmp, 9, COUNTER, ICMPINMSGS, RONLY, 		MIB, 5, 1, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINERRORS, RONLY, 		MIB, 5, 2, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINDESTUNREACHS, RONLY,	MIB, 5, 3, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINTIMEEXCDS, RONLY, 		MIB, 5, 4, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINPARMPROBS, RONLY, 		MIB, 5, 5, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINSRCQUENCHS, RONLY, 	MIB, 5, 6, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINREDIRECTS, RONLY, 		MIB, 5, 7, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINECHOS, RONLY, 		MIB, 5, 8, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINECHOREPS, RONLY, 		MIB, 5, 9, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINTIMESTAMPS, RONLY, 	MIB, 5, 10, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINTIMESTAMPREPS, RONLY, 	MIB, 5, 11, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINADDRMASKS, RONLY, 		MIB, 5, 12, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPINADDRMASKREPS, RONLY, 	MIB, 5, 13, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTMSGS, RONLY, 		MIB, 5, 14, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTERRORS, RONLY,		MIB, 5, 15, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTDESTUNREACHS, RONLY,	MIB, 5, 16, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTTIMEEXCDS, RONLY, 	MIB, 5, 17, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTPARMPROBS, RONLY, 	MIB, 5, 18, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTSRCQUENCHS, RONLY, 	MIB, 5, 19, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTREDIRECTS, RONLY, 	MIB, 5, 20, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTECHOS, RONLY, 		MIB, 5, 21, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTECHOREPS, RONLY, 		MIB, 5, 22, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTTIMESTAMPS, RONLY, 	MIB, 5, 23, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTTIMESTAMPREPS, RONLY, 	MIB, 5, 24, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTADDRMASKS, RONLY, 	MIB, 5, 25, 0),
$MIBent( var_icmp, 9, COUNTER, ICMPOUTADDRMASKREPS, RONLY, 	MIB, 5, 26, 0),

// TCP group
$MIBent( var_tcp, 9, INTEGER, TCPRTOALGORITHM, RONLY,	MIB, 6, 1, 0),
$MIBent( var_tcp, 9, INTEGER, TCPRTOMIN, RONLY,		MIB, 6, 2, 0),
$MIBent( var_tcp, 9, INTEGER, TCPRTOMAX, RONLY, 	MIB, 6, 3, 0),
$MIBent( var_tcp, 9, INTEGER, TCPMAXCONN, RONLY, 	MIB, 6, 4, 0),
$MIBent( var_tcp, 9, COUNTER, TCPACTIVEOPENS, RONLY, 	MIB, 6, 5, 0),
$MIBent( var_tcp, 9, COUNTER, TCPPASSIVEOPENS, RONLY, 	MIB, 6, 6, 0),
$MIBent( var_tcp, 9, COUNTER, TCPATTEMPTFAILS, RONLY, 	MIB, 6, 7, 0),
$MIBent( var_tcp, 9, COUNTER, TCPESTABRESETS, RONLY, 	MIB, 6, 8, 0),
$MIBent( var_tcp, 9, GAUGE,	TCPCURRESTAB, RONLY, 	MIB, 6, 9, 0),
$MIBent( var_tcp, 9, COUNTER, TCPINSEGS, RONLY, 	MIB, 6,10, 0),
$MIBent( var_tcp, 9, COUNTER, TCPOUTSEGS, RONLY, 	MIB, 6,11, 0),
$MIBent( var_tcp, 9, COUNTER, TCPRETRANSSEGS, RONLY, 	MIB, 6,12, 0),

// TCP connection table
$MIBent( var_tcpConn, 20, INTEGER, TCPCONNSTATE, RONLY,
	 MIB, 6, 13, 1, 1,
	 %x"FF', %x'FF', %x'FF', %x'FF', %x'FF",
	 %x"FF', %x'FF', %x'FF', %x'FF', %x'FF"),
$MIBent( var_tcpConn, 20, IPADDRESS, TCPCONNLOCALADDRESS, RONLY,
	 MIB, 6, 13, 1, 2,
	 %x"FF', %x'FF', %x'FF', %x'FF', %x'FF",
	 %x"FF', %x'FF', %x'FF', %x'FF', %x'FF"),
$MIBent( var_tcpConn, 20, INTEGER, TCPCONNLOCALPORT, RONLY,
	 MIB, 6, 13, 1, 3,
	 %x"FF', %x'FF', %x'FF', %x'FF', %x'FF",
	 %x"FF', %x'FF', %x'FF', %x'FF', %x'FF"),
$MIBent( var_tcpConn, 20, IPADDRESS, TCPCONNREMADDRESS, RONLY,
	 MIB, 6, 13, 1, 4,
	 %x"FF', %x'FF', %x'FF', %x'FF', %x'FF",
	 %x"FF', %x'FF', %x'FF', %x'FF', %x'FF"),
$MIBent( var_tcpConn, 20, INTEGER, TCPCONNREMPORT, RONLY,
	 MIB, 6, 13, 1, 5,
	 %x"FF', %x'FF', %x'FF', %x'FF', %x'FF",
	 %x"FF', %x'FF', %x'FF', %x'FF', %x'FF"),


// UDP group
$MIBent( var_udp, 9, COUNTER, UDPINDATAGRAMS, RONLY,	MIB, 7, 1, 0),
$MIBent( var_udp, 9, COUNTER, UDPINNOPORTS, RONLY,	MIB, 7, 2, 0),
$MIBent( var_udp, 9, COUNTER, UDPINERRORS, RONLY,	MIB, 7, 3, 0),
$MIBent( var_udp, 9, COUNTER, UDPOUTDATAGRAMS, RONLY,	MIB, 7, 4, 0)


$MIBend;

LITERAL
    MIBsize = $MIBcnt+1;

!GLOBAL
//    return_buf : VECTOR[64,BYTE];

FORWARD ROUTINE
    compare;

getStatPtr (name,namelen,type,len,acl,exact,access_method)
!
// getStatPtr - return a pointer to the named variable, as well as it's
// type, length, and access control list.
!
// If an exact match for the variable name exists, it is returned.  If not,
// and exact is false, the next variable lexicographically after the
// requested one is returned.
!
// If no appropriate variable can be found, NULL is returned.
!
// The C language interface looks like this:
//    oid		*name;	    /* IN - name of var, OUT - name matched */
//    int		*namelen;   /* IN -number of sub-ids in name, OUT - subid-is in matched name */
//    u_char	*type;	    /* OUT - type of matched variable */
//    int		*len;	    /* OUT - length of matched variable */
//    u_short	*acl;	    /* OUT - access control list */
//    int		exact;	    /* IN - TRUE if exact match wanted */
//    int		*access_method; /* OUT - 1 if function, 0 if char * */
!
{
    MAP
	struct VECTOR * name[,SNMP$K_OIDsize];
    signed long
	found  = 0,
	access,
	result,
	struct variable_struct * vp;

    INCR x FROM 0 TO MIBsize DO
	{
	// Point to Xth entry in MIB table
	vp = variables[x,0,0,0,0];

	// compare should be expanded inline.
	result = compare(name, ..namelen, vp->var$name, vp->var$namelen);
	if (((result LSS 0) || (exact && (result == 0))))
	    {
	    access = (vp->var$findVar)(vp, name, namelen,
					.exact, len, access_method);
	    if ((access != 0))
		EXITLOOP found = 1
	    }
	};

    // did we find one?
    if (NOT found) RETURN 0;

    // vp now points to the approprate struct */
    type = vp->var$type;
    acl = vp->var$acl;
    return access;
    };


compare (name1, len1, name2, len2)
    {
    MAP
	NAME1 : REF VECTOR [],
	NAME2 : REF VECTOR [];
    signed long
	len;

    // len = minimum of len1 and len2
    if ((len1 LSS len2)) len = len1
    else len = len2;

    // find first non-matching OID */
    INCR i FROM 0 to len-1 DO
	{
	if ((name1[i] LSS name2[i])) return -1;
	if ((name2[i] LSS name1[i])) return 1;
	};

    // OIDs match up to length of shorter string
    if ((len1 LSS len2)) RETURN -1;  // name1 shorter, so it is "less"
    if ((len2 LSS len1)) RETURN 1;

    RETURN 0	// both strings are equal
    };

cmp_ipaddr (ip1,ip2)
// Lexicographically compare two ip addresses.
    {
    MAP
	ip1 : VECTOR [4,BYTE],
	ip2 : VECTOR [4,BYTE];

    // find first non-matching BYTE */
    INCR i FROM 0 to 3 DO
	{
	if ((ip1[i] LSS ip2[i])) return -1;
	if ((ip2[i] LSS ip1[i])) return 1;
	};

    RETURN 0	// both IP addresses are equal
    };

cmp_TCB (TCB1,TCB2)
// Lexicographically compare two ip addresses.
    {
    MAP
	TCB1 : REF TCB_structure,
	TCB2 : REF TCB_structure;
    signed long
	result;

    // Everything is less than the NULL TCB
    if (TCB2 EQLA 0) RETURN -1;

    // compare local address
    result = cmp_ipaddr(TCB1->local_host, TCB2->local_host);
    if (result != 0) RETURN result;

    // compare local ports
    if (TCB1->local_port LSSU TCB2->local_port) RETURN -1;
    if (TCB2->local_port LSSU TCB1->local_port) RETURN 1;

    // compare remote address
    result = cmp_ipaddr(TCB1->foreign_host, TCB2->foreign_host);
    if (result != 0) RETURN result;

    // compare remote ports
    if (TCB1->foreign_port LSSU TCB2->foreign_port) RETURN -1;
    if (TCB2->foreign_port LSSU TCB1->foreign_port) RETURN 1;

    RETURN 0
    };


/*
	The remainder of this module is filled with variable access
	routines which take parameters in the following format.

    /* IN - pointer to variable entry that points here */
    register struct variable *vp;

    /* IN/OUT - input name requested, output name found */
    register oid 	*name;

    /* IN/OUT - length of input and output oid's */
    register int	*length;

    /* IN - TRUE if an exact match was requested. */
    int			exact;

    /* OUT - length of variable or 0 if function returned. */
    int			*var_len;

    /* OUT - 1 if function, 0 if char pointer. */
    int			*access_method;

*/



!
// System Variables:
!

BIND
    version_description = %ASCID"CMU-OpenVMS/IP 6.6-5" : BLOCK[8,BYTE],
    version_ident = UPLIT(1, 3, 6, 1, 4, 1, 3, 1, 6) : VECTOR->9;

LITERAL
    version_ident_size = 9;

static signed long
    sys_nnets,
    sys_uptime;


var_system (vp, name, length, exact, var_len, access_method)
    {
    // Declare system variables from IPACP
    EXTERNAL
	Dev_Count;
    EXTERNAL ROUTINE
	CALCULATE_UPTIME;
    MAP
	struct variable_struct * vp,
        struct VECTOR * name[,SNMP$K_OIDsize];

    IF (exact && (compare(name,..length,vp->var$name,vp->var$namelen) != 0))
	THEN RETURN 0;
    ch$move(vp->var$namelen * SNMP$K_OIDsize, vp->var$name, name );

    length = vp->var$namelen;
    access_method = 0;
    var_len = 4;	// default length == 4 bytes

    SELECTONE (vp->var$magic) OF
	SET
	[VERSION_DESCR]:
	    {
	    var_len = version_description->DSC$W_LENGTH;
	    RETURN version_description->DSC$A_POINTER;
	    };
	[VERSION_ID]:
	    {
	    var_len = version_ident_size*SNMP$K_OIDsize;
	    RETURN version_ident;
	    };
	[CFG_NNETS]:
	    {
	    sys_nnets = Dev_Count;
	    RETURN sys_nnets;
	    };
	[UPTIME]:
	    {
	    sys_uptime = CALCULATE_UPTIME();
	    RETURN sys_uptime;
	    };
	[OTHERWISE]:
!!!HACK!!// Put some sort of erro logging in here...
	    0;
	TES;

    0
    };


var_ifEntry (vp, name, length, exact, var_len, access_method)
    {
    // Declare system variables from IPACP
    EXTERNAL 
	DEV_CONFIG_TAB : Device_Configuration_Table,
	Dev_Count;
    MAP
	struct variable_struct * vp,
        struct VECTOR * name[,SNMP$K_OIDsize];
    signed long
	newname : VECTOR[MAX_NAME_LEN,SNMP$K_OIDsize],
	interface,	// interface number
	result,
	struct Device_Configuration_Entry * dev_config;

    ch$move(vp->var$namelen * SNMP$K_OIDsize, vp->var$name, newname );

    // find "next" interface
    interface = 0;
    INCR i FROM 1 TO Dev_Count DO
	{
	newname->10 = i;
	result = compare(name, ..length, newname, vp->var$namelen);
	IF ((exact && (result == 0)) || (NOT exact && (result LSS 0)))
	    THEN EXITLOOP interface = i
	};

    if ((interface == 0)) RETURN 0;

    interface = interface - 1; // translate into internal index of interfaces
    ch$move(vp->var$namelen * SNMP$K_OIDsize, newname, name);

    length = vp->var$namelen;
    access_method = 0;
    var_len = 4;	// default length == 4 bytes

    dev_config = dev_config_tab[interface,dc_begin];
    SELECTONE (vp->var$magic) OF
	SET
	[IFINDEX]:
	    RETURN dev_config->dcmib_IFINDEX;
	[IFDESCR]:
	    {
	    var_len = .(dev_config->dcmib_ifDescr);
	    RETURN .(dev_config->dcmib_ifDescr+4)
	    };
	[IFTYPE]:
	    RETURN dev_config->dcmib_IFTYPE;
	[IFMTU]:
	    RETURN dev_config->dcmib_IFMTU;
	[IFSPEED]:
	    RETURN dev_config->dcmib_IFSPEED;
	[IFPHYSADDRESS]:
	    {
	    var_len = dev_config->dcmib_ifPAsize;
	    RETURN dev_config->dcmib_IFPHYSADDRESS
	    };
	[IFADMINSTATUS]:
	    RETURN dev_config->dcmib_IFADMINSTATUS;
	[IFOPERSTATUS]:
	    RETURN dev_config->dcmib_IFOPERSTATUS;
	[IFLASTSTATE]:
	    RETURN dev_config->dcmib_IFLASTSTATE;
	[IFINOCTETS]:
	    RETURN dev_config->dcmib_IFINOCTETS;
	[IFINUCASTPKTS]:
	    RETURN dev_config->dcmib_IFINUCASTPKTS;
	[IFINNUCASTPKTS]:
	    RETURN dev_config->dcmib_IFINNUCASTPKTS;
	[IFINDISCARDS]:
	    RETURN dev_config->dcmib_IFINDISCARDS;
	[IFINERRORS]:
	    RETURN dev_config->dcmib_IFINERRORS;
	[IFINUNKNOWNPROTOS]:
	    RETURN dev_config->dcmib_IFINUNKNOWNPROTOS;
	[IFOUTOCTETS]:
	    RETURN dev_config->dcmib_IFOUTOCTETS;
	[IFOUTUCASTPKTS]:
	    RETURN dev_config->dcmib_IFOUTUCASTPKTS;
	[IFOUTNUCASTPKTS]:
	    RETURN dev_config->dcmib_IFOUTNUCASTPKTS;
	[IFOUTDISCARDS]:
	    RETURN dev_config->dcmib_IFOUTDISCARDS;
	[IFOUTERRORS]:
	    RETURN dev_config->dcmib_IFOUTERRORS;
	[IFOUTQLEN]:
	    RETURN dev_config->dcmib_IFOUTQLEN;
	[OTHERWISE]:
!!!HACK!!// Put some sort of erro logging in here...
	    0;
	TES;
    0
    };



/*

/* 
 * from arp.c:
 * There is no arp.h, so this must be recreated here.
 */
#define	ARPHLNMAX	6	/* largest arp_hln value needed */
#define	ARPPLN		4	/* length of protocol address (IP) */
struct	arptab {
	iaddr_t at_iaddr;		/* internet address */
	u_char	at_haddr->ARPHLNMAX;	/* hardware address */
	u_char	at_timer;		/* minutes since last reference */
	u_char	at_flags;		/* flags */
	struct	pbuf *at_hold;		/* last packet until resolved/timeout */
};
/* at_flags field values */
#define	ATF_INUSE	1		/* entry in use */
#define ATF_COM		2		/* completed entry (haddr valid) */

#define	ARPTAB_BSIZ	5		/* bucket size */
#define	ARPTAB_NB	11		/* number of buckets */
#define	ARPTAB_SIZE	(ARPTAB_BSIZ * ARPTAB_NB)


u_char *
var_atEntry(vp, name, length, exact, var_len, access_method)
    register struct variable *vp;	/* IN - pointer to variable entry that points here */
    register oid	    *name;	/* IN/OUT - input name requested, output name found */
    register int	    *length;	/* IN/OUT - length of input and output oid's */
    int			    exact;	/* IN - TRUE if an exact match was requested. */
    int			    *var_len;	/* OUT - length of variable or 0 if function returned. */
    int			    *access_method; /* OUT - 1 if function, 0 if char pointer. */
{
    /*
     * object identifier is of form:
     * 1.3.6.1.2.1.3.1.1.1.interface.1.A.B.C.D,  where A.B.C.D is IP address.
     * Interface is at offset 10,
     * IPADDR starts at offset 12.
     */
    oid			    lowest->16;
    oid			    current->16;
    register struct arptab  *arp;
    struct arptab	    *lowarp = 0;
    extern struct arptab    arptab[];
    register struct ipdad   *ipdp;
    struct ipdad	    *lowipdp = 0;
    extern struct ipdad	    ipdad[];
    long		    ipaddr;
    int			    addrlen;
    extern struct conf	    conf;
    register u_char	    *cp;
    register oid	    *op;
    register int	    count;

    /* fill in object part of name for current (less sizeof instance part) */
    bcopy((char *)vp->name, (char *)current, (int)(vp->namelen - 6) * sizeof(oid));
    for(arp = arptab; arp < arptab + ARPTAB_SIZE; arp++){
	if (!(arp->at_flags & ATF_COM))	/* if this entry isn't valid */
	    continue;
	/* create new object id */
	current->10 = 1;	/* ifIndex == 1 (ethernet) */
	current->11 = 1;
	cp = (u_char *)&(arp->at_iaddr);
	op = current + 12;
	for(count = 4; count > 0; count--)
	    *op++ = *cp++;
	if (exact){
	    if (compare(current, 16, name, *length) == 0){
		bcopy((char *)current, (char *)lowest, 16 * sizeof(oid));
		lowarp = arp;
		break;	/* no need to search further */
	    }
	} else {
	    if ((compare(current, 16, name, *length) > 0) && (!lowarp || (compare(current, 16, lowest, 16) < 0))){
		/*
		 * if new one is greater than input and closer to input than
		 * previous lowest, save this one as the "next" one.
		 */
		bcopy((char *)current, (char *)lowest, 16 * sizeof(oid));
		lowarp = arp;
	    }
	}
    }
    ipaddr = conf.ipaddr + conf.ipstatic + 1;
    for(ipdp = ipdad; ipdp < ipdad + NIPDAD; ipdp++, ipaddr++){
	if (ipdp->timer == 0)	/* if this entry is unused, continue */
	    continue;
	/* create new object id */
	current->10 = 2;	/* ifIndex == 2 (appletalk) */
	current->11 = 1;
	cp = (u_char *)&ipaddr;
	op = current + 12;
	for(count = 4; count > 0; count--)
	    *op++ = *cp++;
	if (exact){
	    if (compare(current, 16, name, *length) == 0){
		bcopy((char *)current, (char *)lowest, 16 * sizeof(oid));
		lowipdp = ipdp;
		lowarp = 0;
		break;	/* no need to search further */
	    }
	} else {
	    if ((compare(current, 16, name, *length) > 0) && ((!lowarp && !lowipdp) || (compare(current, 16, lowest, 16) < 0))){
		/*
		 * if new one is greater than input and closer to input than
		 * previous lowest, save this one as the "next" one.
		 */
		bcopy((char *)current, (char *)lowest, 16 * sizeof(oid));
		lowipdp = ipdp;
		/* ipdad entry is lower, so invalidate arp entry */
		lowarp = 0;
	    }
	}
    }
    if (lowarp != 0){	/* arp entry was lowest */
	addrlen = 6;
	bcopy((char *)lowarp->at_haddr, (char *)return_buf, 6);
    } else if (lowipdp != 0) {
	addrlen = 3;
	/*
	 * As far as IP is concerned, the "physical" address includes the Appletalk
	 * network address as well as node number.
	 */
	return_buf->0 = ((u_char *)&lowipdp->net)[0];
	return_buf->1 = ((u_char *)&lowipdp->net)[1];
	return_buf->2 = lowipdp->node;
    } else
	return NULL;	/* no match */
    bcopy((char *)lowest, (char *)name, 16 * sizeof(oid));
    *length = 16;
    *access_method = 0;
    switch(vp->magic){
	[ATIFINDEX]:
	    *var_len = sizeof long_return;
	    long_return = lowest->10;
	    return (u_char *)&long_return;
	[ATPHYSADDRESS]:
	    *var_len = addrlen;
	    return (u_char *)return_buf;
	[ATNETADDRESS]:
	    *var_len = sizeof long_return;
	    cp = (u_char *)&long_return;
	    op = lowest + 12;
	    for(count = 4; count > 0; count--)
		*cp++ = *op++;
	    return (u_char *)&long_return;
	default:
	    ERROR("");
   }
   return NULL;
}
*/

var_IP (vp, name, length, exact, var_len, access_method)
    {
    EXTERNAL
	IP_group_MIB;
    MAP
	struct variable_struct * vp,
        struct VECTOR * name[,SNMP$K_OIDsize];

    IF (exact && (compare(name,..length,vp->var$name,vp->var$namelen) != 0))
	THEN RETURN 0;
    ch$move(vp->var$namelen * SNMP$K_OIDsize, vp->var$name, name );

    length = vp->var$namelen;
    access_method = 0;
    var_len = 4;	// default length == 4 bytes

    RETURN IP_group_MIB + (vp->var$magic-1)*4
    };

/*

u_char *
var_ipRouteEntry(vp, name, length, exact, var_len, access_method)
    register struct variable *vp;   /* IN - pointer to variable entry that points here */
    register oid    	*name;	    /* IN/OUT - input name requested, output name found */
    register int	*length;    /* IN/OUT - length of input and output strings */
    int			exact;	    /* IN - TRUE if an exact match was requested. */
    int			*var_len;   /* OUT - length of variable or 0 if function returned. */
    int			*access_method; /* OUT - 1 if function, 0 if char pointer. */
{
    oid			    newname->MAX_NAME_LEN;
    register int	    entry;
    register struct mib_ipRouteEntry	*routep;
    int			    result;
    register int	    count;
    register u_char	    *cp;
    register oid	    *op;
    extern struct conf	    conf;

    /* set up a routing table to search. All other values are set at startup. */
    routep = mib_ipRouteEntry;
    routep->ROUTE_DEFAULT.ipRouteDest = 0;
    routep->ROUTE_DEFAULT.ipRouteNextHop = conf.iproutedef;
    routep->ROUTE_LOCAL.ipRouteDest = ipnetpart(conf.ipaddr);
    routep->ROUTE_LOCAL.ipRouteNextHop = conf.ipaddr;

    bcopy((char *)vp->name, (char *)newname, (int)vp->namelen * sizeof(oid));
    /* find "next" route */
    for(entry = 0; entry < ROUTE_ENTRIES; entry++){
	cp = (u_char *)&routep->ipRouteDest;
	op = newname + 10;
	for(count = 4; count > 0; count--)
	    *op++ = *cp++;
	result = compare(name, *length, newname, (int)vp->namelen);
	if ((exact && (result == 0)) || (!exact && (result < 0)))
	    break;
	routep++;
    }
    if (entry >= ROUTE_ENTRIES)
	return NULL;
    bcopy((char *)newname, (char *)name, (int)vp->namelen * sizeof(oid));
    *length = vp->namelen;
    *access_method = 0;
    *var_len = sizeof(long);

    routep = &mib_ipRouteEntry->entry;
    switch (vp->magic){
	[IPROUTENEXTHOP]:
	    if (entry == ROUTE_DEFAULT)
		return (u_char *)&conf.iproutedef;
	    else
		return (u_char *)&conf.ipaddr;
	default:
	    return (u_char *)(((u_char *)routep) + vp->magic);
    }
}

*/

var_ipAddrEntry (vp, name, length, exact, var_len, access_method)
    {
    EXTERNAL 
	DEV_CONFIG_TAB : Device_Configuration_Table,
	Dev_Count;
    MAP
	struct variable_struct * vp,
        struct VECTOR * name[,SNMP$K_OIDsize];
    signed long
	newname : VECTOR[MAX_NAME_LEN,SNMP$K_OIDsize],
	result,
	interface,
	IPaddr : UNSIGNED LONG,
	best : UNSIGNED LONG INITIAL(%x"FFFFFFFF");

    ch$move(vp->var$namelen * SNMP$K_OIDsize, vp->var$name, newname );

    //  Now find "next" ipaddr.

    //  Foreach ipaddress entry, cobble up newname with its IP address,
    //  by copying the ipaddress into the 10 - 13"t subid"s
    //  then compare with name.  If greater than name and less than lowest,
    //  save as new lowest.

    // find "next" interface
    interface = -1;
    INCR i FROM 0 TO Dev_Count-1 DO
	{

	// "Cobble" in the new name
	IPaddr = DEV_CONFIG_TAB[i,dc_ip_address];
	newname->13 = IPaddr<24,8,0>;  newname->12 = IPaddr<16,8,0>;
	newname->11 = IPaddr<8,8,0>;  newname->10 = IPaddr<0,8,0>;

	result = compare(name, ..length, newname, vp->var$namelen);
	if ((NOT exact && (result LSS 0)))
	    if (cmp_ipaddr(IPaddr,best) LSS 0)
		{
		best = IPaddr;
		interface = i
		};
	IF (exact && (result == 0))
	    THEN EXITLOOP interface = i;
	};

    if ((interface LSS 0)) RETURN 0;

    // "Cobble" in the new name
    IPaddr = DEV_CONFIG_TAB[interface,dc_ip_address];
    newname->13 = IPaddr<24,8,0>;  newname->12 = IPaddr<16,8,0>;
    newname->11 = IPaddr<8,8,0>;  newname->10 = IPaddr<0,8,0>;

    ch$move(vp->var$namelen * SNMP$K_OIDsize, newname, name );

    length = vp->var$namelen;
    access_method = 0;
    var_len = 4;	// default length == 4 bytes

    SELECTONE (vp->var$magic) OF
	SET
    	[IPADADDR]:
	    RETURN dev_config_tab[interface,dc_ip_address];
        [IPADIFINDEX]:
	    RETURN dev_config_tab[interface,dcmib_IFINDEX];
	[IPADNETMASK]:
	    RETURN dev_config_tab[interface,dc_ip_netmask];
	[IPADBCASTADDR]:
	    RETURN 0;
	TES;

    RETURN 0
    };



var_ICMP (vp, name, length, exact, var_len, access_method)
    {
    EXTERNAL
	ICMP_MIB;
    MAP
	struct variable_struct * vp,
        struct VECTOR * name[,SNMP$K_OIDsize];

    IF (exact && (compare(name,..length,vp->var$name,vp->var$namelen) != 0))
	THEN RETURN 0;
    ch$move(vp->var$namelen * SNMP$K_OIDsize, vp->var$name, name );

    length = vp->var$namelen;
    access_method = 0;
    var_len = 4;	// default length == 4 bytes

    RETURN ICMP_MIB + (vp->var$magic-1)*4
    };



var_UDP (vp, name, length, exact, var_len, access_method)
    {
    EXTERNAL
	UDP_MIB;
    MAP
	struct variable_struct * vp,
        struct VECTOR * name[,SNMP$K_OIDsize];

    IF (exact && (compare(name,..length,vp->var$name,vp->var$namelen) != 0))
	THEN RETURN 0;
    ch$move(vp->var$namelen * SNMP$K_OIDsize, vp->var$name, name );

    length = vp->var$namelen;
    access_method = 0;
    var_len = 4;	// default length == 4 bytes

    RETURN UDP_MIB + (vp->var$magic-1)*4
    };



var_TCP (vp, name, length, exact, var_len, access_method)
    {
    EXTERNAL
	TCP_MIB;
    MAP
	struct variable_struct * vp,
        struct VECTOR * name[,SNMP$K_OIDsize];

    IF (exact && (compare(name,..length,vp->var$name,vp->var$namelen) != 0))
	THEN RETURN 0;
    ch$move(vp->var$namelen * SNMP$K_OIDsize, vp->var$name, name );

    length = vp->var$namelen;
    access_method = 0;
    var_len = 4;	// default length == 4 bytes

    RETURN TCP_MIB + (vp->var$magic-1)*4
    };



var_tcpConn (vp, name, length, exact, var_len, access_method)
    {
    EXTERNAL 
	struct VECTOR * VTCB_PTR->0,
	VTCB_size,
	TCB_Count;
    MAP
	struct variable_struct * vp,
        struct VECTOR * name[,SNMP$K_OIDsize];
    signed long
	count,
	newname : VECTOR[MAX_NAME_LEN,SNMP$K_OIDsize],
	result,
	TCPaddr,
	struct tcb_structure * tcb,
	beststruct tcb_structure * tcb;

    ch$move(vp->var$namelen * SNMP$K_OIDsize, vp->var$name, newname );

    //  Now find "next" TCP connection.
    count = TCB_count;
    bestTCB = 0;

    INCR j FROM 1 TO VTCB_Size DO
	if ((TCB = VTCB_ptr[J]) NEQA 0)
	    {
	    // "Cobble" in the new name
	    TCPaddr = TCB->Local_Host;		// Local Host
	    newname->10 = TCPaddr<0,8,0>;  newname->11 = TCPaddr<8,8,0>;
	    newname->12 = TCPaddr<16,8,0>; newname->13 = TCPaddr<24,8,0>;
	    newname->14 = TCB->Local_Port;	// Local Port

	    TCPaddr = TCB->Foreign_Host;	// Remote Host
	    newname->15 = TCPaddr<0,8,0>;  newname->16 = TCPaddr<8,8,0>;
	    newname->17 = TCPaddr<16,8,0>; newname->18 = TCPaddr<24,8,0>;
	    newname->19 = TCB->Local_Port;	// Remote Port

	    result = compare(name, ..length, newname, vp->var$namelen);
	    if ((NOT exact && (result LSS 0)))
		{
	        result = cmp_TCB(TCB, bestTCB);
		if (result LSS 0) bestTCB = TCB
		};
	    if ((exact && (result == 0))) EXITLOOP bestTCB = TCB;

	    if ((count = count-1) <= 0) !only process what we have.
		EXITLOOP;
	    };

    if ((bestTcb EQLA 0)) RETURN 0;

    // "Cobble" in the new name
    TCPaddr = bestTCB->Local_Host;		// Local Host
    newname->10 = TCPaddr<0,8,0>;  newname->11 = TCPaddr<8,8,0>;
    newname->12 = TCPaddr<16,8,0>; newname->13 = TCPaddr<24,8,0>;
    newname->14 = bestTCB->Local_Port;	// Local Port

    TCPaddr = bestTCB->Foreign_Host;	// Remote Host
    newname->15 = TCPaddr<0,8,0>;  newname->16 = TCPaddr<8,8,0>;
    newname->17 = TCPaddr<16,8,0>; newname->18 = TCPaddr<24,8,0>;
    newname->19 = bestTCB->Local_Port;	// Remote Port

    ch$move(vp->var$namelen * SNMP$K_OIDsize, newname, name );

    length = vp->var$namelen;
    access_method = 0;
    var_len = 4;	// default length == 4 bytes

    SELECTONE (vp->var$magic) OF
	SET
    	[tcpConnState]:
	    {
	    long_return = bestTCB->State+1;
	    RETURN long_return;
	    };
    	[tcpConnLocalAddress]:
	    RETURN bestTCB->local_host;
    	[tcpConnLocalPort]:
	    {
	    long_return = bestTCB->local_port;
	    RETURN long_return;
	    };
    	[tcpConnRemAddress]:
	    RETURN bestTCB->foreign_host;
    	[tcpConnRemPort]:
	    {
	    long_return = bestTCB->foreign_port;
	    RETURN long_return;
	    };
	TES;

    RETURN 0
    };

}
ELUDOM
