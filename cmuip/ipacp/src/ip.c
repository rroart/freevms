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
//TITLE "InterNet Protocol Handler"
//SBTTL "InterNetwork Protocol Handler Overview."
/*

Module:

	IP

Facility:

	Inter-Network protocol handler.

Abstract:

	IP provides TCP with a potentially unreliable datagram delivery
	service.  IP handles the actual network datagram transmission,
	receipt plus inter-network store & forward functions.  In this
	implementation IP is called from TCP for segment transmission &
        IP$Receive is called from a device handler AST routine to handle
        datagram reception.

Author:

	Tim Fallon, Stan Smith
	This version by Vince Fuller, CMU-CSD, Spring/Summer, 1986
	Copyright (c) 1986, 1987, Vince Fuller and Carnegie-Mellon University

Modification History:

     03-Jan-89, Edit bu DHP
        Further mods to allow recognition of several different broadcast
	addresses (as per suggestion of VAF).....all 255"s, all 0"s, and a
	0 instead of the 255 in the "host" field.

     03-Jan-89, Edit bu DHP
	Add patches by Charles Lane (lane@duphy4.drexel.edu) to allow
	reading of broadcasts

*** Begin CMU change log ***

4.5c	18-Jul-1991	Henry W. Miller		USBR
	Use LIB$GET_VM_PAGE and LIB$FREE_VM_PAGE rather then LIB$GET_VM
	and LIB$FREE_VM.

4.5b	25-Mar-1991,	Henry W. Miller		USBR
	Fixed many dot derefs for IP_group_MIB[].

4.5a	13-Jan-1991,	Henry W. Miller		USBR
	Make IPTTL a configurable variable.
	In IP_LOG(), print TTL of header.

4.5  18-Oct-90, Edit by Bruce R. Miller, CMU NetDev
	Fixed dot deref.  (IP_group_MIB->IPMIB$ipForwDatagrams)

4.4  21-Sep-90, Edit by Bruce R. Miller, CMU NetDev
	Modifications from Mark Berryman, SAIC.COM
	Added code for proxy ARPs and restored clone device code.

4.3  06-Feb-90, Edit by Bruce R. Miller, CMU NetDev
	Changed device configuration interface.
	IP_RECEIVE now takes device config entry as last argument.

     17-Nov-89, Edit by Bruce R. Miller, CMU NetDev
	Moved all of the ICMP code into ICMP.BLI.

4.2  20-Nov-87, Edit by VAF
	Change IP$S} to return zero on error (no route) and nonzero on
	success. Also, deallocate packets that can't be sent in this manner.
	Use new $ACPWAKE macro instead of testing "sleeping" and doing $WAKE.
	Flush a few unneeded externals. Log IP errors under (new) LOG$IPERR as
	well as under LOG$IP.

4.1  10-Jun-87, Edit by VAF
	First cut at adding ICMP Echo support. Needs work.

4.0  23-Mar-87, Edit by VAF
	Know how to receive IP fragments. Don't yet know how to handle sending
	them because we don't expect higher levels to generate them.
	Miscellaneous cleanup.

3.9  24-Feb-87, Edit by VAF
	Move QL_FAO and friends into IOUTIL module. Flush "net message queue"
	stuff - we don't keep IP datagrams around, we just send them to the
	device drivers immediately.

3.8  10-Dec-86, Edit by VAF
	Remove Gen_Checksum - it is now in the macro library. Remove first
	parameter to Calc_Checksum.

3.7   3-Nov-86, Edit by VAF
	Before dispatching to ICMP$Input, log the IP header if ICMP logging
	is enabled but IP logging is disabled. This is so we can trace who
	ICMP packets are coming from without having to see all of the IP
	packets.

3.6  12-Sep-86, Edit by VAF
	Know about cloned devices in IP_ROUTE.
	In IP$S}, don't override IP$SRC if it has been specified. It is
	necessary to allow the higher levels to specify the source IP address
	in order to properly support multi-homing.

3.5  12-Aug-86, Edit by VAF
	Put SET_HOSTS here where it belongs (IP$SET_HOSTS).

3.4   8-Aug-86, Edit by VAF
	Remove gateway structure from STRUCTURE.REQ. Initialize gateway
	entry in this module in new routine IP$Gwy_Config.

3.3  29-Jul-86, Edit by VAF
	Move the Gen_Checksum routine into this module for UDP's use.

3.2  22-Jul-86, Edit by VAF
	Do ICMP logging based on LOG$ICMP now.

3.1  22-JUL-86, Edit by Dale Moore
	Change to not preallocate the FAO string with STR$GET1_DX
	and fill in with $FAO.  Instead we use LIB$SYS_FAOL.  The
	old way we weren't freeing the whole string that we had
	allocated.

3.0  20-Jun-86, Edit by VAF
	When logging IP sends, indicate first-hop destination when different
	from final destination.

2.9  19-Jun-86, Edit by VAF
	IP_RECEIVE now takes device index as additional argument.
	Put forwarding code in under control of ip_forward_ok global.
	Fix forwarding code so it won't send something out the same
	interface that it was received on.

2.8  29-Apr-86, Edit by VAF
	Issue $WAKEs here on IP receive, not in device drivers.

2.7  21-Apr-86, Edit by VAF
	Phase II of flushing XPORT - use $FAO for formatting output.

2.6  19-Apr-86, Edit by VAF
	Redo ICMP hashing stuff in same style as ARP, Host table

2.5  18-Apr-86, Edit by VAF
	Implement IP addresses as byte strings, not swapped longwords.

2.4  17-Apr-86, Edit by VAF
	Add dynamic-string function to Q_MESSAGE
	Change IP$Log to be a real routine.

2.3   7-Apr-86, Edit by VAF
	Only log incoming IP packets that are for the local host.
	Know about new logging flags.

2.2  26-Mar-86, Edit by VAF
	More ICMP work.
	Add loopback capability at IP level.

2.1  24-Mar-86, Edit by VAF
	Start adding ICMP code, more flexible IP_ROUTE stuff.
	Add hooks for UDP input.
	Finish flushing ARP code from here.
	Various changes since 2.0.

2.0  20-Feb-86, Edit by VAF
	Start upgrade toward "real" Internet implementation:
	    Flush "known hosts".
	    First pass at implementing IP routing.
	    Flush ARP code from here - it will go elsewhere.
	    Pass first-hop IP address to device driver - let it figure out
	    what the physical address really is.
	Don't forward packets not directed at this host. If VMS is to act as a
	packet router/gateway, more general code needs to be written than what
	exists here.

*** End CMU change log ***

	1.0 - Original version.
        1.1 - Fix buffer delete bug in hc_driver
	1.2 - Prevent AST level error logging if the log file is not active.
        1.3 - Separated IP code and device handler code into separate modules.
        1.4 - Changed to handle buffer from higher protocol that allows space
              to include the IP header right in front of the data without
              having to do a copy.
	1.5 - Changed to allow for multiple internet names for this host.
	1.51 - Added ARP code (Rick Watson)
*/


//SBTTL "Module Definition"

MODULE IP(IDENT="4.5c",LANGUAGE(BLISS32),
	  ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			  NONEXTERNAL=LONG_RELATIVE),
	  LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	  OPTIMIZE,OPTLEVEL=3,ZIP)=
{
#include "SYS$LIBRARY:STARLET";	// VMS system definitions
#include "CMUIP_SRC:[CENTRAL]NETXPORT";		// BLISS transportablity package
#include "CMUIP_SRC:[CENTRAL]NETCOMMON";	// CMU-OpenVMS/IP common decls
#include "CMUIP_SRC:[central]NETCONFIG"; 	// Tranport devices interface
#include "CMUIP_SRC:[CENTRAL]NETVMS";		// VMS specifics
#include "CMUIP_SRC:[CENTRAL]NetTCPIP";		// IP definitions
#include "STRUCTURE";		// TCB & Segment Structure definition
#include "TCPMACROS";		// Local macros
#include "SNMP";

extern
    TIME_STAMP,
 VOID    LOG_FAO,
 VOID    LOG_OUTPUT,
 VOID    OPR_FAO,
    LIB$GET_VM : ADDRESSING_MODE(GENERAL),
    LIB$GET_VM_PAGE : ADDRESSING_MODE(GENERAL),
    LIB$FREE_VM : ADDRESSING_MODE(GENERAL),
    LIB$FREE_VM_PAGE : ADDRESSING_MODE(GENERAL),
    LIB$SYS_FAOL: ADDRESSING_MODE(GENERAL),
    STR$FREE1_DX : ADDRESSING_MODE(GENERAL),

// IOUTIL.BLI

 VOID    ASCII_DEC_BYTES,
 VOID    QL_FAO,

// Maclib.mar

    Calc_checksum,
    Gen_Checksum,
 void    MovByt,
 void    SwapBytes,

// Memgr.bli

    MM$Seg_Get,
 void    MM$Seg_Free,
    MM$QBLK_Get,
 void    MM$QBLK_Free,

// SEGIN.BLI/TCP.BLI

 void    Seg$Input,

// UDP.BLI

 void    UDP$Input,

// ICMP.BLI
    ICMP$Check,
 void    ICMP$Input;


// External data

extern signed long
    SegIN : Queue_Header_Structure(SI_Fields),
    Dev_config_Tab : Device_configuration_Table,
    Dev_count,			// Number of devices known
    Min_Physical_Bufsize,	// Size of "small" device buffers
    Max_Physical_Bufsize;	// Size of "large" device buffers


//SBTTL "Gateway table definition"

$Field GWY_Fields=
    SET
    GWY_Name		= [$BYTES(8)],		// Gateway name
    GWY_Address		= [$Long_Integer],	// Gateway IP address
    GWY_Network		= [$Long_Integer],	// IP network served
    GWY_Netmask		= [$Long_Integer],	// Mask for that network
    GWY_Status		= [$Byte]		// Status (nonzero is "up")
    TES;

LITERAL
    GWY_Size = $Field_Set_Size,
    Max_GWY = 10;	// Max number of gateways we can store

MACRO
    Gateway_Structure = BLOCKVECTOR[Max_GWY,GWY_Size] FIELD(GWY_Fields)%;


//SBTTL "IP Fragment reassembly queue blocks"

// The fragment reassembly queue contains partial IP datagrams which were
// fragmented by the network before reaching us. Items on the queue stay there
// until either the last fragment of the datagram has been seen, a bad fragment
// has been seen, or the block times-out.

$Field RA$DATA_FIELDS (void)
    SET
    RA$Next	= [$Address],	// Next block on queue
    RA$Prev	= [$Address],	// Previous block on queue
    RA$Source	= [$Bytes(4)],	// Source IP address
    RA$Dest	= [$Bytes(4)],	// Destination IP address
    RA$Ident	= [$UWord],	// IP identifier
    RA$Buf	= [$Address],	// Pointer to buffer (first fragment pkt buffer)
    RA$Bufsize	= [$Uword],	// Size of buffer
    RA$Data	= [$Address],	// Pointer to start of protocol data in buffer
    RA$Datend	= [$Address],	// Pointer to first free byte in buffer
    RA$Octet	= [$ULong],	// Fragment octet offset we are waiting for
    RA$Timeout	= [$ULong]	// Timer for how long to wait for fragments
    TES;
LITERAL
    RA$DATA_SIZE = $FIELD_SET_SIZE,
    RA$DATA_BLEN = RA$DATA_SIZE*4;
MACRO
    RA$DATA_BLOCK = BLOCK->RA$DATA_SIZE FIELD(RA$DATA_FIELDS) %;

//SBTTL "Data and definitions associated with IP and ICMP"

LITERAL
    RA_EXPIRE_TIME = 5;		// Reassembly-expire time - multiply by TTL*CSEC
BIND
    RA_CHECK_TIMESTR = %ASCID"0 00:00:30.00";

// Internet Protocol counters and states

signed long
    RETRY_COUNT  = 5,	// Number of time to retry an IP read.
				// Setable through the config file.
    IPTTL  = 32,	// Default time-to-live
				// Setable through the config file.
    Max_Gateways,
    IP_group_MIB : IP_group_MIB_struct;

static signed long
!!!HACK!!// Make this dynamic
    GWY_table: Gateway_structure, // space for list of known gateways
    GWY_count,			// Count of gateways
    RA_QUEUE : QUEUE_HEADER
	       PRESET([QHEAD] = RA_QUEUE,
		      [QTAIL] = RA_QUEUE),
    RA_CHECK_TIME : VECTOR->2;	// Quadword time value for checking RA queue

static signed long
    struct Gateway_structure * GWY_table_ptr INITIAL(GWY_table); // Known gateways


//SBTTL "IP$Gwy_Config - Add a gateway entry to the table"

IP$GWY_CONFIG(GWYNAME_A,GWYADDR,GWYNET,GWYNETMASK) : NOVALUE (void)
    {
    BIND
	GWYNAME	= GWYNAME_A		: $BBLOCK;
    EXTERNAL ROUTINE
	STR$COPY_DX	: BLISS ADDRESSING_MODE (GENERAL);
    signed long
	gwyidx,
	Status;

// Make sure there is room for this entry

    if ((GWY_COUNT GEQ MAX_GWY))
	{
	OPR$FAO("Too many gateways in INET$CONFIG - entry for !AS ignored",
		GWYNAME);
	RETURN;
	};
    GWYIDX = GWY_COUNT;
    GWY_COUNT = GWY_COUNT + 1;

!!!HACK!!// just do it!
// Allocate virtual string and copy it
//    GWY_table_ptr = GWY_table;

    {
    BIND
	Gateway = GWY_table_ptr[GWYIDX, GWY_Name]
		    : BLOCK->GWY_Size FIELD(GWY_Fields),
	Gateway_name = Gateway [GWY_Name] : $BBLOCK->8;

    Gateway_name->DSC$W_LENGTH = 0;
    Gateway_name->DSC$B_DTYPE = DSC$K_DTYPE_T;
    Gateway_name->DSC$B_CLASS = DSC$K_CLASS_D;
    Gateway_name->DSC$A_POINTER = 0;

    Status = STR$COPY_DX (GWY_Table [GWYIDX, GWY_NAME], GWYNAME);
    if (NOT Status) Signal (Status);

// Make the gateway table entry

    Gateway [GWY_ADDRESS] = GWYADDR;
    Gateway [GWY_NETWORK] = GWYNET;
    Gateway [GWY_NETMASK] = GWYNETMASK;
    Gateway [GWY_STATUS] = 1;
    }

    };

//SBTTL "IP_INIT - Initialize state of IP"

IP$INIT : NOVALUE (void)
!
// Initialize the IP reassembly queue and the time value for checking it.
!
    {
    signed long
	RC;

// Set the queue to empty.

    RA_QUEUE->QHEAD = RA_QUEUE->QTAIL = RA_QUEUE->QHEAD;

// Translate time string to quadword value

    RC = $BINTIM(TIMBUF = RA_CHECK_TIMESTR,
		 TIMADR = RA_CHECK_TIME);
    if (NOT RC)
	FATAL$FAO("$BINTIM failed for RA_CHECK_TIMSTR, RC = !XL",RC);

    // Initial the IP group MIB
//    IP_group_MIB->IPMIB$ipForwarding	= 0;	// Initialized by CONFIG.BLI
    IP_group_MIB->IPMIB$ipDefaultTTL	= IPTTL;
    IP_group_MIB->IPMIB$ipInReceives	= 0;
    IP_group_MIB->IPMIB$ipInHdrErrors	= 0;
    IP_group_MIB->IPMIB$ipInAddrErrors	= 0;
    IP_group_MIB->IPMIB$ipForwDatagrams	= 0;
    IP_group_MIB->IPMIB$ipInUnknownProtos= 0;
    IP_group_MIB->IPMIB$ipInDiscards	= 0;
    IP_group_MIB->IPMIB$ipInDelivers	= 0;
    IP_group_MIB->IPMIB$ipOutRequests	= 0;
    IP_group_MIB->IPMIB$ipOutDiscards	= 0;
    IP_group_MIB->IPMIB$ipOutNoRoutes	= 0;
    IP_group_MIB->IPMIB$ipReasmTimeout	= 255*RA_EXPIRE_TIME;	// MAXTTL=255
    IP_group_MIB->IPMIB$ipReasmReqds	= 0;
    IP_group_MIB->IPMIB$ipReasmOKs	= 0;
    IP_group_MIB->IPMIB$ipReasmFails	= 0;
    IP_group_MIB->IPMIB$ipFragOKs	= 0;
    IP_group_MIB->IPMIB$ipFragFails	= 0;
    IP_group_MIB->IPMIB$ipFragCreates	= 0;

    };

//SBTTL "IP$LOG - make a logging entry for an IP packet"

void IP$LOG(NAME,struct IP_Structure * IPHDR) (void)

// IP logging routine.
// Compose message from IP header contents, give to QL_FAO.
// Also called from ICMP.BLI

    {
    signed long
	DESC$STR_ALLOC(SRCSTR,20),
	DESC$STR_ALLOC(DSTSTR,20),
	DATAPTR;
    
// Build IP source and dest strings

    ASCII_DEC_BYTES(SRCSTR,4,IPHDR->IPH$SOURCE,SRCSTR->DSC$W_LENGTH);
    ASCII_DEC_BYTES(DSTSTR,4,IPHDR->IPH$DEST,DSTSTR->DSC$W_LENGTH);

// Queue up the message

    DATAPTR = IPHDR+IPHDR->IPH$IHL*4;
    QL$FAO(%STRING("!%T !AS: S=!AS,D=!AS,HL=!SL,PR=!SL,TL=!SL,ID=!SL,FL=!SL,FR=!SL,TTL=!SL!/",
		   "!_    !AS: HDR=!XL,DATA=!XL!/"),
	   0,NAME,SRCSTR,DSTSTR,IPHDR->IPH$IHL,IPHDR->IPH$PROTOCOL,
	   IPHDR->IPH$TOTAL_LENGTH,IPHDR->IPH$IDENT,IPHDR->IPH$FLAGS,
	   IPHDR->IPH$FRAGMENT_OFFSET,IPHDR->IPH$TTL,NAME,IPHDR,DATAPTR);
    };

//SBTTL "IP routing code"

//    Here is where all routing descisions for IP packet output are made.
//    The basic algorithm is:
!	IF destination is on the same network as one of our interfaces,
!	   return send directly - return device index for interface
!	else
!	    IF ICMP knows a route to that address, return device index for
!	       appropriate gateway address
!	    else
!		IF we know a gateway for that address, return device index
!		   for it
!		else
!		    Return device index for default gateway.

IP_FIND_DEV(IPADDR)

!Find interface for a destination IP address
!Returns:
//   -1	Failure, IP address is not on local network
//  >=0	Success, device index to use is returned

    {
    INCR IDX FROM 0 TO (DEV_COUNT-1) DO
	IF ((IPADDR && DEV_CONFIG_TAB[IDX,DC_IP_NETMASK]) EQL
	   DEV_CONFIG_TAB[IDX,DC_IP_NETWORK]) OR
	    (IPADDR == %X"FFFFFFFF") THEN
	    {
	    signed long
		temp;

	    // If this is a clone device return
	    !	number of device from which it was cloned.
	    temp = dev_config_tab[idx,dc_clone_dev];
	    if ((temp GEQ 0)) return temp;
	    return IDX;
	    };

    return -1;
    };

IP_FIND_GWY(IPADDR)

!Find gateway for a destination IP address.
!Search the ICMP database for an entry matching this IP address. If
!existant, then use the gateway that it returned. Otherwise, look in
!the gateway table for the first gateway which knows how to serve that
!network. Note that a default gateway should always exist with GWY_MASK
!and GWY_NET of 0, indicating service to all networks.
!Returns address of connected gateway or 0 if none defined (or all down)

    {
    INCR IDX FROM 0 TO (GWY_COUNT-1) DO
	IF (IPADDR && GWY_table_ptr[IDX,GWY_NETMASK]) EQL
	   GWY_table_ptr[IDX,GWY_NETWORK] THEN
	    if (GWY_table_ptr[IDX,GWY_STATUS] > 0)
		return GWY_table_ptr[IDX,GWY_ADDRESS];
    return 0;
    };

IP_ROUTE(IPDEST,IPSRC,NEWIPDEST,LEV)

!Obtain source and destination IP addresses for first hop given a
!destination IP address.
//   IPDEST	Pointer to destination IP address
!		(may be changed on broadcasts)
//   IPSRC	Pointer to place to put local IP source address
//   NEWIPDEST	Pointer to place to put first hop IP destination address
!Returns:
//   -1 on failure, no route known to that address
//  >=0 on success, with device index

    {
    signed long
	IDX,GWY;
    LITERAL
	MAX_LEV = 10;

// If this address is on same network, use it

    if ((IDX = IP_FIND_DEV(..IPDEST)) GEQ 0)
	{
	if ((..IPDEST == %X"FFFFFFFF"))
	    {
	    NEWIPDEST = (NOT DEV_CONFIG_TAB[IDX,DC_IP_NETMASK])
		OR DEV_CONFIG_TAB[IDX,DC_IP_NETWORK];
	    IPDEST = ..NEWIPDEST;
	    }
	else
	    NEWIPDEST = ..IPDEST;
	.IPSRC = DEV_CONFIG_TAB[IDX,DC_IP_ADDRESS];
	return IDX;
	};

// Make sure recursion depth limit not exceeded

    if (LEV > MAX_LEV)
	{
	signed long
	    DESC$STR_ALLOC(DSTSTR,20);
	ASCII_DEC_BYTES(DSTSTR,4,IPDEST,DSTSTR->DSC$W_LENGTH);
	XQL$FAO(LOG$IP+LOG$IPERR,
		"!%T IP_ROUTE max recursion depth exceeded, DEST=!AS!/",
		0,DSTSTR);
	return -1;
	};

// Check for ICMP information, and try again

    if ((GWY = ICMP$Check(..IPDEST)) NEQU 0)
	return IP_ROUTE(GWY,IPSRC,NEWIPDEST,LEV+1);

// Check for gateway table and try again.

    if ((GWY = IP_FIND_GWY(..IPDEST)) != 0)
	return IP_ROUTE(GWY,IPSRC,NEWIPDEST,LEV+1);

// None of the above - no route exists.

    return -1;
    };

IP$ISME(IPADDR, STRICT)

!Determine if an IP address refers to this system
!Returns:
//   -1 - not a local address
//  >=0 - address is local, device index is returned

    {

// 127.x.x.x is a loopback address
    if (IPADDR<0,8,0> == 127) return 0;

    INCR IDX FROM 0 TO (DEV_COUNT-1) DO
	{
// Check for exact address match
	if (IPADDR == DEV_CONFIG_TAB[IDX,DC_IP_ADDRESS])
	    return IDX;
// Check for a wildcard match (only if strict was passed as false (0))
	if (STRICT == 0)
	 IF ((((IPADDR && DEV_CONFIG_TAB[IDX,DC_IP_NETMASK]) EQL
	    DEV_CONFIG_TAB[IDX,DC_IP_NETWORK]) AND
	    ((IPADDR || DEV_CONFIG_TAB[IDX, DC_IP_NETMASK]) == -1)) OR
	   (IPADDR == -1) OR
	   (IPADDR == 0) OR
	   (IPADDR == DEV_CONFIG_TAB[IDX,DC_IP_NETWORK]))
	    THEN
	     return IDX;
	};

    // Check for proxy ARP
    if (STRICT > 1)	// ARP passes address, not boolean flag
	if (IP_group_MIB->IPMIB$ipForwarding == 1) // IP forwarding on?
	    {
	    signed long
		temp;

	    temp = IP_Find_Dev (IPADDR);
	    IF (temp GEQ 0) AND
		(Strict != Dev_Config_Tab[temp,dc_dev_interface]) THEN
		return temp;		// yes, make sure it's not
	    };			// device thaqt rcvd the ARP rqst.
    return -1;
    };

signed long BIND ROUTINE
    IP_ISLOCAL = IP_FIND_DEV;

IP$SET_HOSTS(ADRCNT,ADRLST,LCLPTR,FRNPTR) : NOVALUE (void)
!
// Set local/foreign hosts pair given list of foreign addresses.
!
    {
    MAP
	struct VECTOR * ADRLST;	// Assume 32-bit IP addr
    signed long
	LIDX,
	FIDX;

// Look through the list of foriegn host addresses to see if any of
// them are on the same network as one of our addresses. If so, we will use
// the matching pair, otherwise, we use the default (first) pair.

    LIDX = 0;
    FIDX = 0;
    INCR I FROM 0 TO (ADRCNT-1) DO
	{
	signed long
	    J;
	if ((J = IP_ISLOCAL(ADRLST[I])) GEQ 0)
	    {
	    FIDX = I;
	    LIDX = J;
	    EXITLOOP;
	    };
	};
    FRNPTR = ADRLST[FIDX];
    LCLPTR = DEV_CONFIG_TAB[LIDX,DC_IP_ADDRESS];
    };

//SBTTL "IP$S}_RAW:  Send TCP segment to IP for transmission."
/******************************************************************************

Function:

	This routine is called by upper-level protocols to provide the
	InterNetwork protocol service. Basically a Datagram service.
	In the _RAW version of this function, the client provides the
	entire IP segment, including the header.

Inputs:

	Seg - Segment pointer.
	SegSize - Byte length of segment.
        Delete_Seg - Delete the segment after transmission if true
        Buf - Address of buffer containing segment, used to free buffer
        BufSize - Length of Buf

Outputs:

	Nonzero on success, 0 on failure (no route)

Side Effects:

	Protocol data is packaged in IP protocols & transmitted over the
	network.  The only exception to this is if the destination address
	is the same as the source address (eg, local host communication).
	In this case, the packet is looped-back via IP$RECEIVE.

*******************************************************************************
*/

FORWARD ROUTINE
 VOID    IP$RECEIVE;

IP$S}_RAW(IP$Dest,Seg,SegSize,Delete_Seg,Buf,Bufsize)
    {
    MAP
	struct segment_Structure * SEG;
    signed long
        struct Queue_blk_structure * QB(QB_net_send),
	ip_src,
	newip_dest,
        dev;

// Keep track of requests
    IP_group_MIB->IPMIB$ipOutRequests =
	IP_group_MIB->IPMIB$ipOutRequests + 1;


// Find interface and addresses for routing this packet.
// If no route, then flush the packet and return failure.

    !!!HACK!!// IP$ISME takes way too long
    if ((IP$ISME(IP$dest, TRUE) GEQ 0))
	{
	dev = -1;		// Loopback
	}
    else
	{
	signed long
	    newip_src;
	if ((dev = IP_ROUTE(IP$Dest,newip_src,newip_dest,0)) LSS 0)
	    {
	    signed long
		DESC$STR_ALLOC(dststr,20);
	    ASCII_DEC_BYTES(dststr,4,IP$Dest,dststr->DSC$W_LENGTH);
	    XQL$FAO(LOG$IP+LOG$IPERR,
		    "!%T IP send failure - no route to !AS!/",
		    0,dststr);
	    IP_group_MIB->IPMIB$ipOutNoRoutes = 
			IP_group_MIB->IPMIB$ipOutNoRoutes + 1;
	    if (Delete_Seg != 0)
		MM$Seg_Free(Bufsize,Buf);
	    return 0;		// No route exists
	    };
	};
!*********************************
// Send the segment to the network
!*********************************

// Fill in the IP header fields

    if ($$LOGF(LOG$IP))
	{
	IP$LOG(%ASCID"IPSend",Seg);
	if (IP$Dest != newip_dest)
	    {
	    signed long
		DESC$STR_ALLOC(dststr,20);
	    ASCII_DEC_BYTES(dststr,4,newip_dest,dststr->DSC$W_LENGTH);
	    QL$FAO("!%T IPsend: route is !AS!/",0,dststr);
	    };
!	QL$FAO("!%T IPsend: dev index=",1,dev,0)
	};

    if (dev LSS 0)
	// Packet is for local host - use loopback

	if (delete_seg)	// If deleting, then no copy needed
	    IP$Receive(buf,bufsize,Seg,SegSize,0)
	else
	    {		// e wants the seg - need to copy it, then
	    signed long
		buf2,
		seg2 : REF segment_Structure;

	    buf2 = MM$Seg_Get(bufsize);
	    seg2 = seg-.buf+.buf2;
	    CH$MOVE(bufsize,buf,buf2);
	    IP$Receive(buf2,bufsize,Seg2,SegSize,0)
	    }
    else
	{
	signed long
	    struct Device_Configuration_Entry * dev_config;

	dev_config = Dev_config_tab[dev,dc_begin];

// Build a Net_send_q entry for the device handler

	QB = MM$QBLK_get();
	QB->NSQ$Driver = dev_config;
	QB->NSQ$Data = Seg;
	QB->NSQ$Datasize = Segsize;
	QB->NSQ$IP_Dest = newip_dest;
	QB->NSQ$Retry = 0;
	QB->NSQ$Del_buf = Buf;
	QB->NSQ$Del_buf_size = BufSize;
	QB->NSQ$Delete = Delete_Seg;

// Insert this entry into the appropriate Net_Send_Q

	INSQUE(QB,Dev_config->dc_send_Qtail);

// Call appropriate device driver
	(Dev_config->dc_rtn_Xmit)(dev_config);
	};	// Give success return

    return -1;
    };



//SBTTL "IP$S}:  Send TCP segment to IP for transmission."
/******************************************************************************

Function:

	This routine is called by upper-level protocols to provide the
	InterNetwork protocol service. Basically a Datagram service.

Inputs:

	IP$Src - Source Address
	IP$Dest - Destination address (Network & host) = 32-bits.
	Service - Type of IP service required.
	Life - # of internet hops before segment is dead.
	Seg - Segment pointer.
	SegSize - Byte length of segment.
	ID - Identifier code.
	Frag - Don"t Fragment boolean, True = Don"t fragment.
        Delete_Seg - Delete the segment after transmission if true
        Protocol - Protocol number
        Buf - Address of buffer containing segment, used to free buffer
        BufSize - Length of Buf

Outputs:

	Nonzero on success, 0 on failure (no route)

Side Effects:

	Protocol data is packaged in IP protocols & transmitted over the
	network.  The only exception to this is if the destination address
	is the same as the source address (eg, local host communication).
	In this case, the packet is looped-back via IP$RECEIVE.

*******************************************************************************
*/

IP$S}(IP$Src,IP$Dest,Service,Life,Seg,SegSize,
		       ID,Frag,Delete_Seg,Protocol,Buf,Bufsize)
    {
    MAP
	struct segment_Structure * SEG;
    signed long
        struct IP_Structure * IPHDR,
	iplen,
        struct Queue_blk_structure * QB(QB_net_send),
	ip_src,
	newip_dest,
        dev;
    OWN
	fragmentation_data,
	fragmenting : LONG INITIAL(0);


// Keep track of requests
    IP_group_MIB->IPMIB$ipOutRequests =
	IP_group_MIB->IPMIB$ipOutRequests + 1;

// Check to see if fragmentation is necessary and, if so, fragment the
// packet and send it out.

    IF (Frag == 0) AND
	(SegSize > Opt$Max_Recv_Datasize + IP_hdr_byte_size) AND
       (NOT fragmenting) THEN
	{
	signed long
	    subbuff,
	    subseg,
	    frag_offset,
	    frag_size;

	fragmenting = 1;	// Let us know we are fragmenting the packet
	frag_offset = 0;

	DO
	    {
	    frag_size = MIN(Opt$Max_Recv_Datasize, SegSize - frag_offset);
	    subbuff = MM$Seg_Get(Max_Physical_Bufsize);
	    subseg = subbuff + device_header + IP_hdr_byte_size;
	    CH$MOVE(frag_size, Seg + frag_offset, subseg);
	    fragmentation_data = (frag_offset / 8);
	    if ((frag_offset + frag_size) LSS SegSize)
		fragmentation_data = fragmentation_data + %X"2000"; !more frags
	    IP$S}(IP$Src, IP$Dest, Service, Life, subseg, frag_size,
	            ID, Frag, 1, Protocol, subbuff, Max_Physical_Bufsize);
	    frag_offset = frag_offset + frag_size;
	    }
	WHILE frag_offset LSS SegSize;

	fragmenting = 0;	// All done fragmenting
	if (Delete_Seg != 0)
	    MM$Seg_Free(Bufsize,Buf);		// Get rid of the wasted space
	}
    else
	{

// Find interface and addresses for routing this packet.
// If no route, then flush the packet and return failure.

    IP_Src = IP$Src;
    if ((IP$ISME(IP$dest, TRUE) GEQ 0))
	{
	newip_dest = IP$dest;
	dev = -1;		// Loopback
	}
    else
	{
	signed long
	    newip_src;
	if ((dev = IP_ROUTE(IP$Dest,newip_src,newip_dest,0)) LSS 0)
	    {
	    signed long
		DESC$STR_ALLOC(dststr,20);
	    ASCII_DEC_BYTES(dststr,4,IP$Dest,dststr->DSC$W_LENGTH);
	    XQL$FAO(LOG$IP+LOG$IPERR,
		    "!%T IP send failure - no route to !AS!/",
		    0,dststr);
	    IP_group_MIB->IPMIB$ipOutNoRoutes = 
			IP_group_MIB->IPMIB$ipOutNoRoutes + 1;
	    if (Delete_Seg != 0)
		MM$Seg_Free(Bufsize,Buf);
	    return 0;		// No route exists
	    };
	if (IP_Src == 0)
	    IP_Src = newip_src;
	};

// Regenerate the UDP checksum just in case...
    IF Protocol == UDP_Protocol Then
	{
	MAP struct UDPkt_Structure * Seg;
	Seg->UP$Checksum = 0;
	Seg->UP$Checksum = Gen_Checksum(Segsize, Seg, IP_Src, IP$Dest,
					UDP_Protocol);
	};

!*********************************
// Send the segment to the network
!*********************************

// Position buffer for IP header

    IPHDR = Seg - IP_hdr_byte_size;
    iplen = IP_hdr_byte_size + segsize;
!!!HACK!!// Check to see if IPHDR < Buf 

// Fill in the IP header fields

    IPHDR->IPH$Version = IP_Version;
    IPHDR->IPH$IHL = IP_hdr_wd_size;
    IPHDR->IPH$Type_service = Service;
    IPHDR->IPH$Total_length = iplen;
    IPHDR->IPH$Ident = ID;
    if (NOT fragmenting)
	{
	IPHDR->IPH$Flags = 0;
	IPHDR->IPH$Fragment_offset = 0;
	}
    else
	IPHDR->IPH$Fragmentation_Data = fragmentation_data;
    IPHDR->IPH$TTL = Life;
    IPHDR->IPH$Protocol = Protocol;
    IPHDR->IPH$Checksum = 0;
    IPHDR->IPH$Source = IP_Src;
    IPHDR->IPH$Dest = IP$Dest;

    if ($$LOGF(LOG$IP))
	{
	IP$LOG(%ASCID"IPSend",IPHDR);
	if (IP$Dest != newip_dest)
	    {
	    signed long
		DESC$STR_ALLOC(dststr,20);
	    ASCII_DEC_BYTES(dststr,4,newip_dest,dststr->DSC$W_LENGTH);
	    QL$FAO("!%T IPsend: route is !AS!/",0,dststr);
	    };
!	QL$FAO("!%T IPsend: dev index=",1,dev,0)
	};

// Re-arrange bytes and words in IP header

    SwapBytes(IP_hdr_swap_size,IPHDR);

// Compute checksum for IP header

    IPHDR->IPH$Checksum = Calc_Checksum(IP_hdr_byte_size,IPHDR);

    if (dev LSS 0)
	{

// Packet is for local host - use loopback

	if (delete_seg)	// If deleting, then no copy needed
	    IP$Receive(buf,bufsize,iphdr,iplen,0)
	else
	    {		// e wants the seg - need to copy it, then
	    signed long
		iphdr2,
		buf2,
		seg2 : REF segment_Structure;
	    buf2 = MM$Seg_Get(bufsize);
	    seg2 = seg-.buf+.buf2;
	    CH$MOVE(bufsize,buf,buf2);
	    iphdr2 = iphdr-.seg+.seg2;
	    IP$Receive(buf2,bufsize,iphdr2,iplen,0)
	    };
	}
    else
	{
!	BIND
!	    dev_config = Dev_config_tab[dev,dc_begin] : Device_Configuration_Entry;
	signed long
	    struct Device_Configuration_Entry * dev_config;

	dev_config = Dev_config_tab[dev,dc_begin];

// Build a Net_send_q entry for the device handler

	QB = MM$QBLK_get();
	QB->NSQ$Driver = dev_config;
	QB->NSQ$Data = IPHDR;
	QB->NSQ$Datasize = IP_hdr_byte_size + Segsize;
	QB->NSQ$IP_Dest = newip_dest;
	QB->NSQ$Retry = 0;
	QB->NSQ$Del_buf = Buf;
	QB->NSQ$Del_buf_size = BufSize;
	QB->NSQ$Delete = Delete_Seg;

// Insert this entry into the appropriate Net_Send_Q

	INSQUE(QB,Dev_config->dc_send_Qtail);

// Call appropriate device driver
	(Dev_config->dc_rtn_Xmit)(dev_config);
	};// Give success return

    };
    return -1;
    };



//SBTTL "IP$RECEIVE - Receive an IP datagram"
/******************************************************************************

Function:

!!!HACK!!// These comments are wrong.
    This routine is called by the link level device receive handler
    for an incomming network datagram.  The checksum for the
    IP header is checked and the datagram is routed to either the higher
    level protocol in the local host or is discarded.

Inputs:

    Buf - Base address of buffer containing the actual Hdr_addr and Seg_addr
    Buf_size - Byte size of Buf
    Hdr_addr - Address of IP header
    Hdr_length - length  of IP header and segment if included
    Seg_addr - Address of data segment
    Seg_length - Length of data segment, zero if included with header

Outputs:

    Dispatches packet to appropriate protocol handler.

*******************************************************************************
*/

FORWARD ROUTINE
 VOID    IP_FRAGMENT,
 VOID    IP_DISPATCH;

void IP$Receive (Buf,Buf_size,IPHdr,devlen,dev_config) (void)
    {
    EXTERNAL
	IPCB_Count;
    EXTERNAL ROUTINE
	IPU$User_Input;
    MAP
	struct Device_Configuration_Entry * dev_config,
	struct IP_Structure * Iphdr;
    signed long
	struct Queue_blk_structure * QB(QB_net_send),
 	Sum,
	hdrlen,
	IP_src,
	newip_dest,
	IPlen,
	Dev,
	I;

// Keep count of received datagrams.

    IP_group_MIB->IPMIB$ipInReceives = IP_group_MIB->IPMIB$ipInReceives + 1;

// Compute and verify checksum of IP header

    hdrlen = IPhdr->IPH$swap_IHL * 4;
    Sum = Calc_checksum(hdrlen,IPhdr);
    if (Sum NEQU %X"FFFF")
	{			// Checksum error
	IP_group_MIB->IPMIB$ipInHdrErrors =	// Ooops// another error...
		IP_group_MIB->IPMIB$ipInHdrErrors + 1;
	if ($$LOGF(LOG$IP+LOG$IPERR))
	    {
	    QL$FAO("!%T IP Receive checksum error, sum=!XL!/",0,Sum);
	    Swapbytes(IP_hdr_swap_size,Iphdr);
	    IP$LOG(%ASCID"(IPrecv)",IPHDR);
	    };
        MM$Seg_Free(Buf_size,Buf);
	RETURN;
	};

// Do byteswap of word fields in IP header (not including options or addresses).

    Swapbytes(IP_hdr_swap_size,Iphdr);

// Make sure datagram length computed from "Total Length" field of IP header
// is consistent with length actually transferred from device.

    IPlen = IPhdr->IPH$Total_Length;
    if (IPlen > devlen)
	{			// inconsistent length - drop segment
	IP_group_MIB->IPMIB$ipInHdrErrors =	// Ooops// another error...
		IP_group_MIB->IPMIB$ipInHdrErrors + 1;
	if ($$LOGF(LOG$IP+LOG$IPERR))
	    {
	    XQL$FAO(LOG$IP+LOG$IPERR,
		    "!%T IP discarded: device length=!SL, IP length=!SL!/",
		    0,devlen,IPlen);
	    if (NOT $$LOGF(LOG$IP))
		IP$LOG(%ASCID"(IPRecv)",IPHDR);
	    };
	MM$Seg_Free(Buf_size,Buf);
	RETURN;
	};

// Check to see if any one wants to peek at IP packets.

    if (IPCB_Count > 0)
	IPU$User_Input (Iphdr->IPH$Dest , Iphdr->IPH$Source ,
			Iphdr->IPH$Protocol ,
			.Buf_size , Buf ,
			.IPlen , IPHdr );

// Check if segment destination is local host

    !!!HACK!!// IP$ISME too slow?
    if (IP$ISME(Iphdr->IPH$Dest, FALSE) GEQ 0)
	{
	if ($$LOGF(LOG$IP))
	    IP$LOG(%ASCID"IPrecv",IPHDR);

// If this packet is a fragment, then hand it to the reassembly code.

	if (IPHdr->IPH$MF || (IPHdr->IPH$Fragment_Offset != 0))
	    IP_FRAGMENT(IPHdr,IPlen,hdrlen,Buf,Buf_Size)
	else
	    IP_DISPATCH(IPHdr,IPLen,hdrlen,Buf,Buf_Size);
	}
    else
	if (IP_group_MIB->IPMIB$ipForwarding != 1)
	    {		// Drop the packet - not allowed to forward
	    IP_group_MIB->IPMIB$ipInAddrErrors =
		IP_group_MIB->IPMIB$ipInAddrErrors + 1;
	    MM$Seg_Free(Buf_size,Buf)
	    }
	else
	    {		// Try to forward the packet

// Count up how many times this was done

	    IP_group_MIB->IPMIB$ipForwDatagrams = 
		IP_group_MIB->IPMIB$ipForwDatagrams + 1;

// Now handle Store and Forward - look up destination of next hop

	    dev = IP_ROUTE(Iphdr->IPH$Dest,ip_src,newip_dest,0);
	    if (dev LSS 0)
		{
		IP_group_MIB->IPMIB$ipOutNoRoutes = 
			IP_group_MIB->IPMIB$ipOutNoRoutes + 1;
		IP_group_MIB->IPMIB$ipInAddrErrors =
			IP_group_MIB->IPMIB$ipInAddrErrors + 1;
		if ($$LOGF(LOG$IP+LOG$IPERR))
		    {
		    QL$FAO("!%T IPfwd: Cannot route IP destination!/",0);
		    if (NOT $$LOGF(LOG$IP))
			IP$LOG(%ASCID"IPfwd",IPHDR);
		    };
		MM$Seg_Free(Buf_size,Buf);
		RETURN;
		};

// Don't forward a packet back out through the same interface

	    if (dev_config_tab[dev,dc_begin] == dev_config)
		{
		IP_group_MIB->IPMIB$ipInAddrErrors =
			IP_group_MIB->IPMIB$ipInAddrErrors + 1;
		if ($$LOGF(LOG$IP+LOG$IPERR))
		    {
		    QL$FAO("!%T struct used * IPfwd to forward to device !SL!/",
			   0,dev);
		    if (NOT $$LOGF(LOG$IP))
			IP$LOG(%ASCID"IPfwd",IPHDR);
		    };
		MM$Seg_Free(Buf_Size,Buf);
		RETURN;
		};

// Decrement time-to-live and check for expiration

	    Iphdr->IPH$TTL = Iphdr->IPH$TTL - 1;
	    if (Iphdr->IPH$TTL == 0)
		{
		if ($$LOGF(LOG$IP))
		    {
		    QL$FAO("!%T IPfwd: packet TTL expired!/",0);
		    if (NOT $$LOGF(LOG$IP))
			IP$LOG(%ASCID"IPfwd",IPHDR);
		    };
		MM$Seg_Free(Buf_size,Buf);
		RETURN;
		};

// Adjust checksum for decremented lifetime

	    if ((Iphdr->IPH$Checksum && %X"FF00') == %X'FF00")
		// Wrap around checksum overflow bit
		Iphdr->IPH$Checksum = (Iphdr->IPH$Checksum && %X"FF") + 1
	    else
		// Just add one to upper half of field
		Iphdr->IPH$Checksum = Iphdr->IPH$Checksum + %X"100";

// Swap bytes and words in IP header

	    Swapbytes(IP_hdr_swap_size,Iphdr);

// Put this datagram in net send queue

	    dev_config = Dev_config_tab[dev,dc_begin];

	    QB = MM$QBLK_get();
	    QB->NSQ$Driver = dev_config;
	    QB->NSQ$Data = IPhdr;
	    QB->NSQ$Datasize = IPlen;
	    QB->NSQ$IP_Dest = newip_dest;
	    QB->NSQ$Retry = 0;
	    QB->NSQ$Delete = TRUE;
	    QB->NSQ$Del_buf = Buf;
	    QB->NSQ$Del_buf_size = Buf_size;

// Put on the send queue for the device and call device send routine.

	    INSQUE(QB,Dev_config->dc_send_Qtail);
	    (Dev_config->dc_rtn_xmit)(dev_config);
	    };
    };

//SBTTL "Dispatch IP packet to protocol routine"

IP_DISPATCH(IPHDR,IPLEN,HDRLEN,BUF,BUFSIZE) : NOVALUE (void)
!
// Hand a complete protocol segment off to it's handling routine.
// Called from both IP$RECEIVE when it receives a complete packet and from
// IP_FRAGMENT when it receives the last fragment for a segment.
!
    {
    MAP
	struct IP_Structure * IPHDR;
    signed long
	Unknown_flag  = 0,
	SEG,
	SEGSIZE;

// Calculate pointer & size of data

    SEG = IPHDR + HDRLEN;
    SEGSIZE = IPLEN - HDRLEN;

// Dispatch according to protocol type

    SELECTONE IPHDR->IPH$Protocol OF
    SET
    [ICMP_Protocol]:
	{
	if ($$LOGF(LOG$ICMP) && (NOT $$LOGF(LOG$IP)))
	    IP$LOG(%ASCID"ICMRecv",IPHDR);
	ICMP$Input (SEG,SEGSIZE,IPHDR,IPLEN,BUFSIZE,BUF);
	};

    [TCP_protocol]:
	Seg$Input(IPHDR->IPH$Source,IPHDR->IPH$Dest,BUFSIZE,BUF,
		  SEGSIZE,SEG);

    [UDP_Protocol]:
	UDP$Input(IPHDR->IPH$Source,IPHDR->IPH$Dest,BUFSIZE,BUF,
		  SEGSIZE,SEG);

    [OTHERWISE]:	// Unknown protocol
	{
	IP_group_MIB->IPMIB$ipInUnknownProtos =
		IP_group_MIB->IPMIB$ipInUnknownProtos + 1;
	Unknown_Flag = 1;
	MM$Seg_Free(BUFSIZE,BUF);
	};
    TES;

    // Successfully delivered?
    if (Unknown_Flag) IP_group_MIB->IPMIB$ipInDelivers =
				IP_group_MIB->IPMIB$ipInDelivers + 1;

// Wake up mainline procedure if sleeping

    $ACPWAKE;
    };

//SBTTL "Handle reception of IP packet fragment"

FORWARD ROUTINE
 VOID    IP_FRAGMENT_CHECK;

IP_FRAGMENT(IPHDR,IPLEN,HDRLEN,BUF,BUFSIZE) : NOVALUE (void)
!
// Match the fragment with appropriate entry on the fragment reassembly queue.
// If last fragment, then recursively call IP$RECEIVE to re-handle the packet.
!
    {
    MAP
	struct IP_Structure * IPHDR;
    signed long
	RC,
	struct RA$DATA_BLOCK * RAPTR,
	IPDATA,
	OFFSET,
	DATLEN,
	MAXDAT,
	FIRST,
	COPY_MF;
    LABEL
	X,Y;

// Keep count of fragments
    IP_group_MIB->IPMIB$ipReasmReqds = 
		IP_group_MIB->IPMIB$ipReasmReqds + 1;

// See if we can find this fragment's header on the queue

X:  {			// *** Block X ***
    RAPTR = RA_QUEUE->QHEAD;
    WHILE RAPTR != RA_QUEUE->QHEAD DO
	{
	IF (RAPTR->RA$Source == IPHDR->IPH$Source) AND
	   (RAPTR->RA$Dest == IPHDR->IPH$Dest) AND
	   (RAPTR->RA$Ident == IPHDR->IPH$Ident) THEN
	    LEAVE X;
	RAPTR = RAPTR->RA$Next;
	};
    RAPTR = 0;
    };			// *** Block X ***
	
// Handle the fragment according to the case.

    SELECTONE TRUE OF
    SET

// First fragment case. Flush old fragment and add this one.

    [IPHDR->IPH$MF && (IPHDR->IPH$Fragment_Offset == 0)]:
	{
	if (RAPTR != 0)
	    {

// RA data already exists - just flush the old buffer.

	    MM$Seg_Free(RAPTR->RA$Bufsize,RAPTR->RA$Buf);
	    }
	else
	    {

// RA data doesn't exist yet. Allocate it & fill in the IP parameters.

!	    RC = LIB$GET_VM(%REF(RA$Data_BLEN),RAPTR);
	    RC = LIB$GET_VM_PAGE(%REF((RA$Data_BLEN / 512) + 1),RAPTR);
	    if (NOT RC)
		FATAL$FAO("IP_FRAGMENT - LIB$GET_VM failure, RC=!XL",RC);
	    RAPTR->RA$Ident = IPHDR->IPH$Ident;
	    RAPTR->RA$Source = IPHDR->IPH$Source;
	    RAPTR->RA$Dest = IPHDR->IPH$Dest;
	    RAPTR->RA$Timeout = Time_Stamp()+IPHDR->IPH$TTL*RA_EXPIRE_TIME*CSEC;
	    XQL$FAO(LOG$IP,"!%T New reassambly block !XL, Timeout=!UL!/",
		    0,RAPTR,RAPTR->RA$Timeout);

// Insert on the queue and start RA purge timer, if first entry on queue.

	    FIRST = QUEUE_EMPTY(RA_QUEUE);
	    INSQUE(RAPTR,RA_QUEUE->QTAIL);
	    if (FIRST)
		$SETIMR(DAYTIM = RA_CHECK_TIME,
			ASTADR = IP_FRAGMENT_CHECK);
	    };
	RAPTR->RA$BUF = MM$Seg_Get(16384);
	RAPTR->RA$Bufsize = 16384;
	RAPTR->RA$Data = RAPTR->RA$BUF;
	RAPTR->RA$Datend = CH$MOVE(IPLEN,IPHDR,RAPTR->RA$DATA);
	RAPTR->RA$Octet = IPLEN - HDRLEN;
	XQL$FAO(LOG$IP,
		"!%T Fragment at !XL for RA !XL, IPLEN=!SL, next octet=!SL!/",
		0,IPHDR,RAPTR,IPLEN,RAPTR->RA$Octet);
	MM$Seg_Free(BUFSIZE,BUF);
	RETURN;
	};

// Not first fragment. Check for validity of this fragment & append its data.

    [(IPHDR->IPH$Fragment_Offset != 0) && (RAPTR != 0)]:
Y:	{

// Check for match of fragment octet offset

	OFFSET = IPHDR->IPH$Fragment_Offset*8;
	if (OFFSET != RAPTR->RA$OCTET)
	    LEAVE Y;

// Fragment matches. Verify that we can store it or punt.

	DATLEN = IPLEN - HDRLEN;
	MAXDAT = RAPTR->RA$Bufsize - (RAPTR->RA$Datend-RAPTR->RA$Buf);
	if (DATLEN > MAXDAT)
	    {
	    signed long
		NewBUF, NewBUFsize;

// Current buffer is too small.  Double its size, copy the data from the
// old buffer into the new one, and update the two other pointers into
// the innards of the data.

	    NewBUFsize = RAPTR->RA$Bufsize * 2;
	    NewBUF = MM$Seg_Get(NewBUFsize);
	    CH$MOVE(RAPTR->RA$Bufsize, RAPTR->RA$BUF, NewBUF);
	    RAPTR->RA$Data = RAPTR->RA$Data + (NewBUF - RAPTR->RA$BUF);
	    RAPTR->RA$Datend = RAPTR->RA$Datend + (NewBUF - RAPTR->RA$BUF);
	    MM$Seg_Free(RAPTR->RA$Bufsize, RAPTR->RA$BUF);
	    RAPTR->RA$Bufsize = NewBUFsize;
	    RAPTR->RA$BUF = NewBUF;
	    };

// Append the data from this fragment to our current buffer.

	XQL$FAO(LOG$IP,"!%T Append fragment at !XL to RA !XL, LEN=!SL!/",
		0,IPHDR,RAPTR,DATLEN);
	IPDATA = IPHDR + HDRLEN;
	RAPTR[RA$DAT}] = CH$MOVE(DATLEN,IPDATA,RAPTR[RA$DAT}]);
	RAPTR->RA$OCTET = RAPTR->RA$OCTET + DATLEN;

// Copy the MF bit to preserve it from Seg_Free

	COPY_MF = IPHDR->IPH$MF;

// Flush the buffer

	MM$Seg_Free(BUFSIZE,BUF);

// If this is the last fragment, then dispatch the datagram.

	if (NOT COPY_MF)
	    {

// Remove from the queue

	    REMQUE(RAPTR,RAPTR);

// Calculate sizes of entire, reassembled packet & dispatch it.

	    IPHDR = RAPTR->RA$DATA;
	    HDRLEN = IPHDR->IPH$IHL*4;
	    IPLEN = RAPTR[RA$DAT}]-RAPTR->RA$DATA;
	    XQL$FAO(LOG$IP,"!%T Last fragment received, IPLEN=!SL, DLEN=!SL!/",
		    0,IPLEN,IPLEN-.HDRLEN);
	    IP_group_MIB->IPMIB$ipReasmOKs = 
		IP_group_MIB->IPMIB$ipReasmOKs + 1;
	    IP_DISPATCH(IPHDR,IPLEN,HDRLEN,RAPTR->RA$BUF,
			RAPTR->RA$BUFSIZE);

// Deallocate the queue block.

!	    LIB$FREE_VM(%REF(RA$Data_BLEN),RAPTR);
	    LIB$FREE_VM_PAGE(%REF((RA$Data_BLEN / 512) + 1),RAPTR);
	    };
	RETURN;
	};
    TES;

// If we got here, it means the fragment was unacceptable for some reason.
// Log this fact, release the buffer, and ignore the fragment.

    IP_group_MIB->IPMIB$ipReasmFails = 
		IP_group_MIB->IPMIB$ipReasmFails + 1;
    if ($$LOGF(LOG$IP+LOG$IPERR))
	{
	QL$FAO("!%T IP Fragment unusable!/",0);
	if (NOT $$LOGF(LOG$IP))
	    IP$LOG(%ASCID"IPfrag",IPHDR);
	};
    MM$Seg_Free(BUFSIZE,BUF);
    };

IP_FRAGMENT_CHECK : NOVALUE (void)
!
// Routine to periodically check the reassembly queue, purging any entries
// which have expired.
!
    {
    signed long
	NOW,
	struct RA$DATA_BLOCK * RAPTR,
	RANXT;

// Scan the entire queue, looking for expired entries.

    NOW = Time_Stamp();
    RAPTR = RA_QUEUE->QHEAD;
    WHILE RAPTR != RA_QUEUE->QHEAD DO
	{
	RANXT = RAPTR->RA$Next;

// If this entry has expired, then flush it.

	if (RAPTR->RA$Timeout LSS NOW)
	    {

// Flush the buffer & free the block

	    XQL$FAO(LOG$IP,"!%T Flushing expired IP RA block !XL!/",0,RAPTR);
	    REMQUE(RAPTR,RAPTR);
	    MM$Seg_Free(RAPTR->RA$Bufsize,RAPTR->RA$Buf);
!	    LIB$FREE_VM(%REF(RA$Data_BLEN),RAPTR);
	    LIB$FREE_VM_PAGE(%REF((RA$Data_BLEN / 512) + 1),RAPTR);
	    IP_group_MIB->IPMIB$ipReasmFails = 
			IP_group_MIB->IPMIB$ipReasmFails + 1;

	    };

// Check the next block

	RAPTR = RANXT;
	};

// If there are still entries on the reassembly queue, then requeue us

    if (NOT QUEUE_EMPTY(RA_QUEUE))
	$SETIMR(DAYTIM = RA_CHECK_TIME,
		ASTADR = IP_FRAGMENT_CHECK);
    };
}
ELUDOM
