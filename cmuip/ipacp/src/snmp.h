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
/*
 * Definitions for the Simple Network Management Protocol (RFC 1067).
 *
 *
 */
/***********************************************************
	Copyright 1988, 1989 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/

#define SNMP_PORT	    161
#define SNMP_TRAP_PORT	    162

#define SNMP_MAX_LEN	    484

#define SNMP_VERSION_1	    0

#define GET_REQ_MSG	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x0)
#define GETNEXT_REQ_MSG	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x1)
#define GET_RSP_MSG	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x2)
#define SET_REQ_MSG	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x3)
#define TRP_REQ_MSG	    (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x4)

#define SNMP_ERR_NOERROR    (0x0)
#define SNMP_ERR_TOOBIG	    (0x1)
#define SNMP_ERR_NOSUCHNAME (0x2)
#define SNMP_ERR_BADVALUE   (0x3)
#define SNMP_ERR_READONLY   (0x4)
#define SNMP_ERR_GENERR	    (0x5)

#define SNMP_TRAP_COLDSTART		(0x0)
#define SNMP_TRAP_WARMSTART		(0x1)
#define SNMP_TRAP_LINKDOWN		(0x2)
#define SNMP_TRAP_LINKUP		(0x3)
#define SNMP_TRAP_AUTHFAIL		(0x4)
#define SNMP_TRAP_EGPNEIGHBORLOSS	(0x5)
#define SNMP_TRAP_ENTERPRISESPECIFIC	(0x6)

#define PARSE_ERROR	 -1 
#define BUILD_ERROR	 -2 

#define SID_MAX_LEN		 64 
#define MAX_NAME_LEN	 64   // number of subid's in a objid

#define READ	 1 
#define WRITE	 0 

#define RONLY	 %x'0AAAA' 	// read access for everyone
#define RWRITE	 %x'0AABA' 	// add write access for community private
#define NOACCESS 	 %x'0000' 	// no access for anybody

#define INTEGER	 ASN_INTEGER 
#define STRING	 ASN_OCTET_STR 
#define OBJID	 ASN_OBJECT_ID 
#define NULLOBJ	 ASN_NULL 

// defined types (from the SMI, RFC 1065)
#define IPADDRESS	 (ASN_APPLICATION OR 0) 
#define COUNTER	 (ASN_APPLICATION OR 1) 
#define GAUGE	 (ASN_APPLICATION OR 2) 
#define TIMETICKS	 (ASN_APPLICATION OR 3) 
#define    OPAQUE	 (ASN_APPLICATION OR 4)



// IP group MIB (Management Information Block)

// Note:  The IP group magic in SNMP_VARS assumes that every field is a long.
    struct IP_group_MIB_struct {
signed long IPMIB$ipForwarding		 ;
signed long IPMIB$ipDefaultTTL		 ;
signed long IPMIB$ipInReceives		 ;
signed long IPMIB$ipInHdrErrors		 ;
signed long IPMIB$ipInAddrErrors	 ;
signed long IPMIB$ipForwDatagrams	 ;
signed long IPMIB$ipInUnknownProtos	 ;
signed long IPMIB$ipInDiscards		 ;
signed long IPMIB$ipInDelivers		 ;
signed long IPMIB$ipOutRequests		 ;
signed long IPMIB$ipOutDiscards		 ;
signed long IPMIB$ipOutNoRoutes		 ;
signed long IPMIB$ipReasmTimeout	 ;
signed long IPMIB$ipReasmReqds		 ;
signed long IPMIB$ipReasmOKs		 ;
signed long IPMIB$ipReasmFails		 ;
signed long IPMIB$ipFragOKs		 ;
signed long IPMIB$ipFragFails		 ;
signed long IPMIB$ipFragCreates		 ;
    };

#define    IP_group_MIB_Size	sizeof(struct IP_group_MIB_struct)



// ICMP group MIB (Management Information Block)

    struct ICMP_MIB_struct {
signed long MIB$icmpInMsgs		 ;
signed long MIB$icmpInErrors		 ;
signed long MIB$icmpInDestUnreachs	 ;
signed long MIB$icmpInTimeExcds		 ;
signed long MIB$icmpInParamProbs	 ;
signed long MIB$icmpInSrcQuenchs	 ;
signed long MIB$icmpInRedirects		 ;
signed long MIB$icmpInEchos		 ;
signed long MIB$icmpInEchoReps		 ;
signed long MIB$icmpInTimeStamps	 ;
signed long MIB$icmpInTimeStampreps	 ;
signed long MIB$icmpInAddrMasks		 ;
signed long MIB$icmpInAddrMaskReps	 ;
signed long MIB$icmpOutMsgs		 ;
signed long MIB$icmpOutErrors		 ;
signed long MIB$icmpOutDestUnreachs	 ;
signed long MIB$icmpOutTimeExcds	 ;
signed long MIB$icmpOutParamProbs	 ;
signed long MIB$icmpOutSrcQuenchs	 ;
signed long MIB$icmpOutRedirects	 ;
signed long MIB$icmpOutEchos		 ;
signed long MIB$icmpOutEchoReps		 ;
signed long MIB$icmpOutTimeStamps	 ;
signed long MIB$icmpOutTimeStampReps	 ;
signed long MIB$icmpOutAddrMasks	 ;
signed long MIB$icmpOutAddrMaskReps	;
    }

#define    ICMP_MIB_Size	sizeof(ICMP_MIB_struct)



// UDP group MIB (Management Information Block)

struct UDP_MIB_struct {
signed long MIB$UDPINDATAGRAMS	 ;
signed long MIB$UDPNOPORTS	 ;
signed long MIB$UDPINERRORS	 ;
signed long MIB$UDPOUTDATAGRAMS	;
}

#define    UDP_MIB_Size	sizeof(struct UDP_MIB_struct)



// TCP group MIB (Management Information Block)

struct TCP_MIB_struct {
signed long MIB$tcpRtoAlgorithm	 ;
signed long MIB$tcpRtoMin	 ;
signed long MIB$tcpRtoMax	 ;
signed long MIB$tcpMaxConn	 ;
signed long MIB$tcpActiveOpens	 ;
signed long MIB$tcpPassiveOpens	 ;
signed long MIB$tcpAttemptFails	 ;
signed long MIB$tcpEstabResets	 ;
signed long MIB$tcpCurrEstab	 ;
signed long MIB$tcpInSegs	 ;
signed long MIB$tcpOutSegs	 ;
signed long MIB$tcpRetransSegs	 ;
}

#define    TCP_MIB_Size	sizeof(TCP_MIB_struct)



// Magic Numbers

    // sysVar magic
#define VERSION_DESCR	 0 
#define VERSION_ID		 1 
#define UPTIME		 2 
#define CFG_NNETS		 3 

    // ifTable magic
#define IFINDEX		 1 
#define IFDESCR		 2 
#define IFTYPE		 3 
#define IFMTU		 4 
#define IFSPEED		 5 
#define IFPHYSADDRESS	 6 
#define IFADMINSTATUS	 7 
#define IFOPERSTATUS	 8 
#define IFLASTSTATE		 9 
#define IFINOCTETS		 10 
#define IFINUCASTPKTS	 11 
#define IFINNUCASTPKTS	 12 
#define IFINDISCARDS	 13 
#define IFINERRORS		 14 
#define IFINUNKNOWNPROTOS	 15 
#define IFOUTOCTETS		 16 
#define IFOUTUCASTPKTS	 17 
#define IFOUTNUCASTPKTS	 18 
#define IFOUTDISCARDS	 19 
#define IFOUTERRORS 	 20 
#define IFOUTQLEN		 21 

    // IP group magic
#define IPFORWARDING	 1 
#define IPDEFAULTTTL 	 2 
#define IPINRECEIVES 	 3 
#define IPINHDRERRORS 	 4 
#define IPINADDRERRORS 	 5 
#define IPFORWDATAGRAMS 	 6 
#define IPINUNKNOWNPROTOS 	 7 
#define IPINDISCARDS 	 8 
#define IPINDELIVERS 	 9 
#define IPOUTREQUESTS 	 10 
#define IPOUTDISCARDS 	 11 
#define IPOUTNOROUTES 	 12 
#define IPREASMTIMEOUT 	 13 
#define IPREASMREQDS 	 14 
#define IPREASMOKS 		 15 
#define IPREASMFAILS 	 16 
#define IPFRAGOKS 		 17 
#define IPFRAGFAILS 	 18 
#define IPFRAGCREATES 	 19 

    // IP Address Table magic
#define IPADADDR		 1 
#define IPADIFINDEX		 2 
#define IPADNETMASK		 3 
#define IPADBCASTADDR	 4 

    // ICMP group magic
#define ICMPINMSGS		 1 
#define ICMPINERRORS	 2 
#define ICMPINDESTUNREACHS	 3 
#define ICMPINTIMEEXCDS	 4 
#define ICMPINPARMPROBS	 5 
#define ICMPINSRCQUENCHS	 6 
#define ICMPINREDIRECTS	 7 
#define ICMPINECHOS		 8 
#define ICMPINECHOREPS	 9 
#define ICMPINTIMESTAMPS	 10 
#define ICMPINTIMESTAMPREPS	 11 
#define ICMPINADDRMASKS	 12 
#define ICMPINADDRMASKREPS	 13 
#define ICMPOUTMSGS		 14 
#define ICMPOUTERRORS	 15 
#define ICMPOUTDESTUNREACHS	 16 
#define ICMPOUTTIMEEXCDS	 17 
#define ICMPOUTPARMPROBS	 18 
#define ICMPOUTSRCQUENCHS	 19 
#define ICMPOUTREDIRECTS	 20 
#define ICMPOUTECHOS	 21 
#define ICMPOUTECHOREPS	 22 
#define ICMPOUTTIMESTAMPS	 23 
#define ICMPOUTTIMESTAMPREPS 24 
#define ICMPOUTADDRMASKS	 25 
#define ICMPOUTADDRMASKREPS	 26 

    // TCP group magic
#define tcpRtoAlgorithm	 1 
#define tcpRtoMin		 2 
#define tcpRtoMax		 3 
#define tcpMaxConn		 4 
#define tcpActiveOpens	 5 
#define tcpPassiveOpens	 6 
#define tcpAttemptFails	 7 
#define tcpEstabResets	 8 
#define tcpCurrEstab	 9 
#define tcpInSegs		 10 
#define tcpOutSegs		 11 
#define tcpRetransSegs	 12 

    // TCP Connection Table magic
#define tcpConnState	 1 
#define tcpConnLocalAddress	 2 
#define tcpConnLocalPort	 3 
#define tcpConnRemAddress	 4 
#define tcpConnRemPort	 5 

    // UDP group magic
#define UDPINDATAGRAMS	 1 
#define UDPINNOPORTS	 2 
#define UDPINERRORS		 3 
#define UDPOUTDATAGRAMS	4
