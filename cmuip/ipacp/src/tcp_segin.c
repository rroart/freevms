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
//TITLE "TCPSEGIN - Process TCP network input segments"
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
    This version by Vince Fuller, CMU-CSD, Summer, 1987
    Copyright (c) 1986, 1987, Vince Fuller and Carnegie-Mellon University

Modification History:

*** Begin USBR change log ***

6.7 05-Dec-1991 Henry W. Miller     USBR
    Use TCB->snd_ack_threshold instead of ACK_THRESHOLD.
    Set TCB->snd_ack_threshold from TCB->max_seg_data_size.  Sort of
    RFC1122 compliant.

6.6 14-Nov-1991 Henry W. Miller     USBR
    In SEG$Process_Received_Segments(), log various types of currently
    unsupported ICMP segments.
    In Decode_Segment(), let TELNET module ACK it's own data, clean up
    logging.
    Change arithmetic and printout on timers from signed to unsigned.
    Call TCP$Enqueue_ACK() rather than just setting PENDING_ACK or
    calling TCP$Send_ACK() directly.

6.5f    30-Aug-1991 Henry W. Miller     USBR
    In DECODE_SEGMENT(), queue user data if RCV_Q_COUNT is greater then
    zero, not just if it"s big enough to fill user"s buffer.
    This fixes the long outstanding dreaded TELNET pause syndrome.
    (At least here it does...)

6.5e    11-Mar-1991 Henry W. Miller     USBR
    Fix port comparison logic.

6.5d    27-Feb-1991 Henry W. Miller     USBR
    Fix port comparison logic.

6.5c    24-Jan-1991 Henry W. Miller     USBR
    Fixed logging statements to print timestamp.

6.5b    15-Jan-1991 Henry W. Miller     USBR
    Changed SND_WND, RCV_WND, and Old_RCV_WND to unsigned long values
    as recommended by RFC1122.

6.5a    13-Jan-1991 Henry W. Miller     USBR
    In SEG$Log_Segment(), print checksum, data pointer and urgent
    pointer as unsigned word values.
    Updated IDENT to 6.5a.

*** Begin CMU change log ***

6.4 16-MAR-1989 Dale Moore  CMU-CS
    If we aren"t doing Keep alives, don"t timeout connections.

    06-JUN-1988 Dale Moore  CMU-CS/RI
    Check to see if we are truncated the data received before
    we process FIN bit.  We might truncate the data if the
    window size is smal.

6.3 14-JAN-1988 Dale Moore  CMU-CS/RI
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
    In Decode_Segment - set Ucount to 0 if text is not usable (i.e. not in
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
    Make ACK_RT_QUEUE call SEND_DATA if it actually removes anything from
    the RT queue. Idea is to make SEND processing event driven.
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

#if 0
MODULE SEGIN(IDENT="6.7",LANGUAGE(BLISS32),
             ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
                             NONEXTERNAL=LONG_RELATIVE),
             LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
             OPTIMIZE,OPTLEVEL=3,ZIP)=
#endif

#ifdef __i386__
#include <net/checksum.h>
#endif
#include <starlet.h>    // VMS system definitions

                 // not yet #include <cmuip/central/include/netxport.h> // BLISS transportablity package
#include <cmuip/central/include/neterror.h> // Network error codes
#include "netvms.h" // VMS specifics
#include <cmuip/central/include/netcommon.h>// Network common defs
#include <cmuip/central/include/nettcpip.h> // TCP/IP protocols

#include "structure.h"      // TCB & Segment Structure definitions
#include "cmuip.h" // needed before tcpmacros.h
#include "tcpmacros.h"      // Local macros
#include "tcp.h"            // TCP related definitions
#include "snmp.h"           // Simple Network Management Protocol

#include <ssdef.h>
#include <descrip.h>

#undef TCP_DATA_OFFSET
#ifdef __x86_64__
#include <net/checksum.h>
#endif
#define TCP_DATA_OFFSET 5
#define Calc_Checksum(x,y) ip_compute_csum(y,x)
#define Gen_Checksum(a,b,c,d,e) csum_tcpudp_magic(c,d,a,e,csum_partial(b,a,0))

#ifndef NOKERNEL
#define sys$fao exe$fao
#endif

//XQDEFINE;

                 extern struct TCP_MIB_struct *     tcp_mib ;   // TCP management Information Block

extern
LIB$GET_VM();

// USER.BLI

extern void    user$post_io_status();

// TCP_USER.BLI

extern void    tcp$post_active_open_io_status();
extern void    tcp$post_user_close_io_status();
extern  void    tcp$kill_pending_requests();
extern  void    tcp$deliver_user_data();
//extern     Gen_CheckSum();
extern  void    tcp$adlook_done();

// TCP_TELNET.BLI

extern     TELNET_CREATE();
extern     TELNET_OPEN();
extern  void    TELNET_INPUT();
extern  void    TELNET_OUTPUT();

// MEMGR.BLI

extern     tcb$create();
extern  void    tcb$delete();
extern  void    mm$seg_free();
// not yet extern  void    mm$qblk_free();
extern     mm$qblk_get();
extern  void    mm$uarg_free();

// TCP.BLI

extern      tcp$send_data();
extern     tcp$send_ctl();
extern  void    tcp$enqueue_ack();
extern  void    tcp$send_ack();
extern  void    tcp$dump_tcb();
extern  void    tcp$inactivate_tcb();
extern  void    tcp$set_tcb_state();
extern  void    cq_enqueue();
extern     tcp$tcb_close();
extern     tcp$compute_rtt();

// NMLOOK.BLI

extern  void    NML$GETNAME();

// MACLIB.MAR

extern     swapbytes();

// IOUTIL.BLI

extern  void    ASCII_Hex_Bytes();
extern  void    ASCII_Dec_Bytes();
extern  void    Log_Time_Stamp();
#if 0
extern  void    OPR_FAO();
#endif
extern  void    LOG_FAO();
extern  void    LOG_OUTPUT();
extern  void    ACT_FAO();
extern  void    ACT_OUTPUT();

extern unsigned long CONN_TIMEVAL;
extern  struct queue_header_structure(si_fields)  *  segin ;
extern signed long
intdf,
ast_in_progress,
myuic,          // TCP's UIC.
log_state,
//    struct Connection_table_Structure * ConectPtr VOLATILE,

fq_max,         // Max segments allowed on future queue.
max_recv_datasize,      // Configured Maximum Segment Size
telnet_service,     // Flag if internal TELNET server is enabled
keep_alive,         // Flag if Keep alives used

// tcp stats

ts$seg_bad_cksum,       // bad segment checksum
ts$servers_forked,      // # of well-known servers forked.
ts$aco,         // Active opens that were established.
ts$duplicate_segs,      // total duplicate segments received.
ts$oorw_segs,       // Total Out Of Recv Window segs received.
ts$future_rcvd,     // # received in window but after RCV.NXT
ts$future_dups,     // # of future duplicates dropped
ts$future_used,     // # of such used
ts$future_dropped,      // # of such dropped
ts$badseq,          // Packets dropped in Queue_Network_data
ts$abort_drops,     // packets dropped because TCB aborted
ts$qfull_drops,     // packets dropped because TCB NR queue full
ts$sr;          // segments recevied.

// Forward declarations

void    SEG$Log_Segment();

#define GLOBAL_MINSRV 1
#define GLOBAL_MAXSRV 30

//SBTTL "Declaration of WKS server process table & SYN-wait list"

struct wks_structure WKS_LIST[WKS_TABLE_SIZE];
signed long
wks_count  = 0,
syn_wait_count  = 0;

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

#define     ALPHA 90            // Smoothing constant, per RFC973, pg 41
#define     BETA   150          // Delay variance factor
#define     ALPHACOMP   (100 - ALPHA)   // "Compliment" of ALPHA (i.e. 1 - ALPHA)
#define     MINRTT   1          // Minimum reasonable RTT
#define     MINSRTT   10        // Minimum smoothed RTT

void ack_rt_queue(struct tcb_structure * TCB, signed long AckNum)
{
    unsigned long
    Delta,
    crtt;
    signed long
    Tmp,
    deqc,
    oldcount;

    oldcount = TCB->srx_q_count;
    deqc = AckNum - TCB->rx_seq;
    XLOG$FAO(LOG$TCP,
             "!%T ACK-RXQ: TCB=!XL, SEQ=!XL, PTR=!XL, CNT=!SL!/!%T ACK-RXQ: CTL=!SL, ACK=!XL, DEQC=!SL!/",
             0,TCB,TCB->rx_seq,TCB->srx_q_deqp,TCB->srx_q_count,
             0,TCB->rx_ctl,AckNum,deqc);

// If the ACK number is greater than the RX sequence, then we are acknowleging
// at least part of the RX space.

    if (deqc > 0)
    {
        unsigned long oldrtt;
        signed long
        newc,
        oldcount;

// Clear SYN flag if we acked anything, since it is the first sequence number
// Also decrement number of octets we acked - SYN takes up one sequence number.

        TCB->rx_seq = TCB->rx_seq + deqc;
        if (TCB->rx_ctl != 0)
            if ((TCB->rx_ctl == M$SYN) || (TCB->rx_ctl == M$SYN_ACK))
            {
                TCB->rx_ctl = 0;
                deqc = deqc - 1;
            };

// Clear retransmission count

        TCB->rx_count = 0;

// Calculate number of bytes removed from RX data queue.
// newc LSS 0 SHOULD mean that a FIN has been acked.

        newc = TCB->srx_q_count - deqc;
        if (newc < 0)
        {
            if (TCB->rx_ctl == M$FIN)
                TCB->rx_ctl = 0;
            else
                XLOG$FAO(LOG$TCPERR,"!%T ?RX-ACK - newc = !SL!/",0,newc);
            newc = 0;
        };

// Update RX queue count and pointer.

        TCB->srx_q_count = newc;
        if (newc == 0)
            TCB->srx_q_deqp = TCB->snd_q_deqp;
        else
        {
            signed long
            newptr;
            newptr = TCB->srx_q_deqp + deqc;
            if (newptr > TCB->srx_q_end)
                newptr = ((unsigned long)TCB->srx_q_base) + (newptr - ((unsigned long)TCB->srx_q_end));
            TCB->srx_q_deqp = newptr;
        };

// Compute round trip time for adaptive retransmission.

        tcp$compute_rtt(TCB) ;
//  oldrtt = TCB->round_trip_time;
//  crtt = MAXU(Time_Stamp()-TCB->Xmit_Start_Time,MINRTT);

// Compute smoothed round trip time, see RFC793 (TCP) page 41 for details.

//  delta = ((TCB->round_trip_time*ALPHA)/100) +
//      ((crtt*ALPHACOMP)/100);
//  TCB->round_trip_time = delta;
//  delta = (BETA*.delta)/100;
//  TCB->calculated_rto = MINU(MAX_RT_TIMEOUT,MAXU(delta,MIN_RT_TIMEOUT));

//  XLOG$FAO(LOG$TCP,
//      "!%T ACK-RXQ: Prev SRTT=!UL, Cur RTT=!UL, New SRTT=!UL, RTO=!UL!/",
//       0,oldrtt,crtt,TCB->round_trip_time,TCB->calculated_rto);
    };

// If TVT and send queue just became non-full, try to get some more TVT data

    if (TCB->is_tvt)
        if ((oldcount + TCB->snd_q_count) >= TCB->snd_q_size &&
                (oldcount > TCB->srx_q_count))
        {
            TELNET_OUTPUT(TCB);
        };

// If the RT queue is now empty, see if data needs to be sent.
// Since we implement the "Nagle" algorithm, the data send routine is
// effectively blocked by anything on the RT queue.

    if (TCB->srx_q_count == 0)
        tcp$send_data(TCB);
}

void SEG$Purge_RT_Queue(struct tcb_structure * TCB)
//
// Routine to clean out the retransmission queue. All this really involves
// is clearing the RX count and flags and advancing the RX pointer to the
// send dequeue pointer.
//
{
    TCB->srx_q_count = 0;   // Clear retransmit byte count
    TCB->srx_q_deqp = TCB->snd_q_deqp; // Empty retransmit queue
    TCB->rx_ctl = 0;        // And clear retransmit flags
}

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


void Send_Reset(struct tcb_structure * TCB,signed int SEQNum)
{
    signed long
    Sav$Snd_Nxt;

// save send next sequence # & use SEQnum instead.

    Sav$Snd_Nxt = TCB->snd_nxt;
    TCB->snd_nxt = SEQNum;

// Build the "RESET" segment & give it to IP for transmission.

    send_rst(TCB);

// Restore the world to its proper state.

    TCB->snd_nxt = Sav$Snd_Nxt;
}

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


void reset_unknown_connection(Seg,QB)
struct segment_structure * Seg;
struct queue_blk_structure(qb_nr_fields) * QB;
{
    register
    struct tcb_structure * TCB;

// Create & fill in the temporary TCB

    TCB = tcb$create();
    TCB->lp_next = TCB->lp_back = &TCB->lp_next;    // Init Local Port queue.
    TCB->local_port = Seg->sh$dest_port & 0xffff ;
    TCB->local_host = QB->nr$dest_adrs;
    TCB->foreign_port = Seg->sh$source_port & 0xffff ;
    TCB->foreign_host = QB->nr$src_adrs;
    TCB->state = CS$ESTABLISHED;
    TCB->rcv_wnd = TCB->snd_wnd = TCB->rcv_nxt = 100;

// If the ACK bit (received seg) is TRUE, then <SEQ=SEG_ACK><CTL=RST>
// else  <SEQ=0><ACK=SEG_SEQ+SEG_SEQ_SPACE><CTL=RST,ACK>

    if (Seg->sh$c_ack)
        TCB->snd_nxt = Seg->sh$ack;
    else
    {
        TCB->snd_nxt = 0;
        TCB->rcv_nxt = Seg->sh$seq + (QB->nr$size - Seg->sh$data_offset*4)
                       + Seg->sh$c_syn + Seg->sh$c_fin;
    };

// Send the reset.

    XLOG$FAO(LOG$TCPERR,
             "!%T RESET for unknown conn, ACK=!XL (!UL), SEQ=!XL (!UL)!/",
             0,Seg->sh$seq,Seg->sh$seq,Seg->sh$ack,Seg->sh$ack);

    send_rst(TCB);

// Delete the TCB

    tcb$delete(TCB);
}

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


queue_network_data(TCB,QB)
struct tcb_structure * TCB;
struct queue_blk_structure(qb_nr_fields) * QB;
{
    register
    Ucount,
    Uptr;
#define     NR_QMAX   8     // Max segs on receive queue per connection

// Is the TCB aborted? If so, just toss the data.

    if (TCB->is_aborted)
    {
        XLOG$FAO(LOG$TCPERR,
                 "!%T QND: TCB aborted, drop QB !XL, seg !XL, SEQ !XL/!XL!/",
                 0,QB,QB->nr$buf,QB->nr$seq_start,QB->nr$seq_end);
        ts$abort_drops = ts$abort_drops+1;
        return ERROR;       // Deallocate this segment
    };

// Set push flag and pointer if the segment has it on

    if (((QB->nr$eol) || (TCB->rcv_wnd <= TCB->snd_ack_threshold)))
    {
        TCB->rcv_push_flag = TRUE;
        TCB->rcv_pptr = QB->nr$seq_end;
        XLOG$FAO(LOG$TCP,
                 "!%T QND: EOL set, Q cnt !UW !/", 0, TCB->rcv_q_count);
    };

// Enqueue the data onto the receive circular queue

    Ucount = QB->nr$ucount; // Count of user data in segment
    Uptr = QB->nr$uptr; // Address of data
    Ucount = MIN(Ucount,TCB->rcv_q_size-TCB->rcv_q_count);
    if (Ucount <= 0)
    {
        XLOG$FAO(LOG$TCPERR,
                 "!%T QND: TCB rcv q full, drop QB !XL, seg !XL, SEQ !XL/!XL!/",
                 0,QB,QB->nr$buf,QB->nr$seq_start,QB->nr$seq_end);
        ts$qfull_drops = ts$qfull_drops+1;
        return ERROR;       // Deallocate this segment
    };

// Append the segment data to the queue and deallocate it

    XLOG$FAO(LOG$TCP,
             "!%T QND: ENQ !SL from !XL,EQ=!XL,DQ=!XL,RCQ=!XL/!XL!/",
             0,Ucount,Uptr,TCB->rcv_q_enqp,TCB->rcv_q_deqp,
             TCB->rcv_q_base,TCB->rcv_q_end);
    cq_enqueue(&TCB->rcv_q_queue,Uptr,Ucount);
    XLOG$FAO(LOG$TCP,
             "!%T QND: Q cnt !UW !/", 0, TCB->rcv_q_count);

// Adjust the receive window to reflect the resources used by the data bearing
// portion of this segment.

    if ((TCB->rcv_wnd < QB->nr$seq_count))
    {
        XLOG$FAO(LOG$TCPERR,"!%T Segin(queue_net_data) RCV_WND (!UL) < SEQ_CNT (!UL)!/",
                 0, TCB->rcv_wnd, QB->nr$seq_count);
        TCB->rcv_wnd = 0 ;
    }
    else
    {
        TCB->rcv_wnd = TCB->rcv_wnd - QB->nr$seq_count;
    } ;

    return TRUE;
}

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

void seg$input(Src$Adrs,Dest$Adrs,BufSize,Buf,SegSize,Seg)
{
    struct queue_blk_structure(qb_nr_fields) * QB;

    QB = mm$qblk_get();
    QB->nr$buf_size = BufSize;  // Total size of network buffer.
    QB->nr$buf = Buf;       // Start adrs of network buffer.
    QB->nr$size = SegSize;  // byte size of segment within network buffer.
    QB->nr$seg = Seg;       // Start adrs of segment within net buf.
    QB->nr$src_adrs = Src$Adrs;// Source internet adrs.
    QB->nr$dest_adrs = Dest$Adrs; // destination internet adrs.
    QB->nr$flags = 0;       // Clear input flags
    INSQUE(QB,segin->si_qtail); // queue to tail of segment in queue.
//~~~XINSQUE(QB,segin->si_qtail,TCP_Input,Q$SEGIN,segin->si_qhead);

    tcp_mib->MIB$tcpInSegs = tcp_mib->MIB$tcpInSegs + 1;
}

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

void SEG$ICMP(ICMtype,ICMex,ipsrc,ipdst,Seg,Segsize,
              buf,bufsize)

//ICMtype - ICMP packet type
//ICMex - Extra data from ICMP packet (pointer for ICM_PPROBLEM)
//IPsrc - Source address of offending packet
//IPdst - Destination address of offending packet
//Seg - First 64-bits of data from offending packet (will have ports)
//Segsize - Calculated octet count of Seg
//Buf - address of network buffer, released by TCP later
//Bufsize - size of network buffer

{
    struct queue_blk_structure(qb_nr_fields) * QB;

    QB = mm$qblk_get();
    QB->nr$buf_size = bufsize;  // Total size of network buffer.
    QB->nr$buf = buf;       // Start adrs of network buffer.
    QB->nr$size = 8;        // byte size of segment within network buffer.
    QB->nr$seg = Seg;       // Start adrs of segment within net buf.
    QB->nr$src_adrs = ipsrc;    // Source internet adrs (us)
    QB->nr$dest_adrs = ipdst;   // destination internet adrs.
    QB->nr$flags = 0;
    QB->nr$icmp = TRUE;     // Indicate that this is an ICMP packet
    QB->nr$icm_type = ICMtype;  // Remember ICMP type
    QB->nr$icm_ex = ICMex;  // and extra information
    INSQUE(QB,segin->si_qtail); // queue to tail of segment in queue.
    return;
}

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

Decode_Segment();

void SEG$Check_SYN_Wait_List(struct tcb_structure * TCB)
{
    struct queue_blk_structure(qb_nr_fields) * QB;
    struct segment_structure * Seg;
    signed long I,
           LP,
           WIX;

// Find the WKS entry for the port

    LP = TCB->local_port & 0xffff ;
X:
    {
        for (I=0; I<=(wks_count-1); I++)
            if (LP == WKS_LIST[I].WKS$Port)
            {
                WIX = I;
                goto leave_x;
            };
        return;         // Port not found - no SYN pending
    }
leave_x:

// Point at the head of the SYN wait list for this port

    QB = WKS_LIST[WIX].WKS$SYN_Qhead;

// Search the list looking for a match on Foreign host & port.
// Handle wild-card foreign host & port cases.

Y:
    {
        while ((QB != &WKS_LIST[WIX].WKS$SYN_Qhead))
        {
            if ((TCB->foreign_host == WILD) ||
                    (TCB->foreign_host == QB->nr$src_adrs))
                if ((TCB->foreign_port == WILD) ||
                        (TCB->foreign_port == QB->nr$src_port))
                    goto leave_y;
            QB = QB->nr$next;
        };
        return;
    }
leave_y:

// Here on match. Remove the SYN segment from the list & process it.

    XLOG$FAO(LOG$TCP,"!%T SYN-wait-list match,TCB=!XL,QB=!XL,Seg=!XL!/",
             0,TCB,QB,QB->nr$seg);
    REMQUE(QB,&QB);     // Remove entry from syn-wait-list.
    syn_wait_count = syn_wait_count + 1;
    WKS_LIST[WIX].WKS$SYN_Qcount = WKS_LIST[WIX].WKS$SYN_Qcount + 1;
    Seg = QB->nr$seg;       // point at segment.
    if (Decode_Segment(TCB,Seg,QB))
    {
        // Delete segment (match = [-1,0,1]).
        mm$seg_free(QB->nr$buf_size,QB->nr$buf);
        mm$qblk_free(QB);
    };
}

//SBTTL "Fork a Server Process"
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

Fork_Server(Idx, IP_Address, Remote_Port)
{
#define PNAMLEN 20
    DESC$STR_ALLOC(ProcName,PNAMLEN);
    signed long J,
           RC,
           RP,
           NewPID;
    extern  PokeAddr ();


    RP = (Remote_Port & 0xffff) ;

// Try a bunch of times to find one with a non-duplicate name.

    for (J=GLOBAL_MINSRV; J<=WKS_LIST[Idx].WKS$MaxSrv; J++)
    {
        ProcName->dsc$w_length = PNAMLEN;
        $DESCRIPTOR(ctr,"!AS.!SL");
        sys$fao(&ctr,&ProcName->dsc$w_length,ProcName,
                WKS_LIST[Idx].WKS$Process,J);

#if 0
        // check wait
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
#else
        RC = 0;
        NewPID = 0;
#endif

// See if it worked.

        switch (RC)
        {
        case SS$_NORMAL:        // Success - done
        {
            XLOG$FAO(LOG$TCP,"!%T Forked Server: !AS!/",0,ProcName);

            ACT$FAO("!%D Forked Server: !AS(PID:!XW) <!UB.!UB.!UB.!UB>!/",0,
                    ProcName, NewPID&0xffff,
                    IP_Address&0xff,(IP_Address>>8)&0xff,
                    (IP_Address>>16)&0xff,(IP_Address>>24)&0xff
                   );
            ts$servers_forked = ts$servers_forked + 1; // count the servers.
            PokeAddr(NewPID, IP_Address, RP);
            return TRUE;
        };
        break;

        case SS$_DUPLNAM:       // Duplicate name - try next name
            0;  // check return
            break;

        default:        // Hard failure
        {
            XLOG$FAO(LOG$TCPERR,"%T Server CREPRC Failed for !AS, RC = !XL!/",
                     0,ProcName,RC);
            return FALSE;
        };
        };
    };

// Failed after max number of tries. Log error and return failure.

    XLOG$FAO(LOG$TCPERR,"!%T Failed to fork server !AS!/",0,ProcName);
    return FALSE;
}

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

Check_WKS(DHost,DPort,SHost,SPort,QBNEW,SegNew)
struct queue_blk_structure(qb_nr_fields) * QBNEW;
struct segment_structure * SegNew;
{
    struct queue_blk_structure(qb_nr_fields) * QB;
    struct segment_structure * Seg;
    signed long I,
           DP,
           SP,
           WIX;

    DP = DPort & 0xffff ;
    SP = SPort & 0xffff ;

// See if we know about the port

X:
    {
        for (I=0; I<=(wks_count-1); I++)
            if (DP == WKS_LIST[I].WKS$Port)
            {
                WIX = I;
                goto leave;
            };
        return FALSE;
    }
leave:

// See if main queue count exceeded

    if (syn_wait_count <= 0)
    {
        XLOG$FAO(LOG$TCP,"!%T SYN wait list full for SYN on WKS !SL!/",
                 0,DPort);
        return FALSE;
    };

// See if queue count for this WKS exceeded

    if (WKS_LIST[WIX].WKS$SYN_Qcount <= 0)
    {
        XLOG$FAO(LOG$TCP,"!%T SYN wait list for port !SL full!/",0,DPort);
        return FALSE;
    };

// Next, check for duplicate SYN segments on the SYN wait list - drop them.

    XLOG$FAO(LOG$TCP,"!%T SYN for WKS !SL received!/",0,DPort);
    QB = WKS_LIST[WIX].WKS$SYN_Qhead;
    while ((QB != &WKS_LIST[WIX].WKS$SYN_Qhead))
    {

// Check IP addresses.

        if ((DHost == QB->nr$dest_adrs) &&
                (SHost == QB->nr$src_adrs))
        {
            Seg = QB->nr$seg;   // point at TCP segment

// Check TCP ports. If duplicate, return "error" so it will be deallocated.

            if ((DP == Seg->sh$dest_port) &&
                    (SP == Seg->sh$source_port))
            {
                XLOG$FAO(LOG$TCP,"!%T Dup SYN on Syn-wait list dropped!/");
                return ERROR;
            };
        };
        QB = QB->nr$next;   // Look at next element.
    };

// Fire up a server to handle it and queue up the segment on success.

    if (Fork_Server(WIX, SHost, SP))
    {

// Fill in Queue block fields for quick checking during processing of SYN wait
// list.  This prevents having to set up a segment pointer, after all we have
// extra space in the queue block so why not use it.

        QBNEW->nr$src_port = SegNew->sh$source_port;
        QBNEW->nr$dest_port = SegNew->sh$dest_port;
        QBNEW->nr$timeout = Time_Stamp() + MAX_SEG_LIFETIME;

// Save this for later processing.

        INSQUE(QBNEW,WKS_LIST[WIX].WKS$SYN_Qtail);
        WKS_LIST[WIX].WKS$SYN_Qcount = WKS_LIST[WIX].WKS$SYN_Qcount-1;
        syn_wait_count = syn_wait_count - 1;
        return TRUE;
    }
    else
        return FALSE;
}

//SBTTL "Timeout_Syn_Wait_List - Check SYN wait list for expired entries"

void SEG$Timeout_Syn_Wait_List(unsigned long long Now)
{
    struct queue_blk_structure(qb_nr_fields) * QB;
    signed long
    WIX,
    tmp;

    for (WIX=0; WIX<=(wks_count-1); WIX++)
    {
        QB = WKS_LIST[WIX].WKS$SYN_Qhead;
        while (QB != &WKS_LIST[WIX].WKS$SYN_Qhead)
        {
            if (QB->nr$timeout < Now)
            {
                // Timed-out
                REMQUE(QB,&QB); // Remove queue entry.
                WKS_LIST[WIX].WKS$SYN_Qcount=WKS_LIST[WIX].WKS$SYN_Qcount+1;
                syn_wait_count = syn_wait_count + 1;
                mm$seg_free(QB->nr$buf_size,QB->nr$buf); // Release the seg.
                tmp = QB;   // tmp Qblk pointer.
                QB = QB->nr$next; // next entry.
                mm$qblk_free(tmp);// & the queue block.
            }
            else
                QB = QB->nr$next;   // point at next entry.
        };
    };
}

//SBTTL "ADD_WKS - Add an entry to the WKS table"
/*
    Called by the CONFIG module when a WKS command is seen, this routine adds
    the new WKS entry to the table.
 */

void seg$wks_config(PORT,PRNAME_A,IMNAME_A,STAT,PRIV,
                    PRIOR,QLIMIT,MAXSRV,Quotas,
                    INPUT_A, OUTPUT_A, ERROR_A)
struct dsc$descriptor * Quotas;
{
    struct dsc$descriptor * dsc;
    unsigned long // was: $BBLOCK
    PRNAME  = PRNAME_A      ,
    IMNAME = IMNAME_A      ,
    INPUT = INPUT_A       ,
    OUTPUT  = OUTPUT_A      ,
    Error  = ERROR_A       ;
    extern  STR$COPY_DX ();
    signed long
    WIX,
    QUOPTR,
    Status;

// Make sure there is room for this entry

    if ((wks_count >= WKS_TABLE_SIZE))
    {
        OPR$FAO("Too many WKS entries in INET$CONFIG - entry for !AS ignored",
                PRNAME);
        return;
    };
    WIX = wks_count;
    wks_count = wks_count + 1;


// Fill in the WKS entry fields

    WKS_LIST[WIX].WKS$Port = PORT;

    // Process Name
    dsc = & WKS_LIST[WIX].WKS$Process;
    dsc->dsc$w_length = 0;
    dsc->dsc$b_dtype = DSC$K_DTYPE_T;
    dsc->dsc$b_class = DSC$K_CLASS_D;
    dsc->dsc$a_pointer = 0;
    Status = STR$COPY_DX (&WKS_LIST[WIX].WKS$Process, PRNAME);
    if (! Status) Signal (Status);

    // Process image file
    dsc = & WKS_LIST[WIX].WKS$IMAGE;
    dsc->dsc$w_length = 0;
    dsc->dsc$b_dtype = DSC$K_DTYPE_T;
    dsc->dsc$b_class = DSC$K_CLASS_D;
    dsc->dsc$a_pointer = 0;
    Status = STR$COPY_DX (&WKS_LIST [WIX].WKS$IMAGE, IMNAME);
    if (! Status) Signal (Status);

    // Process input stream
    dsc = & WKS_LIST[WIX].WKS$Input;
    dsc->dsc$w_length = 0;
    dsc->dsc$b_dtype = DSC$K_DTYPE_T;
    dsc->dsc$b_class = DSC$K_CLASS_D;
    dsc->dsc$a_pointer = 0;
    Status = STR$COPY_DX (&WKS_LIST[WIX].WKS$Input, INPUT);
    if (! Status) Signal (Status);

    // Process output stream
    dsc = & WKS_LIST[WIX].WKS$Output;
    dsc->dsc$w_length = 0;
    dsc->dsc$b_dtype = DSC$K_DTYPE_T;
    dsc->dsc$b_class = DSC$K_CLASS_D;
    dsc->dsc$a_pointer = 0;
    Status = STR$COPY_DX (&WKS_LIST[WIX].WKS$Output, OUTPUT);
    if (! Status) Signal (Status);

    // Process error stream
    dsc = & WKS_LIST[WIX].WKS$Error;
    dsc->dsc$w_length = 0;
    dsc->dsc$b_dtype = DSC$K_DTYPE_T;
    dsc->dsc$b_class = DSC$K_CLASS_D;
    dsc->dsc$a_pointer = 0;
    Status = STR$COPY_DX (&WKS_LIST[WIX].WKS$Error, ERROR);
    if (! Status) Signal (Status);

    // Allocate the block for the process quota list
    if (Quotas->dsc$w_length > 0)
    {
        Status = LIB$GET_VM(/*%REF*/(Quotas->dsc$w_length),&QUOPTR);
        if (! Status)
            FATAL$FAO("Seg$WKS_Config - failed to allocate quolst, RC = !XL",Status);
        CH$MOVE(Quotas->dsc$w_length,Quotas->dsc$a_pointer,QUOPTR);
        WKS_LIST[WIX].WKS$Quotas = QUOPTR;
    }
    else WKS_LIST[WIX].WKS$Quotas = 0;

    WKS_LIST[WIX].WKS$Stat = STAT;
    CH$MOVE(8,PRIV,&WKS_LIST[WIX].WKS$Priv);
    WKS_LIST[WIX].WKS$Prior = PRIOR;
    if (MAXSRV == 0)
        WKS_LIST[WIX].WKS$MaxSrv=GLOBAL_MAXSRV;
    else
        WKS_LIST[WIX].WKS$MaxSrv=MAXSRV;
    WKS_LIST[WIX].WKS$SYN_Qcount = QLIMIT;
    WKS_LIST[WIX].WKS$SYN_Qhead = WKS_LIST[WIX].WKS$SYN_Qhead;
    WKS_LIST[WIX].WKS$SYN_Qtail = WKS_LIST[WIX].WKS$SYN_Qhead;
}

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

void SEG$Log_Segment(struct segment_structure * seg,signed long size,
                     signed long TR_Flag,signed long BS_Flag)
{
    struct segment_structure segcopy_, * segcopy=&segcopy_;
    struct segment_structure * seghdr;
    signed long
    dataoff;
    struct dsc$descriptor   sptr;

    seghdr = seg;
    if (BS_Flag)        // Need to byteswap the header...
    {
        CH$MOVE(TCP_HEADER_SIZE,CH$PTR(seg,0),CH$PTR(segcopy,0));
        seghdr = segcopy;
        swapbytesseghdr(TCP_HEADER_SIZE/2,seghdr); // Swap header bytes back
        seghdr->sh$seq = ntohl(seghdr->sh$seq/*,16*/);
        seghdr->sh$ack = ntohl(seghdr->sh$ack/*,16*/);
    };

// Point at segment data (past header and possible options)

    dataoff = seghdr->sh$data_offset*4;

    if (TR_Flag)
        sptr = ASCID("Received");
    else
        sptr = ASCID("Sent");

// Write title line...

    LOG$FAO("!%T !AS Network Segment, SEG=!XL, OPT=!XL, DATA=!XL:!/",
            0,&sptr,seg,seg+TCP_HEADER_SIZE,seg+dataoff);

// Log most of the data - N.B. We don't write the final CRLF yet...

    LOG$FAO("!_SrcPrt:!_!XL (!UL)!_DstPrt:!_!XL (!UL)!/!_SEQnum:!_!XL (!UL)!_ACKnum:!_!XL (!UL)!/!_Window:!_!UW!_CKsum:!_!UW!_DatPtr:!_!UW!_UrgPtr:!_!UW!/!_Control Flags:!_!XL",
            seghdr->sh$source_port,seghdr->sh$source_port,
            seghdr->sh$dest_port,seghdr->sh$dest_port,
            seghdr->sh$seq,seghdr->sh$seq,seghdr->sh$ack,seghdr->sh$ack,
            seghdr->sh$window,seghdr->sh$checksum,seghdr->sh$data_offset,
            seghdr->sh$urgent,((short*)&seghdr->sh$ack)[2] /* was: seghdr->sh$c_all_flags*/);
    if (1 /* was: seghdr->sh$c_all_flags != 0*/)
    {
        LOG$OUT(" = ");
        if (seghdr->sh$c_urg)  LOG$OUT("URG ");
        else if (seghdr->sh$c_ack)  LOG$OUT("ACK ");
        else if (seghdr->sh$c_eol)  LOG$OUT("EOL ");
        else if (seghdr->sh$c_rst)  LOG$OUT("RST ");
        else if (seghdr->sh$c_syn)  LOG$OUT("SYN ");
        else if (seghdr->sh$c_fin)  LOG$OUT("FIN ");
    };
#if 0
    LOG$OUT(%CHAR(13,10));
#endif

    if (dataoff > TCP_HEADER_SIZE)
    {
#define     maxopt 20
        DESC$STR_ALLOC(optstr,maxopt*3);
        signed long
        optcnt,
        outcnt;
        optcnt = dataoff-TCP_HEADER_SIZE;
        if (optcnt > maxopt)
            outcnt = maxopt;
        else
            outcnt = optcnt;
        ASCII_HEX_BYTES(optstr,outcnt,seg+TCP_HEADER_SIZE,
                        &optstr->dsc$w_length);
        LOG$FAO("!_Options:!_!SL = !AS!/",optcnt,optstr);
    };

    if (size > dataoff)
    {
#define     maxhex 20
#define     maxasc 50
        DESC$STR_ALLOC(dathex,maxhex*3);
        signed long
        datcnt,
        asccnt,
        hexcnt;
        datcnt = size-dataoff;
        if (datcnt > maxasc)
            asccnt = maxasc;
        else
            asccnt = datcnt;
        if (datcnt > maxhex)
            hexcnt = maxhex;
        else
            hexcnt = datcnt;
        ASCII_HEX_BYTES(dathex,hexcnt,seg+dataoff,&dathex->dsc$w_length);
        LOG$FAO("!_Data Count: !SL!/!_HEX:!_!AS!/!_ASCII:!_!AF!/",
                datcnt,dathex,asccnt,seg+dataoff);
    };
}



//SBTTL "Append a segment to the end of the "future" queue"
/*
 */

Append_Future_Q(TCB, QB, Seg, SEQsize)
struct tcb_structure * TCB;
struct queue_blk_structure(qb_nr_fields) * QB;
struct segment_structure * Seg;
{
    struct queue_blk_structure(qb_nr_fields) * FQB;

    ts$future_rcvd = ts$future_rcvd+1;
    if (TCB->rf_qcount >= fq_max)
    {
        XLOG$FAO(LOG$TCPERR,"!%T FQ full for seg !XL, QB !XL!/",0,Seg,QB);
        ts$future_dropped = ts$future_dropped+1;
        return TRUE;        // Caller should delete
    };
    QB->nr$seq_start = Seg->sh$seq;
    QB->nr$seq_end = Seg->sh$seq + SEQsize;
    QB->nr$seq_count = SEQsize;
    FQB = TCB->rf_qhead;

// Find where to put this segment on the future queue

    while (TRUE)
        if (FQB == TCB->rf_qhead)
        {
            FQB = TCB->rf_qtail;
            break;
        }
        else if (QB->nr$seq_start < FQB->nr$seq_start)
        {
            FQB = FQB->nr$last;
            break;
        }
        else if (QB->nr$seq_start == FQB->nr$seq_start)
        {
            XLOG$FAO(LOG$TCP,
                     "!%T Drop duplicate FQ seg !XL, QB !XL!/",
                     0,Seg,QB);
            ts$future_dups = ts$future_dups+1;
            return TRUE;// Tell caller to deallocate
        }
        else
            FQB = FQB->nr$next;

// Insert this segment in proper sequence

    XLOG$FAO(LOG$TCP,"!%T Seg !XL, QB !XL inserted on FQ at FQB !XL!/",
             0,Seg,QB,FQB);
    TCB->rf_qcount = TCB->rf_qcount+1;
    INSQUE(QB,FQB);
//~~~XINSQUE(QB,FQB,Append_Future_Q,Q$TCBFQ,TCB->rf_qhead);
    return FALSE;       // Don"t deallocate - it"s on our queue
}

//SBTTL "Check and attempt to process segments on "future" queue"
/*
 */

void Check_Future_Q(struct tcb_structure * TCB)
{
    struct queue_blk_structure(qb_nr_fields) * NQB;

// Handle segments on the future queue that are no longer in the future.

    NQB = TCB->rf_qhead;
    while (NQB != &TCB->rf_qhead)
    {
        signed long
        SEQoffset,
        SEQsize,
        delete;
        struct queue_blk_structure(qb_nr_fields) * QB;

        QB = NQB;
        NQB = QB->nr$next;
        delete = FALSE;
        SEQsize = QB->nr$seq_count;
        SEQoffset = TCB->rcv_nxt-QB->nr$seq_start;

        // If first in-window octet is beyond end of segment, then this segment
        // has become obsolete - drop it.

        if (SEQoffset >= SEQsize)
        {
            delete = TRUE;
//~~~        XREMQUE(QB,QB,Check_Future_Q,Q$TCBFQ,TCB->rf_qhead);
            REMQUE(QB,&QB);
            TCB->rf_qcount = TCB->rf_qcount-1;
            XLOG$FAO(LOG$TCP,"!%T Flushing FQ seg !XL, QB !XL, SEQ=!XL,!XL!/",
                     0,QB->nr$seg,QB,QB->nr$seq_start,QB->nr$seq_end);
            ts$future_dropped = ts$future_dropped+1;
        }
        else


            // If first in-window octet is within the segment, then the segment is
            // now usable - process it in Decode_Segment routine.

            if (SEQoffset >= 0)
            {
//~~~       XREMQUE(QB,QB,Check_Future_Q,Q$TCBFQ,TCB->rf_qhead);
                REMQUE(QB,&QB);
                TCB->rf_qcount = TCB->rf_qcount-1;
                XLOG$FAO(LOG$TCP,"!%T Using FQ seg !XL, QB !XL, SEQ=!XL,!XL!/",
                         0,QB->nr$seg,QB,QB->nr$seq_start,QB->nr$seq_end);
                delete = Decode_Segment(TCB,QB->nr$seg,QB);
                ts$future_used = ts$future_used+1;
            };

        // Other cases should mean the segment is still in the future, so we
        // just leave the segment on the queue.

        if (delete)
        {
            mm$seg_free(QB->nr$buf_size,QB->nr$buf);
            mm$qblk_free(QB);
        };
    };
}

//SBTTL "READ_TCP_Options: process TCP segment options in SYN segment"

void read_tcp_options(TCB,SEG)
struct tcb_structure * TCB;
struct segment_structure * SEG;
{
    struct tcp$opt_block * OPTR;
    signed long
    OPTLEN,
    DATAPTR;

// Start at beginning of options area (start of segment data)

    OPTR = SEG->sh$data;
    DATAPTR = (long)SEG+(SEG->sh$data_offset*4);

// Scan the the entire options area until we hit the start of TCP data

    while (OPTR < DATAPTR)
    {

// Deal with options we can handle
// N.B. We may modify the options area by byteswapping.

        switch (OPTR->tcp$opt_kind)
        {
        case TCP$OPT_KIND_EOL:  // End of options list
            goto exitloop;

        case TCP$OPT_KIND_NOP:  // No-op option
            OPTR = (long)OPTR + 1;  // Advance pointer by one byte
            break;

        case TCP$OPT_KIND_MSS:  // Maximum segment size option
        {
            signed long
            SZ;
            OPTLEN = OPTR->tcp$opt_length;
            if (OPTLEN == TCP$OPT_LENGTH_MSS)
            {
                // Length is correct
                SZ = OPTR->tcp$opt_dword;
                SWAPWORD(&SZ);  // Change to VAX byte order
                TCB->max_seg_data_size = MIN(SZ,max_recv_datasize);
                TCB->snd_ack_threshold = TCB->max_seg_data_size ;
                XLOG$FAO(LOG$TCP,
                         "!%T TCP MSS option = !SL set to !SL, TCB=!XL!/",
                         0,SZ,TCB->max_seg_data_size,TCB);
            }
            else
                XLOG$FAO(LOG$TCP+LOG$TCPERR,
                         "!%T ?TCP MSS option wrong size=!SL, TCB=!XL!/",
                         0,OPTR->tcp$opt_length,TCB);
            OPTR = (long)OPTR + OPTR->tcp$opt_length;
        };
        break;

        default:        // Some unknown option type
        {
            XLOG$FAO(LOG$TCP+LOG$TCPERR,
                     "!%T ?Bad TCP option type !SL, size !SL for TCB !XL!/",
                     0,OPTR->tcp$opt_kind,OPTR->tcp$opt_length,TCB);
            OPTR = (long)OPTR + OPTR->tcp$opt_length;
            goto exitloop;
            // not yet break;       // Can't procede, since length may not be valid
        };
        };
    };
exitloop:
    {}
}

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

Decode_Segment(TCB,seg,QB)
struct tcb_structure * TCB;
struct segment_structure * seg;
struct queue_blk_structure(qb_nr_fields) * QB;
{
    extern  TCB_Promote();
    struct queue_blk_structure(qb_ur_fields) * QBR;
    struct queue_blk_structure(qb_nr_fields) * QBN;
    signed long
    RC,
    AckTst_OK,
    SeqTst_OK,
    ack,
    datasize,
    SEQsize,
    SEQcount,
    SEQstart,
    SEQoffset,
    EOL,
    Ucount,
    Uoffset,
    RetCode,
    Seg_Trimmed= 0;

    /*
        if ($$LOGF(LOG$TCBDUMP))
        {
        LOG$FAO("!/!%T Decode Seg - TCB dump of !XL!/",0,TCB);
        Dump_TCB(TCB));
        };
    */

    RetCode = TRUE;     // Let caller handle memory mgmt.

// Compute amount of data available in this segment.
// Also set the value in the associated Queue block.

    datasize = QB->nr$data_size = QB->nr$size - seg->sh$data_offset*4 ;

// Point at first user request

    QBR = TCB->ur_qhead;

//********************************************************
//  Process unsynchronized Connection States.   !
//********************************************************

    switch (TCB->state)
    {

//********************************************************
//      LISTEN State                !
//********************************************************

    case CS$LISTEN:
    {
//  XLOG$FAO(LOG$TCP,"!%T Received Seg in LISTEN State.!/", 0);
        if (seg->sh$c_ack)  // "ACK" ?
        {
            if (! seg->sh$c_rst)
                Send_Reset(TCB,seg->sh$ack);
            return(RetCode);
        }
        else

            if (seg->sh$c_rst)
                XLOG$FAO(LOG$TCP,
                         "!%T Received RST in listen state, TCB=!XL!/",0,TCB);
            else if (seg->sh$c_syn)
            {

// Set TCB's sequence space counters.
// snd_nxt (send next) & snd_Una(send oldest unackowledged seq #) are
// both sent during TCB initialization (Init-TCB).

//      XLOG$FAO(LOG$TCBSTATE,
//           "!%T Decode-Seg: Received SYN for Passive connection.!/",0);
                TCB->rcv_nxt = seg->sh$seq + 1;   // Next expected rcv sequence #.
                TCB->irs = TCB->snd_wl = seg->sh$seq;// set Initial rcv seq #  & window update.
                TCB->snd_wnd = seg->sh$window;    // Send window size in bytes.
                TCB->snd_max_wnd = MAXU(TCB->snd_max_wnd, TCB->snd_wnd) ;
                tcp$set_tcb_state(TCB,CS$SYN_RECV);  // Connection state change.

// Process segment options, if any.

                if (seg->sh$data_offset > TCP_DATA_OFFSET)
                    read_tcp_options(TCB,seg);

// Fill in any unspecified Foreign Host or Port
// Also, if wild foreign host, start an address to name lookup for it.

                if ((TCB->foreign_host == WILD) ||
                        (TCB->foreign_port == WILD))
                {
                    if (TCB->foreign_host == WILD)
                    {
                        TCB->foreign_host = QB->nr$src_adrs;
                        TCB->nmlook_flag = TRUE;
                        NML$GETNAME(QB->nr$src_adrs,tcp$adlook_done,TCB);
                    };

                    if (TCB->foreign_port == WILD)
                        TCB->foreign_port = (seg->sh$source_port & 0xffff) ;

// Move TCB to head of local port list as now it's fully specified.
                    TCB_Promote ( TCB );
                };

// Fill in unspecified local host

                if ((TCB->local_host == WILD))
                    TCB->local_host = QB->nr$dest_adrs;

// Now that the connection has been fully specified we can "ACK" the "SYN".
// If SYN-ACK send fails (no route - config error), flush the connection.

                if (! send_syn$ack(TCB))
                {
                    XLOG$FAO(LOG$TCPERR,
                             "!%T Decode-seg(LISTEN): Send_SYN$ACK failed, TCB=!XL!/",
                             0,TCB);
                    Send_Reset(TCB,seg->sh$ack);
                    tcp$kill_pending_requests(TCB,NET$_NRT);
                    tcb$delete(TCB);
                    return(ERROR);
                };
//      XLOG$FAO(LOG$TCBSTATE,"!%T Decode-seg: SYN-ACK sent.!/", 0);


// If data present then queue it for later.

                if (datasize > 0)
                {
                    XLOG$FAO(LOG$TCPERR,"!%T Decode Seg(Listen): SYN data dropped!/", 0);
//~~~       RetCode = FALSE;
                };

// make sure the connection stays alive.

                TCB->connection_timeout = ACTIVE_OPEN_TIMEOUT + Time_Stamp();
            }
            else

#if 0
[OTHERWISE]:
#endif
                XLOG$FAO(LOG$TCPERR,
                         "!%T Decode Seg(Listen): NON SYN control!/",0);

        return (RetCode);
    };          // "LISTEN" State.
    break;

//********************************************************
//      "SYN-SENT" State.           !
//********************************************************

    case CS$SYN_SENT:
    {
//  XLOG$FAO(LOG$TCP, "!%T Received Seg in SYN-SENT State.!/", 0);
        ack = -1;
        if (seg->sh$c_ack)
        {
            if ((seg->sh$ack <= TCB->iss) ||
                    (seg->sh$ack > TCB->snd_nxt))
            {
                Send_Reset(TCB,seg->sh$ack); // Unacceptable "ACK".
//      XLOG$FAO(LOG$TCPERR, "!%T (Syn-sent)Unacceptable ACK.!/", 0);
                return(TRUE);
            }
            else        // acceptable "ACK"
            {
                if ((TCB->snd_una <= seg->sh$ack) &&
                        (seg->sh$ack <= TCB->snd_nxt))
                {
//          XLOG$FAO(LOG$TCP, "!%T (Syn-sent)Valid ACK.!/", 0);
                    ack = TRUE;
                    TCB->snd_una = seg->sh$ack;
                    ack_rt_queue(TCB,seg->sh$ack);
                }
                else
                    return(TRUE); // Invalid ACK.
            };
        };

// at this juncture we have a valid ACK.
// Check the "RST" control bit in TCP Header.

        if (seg->sh$c_rst)  // Connection refused - reset
        {
            tcp$kill_pending_requests(TCB,NET$_CREF);
            tcp$inactivate_tcb(TCB,NET$_CR);
            XLOG$FAO(LOG$TCBSTATE,
                     "!%T Decode seg:(SYN-Sent) RESET TCB !XL!/",
                     0,TCB);
            return(ERROR);  // Caller deletes seg, Queue-blk.
        }
        else            // Not a reset seg, check for "SYN" bit.
        {
            if (seg->sh$c_syn)
            {

// set TCB sequence space counters.

                TCB->rcv_nxt = seg->sh$seq + 1;
                TCB->snd_wnd = seg->sh$window;
                TCB->snd_max_wnd = MAXU(TCB->snd_max_wnd, TCB->snd_wnd) ;
                TCB->irs = TCB->snd_wl = seg->sh$seq;

// Was our "SYN" ack'ed?

                if (TCB->snd_una > TCB->iss)
                {
                    TCB->is_synched = TRUE;
                    tcp$set_tcb_state(TCB,CS$ESTABLISHED);

// Tell the user the Connection is OPEN as the IO status has yet to be posted.
// Valid only for connections not openned in immediate return mode.

                    if (! TCB->open_nowait)
                        tcp$post_active_open_io_status(TCB,SS$_NORMAL,0);
                    XLOG$FAO(LOG$TCP,
                             "!%T Our SYN ACKed, Connection established.!/",0);
                }
                else
                    tcp$set_tcb_state(TCB,CS$SYN_RECV);

//      TCP$Send_Ack(TCB);
                tcp$enqueue_ack(TCB);

// If data present then queue for later

                if (datasize > 0)
                {
                    XLOG$FAO(LOG$TCPERR,
                             "!%T Decode Seg: SYN-ACK data dropped, TCB=!XL!/",
                             0,TCB);
//~~~           RetCode = FALSE;
                };

// Process segment options, if any.

                if (seg->sh$data_offset > TCP_DATA_OFFSET)
                    read_tcp_options(TCB,seg);
            };
        };

// Reset connection time out as something useful has happened.

        if (keep_alive)
            TCB->connection_timeout = CONN_TIMEVAL + Time_Stamp();
        else
            TCB->connection_timeout = 0;
        return (RetCode);
    };  // End: "SYN-SENT" State.
    };

//*******************************************************
//  Process "Synchronized" Connection States       !
//*******************************************************

// Advance connection timeout since the other end is sending something

    if (keep_alive)
        TCB->connection_timeout = CONN_TIMEVAL + Time_Stamp();
    else
        TCB->connection_timeout = 0;

// Determine the sequence space occupied by this segment.
// Set field "NR$SEQ_END" in the Queue-blk associated with this segment.

    SeqTst_OK = FALSE;      // assume the worst, seg is unacceptable.

// compute sequence space occupied.
// SEQsize is the count of seqence numbers in the packet
// SEQcount is the count of them that are in the window

    SEQsize = datasize; // Start with packet data count
    SEQcount = 0;       // Assume no usable seq #s in packet yet

    if (seg->sh$c_syn)
        SEQsize = SEQsize + 1;  // SYN occupies seqence space.
    if (seg->sh$c_fin)
        SEQsize = SEQsize + 1;  // So does "FIN".

// Set last sequence # in this segment.

    if (SEQsize <= 0)
        QB->nr$seq_end = seg->sh$seq;
    else
        QB->nr$seq_end = (seg->sh$seq + SEQsize) - 1;

// Assume no user data in the segment yet

    Ucount = 0;
    Uoffset = 0;

// Test segment acceptablity by checking that the segment sequence numbers
// fall inside the receive window (TCB->RCV_Nxt + TCB->rcv_wnd).

    if (TCB->rcv_wnd == 0)  // Zero window cases
    {
        if (SEQsize == 0)   // Empty packet case
        {
            if (seg->sh$seq == TCB->rcv_nxt)
                SeqTst_OK = TRUE;
        }
        else if (SEQsize != 0)      // Nonempty packet case
        {
            if (seg->sh$c_ack || seg->sh$c_rst || seg->sh$c_urg)
            {
                // Data is unacceptable but take [ack,rst, urg]
                SeqTst_OK = TRUE;
                SEQsize = SEQsize - datasize; // remove data from SEQ
                datasize = 0;
            };
        }
    }
    else

// Sequence number test doesn't match the TCP spec since we will take a segment
// that exceeds the window and use only the data the lies in the window.

        if (TCB->rcv_wnd != 0)  // Window is open case
        {
            if (SEQsize == 0)   // Empty packet case
            {
                if ((seg->sh$seq == TCB->rcv_nxt))
                    SeqTst_OK = TRUE;
            }
            else if (SEQsize > 0)   // Nonempty packet case (the useful one)
            {
                {

                    // SEQoffset is the offset of the first in-window octet in the
                    // segment.

                    SEQoffset = TCB->rcv_nxt - seg->sh$seq;

                    // SEQoffset >= 0 means it is at or before the left edge of the
                    // window. SEQoffset <= SEQsize means the first in-window octet
                    // is inside the segment and thus the segment is usable.

                    if ((SEQoffset >= 0) && (SEQoffset < SEQsize))
                    {
                        SEQstart = TCB->rcv_nxt;
                        SEQcount = SEQsize - SEQoffset;

                        // Trim to fit in window

                        if (SEQcount > TCB->rcv_wnd)
                        {
                            Seg_Trimmed = 1;
                            SEQcount = TCB->rcv_wnd;
                            QB->nr$seq_end = SEQstart+SEQcount;
                        };

                        SeqTst_OK = TRUE;
                    }
                    else


                        // SEQoffset < 0 means that the segment is beyond the left edge
                        // of the window. TCB->rcv_wnd > (-.SEQoffset) means that the
                        // start of the segment is before the right edge and the segment
                        // may be held for future use.

//      [(SEQoffset < 0) && (TCB->rcv_wnd > (-.SEQoffset))]:
                        if ((SEQoffset < 0) && (TCB->rcv_wnd < (SEQoffset)))
                        {
                            RetCode = Append_Future_Q(TCB,QB,seg,SEQsize);
//      TCB->Pending_ACK = TRUE; // Force an ACK for this
                            tcp$enqueue_ack(TCB);
                            return RetCode;
                        };
                };
            };
        };

// If segment was not acceptable, drop it.
// Test for duplicate segment, if TRUE, re-ack it as the original "ACK"
// might have been lost.  We get a free window update anyway.

    if (! SeqTst_OK)
    {
        ts$oorw_segs = ts$oorw_segs + 1;

// Test for duplicate

        if ((seg->sh$seq <= TCB->rcv_nxt) &&
                (seg->sh$seq >= TCB->irs))
        {

// Probable duplicate.

            if ((seg->sh$ack >= TCB->iss) &&
                    (seg->sh$ack <= TCB->snd_nxt))
            {

// Duplicate segment, RE-ACK.  If time-wait state then reset the TW timer.

                ts$duplicate_segs = ts$duplicate_segs + 1;
                ts$oorw_segs = ts$oorw_segs - 1; // don't count seg twice.

                if (TCB->state == CS$TIME_WAIT)
                    TCB->time_wait_timer = MAX_SEG_LIFETIME + Time_Stamp();

// Use window from duplicate segment as it is more current than our present one.
// Make sure we take into account transmitted but unacknowledged sequence
// numbers.  Check that the update is new than what we have (snd_wl).

                if (TCB->snd_wl <= seg->sh$seq)
                {
                    TCB->snd_wnd= seg->sh$window -
                                  (TCB->snd_nxt - seg->sh$ack);
                    TCB->snd_max_wnd = MAXU(TCB->snd_max_wnd, TCB->snd_wnd) ;
                    TCB->snd_wl = seg->sh$seq;
                };
                XLOG$FAO(LOG$TCPERR,
                         "!%T Decode Seg: dup seg - Seg-ack (!UL) <=Snd-nxt (!UL)!/",
                         0, seg->sh$ack, TCB->snd_nxt);
//      TCP$Send_Ack(TCB);
                tcp$enqueue_ack(TCB);
            }
            else
                XLOG$FAO(LOG$TCPERR,
                         "!%T Duplicate Segment:  Seg-Seq (!UL) <= Recv-Nxt (!UL)!/",
                         0, seg->sh$seq, TCB->rcv_nxt);
        };

        if ($$LOGF(LOG$TCPERR))
            LOG$FAO("!%T SEGIN: !XL (!UL) Failed seq tests, RCV_WND=!UL, RCV_NXT=!XL (!UL), SND_NXT=!XL (!UL)!/",
                    0,
                    seg->sh$seq,seg->sh$seq,TCB->rcv_wnd,TCB->rcv_nxt,
                    TCB->rcv_nxt,TCB->snd_nxt,TCB->snd_nxt);

        return(TRUE);       // Let caller delete the segment.
    };

// Check if ACK actually acknowledges something valid

    if ((TCB->snd_una < seg->sh$ack) &&
            (seg->sh$ack <= TCB->snd_nxt))
        AckTst_OK = TRUE;
    else
        AckTst_OK = FALSE;

// Segment is acceptable (in the receive window->rcv_wnd).

//********************************************************
//      Check the "ACK" bit.            !
//********************************************************

    if (seg->sh$c_ack)
    {
        switch (TCB->state)
        {
        case CS$SYN_RECV:
        {

// If unacceptable ACK then send a RESET reply & drop the segment.
// Otherwise: If NOT a reset segment then our SYN-ACK is acked & the
// connection becomes established.  If the seg contained a RESET flag then
// fall thru the ACK processing & into RESET processing.

            if (! AckTst_OK)
            {
                Send_Reset(TCB,seg->sh$ack);
                return(TRUE);   // caller deletes segment & Qblk.
            };

            if (! seg->sh$c_rst)
            {
                TCB->snd_una = seg->sh$ack;
                ack_rt_queue(TCB,seg->sh$ack);
                TCB->is_synched = TRUE;
                tcp$set_tcb_state(TCB,CS$ESTABLISHED);
                XLOG$FAO(LOG$TCP,"!%T SYN_RECV Connection established.!/",0);

// Make sure we update the send window here, since some systems have a habit of
// opening connections with zero windows.

                if (TCB->snd_wl <= seg->sh$seq)
                {
                    TCB->snd_wnd=seg->sh$window-(TCB->snd_nxt-TCB->snd_una);
                    TCB->snd_max_wnd = MAXU(TCB->snd_max_wnd, TCB->snd_wnd) ;
                    TCB->snd_wl=seg->sh$seq;
                    XLOG$FAO(LOG$TCP,
                             "!%T Updated SND_WND=!UL, SND_NXT=!XL (!UL), SND_UNA=!XL (!UL)!/",
                             0,TCB->snd_wnd,TCB->snd_nxt,TCB->snd_nxt,
                             TCB->snd_una,TCB->snd_una);
                };

// Finish up pending I/O on this TCB.

                if (TCB->is_tvt)
                {

// If it's a TVT, then we need to try to open it. On failure, the TCB has been
// flushed, so no further processing is possible for this segment.

                    if (! TELNET_OPEN(TCB))
                    {
                        reset_unknown_connection(seg,QB);
                        return ERROR;
                    };
                }
                else if (! TCB->open_nowait)
                    tcp$post_active_open_io_status(TCB,SS$_NORMAL,0);
            };
        };
        break;

        case CS$ESTABLISHED:
        case CS$FIN_WAIT_1:
        case CS$FIN_WAIT_2:
        case CS$CLOSE_WAIT:
        {
            if (AckTst_OK)
            {
                TCB->snd_una = seg->sh$ack;
                ack_rt_queue(TCB,seg->sh$ack);

// If state is CS$FIN_WAIT_1 & the RT-Queue is empty (ie, SND_NXT = Seg_ACK)
// then change state to FIN_WAIT_2.

                if (TCB->state == CS$FIN_WAIT_1)
                    if (TCB->snd_nxt == seg->sh$ack)
                        tcp$set_tcb_state(TCB,CS$FIN_WAIT_2);
            };

// Update send window, taking into account transmitted by unacknowleged sequence
// numbers (SND_UNA).
// Snd-WL is updated to the current segment sequence #.
// Record seg seq # at last window update.

            if (TCB->snd_wl <= seg->sh$seq)
            {
                TCB->snd_wnd = seg->sh$window-(TCB->snd_nxt - TCB->snd_una);
                TCB->snd_max_wnd = MAXU(TCB->snd_max_wnd, TCB->snd_wnd) ;
                TCB->snd_wl = seg->sh$seq;
                XLOG$FAO(LOG$TCP,
                         "!%T Updated SND_WND=!UL, SND_NXT=!XL (!UL), SND_UNA=!XL (!UL)!/",
                         0,TCB->snd_wnd,TCB->snd_nxt,TCB->snd_nxt,
                         TCB->snd_una,TCB->snd_una);
            };
        };
        break;

        case CS$TIME_WAIT:
            TCB->time_wait_timer = Time_Stamp() + MAX_SEG_LIFETIME;
            break;

        case CS$CLOSING:
        {

// IF the ACK acknowledges outstanding FIN, then enter Time_Wait state.

            if (TCB->snd_nxt == seg->sh$ack)
            {
                // "FIN" has been ack'ed.
                XLOG$FAO(LOG$TCBSTATE,
                         "!%T DS(Closing): FIN ACKed for conn !XL!/",
                         TCB);
                tcp$set_tcb_state(TCB,CS$TIME_WAIT);
                if (! TCB->close_nowait)
                {
                    tcp$post_user_close_io_status(TCB,SS$_NORMAL,0);
                    TCB->time_wait_timer = Time_Stamp() + MAX_SEG_LIFETIME;
                }
                else
                {
                    tcp$kill_pending_requests(TCB,NET$_KILL);
                    tcb$delete(TCB);
                    XLOG$FAO(LOG$TCBSTATE,
                             "!%T Connection purged and deleted !XL!/", TCB);
                    return(ERROR);
                };
                return(TRUE);   // Let caller delete segment
            }
            else
                return(TRUE);   // Ignore segment, caller deletes.
        };
        break;
// likewise, in LAST-ACK state, but delete connection

        case CS$LAST_ACK:
        {
            if (TCB->snd_nxt == seg->sh$ack)
            {
                // "FIN" has been acked.
                XLOG$FAO(LOG$TCBSTATE,
                         "!%T DS(Last-ACK): FIN ACKed, deleting conn !XL!/",
                         0,TCB);
                tcp$kill_pending_requests(TCB,NET$_CC);
                tcb$delete(TCB);
                return(ERROR);  // TCB gone - let caller delete the segment
            }
            else
                return(TRUE);   // Ignore segment
        };
        };
    };          // End: Check "ACK" Bit.

//********************************************************
//      Check "RST" Bit.            !
//********************************************************
    if (seg->sh$c_rst)
    {

// RESET in SYN-RECV is special in that passive connections return to the
// LISTEN state, and are not actual RESET.

        if ((TCB->state == CS$SYN_RECV) && (! TCB->active_open))
        {
            tcp$set_tcb_state(TCB,CS$LISTEN);
            SEG$Purge_RT_Queue(TCB);
            return(ERROR);  // caller deletes seg, TCB is dead.
        }
        else

// For all other cases, just reset the connection.

#if 0
[OTHERWISE]:
#endif
        {
            if (TCB->state == CS$TIME_WAIT)
                RC = NET$_CC;
            else
                RC = NET$_CR;
            tcp$kill_pending_requests(TCB,RC);
            XLOG$FAO(LOG$TCBSTATE,
                     "!%T DS(!SL): RESET Connection !XL!/", 0,TCB->state,TCB);
            tcp$inactivate_tcb(TCB,RC); // Let user know connection was reset.
            return(ERROR);  // Caller deletes segment.
        };
    };          // End: Check "RST" Bit.

//********************************************************
//      Check "SYN" Bit.            !
//********************************************************

    if (seg->sh$c_syn)
    {

// Is segment in the receive window?

        if (seg->sh$seq > (TCB->rcv_nxt + TCB->rcv_wnd))
            return(TRUE);   // not in window just drop
        else
        {

// If sequence number is same as IRS, just ignore - it is an old duplicate.
// Any other in-window SYN is bad news, however - we RESET the connection.

            if (seg->sh$seq == TCB->irs)
                return(TRUE);   // Old, duplicate SYN - just drop it
            else
            {
                // In-window SYN - Bad news
                Send_Reset(TCB,seg->sh$ack);
                tcp$kill_pending_requests(TCB,NET$_CR);
                XLOG$FAO(LOG$TCBSTATE,
                         "!%T Dup SYN, deleting connection !XL!/",0,TCB);
                tcb$delete(TCB);
                return(ERROR);  // TCB is gone, caller deletes.
            }
        };
    };          // End: Check "SYN" Bit.

//********************************************************
//      Check "URG" Bit.            !
//********************************************************

    if (seg->sh$c_urg)
    {
        if ((TCB->state == CS$ESTABLISHED) ||
                (TCB->state == CS$FIN_WAIT_1) ||
                (TCB->state == CS$FIN_WAIT_2))
        {
            TCB->rcv_up = MAX(TCB->rcv_up,seg->sh$urgent);
            QB->nr$urg = TRUE;
        };
    };

//********************************************************
//  Process Segment Text (Check for Data & EOL).    !
//********************************************************

    if (SEQcount > 0)
    {
        // Have something in window

// EOL asserted?

        if (seg->sh$c_eol)  // Check EOL (Push) flag.
            QB->nr$eol = TRUE;

        switch (TCB->state)
        {
        case CS$ESTABLISHED:
        case CS$FIN_WAIT_1:
        case CS$FIN_WAIT_2:
        {

// Log updated RCV.NXT

            if ($$LOGF(LOG$TCP))
            {
                signed long
                new;
                new = TCB->rcv_nxt + SEQcount;
                LOG$FAO("!%T Update RCV_NXT !XL (!UL) to !XL (!UL)!/",
                        0,TCB->rcv_nxt,TCB->rcv_nxt,new,new);
            };

// Calculate actual number of octets for the user

            Ucount = SEQcount-(SEQsize-datasize);
            Uoffset = SEQoffset;

// Account for sequence space & queue any user data to the TCB for later
// delivery.

            TCB->rcv_nxt = TCB->rcv_nxt+SEQcount; //account for accepted bytes
            if (Ucount > 0)
            {
                // acceptable data for user
                signed long
                dataptr;
                if (TCB->user_timeout != 0)
                    TCB->user_timeout = Time_Stamp() + TCB->user_timeval;
                dataptr = (long)seg+seg->sh$data_offset*4;
                QB->nr$ucount = Ucount; // number of bytes for user
                QB->nr$uptr = dataptr+Uoffset; // point to first data byte
                QB->nr$seq_start = SEQstart; // first usable seq #
                QB->nr$seq_count = SEQcount; // count of usable seq #s
                RetCode = queue_network_data(TCB,QB); // maintain FIFO
//      if (RetCode == TRUE)    // Don't ACK on error...
//          TCB->pending_ack = TRUE;
//          tcp$send_ack(TCB);

// If this is a TVT, tell TVT processing that there's new data.

                if (TCB->is_tvt)
                {
                    TELNET_INPUT(TCB) ;
                }
                else

// If we now have enough data to fill the first user receive request, then
// hurry things along by delivering to the user now.

                {
                    if ((RetCode == TRUE))  // Don't ACK on error...
//          TCB->pending_ack = TRUE;
                    {
                        tcp$enqueue_ack(TCB);
                    } ;

                    if ((QBR != TCB->ur_qhead) &&
//             ((TCB->rcv_q_count >= QBR->UR$Size) ||
                            ((TCB->rcv_q_count > 0) || (QB->nr$eol)))
                    {
                        tcp$deliver_user_data(TCB);
                    } ;
                } ;
            };
        };
        };
    };

//********************************************************
//      Check "FIN" Bit.            !
//********************************************************

    if (seg->sh$c_fin && ! Seg_Trimmed)
    {
//  TCB->Pending_ACK = TRUE;// Flag we need to ACK.
        tcp$enqueue_ack(TCB) ;

// Set PUSH pointer to end of this segment to force any current data to be
// pushed. Also, attempt to give the user any data that is left.

        if (TCB->rcv_q_count > 0)
        {
            TCB->rcv_push_flag = TRUE;
            TCB->rcv_pptr = seg->sh$seq + SEQsize;
            if (TCB->is_tvt)
                TELNET_INPUT(TCB);  // Attempt to give TVT input
            else
                tcp$deliver_user_data(TCB); // Attempt to give user data.
        };

// If all user data delivered, then flush the user receive queue now.

        if (TCB->rcv_q_count == 0)
        {
            while ((REMQUE(TCB->ur_qhead,&QBR)) != EMPTY_QUEUE) // check
            {
                user$post_io_status(QBR->ur$uargs,
                                    SS$_NORMAL,0,NSB$PUSHBIT,0);
                mm$uarg_free(QBR->ur$uargs); // Free user arg blk.
                mm$qblk_free(QBR);// Free Queue Block.
            };
        };

// Change the Connection State.

        switch (TCB->state)
        {
        case CS$ESTABLISHED:
        {
            tcp$set_tcb_state(TCB,CS$CLOSE_WAIT);
            if ((TCB->is_aborted || TCB->is_tvt))
                tcp$tcb_close(&TCB);
        };
        break;

        case CS$FIN_WAIT_1:
            tcp$set_tcb_state(TCB,CS$CLOSING);
            break;

        case CS$FIN_WAIT_2:
        {

// If no data left or TCB is aborted, then finish closing it now.

            if ((TCB->rcv_q_count == 0) || TCB->is_aborted)
            {
                tcp$set_tcb_state(TCB,CS$TIME_WAIT);
                if (! TCB->close_nowait)
                    tcp$post_user_close_io_status(TCB,SS$_NORMAL,0);
                TCB->time_wait_timer = Time_Stamp() + MAX_SEG_LIFETIME;
            }
            else
                TCB->fin_rcvd = TRUE;
        };
        };
    };

// *Done with segment*
// Return value indicating whether or not someone is retaining the segment.
// FALSE means that the segment shouldn't be deallocated (it is on someone
// else queue). TRUE or ERROR means OK to deallocate segment.

    return RetCode;
}



Check_Cum_Ack ( struct tcb_structure * TCB , signed long Idx , signed long P1 , signed long P2 )
{

    if (TCB->pending_ack)
    {
        if (! TCB->is_tvt)
            tcp$deliver_user_data(TCB); // Try to give the user data.
        XLOG$FAO(LOG$TCP,"!%T SEGIN sending cum ACK, TCB=!XL!/",
                 0,TCB);
//  TCP$Send_Ack(TCB); // send the cumulative ACK segment.
        tcp$enqueue_ack(TCB); // send the cumulative ACK segment.
        return 1;
    }
    else return 0;
}


//SBTTL "Network Segment Arrival Main Processing Loop"
/*

Function:

    Process the network segment queue (SEGIN->si_qhead).  This queue
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


#define    WKS$TELNET 23        // Well-known-port for TELNET

void SEG$Process_Received_Segments(void)
{
    extern  TCB_FIND();
    extern  VTCB_SCAN();
    register
    struct tcb_structure * TCB;
    register
    struct segment_structure * seg;
    struct queue_blk_structure(qb_nr_fields) * QB;
    signed long
    RQV,
    sum,
    count,
    Need_2_ACK = FALSE, // assume: NO cumulative ACK's needed.
    IP_Address,
    delete;

// Process segments until queue is empty

//~~~WHILE XREMQUE(segin->si_qhead,QB,Process_Received_Segments,Q$SEGIN,0)
//~~~     != Empty_Queue DO
    while ((RQV = REMQUE(segin->si_qhead,&QB)) != EMPTY_QUEUE) // check
    {
        ts$sr = ts$sr + 1;  // count segments received from IP.
        seg = QB->nr$seg;   // point at segment proper.
        delete = TRUE;      // assume we will delete this segment.
        switch (QB->nr$icmp)
        {
        case TRUE:          // ICMP message for TCP
        {
            swapbytes(QB->nr$size/2,seg); // Swap header bytes back

// Find out what connection this is for. The "segment" is the first part of
// the TCP segment we sent out to generate the ICMP reply.

            TCB = TCB_Find(seg->sh$source_port,QB->nr$dest_adrs,
                           seg->sh$dest_port);
            if (TCB == 0)
            {
                if ($$LOGF(LOG$TCPERR || LOG$ICMP))
                {
                    DESC$STR_ALLOC(fhstr,20);
//!         ASCII_DEC_BYTES(fhstr,4,QB->nr$dest_adrs,
                    ASCII_DEC_BYTES(fhstr,4,QB->nr$dest_adrs,
                                    &fhstr->dsc$w_length);
                    LOG$FAO("!%T ICMP for unknown TCB,FH=!AS,FP=!XL,LP=!XL!/",
                            0,fhstr,seg->sh$dest_port,seg->sh$source_port);
                };
            }
            else
            {
                XLOG$FAO(LOG$ICMP,
                         "!%T ICMP type !SL for TCB !XL!/",
                         0,QB->nr$icm_type,TCB);
                switch (QB->nr$icm_type)
                {
                case ICM_DUNREACH:  // Destination unreachable - treat as RESET
                {
                    tcp$kill_pending_requests(TCB,NET$_URC);
                    XLOG$FAO(LOG$TCBSTATE || LOG$ICMP,
                             "!%T TCB !XL killed by ICMP Dest Unreachable!/",
                             0,TCB);
                    tcp$inactivate_tcb(TCB,NET$_URC);
                };
                break;

                case ICM_TEXCEED:   // Time exceeded - treat as RESET
                {
                    tcp$kill_pending_requests(TCB,NET$_CTO);
                    XLOG$FAO(LOG$TCBSTATE || LOG$ICMP,
                             "!%T TCB !XL killed by ICMP Time Exceeded!/",
                             0,TCB);
                    tcp$inactivate_tcb(TCB,NET$_CTO);
                };
                break;

                case ICM_SQUENCH:   // Source quench - not yet supported
                {
                    XLOG$FAO(LOG$TCBSTATE || LOG$ICMP,
                             "!%T TCB !XL received ICMP Source Quench!/",
                             0,TCB);
                    TCB->squench = TRUE ;
                    TCB->squench_timer = Time_Stamp() + SQUENCH_INTERVAL ;
                };
                break;

                case ICM_REDIRECT:  // Redirect - not yet supported
                {
                    XLOG$FAO(LOG$TCBSTATE || LOG$ICMP,
                             "!%T TCB !XL received ICMP Redirect!/",
                             0,TCB);
                };
                break;

                case ICM_PPROBLEM:  // Parameter problem - not yet supported
                {
                    XLOG$FAO(LOG$TCBSTATE || LOG$ICMP,
                             "!%T TCB !XL received ICMP Parameter Problem!/",
                             0,TCB);
                };
                break;
                };
            };
        };      // ICMP case
        break;
        case FALSE:     // A real TCP segment
X:
            {

// Good segment. Verify checksum.

                sum = Gen_Checksum(QB->nr$size,seg,QB->nr$src_adrs,
                                   QB->nr$dest_adrs,TCP_Protocol);
                if (sum != 0 /* was: 0xffff */ )
                {
                    // Checksum error - punt it
                    if ($$LOGF(LOG$TCPERR))
                    {
                        LOG$FAO("!%T TCP Checksum error (sum=!XL) for segment:!/",
                                0,sum);
                        SEG$Log_Segment(seg,QB->nr$size,TRUE,TRUE);
                    };
                    goto leave_x;
                };

                swapbytesseghdr(TCP_HEADER_SIZE/2,seg); // Swap header bytes back
                seg->sh$seq = ntohl(seg->sh$seq/*,16*/);
                seg->sh$ack = ntohl(seg->sh$ack/*,16*/);

                if ($$LOGF(LOG$TCP))
                    SEG$Log_Segment(seg,QB->nr$size,TRUE,FALSE);


// Now, find the connection that this segment is destined for

                TCB = TCB_Find (seg->sh$dest_port,QB->nr$src_adrs,
                                seg->sh$source_port);

// If no connection found, then check for special controls we can handle.

                if (TCB == 0)
Y:
                {
                    if (seg->sh$c_rst) // RESET segment?
                    {
                        XLOG$FAO(LOG$TCPERR,
                                 "!%T RST received for unknown TCB, SP=!SL,DP=!SL!",
                                 0,seg->sh$source_port,seg->sh$dest_port);
                        goto leave_x;
                    }

                    if (seg->sh$c_syn) // "SYN" Segment?
                    {
                        signed long
                        TMP;

// Check for SYN on well-known-socket (port). If OK, then fork server and queue
// the SYN for later processing on the SYN-wait-list.

                        TMP = Check_WKS(QB->nr$dest_adrs,seg->sh$dest_port,
                                        QB->nr$src_adrs,seg->sh$source_port,
                                        QB,seg);
                        if ((TMP == ERROR)) goto leave_x;
                        if ((TMP == TRUE))
                        {
                            delete = FALSE;
                            goto leave_x;
                        };

// No WKS defined. If TELNET port, then try to create TVT connection.

                        if ((seg->sh$dest_port == WKS$TELNET) &&
                                (telnet_service != 0))
                        {
                            TCB = TELNET_CREATE(QB->nr$dest_adrs,
                                                seg->sh$dest_port,
                                                QB->nr$src_adrs,
                                                seg->sh$source_port);
                            if (TCB != 0)
                                goto leave_y;
                        };

// Connection not found. Give a RESET back.

                        IP_Address = QB->nr$src_adrs;
                        ACT$FAO("!%D SYN received for unknown port !UW from <!UB.!UB.!UB.!UB>!",0,
                                seg->sh$dest_port,
                                IP_Address&0xff,(IP_Address>>8)&0xff,
                                (IP_Address>>16)&0xff,(IP_Address>>24)&0xff);
                        reset_unknown_connection(seg,QB);
                        goto leave_x;
                    }
                    else
                    {

// Connection not found and not SYN or RST - return an RST.

#if 0
[OTHERWISE]:
#endif
                        {
                            reset_unknown_connection(seg,QB);
                            goto leave_x;
                        };
                    }
                };
leave_y:

// Here when we have a TCP connection. Process the segment.
// ** Warning** TCB/connection maybe deleted during segment processing.
// Condition is flaged by Decode-segment return code of Error(-1).

                delete = Decode_Segment(TCB,seg,QB);
                if (delete != ERROR)
                {

// Do we need to check for cumulative ACK transmission after the Segment
// input queue is exhausted?

                    if (TCB->pending_ack)
                        Need_2_ACK = TRUE;

// See if we can remove some stuff from the future queue

                    if (queue_not_empty(TCB->rf_qhead))
                        Check_Future_Q(TCB);
                };
            };      // TCP segment case (block X)
leave_x:
            {
            }
        }

// Clean up, possibly delete segment & queue-block.
// *** Warning *** "Delete" can be one of 3 values(-1,0,1).  Following test
// checks low-bit only( case covers BOTH 1 & -1).  Be aware!
// check if we can really delete the segment or does somebody else lay claim.

        if ((delete != 0))
        {
            mm$seg_free(QB->nr$buf_size,QB->nr$buf);
            mm$qblk_free(QB);
        };
    };          // "While"

// Check if any connections need to have a cumulative ACK transmitted.
// If True then check all valid connections & if the TCB->Pending_ACK
// bit is set then Attempt to deliver user data & send an ACK (cumulative).
// Otherwise continue checking connections.

    if (Need_2_ACK)
        VTCB_Scan ( Check_Cum_Ack , 0 , 0 );

}
