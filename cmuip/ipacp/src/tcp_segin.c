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
//TITLE "TCP_SEGIN - Process TCP network input segments"
//SBTTL "Network Segment Arrival Overview"
/*

Module:

	TCP_SEGIN - TCP input event processor.

Facility:

	Process the newly arrived network segments.

Abstract:

	Network segments are placed on the segment-input queue by IP after
	the inter-network protocols have been removed.  TCP reads the queue
	& processes each segment.  Arrival of segments drives the connection
	states along with user requests.

Author:

	Original version by Stan C. Smith, Fall 1981
	This version by	Vince Fuller, CMU-CSD, Summer, 1987
	Copyright (c) 1986, 1987, Vince Fuller and Carnegie-Mellon University

Modification History:

*** Begin USBR change log ***

6.7	05-Dec-1991	Henry W. Miller		USBR
	Use TCB->SND_ACK_THRESHOLD instead of ACK_THRESHOLD.
	Set TCB->SND_ACK_Threshold from TCB->Max_SEG_Data_Size.  Sort of
	RFC1122 compliant.

6.6	14-Nov-1991	Henry W. Miller		USBR
	In SEG$Process_Received_Segments(), log various types of currently
	unsupported ICMP segments.
	In Decode_Segment(), let TELNET module ACK it's own data, clean up
	logging.
	Change arithmetic and printout on timers from signed to unsigned.
	Call TCP$Enqueue_ACK() rather than just setting P}ING_ACK or
	calling TCP$Send_ACK() directly.

6.5f	30-Aug-1991	Henry W. Miller		USBR
	In DECODE_SEGMENT(), queue user data if RCV_Q_COUNT is greater then
	zero, not just if it"s big enough to fill user"s buffer.
	This fixes the long outstanding dreaded TELNET pause syndrome.
	(At least here it does...)

6.5e	11-Mar-1991	Henry W. Miller		USBR
	Fix port comparison logic.

6.5d	27-Feb-1991	Henry W. Miller		USBR
	Fix port comparison logic.

6.5c	24-Jan-1991	Henry W. Miller		USBR
	Fixed logging statements to print timestamp.

6.5b	15-Jan-1991	Henry W. Miller		USBR
	Changed SND_WND, RCV_WND, and Old_RCV_WND to unsigned long values
	as recommended by RFC1122.

6.5a	13-Jan-1991	Henry W. Miller		USBR
	In SEG$Log_Segment(), print checksum, data pointer and urgent
	pointer as unsigned word values.
	Updated IDENT to 6.5a.

*** Begin CMU change log ***

6.4	16-MAR-1989	Dale Moore	CMU-CS
	If we aren"t doing Keep alives, don"t timeout connections.

	06-JUN-1988	Dale Moore	CMU-CS/RI
	Check to see if we are truncated the data received before
	we process FIN bit.  We might truncate the data if the
	window size is smal.

6.3	14-JAN-1988	Dale Moore	CMU-CS/RI
	In Process_Received_Segments:  On Duplicate SYN, drop packet
	rather than RST connection.

6.2  23-Nov-87, Edit by VAF
	In FORK_SERVER: re-initialize length of PROCNAME descriptor each time
	through loop. Failing to do so restricts the process name string length
	to be whatever the first attempt sets it to (thanks to Mike Iglesias
	at UCI).

6.1  18-Nov-87, Edit by VAF
	Flush connections if Send_SYN$ACK fails for incoming connection.
	This shouldn't happen unless the systems is misconfigured.

6.0   4-Aug-87, Edit by VAF
	Add hooks for internal TELNET Virtual Terminal support.
	Rewrite options processing to be more general.

5.9  21-Jul-87, Edit by VAF
	Clean up the way circular queues are handled a bit.
	Rearrange some code in PROCESS_RECEIVED_SEGMENT to facilitate adding of
	special processing for incoming TELNET.
	Reorder incoming segment handling code for easier reading.

5.8  23-Mar-87, Edit by VAF
	Know about dynamic buffer sizes.
	Log setting of maximum segment size.

5.7   3-Mar-87, Edit by VAF
	Flush I/O request tags. Change calling sequence of User$Post_IO_Status.

5.6  27-Feb-87, Edit by VAF
    	If in FIN-WAIT-2 and there is data pending to be read, don't close the
	connection yet - set FIN_RCVD flag for receive routine to check later.

5.5  18-Feb-87, Edit by VAF
	From Ed Cetron - update send window when SYN-RECV connection becomes
	established. This fixes a problem with hosts that open connections
	with an initial zero receive window.

5.4   9-Feb-87, Edit by VAF
	Make server priviliges be set from the config file.

5.3   4-Feb-87, Edit by VAF
	Give forked servers PRV$V_PHY_IO so they can open priviliged ports.

5.2  10-Dec-86, Edit by VAF
	Change order of arguments to Gen_Checksum.
	Code to wraparound RX pointer was wrong - fix it.

5.1  12-Nov-86, Edit by VAF
	Move WKS data structure here. Make it configured at runtime.
	Flush SYN wait list queue - incorporate it into the WKS structure.

5.0  11-Nov-86, Edit by VAF
	Remove RF_QMAX literal, replace with FQ_MAX variable set by
	configuration routine. Allow up to 30 of each type of forked server.
	Also, only log server creation when it succeedes.
	Rewrite portions of the SYN-wait-list handling routines to get rid
	of silly PASCAL syntax.

4.9   3-Nov-86, Edit by VAF
	Use reasonable coding (i.e. define some literals) in the SRTT
	calculation. Make minor change to calculation to conform with the
	suggested calculation in RFC793.

4.8   1-Oct-86, Edit by VAF
	Set TCB->NMLOOK_Flag When we start an address to name lookup.

4.7  30-Sep-86, Edit by VAF
	Up RF_QMAX from 5 to 8.
	Fix SEG$Purge_RT_Queue - make it clear the retransmission count, flags,
	and update the pointer.

4.6  22-Sep-86, Edit by VAF
	When FIN arrives and we go into CLOSE-WAIT state, force a close if the
	TCB has been aborted (i.e. owning process has exited). In this case,
	the user"s close was done and posted long ago, so it won"t happen
	again.

4.5  12-Sep-86, Edit by VAF
	When we fill-in an unspecified foreign host, also start an name lookup
	to fill-in the foreign host name.

4.4  13-Aug-86, Edit by VAF
	Set TCB->IS_SYNCHED when TCB becomes synchronized. BUILD_HEADER now
	uses this flag to determine if it should set the ACK bit.

4.3   6-Aug-86, Edit by VAF
	Redo retransmission handling to use circular queue.

4.2   5-Aug-86, Edit by VAF
	Redo segment input to use circular queue.

4.1  25-Jul-86, Edit by VAF
	Restrict number of segments on TCB NR queue.

4.0  22-Jul-86, Edit by VAF
	Clean up RST processing a little.
	Really drop SYN data - old code caused segments not to be deallocated.
	Change duplicate SYN processing to cause resets for all states.
	Don't set return value based on UOFFSET - actually set RETCODE to
	false when we deliver some data to the user.
	Separate ICMP logging out with LOG$ICMP flag.

3.9  18-Jul-86, Edit by VAF
	In Queue_Network_Data - drop segments if TCB is aborted.
	In Decode_Segment - set Ucount to 0 if text is not usable (i.e.	not in
	Established, Fin-wait-1 or Fin-wait-2) so seg will be dropped.

3.8  17-Jul-86, Edit by VAF
	Debugging code for tracking incoming segments.

3.7  15-Jul-86, Edit by VAF
	Send out an ACK when we put something on the future queue. Idea is to
	let the bozo know that he is ahead of where he should be.

3.6  15-Jul-86, Edit by VAF
	Correctly handle case where QUEUE_NETWORK_DATA cannot place the segment
	in its sequence ordering. Old code would leave the segment in limbo. We
	fixed it to at least drop the segment. Also, count and log these bad
	(strange?) segments - this really shouldn't be happening.
	Change some LOG$TCPs to use new flag - LOG$TCPERR.
	Rearrange change log.

3.5  11-Jul-86, Edit by VAF
	Limit number of segments on future queue to 5.

3.4   9-Jul-86, Edit by VAF
	Revise sequence number checks. Should handle all cases where sequence
	numbers wrap around from positive to negative.	

3.3  07-JUL-86, Edit by Dale Moore
	Change to include CS$Last_Ack in some of the same SELECT states as
	CS$Closing.  

3.2  30-Jun-86, Edit by VAF
	Implement future segments queue. This queue contains all segments
	received that are in the window but are beyond the left edge of
	the window (and thus are not immediately usable).

3.1  25-Jun-86, Edit by VAF
	Don't use index 0 of VALID_TCB table.

3.0  11-Jun-86, Edit by VAF
	Make ACK_RT_QUEUE call S}_DATA if it actually removes anything from
	the RT queue. Idea is to make S} processing event driven.
	Fix receive sequence number check to agree with RFC.
	(We were sometimes getting undetected duplicates)

2.9  10-Jun-86, Edit by VAF
	When taking input segments, don't subtract SYN/FIN sequence space from
	starting data offset (since they don't really exist).
	Count up incoming segments and data octets in TCB.
	If incoming data exceeds size of 1st element on user receive queue,
	deliver data immediately in Decode_Segment.

2.8  22-May-86, Edit by VAF
	Use VMS error message facility.

2.7   8-May-86, Edit by VAF
	Post user CLOSE only when connection goes into Time_Wait state.
	In CS$Closing state, when ACK of FIN received, go to Time_Wait state
	(old code deleted the TCB, which violates the spec).
	Add LAST_ACK state.

2.6   2-May-86, Edit by VAF
	Fix SEG$Log_Segment data/option printing for byteswapped packets.

2.5  29-Apr-86, Edit by VAF
	Debugging code in TCP ICMP receive.

2.4  21-Apr-86, Edit by VAF
	Phase II of flushing XPORT - use $FAO for formatting output.

2.3  18-Apr-86, Edit by VAF
	Teach SEG$Log_Segment about byteswapping for some packets.

2.2  10-Apr-86, Edit by VAF
	Give sequence numbers, port numbers, etc. in both hex and decimal
	when writing log entries.
	Fix bug where SEQcount not initialized in DECODE_SEGMENT.

2.1   8-Apr-86, Edit by VAF
	Add LOG$TCBSTATE logging flag.
	Minor changes to logging.
	SET_TCB_STATE and INACTIVATE_TCB are now routines, not macros.

2.0   7-Apr-86, Edit by VAF
	First pass at more general logging code.

1.9   2-Apr-86, Edit by VAF
	Move some code to the USER module that belongs there.

1.8  25-Mar-86, Edit by VAF
	Support for ICMP at the TCP level.
	Reorganize segment input handler to make this possible.

1.7  24-Mar-86, Edit by VAF
	Move TCP input AST routine to this module for consistancy.
	Various changes between 1.6 and this, including fixing of problems
	with sequence number checks.

1.6  10-Mar-86, Edit by VAF
	Valid sequence number check was wrong. Accept segments with sequence
	numbers before RCV_NXT if they contain sequence numbers beyond RCV_NXT.
	**N.B. TCB[DASM_*] crap should be flushed.

1.5   7-Mar-86, Edit by VAF
	Phase I of flushing XPORT - get rid of "LOGGER".

1.4  21-Feb-86, Edit by VAF
	Flush "known_hosts" junk.
	Some work should be done to check local host # for incoming segments.

*** End CMU change log ***

1.1  [10-1-83] stan smith
	Original version.

1.2  [7-15-83] stan
	force byte-size on some external literals.

1.3  [5-30-85] noelan olson
	Set index into known host table before sending ACK SYN so that
	the proper internet address will be used to calculate checksum.

*/

//SBTTL "Module Definition"

MODULE SEGIN(IDENT="6.7",LANGUAGE(BLISS32),
	     ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			     NONEXTERNAL=LONG_RELATIVE),
	     LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	     OPTIMIZE,OPTLEVEL=3,ZIP)=
{

#include "SYS$LIBRARY:STARLET";	// VMS system definitions

#include "CMUIP_SRC:[CENTRAL]NETXPORT";	// BLISS transportablity package
#include "CMUIP_SRC:[CENTRAL]NETERROR";	// Network error codes
#include "CMUIP_SRC:[CENTRAL]NETVMS";	// VMS specifics
#include "CMUIP_SRC:[CENTRAL]NETCOMMON";// Network common defs
#include "CMUIP_SRC:[CENTRAL]NETTCPIP";	// TCP/IP protocols

#include "STRUCTURE";		// TCB & Segment Structure definitions
#include "TCPMACROS";		// Local macros
#include "TCP";			// TCP related definitions
#include "SNMP";			// Simple Network Management Protocol

!XQDEFINE;

extern signed long
    TCP_MIB : TCP_MIB_struct;   // TCP management Information Block

extern
    LIB$GET_VM : ADDRESSING_MODE(GENERAL),

// USER.BLI

 void    User$Post_IO_Status,

// TCP_USER.BLI

 void    TCP$Post_Active_Open_IO_Status,
 void    TCP$Post_User_Close_IO_Status,
 VOID    TCP$KILL_P}ING_REQUESTS,
 void    TCP$Deliver_User_Data,
    Gen_CheckSum,
 void    TCP$Adlook_Done,

// TCP_TELNET.BLI

    TELNET_CREATE,
    TELNET_OPEN,
 VOID    TELNET_INPUT,
 VOID    TELNET_OUTPUT,

// MEMGR.BLI

    TCB$Create,
 void    TCB$Delete,
 void    MM$Seg_Free,
 void    MM$QBLK_Free,
    MM$QBLK_Get,
 void    MM$UArg_Free,

// TCP.BLI

 void    TCP$Send_Data,
    TCP$Send_Ctl,
 void    TCP$Enqueue_Ack,
 void    TCP$Send_Ack,
 void    TCP$Dump_TCB,
 void    TCP$Inactivate_TCB,
 void    TCP$Set_TCB_State,
 void    CQ_Enqueue,
    TCP$TCB_CLOSE,
    TCP$Compute_RTT,

// NMLOOK.BLI

 VOID    NML$GETNAME,

// MACLIB.MAR

    Time_Stamp,
 void    SwapBytes,

// IOUTIL.BLI

 void    ASCII_Hex_Bytes,
 void    ASCII_Dec_Bytes,
 void    Log_Time_Stamp,
 VOID    OPR_FAO,
 VOID    LOG_FAO,
 VOID    LOG_OUTPUT,
 VOID    ACT_FAO,
 VOID    ACT_OUTPUT;

extern signed long
    INTDF,
    AST_IN_PROGRESS,
    CONN_TIMEVAL	:	UNSIGNED,
    MYUIC,			// TCP's UIC.
    Log_State,
    SegIN : Queue_Header_Structure(SI_Fields),
//    struct Connection_table_Structure * ConectPtr VOLATILE,

    FQ_MAX,			// Max segments allowed on future queue.
    MAX_RECV_DATASIZE,		// Configured Maximum Segment Size
    TELNET_SERVICE,		// Flag if internal TELNET server is enabled
    Keep_Alive,			// Flag if Keep alives used

// tcp stats

    TS$SEG_BAD_CKSUM,		// bad segment checksum
    TS$SERVERS_FORKED,		// # of well-known servers forked.
    TS$ACO,			// Active opens that were established.
    TS$DUPLICATE_SEGS,		// total duplicate segments received.
    TS$OORW_SEGS,		// Total Out Of Recv Window segs received.
    TS$FUTURE_RCVD,		// # received in window but after RCV.NXT
    TS$FUTURE_DUPS,		// # of future duplicates dropped
    TS$FUTURE_USED,		// # of such used
    TS$FUTURE_DROPPED,		// # of such dropped
    TS$BADSEQ,			// Packets dropped in Queue_Network_data
    TS$ABORT_DROPS,		// packets dropped because TCB aborted
    TS$QFULL_DROPS,		// packets dropped because TCB NR queue full
    TS$SR;			// segments recevied.			

// Forward declarations

Forward Routine
 void    SEG$Log_Segment;

LITERAL
	GLOBAL_MINSRV = 1,
	GLOBAL_MAXSRV = 30;

//SBTTL "Declaration of WKS server process table & SYN-wait list"

signed long
    WKS_COUNT  = 0,
    WKS_LIST : WKS_Structure,
    SYN_WAIT_COUNT  = 0;

//SBTTL "ACK-RT-Queue: Check if "ACK" segment acks any segments."
/*

Function:

	Check if "ACK" segment acks any segments currently on the retransmission
	queue. If so then delete the segment from the queue.

Inputs:

	TCB = TCB pointer.
	ACKNUM = All sequence #"s  < acknum are ack"ed.

Implicit Inputs:

	TCB->RT_Qhead & TCB->RT_Qtail form the re-transmission queue header.

Outputs:

	None.

Side Effects:

	Re-transmission queue elements may be deleted if ACK'ed.

*/

LITERAL
    ALPHA = 90,			// Smoothing constant, per RFC973, pg 41
    BETA = 150,			// Delay variance factor
    ALPHACOMP = (100 - ALPHA),	// "Compliment" of ALPHA (i.e. 1 - ALPHA)
    MINRTT = 1,			// Minimum reasonable RTT
    MINSRTT = 10;		// Minimum smoothed RTT

void ACK_RT_Queue(struct tcb_structure * tcb,AckNum) (void)
    {
    signed long
	Delta: UNSIGNED,
	crtt : UNSIGNED,
	Tmp,
	deqc,
	oldcount;

    oldcount = TCB->SRX_Q_Count;
    deqc = AckNum - TCB->RX_SEQ;
    XLOG$FAO(LOG$TCP,%STRING(
	     "!%T ACK-RXQ: TCB=!XL, SEQ=!XL, PTR=!XL, CNT=!SL!/",
	     "!%T ACK-RXQ: CTL=!SL, ACK=!XL, DEQC=!SL!/"),
	     0,TCB,TCB->RX_SEQ,TCB->SRX_Q_DEQP,TCB->SRX_Q_Count,
	     0,TCB->RX_CTL,AckNum,DEQC);

// If the ACK number is greater than the RX sequence, then we are acknowleging
// at least part of the RX space.

    if (deqc > 0)
	{
	signed long
	    oldrtt	:	UNSIGNED,
	    newc,
	    oldcount;

// Clear SYN flag if we acked anything, since it is the first sequence number
// Also decrement number of octets we acked - SYN takes up one sequence number.

	TCB->RX_SEQ = TCB->RX_SEQ + deqc;
	if (TCB->RX_CTL != 0)
	    if ((TCB->RX_CTL == M$SYN) || (TCB->RX_CTL == M$SYN_ACK))
		{
		TCB->RX_CTL = 0;
		deqc = deqc - 1;
		};

// Clear retransmission count

	TCB->RX_Count = 0;

// Calculate number of bytes removed from RX data queue.
// newc LSS 0 SHOULD mean that a FIN has been acked.

	newc = TCB->SRX_Q_Count - deqc;
	if (newc LSS 0)
	    {
	    if (TCB->RX_CTL == M$FIN)
		TCB->RX_CTL = 0
	    else
		XLOG$FAO(LOG$TCPERR,"!%T ?RX-ACK - newc = !SL!/",0,newc);
	    newc = 0;
	    };

// Update RX queue count and pointer.

	TCB->SRX_Q_Count = newc;
	if (newc == 0)
	    TCB->SRX_Q_DEQP = TCB->SND_Q_DEQP
	else
	    {
	    signed long
		newptr;
	    newptr = TCB->SRX_Q_DEQP + deqc;
	    if (newptr > TCB[SRX_Q_}])
		newptr = TCB->SRX_Q_BASE + (newptr - TCB[SRX_Q_}]);
	    TCB->SRX_Q_DEQP = newptr;
	    };

// Compute round trip time for adaptive retransmission.

	TCP$Compute_RTT(TCB) ;
!	oldrtt = TCB->round_trip_time;
!	crtt = MAXU(Time_Stamp()-TCB->Xmit_Start_Time,MINRTT);

// Compute smoothed round trip time, see RFC793 (TCP) page 41 for details.

!	delta = ((TCB->round_trip_time*ALPHA)/100) +
!		((crtt*ALPHACOMP)/100);
!	TCB->round_trip_time = delta;
!	delta = (BETA*.delta)/100;
!	TCB->calculated_rto = MINU(MAX_RT_TIMEOUT,MAXU(delta,MIN_RT_TIMEOUT));

!	XLOG$FAO(LOG$TCP,
!	    "!%T ACK-RXQ: Prev SRTT=!UL, Cur RTT=!UL, New SRTT=!UL, RTO=!UL!/",
!		 0,oldrtt,crtt,TCB->round_trip_time,TCB->calculated_rto);
	};

// If TVT and send queue just became non-full, try to get some more TVT data

    if (TCB->IS_TVT)
	IF (oldcount + TCB->SND_Q_Count) GEQ TCB->SND_Q_Size AND
	   (oldcount > TCB->SRX_Q_Count) THEN
	    {
	    TELNET_OUTPUT(TCB);
	    };

// If the RT queue is now empty, see if data needs to be sent.
// Since we implement the "Nagle" algorithm, the data send routine is
// effectively blocked by anything on the RT queue.

    if (TCB->SRX_Q_Count == 0)
	TCP$Send_Data(TCB);
    };

SEG$Purge_RT_Queue(struct tcb_structure * tcb): NOvalue=
!
// Routine to clean out the retransmission queue. All this really involves
// is clearing the RX count and flags and advancing the RX pointer to the
// send dequeue pointer.
!
    {
    TCB->SRX_Q_Count = 0;	// Clear retransmit byte count
    TCB->SRX_Q_DEQP = TCB->SND_Q_DEQP; // Empty retransmit queue
    TCB->RX_CTL = 0;		// And clear retransmit flags
    };

//SBTTL "Send-Reset: RESET a Known connection"
/*

Function:

	Send a "RESET" segment in response to receiving strange or unexpected
	segments.

Inputs:

	TCB = TCB pointer.
	SeqNum = Number to use as the segments sequence number.

Outputs:

	None.

Side Effects:

	"RESET" segment is constructed & delivered to IP for network transmission.

*/


Send_Reset(struct tcb_structure * tcb,SEQNum): NOvalue=
    {
    signed long
	Sav$Snd_Nxt;

// save send next sequence # & use SEQnum instead.

    Sav$Snd_Nxt = TCB->Snd_Nxt;
    TCB->Snd_Nxt = SEQNum;

// Build the "RESET" segment & give it to IP for transmission.

    Send_RST(TCB);

// Restore the world to its proper state.

    TCB->Snd_Nxt = Sav$Snd_Nxt;
    };

//SBTTL "Reset-Unknown-Connection"
/*

Function:

	Send a "RST" reset segment in reply to receiving a segment for
	a unknown connection that was NOT a well-known local port.

Inputs:

	SEG - points at offending segment.
	QB - points at it's queue block, Network receive queue (QB_NR_Fields).

Outputs:

	None.

Side Effects:

	"RST" segment is built & sent.  A FAKE TCB must be created &
	partially initialized for the control-segment send rtns.  After
	the RESET has been sent, delete the fake TCB.
*/


RESET_UNKNOWN_CONNECTION(SEG,QB): NOVALUE (void)
    {
    MAP
	struct Segment_Structure * Seg,
	struct Queue_Blk_Structure * QB(QB_NR_Fields);
    REGISTER
	struct tcb_structure * tcb;

// Create & fill in the temporary TCB

    TCB = TCB$Create();
    TCB->LP_Next = TCB->LP_Back = TCB->LP_Next;	// Init Local Port queue.
    TCB->Local_Port = Seg->SH$Dest_Port && %X"FFFF" ;
    TCB->Local_Host = QB->NR$Dest_Adrs;
    TCB->Foreign_Port = Seg->SH$Source_Port && %X"FFFF" ;
    TCB->Foreign_Host = QB->NR$Src_Adrs;
    TCB->State = CS$Established;
    TCB->Rcv_Wnd = TCB->Snd_Wnd = TCB->Rcv_Nxt = 100;

// If the ACK bit (received seg) is TRUE, then <SEQ=SEG_ACK><CTL=RST>
// else  <SEQ=0><ACK=SEG_SEQ+SEG_SEQ_SPACE><CTL=RST,ACK>

    if (Seg->SH$C_ACK)
	TCB->Snd_Nxt = Seg->SH$ACK
    else
	{
	TCB->Snd_Nxt = 0;
	TCB->Rcv_Nxt = Seg->SH$SEQ + (QB->NR$Size - Seg->SH$Data_Offset*4)
				    + seg->sh$c_syn + seg->sh$c_fin;
	};

// Send the reset.

	XLOG$FAO(LOG$TCPERR,
		 "!%T RESET for unknown conn, ACK=!XL (!UL), SEQ=!XL (!UL)!/",
		 0,Seg->SH$SEQ,Seg->SH$SEQ,Seg->SH$ACK,Seg->SH$ACK);

    Send_RST(TCB);

// Delete the TCB

    TCB$Delete(TCB);
    };

//SBTTL "Queue-Network-Data"
/*

Function:

	Queue sequenced data to the connection TCB (NR_Qhead).  Insert
	the queue block in the queue while maintaining sequence space
	ordering for the data.

Inputs:

	TCB = TCB pointer.
	QB = Queue block for current data bearing segment (QB_NR_Fields).

Outputs:

	Boolean indicating whether segment should be deallocated.

Side Effects:

	Segment is place in sequence space order.
	Connection Receive window is decremented by the data size of this
	segment.
*/


QUEUE_NETWORK_DATA(TCB,QB)
    {
    MAP
	struct tcb_structure * tcb,
	struct Queue_BLK_Structure * QB(QB_NR_Fields);
    REGISTER
	Ucount,
	Uptr;
    LITERAL
	NR_Qmax = 8;		// Max segs on receive queue per connection

// Is the TCB aborted? If so, just toss the data.

    if (TCB->IS_Aborted)
	{
	XLOG$FAO(LOG$TCPERR,
		 "!%T QND: TCB aborted, drop QB !XL, seg !XL, SEQ !XL/!XL!/",
		 0,QB,QB->NR$Buf,QB->NR$SEQ_Start,QB->NR$SEQ_End);
	ts$abort_drops = ts$abort_drops+1;
	return ERROR;		// Deallocate this segment
	};

// Set push flag and pointer if the segment has it on

    if (((QB->NR$EOL) || (TCB->RCV_WND LEQU TCB->SND_ACK_THRESHOLD)))
	{
	TCB->RCV_Push_Flag = TRUE;
	TCB->RCV_PPtr = QB->NR$SEQ_End;
	XLOG$FAO(LOG$TCP,
	    "!%T QND: EOL set, Q cnt !UW !/", 0, TCB->RCV_Q_Count);
	};

// Enqueue the data onto the receive circular queue

    Ucount = QB->NR$Ucount;	// Count of user data in segment
    Uptr = QB->NR$Uptr;	// Address of data
    Ucount = MIN(Ucount,TCB->RCV_Q_Size-TCB->RCV_Q_Count);
    if (Ucount <= 0)
	{
	XLOG$FAO(LOG$TCPERR,
		"!%T QND: TCB rcv q full, drop QB !XL, seg !XL, SEQ !XL/!XL!/",
		0,QB,QB->NR$Buf,QB->NR$SEQ_Start,QB->NR$SEQ_End);
	ts$qfull_drops = ts$qfull_drops+1;
	return ERROR;		// Deallocate this segment
	};

// Append the segment data to the queue and deallocate it

    XLOG$FAO(LOG$TCP,
	     "!%T QND: ENQ !SL from !XL,EQ=!XL,DQ=!XL,RCQ=!XL/!XL!/",
	     0,Ucount,Uptr,TCB->RCV_Q_ENQP,TCB->RCV_Q_DEQP,
	     TCB->RCV_Q_BASE,TCB[RCV_Q_}]);
    CQ_Enqueue(TCB->RCV_Q_Queue,Uptr,Ucount);
    XLOG$FAO(LOG$TCP,
	"!%T QND: Q cnt !UW !/", 0, TCB->RCV_Q_Count);

// Adjust the receive window to reflect the resources used by the data bearing
// portion of this segment.

    if ((TCB->Rcv_Wnd < QB->NR$SEQ_count))
	{
	XLOG$FAO(LOG$TCPERR,"!%T Segin(queue_net_data) RCV_WND (!UL) < SEQ_CNT (!UL)!/",
	    0, TCB->RCV_WND, QB->NR$SEQ_count);
	TCB->Rcv_Wnd = 0 ;
	}
    else
	{
	TCB->Rcv_Wnd = TCB->Rcv_Wnd - QB->NR$SEQ_count;
	} ;

    return TRUE;
    };

//SBTTL "Handle TCP segment input"
/******************************************************************************

Function:

	Queue a network segment to TCP.  Place the segment portion of a
	datagram on TCP's segment input queue.
	Called at AST level from IP input handling.

Inputs:

        Src$Adrs = Source internet address
        Dest$Adrs = Destinatin internet address
        BufSize = size of buffer containing segment (for seg$free)
        Buf = address of buffer containing segment   (for seg$free)
	SegSize = size in bytes of the segment.
        Seg = address of TCP segment

Outputs:

	None.

*******************************************************************************
*/

SEG$Input(Src$Adrs,Dest$Adrs,BufSize,Buf,SegSize,Seg): NOVALUE (void)
    {
    signed long
	struct Queue_Blk_Structure * QB(QB_NR_Fields);

    QB = MM$QBLK_Get();
    QB->NR$Buf_Size = BufSize;	// Total size of network buffer.
    QB->NR$Buf = Buf;		// Start adrs of network buffer.
    QB->NR$Size = SegSize;	// byte size of segment within network buffer.
    QB->NR$Seg = Seg;		// Start adrs of segment within net buf.
    QB->NR$SRC_Adrs = SRC$Adrs;// Source internet adrs.
    QB->NR$Dest_Adrs = Dest$Adrs; // destination internet adrs.
    QB->NR$Flags = 0;		// Clear input flags
    INSQUE(QB,Segin->SI_Qtail); // queue to tail of segment in queue.
!~~~XINSQUE(QB,Segin->SI_Qtail,TCP_Input,Q$SEGIN,Segin->SI_Qhead);

    TCP_MIB->MIB$tcpInSegs = TCP_MIB->MIB$tcpInSegs + 1;
    };

//SBTTL "ICMP handler for TCP"
/*
    Handle ICMP conditions that were generated by TCP segments that we
    sent out. Called at AST level from ICMP protocol handler whenever
    one of the following types of ICMP message is received:

	ICM_DUNREACH - Destination Unreachable.  Should cause connection
	to be aborted.

	ICM_SQUENCH - Source Quench.  Should cause us to back off on
	transmission rate.

	ICM_REDIRECT - Redirect.  Maybe cause us to switch to a new first-hop
	destination for packets (currently, IP makes all of these descisions,
	but we could reduce routing costs by remembering redirects here).

	ICM_TEXCEED - Time Limit Exceeded.  Probably should cause any Open
	in progress to be aborted.

	ICM_PPROBLEM - Parameter Problem.  Check to see if it was a TCP option
	that caused the problem.  If so, there is a bug somewhere and TCP
	should probably crash.

    Makes a dummy "segment" containing relavent ICMP info for processing by
     normal segment input handler.
*/

SEG$ICMP(ICMtype,ICMex,IPsrc,IPdst,Seg,Segsize,
			buf,bufsize) : NOVALUE (void)

!ICMtype - ICMP packet type
!ICMex - Extra data from ICMP packet (pointer for ICM_PPROBLEM)
!IPsrc - Source address of offending packet
!IPdst - Destination address of offending packet
!Seg - First 64-bits of data from offending packet (will have ports)
!Segsize - Calculated octet count of Seg
!Buf - address of network buffer, released by TCP later
!Bufsize - size of network buffer

    {
    signed long
	struct Queue_Blk_Structure * QB(QB_NR_Fields);

    QB = MM$QBLK_Get();
    QB->NR$Buf_Size = BufSize;	// Total size of network buffer.
    QB->NR$Buf = Buf;		// Start adrs of network buffer.
    QB->NR$Size = 8;		// byte size of segment within network buffer.
    QB->NR$Seg = Seg;		// Start adrs of segment within net buf.
    QB->NR$SRC_Adrs = ipsrc;	// Source internet adrs (us)
    QB->NR$Dest_Adrs = ipdst;	// destination internet adrs.
    QB->NR$FLAGS = 0;
    QB->NR$ICMP = TRUE;		// Indicate that this is an ICMP packet
    QB->NR$ICM_TYPE = ICMtype;	// Remember ICMP type
    QB->NR$ICM_EX = ICMex;	// and extra information
    INSQUE(QB,Segin->SI_Qtail); // queue to tail of segment in queue.
    RETURN;
    };

//SBTTL "Check-SYN-Wait-List"
/*

Function:

	Check the SYN wait list for a "SYN" segment which caused a
	server to be forked.  Idea is that when a server (well-known
	local-port) is passivly OPEN'ed there maybe a "SYN" segment waiting
	for the passive open to establish a connection.

Inputs:

	TCB - TCB pointer.

Outputs:

	None.

Side Effects:

	IF a match is made for Foreign_Host & port then the SYN was destined
	for this connection.  "SYN" segmenet is processed as if it had just
	arrived.  Connection handshake is started.

*/

FORWARD ROUTINE
    Decode_Segment;

void SEG$Check_SYN_Wait_List(struct tcb_structure * tcb) (void)
    {
    signed long
	struct Queue_Blk_Structure * QB(QB_NR_Fields),
	struct Segment_Structure * Seg,
	LP,
	WIX;
    LABEL
	X,Y;

// Find the WKS entry for the port

    LP = TCB->Local_Port && %X"FFFF" ;
X:  {
    INCR I FROM 0 TO (WKS_COUNT-1) DO
	if (LP == WKS_LIST[I,WKS$Port])
	    {
	    WIX = I;
	    LEAVE X;
	    };
    RETURN;			// Port not found - no SYN pending
    };

// Point at the head of the SYN wait list for this port

    QB = WKS_LIST[WIX,WKS$SYN_Qhead];

// Search the list looking for a match on Foreign host & port.
// Handle wild-card foreign host & port cases.

Y:  {
    WHILE (QB NEQA WKS_LIST[WIX,WKS$SYN_Qhead]) DO
	{
	IF (TCB->Foreign_Host == Wild) OR
	   (TCB->Foreign_Host == QB->NR$Src_Adrs) THEN
	    IF (TCB->Foreign_Port == Wild) OR
	       (TCB->Foreign_Port == QB->NR$Src_Port) THEN
		LEAVE Y;
	QB = QB->NR$NEXT;
	};
    RETURN;
    };

// Here on match. Remove the SYN segment from the list & process it.

    XLOG$FAO(LOG$TCP,"!%T SYN-wait-list match,TCB=!XL,QB=!XL,Seg=!XL!/",
	     0,TCB,QB,QB->NR$Seg);
    REMQUE(QB,QB);		// Remove entry from syn-wait-list.
    SYN_WAIT_COUNT = SYN_WAIT_COUNT + 1;
    WKS_LIST[WIX,WKS$SYN_QCOUNT] = WKS_LIST[WIX,WKS$SYN_QCOUNT] + 1;
    Seg = QB->NR$Seg;		// point at segment.
    if (Decode_Segment(TCB,Seg,QB))
	{			// Delete segment (match = [-1,0,1]).
	MM$Seg_Free(QB->NR$Buf_Size,QB->NR$Buf);
	MM$QBlk_Free(QB);
	};
    };

//SBTTL	"Fork a Server Process"
/*

Function:

	Create a Detached processes to handle the Well-Known local port
	function.  Process runs detached & does a wild-card Foreign-Host
	& Foreign-Port network connection open.  Waiting for the server
	is the SYN which started this mess in the first place.
	Method in creating a process is to give it a unique name so we don't
	get a SS$_DUPLNAM error return from $CREPRC.  What happens is that
	we will try to crate the process in a Loop "n" times.  The loop index
	value plus a "." is appended to the end of the server process name in
	an attempt to make it unique.  The reason this works is that most
	servers will change their UIC to the requesting user's thus allowing
	additional servers to be forked under TCP's UCI.

Inputs:

	IDX = Index into well-known server table.  Entry contains information
	about how to create the server process.

Outputs:

	True = process forked succsssfully.
	False = Error in process creation.

Side Effects:

	None.
*/

Fork_Server(IDX, IP_Address, Remote_Port)
    {
    LITERAL
	PNAMLEN = 20;
    signed long
	RC,
	RP,
	DESC$STR_ALLOC(ProcName,PNAMLEN),
	NewPID;
    EXTERNAL ROUTINE
	PokeAddr : ADDRESSING_MODE(GENERAL);


    RP = (Remote_Port && %X"FFFF") ;

// Try a bunch of times to find one with a non-duplicate name.

    INCR J FROM GLOBAL_MINSRV TO WKS_LIST[Idx,WKS$MaxSrv] DO
	{
	ProcName->DSC$W_LENGTH = PNAMLEN;
	$FAO(%ASCID"!AS.!SL",ProcName->DSC$W_LENGTH,ProcName,
	     WKS_LIST[IDX,WKS$Process],J);

	RC = $CREPRC(PIDADR=NewPID,
		     IMAGE=WKS_LIST[Idx,WKS$Image],
		     INPUT=WKS_LIST[Idx,WKS$Input],
		     OUTPUT=WKS_LIST[Idx,WKS$Output],
		     ERROR=WKS_LIST[Idx,WKS$Error],
		     BASPRI=.WKS_LIST[Idx,WKS$Prior],
		     PRCNAM=ProcName,
		     PRVADR=WKS_LIST[Idx,WKS$Priv],
		     UIC=.MYUIC,
		     STSFLG=.WKS_LIST[Idx,WKS$Stat],
		     QUOTA = WKS_LIST[Idx,WKS$Quotas]
		     );

// See if it worked.

	SELECTONE RC OF
	SET
	[SS$_Normal]:		// Success - done
	    {
	    XLOG$FAO(LOG$TCP,"!%T Forked Server: !AS!/",0,ProcName);

	    ACT$FAO("!%D Forked Server: !AS(PID:!XW) <!UB.!UB.!UB.!UB>!/",0,
		    ProcName, NewPID<0,16,0>,
		    IP_Address<0,8>,IP_Address<8,8>,
		    IP_Address<16,8>,IP_Address<24,8>
		   );
	    ts$servers_forked = ts$servers_forked + 1; // count the servers.
	    PokeAddr(NewPID, IP_Address, RP);
	    return TRUE;
	    };

	[SS$_DuplNam]:		// Duplicate name - try next name
	    0;

	[OTHERWISE]:		// Hard failure
	    {
	    XLOG$FAO(LOG$TCPERR,"%T Server CREPRC Failed for !AS, RC = !XL!/",
		     0,ProcName,RC);
	    return FALSE;
	    };
	TES;
	};

// Failed after max number of tries. Log error and return failure.

    XLOG$FAO(LOG$TCPERR,"!%T Failed to fork server !AS!/",0,ProcName);
    return FALSE;
    };

//SBTTL "Check-Well-Known-Port: Is this a server port?"
/*

Function:

	Check if specified port is in well known range.  If so then
	fork the associated server process.

Inputs:

	Newly received SYN segment addresses:

	Dhost = Destination Host address.
	DPort = Destination port #.
	Shost = Senders Host address
	Sport = Senders port #.

Outputs:
	TRUE  - Segment matches WKS entry, server started, segment queued
	FALSE - Segment not for a WKS that we support, or queue full,
		or failed to fork process (need to send RST)
	ERROR - Segment matches but is duplicate

Side Effects:

	Segment queue block may be INSQUE'd onto the SYN wait list for the
	well-known-port, server process may be started.
*/

Check_WKS(DHost,DPort,SHost,SPort,QBNEW,Segnew)
    {
    MAP
	struct Queue_BLK_Structure * QBNEW(QB_NR_Fields),
	struct Segment_Structure * SegNew;
    signed long
	struct Queue_Blk_Structure * QB(QB_NR_Fields),
	struct Segment_Structure * Seg,
	DP,
	SP,
	WIX;
    LABEL
	X;

    DP = DPort && %X"FFFF" ;
    SP = SPort && %X"FFFF" ;

// See if we know about the port

X:  {
    INCR I FROM 0 TO (WKS_COUNT-1) DO
	if (DP == WKS_LIST[I,WKS$PORT])
	    {
	    WIX = I;
	    LEAVE X;
	    };
    return FALSE;
    };

// See if main queue count exceeded

    if (SYN_WAIT_COUNT <= 0)
	{
	XLOG$FAO(LOG$TCP,"!%T SYN wait list full for SYN on WKS !SL!/",
		 0,Dport);
	return FALSE;
	};

// See if queue count for this WKS exceeded

    if (WKS_LIST[WIX,WKS$SYN_QCOUNT] <= 0)
	{
	XLOG$FAO(LOG$TCP,"!%T SYN wait list for port !SL full!/",0,Dport);
	return FALSE;
	};

// Next, check for duplicate SYN segments on the SYN wait list - drop them.

    XLOG$FAO(LOG$TCP,"!%T SYN for WKS !SL received!/",0,Dport);
    QB = WKS_LIST[WIX,WKS$SYN_QHEAD];
    WHILE (QB NEQA WKS_LIST[WIX,WKS$SYN_QHEAD]) DO
	{

// Check IP addresses.

	IF (DHost == QB->NR$Dest_Adrs) AND
	   (Shost == QB->NR$Src_Adrs) THEN
	    {
	    Seg = QB->NR$Seg;	// point at TCP segment

// Check TCP ports. If duplicate, return "error" so it will be deallocated.

	    IF (DP == Seg->SH$Dest_Port) AND
	       (SP == Seg->SH$Source_Port) THEN
		{
		XLOG$FAO(LOG$TCP,"!%T Dup SYN on Syn-wait list dropped!/");
		return ERROR;
		};
	    };
	QB = QB->NR$Next;	// Look at next element.
	};

// Fire up a server to handle it and queue up the segment on success.

    if (Fork_Server(WIX, Shost, SP))
	{

// Fill in Queue block fields for quick checking during processing of SYN wait
// list.  This prevents having to set up a segment pointer, after all we have
// extra space in the queue block so why not use it.

	QBNEW->NR$Src_Port = SegNew->SH$Source_Port;
	QBNEW->NR$Dest_Port = SegNew->SH$Dest_Port;
	QBNEW->NR$TimeOut = Time_Stamp() + Max_Seg_Lifetime;

// Save this for later processing.

	INSQUE(QBNEW,WKS_LIST[WIX,WKS$SYN_QTAIL]);
	WKS_LIST[WIX,WKS$SYN_QCOUNT] = WKS_LIST[WIX,WKS$SYN_QCOUNT]-1;
	SYN_WAIT_COUNT = SYN_WAIT_COUNT - 1;
	return TRUE;
	}
    else
	return FALSE;
    };

//SBTTL "Timeout_Syn_Wait_List - Check SYN wait list for expired entries"

void SEG$Timeout_Syn_Wait_List(Now: UNSIGNED) (void)
    {
    signed long
	WIX,
	struct Queue_BLK_Structure * QB(QB_NR_Fields),
	Tmp;

    INCR WIX FROM 0 TO (WKS_COUNT-1) DO
	{
	QB = WKS_LIST[WIX,WKS$SYN_Qhead];
	WHILE QB NEQA WKS_LIST[WIX,WKS$SYN_Qhead] DO
	    {
	    if (QB->NR$TimeOut < now)
		{		// Timed-out
		REMQUE(QB,QB);	// Remove queue entry.
		WKS_LIST[WIX,WKS$SYN_Qcount]=.WKS_LIST[WIX,WKS$SYN_Qcount]+1;
		SYN_WAIT_COUNT = SYN_WAIT_COUNT + 1;
		MM$Seg_Free(qb->NR$Buf_Size,qb->NR$Buf); // Release the seg.
		TMP = QB;	// tmp Qblk pointer.
		QB = QB->NR$Next; // next entry.
		MM$QBlk_Free(TMP);// & the queue block.
		}
	    else
		QB = QB->NR$Next;	// point at next entry.
	    };
	};
    };

//SBTTL "ADD_WKS - Add an entry to the WKS table"
/*
    Called by the CONFIG module when a WKS command is seen, this routine adds
    the new WKS entry to the table.
 )%

SEG$WKS_CONFIG(PORT,PRNAME_A,IMNAME_A,STAT,PRIV,
			  PRIOR,QLIMIT,MAXSRV,Quotas,
void 			  Input_A, Output_A, Error_A) (void)
    {
    MAP
	QUOTAS	: REF $BBLOCK;
    BIND
	PRNAME	= PRNAME_A		: $BBLOCK,
	IMNAME	= IMNAME_A		: $BBLOCK,
	INPUT	= INPUT_A		: $BBLOCK,
	OUTPUT	= OUTPUT_A		: $BBLOCK,
	Error	= ERROR_A		: $BBLOCK;
    EXTERNAL ROUTINE
	STR$COPY_DX	: BLISS ADDRESSING_MODE (GENERAL);
    signed long
	WIX,
	QUOPTR,
	Status;

// Make sure there is room for this entry

    if ((WKS_COUNT GEQ WKS_TABLE_SIZE))
	{
	OPR$FAO("Too many WKS entries in INET$CONFIG - entry for !AS ignored",
		PRNAME);
	RETURN;
	};
    WIX = WKS_COUNT;
    WKS_COUNT = WKS_COUNT + 1;


// Fill in the WKS entry fields

    WKS_LIST[WIX,WKS$Port] = PORT;

    // Process Name
    $BBLOCK [WKS_List [WIX, WKS$Process], DSC$W_LENGTH] = 0;
    $BBLOCK [WKS_List [WIX, WKS$Process], DSC$B_DTYPE] = DSC$K_DTYPE_T;
    $BBLOCK [WKS_List [WIX, WKS$Process], DSC$B_CLASS] = DSC$K_CLASS_D;
    $BBLOCK [WKS_List [WIX, WKS$Process], DSC$A_POINTER] = 0;
    Status = STR$COPY_DX (WKS_LIST[WIX,WKS$Process], PRNAME);
    if (NOT Status) Signal (Status);

    // Process image file
    $BBLOCK [WKS_List [WIX, WKS$Image], DSC$W_LENGTH] = 0;
    $BBLOCK [WKS_List [WIX, WKS$Image], DSC$B_DTYPE] = DSC$K_DTYPE_T;
    $BBLOCK [WKS_List [WIX, WKS$Image], DSC$B_CLASS] = DSC$K_CLASS_D;
    $BBLOCK [WKS_List [WIX, WKS$Image], DSC$A_POINTER] = 0;
    Status = STR$COPY_DX (WKS_List [WIX, WKS$Image], IMNAME);
    if (NOT Status) Signal (Status);

    // Process input stream
    $BBLOCK [WKS_List [WIX, WKS$Input], DSC$W_LENGTH] = 0;
    $BBLOCK [WKS_List [WIX, WKS$Input], DSC$B_DTYPE] = DSC$K_DTYPE_T;
    $BBLOCK [WKS_List [WIX, WKS$Input], DSC$B_CLASS] = DSC$K_CLASS_D;
    $BBLOCK [WKS_List [WIX, WKS$Input], DSC$A_POINTER] = 0;
    Status = STR$COPY_DX (WKS_LIST[WIX,WKS$Input], Input);
    if (NOT Status) Signal (Status);

    // Process output stream
    $BBLOCK [WKS_List [WIX, WKS$Output], DSC$W_LENGTH] = 0;
    $BBLOCK [WKS_List [WIX, WKS$Output], DSC$B_DTYPE] = DSC$K_DTYPE_T;
    $BBLOCK [WKS_List [WIX, WKS$Output], DSC$B_CLASS] = DSC$K_CLASS_D;
    $BBLOCK [WKS_List [WIX, WKS$Output], DSC$A_POINTER] = 0;
    Status = STR$COPY_DX (WKS_LIST[WIX,WKS$Output], Output);
    if (NOT Status) Signal (Status);

    // Process error stream
    $BBLOCK [WKS_List [WIX, WKS$Error], DSC$W_LENGTH] = 0;
    $BBLOCK [WKS_List [WIX, WKS$Error], DSC$B_DTYPE] = DSC$K_DTYPE_T;
    $BBLOCK [WKS_List [WIX, WKS$Error], DSC$B_CLASS] = DSC$K_CLASS_D;
    $BBLOCK [WKS_List [WIX, WKS$Error], DSC$A_POINTER] = 0;
    Status = STR$COPY_DX (WKS_LIST[WIX,WKS$Error], Error);
    if (NOT Status) Signal (Status);

    // Allocate the block for the process quota list
    WKS_LIST[WIX,WKS$QUOTAS] = (if (Quotas->DSC$W_LENGTH > 0)
	    {
	    Status = LIB$GET_VM(%REF(Quotas->DSC$W_LENGTH),QUOPTR);
    	    if (NOT Status)
		FATAL$FAO("Seg$WKS_Config - failed to allocate quolst, RC = !XL",Status);
	    CH$MOVE(QUOTAS [DSC$W_LENGTH],QUOTAS [DSC$A_POINTER],QUOPTR);
	    QUOPTR
    	    }
	else 0);

    WKS_LIST[WIX,WKS$Stat] = STAT;
    CH$MOVE(8,PRIV,WKS_LIST[WIX,WKS$PRIV]);
    WKS_LIST[WIX,WKS$Prior] = PRIOR;
    WKS_LIST[WIX,WKS$MaxSrv] =
	(if (MAXSRV == 0)
	    GLOBAL_MAXSRV
	else
	    MAXSRV);
    WKS_LIST[WIX,WKS$SYN_Qcount] = QLIMIT;
    WKS_LIST[WIX,WKS$SYN_Qhead] = WKS_LIST[WIX,WKS$SYN_Qhead];
    WKS_LIST[WIX,WKS$SYN_Qtail] = WKS_LIST[WIX,WKS$SYN_Qhead];
    };

//SBTTL "Log-Segment: dump debug data about segment to log file."

/*

Inputs:

	Seg = segment pointer
	Size = segment size in bytes
	TR_Flag: True = called on segment reception.
		 False = called from seg transmission.

Outputs:

	None.

Side effects:

	Output to log file.

*/

SEG$Log_Segment(struct Segment_Structure * Seg,Size,
			   TR_Flag,BS_Flag): NOVALUE=
    {
    signed long
	dataoff,
	segcopy : Segment_Structure,
	struct Segment_Structure * seghdr,
	sptr;

    seghdr = seg;
    if (BS_Flag)		// Need to byteswap the header...
	{
	CH$MOVE(TCP_Header_Size,CH$PTR(seg),CH$PTR(segcopy));
	seghdr = segcopy;
	SwapBytes(TCP_Header_size/2,seghdr); // Swap header bytes back
	Seghdr->SH$SEQ = ROT(Seghdr->SH$SEQ,16);
	Seghdr->SH$ACK = ROT(Seghdr->SH$ACK,16);
	};

// Point at segment data (past header and possible options)

    dataoff = seghdr->sh$data_offset*4;

    if (TR_Flag)
	sptr = %ASCID"Received"
    else
	sptr = %ASCID"Sent";

// Write title line...

    LOG$FAO("!%T !AS Network Segment, SEG=!XL, OPT=!XL, DATA=!XL:!/",
	    0,sptr,seg,seg+TCP_Header_size,seg+.dataoff);

// Log most of the data - N.B. We don't write the final CRLF yet...

    LOG$FAO(%STRING("!_SrcPrt:!_!XL (!UL)!_DstPrt:!_!XL (!UL)!/",
		   "!_SEQnum:!_!XL (!UL)!_ACKnum:!_!XL (!UL)!/",
		   "!_Window:!_!UW!_CKsum:!_!UW!_DatPtr:!_!UW!_UrgPtr:!_!UW!/",
		   "!_Control Flags:!_!XL"),
	    seghdr->SH$Source_port,seghdr->SH$Source_port,
	    seghdr->SH$Dest_port,seghdr->SH$Dest_port,
	    seghdr->SH$SEQ,seghdr->SH$SEQ,seghdr->SH$ACK,seghdr->SH$ACK,
	    seghdr->SH$Window,seghdr->SH$Checksum,seghdr->SH$data_offset,
	    seghdr->SH$Urgent,seghdr->SH$C_All_Flags);
    if (Seghdr->SH$C_All_Flags != 0)
	{
	LOG$OUT(" = ");
	SELECT TRUE of
	SET
	[Seghdr->SH$C_URG]:  LOG$OUT("URG ");
	[Seghdr->SH$C_ACK]:  LOG$OUT("ACK ");
	[Seghdr->SH$C_EOL]:  LOG$OUT("EOL ");
	[Seghdr->SH$C_RST]:  LOG$OUT("RST ");
	[Seghdr->SH$C_SYN]:  LOG$OUT("SYN ");
	[Seghdr->SH$C_FIN]:  LOG$OUT("FIN ");
	TES;
	};
    LOG$OUT(%CHAR(13,10));

    if (dataoff > TCP_header_size)
	{
	LITERAL
	    maxopt = 20;
	signed long
	    DESC$STR_ALLOC(optstr,maxopt*3),
	    optcnt,
	    outcnt;
	optcnt = dataoff-TCP_header_size;
	if (optcnt > maxopt)
	    outcnt = maxopt
	else
	    outcnt = optcnt;
	ASCII_Hex_Bytes(optstr,outcnt,seg+TCP_header_size,
			optstr->DSC$W_LENGTH);
	LOG$FAO("!_Options:!_!SL = !AS!/",optcnt,optstr);
	};

    if (Size > dataoff)
	{
	LITERAL
	    maxhex = 20,
	    maxasc = 50;
	signed long
	    datcnt,
	    asccnt,
	    hexcnt,
	    DESC$STR_ALLOC(dathex,maxhex*3);
	datcnt = size-.dataoff;
	if (datcnt > maxasc)
	    asccnt = maxasc
	else
	    asccnt = datcnt;
	if (datcnt > maxhex)
	    hexcnt = maxhex
	else
	    hexcnt = datcnt;
	ASCII_Hex_Bytes(dathex,hexcnt,seg+.dataoff,dathex->DSC$W_LENGTH);
	LOG$FAO("!_Data Count: !SL!/!_HEX:!_!AS!/!_ASCII:!_!AF!/",
		.datcnt,dathex,asccnt,seg+.dataoff);
	};
    };



//SBTTL "Append a segment to the end of the "future" queue"
/*
 )%

Append_Future_Q(TCB, QB, Seg, SEQsize)
    {
    MAP
	struct tcb_structure * tcb,
	struct Queue_Blk_Structure * QB(QB_NR_Fields),
	struct Segment_Structure * Seg;
    signed long
	struct Queue_Blk_Structure * FQB(QB_NR_Fields);

    ts$future_rcvd = ts$future_rcvd+1;
    if (TCB->RF_Qcount GEQ FQ_MAX)
	{
	XLOG$FAO(LOG$TCPERR,"!%T FQ full for seg !XL, QB !XL!/",0,Seg,QB);
	ts$future_dropped = ts$future_dropped+1;
	return TRUE;		// Caller should delete
	};
    QB->NR$SEQ_Start = Seg->SH$SEQ;
    QB->NR$SEQ_End = Seg->SH$SEQ + SEQsize;
    QB->NR$SEQ_Count = SEQsize;
    FQB = TCB->RF_Qhead;

 // Find where to put this segment on the future queue

    WHILE TRUE DO
	if (FQB == TCB->RF_Qhead)
	    EXITLOOP (FQB = TCB->RF_Qtail)
	else
	    if (QB->NR$SEQ_Start < FQB->NR$SEQ_Start)
		EXITLOOP (FQB = FQB->NR$Last)
	    else
		if (QB->NR$SEQ_Start == FQB->NR$SEQ_Start)
		    {
		    XLOG$FAO(LOG$TCP,
			     "!%T Drop duplicate FQ seg !XL, QB !XL!/",
			     0,Seg,QB);
		    ts$future_dups = ts$future_dups+1;
		    return TRUE;// Tell caller to deallocate
		    }
		else
		    FQB = FQB->NR$Next;

// Insert this segment in proper sequence

    XLOG$FAO(LOG$TCP,"!%T Seg !XL, QB !XL inserted on FQ at FQB !XL!/",
	     0,Seg,QB,FQB);
    TCB->RF_Qcount = TCB->RF_Qcount+1;
    INSQUE(QB,FQB);
!~~~XINSQUE(QB,FQB,Append_Future_Q,Q$TCBFQ,TCB->RF_Qhead);
    return FALSE;		// Don"t deallocate - it"s on our queue
    };

//SBTTL "Check and attempt to process segments on "future" queue"
/*
 )%

void Check_Future_Q(struct tcb_structure * tcb) (void)
    {
    signed long
	struct Queue_Blk_Structure * NQB(QB_NR_Fields);

// Handle segments on the future queue that are no longer in the future.

    NQB = TCB->RF_Qhead;
    WHILE NQB NEQA TCB->RF_Qhead DO
	{
	signed long
	    SEQoffset,
	    SEQsize,
	    delete,
	    struct Queue_Blk_Structure * QB(QB_NR_Fields);

	QB = NQB;
	NQB = QB->NR$Next;
	delete = false;
	SEQsize = QB->NR$SEQ_Count;
	SEQoffset = TCB->RCV_NXT-QB->NR$SEQ_Start;
	SELECTONE TRUE OF
	SET

	// If first in-window octet is beyond end of segment, then this segment
	// has become obsolete - drop it.

	[SEQoffset GEQU SEQsize]:
	     {
	     delete = true;
!~~~	     XREMQUE(QB,QB,Check_Future_Q,Q$TCBFQ,TCB->RF_Qhead);
	     REMQUE(QB,QB);
	     TCB->RF_Qcount = TCB->RF_Qcount-1;
	     XLOG$FAO(LOG$TCP,"!%T Flushing FQ seg !XL, QB !XL, SEQ=!XL,!XL!/",
		      0,QB->NR$Seg,QB,QB->NR$SEQ_Start,QB->NR$SEQ_End);
	     ts$future_dropped = ts$future_dropped+1;
	     };


	// If first in-window octet is within the segment, then the segment is
	// now usable - process it in Decode_Segment routine.

	[SEQoffset GEQ 0]:
	    {
!~~~	    XREMQUE(QB,QB,Check_Future_Q,Q$TCBFQ,TCB->RF_Qhead);
	    REMQUE(QB,QB);
	    TCB->RF_Qcount = TCB->RF_Qcount-1;
	    XLOG$FAO(LOG$TCP,"!%T Using FQ seg !XL, QB !XL, SEQ=!XL,!XL!/",
		     0,QB->NR$Seg,QB,QB->NR$SEQ_Start,QB->NR$SEQ_End);
	    delete = Decode_Segment(TCB,QB->NR$Seg,QB);
	    ts$future_used = ts$future_used+1;
	    };


	// Other cases should mean the segment is still in the future, so we
	// just leave the segment on the queue.

	TES;
	if (delete)
	    {
	    MM$Seg_Free(QB->NR$Buf_Size,QB->NR$Buf);
	    MM$QBLK_Free(QB);
	    };
	};
    };

//SBTTL "READ_TCP_Options: process TCP segment options in SYN segment"

READ_TCP_OPTIONS(TCB,SEG) : NOVALUE (void)
    {
    MAP
	struct tcb_structure * tcb,
	struct Segment_Structure * SEG;
    signed long
	struct TCP$OPT_BLOCK * OPTR,
	OPTLEN,
	DATAPTR;

// Start at beginning of options area (start of segment data)

    OPTR = SEG->SH$DATA;
    DATAPTR = SEG+(SEG->SH$DATA_OFFSET*4);

// Scan the the entire options area until we hit the start of TCP data

    WHILE OPTR LSS DATAPTR DO
	{

// Deal with options we can handle
// N.B. We may modify the options area by byteswapping.

	SELECTONE OPTR->TCP$OPT_KIND OF
	SET
	[TCP$OPT_KIND_EOL]:	// End of options list
	    EXITLOOP;

	[TCP$OPT_KIND_NOP]:	// No-op option
	    OPTR = OPTR + 1;	// Advance pointer by one byte

	[TCP$OPT_KIND_MSS]:	// Maximum segment size option
	    {
	    signed long
		SZ;
	    OPTLEN = OPTR->TCP$OPT_LENGTH;
	    if (OPTLEN == TCP$OPT_LENGTH_MSS)
		{		// Length is correct
		SZ = OPTR->TCP$OPT_DWORD;
		SWAPWORD(SZ);	// Change to VAX byte order
		TCB->MAX_SEG_DATA_SIZE = MIN(SZ,MAX_RECV_DATASIZE);
		TCB->SND_ACK_Threshold = TCB->Max_SEG_Data_Size ;
		XLOG$FAO(LOG$TCP,
			 "!%T TCP MSS option = !SL set to !SL, TCB=!XL!/",
			 0,SZ,TCB->MAX_SEG_DATA_SIZE,TCB);
		}
	    else
		XLOG$FAO(LOG$TCP+LOG$TCPERR,
			 "!%T ?TCP MSS option wrong size=!SL, TCB=!XL!/",
			 0,OPTR->TCP$OPT_LENGTH,TCB);
	    OPTR = OPTR + OPTR->TCP$OPT_LENGTH;
	    };

	[OTHERWISE]:		// Some unknown option type
	    {
	    XLOG$FAO(LOG$TCP+LOG$TCPERR,
		     "!%T ?Bad TCP option type !SL, size !SL for TCB !XL!/",
		     0,OPTR->TCP$OPT_KIND,OPTR->TCP$OPT_LENGTH,TCB);
	    EXITLOOP;		// Can't procede, since length may not be valid
	    };
	TES;
	};
    };

//SBTTL "Decode-Segment: Figure out what to do with a network segment."
/*

Function:

	We have received this segment from the network, figure out what to
	do with it.  Examine the Connection state & the TCP header.
	Process control information & optional data.

Inputs:

	TCB = TCB of connection which gets this segment.
	Seg = Segment pointer.
	QB = Network receive queue block.

Implicit Inputs:

	Segment has passed checksum test & is bound for this connection (TCB).

Outputs:

	True(1): Have caller delete Segment & Queue Block data structures.
	False(0): Segment & queue-Blk have been put on another queue, don't delete.
	Error(-1): TCB has been (deleted or inactivated), caller deletes
		   Segment & queue-blk.

Side Effects:

	Many......This is where connection states change due to network segment
	arrival.  Connection timeout reset if valid segment for this
	connection.
*/

Decode_Segment(TCB,Seg,QB)
    {
    EXTERNAL ROUTINE
	TCB_Promote;
    MAP
	struct tcb_structure * tcb,
	struct Segment_Structure * Seg,
	struct Queue_Blk_Structure * QB(QB_NR_Fields);
    signed long
	RC,
	AckTst_OK,
	SeqTst_OK,
	Ack,
	DataSize,
	SEQsize,
	SEQcount,
	SEQstart,
	SEQoffset,
	struct Queue_Blk_Structure * QBR(QB_UR_Fields),
	EOL,
	struct Queue_Blk_Structure * QBN(QB_NR_Fields),
	Ucount,
	Uoffset,
	RetCode,
	Seg_Trimmed	: INITIAL (0);

/*
    if ($$LOGF(LOG$TCBDUMP))
	{
	LOG$FAO("!/!%T Decode Seg - TCB dump of !XL!/",0,TCB);
	Dump_TCB(TCB));
	};
*/

    RetCode = True;		// Let caller handle memory mgmt.

// Compute amount of data available in this segment.
// Also set the value in the associated Queue block.

    DataSize = QB->NR$Data_Size = QB->NR$Size - Seg->SH$Data_Offset*4 ;

// Point at first user request

    QBR = TCB->UR_Qhead;

!********************************************************
!	Process unsynchronized Connection States.	!
!********************************************************

    SELECTONE TCB->State OF
    SET

!********************************************************
!		LISTEN State				!
!********************************************************

    [CS$LISTEN]:
	{
!	XLOG$FAO(LOG$TCP,"!%T Received Seg in LISTEN State.!/", 0);
	SELECTONE TRUE OF
	SET
	[Seg->SH$C_ACK]:	// "ACK" ?
	    {
	    if (NOT Seg->SH$C_RST)
		Send_Reset(TCB,Seg->SH$Ack);
	    RETURN(RetCode);
	    };

	[Seg->SH$C_RST]:
	    XLOG$FAO(LOG$TCP,
		     "!%T Received RST in listen state, TCB=!XL!/",0,TCB);

	[Seg->SH$C_SYN]:
	    {

// Set TCB's sequence space counters.
// snd_nxt (send next) & snd_Una(send oldest unackowledged seq #) are
// both sent during TCB initialization (Init-TCB).

!	    XLOG$FAO(LOG$TCBSTATE,
!		     "!%T Decode-Seg: Received SYN for Passive connection.!/",0);
	    TCB->RCV_NXT = Seg->SH$Seq + 1;	  // Next expected rcv sequence #.
	    TCB->IRS = TCB->Snd_WL = Seg->SH$Seq;// set Initial rcv seq #  & window update.
	    TCB->Snd_Wnd = Seg->SH$Window; 	  // Send window size in bytes.
	    TCB->Snd_Max_Wnd = MAXU(TCB->Snd_Max_Wnd, TCB->Snd_Wnd) ;
	    TCP$Set_TCB_State(TCB,CS$SYN_RECV);  // Connection state change.

// Process segment options, if any.

	    if (seg->sh$data_offset > tcp_data_offset)
		Read_TCP_Options(TCB,Seg);

// Fill in any unspecified Foreign Host or Port
// Also, if wild foreign host, start an address to name lookup for it.

	    IF (TCB->Foreign_Host == Wild) OR
	       (TCB->Foreign_Port eql Wild) THEN
		{
		if (TCB->Foreign_Host == WILD)
		    {
		    TCB->Foreign_Host = QB->NR$Src_Adrs;
		    TCB->NMLOOK_Flag = TRUE;
		    NML$GETNAME(QB->NR$Src_Adrs,TCP$Adlook_Done,TCB);
		    };

		if (TCB->Foreign_Port == Wild)
		    TCB->Foreign_Port = (Seg->SH$Source_Port && %X"FFFF") ;

// Move TCB to head of local port list as now it's fully specified.
		TCB_Promote ( TCB );
		};

// Fill in unspecified local host

	    if ((TCB->Local_Host eql Wild))
		TCB->Local_Host = QB->NR$Dest_Adrs;

// Now that the connection has been fully specified we can "ACK" the "SYN".
// If SYN-ACK send fails (no route - config error), flush the connection.

	    if (NOT Send_Syn$Ack(TCB))
		{
		XLOG$FAO(LOG$TCPERR,
		     "!%T Decode-seg(LISTEN): Send_SYN$ACK failed, TCB=!XL!/",
		     0,TCB);
		Send_Reset(TCB,Seg->SH$Ack);
		TCP$KILL_P}ING_REQUESTS(TCB,NET$_NRT);
		TCB$Delete(TCB);
		Return(Error);
		};
!	    XLOG$FAO(LOG$TCBSTATE,"!%T Decode-seg: SYN-ACK sent.!/", 0);


// If data present then queue it for later.

	    if (DataSize > 0)
		{
		XLOG$FAO(LOG$TCPERR,"!%T Decode Seg(Listen): SYN data dropped!/", 0);
!~~~		RetCode = False;
		};

// make sure the connection stays alive.

	    TCB->Connection_TimeOut = Active_Open_Timeout + Time_Stamp();
	    };

	[OTHERWISE]:
	    XLOG$FAO(LOG$TCPERR,
		     "!%T Decode Seg(Listen): NON SYN control!/",0);
	TES;

	return (RetCode);
	};			// "LISTEN" State.

!********************************************************
!		"SYN-SENT" State.			!
!********************************************************

    [CS$SYN_SENT]:
	{
!	XLOG$FAO(LOG$TCP, "!%T Received Seg in SYN-SENT State.!/", 0);
	ACK = -1;
	if (Seg->SH$C_ACK)
	    {
	    IF (Seg->SH$ACK LEQU TCB->ISS) OR
	       (Seg->SH$ACK > TCB->Snd_Nxt) THEN
		{
		Send_Reset(TCB,Seg->SH$Ack); // Unacceptable "ACK".
!		XLOG$FAO(LOG$TCPERR, "!%T (Syn-sent)Unacceptable ACK.!/", 0);
		RETURN(True);
		}
	    else		// acceptable "ACK"
		{
		IF (TCB->Snd_UNA LEQU Seg->SH$Ack) AND
		   (Seg->SH$Ack LEQU TCB->Snd_Nxt) THEN
		    {
!		    XLOG$FAO(LOG$TCP, "!%T (Syn-sent)Valid ACK.!/", 0);
		    Ack = True;
		    TCB->Snd_UNA = Seg->SH$ACK;
		    ACK_RT_Queue(TCB,Seg->SH$ACK);
		    }
		else
		    RETURN(True); // Invalid ACK.
		};
	    };

// at this juncture we have a valid ACK.
// Check the "RST" control bit in TCP Header.

	if (Seg->SH$C_RST)	// Connection refused - reset
	    {
	    TCP$KILL_P}ING_REQUESTS(TCB,NET$_CRef);
	    TCP$Inactivate_TCB(TCB,NET$_CR);
	    XLOG$FAO(LOG$TCBSTATE,
		     "!%T Decode seg:(SYN-Sent) RESET TCB !XL!/",
		     0,TCB);
	    RETURN(Error);	// Caller deletes seg, Queue-blk.
	    }
	else			// Not a reset seg, check for "SYN" bit.
	    {
	    if (Seg->SH$C_SYN)
		{

// set TCB sequence space counters.

		TCB->RCV_NXT = Seg->SH$SEQ + 1;
		TCB->Snd_Wnd = Seg->SH$Window;
		TCB->Snd_Max_Wnd = MAXU(TCB->Snd_Max_Wnd, TCB->Snd_Wnd) ;
		TCB->IRS = TCB->Snd_WL = Seg->SH$SEQ;

// Was our "SYN" ack'ed?

		if (TCB->Snd_UNA > TCB->ISS)
		    {
		    TCB->IS_Synched = TRUE;
		    TCP$Set_TCB_State(TCB,CS$ESTABLISHED);

// Tell the user the Connection is OPEN as the IO status has yet to be posted.
// Valid only for connections not openned in immediate return mode.

		    if (NOT TCB->Open_NoWait)
			TCP$Post_Active_Open_IO_Status(TCB,SS$_Normal,0);
		    XLOG$FAO(LOG$TCP,
			     "!%T Our SYN ACKed, Connection established.!/",0);
		    }
		else
		    TCP$Set_TCB_State(TCB,CS$SYN_RECV);

!		TCP$Send_Ack(TCB);
		TCP$Enqueue_Ack(TCB);

// If data present then queue for later

		if (DataSize > 0)
		    {
		    XLOG$FAO(LOG$TCPERR,
			     "!%T Decode Seg: SYN-ACK data dropped, TCB=!XL!/",
			     0,TCB);
!~~~		    RetCode = False;
		    };

// Process segment options, if any.

		if (seg->sh$data_offset > tcp_data_offset)
		    Read_TCP_Options(TCB,Seg);
		};
	    };

// Reset connection time out as something useful has happened.

	IF Keep_Alive
	    THEN TCB->Connection_Timeout = CONN_TIMEVAL + Time_Stamp()
	    else TCB->Connection_TimeOut = 0;
	return (RetCode);
	};	// End: "SYN-SENT" State.
    TES;

!*******************************************************
!	Process "Synchronized" Connection States       !
!*******************************************************

// Advance connection timeout since the other end is sending something

    IF Keep_Alive
	THEN TCB->Connection_TimeOut = CONN_TIMEVAL + Time_Stamp()
	else TCB->Connection_TimeOut = 0;

// Determine the sequence space occupied by this segment.
// Set field "NR$SEQ_}" in the Queue-blk associated with this segment.

    SeqTst_OK = False;		// assume the worst, seg is unacceptable.

// compute sequence space occupied.
// SEQsize is the count of seqence numbers in the packet
// SEQcount is the count of them that are in the window

    SEQsize = DataSize;	// Start with packet data count
    SEQcount = 0;		// Assume no usable seq #s in packet yet

    if (Seg->SH$C_SYN)
	SEQSize = SEQsize + 1;	// SYN occupies seqence space.
    if (Seg->SH$C_FIN)
	SEQsize = SEQsize + 1;	// So does "FIN".

// Set last sequence # in this segment.

    if (SEQsize <= 0)
	qb->nr$seq_end = Seg->SH$SEQ
    else
	qb->nr$seq_end = (Seg->SH$SEQ + SEQsize) - 1;

// Assume no user data in the segment yet

    Ucount = 0;
    Uoffset = 0;

// Test segment acceptablity by checking that the segment sequence numbers
// fall inside the receive window (TCB->RCV_Nxt + TCB->RCV_WND).

    SELECTONE TRUE OF
    SET
    [TCB->RCV_WND == 0]:	// Zero window cases
	{
	SELECTONE TRUE OF
	SET
	[SEQsize == 0]:	// Empty packet case
	    if (Seg->SH$Seq == TCB->Rcv_Nxt)
		SeqTst_OK = True;

	[SEQsize != 0]:	// Nonempty packet case
	    if (Seg->SH$C_ACK || Seg->SH$C_RST || Seg->SH$C_URG)
		{    	// Data is unacceptable but take [ack,rst, urg]
		SeqTst_OK = True;
		SEQsize = SEQsize - DataSize; // remove data from SEQ
		DataSize = 0;
		};
	TES;
	};

// Sequence number test doesn't match the TCP spec since we will take a segment
// that exceeds the window and use only the data the lies in the window.

    [TCB->RCV_WND NEQU 0]:	// Window is open case
	{
	SELECTONE TRUE OF
	SET
	[SEQsize == 0]:	// Empty packet case
	    if ((Seg->SH$SEQ == TCB->RCV_NXT))
		SeqTst_OK = True;

	[SEQsize > 0]:	// Nonempty packet case (the useful one)
	    {

	    // SEQoffset is the offset of the first in-window octet in the
	    // segment.

	    SEQoffset = TCB->RCV_NXT - Seg->SH$SEQ;
	    SELECTONE TRUE OF
	    SET

	    // SEQoffset >= 0 means it is at or before the left edge of the
	    // window. SEQoffset <= SEQsize means the first in-window octet
	    // is inside the segment and thus the segment is usable.

	    [(SEQoffset GEQ 0) && (SEQoffset LSS SEQsize)]:
		{
		SEQstart = TCB->RCV_NXT;
		SEQcount = SEQsize - SEQoffset;

		// Trim to fit in window

		if (SEQcount > TCB->RCV_WND)
		    {
		    Seg_Trimmed = 1;
		    SEQcount = TCB->RCV_WND;
		    QB->NR$SEQ_end = SEQstart+.SEQcount;
		    };

		SeqTst_OK = True;
		};


	    // SEQoffset < 0 means that the segment is beyond the left edge
	    // of the window. TCB->RCV_WND > (-.SEQoffset) means that the
	    // start of the segment is before the right edge and the segment
	    // may be held for future use.

!	    [(SEQoffset LSS 0) && (TCB->RCV_WND > (-.SEQoffset))]:
	    [(SEQoffset LSS 0) && (TCB->RCV_WND < (SEQoffset))]:
		{
		RetCode = Append_Future_Q(TCB,QB,Seg,SEQSize);
!		TCB->Pending_ACK = TRUE; // Force an ACK for this
		TCP$Enqueue_Ack(TCB);
		return RetCode;
		};
	    TES;
	    };
	TES;
	};
    TES;

// If segment was not acceptable, drop it.
// Test for duplicate segment, if True, re-ack it as the original "ACK"
// might have been lost.  We get a free window update anyway.

    if (NOT SeqTST_ok)
	{
	ts$oorw_segs = ts$oorw_segs + 1;

// Test for duplicate

	IF (Seg->SH$SEQ LEQU TCB->Rcv_Nxt) AND
	   (Seg->SH$Seq GEQU TCB->IRS) THEN
	    {

// Probable duplicate.

	    IF (Seg->SH$ACK GEQU TCB->ISS) AND
	       (Seg->SH$ACK LEQU TCB->Snd_Nxt) THEN
		{

// Duplicate segment, RE-ACK.  If time-wait state then reset the TW timer.

		ts$duplicate_segs = ts$duplicate_segs + 1;
		ts$oorw_segs = ts$oorw_segs - 1; // don't count seg twice.

		if (TCB->State == CS$Time_Wait)
		    TCB->Time_Wait_Timer = Max_Seg_Lifetime + Time_Stamp();

// Use window from duplicate segment as it is more current than our present one.
// Make sure we take into account transmitted but unacknowledged sequence
// numbers.  Check that the update is new than what we have (snd_wl).

		if (TCB->Snd_Wl LEQU Seg->SH$Seq)
		    {
		    TCB->snd_wnd= seg->sh$window - 
				  (TCB->snd_nxt - seg->sh$ack);
		    TCB->Snd_Max_Wnd = MAXU(TCB->Snd_Max_Wnd, TCB->Snd_Wnd) ;
		    TCB->snd_wl = seg->sh$seq;
		    };
		XLOG$FAO(LOG$TCPERR,
			 "!%T Decode Seg: dup seg - Seg-ack (!UL) <=Snd-nxt (!UL)!/",
				0, seg->sh$ack, TCB->SND_NXT);
!		TCP$Send_Ack(TCB);
		TCP$Enqueue_Ack(TCB);
		}
	    else
		XLOG$FAO(LOG$TCPERR,
			 "!%T Duplicate Segment:  Seg-Seq (!UL) <= Recv-Nxt (!UL)!/",
				0, seg->sh$seq, TCB->RCV_NXT);
	    };

	if ($$LOGF(LOG$TCPERR))
	    LOG$FAO("!%T SEGIN: !XL (!UL) Failed seq tests, RCV_WND=!UL, RCV_NXT=!XL (!UL), SND_NXT=!XL (!UL)!/",
		    0,
		    seg->sh$seq,seg->sh$seq,TCB->rcv_wnd,TCB->rcv_nxt,
		    TCB->rcv_nxt,TCB->snd_nxt,TCB->snd_nxt);

	RETURN(True);		// Let caller delete the segment.
	};

// Check if ACK actually acknowledges something valid

    IF (TCB->Snd_Una < Seg->SH$Ack) AND
       (Seg->SH$Ack LEQU TCB->Snd_Nxt) THEN
	ACKTst_OK = True
    else
	AckTST_OK = False;

// Segment is acceptable (in the receive window->rcv_wnd).

!********************************************************
!		Check the "ACK" bit.			!
!********************************************************

    if (seg->SH$c_ack)
	{
	SELECTONE TCB->State OF
	SET
	[CS$SYN_RECV]:
	    {

// If unacceptable ACK then send a RESET reply & drop the segment.
// Otherwise: If NOT a reset segment then our SYN-ACK is acked & the
// connection becomes established.  If the seg contained a RESET flag then
// fall thru the ACK processing & into RESET processing.

	    if (NOT AckTST_OK)
		{
		Send_Reset(TCB,Seg->SH$ACK);
		RETURN(True);	// caller deletes segment & Qblk.
		};

	    if (NOT Seg->SH$C_RST)
		{
		TCB->Snd_Una = Seg->SH$Ack;
		Ack_RT_Queue(TCB,Seg->SH$ACK);
		TCB->IS_Synched = TRUE;
		TCP$Set_TCB_State(TCB,CS$Established);
		XLOG$FAO(LOG$TCP,"!%T SYN_RECV Connection established.!/",0);

// Make sure we update the send window here, since some systems have a habit of
// opening connections with zero windows.

		if (TCB->SND_WL LEQU Seg->SH$Seq)
		    {
		    TCB->snd_wnd=seg->sh$window-(TCB->snd_nxt-TCB->snd_una);
		    TCB->Snd_Max_Wnd = MAXU(TCB->Snd_Max_Wnd, TCB->Snd_Wnd) ;
		    TCB->snd_wl=seg->sh$seq;
		    XLOG$FAO(LOG$TCP,
			 "!%T Updated SND_WND=!UL, SND_NXT=!XL (!UL), SND_UNA=!XL (!UL)!/",
			 0,TCB->snd_wnd,TCB->snd_nxt,TCB->snd_nxt,
			 TCB->snd_una,TCB->snd_una);
		    };

// Finish up pending I/O on this TCB.

		if (TCB->IS_TVT)
		    {

// If it's a TVT, then we need to try to open it. On failure, the TCB has been
// flushed, so no further processing is possible for this segment.

		    if (NOT TELNET_OPEN(TCB))
			{
			Reset_Unknown_Connection(Seg,QB);
			return Error;
			};
		    }
		else
		    if (NOT TCB->Open_NoWait)
			TCP$Post_Active_Open_IO_Status(TCB,SS$_Normal,0);
		};
	    };

	[CS$Established,CS$Fin_Wait_1,CS$Fin_Wait_2,CS$Close_Wait]:
	    {
	    if (AckTst_ok)
		{
		TCB->Snd_Una = Seg->SH$Ack;
		ACK_RT_Queue(TCB,Seg->SH$Ack);

// If state is CS$FIN_WAIT_1 & the RT-Queue is empty (ie, SND_NXT = Seg_ACK)
// then change state to FIN_WAIT_2.

		if (TCB->State == CS$Fin_Wait_1)
		    if (TCB->Snd_Nxt == Seg->SH$ACK)
			TCP$Set_TCB_State(TCB,CS$Fin_Wait_2);
		};

// Update send window, taking into account transmitted by unacknowleged sequence
// numbers (SND_UNA).
// Snd-WL is updated to the current segment sequence #.
// Record seg seq # at last window update.

	    if (TCB->Snd_Wl LEQU Seg->SH$Seq)
		{
		TCB->snd_wnd = seg->sh$window-(TCB->snd_nxt - TCB->snd_una);
	        TCB->Snd_Max_Wnd = MAXU(TCB->Snd_Max_Wnd, TCB->Snd_Wnd) ;
		TCB->snd_wl = seg->sh$seq;
		XLOG$FAO(LOG$TCP,
			 "!%T Updated SND_WND=!UL, SND_NXT=!XL (!UL), SND_UNA=!XL (!UL)!/",
			 0,TCB->snd_wnd,TCB->snd_nxt,TCB->snd_nxt,
			 TCB->snd_una,TCB->snd_una);
		};
	    };

	[CS$Time_Wait]:
	    TCB->Time_Wait_Timer = Time_Stamp() + Max_Seg_LifeTime;

	[CS$Closing]:
	    {

// If the ACK acknowledges outstanding FIN, then enter Time_Wait state.

	    if (TCB->snd_nxt == Seg->sh$ack)
		{		// "FIN" has been ack'ed.
		XLOG$FAO(LOG$TCBSTATE,
			 "!%T DS(Closing): FIN ACKed for conn !XL!/",
			 TCB);
		TCP$Set_TCB_State(TCB,CS$Time_Wait);
		if (NOT TCB->Close_NoWait)
		    {
		    TCP$Post_User_Close_IO_Status(TCB,SS$_Normal,0);
		    TCB->Time_Wait_Timer = Time_Stamp() + Max_Seg_LifeTime;
		    }
		else
		    {
		    TCP$KILL_P}ING_REQUESTS(TCB,NET$_KILL);
		    TCB$Delete(TCB);
		    XLOG$FAO(LOG$TCBSTATE,
			     "!%T Connection purged and deleted !XL!/", TCB);
		    RETURN(Error);
		    };
		RETURN(True);	// Let caller delete segment
		}
	    else
		RETURN(True);	// Ignore segment, caller deletes.
	    };

// likewise, in LAST-ACK state, but delete connection

	[CS$Last_Ack]:
	    {
	    if (TCB->snd_nxt == Seg->sh$ack)
		{		// "FIN" has been acked.
		XLOG$FAO(LOG$TCBSTATE,
			 "!%T DS(Last-ACK): FIN ACKed, deleting conn !XL!/",
			 0,TCB);
		TCP$KILL_P}ING_REQUESTS(TCB,NET$_CC);
		TCB$Delete(TCB);
		RETURN(Error);	// TCB gone - let caller delete the segment
		}
	    else
		RETURN(True);	// Ignore segment
	    };
	TES;
	};			// End: Check "ACK" Bit.

!********************************************************
!		Check "RST" Bit.			!
!********************************************************

    if (Seg->sh$c_rst)
	{
	SELECTONE TRUE OF
	SET

// RESET in SYN-RECV is special in that passive connections return to the
// LISTEN state, and are not actual RESET.

	[(TCB->State == CS$SYN_RECV) && (NOT TCB->Active_Open)]:
	    {
	    TCP$Set_TCB_State(TCB,CS$Listen);
	    SEG$Purge_RT_Queue(TCB);
	    RETURN(Error);	// caller deletes seg, TCB is dead.
	    };

// For all other cases, just reset the connection.

	[OTHERWISE]:
	    {
	    if (TCB->State == CS$Time_Wait)
		rc = NET$_CC
	    else
		rc = NET$_CR;
	    TCP$KILL_P}ING_REQUESTS(TCB,rc);
	    XLOG$FAO(LOG$TCBSTATE,
		     "!%T DS(!SL): RESET Connection !XL!/", 0,TCB->State,TCB);
	    TCP$Inactivate_TCB(TCB,rc); // Let user know connection was reset.
	    RETURN(Error);	// Caller deletes segment.
	    };
	TES;
	};			// End: Check "RST" Bit.

!********************************************************
!		Check "SYN" Bit.			!
!********************************************************

    if (Seg->sh$c_syn)
	{

// Is segment in the receive window?

	if (Seg->SH$Seq > (TCB->Rcv_Nxt + TCB->Rcv_Wnd))
	    RETURN(True)	// not in window just drop
	else
	    {

// If sequence number is same as IRS, just ignore - it is an old duplicate.
// Any other in-window SYN is bad news, however - we RESET the connection.

	    if (Seg->SH$Seq == TCB->IRS)
		RETURN(True)	// Old, duplicate SYN - just drop it
	    else
		{		// In-window SYN - Bad news
		Send_Reset(TCB,Seg->SH$Ack);
		TCP$KILL_P}ING_REQUESTS(TCB,NET$_CR);
		XLOG$FAO(LOG$TCBSTATE,
			 "!%T Dup SYN, deleting connection !XL!/",0,TCB);
		TCB$Delete(TCB);
		RETURN(Error);	// TCB is gone, caller deletes.
		}
	    };
	};			// End: Check "SYN" Bit.

!********************************************************
!		Check "URG" Bit.			!
!********************************************************

    if (Seg->SH$C_URG)
	{
	IF (TCB->State == CS$Established) OR
	   (TCB->State == CS$Fin_Wait_1) OR
	   (TCB->state == CS$Fin_Wait_2) THEN
	    {
	    TCB->Rcv_UP = MAX(TCB->Rcv_UP,Seg->SH$Urgent);
	    QB->NR$Urg = True;
	    };
	};

!********************************************************
!	Process Segment Text (Check for Data & EOL).	!
!********************************************************

    if (SEQcount > 0)
	{			// Have something in window

// EOL asserted?

	if (Seg->SH$C_EOL)	// Check EOL (Push) flag.
	    QB->NR$EOL = True;

	SELECTONE TCB->State OF
	SET
	[CS$Established,CS$Fin_Wait_1,CS$Fin_Wait_2]:
	    {

// Log updated RCV.NXT

	    if ($$LOGF(LOG$TCP))
		{
		signed long
		    new;
		new = TCB->RCV_NXT + SEQcount;
		LOG$FAO("!%T Update RCV_NXT !XL (!UL) to !XL (!UL)!/",
			0,TCB->RCV_NXT,TCB->RCV_NXT,new,new);
		};

// Calculate actual number of octets for the user

	    Ucount = SEQcount-(SEQsize-.datasize);
	    Uoffset = SEQoffset;

// Account for sequence space & queue any user data to the TCB for later
// delivery.

	    TCB->Rcv_Nxt = TCB->RCV_NXT+.SEQcount; !account for accepted bytes
	    if (Ucount > 0)
		{		// acceptable data for user
		signed long
		    dataptr;
		if (TCB->User_timeout != 0)
		    TCB->User_timeout = Time_stamp() + TCB->User_timeval;
		dataptr = Seg+Seg->SH$Data_Offset*4;
		QB->NR$Ucount = Ucount; // number of bytes for user
		QB->NR$Uptr = dataptr+.Uoffset; // point to first data byte
		QB->NR$SEQ_start = SEQstart; // first usable seq #
		QB->NR$SEQ_count = SEQcount; // count of usable seq #s
		RetCode = Queue_Network_Data(TCB,QB); // maintain FIFO
!		if (Retcode == TRUE)	// Don't ACK on error...
!		    TCB->pending_ack = TRUE;
!		    TCP$Send_Ack(TCB);

// If this is a TVT, tell TVT processing that there's new data.

		if (TCB->IS_TVT)
		    {
		    TELNET_INPUT(TCB) ;
		    }
		else

// If we now have enough data to fill the first user receive request, then
// hurry things along by delivering to the user now.

		    {
		    if ((Retcode == TRUE))	// Don't ACK on error...
!		    TCB->pending_ack = TRUE;
			{
			TCP$Enqueue_Ack(TCB);
			} ;

		    IF (QBR != TCB->UR_Qhead) AND
!		       ((TCB->RCV_Q_Count GEQ QBR->UR$Size) OR
		       ((TCB->RCV_Q_Count > 0) || (QB->NR$EOL)) THEN
			{
			TCP$Deliver_User_Data(TCB);
			} ;
		    } ;
		};
	    };
	TES;
	};

!********************************************************
!		Check "FIN" Bit.			!
!********************************************************

    if (Seg->SH$C_FIN && NOT Seg_Trimmed)
	{
!	TCB->Pending_ACK = True;// Flag we need to ACK.
	TCP$Enqueue_ACK(TCB) ;

// Set PUSH pointer to end of this segment to force any current data to be
// pushed. Also, attempt to give the user any data that is left.

	if (TCB->RCV_Q_Count > 0)
	    {
	    TCB->RCV_Push_Flag = TRUE;
	    TCB->RCV_PPtr = Seg->SH$SEQ + SEQsize;
	    if (TCB->IS_TVT)
		TELNET_INPUT(TCB)	// Attempt to give TVT input
	    else
		TCP$Deliver_User_Data(TCB); // Attempt to give user data.
	    };

// If all user data delivered, then flush the user receive queue now.

	if (TCB->RCV_Q_Count == 0)
	    {
	    WHILE (REMQUE(TCB->UR_Qhead,QBR)) != Empty_Queue DO
		{
		User$Post_IO_Status(QBR->UR$Uargs,
				     SS$_NORMAL,0,NSB$PUSHBIT,0);
		MM$UArg_Free(QBR->UR$Uargs); // Free user arg blk.
		MM$QBlk_Free(QBR);// Free Queue Block.
		};
	    };

// Change the Connection State.

	SELECTONE TCB->State OF
	SET
	[CS$Established]:
	    {
	    TCP$Set_TCB_State(TCB,CS$Close_Wait);
	    if ((TCB->IS_Aborted || TCB->IS_TVT))
		TCP$TCB_Close(TCB);
	    };

	[CS$Fin_Wait_1]:
	    TCP$Set_TCB_State(TCB,CS$Closing);

	[CS$Fin_Wait_2]:
	    {

// If no data left or TCB is aborted, then finish closing it now.

    	    if ((TCB->RCV_Q_Count == 0) || TCB->IS_Aborted)
	    	{
    		TCP$Set_TCB_State(TCB,CS$Time_Wait);
		if (NOT TCB->Close_NoWait)
		    TCP$Post_User_Close_IO_Status(TCB,SS$_Normal,0);
		TCB->Time_Wait_Timer = Time_Stamp() + Max_Seg_LifeTime;
		}
	    else
	    	TCB->FIN_RCVD = TRUE;
	    };
	TES;
	};

// *Done with segment*
// Return value indicating whether or not someone is retaining the segment.
// FALSE means that the segment shouldn't be deallocated (it is on someone
// else queue). TRUE or ERROR means OK to deallocate segment.

    return RetCode;
    };



Check_Cum_Ack ( struct tcb_structure * tcb , Idx , P1 , P2 )
    {

    if (TCB->Pending_ACK)
	{
	if (NOT TCB->IS_TVT)
	    TCP$Deliver_User_Data(TCB); // Try to give the user data.
	XLOG$FAO(LOG$TCP,"!%T SEGIN sending cum ACK, TCB=!XL!/",
			     0,TCB);
!	TCP$Send_Ack(TCB); // send the cumulative ACK segment.
	TCP$Enqueue_Ack(TCB); // send the cumulative ACK segment.
	1
	}
    else 0
    };


//SBTTL "Network Segment Arrival Main Processing Loop"
/*

Function:

	Process the network segment queue (SEGIN->SI_Qhead).  This queue
	is built by IP.  When IP receives a datagram from the network it
	handles IP protocols & removes them.  IP then places the TCP segment
	on the SEGIN queue.  Each element in the queue is a standard queue
	block with fields defined by "QB_NR_Fields".  These queue blocks
	point at the actual TCP segments & contain information about the
	segment.

Inputs:

	None.

Implicit Inputs:

	SEGIN queue header ("SEGIN") is valid.

Outputs:

	None.

Side Effects:

	TCP segments are processed according to TCP header fields.  Actual
	segment maybe deleted or queued depending on the segments contents.
*/


LITERAL
    WKS$TELNET = 23;		// Well-known-port for TELNET

SEG$Process_Received_Segments : NOVALUE (void)
    {
    EXTERNAL ROUTINE
	TCB_FIND,
	VTCB_SCAN;
    REGISTER
	struct tcb_structure * tcb,
	struct Segment_Structure * SEG;
    signed long
	RQV,
	struct Queue_Blk_Structure * QB(QB_NR_Fields),
 	sum,
	count,
	Need_2_ACK = False, // assume: NO cumulative ACK's needed.
	IP_Address,
	Delete;
    LABEL
	X,Y;

// Process segments until queue is empty

!~~~WHILE XREMQUE(Segin->SI_Qhead,QB,Process_Received_Segments,Q$SEGIN,0)
!~~~	  != Empty_Queue DO
    WHILE (RQV = REMQUE(Segin->SI_Qhead,QB)) != Empty_Queue DO
	{
	ts$sr = ts$sr + 1;	// count segments received from IP.
	Seg = QB->NR$Seg;	// point at segment proper.
	Delete = True;		// assume we will delete this segment.
	SELECTONE QB->NR$ICMP OF
	SET
	[TRUE]:			// ICMP message for TCP
	    {
	    SwapBytes(QB->NR$Size/2,Seg); // Swap header bytes back

// Find out what connection this is for. The "segment" is the first part of
// the TCP segment we sent out to generate the ICMP reply.

	    TCB = TCB_Find(Seg->SH$Source_Port,QB->NR$Dest_Adrs,
			   Seg->SH$Dest_Port);
	    if (TCB == 0)
		{
		if ($$LOGF(LOG$TCPERR || LOG$ICMP))
		    {
		    signed long
			DESC$STR_ALLOC(fhstr,20);
!!		    ASCII_DEC_BYTES(fhstr,4,QB->NR$Dest_Adrs,
		    ASCII_DEC_BYTES(fhstr,4,QB->NR$Dest_Adrs,
				    fhstr->DSC$W_LENGTH);
		    LOG$FAO("!%T ICMP for unknown TCB,FH=!AS,FP=!XL,LP=!XL!/",
			    0,fhstr,seg->SH$Dest_Port,seg->SH$Source_Port);
		    };
		}
	    else
		{
		XLOG$FAO(LOG$ICMP,
			 "!%T ICMP type !SL for TCB !XL!/",
			 0,QB->NR$ICM_TYPE,TCB);
		SELECTONE QB->NR$ICM_TYPE OF
		SET
		[ICM_DUNREACH]:	// Destination unreachable - treat as RESET
		    {
		    TCP$KILL_P}ING_REQUESTS(TCB,NET$_URC);
		    XLOG$FAO(LOG$TCBSTATE || LOG$ICMP,
			     "!%T TCB !XL killed by ICMP Dest Unreachable!/",
			     0,TCB);
		    TCP$Inactivate_TCB(TCB,NET$_URC);
		    };

		[ICM_TEXCEED]:	// Time exceeded - treat as RESET
		    {
		    TCP$KILL_P}ING_REQUESTS(TCB,NET$_CTO);
		    XLOG$FAO(LOG$TCBSTATE || LOG$ICMP,
			     "!%T TCB !XL killed by ICMP Time Exceeded!/",
			     0,TCB);
		    TCP$Inactivate_TCB(TCB,NET$_CTO);
		    };

		[ICM_SQUENCH]:	// Source quench - not yet supported
		    {
		    XLOG$FAO(LOG$TCBSTATE || LOG$ICMP,
			     "!%T TCB !XL received ICMP Source Quench!/",
			     0,TCB);
		    TCB->SQUENCH = TRUE ;
		    TCB->SQUENCH_Timer = Time_Stamp() + SQUENCH_Interval ;
		    };

		[ICM_REDIRECT]:	// Redirect - not yet supported
		    {
		    XLOG$FAO(LOG$TCBSTATE || LOG$ICMP,
			     "!%T TCB !XL received ICMP Redirect!/",
			     0,TCB);
		    };

		[ICM_PPROBLEM]:	// Parameter problem - not yet supported
		    {
		    XLOG$FAO(LOG$TCBSTATE || LOG$ICMP,
			     "!%T TCB !XL received ICMP Parameter Problem!/",
			     0,TCB);
		    };
		TES;
		};
	    };		// ICMP case

	[FALSE]:		// A real TCP segment
X:	    {

// Good segment. Verify checksum.

	    sum = Gen_Checksum(QB->NR$Size,Seg,QB->NR$Src_Adrs,
			       QB->NR$Dest_Adrs,TCP_Protocol);
	    if (sum NEQU %X"FFFF")
		{		// Checksum error - punt it
		if ($$LOGF(LOG$TCPERR))
		    {
		    LOG$FAO("!%T TCP Checksum error (sum=!XL) for segment:!/",
			    0,sum);
		    SEG$Log_Segment(Seg,QB->NR$Size,True,True);
		    };
		LEAVE X;
		};

	    SwapBytes(TCP_Header_size/2,Seg); // Swap header bytes back
	    Seg->SH$SEQ = ROT(Seg->SH$SEQ,16);
	    Seg->SH$ACK = ROT(Seg->SH$ACK,16);

	    if ($$LOGF(LOG$TCP))
		SEG$LOG_Segment(Seg,QB->NR$Size,True,False);
		

// Now, find the connection that this segment is destined for

	    TCB = TCB_Find (Seg->SH$Dest_Port,QB->NR$Src_Adrs,
			    Seg->SH$Source_Port);

// If no connection found, then check for special controls we can handle.

	    if (TCB == 0)
Y:		{
		SELECTONE TRUE OF
		SET
		[Seg->SH$C_RST]: // RESET segment?
		    {
		    XLOG$FAO(LOG$TCPERR,
			"!%T RST received for unknown TCB, SP=!SL,DP=!SL!/",
			0,Seg->SH$Source_Port,Seg->SH$Dest_Port);
		    LEAVE X;
		    };

		[Seg->SH$C_SYN]: // "SYN" Segment?
		    {
		    signed long
			TMP;

// Check for SYN on well-known-socket (port). If OK, then fork server and queue
// the SYN for later processing on the SYN-wait-list.

		    TMP = Check_WKS(QB->NR$Dest_Adrs,Seg->SH$Dest_Port,
				    QB->NR$Src_Adrs,Seg->SH$Source_Port,
				    QB,Seg);
		    if ((TMP == ERROR)) LEAVE X;
		    if ((TMP == TRUE))
			{
			Delete = FALSE;
			LEAVE X;
			};

// No WKS defined. If TELNET port, then try to create TVT connection.

		    IF (Seg->SH$Dest_Port == WKS$TELNET) AND
		       (TELNET_SERVICE != 0) THEN
			{
			TCB = TELNET_CREATE(QB->NR$Dest_Adrs,
					    Seg->SH$Dest_Port,
					    QB->NR$Src_Adrs,
					    Seg->SH$Source_Port);
			if (TCB != 0)
			    LEAVE Y;
			};

// Connection not found. Give a RESET back.

		    IP_Address = QB->NR$Src_Adrs;
    ACT$FAO("!%D SYN received for unknown port !UW from <!UB.!UB.!UB.!UB>!/",0,
			Seg->SH$Dest_Port,
			.IP_Address<0,8>,IP_Address<8,8>,
			.IP_Address<16,8>,IP_Address<24,8>);
		    Reset_Unknown_Connection(Seg,QB);
		    LEAVE X;
		    };

// Connection not found and not SYN or RST - return an RST.

		[OTHERWISE]:
		    {
		    Reset_Unknown_Connection(Seg,QB);
		    LEAVE X;
		    };
		TES;
		};

// Here when we have a TCP connection. Process the segment.
// ** Warning** TCB/connection maybe deleted during segment processing.
// Condition is flaged by Decode-segment return code of Error(-1).

	    delete = Decode_Segment(TCB,Seg,QB);
	    if (delete != Error)
		{

// Do we need to check for cumulative ACK transmission after the Segment
// input queue is exhausted?

		if (TCB->pending_ack)
		    Need_2_ACK = True;

// See if we can remove some stuff from the future queue

		if (Queue_Not_Empty(TCB->RF_Qhead))
		    Check_Future_Q(TCB);
		};
	    };		// TCP segment case (block X)
	TES;

// Clean up, possibly delete segment & queue-block.
// *** Warning *** "Delete" can be one of 3 values(-1,0,1).  Following test
// checks low-bit only( case covers BOTH 1 & -1).  Be aware!
// check if we can really delete the segment or does somebody else lay claim.

        if ((Delete != 0))
	    {
	    MM$Seg_Free(QB->NR$Buf_Size,QB->NR$Buf);
	    MM$QBLK_Free(QB);
	    };
	};			// "While"

// Check if any connections need to have a cumulative ACK transmitted.
// If True then check all valid connections & if the TCB->Pending_ACK
// bit is set then Attempt to deliver user data & send an ACK (cumulative).
// Otherwise continue checking connections.

    if (Need_2_ACK)
	VTCB_Scan ( Check_Cum_Ack , 0 , 0 );

    };
}
ELUDOM
