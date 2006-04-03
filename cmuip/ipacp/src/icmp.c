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
//SBTTL "InterNetwork Control Message Protocol Handler Overview."
/*

Module:

	ICMP

Facility:

	Inter-Network control message protocol handler.

Abstract:

	ICMP is defined in RFC 792

Author:
	Bruce R. Miller		CMU Network Development
	November 17, friday, 1989
	Copyright (c) 1989 Carnegie-Mellon University

Modifications:

1.0b	29-Aug-1991	Henry W. Miller		USBR
	In ICMP$INPUT(), protect against bad IP header in data portion of
	ICMP packet - scrambled data was causing Access Violations.  If
	supposed data portion of IP header in ICMP packet is greater than
	size of the buffer, it's a good clue we got a bad packet.  Bounce
	it.  Yell at operator.

1.0a	24-Apr-1991	Henry W. Miller		USBR
	Make ICMPTTL a configurable variable.
	Add hooks for generating ICMP DUNR replies.  NYI.

 1.0	17-Nov-89	Bruce R. Miller		CMU NetDev
	Initial version.  I copied all of the other ICMP stuff
	into this module.

*/


//SBTTL "Module Definition"

#if 0
MODULE ICMP(IDENT="1.0B",LANGUAGE(BLISS32),
	  ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			  NONEXTERNAL=LONG_RELATIVE),
	  LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	  OPTIMIZE,OPTLEVEL=3,ZIP)=
#endif

#include <starlet.h>	// VMS system definitions
#include <cmuip/central/include/netcommon.h>	// Common decls
// not yet #include "CMUIP_SRC:[CENTRAL]NETXPORT";		// BLISS transportablity package
#include "netvms.h">		// VMS specific
#include <cmuip/central/include/nettcpip.h>		// IP & ICMP definitions
#include "structure.h"		// TCB & Segment Structure definition
#include "tcpmacros.h"		// Local macros
#include "snmp.h"

#include <descrip.h>

#undef TCP_DATA_OFFSET
#include <net/checksum.h>
#define Calc_Checksum(x,y) ip_compute_csum(y,x)

// Memgr.bli
extern     mm$seg_get();
extern  void    mm$seg_free();

// IOUTIL.BLI
extern  void    ASCII_DEC_BYTES();

// MacLib.mar
extern     CALC_CHECKSUM();		// MacLib
extern  void    swapbytes();	// MacLib

// IP.bli
extern  void    ip$send();
extern  void    ip$log();

// TCP_Segin
extern  void    SEG$ICMP();

// UDP_Segin
extern  void    udp$icmp();

extern signed long
log_state,
    icmpttl,
    min_physical_bufsize,	// Size of "small" device buffers
    max_physical_bufsize;	// Size of "large" device buffers



//SBTTL  "ICMP database definitions"

// The ICMP routing database consists of a hash/index table containing IP
// addresses and pointers to ICMP data for each address. When an ICMP redirect
// is received, we add an entry to the database for that host, recording the
// gateway we were told to use. ICMP_CHECK will return this gateway when it is
// called by IP_ROUTE to find a route to a host not on the local network.
// At present, the ICMP information is never flushed, and therefore no locking
// is needed to maintain database integrity.

//Format of a host ICMP status block.

struct ICM_DBLOCK
{
void *     icm$next	;
unsigned int     icm$address	;
unsigned int    icm$gwy
};

#define    ICM_DSIZE sizeof (struct ICM_DBLOCK)
#if 0
MACRO
    ICM_DBLOCK = BLOCK->ICM_Dsize FIELD(ICM_Dblock_Fields) %;
#endif

#define    ICM_HSHLEN 128		// Length of hash table
#define    ICM_HSHAND ICM_HSHLEN-1	// && value for forming hash values

static signed long
    ICMHTB [ICM_HSHLEN]; // ICMP database hash table

    struct ICMP_MIB_struct icmp_mib_, * icmp_mib = &icmp_mib_ ;	// ICMP Management Information Block


//SBTTL "ICMP routing code"

// This code maintains the ICMP database, containing alternate routes to hosts
// for which we have received redirects.

ICMP_Hash(IPA)

//Hash an IP address. Returns hash value (index into ICMHTB)

     long IPA;
    {
      char * IPAP=&IPA;
    return ((IPAP[0]+IPAP[1]+IPAP[2]+IPAP[3]) & (ICM_HSHAND));
    }

ICMP_Find(IPADDR)

//Search ICMP hash chains for entry for this address.
//Returns:
//   0 - No entry exists for this IP address.
//   nonzero - pointer to ICMP data block for this address.
//N.B. If ICMP entries are ever timed out, this routine should only be called
//with AST's disabled.

    {
	struct ICM_DBLOCK * ICPTR;
    ICPTR = ICMHTB[ICMP_Hash(IPADDR)];
    while (ICPTR != 0)
	if (ICPTR->icm$address == IPADDR)
	  return ICPTR;
	else
	    ICPTR = ICPTR->icm$next;
    return 0;
    }

void ICMP_Add(long GWY_Addr,struct ip_structure * IP_Pkt)

//Here to add an entry to the ICMP routing table.
//   GWY_Addr has the IP address of a gateway
//   IP_Pkt has the header and first 64-bits of the IP packet which caused
//   the ICMP redirect to be generated.

    {
extern	LIB$GET_VM();
    signed long
    IPdest;
	struct ICM_DBLOCK * ICMblock;
    IPdest = IP_Pkt->iph$dest;
    ICMblock = ICMP_Find(IPdest);
    if (ICMblock == 0)
	{
	signed long
	    HSHVAL;
	LIB$GET_VM(/*%REF*/(ICM_DSIZE),&ICMblock);
	HSHVAL = ICMP_Hash(IPdest);
	ICMblock->icm$next = ICMHTB[HSHVAL];
	ICMHTB[HSHVAL] = ICMblock;
	};
    ICMblock->icm$address = IPdest;
    ICMblock->icm$gwy = GWY_Addr;
    }

icmp$check(IPaddr)

//Check if the ICMP routing table has an entry for this IP address.
//Returns:
//   0	No ICMP routing info
//  !=0	IP address of gateway from ICMP info.

    {
	struct ICM_DBLOCK * ICMptr;
    if ((ICMptr = ICMP_Find(IPaddr)) == 0)
      return 0;
    else
	return ICMptr->icm$gwy;
    }


//SBTTL  "Handle ICMP packet input"
/*
    Decode input ICMP packet and dispatch according to packet type.
    Also, pass packet up to protocol-specific ICMP handler if we think
    that a higher-level protocol wants to know about it.
 */

void    ICMP_Pproblem();
void    ICMP_Tstamp();
void    ICMP_Info();
 void    ICMP_Echo();

void icmp$input (ICMptr,ICMlen,IPptr,IPlen,bufsize,buf)

// Main ICMP input routine.
// Verify ICMP checksum and dispatch according to function code.

     struct ip_structure * IPptr;
	struct icmp_header * ICMptr;
    {
extern	icmp$user_input();
    signed long
	ICMP_swapped  = 0,
	ICMdat,ICMtype,
	Sum,
	DataSize ;

    // Keep count of incoming packets
    icmp_mib->MIB$icmpInMsgs = icmp_mib->MIB$icmpInMsgs + 1;

    Sum = Calc_Checksum(ICMlen,ICMptr);
    if (Sum != 0 /* was: 0xFFFF */)
	{
	icmp_mib->MIB$icmpInErrors = icmp_mib->MIB$icmpInErrors + 1;
	if ($$LOGF(LOG$ICMP))
	    QL$FAO("!%T ICMP recv checksum error, cksum=!XL!/",0,Sum);
	mm$seg_free(bufsize,buf);
	return;
	};

    // Fix-up the ICMP word ordering
    swapbytesicmphdr(ICMP_HEADER_SIZE/2,ICMptr);
    ICMdat = 0;
    ICMtype = ICMptr->icm$type;

    if ($$LOGF(LOG$ICMP))
	QL$FAO("!%T ICMP recv: type=!SL,code=!SL,var=!XL!/",
		0,ICMtype,ICMptr->icm$code,ICMptr->icm$var);

    // Now check to see if any users are looking at ICMP packets...
    icmp$user_input(IPptr->iph$source,IPptr->iph$dest,
		    bufsize,buf,
		    ICMlen,ICMptr);

    // Do IP/ICMP level processing for ICMP packet

    switch ( ICMtype) 
      {

	// First are "special" ICMP message - not in reply to IP packet
	case ICM_ECHO:
	    {
	    icmp_mib->MIB$icmpInEchos =
		icmp_mib->MIB$icmpInEchos + 1;

	    ICMP_Echo(ICMptr,ICMlen,IPptr,IPlen);
	    };
	    break;

	case ICM_TSTAMP:
	    {
	    icmp_mib->MIB$icmpInTimeStamps =
		icmp_mib->MIB$icmpInTimeStamps + 1;

	    ICMP_Tstamp(ICMptr,ICMlen,IPptr,IPlen);
	    };
	    break;

	case ICM_IREQUEST:
	    {
	    ICMP_Info(ICMptr,ICMlen,IPptr,IPlen);
	    };
	    break;

	case ICM_AMREQUEST:
	    {
	    icmp_mib->MIB$icmpInAddrMasks =
		icmp_mib->MIB$icmpInAddrMasks + 1;

	    ICMP_Tstamp(ICMptr,ICMlen,IPptr,IPlen);
	    };
	    break;

	// Others are "standard" ICMP messages,
	// and contain the header of the offending packet
	case ICM_DUNREACH: case ICM_SQUENCH: case ICM_REDIRECT: case ICM_TEXCEED: case ICM_PPROBLEM:
	    {
	      struct ip_structure * IPhdr;
	    signed long
		ptype,IPdat,IPhlen,IPlen,passup;
	    passup = TRUE;
	    IPhdr = ICMptr->icm$data;
	    IPhlen = IPhdr->iph$ihl*4; // was: swap_ihl

// Fix the byteswapped packet

	    swapbytesiphdr(IP_HDR_SWAP_SIZE,IPhdr);
	    if ($$LOGF(LOG$ICMP)) {
	      $DESCRIPTOR(dsc,"ICMrcv/IP");
		ip$log(&dsc,IPhdr);
	    }

// Calculate lengths, pointer to protocol data

	    ptype = IPhdr->iph$protocol;
	    IPdat = IPhdr+IPhlen;
	    IPlen = IPhdr->iph$total_length;
	    switch ( ICMtype)
	      {
	    case ICM_DUNREACH:
		{
		icmp_mib->MIB$icmpInDestUnreachs =
			icmp_mib->MIB$icmpInDestUnreachs + 1;

		if ($$LOGF(LOG$ICMP))
			QL$FAO("!%T ICMP recv: Destination Unreachable!/",0);
		};
		  break;

	    case ICM_SQUENCH:
		{
		icmp_mib->MIB$icmpInSrcQuenchs =
			icmp_mib->MIB$icmpInSrcQuenchs + 1;

		if ($$LOGF(LOG$ICMP))
			QL$FAO("!%T ICMP recv: Source Quench!/",0);
		};
		break;

	    case ICM_REDIRECT:
		{
		icmp_mib->MIB$icmpInRedirects =
			icmp_mib->MIB$icmpInRedirects + 1;

		swapbytesicmphdr(2,ICMptr->icm$r_gwy);
		ICMP_Add(ICMptr->icm$r_gwy,ICMptr->icm$data);
		};
		break;

	    case ICM_TEXCEED:
		{
		icmp_mib->MIB$icmpInTimeExcds =
			icmp_mib->MIB$icmpInTimeExcds + 1;
		};
		break;

	    case ICM_PPROBLEM:
		{
		icmp_mib->MIB$icmpInParamProbs =
			icmp_mib->MIB$icmpInParamProbs + 1;

		if (ICMptr->icm$code != 0)
		  ICMdat = ICMptr->icm$p_ptr;
		else
		    ICMdat = 0;
		if (ICMdat < IPdat) // IP-level problem
		    {
		    ICMP_Pproblem(ICMptr,ICMlen,IPptr,IPlen,ICMdat);
		    passup = FALSE;
		    };
		};
	    };

// Do higher-level processing for ICMP packet

	    DataSize = IPlen - IPhlen ;
	    if (passup)
	      switch ( ptype)
		{
		case TCP_PROTOCOL:
		    {
		    if ((DataSize > bufsize))
			{
			OPR$FAO("Bad ICMP packet for TCP, discarding") ;
			}
		    else
			{
			SEG$ICMP(ICMtype,ICMdat,IPhdr->iph$source,
			     IPhdr->iph$dest,IPdat,DataSize,
			     buf,bufsize);
			} ;
		    return;
		    };
		    break;

		case UDP_PROTOCOL:
		    {
		    if ((DataSize > bufsize))
			{
			OPR$FAO("Bad ICMP packet for UDP, discarding") ;
			}
		    else
			{
			udp$icmp(ICMtype,ICMdat,IPhdr->iph$source,
			     IPhdr->iph$dest,IPdat,DataSize,
			     buf,bufsize);
			} ;
		    return;
		    };
		    break;

		default:
		    if ($$LOGF(LOG$ICMP))
			QL$FAO("!%T ICMP - Unknown protocol !XL!/",0,ptype);
		};
	    };

	case ICM_TSREPLY :
	    {
	    icmp_mib->MIB$icmpInTimeStampreps =
		icmp_mib->MIB$icmpInTimeStampreps + 1;
	    QL$FAO("!%T ICMP recv: Time Stamp reply. how did this happen?!/",0);
	    };
	    break;

	case ICM_IREPLY :
	    {
	      QL$FAO("!%T ICMP recv: Info reply. how did this happen?!/",0);
	    };
	    break;

	case ICM_AMREPLY :
	    {
	    icmp_mib->MIB$icmpInAddrMaskReps =
		icmp_mib->MIB$icmpInAddrMaskReps + 1;
	    QL$FAO("!%T ICMP recv: AddrMask reply. how did this happen?!/",0);
	    };
	    break;

	case ICM_EREPLY:
	    {
	    icmp_mib->MIB$icmpInEchoReps =
		icmp_mib->MIB$icmpInEchoReps + 1;
	    QL$FAO("!%T ICMP recv: Echo reply!/",0);
	    };
	    break;

	default:
	    {
	    icmp_mib->MIB$icmpInErrors = icmp_mib->MIB$icmpInErrors + 1;
	    if ($$LOGF(LOG$ICMP))
		QL$FAO("!%T ICMP recv: unknown ICMP type !XL!/",0,ICMtype);
	    };
    };

    // Release the buffer
    mm$seg_free(bufsize,buf);
    }

void ICMP_Pproblem(ICMpkt,ICMlen,IPpkt,IPlen,ICMpptr)

// Handle ICMP Parameter Problem packet.
// Called if problem is at the IP level. This is unexpected and is probably
// a bug at the remote end, since we don't send any IP options.

    {
    XQL$FAO(LOG$ICMP,"!%T ICMP recv: IP param=!XL!/",ICMpptr);
    }

void ICMP_Echo(ICMpkt,ICMlen,IPPKT,IPlen)

// Handle ICMP echo request.
// Copy packet, convert to echo reply, queue for output.
//~~~ This routine should be simpler - should just reverse the packet and
//~~~ queue for output, instead of creating new packet, etc.

     struct icmp_header * ICMpkt;
	struct ip_structure * IPPKT;
    {
    signed long
	Buf,
	Bufsize,
      Segsize;
    struct icmp_header * Seg;
    DESC$STR_ALLOC(srcstr,20);
	DESC$STR_ALLOC(dststr,20);

// Check the TTL to see if this packet has expired

    IPPKT->iph$ttl = IPPKT->iph$ttl - 1;
    if (IPPKT->iph$ttl <= 0)
	{
	if ($$LOGF(LOG$ICMP))
	    {
	    ASCII_DEC_BYTES(srcstr,4,IPPKT->iph$source,
			    &srcstr->dsc$w_length);
	    ASCII_DEC_BYTES(dststr,4,IPPKT->iph$dest,
			    &dststr->dsc$w_length);
	    icmp_mib->MIB$icmpOutErrors =
		icmp_mib->MIB$icmpOutErrors + 1;
	    QL$FAO("!%T ICMP_ECHO: TTL exceeded, SRC=!AS,DST=!AS,ID=!UL!/",
		   0,srcstr,dststr,IPPKT->iph$ident);
	    };
	return;
	};

// Calculate size of physical buffer to use

    Bufsize = DEVICE_HEADER + IPlen;
    if (Bufsize <= min_physical_bufsize)
      Bufsize = min_physical_bufsize;
    else
	if (Bufsize <= max_physical_bufsize)
	  Bufsize = max_physical_bufsize;
	else
	    {
	    if ($$LOGF(LOG$ICMP))
		{
		ASCII_DEC_BYTES(srcstr,4,IPPKT->iph$source,
			        &srcstr->dsc$w_length);
		ASCII_DEC_BYTES(dststr,4,IPPKT->iph$dest,
				&dststr->dsc$w_length);

		icmp_mib->MIB$icmpOutErrors =
			icmp_mib->MIB$icmpOutErrors + 1;
		QL$FAO("!%T ICMP_ECHO: PKT too large,SRC=!AS,DST=!AS,ID=!UL!/",
		   0,srcstr,dststr,IPPKT->iph$ident);
		};
	    return;
	    };

// Allocate the segment.

    Buf = mm$seg_get(Bufsize);
    Seg = Buf + DEVICE_HEADER + IP_HDR_BYTE_SIZE;
    Segsize = ICMlen;

// Copy the packet

    CH$MOVE(ICMlen, ICMpkt, Seg);

// Turn it into an echo reply

    Seg->icm$type = ICM_EREPLY;

// Calculate ICMP checksum

    Seg->icm$cksum = 0;

// Do logging, if necessary

    if ($$LOGF(LOG$ICMP))
	QL$FAO("!%T ICMP echo rply: id=!SL, seq=!SL!/",
	       0, Seg->icm$e_id, Seg->icm$e_seq);

// Swap the header bytes and compute the checksum

    swapbytesicmphdr(ICMP_HEADER_SIZE/2,Seg);
    Seg->icm$cksum = Calc_Checksum(Segsize, Seg);

// Send packet, preserving ID, TOS, TTL, etc.
//!!HACK!!// IPPKT->iph$dest is wrong//  what about broadcasts?

    ip$send(IPPKT->iph$dest, IPPKT->iph$source, IPPKT->iph$type_service,
	    IPPKT->iph$ttl, Seg, Segsize, IPPKT->iph$ident,
	    FALSE, TRUE, ICMP_Protocol, Buf, Bufsize);

    // Keep count of outgoing packets
    icmp_mib->MIB$icmpOutMsgs = icmp_mib->MIB$icmpOutMsgs + 1;

    icmp_mib->MIB$icmpOutEchoReps =
	icmp_mib->MIB$icmpOutEchoReps + 1;
    }

void ICMP_Tstamp(ICMpkt,ICMlen,IPpkt,IPlen)

// Handle ICMP Timestamp request.
// Copy packet, convert to Timestamp Reply, queue for output.
// Not yet implemented.

    {
    return;
    }

void ICMP_Info(ICMpkt,ICMlen,IPpkt,IPlen)

// Handle ICMP Information request
// Copy packet, convert to Information Reply, queue for output.
// Not yet implemented.

    {
    return;
    }

void ICMP_Send_DUNR(ICMpkt,ICMlen,IPPKT,IPlen,code)

// Send ICMP Destination Unreachable
// Copy packet, queue for output.

     struct icmp_header * ICMpkt;
	struct ip_structure * IPPKT;
    {
    signed long
	Buf,
	Bufsize,
      Segsize;
    struct icmp_header * Seg;
    DESC$STR_ALLOC(srcstr,20);
	DESC$STR_ALLOC(dststr,20);

// Calculate size of physical buffer to use

    Bufsize = DEVICE_HEADER + IPlen;
    if (Bufsize <= min_physical_bufsize)
      Bufsize = min_physical_bufsize;
    else
	if (Bufsize <= max_physical_bufsize)
	  Bufsize = max_physical_bufsize;
	else
	    {
	    if ($$LOGF(LOG$ICMP))
		{
		ASCII_DEC_BYTES(srcstr,4,IPPKT->iph$source,
			        &srcstr->dsc$w_length);
		ASCII_DEC_BYTES(dststr,4,IPPKT->iph$dest,
				&dststr->dsc$w_length);

		icmp_mib->MIB$icmpOutErrors =
			icmp_mib->MIB$icmpOutErrors + 1;
		QL$FAO("!%T ICMP_ECHO: PKT too large,SRC=!AS,DST=!AS,ID=!UL!/",
		   0,srcstr,dststr,IPPKT->iph$ident);
		};
	    return;
	    };

// Allocate the segment.

    Buf = mm$seg_get(Bufsize);
    Seg = Buf + DEVICE_HEADER + IP_HDR_BYTE_SIZE;
    Segsize = ICMlen;

// Copy the packet

    CH$MOVE(ICMlen, ICMpkt, Seg);

// Set Type and Code

    Seg->icm$type = ICM_DUNREACH ;
    Seg->icm$code = code ;

// Calculate ICMP checksum

    Seg->icm$cksum = 0;

// Do logging, if necessary

    if ($$LOGF(LOG$ICMP))
	QL$FAO("!%T ICMP send DUNR: id=!SL, seq=!SL, code=!SL!/",
	       0, Seg->icm$e_id, Seg->icm$e_seq, Seg->icm$code);

// Swap the header bytes and compute the checksum

    swapbytesicmphdr(ICMP_HEADER_SIZE/2,Seg);
    Seg->icm$cksum = Calc_Checksum(Segsize, Seg);

// Send packet, preserving ID, TOS, TTL, etc.
//!!HACK!!// IPPKT->iph$dest is wrong//  what about broadcasts?

    ip$send(IPPKT->iph$dest, IPPKT->iph$source, IPPKT->iph$type_service,
	    icmpttl, Seg, Segsize, IPPKT->iph$ident,
	    FALSE, TRUE, ICMP_Protocol, Buf, Bufsize);

    // Keep count of outgoing packets
    icmp_mib->MIB$icmpOutMsgs = icmp_mib->MIB$icmpOutMsgs + 1;

    icmp_mib->MIB$icmpOutDestUnreachs =
	icmp_mib->MIB$icmpOutDestUnreachs + 1;
    }
