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

MODULE ICMP(IDENT="1.0B",LANGUAGE(BLISS32),
	  ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			  NONEXTERNAL=LONG_RELATIVE),
	  LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	  OPTIMIZE,OPTLEVEL=3,ZIP)=
{

#include "SYS$LIBRARY:STARLET";	// VMS system definitions
#include "CMUIP_SRC:[CENTRAL]NETCOMMON";	// Common decls
#include "CMUIP_SRC:[CENTRAL]NETXPORT";		// BLISS transportablity package
#include "CMUIP_SRC:[CENTRAL]NETVMS";		// VMS specific
#include "CMUIP_SRC:[CENTRAL]NetTCPIP";		// IP & ICMP definitions
#include "STRUCTURE";		// TCB & Segment Structure definition
#include "TCPMACROS";		// Local macros
#include "SNMP";

extern
// Memgr.bli
    MM$Seg_Get,
 void    MM$Seg_Free,

// IOUTIL.BLI
 VOID    ASCII_DEC_BYTES,

// MacLib.mar
    CALC_CHECKSUM,		// MacLib
 void    SwapBytes,	// MacLib

// IP.bli
 VOID    IP$S},
 void    IP$Log,

// TCP_Segin
 VOID    SEG$ICMP,

// UDP_Segin
 VOID    UDP$ICMP;

extern signed long
    ICMPTTL,
    Min_Physical_Bufsize,	// Size of "small" device buffers
    Max_Physical_Bufsize;	// Size of "large" device buffers



//SBTTL  "ICMP database definitions"

// The ICMP routing database consists of a hash/index table containing IP
// addresses and pointers to ICMP data for each address. When an ICMP redirect
// is received, we add an entry to the database for that host, recording the
// gateway we were told to use. ICMP_CHECK will return this gateway when it is
// called by IP_ROUTE to find a route to a host not on the local network.
// At present, the ICMP information is never flushed, and therefore no locking
// is needed to maintain database integrity.

!Format of a host ICMP status block.

$FIELD ICM_DBLOCK_FIELDS (void)
    SET
    ICM$Next	= [$Address],
    ICM$Address	= [$Bytes(4)],
    ICM$Gwy	= [$Bytes(4)]
    TES;

LITERAL
    ICM_Dsize = $Field_Set_Size;
MACRO
    ICM_DBLOCK = BLOCK->ICM_Dsize FIELD(ICM_Dblock_Fields) %;

LITERAL
    ICM_HSHLEN = 128,		// Length of hash table
    ICM_HSHAND = ICM_HSHLEN-1;	// && value for forming hash values

static signed long
    ICMHTB : VECTOR->ICM_HSHLEN; // ICMP database hash table

signed long
    ICMP_MIB : ICMP_MIB_struct;	// ICMP Management Information Block


//SBTTL "ICMP routing code"

// This code maintains the ICMP database, containing alternate routes to hosts
// for which we have received redirects.

ICMP_Hash(IPA)

!Hash an IP address. Returns hash value (index into ICMHTB)

    {
    return (IPA<0,8>+.IPA<8,8>+.IPA<16,8>+.IPA<24,8>) && ICM_HSHAND;
    }

ICMP_Find(IPADDR)

!Search ICMP hash chains for entry for this address.
!Returns:
//   0 - No entry exists for this IP address.
//   nonzero - pointer to ICMP data block for this address.
!N.B. If ICMP entries are ever timed out, this routine should only be called
!with AST's disabled.

    {
    signed long
	struct ICM_DBLOCK * ICPTR;
    ICPTR = ICMHTB[ICMP_HASH(IPADDR)];
    while (ICPTR != 0)
	if (ICPTR->ICM$Address == IPADDR)
	    return ICPTR
	else
	    ICPTR = ICPTR->ICM$Next;
    return 0;
    }

void ICMP_Add(GWY_Addr,struct IP_Structure * IP_Pkt) (void)

!Here to add an entry to the ICMP routing table.
//   GWY_Addr has the IP address of a gateway
//   IP_Pkt has the header and first 64-bits of the IP packet which caused
//   the ICMP redirect to be generated.

    {
    EXTERNAL ROUTINE
	LIB$GET_VM : ADDRESSING_MODE(GENERAL);
    signed long
	IPdest,
	struct ICM_DBLOCK * ICMblock;
    IPdest = IP_pkt->IPH$Dest;
    ICMblock = ICMP_Find(IPdest);
    if (ICMblock == 0)
	{
	signed long
	    HSHVAL;
	LIB$GET_VM(%REF(ICM_DSIZE*4),ICMblock);
	HSHVAL = ICMP_Hash(IPdest);
	ICMblock->ICM$Next = ICMHTB[HSHVAL];
	ICMHTB[HSHVAL] = ICMblock;
	};
    ICMblock->ICM$Address = IPdest;
    ICMblock->ICM$GWY = GWY_addr;
    }

ICMP$Check(IPaddr)

!Check if the ICMP routing table has an entry for this IP address.
!Returns:
//   0	No ICMP routing info
//  !=0	IP address of gateway from ICMP info.

    {
    signed long
	struct ICM_DBLOCK * ICMptr;
    if ((ICMptr = ICMP_Find(IPaddr)) == 0)
	return 0
    else
	return ICMptr->ICM$GWY;
    }


//SBTTL  "Handle ICMP packet input"
/*
    Decode input ICMP packet and dispatch according to packet type.
    Also, pass packet up to protocol-specific ICMP handler if we think
    that a higher-level protocol wants to know about it.
 )%

FORWARD ROUTINE
 void    ICMP_Pproblem,
 void    ICMP_Tstamp,
 void    ICMP_Info,
 void    ICMP_Echo;

ICMP$INPUT (ICMptr,ICMlen,IPptr,IPlen,bufsize,buf): NOVALUE (void)

// Main ICMP input routine.
// Verify ICMP checksum and dispatch according to function code.

    {
    EXTERNAL ROUTINE
	ICMP$User_Input;
    signed long
	ICMP_swapped  = 0,
	ICMdat,ICMtype,
	Sum,
	DataSize ;
    MAP
	struct IP_Structure * IPptr,
	struct ICMP_Structure * ICMptr;

    // Keep count of incoming packets
    ICMP_MIB->MIB$icmpInMsgs = ICMP_MIB->mib$icmpInMsgs + 1;

    Sum = Calc_checksum(ICMlen,ICMptr);
    if (sum NEQU %X"FFFF")
	{
	ICMP_MIB->MIB$icmpInErrors = ICMP_MIB->mib$icmpInErrors + 1;
	if ($$LOGF(LOG$ICMP))
	    QL$FAO("!%T ICMP recv checksum error, cksum=!XL!/",0,Sum);
	MM$Seg_Free(Bufsize,Buf);
	RETURN
	};

    // Fix-up the ICMP word ordering
    SwapBytes(ICMP_Header_Size/2,ICMptr);
    ICMdat = 0;
    ICMtype = ICMptr->ICM$TYPE;

    if ($$LOGF(LOG$ICMP))
	QL$FAO("!%T ICMP recv: type=!SL,code=!SL,var=!XL!/",
		0,ICMtype,ICMptr->ICM$CODE,ICMptr->ICM$VAR);

    // Now check to see if any users are looking at ICMP packets...
    ICMP$User_Input(IPptr->IPH$Source,IPptr->IPH$Dest,
		    bufsize,buf,
		    ICMlen,ICMptr);

    // Do IP/ICMP level processing for ICMP packet

    SELECTONE ICMtype OF
	SET

	// First are "special" ICMP message - not in reply to IP packet
	[ICM_ECHO]:
	    {
	    ICMP_MIB->MIB$icmpInEchos =
		ICMP_MIB->mib$icmpInEchos + 1;

	    ICMP_Echo(ICMptr,ICMlen,IPptr,IPlen);
	    };
	[ICM_TSTAMP]:
	    {
	    ICMP_MIB->MIB$icmpInTimeStamps =
		ICMP_MIB->mib$icmpInTimeStamps + 1;

	    ICMP_Tstamp(ICMptr,ICMlen,IPptr,IPlen);
	    };
	[ICM_IREQUEST]:
	    {
	    ICMP_Info(ICMptr,ICMlen,IPptr,IPlen);
	    };

	[ICM_AMREQUEST]:
	    {
	    ICMP_MIB->MIB$icmpInAddrMasks =
		ICMP_MIB->mib$icmpInAddrMasks + 1;

	    ICMP_Tstamp(ICMptr,ICMlen,IPptr,IPlen);
	    };

	// Others are "standard" ICMP messages,
	// and contain the header of the offending packet
	[ICM_DUNREACH,ICM_SQUENCH,ICM_REDIRECT,ICM_TEXCEED,ICM_PPROBLEM]:
	    {
	    signed long
		ptype,struct IP_Structure * IPhdr,IPdat,IPhlen,IPlen,passup;
	    passup = TRUE;
	    IPhdr = ICMptr->ICM$Data;
	    IPhlen = IPhdr->IPH$Swap_IHL*4;

// Fix the byteswapped packet

	    Swapbytes(IP_hdr_swap_size,IPhdr);
	    if ($$LOGF(LOG$ICMP))
		IP$Log(%ASCID"ICMrcv/IP",IPhdr);

// Calculate lengths, pointer to protocol data

	    ptype = IPhdr->IPH$Protocol;
	    IPdat = IPhdr+.IPhlen;
	    IPlen = IPhdr->IPH$Total_Length;
	    SELECTONE ICMtype OF
	    SET
	    [ICM_DUNREACH]:
		{
		ICMP_MIB->MIB$icmpInDestUnreachs =
			ICMP_MIB->mib$icmpInDestUnreachs + 1;

		if ($$LOGF(LOG$ICMP))
			QL$FAO("!%T ICMP recv: Destination Unreachable!/",0);
		};

	    [ICM_SQUENCH]:
		{
		ICMP_MIB->MIB$icmpInSrcQuenchs =
			ICMP_MIB->mib$icmpInSrcQuenchs + 1;

		if ($$LOGF(LOG$ICMP))
			QL$FAO("!%T ICMP recv: Source Quench!/",0);
		};

	    [ICM_REDIRECT]:
		{
		ICMP_MIB->MIB$icmpInRedirects =
			ICMP_MIB->mib$icmpInRedirects + 1;

		Swapbytes(2,ICMptr->ICM$R_GWY);
		ICMP_Add(ICMptr->ICM$R_GWY,ICMptr->ICM$Data)
		};

	    [ICM_TEXCEED]:
		{
		ICMP_MIB->MIB$icmpInTimeExcds =
			ICMP_MIB->mib$icmpInTimeExcds + 1;
		};

	    [ICM_PPROBLEM]:
		{
		ICMP_MIB->MIB$icmpInParamProbs =
			ICMP_MIB->mib$icmpInParamProbs + 1;

		if (ICMptr->ICM$Code != 0)
		    ICMdat = ICMptr->ICM$P_Ptr
		else
		    ICMdat = 0;
		if (ICMdat LSS IPdat) // IP-level problem
		    {
		    ICMP_Pproblem(ICMptr,ICMlen,IPptr,IPlen,ICMdat);
		    passup = FALSE;
		    };
		};
	    TES;

// Do higher-level processing for ICMP packet

	    DataSize = IPlen - IPhlen ;
	    if (passup)
		SELECTONE ptype OF
		SET
		[TCP_Protocol]:
		    {
		    if ((DataSize > bufsize))
			{
			OPR$FAO("Bad ICMP packet for TCP, discarding") ;
			}
		    else
			{
			SEG$ICMP(ICMtype,ICMdat,IPhdr->IPH$Source,
			     IPhdr->IPH$Dest,IPdat,DataSize,
			     buf,bufsize);
			} ;
		    RETURN;
		    };

		[UDP_Protocol]:
		    {
		    if ((DataSize > bufsize))
			{
			OPR$FAO("Bad ICMP packet for UDP, discarding") ;
			}
		    else
			{
			UDP$ICMP(ICMtype,ICMdat,IPhdr->IPH$Source,
			     IPhdr->IPH$Dest,IPdat,DataSize,
			     buf,bufsize);
			} ;
		    RETURN;
		    };

		[OTHERWISE]:
		    if ($$LOGF(LOG$ICMP))
			QL$FAO("!%T ICMP - Unknown protocol !XL!/",0,ptype);
		TES;
	    };

	[ICM_TSREPLY] :
	    {
	    ICMP_MIB->MIB$icmpInTimestampReps =
		ICMP_MIB->mib$icmpInTimestampReps + 1;
	    QL$FAO("!%T ICMP recv: Time Stamp reply. how did this happen?!/",0)
	    };

	[ICM_IREPLY] :
	    {
	    QL$FAO("!%T ICMP recv: Info reply. how did this happen?!/",0)
	    };

	[ICM_AMREPLY] :
	    {
	    ICMP_MIB->MIB$icmpInAddrMaskReps =
		ICMP_MIB->mib$icmpInAddrMaskReps + 1;
	    QL$FAO("!%T ICMP recv: AddrMask reply. how did this happen?!/",0)
	    };

	[ICM_EREPLY]:
	    {
	    ICMP_MIB->MIB$icmpInEchoReps =
		ICMP_MIB->mib$icmpInEchoReps + 1;
	    QL$FAO("!%T ICMP recv: Echo reply!/",0)
	    };

	[OTHERWISE]:
	    {
	    ICMP_MIB->MIB$icmpInErrors = ICMP_MIB->mib$icmpInErrors + 1;
	    if ($$LOGF(LOG$ICMP))
		QL$FAO("!%T ICMP recv: unknown ICMP type !XL!/",0,ICMtype);
	    };
    TES;

    // Release the buffer
    MM$Seg_Free(BUFSIZE,BUF);
    }

ICMP_Pproblem(ICMpkt,ICMlen,IPpkt,IPlen,ICMpptr) : NOVALUE (void)

// Handle ICMP Parameter Problem packet.
// Called if problem is at the IP level. This is unexpected and is probably
// a bug at the remote end, since we don't send any IP options.

    {
    XQL$FAO(LOG$ICMP,"!%T ICMP recv: IP param=!XL!/",ICMpptr);
    }

ICMP_Echo(ICMpkt,ICMlen,IPpkt,IPlen) : NOVALUE (void)

// Handle ICMP echo request.
// Copy packet, convert to echo reply, queue for output.
!~~~ This routine should be simpler - should just reverse the packet and
!~~~ queue for output, instead of creating new packet, etc.

    {
    MAP
	struct ICMP_Structure * ICMpkt,
	struct IP_Structure * IPpkt;
    signed long
	Buf,
	Bufsize,
	struct ICMP_Structure * Seg,
	Segsize,
	DESC$STR_ALLOC(srcstr,20),
	DESC$STR_ALLOC(dststr,20);

// Check the TTL to see if this packet has expired

    IPPKT->IPH$TTL = IPPKT->IPH$TTL - 1;
    if (IPPKT->IPH$TTL <= 0)
	{
	if ($$LOGF(LOG$ICMP))
	    {
	    ASCII_DEC_BYTES(srcstr,4,IPPKT->IPH$SOURCE,
			    srcstr->DSC$W_LENGTH);
	    ASCII_DEC_BYTES(dststr,4,IPPKT->IPH$DEST,
			    dststr->DSC$W_LENGTH);
	    ICMP_MIB->MIB$icmpOutErrors =
		ICMP_MIB->mib$icmpOutErrors + 1;
	    QL$FAO("!%T ICMP_ECHO: TTL exceeded, SRC=!AS,DST=!AS,ID=!UL!/",
		   0,srcstr,dststr,IPPKT->IPH$Ident);
	    };
	RETURN;
	};

// Calculate size of physical buffer to use

    Bufsize = Device_Header + IPlen;
    if (Bufsize <= MIN_PHYSICAL_BUFSIZE)
	Bufsize = MIN_PHYSICAL_BUFSIZE
    else
	if (Bufsize <= MAX_PHYSICAL_BUFSIZE)
	    Bufsize = MAX_PHYSICAL_BUFSIZE
	else
	    {
	    if ($$LOGF(LOG$ICMP))
		{
		ASCII_DEC_BYTES(srcstr,4,IPPKT->IPH$SOURCE,
			        srcstr->DSC$W_LENGTH);
		ASCII_DEC_BYTES(dststr,4,IPPKT->IPH$DEST,
				dststr->DSC$W_LENGTH);

		ICMP_MIB->MIB$icmpOutErrors =
			ICMP_MIB->mib$icmpOutErrors + 1;
		QL$FAO("!%T ICMP_ECHO: PKT too large,SRC=!AS,DST=!AS,ID=!UL!/",
		   0,srcstr,dststr,IPPKT->IPH$Ident);
		};
	    RETURN;
	    };

// Allocate the segment.

    Buf = MM$Seg_Get(Bufsize);
    Seg = Buf + Device_header + IP_hdr_byte_size;
    Segsize = ICMlen;

// Copy the packet

    CH$MOVE(ICMlen, ICMpkt, Seg);

// Turn it into an echo reply

    Seg->ICM$TYPE = ICM_EREPLY;

// Calculate ICMP checksum

    Seg->ICM$CKSUM = 0;

// Do logging, if necessary

    if ($$LOGF(LOG$ICMP))
	QL$FAO("!%T ICMP echo rply: id=!SL, seq=!SL!/",
	       0, Seg->ICM$E_ID, Seg->ICM$E_SEQ);

// Swap the header bytes and compute the checksum

    SwapBytes(ICMP_Header_Size/2,Seg);
    Seg->ICM$CKSUM = Calc_Checksum(Segsize, Seg);

// Send packet, preserving ID, TOS, TTL, etc.
!!!HACK!!// IPPKT->IPH$DEST is wrong//  what about broadcasts?

    IP$S}(IPPKT->IPH$DEST, IPPKT->IPH$SOURCE, IPPKT->IPH$Type_Service,
	    IPPKT->IPH$TTL, Seg, Segsize, IPPKT->IPH$IDent,
	    False, True, ICMP_Protocol, Buf, Bufsize);

    // Keep count of outgoing packets
    ICMP_MIB->MIB$icmpOutMsgs = ICMP_MIB->mib$icmpOutMsgs + 1;

    ICMP_MIB->MIB$icmpOutEchoReps =
	ICMP_MIB->mib$icmpOutEchoReps + 1;
    }

ICMP_Tstamp(ICMpkt,ICMlen,IPpkt,IPlen) : NOVALUE (void)

// Handle ICMP Timestamp request.
// Copy packet, convert to Timestamp Reply, queue for output.
// Not yet implemented.

    {
    RETURN;
    }

ICMP_Info(ICMpkt,ICMlen,IPpkt,IPlen) : NOVALUE (void)

// Handle ICMP Information request
// Copy packet, convert to Information Reply, queue for output.
// Not yet implemented.

    {
    RETURN;
    }

ICMP_Send_DUNR(ICMpkt,ICMlen,IPpkt,IPlen,Code) : NOVALUE (void)

// Send ICMP Destination Unreachable
// Copy packet, queue for output.

    {
    MAP
	struct ICMP_Structure * ICMpkt,
	struct IP_Structure * IPpkt;
    signed long
	Buf,
	Bufsize,
	struct ICMP_Structure * Seg,
	Segsize,
	DESC$STR_ALLOC(srcstr,20),
	DESC$STR_ALLOC(dststr,20);

// Calculate size of physical buffer to use

    Bufsize = Device_Header + IPlen;
    if (Bufsize <= MIN_PHYSICAL_BUFSIZE)
	Bufsize = MIN_PHYSICAL_BUFSIZE
    else
	if (Bufsize <= MAX_PHYSICAL_BUFSIZE)
	    Bufsize = MAX_PHYSICAL_BUFSIZE
	else
	    {
	    if ($$LOGF(LOG$ICMP))
		{
		ASCII_DEC_BYTES(srcstr,4,IPPKT->IPH$SOURCE,
			        srcstr->DSC$W_LENGTH);
		ASCII_DEC_BYTES(dststr,4,IPPKT->IPH$DEST,
				dststr->DSC$W_LENGTH);

		ICMP_MIB->MIB$icmpOutErrors =
			ICMP_MIB->mib$icmpOutErrors + 1;
		QL$FAO("!%T ICMP_ECHO: PKT too large,SRC=!AS,DST=!AS,ID=!UL!/",
		   0,srcstr,dststr,IPPKT->IPH$Ident);
		};
	    RETURN;
	    };

// Allocate the segment.

    Buf = MM$Seg_Get(Bufsize);
    Seg = Buf + Device_header + IP_hdr_byte_size;
    Segsize = ICMlen;

// Copy the packet

    CH$MOVE(ICMlen, ICMpkt, Seg);

// Set Type and Code

    Seg->ICM$TYPE = ICM_DUNREACH ;
    Seg->ICM$CODE = code ;

// Calculate ICMP checksum

    Seg->ICM$CKSUM = 0;

// Do logging, if necessary

    if ($$LOGF(LOG$ICMP))
	QL$FAO("!%T ICMP send DUNR: id=!SL, seq=!SL, code=!SL!/",
	       0, Seg->ICM$E_ID, Seg->ICM$E_SEQ, Seg->ICM$CODE);

// Swap the header bytes and compute the checksum

    SwapBytes(ICMP_Header_Size/2,Seg);
    Seg->ICM$CKSUM = Calc_Checksum(Segsize, Seg);

// Send packet, preserving ID, TOS, TTL, etc.
!!!HACK!!// IPPKT->IPH$DEST is wrong//  what about broadcasts?

    IP$S}(IPPKT->IPH$DEST, IPPKT->IPH$SOURCE, IPPKT->IPH$Type_Service,
	    ICMPTTL, Seg, Segsize, IPPKT->IPH$IDent,
	    False, True, ICMP_Protocol, Buf, Bufsize);

    // Keep count of outgoing packets
    ICMP_MIB->MIB$icmpOutMsgs = ICMP_MIB->mib$icmpOutMsgs + 1;

    ICMP_MIB->MIB$icmpOutDestUnreachs =
	ICMP_MIB->mib$icmpOutDestUnreachs + 1;
    }

} ELUDOM
