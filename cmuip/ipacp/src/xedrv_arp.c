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
//TITLE "XEDrv Address Resolution Protocol handler"
!++
// Module:
!
!	XEDrv_ARP - Handle common portions of Address Resolution Protocol
!
// Facility:
!
!	Provides a common mechanism for building ARP packets and for
!	maintaining a cache of ARP responses for doing IP to hardware
!	address translation on Digital's EtherNet devices.
!
// Abstract:
!
!	Exports the following routines for use by the device drivers:
VOID !	    ARP_INIT
!		Initialize the ARP package.
VOID !	    ARP_DEV_INIT(DEVIDX,HWTYPE,IPTYPE,HWADDR,HDRLEN)
!		Initialize the ARP parameters for a device.
VOID !	    ARP_INPUT(DEVIDX)
!		Handle reception of an ARP packet by the device driver.
!		Adds ARP information to the cache or generates a reply.
!	    ARP_CHECK(DEVIDX, IPADDR)
!		Perform address resolution on specified IP address.
!		If the address exists in the cache, the corresponding
!		physical address is returned. Otherwise, an ARP request
!		packet is generated and transmitted.
!	    ARP_DUMP(ACIDX,RBLOCK,RBSIZE)
!		Dump out part of the ARP cache.
!
// Author:
!
!	Vince Fuller, CMU-CSD, April 1986
!	Copyright (c) 1986, 1987, Vince Fuller and Carnegie-Mellon University
!
// Change log:
!
!1.9A	07-Aug-1991	Henry W. Miller		USBR
!	Numerous changes:
!
!	Use LIB$GET_VM_PAGE/LIB$FREE_VM_PAGE rather than LIB$GET_VM/
!	LIB$FREE_VM.
!	Check error returns from memory allocation routines and handle
!	properly.
!	Handle error returns properly in XE$ARP_CHECK() and other places.
!
//     06-Feb-90, Bruce R. Miller	CMU Network Development
!	Made the module EtherNet specific since it practically was anyways.
!	Moved it from the IPACP into the XEDrv.EXE image.
!
//     03-Jan-89, Edit bu DHP
!	Add patches by Charles Lane (lane@duphy4.drexel.edu) to allow
!	reading of broadcasts
!	Fixed them so that we wouldn't respond with an ARP for the broadcast
!	address
!
!	06-SEP-1988	Dale Moore	CMU-Computer Science
!	Was using a local variable before initialization.
!	The variable was the source IP address of the incoming
!	ARP request.
!
// 1.9  16-Sep-87, Edit by VAF
!	Fix longstanding bug - ARP_UPDATE was not passing hardware length to
!	ARP_CNEW as it expected. It's a wonder the code ever worked.
!
// 1.8  24-Feb-87, Edit by VAF
!	Flush reference to Q_MESSAGE.
!
// 1.7  28-Oct-86, Edit by VAF
!	Try to keep the IP packet around in ARP_CHECK and retransmit it when
!	we get a reply (in ARP_UPDATE).
!
// 1.6  12-Aug-86, Edit by VAF
!	Teach ARP_INPUT about "cloned" devices.
!
// 1.5  29-Aug-86, Edit by VAF
!	Add code to do cache sweeping every 2 minutes.
!	Add code to try and refresh cache entries when ARP_CHECK detects that
!	an entry is getting near expiration.
!
// 1.4  28-Aug-86, Edit by VAF
!	Add ARP_DUMP routine - dump out cache blocks.
!
// 1.3  29-May-86, Edit by VAF
!	Check for nonexistant ARP block in ARP_INPUT, ARP_CHECK
!	Allow device ARP block to be reinitialized
!
// 1.2  20-May-86, Edit by VAF
!	New AST locking scheme (NOINT/OKINT macros)
!
// 1.1  21-Apr-86, Edit by VAF
!	Phase II of flushing XPORT - use $FAO instead of XPORT string stuff.
!--

MODULE ARP( IDENT="1.9A",LANGUAGE(BLISS32),
	    ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			    NONEXTERNAL=LONG_RELATIVE),
	    LIST(NOEXPAND,TRACE,NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	    OPTIMIZE,OPTLEVEL=3,ZIP)=

{

#include "SYS$LIBRARY:STARLET";
!LIBRARY "TCPMACROS";
!LIBRARY "STRUCTURE";
!LIBRARY "VMS";

#include "CMUIP_SRC:[central]NETXPORT";   // VMS peculiarities.
#include "CMUIP_SRC:[central]NETVMS";   // VMS peculiarities.
#include "CMUIP_SRC:[central]NETCOMMON";   // CMU-OpenVMS/IP common decls
#include "CMUIP_SRC:[central]NETCONFIG";   // Device interface specs.
#include "CMUIP_SRC:[central]NETDEVICES";  // Helpfull macros...

#include "XEDRV";
#include "XEARP";


// Externals used by this module

extern signed long
    struct IPACP_Info_Structure * IPACP_Interface;

extern

// NETMACLIB.OBJ
    Time_Stamp,
    SWAPBYTES,

// NETDEVICES.OBJ
 VOID    ASCII_HEX_BYTES,
 VOID    ASCII_DEC_BYTES,

// XEDRV.BLI
    XE$Xmit,
    XE$ARP_Xmit,

//    STR$GET1_DX : ADDRESSING_MODE(GENERAL),
    LIB$GET_VM : ADDRESSING_MODE(GENERAL),
    LIB$GET_VM_PAGE : ADDRESSING_MODE(GENERAL),
    LIB$FREE_VM : ADDRESSING_MODE(GENERAL),
    LIB$FREE_VM_PAGE : ADDRESSING_MODE(GENERAL);

// Structure of device ARP data block

$FIELD ARP_BLK_FIELDS =
    SET
    AB_HWSIZE		= [$BYTES(2)],	// Size of hardware address
    AB_HDRLEN		= [$BYTES(2)],	// ARP buffer header length
    AB_FLAGS		= [$BYTES(2)],  // ARP flags
    $OVERLAY(AB_FLAGS)
	AB_SWAP_PAR	= [$BIT],	// If HWTYPE, etc need to be swapped
	AB_SWAP_16      = [$BIT],	// If 3MB-style word swapping needed
    $CONTINUE
    AB_HWADDR		= [$BYTES(ARP_HDW_LEN)], // Hardware address
    AB_IPADDR		= [$BYTES(4)],	// ARP IP address
    $ALIGN(FULLWORD)
    AB_SWP_START	= [$BYTES(0)],	// Start of items to be swapped...
    AB_HWTYPE		= [$BYTES(2)],	// ARP hardware type code
    AB_IPTYPE		= [$BYTES(2)],	// ARP/IP hardware protocol code
    AB_RQUEST		= [$BYTES(2)],	// Code for ARP REQUEST
    AB_REPLY		= [$BYTES(2)],	// Code for ARP REPLY
    $ALIGN(FULLWORD)			// Don't let SWAPBYTES get carried away
    AB_SWP_}		= [$BYTES(0)]	// End of items to be swapped...
    TES;
LITERAL ARP_BLK_LEN = $FIELD_SET_SIZE;
MACRO ARP_BLK = BLOCK->ARP_BLK_LEN FIELD(ARP_BLK_FIELDS) %;

// Expiration time for ARP cache entries

LITERAL
    ARP_INI_TIME = 1*60*100,	// 1 minute for unresolved cache entries
    ARP_EXP_TIME = 10*60*100,	// 10 minutes for cache timeout
    ARP_EXP_HALF = ARP_EXP_TIME/2, // 5 minutes to refresh cache entry
    ARP_RFTIME = 30*100;	// Minimum interval between refresh attempts

// Structure of an ARP cache entry

$FIELD ACACHE_FIELDS =
    SET
    AC$NEXT	= [$ADDRESS],	// Next entry on hash chain
    AC$EXPIRE	= [$LONG_INTEGER], // Expiration time of this entry
    AC$RFTIME	= [$LONG_INTEGER], // Next time to try refresh
    AC$DEVICE	= [$ADDRESS],	// pntr to dev_config entry of this address
    AC$IPADDR	= [$BYTES(4)],	// IP address
    AC$SAVEQB	= [$ADDRESS],	// Pointer to saved IP packet if nonzero
    AC$FLAGS	= [$BYTES(2)],	// Status flags
    $OVERLAY(AC$FLAGS)
      AC$VALID	= [$BIT],	// Nonzero if this entry valid
    $CONTINUE
    AC$HWSIZE	= [$BYTES(2)],	// Length of this address
    AC$HWADDR	= [$BYTES(ARP_HDW_LEN)] // Physical address
    TES;
LITERAL ACACHE_LEN = $FIELD_SET_SIZE;
MACRO ACACHE_BLK = BLOCK->ACACHE_LEN FIELD(ACACHE_FIELDS) %;

// Hash table for hashing IP addresses

LITERAL
    ARP_HSHLEN = 128,		// Length of hash table
    ARP_HSHAND = ARP_HSHLEN-1;	// && value for forming hash values
static signed long
    ARP_SWP_TIME : VECTOR->2,	// Delta time to sweep cache
    ARPHTB : VECTOR->ARP_HSHLEN;

MACRO
    ARP_SWP_TTXT_STR = "0000 00:02:00.00"%; // Every 2 minutes...



MACRO XEARP$_LOG (NAME)=
    {
    signed long
	STR_DESC : VECTOR->2;

    STR_DESC->0 = %CHARCOUNT(NAME);
    STR_DESC->1 = UPLIT(NAME);

    XEARP$LOG(STR_DESC,%REMAINING)
    } %;

XEARP$LOG(NAME,IPADDR,HWLEN,HWADDR) : NOVALUE =

// Write a logging entry for ARP.

    {
    signed long
	DESC$STR_ALLOC(IPSTR,20),
	DESC$STR_ALLOC(PHYSTR,50);

// Translate IP address and physical address into ASCII

    ASCII_DEC_BYTES(IPSTR,4,IPADDR,IPSTR->DSC$W_LENGTH);
    ASCII_HEX_BYTES(PHYSTR,HWLEN,HWADDR,PHYSTR->DSC$W_LENGTH);

// Queue up a message for later output

    DRV$QL_FAO("!%T !AS: IP=!AS, PHY=!AS!/",0,NAME,IPSTR,PHYSTR);
    };


FORWARD ROUTINE
 VOID    ARP_SWEEP;

XEARP$INIT : NOVALUE =

// Initialize ARP package.
// Initializes all hash buckets to be circular lists.
// Also, initialize cache sweep time and start timer.

    {
    signed long
	ARP_SWP_TTXT : VECTOR->2;

    ARP_SWP_TTXT->0 = %CHARCOUNT( ARP_SWP_TTXT_STR );
    ARP_SWP_TTXT->1 = UPLIT ( ARP_SWP_TTXT_STR );

    INCR I FROM 0 TO (ARP_HSHLEN-1) DO
	ARPHTB[I] = 0;
    $BINTIM(TIMBUF = ARP_SWP_TTXT,
	    TIMADR = ARP_SWP_TIME);
    $SETIMR(DAYTIM = ARP_SWP_TIME,
	    ASTADR = ARP_SWEEP);
    };

LITERAL
    MAX_HDR_SIZE = 100;		// Max size of device header on ARP packet

XEARP$DEV_INIT(XE_Int,HWTYPE,IPTYPE,HWADDR,HDRSIZ,
VOID 			    SWAPPF,SWAP16F) =

// Initialize ARP portions of device status block entry.
// Inputs:
//   HWTYPE - Hardware address space code
//   IPTYPE - IP protocol code for that hardware
//   HWADDR - Pointer to byte string for hardware address
//   HDRSIZ - Size of ARP receive buffer header
//   SWAPPF - On if parameters need to be byteswapped
//   SWAP16F - On if 3mb-style word swapping is needed
// Outputs:
//   None.
// Side effects:
//   Creates DC_ARP_BLOCK entry in DEV_CONFIG table entry for device.
//   Must be called AFTER device init routine has been called.

    {
    MAP
	struct XE_Interface_Structure * XE_Int;
    signed long
	struct Device_Configuration_Entry * dev_config,
	struct ARP_BLK * ARBLK,
	RC ;

    dev_config = XE_Int->XEI$dev_config;

    if (HDRSIZ > MAX_HDR_SIZE)
	{
	DRV$WARN_FAO("XE header size exceeds ARP max, size=!UL",HDRSIZ);
	DRV$FATAL_FAO("Device is !AS",dev_config->dc_devname);
	};

// If this device already has an ARP block, use it, else allocate one

    if (XE_Int->XEI$ARP_BLOCK != 0)
	ARBLK = XE_Int->XEI$ARP_BLOCK
    else
!	LIB$GET_VM(%REF(ARP_BLK_LEN*4),ARBLK);
	RC = LIB$GET_VM_PAGE(%REF(((ARP_BLK_LEN*4) / 512) + 1), ARBLK);
	if (NOT RC)
	    {
	    DRV$WARN_FAO("XE ARP memory allocation error, RC=!XL",RC);
	    DRV$FATAL_FAO("Device is !AS",dev_config->dc_devname);
	    };

// Fill in the fields

    ARBLK->AB_IPADDR = DEV_CONFIG->DC_IP_ADDRESS;
    ARBLK->AB_HWTYPE = HWTYPE;
    ARBLK->AB_IPTYPE = IPTYPE;
    ARBLK->AB_HDRLEN = HDRSIZ;
    ARBLK->AB_HWSIZE = XE_Int->XEI$PHY_SIZE;
    CH$MOVE(ARBLK->AB_HWSIZE,CH$PTR(HWADDR),CH$PTR(ARBLK->AB_HWADDR));
    ARBLK->AB_RQUEST = AR$OP_REQUEST;
    ARBLK->AB_REPLY = AR$OP_REPLY;
    ARBLK->AB_SWAP_PAR = SWAPPF;
    ARBLK->AB_SWAP_16 = SWAP16F;

// If byteswapping is needed, then some fields need to be fixed

    if (SWAPPF)
	SWAPBYTES((ARBLK[AB_SWP_}]-ARBLK->AB_SWP_START)/2,
		  ARBLK->AB_SWP_START);
    XE_Int->XEI$ARP_BLOCK = ARBLK;
    };

FORWARD ROUTINE
    ARP_HASH,
    ARP_FIND,
    ARP_CNEW;

XEARP$CHECK(XE_Int, IPADDR, HWADDR, QB)

// Get physical address on an interface for given IP address.
// Inputs:
//   XE_Int - Address of XE interface block for this device.
//   IPADDR - IP address to resolve
//   HWADDR - Pointer to area to store the physical address
//   QB - Address of QBLK for packet to retransmit on ARP reply
// Outputs:
//   +1 on success, item was in the cache, HWADDR updated with physical address
//    0 on failure, item not in queue, ARP transmitted
//   -1 on failure, item not in queue, ARP transmitted, QB kept for later use

    {
    MAP
	struct BLOCK * QB[] FIELD (QB_Net_Send),
	struct XE_Interface_Structure * XE_Int;
    OWN
	NULADR : VECTOR[CH$ALLOCATION(ARP_HDW_LEN)];
    signed long
        XE_DESC : VECTOR->2,
	FOUND,
	RFLAG,
	struct ACACHE_BLK * ACPTR,
	struct ARP_BLK * ARBLK;
    MACRO
	XE_PREFRIX_STR = "XE:" %;

    XE_DESC->0 = %CHARCOUNT( XE_PREFRIX_STR );
    XE_DESC->1 = UPLIT( XE_PREFRIX_STR );
    CH$FILL(0,ARP_HDW_LEN,NULADR);

// Get, validate pointer to device ARP block.

    if ((ARBLK = XE_Int->XEI$ARP_BLOCK) == 0)
	{
	DRV$OPR_FAO("ARP_CHECK: Device not initialized, DEV=!XL",XE_DESC);
	RETURN 0;
	};

// See if this address is in the cache. Must run NOINT between ARP_FIND and
// references to ACPTR to prevent cache from being modified.

    RFLAG = 0;			// Clear retransmit flag
    FOUND = 0;			// and return status
    DRV$NOINT;			// Disable AST's through here
    ACPTR = ARP_FIND(IPADDR,XE_Int);

    if (ACPTR == 0)
	{
	ACPTR = ARP_CNEW(IPADDR,XE_Int,ARBLK->AB_HWSIZE);
	if (ACPTR == -1)
	    {
	    DRV$OKINT;			// Re-allow AST now
	    RETURN FOUND ;
	    } ;
	if (QB->NSQ$DELETE)
	    {
	    ACPTR->AC$SAVEQB = QB;
	    FOUND = -1
	    }
	}
    else
	{			// Found it - copy cached data and give success
	signed long
	    HWLEN,
	    NOW,
	    CTIME;

// Ignore this entry if it isn't valid yet

	if (ACPTR->AC$VALID)
	    {

// Ignore this entry if it has expired

	    NOW = Time_Stamp();
	    CTIME = ACPTR->AC$EXPIRE - NOW;
	    if (CTIME > 0)
		{
		FOUND = 1;
		HWLEN = ACPTR->AC$HWSIZE;
		CH$MOVE(HWLEN,ACPTR->AC$HWADDR,CH$PTR(HWADDR));
		if ($$LOGF(LOG$ARP))
		    XEARP$_LOG("ARP cache hit",IPADDR,HWLEN,HWADDR);

// See if we should try to refresh this entry now.

		IF (CTIME LSS ARP_EXP_HALF) && 
		   (NOW > ACPTR->AC$RFTIME) THEN
		    {
		    ACPTR->AC$RFTIME = NOW + ARP_RFTIME;
!		    RFLAG = -1;
		    };
		}
	    else
		if (QB->NSQ$DELETE)
		    {
		    ACPTR->AC$SAVEQB = QB;
		    FOUND = -1
		    };
	    };
	};

    DRV$OKINT;			// Re-allow AST now

// Now, we'll send an ARP if one of the following is true:
//   - The IP address was not found in the cache (FOUND = 0)
//   - The cache entry is near expiration (CTIME LSS ARP_EXP_HALF)

    if ((FOUND <= 0) || (RFLAG != 0))
	{
	signed long
	    ARPLEN,
	    HWLEN,
	    HDRLEN,
	    ARPKT : BLOCK[MAX_HDR_SIZE+ARP_LEN],
	    struct ARP_PKT * ARBUF,
	    SWPTMP,
	    APTR,
	    DPTR;

// Get ARP packet format, make a pointer to the packet itself

	HDRLEN = ARBLK->AB_HDRLEN;
	HWLEN = ARBLK->AB_HWSIZE;
	ARBUF = ARPKT+.HDRLEN;

// Set fixed fields in ARP packet

	ARBUF->AR$HRD = ARBLK->AB_HWTYPE;
	ARBUF->AR$PRO = ARBLK->AB_IPTYPE;
	ARBUF->AR$HLN = HWLEN;
	ARBUF->AR$PLN = ARP_IP_LEN;
	ARBUF->AR$OP = ARBLK->AB_RQUEST;

// Now, fill in the dynamic portions.

	APTR = CH$PTR(ARBUF->AR$XTRA);

// Source physical address (n bytes)

	DPTR = APTR;
	APTR = CH$MOVE(HWLEN,CH$PTR(ARBLK->AB_HWADDR),APTR);

// Source IP address (m bytes)

	DPTR = APTR;
	APTR = CH$MOVE(ARP_IP_LEN,CH$PTR(ARBLK->AB_IPADDR),APTR);

// Target hardware address (unknown)

	APTR = CH$FILL(0,HWLEN,APTR);

// Target IP address (m bytes)

	DPTR = APTR;
	APTR = CH$MOVE(ARP_IP_LEN,CH$PTR(IPADDR),APTR);

// Calculate length

	ARPLEN = APTR-.ARBUF;

// See if we need to byteswap the words

	if (ARBLK->AB_SWAP_16)
	    {
	    SWAPBYTES(1,ARBUF->AR$HLN);
	    SWAPBYTES((HWLEN+ARP_IP_LEN)/2,ARBUF->AR$XTRA);
	    };

	if ($$LOGF(LOG$ARP))
	    {
	    signed long
		ADRPTR;
	    if (FOUND == 0)
		ADRPTR = NULADR
	    else
		ADRPTR = HWADDR;
	    XEARP$_LOG("ARP RQ XMIT",IPADDR,HWLEN,ADRPTR);
	    };

// Now, call the device-dependant routine to send an ARP
// HWADDR either points to the hardware address string (if we set it above), or
// it is 0, which tells the ARP_XMIT routine to send a broadcast.

	if (FOUND <= 0)
	    HWADDR = 0;
	XE$ARP_Xmit(XE_Int,ARBUF,ARPLEN,HWADDR);
	};

// Return appropriate reply

    RETURN FOUND;
    };

FORWARD ROUTINE
 VOID    ARP_UPDATE;

void XEARP$INPUT ( XE_Int , struct ARP_PKT * ARBUF ) =

// Handle ARP reception by device driver.
// Called at AST level when an ARP packet has been
// received.
// Inputs:
//   ARBUF - Pointer to ARP portion of packet.
//   XE_INT - Address of XE Interface block
// Outputs:
//   none.
// Side effects:
//   If the ARP packet is for this host then
!	If it is an ARP reply, the ARP cache is updated
//       Else
!	    If it is an ARP request, a reply is generated and transmitted
!	    Else drop packet
//   Else drop packet

    {
    MAP
	struct XE_Interface_Structure * XE_Int;
    signed long
	XDEV,
	APTR,
	HWLEN,
	PROTYPE,
	struct ARP_BLK * ARBLK,
	AR_SHA : $BBLOCK->ARP_HDW_LEN,
	AR_SPA,
	AR_THA : $BBLOCK->ARP_HDW_LEN,
	AR_TPA;

    if ((ARBLK = XE_Int->XEI$ARP_BLOCK) == 0)
	{
	DRV$OPR_FAO("ARP_INPUT: Device not initialized, XEI=!XL",XE_Int);
	RETURN;
	};

    HWLEN = ARBLK->AB_HWSIZE;

// Check some fields in the ARP header

    IF (ARBUF->AR$HRD == ARBLK->AB_HWTYPE) AND
       (ARBUF->AR$PRO == ARBLK->AB_IPTYPE) AND
       (ARBUF->AR$HLN == HWLEN) AND
       (ARBUF->AR$PLN == ARP_IP_LEN) THEN
	{

// See if we need to byteswap the words

	if (ARBLK->AB_SWAP_16)
	    {
	    SWAPBYTES(1,ARBUF->AR$HLN);
	    SWAPBYTES((HWLEN+ARP_IP_LEN)/2,ARBUF->AR$XTRA);
	    };

// Copy the dynamic fields out of the ARP buffer. Order is same as above

	APTR = CH$PTR(ARBUF->AR$XTRA);
	CH$MOVE(HWLEN,APTR,CH$PTR(AR_SHA));
	APTR = CH$PLUS(APTR,HWLEN);
	CH$MOVE(ARP_IP_LEN,APTR,CH$PTR(AR_SPA));
	APTR = CH$PLUS(APTR,ARP_IP_LEN);
	CH$MOVE(HWLEN,APTR,CH$PTR(AR_THA));
	APTR = CH$PLUS(APTR,HWLEN);
	CH$MOVE(HWLEN,APTR,CH$PTR(AR_TPA));

// See if this packet is for us

	XDEV = DRV$IP_ISME(AR_TPA, XE_Int);
	if (XDEV GEQ 0)

// Yes - select on packet type

	    {
	    SELECTONE ARBUF->AR$OP OF
	    SET
	    [ARBLK->AB_RQUEST]:
		{
		signed long
		    DPTR,ARPLEN,IPADDR;

// If he wants to talk to us, chances are we'll want to talk back - update
// our cache entry for him now.
// N.B. ARP_UPDATE assumes that we are NOINT or at AST level.

		IPADDR = AR_SPA;
		ARP_UPDATE ( IPADDR , XE_Int , HWLEN , AR_SHA );

		if ($$LOGF(LOG$ARP))
		    XEARP$_LOG("ARP RQ RCV",IPADDR,HWLEN,AR_SHA);

// Doing reply now

		ARBUF->AR$OP = ARBLK->AB_REPLY;
		APTR = CH$PTR(ARBUF->AR$XTRA);

// Fill in my physical address

		DPTR = APTR;
		APTR = CH$MOVE(HWLEN,CH$PTR(ARBLK->AB_HWADDR),APTR);

// Reverse the destination IP address

		DPTR = APTR;
		APTR = CH$MOVE(ARP_IP_LEN,CH$PTR(AR_TPA),APTR);

// And switch the source to the destination

		APTR = CH$MOVE(HWLEN,CH$PTR(AR_SHA),APTR);
		APTR = CH$MOVE(ARP_IP_LEN,CH$PTR(AR_SPA),APTR);

		ARPLEN = APTR-.ARBUF;

// See if we need to byteswap the words (again, *sigh*)

		if (ARBLK->AB_SWAP_16)
		    {
		    SWAPBYTES(1,ARBUF->AR$HLN);
		    SWAPBYTES((HWLEN+ARP_IP_LEN)/2,ARBUF->AR$XTRA);
		    };

// Do an ARP output

		XE$ARP_Xmit(XE_Int,ARBUF,ARPLEN,AR_SHA);
		};


	    [ARBLK->AB_REPLY]:
		{
		signed long
		    IPADDR,
		    HWADDR : $BBLOCK->ARP_HDW_LEN,
		    ACPTR;

// Extract the source hardware address

		APTR = CH$PTR(ARBUF->AR$XTRA);
		CH$MOVE(HWLEN,APTR,CH$PTR(HWADDR));
		APTR = CH$PLUS(APTR,HWLEN);

// Extract and swap source IP address into longword form

		CH$MOVE(ARP_IP_LEN,APTR,CH$PTR(IPADDR));

// Add this entry to the cache, replacing old entry if necessary
// N.B. We are at AST level here, which makes this OK

		ARP_UPDATE ( IPADDR , XE_Int , HWLEN , HWADDR );

		if ($$LOGF(LOG$ARP))
		    XEARP$_LOG("ARP RPLY RCV",IPADDR,HWLEN,HWADDR);
		};
	    TES;
	    };
	};
    };


ARP_HASH(IPA)

// Hash an IP address.
// Returns hash value for this IP address.

    {
    RETURN (IPA<0,8>+.IPA<8,8>+.IPA<16,8>+.IPA<24,8>) && ARP_HSHAND;
    };

ARP_FIND ( IPADDR , struct XE_Interface_Structure * XE_Int )

// Find hash table entry for this IP address.
// Returns 0 if entry is not in cache, or nonzero pointer to entry.
// N.B. Non-AST routines should take care to disable AST's between calling
// ARP_FIND and using the returned info.

    {
    signed long
	struct ACACHE_BLK * ACPTR,
	AHEAD;

    ACPTR = ARPHTB[ARP_HASH(IPADDR)];
    !!!HACK!!// Screw this.  give each XE interface it's own hash table.
    WHILE ACPTR != 0 DO
	IF (ACPTR->AC$DEVICE == XE_Int) AND
	   (ACPTR->AC$IPADDR == IPADDR) THEN
	    RETURN ACPTR
	else
	    ACPTR = ACPTR->AC$NEXT;
    RETURN 0;
    };

ARP_CNEW(IPADDR,XE_Int,HWLEN)

// Create a new ARP cache entry. Assumes that it doesn't already exist.
// Returns a pointer to the new entry.

    {
    MAP
	struct XE_Interface_Structure * XE_Int;
    signed long
	struct ACACHE_BLK * ACPTR,
	HSHVAL,
	RC ;

// Get a new block and hash the address.

//    LIB$GET_VM(%REF(ACACHE_LEN*4),ACPTR);
    RC = LIB$GET_VM_PAGE(%REF(((ACACHE_LEN * 4) / 512) + 1), ACPTR);
    if (NOT RC)
	{
	DRV$WARN_FAO("XE ARP CNEW memory allocation error, RC=!XL",RC);
	RETURN -1 ;
	};
    HSHVAL = ARP_HASH(IPADDR);

// Insert IP address, device index and hardware length

    ACPTR->AC$IPADDR = IPADDR;
    ACPTR->AC$DEVICE = XE_Int;
    ACPTR->AC$HWSIZE = HWLEN;

// Initialize remaining fields

    CH$FILL(0,HWLEN,CH$PTR(ACPTR->AC$HWADDR));
    ACPTR->AC$SAVEQB = 0;
    ACPTR->AC$FLAGS = 0;
    ACPTR->AC$RFTIME = 0;
    ACPTR->AC$EXPIRE = Time_Stamp()+ARP_INI_TIME;

// Insert this block into the hash chain. N.B. It is important this this step
// be done either at AST level or NOINT.

    ACPTR->AC$NEXT = ARPHTB[HSHVAL];
    ARPHTB[HSHVAL] = ACPTR;
    RETURN ACPTR;
    };

void ARP_UPDATE( IPADDR , XE_Int , HWLEN , HWADDR ) =

// Update an ARP cache entry, creating a new one if none exists.
// N.B. Should be called either at AST level or with AST's disabled

    {
    MAP
	struct XE_Interface_Structure * XE_Int;
    signed long
	struct ACACHE_BLK * ACPTR,
	HSHVAL;
    if ((ACPTR = ARP_FIND(IPADDR,XE_Int,HWLEN)) == 0)
	ACPTR = ARP_CNEW(IPADDR,XE_Int,HWLEN)
    else
	{
	ACPTR->AC$EXPIRE = Time_Stamp()+ARP_EXP_TIME;
	ACPTR->AC$RFTIME = 0;
	};

// Check ACPTR validity

    if (ACPTR == -1)
	{
	RETURN ;
	} ;

// Update the hardware address and mark this as valid

    CH$MOVE(HWLEN,CH$PTR(HWADDR),CH$PTR(ACPTR->AC$HWADDR));
    ACPTR->AC$VALID = TRUE;

// Retransmit the saved packet, if we have one

//    if ($$LOGF(LOG$ARP))
!	DRV$OPR_FAO("Saved QB=!XL!",ACPTR->AC$SAVEQB);
    if (ACPTR->AC$SAVEQB != 0)
	{
	BIND
	    dev_config = XE_Int->XEI$dev_config : Device_Configuration_Entry;
	signed long
	    QB;

	QB = ACPTR->AC$SAVEQB;
	ACPTR->AC$SAVEQB = 0;
	XE_Int = ACPTR->AC$DEVICE;
	INSQUE(QB,DEV_CONFIG[DC_S}_QTAIL]);
	XE$Xmit(dev_config);
	};
    };

ARP_SWEEP : NOVALUE =

// Sweep the ARP cache, deleting expired entries.
// Called periodically (about every 2 minutes).
// Must run at AST level or with AST's disabled.

    {
    signed long
	struct ACACHE_BLK * ACPTR,
	APREV,
	CTIME;

    CTIME = Time_Stamp();
    if ($$LOGF(LOG$ARP))
	DRV$QL_FAO("!%T ARP_SWEEP running!/",0);
    DECR I FROM (ARP_HSHLEN-1) TO 0 DO
	{

// Start at hash bucket. NB: APREV points at pointer to update on unlink

	ACPTR = ARPHTB[I];
	APREV = ARPHTB[I];
	WHILE ACPTR != 0 DO
	    {
	    if (ACPTR->AC$EXPIRE LSS CTIME)
		{

		// Unlink the cache entry from the hash chain

		.APREV = ACPTR->AC$NEXT;

		// First, delete any saved QB and packet

		if (ACPTR->AC$SAVEQB != 0)
		    {
		    signed long
			struct BLOCK * QB[] FIELD (QB_Net_Send);

		    QB = ACPTR->AC$SAVEQB;
		    DRV$Seg_Free(QB->NSQ$Del_Buf_Size,QB->NSQ$Del_Buf);
		    DRV$QBLK_Free(QB);
		    };

		// Free up the cache entry's memory.

!		LIB$FREE_VM(%REF(ACACHE_LEN*4),ACPTR);
		LIB$FREE_VM_PAGE(%REF(((ACACHE_LEN * 4) / 512) + 1), ACPTR);
		ACPTR = ..APREV;
		}
	    else
		{		// Advance to next cache entry
		APREV = ACPTR->AC$NEXT;
		ACPTR = ACPTR->AC$NEXT;
		};
	    };
	};

// Reset sweep timer

    $SETIMR(DAYTIM = ARP_SWP_TIME,
	    ASTADR = ARP_SWEEP);    
    };

//SBTTL "ARP_DUMP - Dump ARP cache"
/*
    Dump a portion of the ARP cache into a block for return to the user.
    ACIDX is the ARP cache "index", RBLOCK is the address of the block,
    RBSIZE is the size of the block.
    Returns: -1 on error, or size of block returned.
*/

XE$ARP_DUMP(ACIDX,RBLOCK,RBSIZE)
    {
    LABEL
	X;
    signed long
	LRSIZE,
	CIDX,
	HTIDX,
	struct ACACHE_BLK * ACPTR,
	NOW;

// Scan through the ARP hash table

    NOW = Time_Stamp();
    LRSIZE = RBSIZE;
    CIDX = 0;
    DRV$NOINT;			// Don't allow anything to change
X:  {			// Labelled block X
    INCR HTIDX FROM 0 TO ARP_HSHLEN-1 DO
	{
	ACPTR = ARPHTB[HTIDX];
	WHILE ACPTR != 0 DO
	    {
	    if (ACIDX > 0)
		// If we haven't gotten to the one he wants yet, advance
		ACIDX = ACIDX-1
	    else
		// Else, copy the info about this entry to his block
		{
		MAP
		    struct D$ARP_Dump_Return_Blk * RBLOCK();
		signed long
		    ETIME;
		ETIME = ACPTR->AC$EXPIRE - NOW;
		if (ETIME LSS 0)
		    ETIME = 0;
		RBLOCK->DU$ARP_INDEX = CIDX;

		RBLOCK->DU$ARP_DEVICE = ACPTR->AC$DEVICE;
		RBLOCK->DU$ARP_IPADDR = ACPTR->AC$IPADDR;
		RBLOCK->DU$ARP_EXPIRE = ETIME;
		RBLOCK->DU$ARP_SAVEQB = ACPTR->AC$SAVEQB;
		RBLOCK->DU$ARP_FLAGS = ACPTR->AC$FLAGS;
		RBLOCK->DU$ARP_HWSIZE = ACPTR->AC$HWSIZE;
		CH$MOVE(ACPTR->AC$HWSIZE,ACPTR->AC$HWADDR,
			RBLOCK->DU$ARP_HWADDR);
		RBLOCK = RBLOCK + D$ARP_Dump_Blksize;
		LRSIZE = LRSIZE - D$ARP_Dump_Blksize;

// Check for user block full

		if (LRSIZE LSS D$ARP_Dump_Blksize)
		    LEAVE X;
		};

// Advance to next ARP cache entry

	    ACPTR = ACPTR->AC$NEXT;
	    CIDX = CIDX + 1;
	    };
	};
    };			// Labelled block X

// Return length of block

    DRV$OKINT;
    RETURN RBSIZE - LRSIZE;
    };

}
ELUDOM
