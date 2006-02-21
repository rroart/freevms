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
	Change IP$SEND to return zero on error (no route) and nonzero on
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
	In IP$SEND, don't override IP$SRC if it has been specified. It is
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

#if 0
MODULE IP(IDENT="4.5c",LANGUAGE(BLISS32),
	  ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			  NONEXTERNAL=LONG_RELATIVE),
	  LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	  OPTIMIZE,OPTLEVEL=3,ZIP)=
#endif

#include <starlet.h>	// VMS system definitions
// not yet #include "CMUIP_SRC:[CENTRAL]NETXPORT";		// BLISS transportablity package
#include <cmuip/central/include/netcommon.h>	// CMU-OpenVMS/IP common decls
#include <cmuip/central/include/netconfig.h> 	// Tranport devices interface
#include <cmuip/central/include/nettcpip.h>		// IP definitions
#include "netvms.h"		// VMS specifics
#include "structure.h"		// TCB & Segment Structure definition
#include "tcpmacros.h"		// Local macros
#include "snmp.h"

#include <ssdef.h>
#include <descrip.h>

#ifndef NOKERNEL
#define sys$setimr exe$setimr
#endif

#undef TCP_DATA_OFFSET
#include <net/checksum.h>
#define Calc_Checksum(x,y) ip_compute_csum(y,x)
#define Gen_Checksum(a,b,c,d,e) csum_tcpudp_magic(c,d,a,e,csum_partial(b,a,0))

extern     TIME_STAMP();
extern  void    LOG_FAO();
extern  void    LOG_OUTPUT();
#if 0
extern  void    OPR_FAO();
#endif
extern   LIB$GET_VM ();
extern   LIB$GET_VM_PAGE ();
extern   LIB$FREE_VM();
extern   LIB$FREE_VM_PAGE();
extern   LIB$SYS_FAOL();
extern   STR$FREE1_DX ();

// IOUTIL.BLI

extern  void    ASCII_DEC_BYTES();
extern  void    QL_FAO();

// Maclib.mar

extern     Calc_checksum();
//extern     Gen_Checksum();
extern  void    MovByt();
extern  void    swapbytes();

// Memgr.bli

extern     mm$seg_get();
extern  void    mm$seg_free();
extern     mm$qblk_get();
extern  void    mm$qblk_free();

// SEGIN.BLI/TCP.BLI

extern  void    seg$input();

// UDP.BLI

extern  void    udp$input();

// ICMP.BLI
extern     icmp$check();
 void    icmp$input();


// External data

extern   struct queue_header_structure(si_fields)  SegIN;
extern   Device_Configuration_Entry  dev_config_tab[];

extern signed long
    dev_count,			// Number of devices known
    min_physical_bufsize,	// Size of "small" device buffers
    max_physical_bufsize;	// Size of "large" device buffers

extern signed long log_state;


//SBTTL "Gateway table definition"

struct gateway_structure
{
unsigned char    gwy_name[8];		// Gateway name
long int     gwy_address		;	// Gateway IP address
long int     gwy_network		;	// IP network served
long int     gwy_netmask		;	// Mask for that network
char    gwy_status;		// Status (nonzero is "up")
    };

#define MAX_GWY 10
#if 0
LITERAL
    GWY_Size = $Field_Set_Size,
    Max_GWY = 10;	// Max number of gateways we can store

MACRO
    Gateway_Structure = BLOCKVECTOR[Max_GWY,GWY_Size] FIELD(GWY_Fields)%;
#endif


//SBTTL "IP Fragment reassembly queue blocks"

// The fragment reassembly queue contains partial IP datagrams which were
// fragmented by the network before reaching us. Items on the queue stay there
// until either the last fragment of the datagram has been seen, a bad fragment
// has been seen, or the block times-out.

struct RA$DATA_BLOCK
{
void *     ra$next	;	// Next block on queue
void *     ra$prev	;	// Previous block on queue
unsigned long     ra$source;	// Source IP address
unsigned long     ra$dest;	// Destination IP address
unsigned short     ra$ident	;	// IP identifier
void *     ra$buf	;	// Pointer to buffer (first fragment pkt buffer)
unsigned short     ra$bufsize	;	// Size of buffer
void *     ra$data	;	// Pointer to start of protocol data in buffer
void *     ra$datend	;	// Pointer to first free byte in buffer
unsigned long     ra$octet	;	// Fragment octet offset we are waiting for
unsigned long     ra$timeout	;// Timer for how long to wait for fragments
    };

#define    RA$DATA_SIZE sizeof(struct RA$DATA_BLOCK)
#define    RA$DATA_BLEN RA$DATA_SIZE*4

#if 0
MACRO
    RA$DATA_BLOCK = BLOCK->RA$DATA_SIZE FIELD(RA$DATA_FIELDS) %;
#endif

//SBTTL "Data and definitions associated with IP and ICMP"

#define    RA_EXPIRE_TIME 5		// Reassembly-expire time - multiply by TTL*CSEC

struct dsc$descriptor    RA_CHECK_TIMESTR = ASCID2(13,"0 00:00:30.00");

// Internet Protocol counters and states

signed long
    retry_count  = 5,	// Number of time to retry an IP read.
				// Setable through the config file.
    ipttl  = 32,	// Default time-to-live
				// Setable through the config file.
  max_gateways;
    struct IP_group_MIB_struct IP_group_MIB_, * IP_group_MIB = &IP_group_MIB_;

static   struct gateway_structure  gwy_table[MAX_GWY]; // space for list of known gateways
static signed long
//!!HACK!!// Make this dynamic
    gwy_count;			// Count of gateways
static  struct queue_header_fields   RA_QUEUE_ = {
	       qhead: &RA_QUEUE_,
	       qtail: &RA_QUEUE_}, *RA_QUEUE=&RA_QUEUE_;
static    RA_CHECK_TIME[2];	// Quadword time value for checking RA queue

    struct gateway_structure * gwy_table_ptr = gwy_table; // Known gateways


//SBTTL "IP$Gwy_Config - Add a gateway entry to the table"

void ip$gwy_config(GWYNAME_A,GWYADDR,GWYNET,GWYNETMASK)
    {
    long
      GWYNAME	= GWYNAME_A;
extern      STR$COPY_DX();
    signed long
	gwyidx,
	Status;

// Make sure there is room for this entry

    if ((gwy_count >= MAX_GWY))
	{
	OPR$FAO("Too many gateways in INET$CONFIG - entry for !AS ignored",
		GWYNAME);
	return;
	};
    gwyidx = gwy_count;
    gwy_count = gwy_count + 1;

//!!HACK!!// just do it!
// Allocate virtual string and copy it
//    GWY_table_ptr = GWY_table;

    {
      struct gateway_structure * Gateway = gwy_table_ptr[gwyidx].gwy_name;
    struct dsc$descriptor * Gateway_name = Gateway ->gwy_name ;

    Gateway_name->dsc$w_length = 0;
    Gateway_name->dsc$b_dtype = DSC$K_DTYPE_T;
    Gateway_name->dsc$b_class = DSC$K_CLASS_D;
    Gateway_name->dsc$a_pointer = 0;

    Status = STR$COPY_DX (gwy_table [gwyidx]. gwy_name, GWYNAME);
    if (! Status) Signal (Status);

// Make the gateway table entry

    Gateway ->gwy_address = GWYADDR;
    Gateway ->gwy_network = GWYNET;
    Gateway ->gwy_netmask = GWYNETMASK;
    Gateway ->gwy_status = 1;
    }

    }

//SBTTL "IP_INIT - Initialize state of IP"

void ip$init  (void)
//
// Initialize the IP reassembly queue and the time value for checking it.
//
    {
    signed long
	RC;

// Set the queue to empty.

    RA_QUEUE->qhead = RA_QUEUE->qtail = &RA_QUEUE->qhead;

// Translate time string to quadword value

    RC = sys$bintim( &RA_CHECK_TIMESTR,&RA_CHECK_TIME);
    if (BLISSIFNOT(RC))
	FATAL$FAO("$BINTIM failed for RA_CHECK_TIMSTR, RC = !XL",RC);

    // Initial the IP group MIB
//    IP_group_MIB->IPMIB$ipForwarding	= 0;	// Initialized by CONFIG.BLI
    IP_group_MIB->IPMIB$ipDefaultTTL	= ipttl;
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

    }

//SBTTL "IP$LOG - make a logging entry for an IP packet"

void ip$log(long NAME,struct ip_structure * IPHDR)

// IP logging routine.
// Compose message from IP header contents, give to QL_FAO.
// Also called from ICMP.BLI

    {
DESC$STR_ALLOC(SRCSTR,20);
DESC$STR_ALLOC(DSTSTR,20);
    signed long
	DATAPTR;
    
// Build IP source and dest strings

    ASCII_DEC_BYTES(SRCSTR,4,IPHDR->iph$source,&SRCSTR->dsc$w_length);
    ASCII_DEC_BYTES(DSTSTR,4,IPHDR->iph$dest,&DSTSTR->dsc$w_length);

// Queue up the message

    DATAPTR = IPHDR+IPHDR->iph$ihl*4;
    QL$FAO("!%T !AS: S=!AS,D=!AS,HL=!SL,PR=!SL,TL=!SL,ID=!SL,FL=!SL,FR=!SL,TTL=!SL!/!_    !AS: HDR=!XL,DATA=!XL!/",
	   0,NAME,SRCSTR,DSTSTR,IPHDR->iph$ihl,IPHDR->iph$protocol,
	   IPHDR->iph$total_length,IPHDR->iph$ident,IPHDR->iph$fragment_offset>>13,
	   IPHDR->iph$fragment_offset,IPHDR->iph$ttl,NAME,IPHDR,DATAPTR);
    }

//SBTTL "IP routing code"

//    Here is where all routing descisions for IP packet output are made.
//    The basic algorithm is:
//	IF destination is on the same network as one of our interfaces,
//	   return send directly - return device index for interface
//	else
//	    IF ICMP knows a route to that address, return device index for
//	       appropriate gateway address
//	    else
//		IF we know a gateway for that address, return device index
//		   for it
//		else
//		    Return device index for default gateway.

ip_find_dev(IPADDR)

//Find interface for a destination IP address
//Returns:
//   -1	Failure, IP address is not on local network
//  >=0	Success, device index to use is returned
    {
      signed long IDX;
    for (IDX=0;IDX<=(dev_count-1);IDX++)
	if (((IPADDR & dev_config_tab[IDX].dc_ip_netmask) ==
	   dev_config_tab[IDX].dc_ip_network) ||
	    (IPADDR == 0xFFFFFFFF))
	    {
	    signed long
		temp;

	    // If this is a clone device return
	    //	number of device from which it was cloned.
	    temp = dev_config_tab[IDX].dc_clone_dev;
	    if ((temp >= 0)) return temp;
	    return IDX;
	    };

    return -1;
    }

IP_FIND_GWY(IPADDR)

//Find gateway for a destination IP address.
//Search the ICMP database for an entry matching this IP address. If
//existant, then use the gateway that it returned. Otherwise, look in
//the gateway table for the first gateway which knows how to serve that
//network. Note that a default gateway should always exist with GWY_MASK
//and GWY_NET of 0, indicating service to all networks.
//Returns address of connected gateway or 0 if none defined (or all down)

    {
      long IDX;
    for (IDX=0;IDX<=(gwy_count-1);IDX++)
	if ((IPADDR & gwy_table_ptr[IDX].gwy_netmask) ==
	   gwy_table_ptr[IDX].gwy_network)
	    if (gwy_table_ptr[IDX].gwy_status > 0)
		return gwy_table_ptr[IDX].gwy_address;
    return 0;
    }

IP_ROUTE(IPDEST,IPSRC,NEWIPDEST,LEV)

//Obtain source and destination IP addresses for first hop given a
//destination IP address.
//   IPDEST	Pointer to destination IP address
//		(may be changed on broadcasts)
//   IPSRC	Pointer to place to put local IP source address
//   NEWIPDEST	Pointer to place to put first hop IP destination address
//Returns:
//   -1 on failure, no route known to that address
//  >=0 on success, with device index

     long * IPDEST, * IPSRC, * NEWIPDEST; // check
    {
    signed long
	IDX,GWY;
#define	MAX_LEV 10

// If this address is on same network, use it

    if ((IDX = ip_find_dev(*IPDEST)) >= 0)
	{
	if ((*IPDEST == 0xFFFFFFFF))
	    {
	    *NEWIPDEST = (~ dev_config_tab[IDX].dc_ip_netmask)
		| dev_config_tab[IDX].dc_ip_network;
	    *IPDEST = *NEWIPDEST;
	    }
	else
	    *NEWIPDEST = *IPDEST;
	*IPSRC = dev_config_tab[IDX].dc_ip_address;
	return IDX;
	};

// Make sure recursion depth limit not exceeded

    if (LEV > MAX_LEV)
	{
	    DESC$STR_ALLOC(DSTSTR,20);
	ASCII_DEC_BYTES(DSTSTR,4,*IPDEST,&DSTSTR->dsc$w_length);
	XQL$FAO(LOG$IP+LOG$IPERR,
		"!%T IP_ROUTE max recursion depth exceeded, DEST=!AS!/",
		0,DSTSTR);
	return -1;
	};

// Check for ICMP information, and try again

    if ((GWY = icmp$check(*IPDEST)) != 0)
	return IP_ROUTE(&GWY,IPSRC,NEWIPDEST,LEV+1);

// Check for gateway table and try again.

    if ((GWY = IP_FIND_GWY(*IPDEST)) != 0)
	return IP_ROUTE(&GWY,IPSRC,NEWIPDEST,LEV+1);

// None of the above - no route exists.

    return -1;
    }

ip$isme(IPADDR, STRICT)

//Determine if an IP address refers to this system
//Returns:
//   -1 - not a local address
//  >=0 - address is local, device index is returned

    {
long IDX;
// 127.x.x.x is a loopback address
    if ((IPADDR&0x7f000000) == 127) return 0;

    for (IDX=0;IDX<=(dev_count-1);IDX++)
	{
// Check for exact address match
	if (IPADDR == dev_config_tab[IDX].dc_ip_address)
	    return IDX;
// Check for a wildcard match (only if strict was passed as false (0))
	if (STRICT == 0)
	 if (((((IPADDR & dev_config_tab[IDX].dc_ip_netmask) ==
	    dev_config_tab[IDX].dc_ip_network) &&
	    ((IPADDR | dev_config_tab[IDX].dc_ip_netmask) == -1)) ||
	   (IPADDR == -1) ||
	   (IPADDR == 0) ||
	      (IPADDR == dev_config_tab[IDX].dc_ip_network)))
	   return IDX;
	};

    // Check for proxy ARP
    if (STRICT > 1)	// ARP passes address, not boolean flag
	if (IP_group_MIB->IPMIB$ipForwarding == 1) // IP forwarding on?
	    {
	    signed long
		temp;

	    temp = ip_find_dev (IPADDR);
	    if ((temp >= 0) &&
		(STRICT != dev_config_tab[temp].dc_dev_interface))
		return temp;		// yes, make sure it's not
	    };			// device thaqt rcvd the ARP rqst.
    return -1;
    }

int (*    ip_islocal)(int) = ip_find_dev;

void IP$SET_HOSTS(ADRCNT,ADRLST,LCLPTR,FRNPTR)
//
// Set local/foreign hosts pair given list of foreign addresses.
//
	long * ADRLST;	// Assume 32-bit IP addr
long * LCLPTR, * FRNPTR;
    {
signed long I,
	LIDX,
	FIDX;

// Look through the list of foriegn host addresses to see if any of
// them are on the same network as one of our addresses. If so, we will use
// the matching pair, otherwise, we use the default (first) pair.

    LIDX = 0;
    FIDX = 0;
    for (I=0;I<=(ADRCNT-1);I++)
	{
	signed long
	    J;
	if ((J = ip_islocal(ADRLST[I])) >= 0)
	    {
	    FIDX = I;
	    LIDX = J;
	    break;
	    };
	};
    *FRNPTR = ADRLST[FIDX];
    *LCLPTR = dev_config_tab[LIDX].dc_ip_address;
    }

//SBTTL "IP$SEND_RAW:  Send TCP segment to IP for transmission."
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

 void    ip$receive();

ip$send_raw(IP$Dest,Seg,SegSize,Delete_Seg,Buf,Bufsize)
	struct segment_structure * Seg;
    {
      Net_Send_Queue_Element * QB;
    signed long
	ip_src,
	newip_dest,
        dev;

// Keep track of requests
    IP_group_MIB->IPMIB$ipOutRequests =
	IP_group_MIB->IPMIB$ipOutRequests + 1;


// Find interface and addresses for routing this packet.
// If no route, then flush the packet and return failure.

    //!!HACK!!// IP$ISME takes way too long
    if ((ip$isme(IP$Dest, TRUE) >= 0))
	{
	dev = -1;		// Loopback
	}
    else
	{
	signed long
	    newip_src;
	if ((dev = IP_ROUTE(&IP$Dest,&newip_src,&newip_dest,0)) < 0)
	    {
		DESC$STR_ALLOC(dststr,20);
	    ASCII_DEC_BYTES(dststr,4,IP$Dest,&dststr->dsc$w_length);
	    XQL$FAO(LOG$IP+LOG$IPERR,
		    "!%T IP send failure - no route to !AS!/",
		    0,dststr);
	    IP_group_MIB->IPMIB$ipOutNoRoutes = 
			IP_group_MIB->IPMIB$ipOutNoRoutes + 1;
	    if (Delete_Seg != 0)
		mm$seg_free(Bufsize,Buf);
	    return 0;		// No route exists
	    };
	};
//*********************************
// Send the segment to the network
//*********************************

// Fill in the IP header fields

    if ($$LOGF(LOG$IP))
	{
	  $DESCRIPTOR(dsc,"IPSend");
	ip$log(&dsc,Seg);
	if (IP$Dest != newip_dest)
	    {
		DESC$STR_ALLOC(dststr,20);
	    ASCII_DEC_BYTES(dststr,4,newip_dest,&dststr->dsc$w_length);
	    QL$FAO("!%T IPsend: route is !AS!/",0,dststr);
	    };
//	QL$FAO("!%T IPsend: dev index=",1,dev,0)
	};

    if (dev < 0)
	// Packet is for local host - use loopback

	if (Delete_Seg)	// If deleting, then no copy needed
	  ip$receive(Buf,Bufsize,Seg,SegSize,0);
	else
	    {		// e wants the seg - need to copy it, then
	    signed long
	      Buf2;
		struct segment_structure * Seg2;

	    Buf2 = mm$seg_get(Bufsize);
	    Seg2 = (long)Seg-Buf+Buf2;
	    CH$MOVE(Bufsize,Buf,Buf2);
	    ip$receive(Buf2,Bufsize,Seg2,SegSize,0);
	    }
    else
	{
	    Device_Configuration_Entry * dev_config;

	dev_config = dev_config_tab[dev].dc_begin;

// Build a Net_send_q entry for the device handler

	QB = mm$qblk_get();
	QB->NSQ$Driver = dev_config;
	QB->NSQ$Data = Seg;
	QB->NSQ$Datasize = SegSize;
	QB->NSQ$IP_Dest = newip_dest;
	QB->NSQ$Retry = 0;
	QB->NSQ$Del_Buf = Buf;
	QB->NSQ$Del_buf_size = Bufsize;
	QB->NSQ$Delete = Delete_Seg;

// Insert this entry into the appropriate Net_Send_Q

	INSQUE(QB,dev_config->dc_send_Qtail);

// Call appropriate device driver
	(dev_config->dc_rtn_Xmit)(dev_config);
	};	// Give success return

    return -1;
    }



//SBTTL "IP$SEND:  Send TCP segment to IP for transmission."
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

ip$send(IP$Src,IP$Dest,Service,Life,Seg,SegSize,
		       ID,Frag,Delete_Seg,Protocol,Buf,Bufsize)
 struct udpkt_structure * Seg;
    {
      Net_Send_Queue_Element * QB;
      struct ip_structure * IPHDR;
    signed long
	iplen,
	ip_src,
	newip_dest,
        dev;
    static
      fragmentation_data;
    static fragmenting =0;


// Keep track of requests
    IP_group_MIB->IPMIB$ipOutRequests =
	IP_group_MIB->IPMIB$ipOutRequests + 1;

// Check to see if fragmentation is necessary and, if so, fragment the
// packet and send it out.

    if ((Frag == 0) &&
	(SegSize > OPT$MAX_RECV_DATASIZE + IP_HDR_BYTE_SIZE) &&
       (! fragmenting))
	{
	signed long
	    subbuff,
	    subseg,
	    frag_offset,
	    frag_size;

	fragmenting = 1;	// Let us know we are fragmenting the packet
	frag_offset = 0;

	do
	    {
	    frag_size = MIN(OPT$MAX_RECV_DATASIZE, SegSize - frag_offset);
	    subbuff = mm$seg_get(max_physical_bufsize);
	    subseg = subbuff + DEVICE_HEADER + IP_HDR_BYTE_SIZE;
	    CH$MOVE(frag_size, Seg + frag_offset, subseg);
	    fragmentation_data = (frag_offset / 8);
	    if ((frag_offset + frag_size) < SegSize)
	      fragmentation_data = fragmentation_data + 0x2000; //more frags
	    ip$send(IP$Src, IP$Dest, Service, Life, subseg, frag_size,
	            ID, Frag, 1, Protocol, subbuff, max_physical_bufsize);
	    frag_offset = frag_offset + frag_size;
	    }
	while (frag_offset < SegSize);

	fragmenting = 0;	// All done fragmenting
	if (Delete_Seg != 0)
	    mm$seg_free(Bufsize,Buf);		// Get rid of the wasted space
	}
    else
	{

// Find interface and addresses for routing this packet.
// If no route, then flush the packet and return failure.

    ip_src = IP$Src;
    if ((ip$isme(IP$Dest, TRUE) >= 0))
	{
	newip_dest = IP$Dest;
	dev = -1;		// Loopback
	}
    else
	{
	signed long
	    newip_src;
	if ((dev = IP_ROUTE(&IP$Dest,&newip_src,&newip_dest,0)) < 0)
	    {
		DESC$STR_ALLOC(dststr,20);
	    ASCII_DEC_BYTES(dststr,4,IP$Dest,&dststr->dsc$w_length);
	    XQL$FAO(LOG$IP+LOG$IPERR,
		    "!%T IP send failure - no route to !AS!/",
		    0,dststr);
	    IP_group_MIB->IPMIB$ipOutNoRoutes = 
			IP_group_MIB->IPMIB$ipOutNoRoutes + 1;
	    if (Delete_Seg != 0)
		mm$seg_free(Bufsize,Buf);
	    return 0;		// No route exists
	    };
	if (ip_src == 0)
	    ip_src = newip_src;
	};

// Regenerate the UDP checksum just in case...
    if (Protocol == UDP_PROTOCOL)
	{
	Seg->up$checksum = 0;
	Seg->up$checksum = Gen_Checksum(SegSize, Seg, ip_src, IP$Dest,
					UDP_PROTOCOL);
	};

//*********************************
// Send the segment to the network
//*********************************

// Position buffer for IP header

    IPHDR = (long)Seg - IP_HDR_BYTE_SIZE;
    iplen = IP_HDR_BYTE_SIZE + SegSize;
//!!HACK!!// Check to see if IPHDR < Buf 

// Fill in the IP header fields

    IPHDR->iph$version = IP_VERSION;
    IPHDR->iph$ihl = IP_HDR_WD_SIZE;
    IPHDR->iph$type_service = Service;
    IPHDR->iph$total_length = iplen;
    IPHDR->iph$ident = ID;
    if (! fragmenting)
	{
	IPHDR->iph$fragmentation_data = 0;
	}
    else
	IPHDR->iph$fragmentation_data = fragmentation_data;
    IPHDR->iph$ttl = Life;
    IPHDR->iph$protocol = Protocol;
    IPHDR->iph$checksum = 0;
    IPHDR->iph$source = ip_src;
    IPHDR->iph$dest = IP$Dest;

    if ($$LOGF(LOG$IP))
	{
	  $DESCRIPTOR(dsc,"IPSend");
	ip$log(&dsc,IPHDR);
	if (IP$Dest != newip_dest)
	    {
		DESC$STR_ALLOC(dststr,20);
	    ASCII_DEC_BYTES(dststr,4,newip_dest,&dststr->dsc$w_length);
	    QL$FAO("!%T IPsend: route is !AS!/",0,dststr);
	    };
//	QL$FAO("!%T IPsend: dev index=",1,dev,0)
	};

// Re-arrange bytes and words in IP header

    swapbytesiphdr(IP_HDR_SWAP_SIZE,IPHDR);

// Compute checksum for IP header

    IPHDR->iph$checksum = Calc_Checksum(IP_HDR_BYTE_SIZE,IPHDR);

    if (dev < 0)
	{

// Packet is for local host - use loopback

	if (Delete_Seg)	// If deleting, then no copy needed
	  ip$receive(Buf,Bufsize,IPHDR,iplen,0);
	else
	    {		// e wants the seg - need to copy it, then
	    signed long
		iphdr2,
	      buf2;
	    struct segment_structure * seg2;
	    buf2 = mm$seg_get(Bufsize);
	    seg2 = (long)Seg-Buf+buf2;
	    CH$MOVE(Bufsize,Buf,buf2);
	    iphdr2 = (long)IPHDR-(long)Seg+(long)seg2;
	    ip$receive(buf2,Bufsize,iphdr2,iplen,0);
	    };
	}
    else
	{
//	BIND
//	    dev_config = dev_config_tab[dev].dc_begin : Device_Configuration_Entry;
	    Device_Configuration_Entry * dev_config;

	dev_config = dev_config_tab[dev].dc_begin;

// Build a Net_send_q entry for the device handler

	QB = mm$qblk_get();
	QB->NSQ$Driver = dev_config;
	QB->NSQ$Data = IPHDR;
	QB->NSQ$Datasize = IP_HDR_BYTE_SIZE + SegSize;
	QB->NSQ$IP_Dest = newip_dest;
	QB->NSQ$Retry = 0;
	QB->NSQ$Del_Buf = Buf;
	QB->NSQ$Del_buf_size = Bufsize;
	QB->NSQ$Delete = Delete_Seg;

// Insert this entry into the appropriate Net_Send_Q

	INSQUE(QB,dev_config->dc_send_Qtail);

// Call appropriate device driver
	(dev_config->dc_rtn_Xmit)(dev_config);
	};// Give success return

    }
    return -1;
    }



//SBTTL "IP$RECEIVE - Receive an IP datagram"
/******************************************************************************

Function:

//!!HACK!!// These comments are wrong.
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

void    IP_FRAGMENT();
 void    IP_DISPATCH();

void ip$receive (Buf,Buf_size,iphdr,devlen,dev_config)
     Device_Configuration_Entry * dev_config;
	struct ip_structure * iphdr;
    {
    extern
	IPCB_Count;
    extern	ipu$user_input();
    Net_Send_Queue_Element * QB;
    signed long
 	Sum,
	hdrlen,
	ip_src,
	newip_dest,
	iplen,
	dev,
	I;

// Keep count of received datagrams.

    IP_group_MIB->IPMIB$ipInReceives = IP_group_MIB->IPMIB$ipInReceives + 1;

// Compute and verify checksum of IP header

    hdrlen = iphdr->iph$ihl * 4; // was: swap_ihl
    Sum = ip_fast_csum(iphdr,iphdr->iph$ihl); // was: Calc_Checksum(hdrlen,iphdr);
    if (Sum != 0/* was: 0xFFFF*/)
	{			// Checksum error
	IP_group_MIB->IPMIB$ipInHdrErrors =	// Ooops// another error...
		IP_group_MIB->IPMIB$ipInHdrErrors + 1;
	if ($$LOGF(LOG$IP+LOG$IPERR))
	    {
	    QL$FAO("!%T IP Receive checksum error, sum=!XL!/",0,Sum);
	    swapbytesiphdr(IP_HDR_SWAP_SIZE,iphdr);
	    $DESCRIPTOR(dsc,"(IPrecv)");
	    ip$log(&dsc,iphdr);
	    };
        mm$seg_free(Buf_size,Buf);
	return;
	};

// Do byteswap of word fields in IP header (not including options or addresses).

    swapbytesiphdr(IP_HDR_SWAP_SIZE,iphdr);

// Make sure datagram length computed from "Total Length" field of IP header
// is consistent with length actually transferred from device.

    iplen = iphdr->iph$total_length;
    if (iplen > devlen)
	{			// inconsistent length - drop segment
	IP_group_MIB->IPMIB$ipInHdrErrors =	// Ooops// another error...
		IP_group_MIB->IPMIB$ipInHdrErrors + 1;
	if ($$LOGF(LOG$IP+LOG$IPERR))
	    {
	    XQL$FAO(LOG$IP+LOG$IPERR,
		    "!%T IP discarded: device length=!SL, IP length=!SL!/",
		    0,devlen,iplen);
	    if (! $$LOGF(LOG$IP)) {
	      $DESCRIPTOR(dsc,"(IPRecv)");
		ip$log(&dsc,iphdr);
	    }
	    };
	mm$seg_free(Buf_size,Buf);
	return;
	};

// Check to see if any one wants to peek at IP packets.

    if (IPCB_Count > 0)
	ipu$user_input (iphdr->iph$dest , iphdr->iph$source ,
			iphdr->iph$protocol ,
			Buf_size , Buf ,
			iplen , iphdr );

// Check if segment destination is local host

    //!!HACK!!// IP$ISME too slow?
    if (ip$isme(iphdr->iph$dest, FALSE) >= 0)
	{
	  if ($$LOGF(LOG$IP)) {
	    $DESCRIPTOR(dsc,"IPrecv");
	    ip$log(&dsc,iphdr);
	  }

// If this packet is a fragment, then hand it to the reassembly code.

	if ((iphdr->iph$fragmentation_data&0x80000000) || (iphdr->iph$fragment_offset != 0))
	  IP_FRAGMENT(iphdr,iplen,hdrlen,Buf,Buf_size);
	else
	    IP_DISPATCH(iphdr,iplen,hdrlen,Buf,Buf_size);
	}
    else
	if (IP_group_MIB->IPMIB$ipForwarding != 1)
	    {		// Drop the packet - not allowed to forward
	    IP_group_MIB->IPMIB$ipInAddrErrors =
		IP_group_MIB->IPMIB$ipInAddrErrors + 1;
	    mm$seg_free(Buf_size,Buf);
	    }
	else
	    {		// Try to forward the packet

// Count up how many times this was done

	    IP_group_MIB->IPMIB$ipForwDatagrams = 
		IP_group_MIB->IPMIB$ipForwDatagrams + 1;

// Now handle Store and Forward - look up destination of next hop

	    dev = IP_ROUTE(&iphdr->iph$dest,&ip_src,&newip_dest,0);
	    if (dev < 0)
		{
		IP_group_MIB->IPMIB$ipOutNoRoutes = 
			IP_group_MIB->IPMIB$ipOutNoRoutes + 1;
		IP_group_MIB->IPMIB$ipInAddrErrors =
			IP_group_MIB->IPMIB$ipInAddrErrors + 1;
		if ($$LOGF(LOG$IP+LOG$IPERR))
		    {
		    QL$FAO("!%T IPfwd: Cannot route IP destination!/",0);
		    if (! $$LOGF(LOG$IP)) {
		      $DESCRIPTOR(dsc,"IPfwd");
			ip$log(&dsc,iphdr);
		    }
		    };
		mm$seg_free(Buf_size,Buf);
		return;
		};

// Don't forward a packet back out through the same interface

	    if (dev_config_tab[dev].dc_begin == dev_config)
		{
		IP_group_MIB->IPMIB$ipInAddrErrors =
			IP_group_MIB->IPMIB$ipInAddrErrors + 1;
		if ($$LOGF(LOG$IP+LOG$IPERR))
		    {
		    QL$FAO("!%T struct used * IPfwd to forward to device !SL!/",
			   0,dev);
		    if (! $$LOGF(LOG$IP)) {
		      $DESCRIPTOR(dsc,"IPfwd");
			ip$log(&dsc,iphdr);
		    }
		    };
		mm$seg_free(Buf_size,Buf);
		return;
		};

// Decrement time-to-live and check for expiration

	    iphdr->iph$ttl = iphdr->iph$ttl - 1;
	    if (iphdr->iph$ttl == 0)
		{
		if ($$LOGF(LOG$IP))
		    {
		    QL$FAO("!%T IPfwd: packet TTL expired!/",0);
		    if (! $$LOGF(LOG$IP)) {
		      $DESCRIPTOR(dsc,"IPfwd");
			ip$log(&dsc,iphdr);
		    }
		    };
		mm$seg_free(Buf_size,Buf);
		return;
		};

// Adjust checksum for decremented lifetime

	    if ((iphdr->iph$checksum & 0xFF00) == 0xFF00)
		// Wrap around checksum overflow bit
	      iphdr->iph$checksum = (iphdr->iph$checksum & 0xFF) + 1;
	    else
		// Just add one to upper half of field
		iphdr->iph$checksum = iphdr->iph$checksum + 0x100;

// Swap bytes and words in IP header

	    swapbytesiphdr(IP_HDR_SWAP_SIZE,iphdr);

// Put this datagram in net send queue

	    dev_config = dev_config_tab[dev].dc_begin;

	    QB = mm$qblk_get();
	    QB->NSQ$Driver = dev_config;
	    QB->NSQ$Data = iphdr;
	    QB->NSQ$Datasize = iplen;
	    QB->NSQ$IP_Dest = newip_dest;
	    QB->NSQ$Retry = 0;
	    QB->NSQ$Delete = TRUE;
	    QB->NSQ$Del_Buf = Buf;
	    QB->NSQ$Del_buf_size = Buf_size;

// Put on the send queue for the device and call device send routine.

	    INSQUE(QB,dev_config->dc_send_Qtail);
	    (dev_config->dc_rtn_Xmit)(dev_config);
	    };
    }

//SBTTL "Dispatch IP packet to protocol routine"

void IP_DISPATCH(iphdr,iplen,HDRLEN,BUF,BUFSIZE)
//
// Hand a complete protocol segment off to it's handling routine.
// Called from both IP$RECEIVE when it receives a complete packet and from
// IP_FRAGMENT when it receives the last fragment for a segment.
//
	struct ip_structure * iphdr;
    {
    signed long
	unknown_flag  = 0,
	SEG,
	SEGSIZE;

// Calculate pointer & size of data

    SEG = (long)iphdr + HDRLEN;
    SEGSIZE = iplen - HDRLEN;

// Dispatch according to protocol type

    switch (iphdr->iph$protocol)
      {
    case ICMP_PROTOCOL:
	{
	  if ($$LOGF(LOG$ICMP) & (! $$LOGF(LOG$IP))) {
	    $DESCRIPTOR(dsc,"ICMRecv");
	    ip$log(&dsc,iphdr);
	  }
	icmp$input (SEG,SEGSIZE,iphdr,iplen,BUFSIZE,BUF);
	};
	break;

    case TCP_PROTOCOL:
	seg$input(iphdr->iph$source,iphdr->iph$dest,BUFSIZE,BUF,
		  SEGSIZE,SEG);
	break;

    case UDP_PROTOCOL:
	udp$input(iphdr->iph$source,iphdr->iph$dest,BUFSIZE,BUF,
		  SEGSIZE,SEG);
	break;

    default:	// Unknown protocol
	{
	IP_group_MIB->IPMIB$ipInUnknownProtos =
		IP_group_MIB->IPMIB$ipInUnknownProtos + 1;
	unknown_flag = 1;
	mm$seg_free(BUFSIZE,BUF);
	};
    };

    // Successfully delivered?
    if (unknown_flag) IP_group_MIB->IPMIB$ipInDelivers =
				IP_group_MIB->IPMIB$ipInDelivers + 1;

// Wake up mainline procedure if sleeping

    $ACPWAKE;
    }

//SBTTL "Handle reception of IP packet fragment"

 void    IP_FRAGMENT_CHECK();

void IP_FRAGMENT(iphdr,iplen,HDRLEN,BUF,BUFSIZE) 
//
// Match the fragment with appropriate entry on the fragment reassembly queue.
// If last fragment, then recursively call IP$RECEIVE to re-handle the packet.
//
	struct ip_structure * iphdr;
    {
 struct RA$DATA_BLOCK * RAPTR;
    signed long
	RC,
	IPDATA,
	OFFSET,
	DATLEN,
	MAXDAT,
	FIRST,
	COPY_MF;

// Keep count of fragments
    IP_group_MIB->IPMIB$ipReasmReqds = 
		IP_group_MIB->IPMIB$ipReasmReqds + 1;

// See if we can find this fragment's header on the queue

X:  {			// *** Block X ***
    RAPTR = RA_QUEUE->qhead;
    while (RAPTR != RA_QUEUE->qhead)
	{
	if ((RAPTR->ra$source == iphdr->iph$source) &&
	   (RAPTR->ra$dest == iphdr->iph$dest) &&
	   (RAPTR->ra$ident == iphdr->iph$ident))
	    goto leave_x;
	RAPTR = RAPTR->ra$next;
	};
    RAPTR = 0;
    }			// *** Block X ***
    leave_x:

// Handle the fragment according to the case.

// First fragment case. Flush old fragment and add this one.

    if ((iphdr->iph$fragmentation_data&0x80000000) && (iphdr->iph$fragment_offset == 0))
	{
	if (RAPTR != 0)
	    {

// RA data already exists - just flush the old buffer.

	    mm$seg_free(RAPTR->ra$bufsize,RAPTR->ra$buf);
	    }
	else
	    {

// RA data doesn't exist yet. Allocate it & fill in the IP parameters.

//	    RC = LIB$GET_VM(/*%REF*/(RA$Data_BLEN),RAPTR);
	    RC = LIB$GET_VM_PAGE(/*%REF*/((RA$DATA_BLEN / 512) + 1),&RAPTR);
	    if (BLISSIFNOT(RC))
		FATAL$FAO("IP_FRAGMENT - LIB$GET_VM failure, RC=!XL",RC);
	    RAPTR->ra$ident = iphdr->iph$ident;
	    RAPTR->ra$source = iphdr->iph$source;
	    RAPTR->ra$dest = iphdr->iph$dest;
	    RAPTR->ra$timeout = Time_Stamp()+iphdr->iph$ttl*RA_EXPIRE_TIME*CSEC;
	    XQL$FAO(LOG$IP,"!%T New reassambly block !XL, Timeout=!UL!/",
		    0,RAPTR,RAPTR->ra$timeout);

// Insert on the queue and start RA purge timer, if first entry on queue.

	    FIRST = queue_empty(RA_QUEUE);
	    INSQUE(RAPTR,RA_QUEUE->qtail);
	    if (FIRST)
		sys$setimr(0, RA_CHECK_TIME, IP_FRAGMENT_CHECK, 0, 0);
	    };
	RAPTR->ra$buf = mm$seg_get(16384);
	RAPTR->ra$bufsize = 16384;
	RAPTR->ra$data = RAPTR->ra$buf;
	RAPTR->ra$datend = CH$MOVE(iplen,iphdr,RAPTR->ra$data);
	RAPTR->ra$octet = iplen - HDRLEN;
	XQL$FAO(LOG$IP,
		"!%T Fragment at !XL for RA !XL, IPLEN=!SL, next octet=!SL!/",
		0,iphdr,RAPTR,iplen,RAPTR->ra$octet);
	mm$seg_free(BUFSIZE,BUF);
	return;
	}

// Not first fragment. Check for validity of this fragment & append its data.
    else
      if ((iphdr->iph$fragment_offset != 0) && (RAPTR != 0))
Y:	{

// Check for match of fragment octet offset

	OFFSET = iphdr->iph$fragment_offset*8;
	if (OFFSET != RAPTR->ra$octet)
	    goto leave_y;

// Fragment matches. Verify that we can store it or punt.

	DATLEN = iplen - HDRLEN;
	MAXDAT = RAPTR->ra$bufsize - (RAPTR->ra$datend-RAPTR->ra$buf);
	if (DATLEN > MAXDAT)
	    {
	    signed long
		NewBUF, NewBUFsize;

// Current buffer is too small.  Double its size, copy the data from the
// old buffer into the new one, and update the two other pointers into
// the innards of the data.

	    NewBUFsize = RAPTR->ra$bufsize * 2;
	    NewBUF = mm$seg_get(NewBUFsize);
	    CH$MOVE(RAPTR->ra$bufsize, RAPTR->ra$buf, NewBUF);
	    RAPTR->ra$data = (long)RAPTR->ra$data + ((long)NewBUF - (long)RAPTR->ra$buf);
	    RAPTR->ra$datend = (long)RAPTR->ra$datend + ((long)NewBUF - (long)RAPTR->ra$buf);
	    mm$seg_free(RAPTR->ra$bufsize, RAPTR->ra$buf);
	    RAPTR->ra$bufsize = NewBUFsize;
	    RAPTR->ra$buf = NewBUF;
	    };

// Append the data from this fragment to our current buffer.

	XQL$FAO(LOG$IP,"!%T Append fragment at !XL to RA !XL, LEN=!SL!/",
		0,iphdr,RAPTR,DATLEN);
	IPDATA = iphdr + HDRLEN;
	RAPTR->ra$datend = CH$MOVE(DATLEN,IPDATA,RAPTR->ra$datend);
	RAPTR->ra$octet = RAPTR->ra$octet + DATLEN;

// Copy the MF bit to preserve it from Seg_Free

	COPY_MF = iphdr->iph$fragmentation_data&0x80000000;

// Flush the buffer

	mm$seg_free(BUFSIZE,BUF);

// If this is the last fragment, then dispatch the datagram.

	if (! COPY_MF)
	    {

// Remove from the queue

	    REMQUE(RAPTR,&RAPTR);

// Calculate sizes of entire, reassembled packet & dispatch it.

	    iphdr = RAPTR->ra$data;
	    HDRLEN = iphdr->iph$ihl*4;
	    iplen = RAPTR->ra$datend-RAPTR->ra$data;
	    XQL$FAO(LOG$IP,"!%T Last fragment received, IPLEN=!SL, DLEN=!SL!/",
		    0,iplen,iplen-HDRLEN);
	    IP_group_MIB->IPMIB$ipReasmOKs = 
		IP_group_MIB->IPMIB$ipReasmOKs + 1;
	    IP_DISPATCH(iphdr,iplen,HDRLEN,RAPTR->ra$buf,
			RAPTR->ra$bufsize);

// Deallocate the queue block.

//	    LIB$FREE_VM(/*%REF*/(RA$Data_BLEN),RAPTR);
	    LIB$FREE_VM_PAGE(/*%REF*/((RA$DATA_BLEN / 512) + 1),RAPTR);
	    };
	return;
	};
    leave_y:

// If we got here, it means the fragment was unacceptable for some reason.
// Log this fact, release the buffer, and ignore the fragment.

    IP_group_MIB->IPMIB$ipReasmFails = 
		IP_group_MIB->IPMIB$ipReasmFails + 1;
    if ($$LOGF(LOG$IP+LOG$IPERR))
	{
	QL$FAO("!%T IP Fragment unusable!/",0);
	if (! $$LOGF(LOG$IP)) {
	  $DESCRIPTOR(dsc,"IPfrag");
	    ip$log(&dsc,iphdr);
	}
	};
    mm$seg_free(BUFSIZE,BUF);
    }

void IP_FRAGMENT_CHECK  (void)
//
// Routine to periodically check the reassembly queue, purging any entries
// which have expired.
//
    {
      struct RA$DATA_BLOCK * RAPTR;
    signed long
	NOW,
	RANXT;

// Scan the entire queue, looking for expired entries.

    NOW = Time_Stamp();
    RAPTR = RA_QUEUE->qhead;
    while (RAPTR != RA_QUEUE->qhead)
	{
	RANXT = RAPTR->ra$next;

// If this entry has expired, then flush it.

	if (RAPTR->ra$timeout < NOW)
	    {

// Flush the buffer & free the block

	    XQL$FAO(LOG$IP,"!%T Flushing expired IP RA block !XL!/",0,RAPTR);
	    REMQUE(RAPTR,&RAPTR);
	    mm$seg_free(RAPTR->ra$bufsize,RAPTR->ra$buf);
//	    LIB$FREE_VM(/*%REF*/(RA$Data_BLEN),RAPTR);
	    LIB$FREE_VM_PAGE(/*%REF*/((RA$DATA_BLEN / 512) + 1),RAPTR);
	    IP_group_MIB->IPMIB$ipReasmFails = 
			IP_group_MIB->IPMIB$ipReasmFails + 1;

	    };

// Check the next block

	RAPTR = RANXT;
	};

// If there are still entries on the reassembly queue, then requeue us

    if (! queue_empty(RA_QUEUE))
	sys$setimr(0, RA_CHECK_TIME, IP_FRAGMENT_CHECK, 0, 0);
    }
