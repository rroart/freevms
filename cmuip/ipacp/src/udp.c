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
//TITLE "User Datagram Protocol Handler"
//SBTTL "User Datagram Protocol Handler Overview"
/*

Module:

	UDP

Facility:

	User Datagram Protocol (UDP) handler

Abstract:

	UDP provides the user with a potentially unreliable datagram
	service via the Internet Protocol (IP). This module handles the
	UDP interface between the user and the IP layer.

Author:

	Vince Fuller, CMU-CSD, March, 1986
	Copyright (c) 1986, 1987, Vince Fuller and Carnegie-Mellon University

Modification History

4.0e	21-Jan-1992	Marc A. Shannon		CMU Group N
	Don't compute checksum for UDP packets going out from here to IP
	since a broadcast packet (to 255.255.255.255) will need its checksum
	recomputed when the destination address is changed to the local
	broadcast (something like 128.2.255.255).

4.0d	28-Aug-1991	Henry W. Miller		USBR
	Range check size of buffers and return NET$_IR if requested
	buffer size is greater than Max_UDP_Data_Size.

4.0c	18-Jul-1991	Henry W. Miller		USBR
	Use LIB$GET_VM_PAGE and LIB$FREE_VM_PAGE rather then LIB$GET_VM
	and LIB$FREE_VM, and check return status.

4.0b	30-May-1991	Henry W. Miller		USBR
	In UDP_COPEN_DONE(), log failures.

4.0a	13-Jan-1991	Henry W. Miller		USBR
	Make UDPTTL a configurable variable.
	Updated IDENT.

	10-Sep-1990     Henry W. Miller 	USBR
	Make UDP_CSEND() use DEFTTL.

4.0   04-Dec-1989, Bruce R. Miller	CMU Network Development
	Restructured the UDP packet.  Wildcard receives are passed
	by the driver into a user supplied buffer instead of being
	appended to the begining of the user's data buffer.

	Removed the Internal UDP processing.  No one uses it anymore.
	Changed name UCB to UDPCB to avoid confusion with Unit
	Control Blocks.  Removed address mode.  UDP address can
	now be spedified with a user supplid buffer.

3.7   11-FEB-1988, Dale Moore
	On UDP$SEND and ADDR_MODE check to see if sufficient bytes
	given by user for udp header. If not return BTS.

3.6  19-Nov-87, Edit by VAF
	Know about IP$SEND failures and give user return status of NET$_NRT
	on failures (no route to destination).

3.5  23-Sep-87, Edit by VAF
	Don't check checksums for packets with no checksums (i.e. checksum=0)
	Believe it or not, there are still weenies out there who don't put
	checksums in their UDP packets...

3.4  30-Jul-87, Edit by VAF
	Use $$KCALL macro instead of using $CMKRNL directly.

3.3  26-Mar-87, Edit by VAF
	Check for aborted UCB status when receiving UDP messages from the
	network and when queuing user send and receive requests.

3.2  23-Mar-87, Edit by VAF
	Use TCP's packet buffer sizes for speed.

3.1  10-Mar-87, Edit by VAF
	Make "address-mode" also handle the UDP ports, so one connection can
	handle packets to/from multiple foreign ports. Note that the local
	port is always resolved at open time and cannot be set this way.

3.0   3-Mar-87, Edit by VAF
	Change calling sequence of User$Post_IO_Status. Add facility for
	returning ICMP messages to the user for "address mode" connections.

2.9   2-Mar-87, Edit by VAF
	Add support for klugy "address mode" - allows user to specify the IP
	addresses for each UDP$SEND and returns the IP addresses of the input
	packet for each UDP$RECEIVE.

2.8  19-Feb-87, Edit by VAF
	Use new name lookup routines.
	Fix a bug in UCB_FIND - infinite loop if no UCB's...

2.7  18-Feb-87, Edit by VAF
	Support open with IP address instead of host name.

2.6  12-Feb-87, Edit by VAF
	Modifications for domain service.

2.5   5-Feb-87, Edit by VAF
	Call USER$CHECK_ACCESS for user UDP opens.

2.4   4-Feb-87, Edit by VAF
	Recode UDP open to avoid name lookup for internal opens.

2.3   3-Feb-87, Edit by VAF
	Fix bug in UDP_ADLOOK_DONE - was using the name pointer as the name
	length when copying it back to the UCB. Crashes ACP.

2.2  10-Dec-86, Edit by VAF
	Change order of arguments to Gen_Checksum.

2.1   2-Oct-86, Edit by VAF
	On ICMP errors, don"t close internal UCB"s - just abort all of their
	pending requests. Internal UCB's have to take care of themselves...

2.0  30-Sep-86, Edit by VAF
	Make IUDP_CLOSE give the correct number of arguments to UCB_CLOSE.

1.9  14-Aug-86, Edit by VAF
	Be NOINT when examining UCB->UCB$ARGBLK.
	Make Kill_UDP_Reqeuests also post UCB->UCB$ARGBLK.

1.8  13-Aug-86, Edit by VAF
	Add UDP debugging dump routines.

1.7  12-Aug-86, Edit by VAF
	Add internal entry points for use by ACP green protocol module.

1.6  12-Aug-86, Edit by VAF
	Make UDP$OPEN use the green protocol name lookup routines.

1.5   9-Aug-86, Edit by VAF
	Don't use SET_HOSTS any more - domain system makes things too
	complicated to use common routine for both TCP and UDP.

1.4   4-Aug-86, Edit by VAF
	Forgot to post request in UDP$CLOSE/UDP$ABORT.

1.3  31-Jul-86, Edit by VAF
	Debugging, general reorganization.

1.2  29-Jul-86, Edit by VAF
	Start working on the real implementation.

1.1  23-Jul-86, Edit by VAF
	Add stub routines for user interface.

1.0  24-Mar-86, Edit by VAF
	Original, nonfunctional stub routines for IP interface.
*/

//SBTTL "Module definition"	

#if 0
MODULE UDP( IDENT="4.0e",LANGUAGE(BLISS32),
	    ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			    NONEXTERNAL=LONG_RELATIVE),
	    LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	    OPTIMIZE,OPTLEVEL=3,ZIP)=

#endif

// Include standard definition files

#include <starlet.h>
#include <cmuip/central/include/neterror.h>
// not yet#include "CMUIP_SRC:[CENTRAL]NETXPORT";
#include "netvms.h"
#include <cmuip/central/include/netcommon.h>
#include <cmuip/central/include/nettcpip.h> 
#include "structure.h"
#include "tcpmacros.h"
#include "snmp.h"

#include <ssdef.h>
#include <descrip.h>

//*** Special literals from USER.BLI ***

extern signed long
    UCB$Q_DDP,
    UCB$L_CBID,
    UCB$L_EXTRA;

// External data items

extern signed long
    INTDF,
    AST_In_Progress,
    UDP_User_LP,
    log_state,
    MIN_PHYSICAL_BUFSIZE,
    MAX_PHYSICAL_BUFSIZE;

// External routines


// MACLIB.MAR

extern  void    Swapbytes();
extern  void    Movbyt();

// MEMGR.BLI

extern  void    MM$UArg_Free();
extern     MM$QBLK_Get();
extern  void    MM$QBLK_Free();
extern     MM$Seg_Get();
extern  void    MM$Seg_Free();

// USER.BLI

extern     USER$GET_LOCAL_PORT();
extern     USER$CHECK_ACCESS();
extern     USER$Err();
extern  void    IO$POST();
extern  void    User$Post_IO_Status();

// IP.BLI

extern  void    IP$SET_HOSTS();
extern     IP$SEND();
extern     Gen_Checksum();

// NMLOOK.BLI

extern  void    NML$CANCEL();
extern  void    NML$GETALST();
extern  void    NML$GETNAME();

// IOUTIL.BLI

extern     GET_IP_ADDR();
extern  void    ASCII_DEC_BYTES();
extern  void    ASCII_HEX_BYTES();
extern  void    LOG_FAO();
extern  void    QL_FAO();

// SNMP.BLI

extern     SNMP$NET_INPUT();

// RPC.BLI

extern     RPC$INPUT();
extern    RPC$CHECK_PORT();

signed long
    UDPTTL	= 32;

extern signed long
    SNMP_SERVICE,
    RPC_SERVICE;


//SBTTL "UDP data structures"

#define    Max_UDP_Data_Size  16384	// Max UDP data size
#define    UDPTOS  0			// Type of service
#define    UDPDF  FALSE		// Don't fragment flag (try fragmenting)

// Define the "UDPCB" - UDP analogue of TCB.

struct  UDPCB_Structure
{
unsigned long     udpcb$foreign_host	;	// UDP foreign host number
unsigned long     udpcb$foreign_port	;	//     foreign port
unsigned long     udpcb$local_host	;	//     local host
unsigned long     udpcb$local_port	;	//     local port
unsigned char     udpcb$foreign_hname	[MAX_HNAME];
unsigned short int     udpcb$foreign_hnlen	;
void *     udpcb$usr_qhead	;	// User receive request queue
void *     udpcb$usr_qtail	;
void *     udpcb$nr_qhead	;	// Net receive queue
void *     udpcb$nr_qtail	;
unsigned short int     udpcb$nr_qcount	;
  union {
unsigned char     udpcb$flags		[2];
    struct {
unsigned  	udpcb$wildcard	 : 1;	// UDPCB opened with wild FH/FP/LH
unsigned  	udpcb$addr_mode	 : 1;	// IP addresses in data buffer
unsigned  	udpcb$aborting	 : 1;	// UDPCB is closing
unsigned  	udpcb$nmlook	 : 1;	// UDPCB has an outstanding name lookup
	  };
  };
void *     udpcb$udpcbid	;	// UDPCB_Table index for this connection
void *     udpcb$ucb_adrs	;	// Connection UDPCB address
void *     udpcb$uargs		;	// Uarg block in pending open
unsigned long     udpcb$user_id	;	// Process ID of owner
unsigned short     udpcb$piochan;// Process IO channel
    };

#define    UDPCB_Size sizeof(struct UDPCB_Structure)

//MESSAGE(%NUMBER(UDPCB_Size)," longwords per UDPCB")


//SBTTL "UDP data storage"

static signed long
    UDPIPID  = 1,	// Current IP packet ID
    UDPCB_Count  = 0;	// Count of active UDPCBs
long    UDPCB_TABLE[MAX_UDPCB+1];// Table of UDPCBs

    struct UDP_MIB_struct * UDP_MIB ;	// UDP Management Information Block


//SBTTL "UDP packet logger"
/*
    Queue up a log entry to dump out a UDP packet.
 */

void Log_UDP_Packet(Seg,SwapFlag,SendFlag)
	struct udpkt_structure * Seg;
    {
      struct dsc$descriptor sptr;
    signed long
      segdata;
    struct udpkt_structure * segcopy ;
    struct udpkt_structure * seghdr;

    seghdr = Seg;		// Point at segment header
    segdata = Seg + UDP_HEADER_SIZE;
    if (SwapFlag)		// Need to byteswap header?
	{
	CH$MOVE(UDP_HEADER_SIZE,CH$PTR(Seg),CH$PTR(segcopy)); // Make a copy
	seghdr = segcopy;	// Point at this version...
	SwapBytes(UDP_HEADER_SIZE/2,seghdr); // Swap header bytes
	};

// Print first part of info

    if (SendFlag)
      sptr = ASCID("Sent");
    else
	sptr = ASCID("Received");

// Log the contents of the UDP header

    QL$FAO(/*%STRING*/("!%T !AS UDP packet, SEG=!XL, DATA=!XL!/",
		   "!_SrcPrt:!_!XL (!UL)!_DstPrt:!_!XL (!UL)!/",
		   "!_Length:!_!SL!_CKsum:!_!SL!/"),
	    0,sptr,Seg,segdata,
	   seghdr->up$source_port,seghdr->up$source_port,
	   seghdr->up$dest_port,seghdr->up$dest_port,
	   seghdr->up$length,seghdr->up$checksum);

// If there is any data in the segment, then dump it, too

    if (seghdr->up$length > UDP_HEADER_SIZE)
	{
#define	    maxhex  20
#define	    maxasc 50
	signed long
	    datalen,
	    asccnt,
	  hexcnt;
	DESC$STR_ALLOC(dathex,maxhex*3);
	datalen = seghdr->up$length - UDP_HEADER_SIZE;
	if (datalen > maxasc)
	  asccnt = maxasc;
	else
	    asccnt = datalen;
	if (datalen > maxhex)
	  hexcnt = maxhex;
	else
	    hexcnt = datalen;
	ASCII_Hex_Bytes(dathex,hexcnt,segdata,dathex->dsc$w_length);
	QL$FAO(/*%STRING*/("!_Data Count: !SL!/",
		       "!_HEX:!_!AS!/",
		       "!_ASCII:!_!AF!/"),
	       datalen,dathex,asccnt,segdata);
	};
    }

//SBTTL "UDPCB_Find - look up UDP control block"

UDPCB_Find(Src$Adrs,Src$Port,Dest$Port)
    {
      struct UDPCB_Structure * UDPCB;
    signed long
	Ucount,
      UDPCBIX;

    Ucount = UDPCB_Count;
    UDPCBIX = 1;
    while ((Ucount > 0) && (UDPCBIX <= MAX_UDPCB))
	{
	if ((UDPCB = UDPCB_TABLE[UDPCBIX]) != 0)
	    {
	    if (((UDPCB->udpcb$foreign_host == WILD) ||
		(UDPCB->udpcb$foreign_host == Src$Adrs)) &&
	       ((UDPCB->udpcb$foreign_port == WILD) ||
	        (UDPCB->udpcb$foreign_port == Src$Port)) &&
	       (UDPCB->udpcb$local_port == Dest$Port))
		return UDPCB;
	    Ucount = Ucount-1;
	    };
	UDPCBIX = UDPCBIX + 1;
	};
    return 0;
    }


//SBTTL "UDP input handler"
/*
    Come here at AST level when input packet is determined to be UDP packet.
    At present, all UDP input handling is done at AST level, so we search
    the UDPCB list and queue the UDP packet for deliver here.
*/

UDP_SEND();
    Queue_User_UDP();

void UDP$Input(Src$Adrs,Dest$Adrs,BufSize,Buf,SegSize,Seg)
	struct udpkt_structure * Seg;
    {
    signed long
	RC,
	Uptr,
	Ucount,
	RPC_index,
	UDPCBIX,
	sum,
      delete;
    ipadr$address_block * Aptr;
    struct UDPCB_Structure * UDPCB;

// Assume this packet should be deleted

    delete = TRUE;

// Log the UDP packet if desired

    if ($$LOGF(LOG$UDP))
	Log_UDP_Packet(Seg,TRUE,FALSE);

// Verify UDP checksum, if there is one

    if (Seg->up$checksum != 0)
	{
	sum = Gen_Checksum(SegSize,Seg,Src$Adrs,Dest$Adrs,UDP_Protocol);
	if (sum != 0xFFFF)
	    {			// Bad checkum - log & drop packet
	    UDP_MIB->MIB$UDPINERRORS = UDP_MIB->MIB$UDPINERRORS + 1;
	    if ($$LOGF(LOG$UDP))
		QL$FAO("!%T UDP seg !XL dropped on bad checksum (!XL)!/",0,
			Seg,sum);
	    return;
	    };
	};

// Fix byte order of the UDP header

    SwapBytes(UDP_HEADER_SIZE/2,Seg);

    
// Setup pointer to UDP data and UDP data size

    Uptr = Seg + UDP_HEADER_SIZE;
    Ucount = SegSize - UDP_HEADER_SIZE;

    // Check to see if it's an RPC port
    if ((RPC_SERVICE &&
	((RPC_index = RPC$CHECK_PORT(Seg->up$dest_port)) > 0)))
	{
	signed long
	  out_len;
	unsigned char out_buff  [16384]; // check will destroy stack and pcb

	// Keep count
	UDP_MIB->MIB$UDPINDATAGRAMS = UDP_MIB->MIB$UDPINDATAGRAMS + 1;

	out_len = 16384; // initial size of buffer

	// Pass the UDP data to the Port Mapper module
	// The results are returned in out_buff, size in out_len
	RC = RPC$INPUT( RPC_index, Src$Adrs,Dest$Adrs,
			     Seg->up$source_port,Seg->up$dest_port,
			     Ucount,Uptr,out_buff,out_len);
        if ((RC && (out_len > 0)))
	    {
	    // Send off the reply datagram
	    RC = UDP_SEND(Dest$Adrs,Src$Adrs,
			  Seg->up$dest_port,Seg->up$source_port,
		 	  out_buff,out_len);
	    if (RC != SS$_NORMAL)
		XLOG$FAO(LOG$UDP,"!%T UDP RPC reply error, RC=!XL!/",0,RC);
	    };

	// Release the input datagram buffer
	if (delete)
	    MM$Seg_Free(BufSize,Buf);

	return;	// Don't pass this buffer on to the upper layers
	};

// Check to see if it's an SNMP packet
    if ((Seg->up$dest_port == UDP_PORT_SNMP) && SNMP_SERVICE)
	{
	signed long
	  out_len;
	unsigned char    out_buff [512];

	// Keep count
	UDP_MIB->MIB$UDPINDATAGRAMS = UDP_MIB->MIB$UDPINDATAGRAMS + 1;

	out_len = 512; // initial size of buffer

	// Pass the UDP data to the SNMP module
	// The results are returned in out_buff, size in out_len
	RC = SNMP$NET_INPUT( Src$Adrs,Dest$Adrs,
			     Seg->up$source_port,Seg->up$dest_port,
			     Ucount,Uptr,out_buff,out_len);

	// Send off the reply datagram
	RC = UDP_SEND(Dest$Adrs,Src$Adrs,
		      Seg->up$dest_port,Seg->up$source_port,
		      out_buff,out_len);
	if (RC != SS$_NORMAL)
	    XLOG$FAO(LOG$UDP,"!%T UDP SNMP reply error, RC=!XL!/",0,RC);

	// Release the input datagram buffer
	if (delete)
	    MM$Seg_Free(BufSize,Buf);

	return;	// Don't pass this buffer on to the upper layers
	};

// Try to match the input packet up with a UDPCB

    UDPCB = UDPCB_Find(Src$Adrs,Seg->up$source_port,Seg->up$dest_port);
    if (UDPCB == 0)
	{
	UDP_MIB->MIB$UDPNOPORTS = UDP_MIB->MIB$UDPNOPORTS + 1;
	if ($$LOGF(LOG$UDP))
	    QL$FAO("!%T No UDPCB found for segment !XL, SP=!SL, DP=!SL!/",
		   0,Seg,Seg->up$source_port,Seg->up$dest_port);
	}
    else
X2:	{

// Log that it was found

	if ($$LOGF(LOG$UDP))
	    QL$FAO("!%T UDPCB !XL found for UDP !XL, SP=!SL, DP=!SL!/",
		   0,Seg,UDPCB,Seg->up$source_port,Seg->up$dest_port);

// Make sure the UDPCB isn't aborted...

	if (UDPCB->udpcb$aborting)
	    {
	    XQL$FAO(LOG$UDP,"!%T UDP input !XL for aborted UDPCB !XL dropped!/",
		    0,Seg,UDPCB);
	    goto leave_x2;
	    };

// "Normal" UDPCB's stop being wildcarded when they receive something....

//	if (! UDPCB->udpcb$addr_mode)
//	    {

// If the connection was wildcarded, resolve hosts and ports now

//	    if (UDPCB->udpcb$wildcard)
	    if (0)
		{
		UDPCB->udpcb$wildcard = FALSE;
		if (UDPCB->udpcb$foreign_host == WILD)
		    UDPCB->udpcb$foreign_host = Src$Adrs;
		else
		  if (UDPCB->udpcb$local_host == WILD)
		    UDPCB->udpcb$local_host = Dest$Adrs;
		else
		  if (UDPCB->udpcb$foreign_port == WILD)
		    UDPCB->udpcb$foreign_port = Seg->up$source_port;
		};
//	    };		// (non ADDR_MODE case)

// Kluge. Overwrite the UDP/IP header in the buffer, since we don't need it.
//!!HACK!!!
	Aptr = Uptr - IPADR$UDP_ADDRESS_BLEN;
	Ucount = Ucount + IPADR$UDP_ADDRESS_BLEN;
	Aptr->ipadr$src_port = Seg->up$source_port;
	Aptr->ipadr$dst_port = Seg->up$dest_port;
	Aptr->ipadr$src_host = Src$Adrs;
	Aptr->ipadr$dst_host = Dest$Adrs;

// Give the segment to the user now.

	delete = Queue_User_UDP (UDPCB,Aptr,Ucount,Buf,BufSize,0);
	}; // End of block X2
    leave_x2:

// If the packet hasn't been given to the user, delete it now

    if (delete)
	MM$Seg_Free(BufSize,Buf);
    }

//SBTTL "ICMP input handler for UDP"
/*
    Handles ICMP messages received in response to our UDP packets.
    For "normal" UDP connections, ICMP messages generally cause connection
    aborts. For "address-mode" connections, ICMP messages are delivered to
    the user.
*/

void    Deliver_UDP_Data();
 void    UDPCB_ABORT();

void UDP$ICMP(ICMtype,ICMex,IPsrc,IPdest,UDPptr,UDPlen,
			buf,bufsize)
//ICMtype - ICMP packet type
//ICMex - extra data from ICMP packet (pointer for ICM_PPROBLEM)
//IPsrc - source address of offending packet
//IPdest - destination address of offending packet
//UDPptr - first 64-bits of data from offending packet
//UDPlen - calculated octet count of data
//Buf - address of network buffer
//Bufsize - size of network buffer

	struct udpkt_structure * UDPptr;
    {
      struct UDPCB_Structure * UDPCB;
    signed long
	delete;

// Fix byte order of the UDP header

    if ($$LOGF(LOG$ICMP+LOG$UDP))
	QL$FAO("!%T UDP$ICMP UDPlen !SL Bufsize !SL!/",0,UDPlen,bufsize);
    SwapBytes(UDPlen/2,UDPptr);

// Find the connection that this ICMP message is for
// Remember that the ICMP/IP message is what we originally sent, so the
// hosts and ports are reversed. UDPCB_FIND(FRN_HOST,FRN_PORT,LCL_PORT)

    delete = TRUE;
    UDPCB = UDPCB_Find(IPdest,UDPptr->up$dest_port,UDPptr->up$source_port);
    if (UDPCB == 0)
	{			// Bogus UDP/ICMP message
	if ($$LOGF(LOG$ICMP))
	    {
		DESC$STR_ALLOC(fhstr,20);

	    ASCII_DEC_BYTES(fhstr,4,IPdest,fhstr->dsc$w_length);
	    QL$FAO("!%T ICMP for unknown UDPCB, FH=!AS, FP=!XL, LP=!XL!/",
		    0,fhstr,UDPptr->up$dest_port,UDPptr->up$source_port);
	    };
	}
    else
X:	{			// Good UDP/ICMP message
	if ($$LOGF(LOG$ICMP+LOG$UDP))
	    QL$FAO("!%T ICMP type !SL for UDPCB !XL!/",0,ICMtype,UDPCB);

// Ignore the packet, if the connection is aborted

	if (UDPCB->udpcb$aborting)
	    {
	    XQL$FAO(LOG$UDP,"!%T ICMP message for aborted UDPCB !XL dropped!/",
		       0,UDPCB);
	    goto leave_x;
	    };

// ADDR_MODE UDPCB's get the ICMP message delivered to them, with IOSB flags set
// indicating that this is an ICMP message and the ICMP error code
//!!HACK!!// How will we send back the ICMP message?
//!!HACK!!// Are you sure???
	if (1)
	    {
	      struct queue_blk_structure(qb_nr_fields) * QB;
	      ipadr$address_block * Uptr;
	    signed long
		Ucount;
extern		MM$QBlk_Get();

// Allocate and setup the fields in the QB. ** N.B. We overwrite part of the IP
// header, so be careful if you change the size of the UDPUSER block **

	    QB = MM$QBLK_Get();
	    QB->nr$icmp = TRUE;
	    QB->nr$icm_type = ICMtype;
	    QB->nr$icm_code = 0;
	    QB->nr$icm_ex = ICMex;
	    Uptr = UDPptr - IPADR$UDP_ADDRESS_BLEN;
	    Uptr->ipadr$src_host = IPdest;
	    Uptr->ipadr$dst_host = IPsrc;
	    Uptr->ipadr$src_port = UDPptr->up$dest_port;
	    Uptr->ipadr$dst_port = UDPptr->up$source_port;
	    Ucount = UDPlen + IPADR$UDP_ADDRESS_BLEN;
	    delete = Queue_User_UDP(UDPCB,Uptr,Ucount,buf,bufsize,QB);
	    }
	else
	    {

// Normal connections are generally aborted.

	      switch (ICMtype)
		{
	    case ICM_DUNREACH:	// Destination Unreachable - abort connection
		{
		UDPCB_Abort(UDPCB,NET$_URC);
		if ($$LOGF(LOG$ICMP || LOG$UDP))
		    QL$FAO(
		       "!%T UDPCB !XL killed by ICMP Destination Unreachable!/",
		       0,UDPCB);
		};
		break;

	    case ICM_TEXCEED:	// Time exceeded - abort
		{
		UDPCB_Abort(UDPCB,NET$_CTO);
		if ($$LOGF(LOG$ICMP || LOG$UDP))
		    QL$FAO("!%T UDPCB !XL killed by ICMP Time Exceeded!/",
			   0,UDPCB);
		};
		break;

	    case ICM_SQUENCH:	// Source quench - currently unsupported
		{
		0;
		};
		break;

	    case ICM_REDIRECT:	// Redirect - not supported in this module
		{
		0;
		};
		break;

	    case ICM_PPROBLEM:	// Parameter problem - not yet supported
		{
		0;
		};
		break;
	    };
	    };
	};
    leave_x:

// In any case, free up the buffer
// No, dont't clear the buffer.  What if ICMP needs it?
    if (delete)
	MM$Seg_Free(bufsize,buf);
    }

//SBTTL "Queue_User_UDP - Queue up UDP packet for delivery to user"
/*
    Called by UDP$Input at AST level when an input packet matches a
    user UDP "connection". Function of this routine is to either deliver
    the UDP packet to the user (if a user read request is available) or
    queue it for later deliver.
    Returns TRUE if the UDP packet has been fully disposed of (i.e. the
    caller may deallocate the packet), FALSE otherwise (i.e. the packet
    has been placed on a queue and may not be deallocated yet).
 */

Queue_User_UDP(UDPCB,Uptr,Usize,Buf,Bufsize,QB)
     struct UDPCB_Structure * UDPCB;
	struct queue_blk_structure(qb_nr_fields) * QB;
    {
    signed long
	QBR;
extern	MM$QBlk_Get();
#define	UDPCB$NR_Qmax 5	// Max input packets permitted on input queue

// See if the input queue is full for this UDPCB

    if (UDPCB->udpcb$nr_qcount > UDPCB$NR_Qmax)
	{
	UDP_MIB->MIB$UDPINERRORS = UDP_MIB->MIB$UDPINERRORS + 1;
	if ($$LOGF(LOG$UDP))
	    QL$FAO("!%T UDP at !XL dropped - UDPCB NR queue full!/",0,Uptr);
	return TRUE;		// Drop the packet - no room
	};

// Allocate a queue block and insert onto user receive queue

    if (QB == 0)
	QB = MM$QBLK_Get();
    QB->nr$buf_size = Bufsize;	// Total size of network buffer
    QB->nr$buf = Buf;		// Pointer to network buffer
    QB->nr$ucount = Usize;	// Length of the data
    QB->nr$uptr = Uptr;	// Pointer to the data

// If there is a user read outstanding, deliver data, else queue for later

    if (REMQUE(UDPCB->udpcb$usr_qhead,QBR) != EMPTY_QUEUE) // check
      Deliver_UDP_Data (UDPCB,QB,QBR);
    else
	INSQUE(QB,UDPCB->udpcb$nr_qtail);
    return FALSE;		// Don't deallocate this segment...
    }

//SBTTL "Deliver_UDP_Data - Deliver UDP data to user"
/*
    Perform actual delivery of UDP packet to a user request.
    UDP packet is copied into the user buffer and the user I/O request
    is posted.
 */

void Deliver_UDP_Data(UDPCB,QB,URQ)
     struct UDPCB_Structure * UDPCB;
     struct queue_blk_structure(qb_nr_fields) * QB;
	struct queue_blk_structure(qb_ur_fields) * URQ;
    {
      struct User_RECV_Args * UArgs;
      struct User_RECV_Args * Sargs;
      void * IRP;
      signed long
	FLAGS,
	ICMTYPE,
	PID,
	Aptr,
	Uptr,
	Ucount;

// Determine data start and data count

    Ucount = QB->nr$ucount - IPADR$UDP_ADDRESS_BLEN;
    Uptr = QB->nr$uptr + IPADR$UDP_ADDRESS_BLEN;
    Aptr = QB->nr$uptr;

// Truncate to user receive request size

//!!HACK!!// You can't just drop data like that//  Or can you?
    if (Ucount > URQ->ur$size)
	Ucount = URQ->ur$size;

    if ($$LOGF(LOG$UDP))
	QL$FAO("!%T Posting UDP receive,Size=!SL,UDPCB=!XL,IRP=!XL,UDPCB_A=!XL PID=!XL!/",
	       0,Ucount,UDPCB,URQ->ur$irp_adrs,URQ->ur$ucb_adrs,PID);

// Copy from our buffer to the user system buffer

#if 0
    // check
    $$KCALL(MOVBYT,Ucount,Uptr,URQ->ur$data);
#endif

// Copy UDP Source and destination addresses to system space Diag Buff

    UArgs = URQ->ur$uargs;
    IRP = URQ->ur$irp_adrs;
#if 0
    if (Uargs->re$ph_buff != 0)
	$$KCALL(MOVBYT,IPADR$UDP_ADDRESS_BLEN,
		.Aptr,Uargs->re$ph_buff);
#endif

// Post the I/O and free up memory

    ICMTYPE = 0;
    FLAGS = 0;
    if (QB->nr$icmp)
	{
	ICMTYPE = QB->nr$icm_type;
	FLAGS = FLAGS || NSB$ICMPBIT;
	};
    User$Post_IO_Status(URQ->ur$uargs,SS$_NORMAL,
			Ucount,FLAGS,ICMTYPE);
    MM$UArg_Free(URQ->ur$uargs);

    MM$QBLK_Free(URQ);
    MM$Seg_Free(QB->nr$buf_size,QB->nr$buf);
    MM$QBLK_Free(QB);
    UDP_MIB->MIB$UDPINDATAGRAMS = UDP_MIB->MIB$UDPINDATAGRAMS + 1;
    }

//SBTTL "UDPCB_OK - Match connection ID to UDPCB address"

UDPCB_OK(long Conn_ID,long RCaddr,struct user_default_args * Uargs)
    {
	struct UDPCB_Structure * UDPCB;
#define	UDPCBERR(EC) { RCaddr = EC; return 0;}

// Range check the connection id. This should never fail, since the user should
// not be fondling connection IDs.

    if ((Conn_ID <= 0) || (Conn_ID > MAX_UDPCB))
	UDPCBERR(NET$_CDE);	// Nonexistant connection ID
    UDPCB = UDPCB_TABLE[Conn_ID];

// Make sure the table had something reasonable for this connection ID

    if (UDPCB <= 0)
	UDPCBERR(NET$_CDE);	// UDPCB has been deleted (possible)

// Check consistancy of UDPCB back-pointer into table

    if ((UDPCB->udpcb$udpcbid != Conn_ID) ||
       (UDPCB->udpcb$ucb_adrs != Uargs->ud$ucb_adrs))
	UDPCBERR(NET$_CDE);	// Confusion (can this happen?)

// Everything is good - return the UDPCB address

    return UDPCB;
    }

//SBTTL "UDPCB_Get - Allocate and initialize one UDPCB"

UDPCB_Get(IDX,Src$Port)
    {
      extern	LIB$GET_VM();
      extern	LIB$GET_VM_PAGE();
      struct UDPCB_Structure * UDPCB;
      signed long I,
	Ucount,
	UDPCBIDX,
	RC ;

// Check to make sure we haven't already allocated this local port

    Ucount = UDPCB_Count;
    UDPCBIDX = 1;
    while ((Ucount > 0) && (UDPCBIDX <= MAX_UDPCB))
	{
	if ((UDPCB = UDPCB_TABLE[UDPCBIDX]) != 0)
	    {
	    if ((UDPCB->udpcb$local_port == Src$Port))
		return 0;
	    Ucount = Ucount-1;
	    };
	UDPCBIDX = UDPCBIDX + 1;
	};

// Find a free slot in the UDPCB table

X:  {			// ** Block X **
    UDPCBIDX = 0;
    for (I=1;I<=MAX_UDPCB;I++)
	if (UDPCB_TABLE[I] == 0)
	  { UDPCBIDX = I; goto leave_x; }
    return 0;			// Failed to allocate a UDPCB
    }			// ** Block X **
    leave_x:

// Allocate some space for the UDPCB

//    LIB$GET_VM(%REF(UDPCB_Size*4),UDPCB);
    RC = LIB$GET_VM_PAGE(/*%REF*/(((UDPCB_Size * 4) / 512) + 1),UDPCB);
    if (! RC)
	FATAL$FAO("UDPCB_GET - LIB$GET_VM failure, RC=!XL",RC);

// Clear it out and set it in the table

    UDPCB_TABLE[UDPCBIDX] = UDPCB;
    CH$FILL(/*%CHAR*/(0),UDPCB_Size*4,UDPCB);
    UDPCB_Count = UDPCB_Count+1;

// Initialize queue headers for the UDPCB

    UDPCB->udpcb$nr_qhead = UDPCB->udpcb$nr_qtail = UDPCB->udpcb$nr_qhead;
    UDPCB->udpcb$usr_qhead = UDPCB->udpcb$usr_qtail = UDPCB->udpcb$usr_qhead;

// Set the connection ID

    UDPCB->udpcb$udpcbid = UDPCBIDX;

// Return the pointer

    IDX = UDPCBIDX;
    return UDPCB;
    }

//SBTTL "UDPCB_Free - Deallocate a UDPCB"

void UDPCB_Free(long UDPCBIX,struct UDPCB_Structure * UDPCB)
    {
      extern	LIB$FREE_VM();
      extern LIB$FREE_VM_PAGE();
    signed long
	RC ;

// Clear the table entry

    UDPCB_TABLE[UDPCBIX] = 0;

// Free the memory and decrement our counter.

//    LIB$FREE_VM(/*%REF*/(UDPCB_Size*4),UDPCB);
    RC = LIB$FREE_VM_PAGE(/*%REF*/(((UDPCB_Size * 4) / 512) + 1),UDPCB);
    if (! RC)
	FATAL$FAO("UDPCB_FREE - LIB$FREE_VM failure, RC=!XL",RC);
    UDPCB_Count = UDPCB_Count-1;
    }

//SBTTL "Kill_UDP_Requests - purge all I/O requests for a connection"

void Kill_UDP_Requests(struct UDPCB_Structure * UDPCB,long RC)
    {
      struct queue_blk_structure(qb_ur_fields) * URQ;
	struct queue_blk_structure(qb_nr_fields) * QB;

// Make sure we aren't doing this more than once
//
//   if (UDPCB->udpcb$aborting)
//	RETURN;

// Say that this connection is aborting (prevent future requests)

    UDPCB->udpcb$aborting = TRUE;

// Cancel any name lookup in progess

    if (UDPCB->udpcb$nmlook)
	{
	NML$CANCEL(UDPCB, 0, 0);
	UDPCB->udpcb$nmlook = FALSE;
	};

// Kill any pending open

    NOINT;
    if (UDPCB->udpcb$uargs != 0)
	{
	USER$Err(UDPCB->udpcb$uargs,RC);
	UDPCB->udpcb$uargs = 0;
	};
    OKINT;

// Purge the user request queue, posting all requests

    while (REMQUE(UDPCB->udpcb$usr_qhead,URQ) != EMPTY_QUEUE) // check
	{
	    User$Post_IO_Status(URQ->ur$uargs,RC,0,0,0);
	    MM$UArg_Free(URQ->ur$uargs);
	    MM$QBlk_Free(URQ);	
	};

// Purge any received qblocks as well

    while (REMQUE(UDPCB->udpcb$nr_qhead,QB) != EMPTY_QUEUE) // check
	{
	MM$Seg_Free(QB->nr$buf_size,QB->nr$buf);
	MM$QBlk_Free(QB);
	};
    }

//SBTTL "UDPCB_Close - Close/deallocate a UDPCB"

void UDPCB_Close(long UIDX,struct UDPCB_Structure * UDPCB,long RC)
    {
    Kill_UDP_Requests(UDPCB,RC);
    UDPCB_FREE(UIDX,UDPCB);
    }

void UDPCB_Abort(struct UDPCB_Structure * UDPCB,long RC)
//
// Abort a UDPCB - called by ICMP code.
//
    {
      UDPCB_CLOSE(UDPCB->udpcb$udpcbid,UDPCB,RC);
    }



//SBTTL "Purge_All_UDP_IO - delete UDP database before network exits"

void UDP$Purge_All_IO  (void)
    {
    signed long
      UDPCBIDX;
	struct UDPCB_Structure * UDPCB;

// Loop for all connections, purge them, and delete them.

    for (UDPCBIDX=1;UDPCBIDX<=MAX_UDPCB;UDPCBIDX++)
	if ((UDPCB = UDPCB_TABLE[UDPCBIDX]) != 0)
	    UDPCB_Close(UDPCBIDX,UDPCB,NET$_TE);
    }

//SBTTL "UDP_Conn_Unique - Check for unique UDP connection"
/*
    Verify that a given set of Local Port, Foreign Host, Foreign Port
    are not already in use by an existing UDP connection.
    Returns TRUE if the connection is unique, false otherwise.
 */

UDP_Conn_Unique(LP,FH,FP)
    {
      struct UDPCB_Structure * UDPCB;
      signed long I,
	Ucount;

    Ucount = UDPCB_Count;
    for (I=1;I<=MAX_UDPCB;I++)
	if ((UDPCB = UDPCB_TABLE[I]) != 0)
	    {
	    if ((UDPCB->udpcb$foreign_host == FH) &&
	       (UDPCB->udpcb$foreign_port == FP) &&
	       (UDPCB->udpcb$local_port == LP))
		return FALSE;
	    if ((Ucount = Ucount-1) <= 0)
		return TRUE;
	    };
    return TRUE;
    }

//SBTTL "UDP$OPEN - open a UDP "connection""
/*
    Open a UDP "connection". Create a UDP Control Block, which serves as a
    place to hang incoming packets and user receive requests.
 */

UDP_COPEN_DONE();
void    UDP_NMLOOK_DONE();
void    UDP_ADLOOK_DONE();

void UDP$OPEN(struct user_open_args * Uargs)
    {
      ipadr$address_block * ProtoHdr;
      struct UDPCB_Structure * UDPCB;
      unsigned long NAMPTR;
    signed long
	IPADDR,
	NAMLEN,
	UIDX,
	udpcbptr,
	Args[4];

    XLOG$FAO(LOG$USER,"!%T UDP$OPEN: PID=!XL,CHAN=!XL,FLAGS=!XL X1=!XL!/",
	     0,Uargs->op$pid,Uargs->op$piochan,Uargs->op$flags,
	     Uargs->op$ext1);

    ProtoHdr = Uargs->op$protohdrblk;

// First create a UDPCB for this connection.

    if ((UDPCB = UDPCB_Get(UIDX,ProtoHdr->ipadr$src_port)) <= 0)
	{
	USER$Err(Uargs,NET$_UCT);
	return;
	};

// Initialize user mode values

    UDPCB->udpcb$ucb_adrs = Uargs->op$ucb_adrs;
    UDPCB->udpcb$user_id = Uargs->op$pid;
    UDPCB->udpcb$piochan = Uargs->op$piochan;

// At this point, the connection exists. Write the connection ID
// back into the Unit Control Block for this connection.

    udpcbptr = Uargs->op$ucb_adrs + UCB$L_CBID;
#if 0
    $$KCALL(MOVBYT,4,UIDX,udpcbptr);
#endif

// Initialize queue headers for the UDPCB

    UDPCB->udpcb$nr_qhead = UDPCB->udpcb$nr_qtail = UDPCB->udpcb$nr_qhead;
    UDPCB->udpcb$usr_qhead = UDPCB->udpcb$usr_qtail = UDPCB->udpcb$usr_qhead;

// Copy user arguments to UDPCB

    if ((Uargs->op$mode == OP$MODE_UDPADDR))
	UDPCB->udpcb$addr_mode = TRUE;
    ProtoHdr = Uargs->op$protohdrblk;
    UDPCB->udpcb$local_port = ProtoHdr->ipadr$src_port;
    UDPCB->udpcb$foreign_port = ProtoHdr->ipadr$dst_port;
    if (UDPCB->udpcb$foreign_port == WILD)
	UDPCB->udpcb$wildcard = TRUE;

// Handle wildcard host

    NAMPTR = CH$PTR(Uargs->op$foreign_host);
    NAMLEN = Uargs->op$foreign_hlen;
    if ((NAMLEN == 0) && (! Uargs->op$addr_flag))
	{
	UDPCB->udpcb$wildcard = TRUE;
	UDPCB->udpcb$foreign_host = WILD;
	UDPCB->udpcb$foreign_hnlen = 0;
	UDPCB->udpcb$local_host = WILD;
	UDPCB->udpcb$uargs = Uargs;
	UDP_NMLOOK_DONE(UDPCB,SS$_NORMAL,0,0,0,0);
	return;
	};

// Check for supplied IP address instead of name

X:  {			// *** Block X ***
    if (Uargs->op$addr_flag)
      IPADDR = Uargs->op$foreign_address;
    else
	if (GET_IP_ADDR(NAMPTR,IPADDR) < 0)
	    goto leave_x;
    UDPCB->udpcb$foreign_hnlen = 0;
    UDPCB->udpcb$uargs = Uargs;
    UDP_NMLOOK_DONE(UDPCB,SS$_NORMAL,1,IPADDR,0,0);
    UDPCB->udpcb$nmlook = TRUE;
    NML$GETNAME(IPADDR,UDP_ADLOOK_DONE,UDPCB);
    return;
    }			// *** Block X ***
    leave_x:

// "standard" case, host name is supplied - start name lookup for it

    UDPCB->udpcb$uargs = Uargs;
    UDPCB->udpcb$nmlook = TRUE;
    NML$GETALST(NAMPTR,NAMLEN,UDP_NMLOOK_DONE,UDPCB);
    }



//SBTTL "UDP_NMLOOK_DONE - Second phase of UDP$OPEN when namelookup done"
/*
    Come here when the foreign host name has been resolved.
    At this point, we set the local & foreign hosts/ports in the UDPCB
    and post the users open request.
*/

void UDP_NMLOOK_DONE(UDPCB,STATUS,ADRCNT,ADRLST,NAMLEN,NAMPTR)
	struct UDPCB_Structure * UDPCB;
    {
    signed long
      RC;
	struct user_open_args * Uargs;
	 netio_status_block * IOSB ;
#define	UOP_ERROR(EC) \ 
	    { \
	    USER$Err(Uargs,EC); \
	    UDPCB_FREE(UDPCB->udpcb$udpcbid,UDPCB); \
	    return; \
	    }

// Clear name lookup flag and get uargs

    NOINT;
    UDPCB->udpcb$nmlook = FALSE;
    Uargs = UDPCB->udpcb$uargs;
    UDPCB->udpcb$uargs = 0;
    OKINT;

// Check status of the name lookup

    if (! STATUS)
	UOP_ERROR(STATUS);

// Finish up the common part of the open

    RC = UDP_COPEN_DONE(UDPCB,ADRCNT,ADRLST);
    if (! RC)
	UOP_ERROR(RC);

// Verify that we have access to the host/port set

    RC = USER$CHECK_ACCESS(UDPCB->udpcb$user_id,UDPCB->udpcb$local_host,
		      UDPCB->udpcb$local_port,UDPCB->udpcb$foreign_host,
		      UDPCB->udpcb$foreign_port);
    if (! RC)
	UOP_ERROR(RC);

// Set the foreign host name in the UDPCB

    UDPCB->udpcb$foreign_hnlen = NAMLEN;
    if (NAMLEN != 0)
	CH$MOVE(NAMLEN,NAMPTR,CH$PTR(UDPCB->udpcb$foreign_hname));

// Finally, post the status

    IOSB->nsb$status = SS$_NORMAL; // Success return
    IOSB->nsb$byte_count = 0;
    IOSB->net_status.nsb$xstatus = 0;
    IO$POST(IOSB,Uargs);
    MM$UArg_Free(Uargs);
    }

//SBTTL "UDP_COPEN_DONE - Common user UDP open done routine"

UDP_COPEN_DONE(UDPCB,ADRCNT,ADRLST)
	struct UDPCB_Structure * UDPCB;
    {
      signed long I,
	IP_Address ;

// Set local and foreign host numbers according to our info

    if (ADRCNT > 0)
	IP$SET_HOSTS(ADRCNT,ADRLST,UDPCB->udpcb$local_host,
		     UDPCB->udpcb$foreign_host);

// Now, check that this connection is unique and get a local port, if needed.

    if (UDPCB->udpcb$local_port != WILD)
	{
	if (! UDP_Conn_Unique(UDPCB->udpcb$local_port,UDPCB->udpcb$foreign_host,
				UDPCB->udpcb$foreign_port))
	    return NET$_NUC;
	}
    else
	{
#define	    Max_LP_Tries 100

// Try a bunch of times to find a unique local port...

	for (I=1;I<=Max_LP_Tries;I++)
	    {
	    signed long
		LP;
	    LP = USER$GET_LOCAL_PORT(UDP_User_LP);
	    if (UDP_Conn_Unique(LP,UDPCB->udpcb$foreign_host,
				UDPCB->udpcb$foreign_port))
		EXITLOOP (UDPCB->udpcb$local_port = LP);
	    };

// If it failed, then no connections available - punt

	if (UDPCB->udpcb$local_port == 0)
	    {
	    XLOG$FAO(LOG$USER,"!%T UDB_COPEN: Conn failed !/", 0);
	    ACT$FAO("!%D Open UDP Port failed !/", 0 );
	    return NET$_CSE;
	    } ;
	};

// Done at last - log success

    XLOG$FAO(LOG$USER,"!%T UDB_COPEN: Conn idx = !XL, UDPCB = !XL!/",
	     0,UDPCB->udpcb$udpcbid,UDPCB);
    IP_Address = UDPCB->udpcb$foreign_host ;
#if 0
    ACT$FAO("!%D Open UDP Port !UW (!UW) <!UB.!UB.!UB.!UB>!/",0,
	UDPCB->udpcb$local_port,
	UDPCB->udpcb$foreign_port,
	.IP_Address<0,8>,IP_Address<8,8>,
	.IP_Address<16,8>,IP_Address<24,8>
		   );
#endif
    return SS$_NORMAL;
    }

//SBTTL "UDP_ADLOOK_DONE - Finish UDP address to name lookup"

void UDP_ADLOOK_DONE(UDPCB,STATUS,NAMLEN,NAMPTR)
	struct UDPCB_Structure * UDPCB;
    {

// Clear pending name lookup flag

    UDPCB->udpcb$nmlook = FALSE;

// Check status

    if (! STATUS)
	return;

// Copy the hostname into the UDPCB

    UDPCB->udpcb$foreign_hnlen = NAMLEN;
    CH$MOVE(NAMLEN,NAMPTR,CH$PTR(UDPCB->udpcb$foreign_hname));
    }

//SBTTL "UDP$CLOSE - close UDP "connection""
/*
    Close a UDP "connection". Kills any receive requests that haven't
    finished yet and deallocates the UDPCB and any other data structures
    associated with a connection.
*/

void UDP$CLOSE(struct user_close_args * Uargs)
    {
      struct UDPCB_Structure * UDPCB;
    signed long
	RC;

// Check for valid UDPCB

    if ((UDPCB = UDPCB_OK(Uargs->cl$local_conn_id,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);
	return;
	};

// Use common routine for closing

    UDPCB_Close(Uargs->cl$local_conn_id,UDPCB,NET$_CC);

// Close done - post user request and free argblk

    User$Post_IO_Status(Uargs,SS$_NORMAL,0,0,0);
    MM$UArg_Free(Uargs);
    }

//SBTTL "UDP$ABORT - abort UDP "connection""
/*
    Abort a UDP "connection". Identical in functionality to UDP$CLOSE.
 */

void UDP$ABORT(struct user_abort_args * Uargs)
    {
      struct UDPCB_Structure * UDPCB;
    signed long
	RC;

// Check for valid UDPCB

    if ((UDPCB = UDPCB_OK(Uargs->ab$local_conn_id,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);
	return;
	};

// Use common routine for closing

    UDPCB_Close(Uargs->ab$local_conn_id,UDPCB,NET$_CC);

// Done. Clean up.

    User$Post_IO_Status(Uargs,SS$_NORMAL,0,0,0);
    MM$UArg_Free(Uargs);
    }

//SBTTL "UDP$SEND - send UDP packet"
/*
    Handle user send request for UDP connection. Form a UDP packet from the
    user's data buffer and hand it to IP layer for transmission.
 */

void UDP$SEND(struct user_send_args * Uargs)
    {
    signed long
	RC,
	ForeignAddr,LocalAddr,
      ForeignPort,LocalPort;
    ipadr$address_block * UAddr;
    struct UDPCB_Structure * UDPCB;

// Validate connection ID and get UDPCB pointer

    if ((UDPCB = UDPCB_OK(Uargs->se$local_conn_id,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// No such connection
	return;
	};
    XLOG$FAO(LOG$USER,"!%T UDP$SEND: Conn=!XL, UDPCB=!XL, Size=!SL, X1=!XL, X2=!XL!/",
	     0,Uargs->se$local_conn_id,UDPCB,Uargs->se$buf_size,
	     Uargs->se$ext1,Uargs->se$ext2);

// Check for aborted connection

    if (UDPCB->udpcb$aborting)
	{
	XLOG$FAO(LOG$USER,"!%T UDP$SEND for aborted UDPCB !XL!/",0,UDPCB);
	USER$Err(Uargs,NET$_CC);
	return;
	};

// Check for invalid buffer size

    if (Uargs->se$buf_size < 0)
	{
	USER$Err(Uargs,NET$_BTS);
	return;
	};

    if (Uargs->se$buf_size > Max_UDP_Data_Size)
	{
	USER$Err(Uargs,NET$_IR);
	return;
	};

// Check for "address mode" connection and set host addresses from user buffer
// in that case.

    UAddr = Uargs->se$protohdrblk;

    ForeignAddr = UAddr->ipadr$dst_host;
    if (ForeignAddr == WILD)
	ForeignAddr = UDPCB->udpcb$foreign_host;

    LocalAddr = UAddr->ipadr$src_host;
    if (LocalAddr == WILD)
    IP$SET_HOSTS(1,ForeignAddr,LocalAddr,ForeignAddr);

    LocalPort = UAddr->ipadr$src_port;
    if (LocalPort == WILD)
	LocalPort = UDPCB->udpcb$local_port;

    ForeignPort = UAddr->ipadr$dst_port;
    if (ForeignPort == WILD)
	ForeignPort = UDPCB->udpcb$foreign_port;

   if ((ForeignAddr == WILD) || (ForeignPort == WILD))
	{
	USER$Err(Uargs,NET$_NOPN);
	return;
	};

// Do common portion of the send

    RC = UDP_SEND(LocalAddr, ForeignAddr, LocalPort, ForeignPort,
		  Uargs->se$data_start, Uargs->se$buf_size );

// Post the I/O request back to the user

    User$Post_IO_Status(Uargs,RC,0,0,0);
    MM$UArg_Free(Uargs);
    }



//SBTTL "UDP_SEND - Common routine for sending UDP datagrams"

UDP_SEND ( LocalAddr, ForeignAddr, LocalPort, ForeignPort,
			  UData, Usize )
//
// Returns success or failure of IP$SEND.
//
    {
      struct udpkt_structure * Seg;
    signed long
	RC,
	bufsize,
	buf,
	Segsize;

// Allocate an output buffer and build an IP packet

    if (Usize > Max_UDP_Data_Size)
	Usize = Max_UDP_Data_Size;

// Use preallocated buffer sizes to reduce dynamic memory load

    bufsize = Usize + UDP_HEADER_SIZE + IP_HDR_BYTE_SIZE + DEVICE_HEADER;
    if (bufsize <= MIN_PHYSICAL_BUFSIZE)
      bufsize = MIN_PHYSICAL_BUFSIZE;
    else
	if (bufsize <= MAX_PHYSICAL_BUFSIZE)
	    bufsize = MAX_PHYSICAL_BUFSIZE;
    buf = MM$Seg_Get(bufsize);	// Get a buffer
    Seg = buf + DEVICE_HEADER + IP_HDR_BYTE_SIZE; // Point at UDP segment
    Segsize = Usize+UDP_HEADER_SIZE; // Length of segment + UDP header

// Set up the UDP header

    Seg->up$source_port = LocalPort;
    Seg->up$dest_port = ForeignPort;
    Seg->up$length = Segsize;
    Seg->up$checksum = 0;

// Copy the user data into the data area

#if 0
    // not yet
    $$KCALL(MOVBYT,Usize,UData,Seg->up$data);
#endif

// Log the UDP packet if desired

    if ($$LOGF(LOG$UDP))
	Log_UDP_Packet(Seg,FALSE,TRUE);

// Swap the header bytes and compute the checksum
// No longer compute the checksum as it is now done in IP (see header comments)

    SwapBytes(UDP_HEADER_SIZE/2,Seg);
//    Seg->UP$Checksum=Gen_Checksum(Segsize,Seg,LocalAddr,ForeignAddr,
//				  UDP_Protocol);

// Send the segment to IP (it will deallocate it)

    UDPIPID = UDPIPID+1;	// Increment packet ID
    RC = IP$SEND(LocalAddr,ForeignAddr,UDPTOS,UDPTTL,
		   Seg,Segsize,UDPIPID,UDPDF,TRUE,UDP_Protocol,
		   buf,bufsize);

    UDP_MIB->MIB$UDPOUTDATAGRAMS = UDP_MIB->MIB$UDPOUTDATAGRAMS + 1;

    // Return an appropriate code.
    if (RC == 0)
      return NET$_NRT;
	  else return SS$_NORMAL;
    }



//SBTTL "UDP$RECEIVE - receive a UDP packet"
/*
    Handle user receive request for UDP connection. If there is a packet
    available on the UDP receive queue, then deliver it to the user
    immediately. Otherwise, queue up the user receive for later.
 */

void UDP$RECEIVE(struct user_recv_args * Uargs)
    {
    signed long
	RC;
    struct UDPCB_Structure * UDPCB;
    struct queue_blk_structure(qb_nr_fields) * QB;
    struct queue_blk_structure(qb_ur_fields) * URQ;

// Validate connection ID and get UDPCB pointer

    if ((UDPCB = UDPCB_OK(Uargs->re$local_conn_id,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// No such connection
	return;
	};
    XLOG$FAO(LOG$USER,"!%T UDP$RECEIVE: Conn=!XL, UDPCB=!XL, Size=!SL!/",
	     0,Uargs->re$local_conn_id,UDPCB,Uargs->re$buf_size);

// Check for aborted connection

    if (UDPCB->udpcb$aborting)
	{
	XLOG$FAO(LOG$USER,"!%T UDP$RECEIVE for aborted UDPCB !XL!/",0,UDPCB);
	USER$Err(Uargs,NET$_CC);
	return;
	};

// Check for invalid buffer size

    if (Uargs->re$buf_size <= 0)
	{
	USER$Err(Uargs,NET$_BTS);
	return;
	};

    if (Uargs->re$buf_size > Max_UDP_Data_Size)
	{
	USER$Err(Uargs,NET$_IR);
	return;
	};

// Make a request block for the receive

    URQ = MM$QBLK_Get();		// Get a queue block
    URQ->ur$size = Uargs->re$buf_size; // # of bytes this rq can take
    URQ->ur$data = Uargs->re$data_start; // Address of system buffer
    URQ->ur$irp_adrs = Uargs->re$irp_adrs; // IO request packet address
    URQ->ur$ucb_adrs = Uargs->re$ucb_adrs; // Unit Control Block address
    URQ->ur$uargs = Uargs;	// User argument block address

// If anything is available on the queue, deliver it now, else queue for later

    NOINT;
    if (REMQUE(UDPCB->udpcb$nr_qhead,QB) != EMPTY_QUEUE) // check
      Deliver_UDP_Data(UDPCB,QB,URQ);
    else
	INSQUE(URQ,UDPCB->udpcb$usr_qtail);
    OKINT;
    }



//SBTTL "UDP$INFO - get info about UDP "connection""
/*
    Read the host names/numbers and the ports for a UDP connection.
 */

void UDP$INFO(struct user_info_args * Uargs)
    {
	USER$Net_Connection_Info ();
	struct UDPCB_Structure * UDPCB;
    signed long
	RC;

// Validate the connection ID

    if ((UDPCB = UDPCB_OK(Uargs->if$local_conn_id,RC,Uargs)) == 0)
	{
	USER$Err(Uargs,RC);	// Bad connection ID
	return;
	};

// Give the information back (common TCP/UDP routine in USER.BLI)

    USER$Net_Connection_Info(Uargs,UDPCB->udpcb$local_host,UDPCB->udpcb$foreign_host,
			UDPCB->udpcb$local_port,UDPCB->udpcb$foreign_port,
			UDPCB->udpcb$foreign_hname,UDPCB->udpcb$foreign_hnlen);
    }


//SBTTL "UDP$STATUS - get status of UDP "connection""
/*
    This routine is a placeholder for the network STATUS command, which is
    currently implemented for the UDP protocol.
 */

void UDP$STATUS(struct user_status_args * Uargs)
    {
    USER$Err(Uargs,NET$_NYI);
    }

//SBTTL "UDP$CANCEL - Handle VMS cancel for UDP connection"
/*
    Handle process abort/$CANCEL request for a UDP connection. Identical
    in functionality to UDP$CLOSE/UDP$ABORT except for calling procedure.
 */

UDP$CANCEL(struct vms$cancel_args * Uargs)
    {
      struct UDPCB_Structure * UDPCB;
      signed long I,
	Fcount;

    Fcount = 0;

// Check all valid UDPCB's looking for a match on pid and channel #.

    for (I=1;I<=MAX_UDPCB;I++)
	if ((UDPCB = UDPCB_TABLE[I]) != 0)
	    {

// If the process doing the cancel owns this connection, then delete it.

	    if ((UDPCB->udpcb$user_id == Uargs->vc$pid) &&
		(UDPCB->udpcb$piochan == Uargs->vc$piochan))
		{
		XLOG$FAO(LOG$USER,"!%T UDP$Cancel: UDPCB=!XL!/",0,UDPCB);
		UDPCB_Close(I,UDPCB,NET$_CCAN);
		Fcount = Fcount + 1;
		};
	    };
    return Fcount;
    }

//SBTTL "UDP dump routines"

void UDP$Connection_List(RB)
//
// Dump out the list of UDP connections.
//
	 D$UDP_LIST_RETURN_BLK RB;
    {
      signed long I,
	RBIX;
    RBIX = 1;
    for (I=1;I<=MAX_UDPCB-1;I++)
	if (UDPCB_TABLE[I] != 0)
	    {
	    RB[RBIX] = I;
	    RBIX = RBIX + 1;
	    };
    RB[0] = RBIX - 1;
    }

UDP$UDPCB_DUMP(UDPCBIX,RB)
//
// Dump out a single UDP connection
//
	d$udpcb_dump_return_blk * RB;
    {
      struct UDPCB_Structure * UDPCB;
      struct queue_blk_structure(qb_nr_fields) * QB;
    signed long
	Qcount;

// Validate that there is a real UDPCB there

    if ((UDPCBIX < 1) || (UDPCBIX > MAX_UDPCB) ||
	((UDPCB = UDPCB_TABLE[UDPCBIX]) == 0))
	return FALSE;

// Copy the UDPCB contents

    RB->du$udpcb_address = UDPCB;
    RB->du$udpcb_foreign_host = UDPCB->udpcb$foreign_host;
    RB->du$udpcb_foreign_port = UDPCB->udpcb$foreign_port;
    RB->du$udpcb_local_host = UDPCB->udpcb$local_host;
    RB->du$udpcb_local_port = UDPCB->udpcb$local_port;
    RB->du$udpcb_flags = UDPCB->udpcb$flags;
    RB->du$udpcb_user_id = UDPCB->udpcb$user_id;

// Get length of network receive queue

    QB = UDPCB->udpcb$nr_qhead;
    Qcount = 0;
    while ((QB != UDPCB->udpcb$nr_qhead))
	{
	Qcount = Qcount + 1;
	QB = QB->nr$next;
	};
    RB->du$udpcb_nr_qcount = Qcount;

// Get length of user receive queue

    QB = UDPCB->udpcb$usr_qhead;
    Qcount = 0;
    while ((QB != UDPCB->udpcb$usr_qhead))
	{
	Qcount = Qcount + 1;
	QB = QB->nr$next; // was: UR$NEXT, but the same plade
	};
    RB->du$udpcb_ur_qcount = Qcount;

// Done.

    return TRUE;
    }
