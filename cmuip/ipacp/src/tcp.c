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
//TITLE "TCP - major routines for servicing TCP connections"
/*
Facility:

	TCP - Main routines for implementing Internet TCP (RFC 792).

Abstract:
!!!HACK!!// is this accurate?
	Handles tasks which must be "scheduled" in some way, as opposed to
	event driven tasks such as user calls (handled in USER, TCP_USER)
	or network input (handled in TCP_SEGIN).

Language:

	BLISS-32

System:

	VAX/VMS

Author:

	Original version by Stan C. Smith, Summer 1981
	This version by	Vince Fuller, CMU-CSD, Spring/Summer, 1986
	Copyright (c) 1986,1987, Vince Fuller and Carnegie-Mellon University

Module Modification History:

*** Begin USBR change log ***

8.2	05-Dec-1991	Henry W. Miller		USBR
	Set Window_Default to 8192.
	Set ACK_Threshold to 1024.
	Rework arithmetic concerning SND_PPTR.  (Unsigned vrs. signed).

8.1	14-Nov-1991	Henry W. Miller		USBR
	Change arithmetic, printout on timers from signed to unsigned.
	Added routine TCP$Enqueue_ACK() to queue up an ACK for future
	delivery.  (Currently only sets the PENDING_ACK flag, which now
	appears to work due to the correct arithmmetic comparisons).
	In TCP$Send_Data(), check to see if we received an ICMP Source
	Quench packet, and if so, delay delivery

8.0h	17-Sep-1991	Henry W. Miller		USBR
	In TCP$Send_Data(), rework Nagle algorithim as per RFC1122.
	Also add hook for "de-Nagle'ing" the connection.

8.0g	26-Jul-1991	Henry W. Miller		USBR
	In CHECK_TCB(), do sanity check to see if TCB is valid, i.e.,
	not equal to 0.

8.0f	17-Jul-1991	Henry W. Miller		USBR
	In TCP$TCB_SET_STATE(), if changing from CS$NAMELOOK and lookup's
	pending, cancel pending requests.

8.0e	06-Jun-1991	Henry W. Miller		USBR
	Set default ACK_THRESHOLD to 4096.
	Set default WINDOW_DEFAULT to 32768.

8.0d	25-Jan-1991	Henry W. Miller		USBR
	Make WINDOW_DEFAULT and ACK_THRESHOLD configurable variables.

8.0c	15-Jan-1991	Henry W. Miller		USBR
	Changed SND_WND, RCV_WND, Old_RCV_WND, SND_NXT and RCV_NXT to
	unsigned long values as recommended by RFC1122.

8.0b	13-Jan-1991	Henry W. Miller		USBR
	Make TCPTTL a configurable variable.

8.0a	11-Jan-1991	Henry W. Miller		USBR
	Cleanup printout in TCP$Service_Connections().

*** Begin CMU change log ***

8.0	06-Feb-1990	Bruce R. Miller		CMU NetDev
	This module used to be TCP_Main, the main module
	for the IPACP.  I chopped out all of the ACP init
	stuff and changed the name to TCP.Bli.  The ACP
	Init stuff is now in MAIN.BLI.

	20-Oct-1989	Bruce R. Miller		CMU NetDev
	Added a bit of code to initialize the activity log.

	30-Aug-1989	Bruce R. Miller		CMU NetDev
	Made Conect Table into a hash table structure.

	07-Aug-1989	Bruce R. Miller		CMU NetDev
	Added NML$Cancel call to Inactivate_TCB routine.  When a TCB
	is inactivated any outstanding NQE's should be deleted.  If this
	were not done, and a new TCB with the old address were created,
	the the Name Query reply from the first TCB would affect the
	new TCB (which would be totally BOGUS!).

	03-Jul-1989	Bruce R. Miller		CMU NetDev
	Fixed DUMP_TCB to handle unknown TCP state.

	16-MAR-1989	Dale Moore	CMU-CS/RI
	If connection timeout is zero, then don't timeout connection.

	20-OCT-1988	Dale Moore	CMU-CS/RI
	Changed Keep_Alives (Do_Probe) to be runtime configurable.

	04-OCT-1988	Dale Moore	CMU-CS/RI
	Changed TOS from %O"15" to 0.  Evidently some busted
	implementations that couldn't take something in this
	field that wasn't what they expected.

7.10  22-JUN-1988, Dale Moore	CMU-CS/RI
	Change the call on retransmit of SYN to send size of options
	as long	rather than octets.  Thanks to Charles Lane of
	Drexel University.

7.9  10-FEB-1988, Dale Moore
	Modify the Maximum segment size to be dependent upon whether
	the remote address is local or not.

7.8  18-Nov-87, Edit by VAF
	Know about IP$SEND returning failure when no route available. Make
	SEND_CTL propagate this information back to its caller. This allows
	the TCP user interface to detect this condition immediately when
	sending a SYN and abort the connection appropriately.

7.7   4-Aug-87, Edit by VAF
	Remove CQ_DEQUEUE and CQ_ENQUEUE - they are now in MACLIB.
	Use $$KCALL macro to do $CMKRNL stuff.
	Disable Resource Wait Mode at startup so the ACP crashes instead of
	waiting for resources.
	Rewrite send options handling so it can be easily made more general.

7.6  28-Jul-87, Edit by VAF
	Know about TVT's when trying to move data in main processing loop.

7.5  22-Jul-87, Edit by VAF
	Know about internal TCB's in timeout processing.

7.4  20-Jul-87, Edit by VAF
	Clean up handling of circular queues a bit.

7.3  23-Mar-87, Edit by VAF
	Make allocated buffer sizes and maximum segment size dynamic.
	Save and reset process name when ACP starts/stops.
	Call new IP initialization routine.

7.2   3-Mar-87, Edit by VAF
	Flush I/O request tags from all requests. New calling sequence for
	User$Post_IO_Status.

7.1  24-Feb-87, Edit by VAF
	"Net message queue" has been flushed - remove references. Replace with
	call to CHECK_ERRMSG_Q at end of main TCP processing loop.

7.0  19-Feb-87, Edit by VAF
	Use new NMLOOK module entry points for name lookups.

6.9  18-Feb-87, Edit by VAF
	Use OPR$FAO and FATAL$FAO instead of old error handling stuff.

6.8  17-Feb-87, Edit by VAF
	Fix spelling of "Exception_Handler".

6.7  12-Feb-87, Edit by VAF
	Add support for domain service.

6.6   9-Feb-87, Edit by VAF
	Flush external reference to Error_Processor.

6.5   6-Feb-87, Edit by VAF
	Print modification date of ACP during startup.

6.4  10-Dec-86, Edit by VAF
	Change order of arguments to Gen_Checksum.

6.3   9-Dec-86, Edit by VAF
	Don't log "Sending data for TCB..." under LOG$TCP.

6.2  12-Nov-86, Edit by VAF
	Flush SYN_WAIT_LIST and WKS global structures. Add external call to
	TIMEOUT_SYN_WAIT_LIST in main processing loop.

6.1  11-Nov-86, Edit by VAF
	Add FQ_MAX cell set by config routines & used by SEGIN.

6.0   3-Nov-86, Edit by VAF
	Set Base_RT_Timeout (initial RX time value) to 5 seconds. The idea of
	setting it to 1 second was a kluge to get around ARP eating the first
	packet. Make a minor change in the calculation of tcb->rx_timer.

5.9  24-Oct-86, Edit by VAF
	Additions to debugging code.
	When enqueuing user data on send queue, know about retransmission
	count in determining amount of space left on queue.

5.8  30-Sep-86, Edit by VAF
	In INACTIVATE_TCB - don't inactivate an already inactive connection -
	log an error if this occurs. This is to detect cases (like the RX
	timeout bug) where TCB's are being inactivated repeatedly.
	Rewrite Check_Rexmit_Queue to be able to build a segment with both
	data and control information.

5.7  29-Aug-86, Edit by VAF
	Call ARP_INIT in main initialization routine.

5.6  14-Aug-86, Edit by VAF
	Make sure name lookup cancel is done NOINT.

5.5  13-Aug-86, Edit by VAF
	In Build_Header turn on ACK if IS_SYNCHED is set, not by state
	GEQ CS$ESTABLISHED.

5.4  12-Aug-86, Edit by VAF
	Add CS$NAMELOOK - Name lookup wait state. Make CLOSE_TCB know about
	this state (no others should ever see it). Make CLOSE_TCB know about
	NMLook_Flag and do a GREEN_CANCEL on the TCB if it is set.
	Use GREEN_GETNAME to get Local_Name value.

5.3   9-Aug-86, Edit by VAF
	Get and store local host name in LOCAL_NAME descriptor.

5.2   6-Aug-86, Edit by VAF
	Modify retransmission code to use send circular queue.

5.1   6-Aug-86, Edit by VAF
	Modify SEND_DATA to use circular queue of buffered send data.

5.0   5-Aug-86, Edit by VAF
	Add routines for enq/deq into circular queues.
	N.B. Do not use these at AST level, since they don't do locking.

4.9  29-Jul-86, Edit by VAF
	Minor rearrangements for UDP's sake...
	Move TCP_Protocol definition out of here, change names of TTL,TOS,DF.

4.8  25-Jul-86, Edit by VAF
	Add ts$qfull_drops counter.

4.7  18-Jul-86, Edit by VAF
	Add ts$abort_drops counter.

4.6  17-Jul-86, Edit by VAF
	Pass reference to TCB pointer in CLOSE_TCB so it can clear the
	pointer when it deletes the connection.

4.5  15-Jul-86, Edit by VAF
	Add status cell ts$badseq.
	Rearrange change log.

4.4   9-Jul-86, Edit by VAF
	Add status cell ts$future_dups.

4.3   1-Jul-86, Edit by VAF
	Add some more status cells.

4.2  25-Jun-86, Edit by VAF
	Don't ever use index 0 into VALID_TCB table.

4.1  16-Jun-86, Edit by VAF
	Don't go into FIN-WAIT-1 when setting Pending_close - wait until
	the FIN is really sent.

4.0  12-Jun-86, Edit by VAF
	Simplify buffering of user send requests.

3.9  10-Jun-86, Edit by VAF
	Implement buffering of user send requests.

3.8  10-Jun-86, Edit by VAF
	Add new counters for user send queue in preparation for implementation
	of send buffering.

3.7  22-May-86, Edit by VAF
	Use VMS error message facility.

3.6   9-May-86, Edit by VAF
	Make SEND_DATA return count sent. When sending an ACK, we need to make
	sure that SEND_DATA actually sent something.

3.5   8-May-86, Edit by VAF
	Add CLOSE_TIMEOUT. Implement blocking close (USER,SEGIN)
	Add LAST_ACK state.
	Know about U$CLOSE and M$CANCEL function timeouts.

3.4   5-May-86, Edit by VAF
	Implement the "Nagle algorithm" (RFC896) in SEND_DATA.

3.3   1-May-86, Edit by VAF
	Allow delivery of user data when in FIN_WAIT_1 or FIN_WAIT_2.
	Have Service_Connections and Check_ReXmit_Queue return next service
	time values to simplify Wait_For_Something_2_Do.
	Add code to compress retransmission queue when doing retransmisson.

3.2  30-Apr-86, Edit by VAF
	Debugging code for indicating packet origination.
	Turn off pending_ack flag in Build_Header when sending ACKs.

3.1  29-Apr-86, Edit by VAF
	Add some debugging code for logging TCB servicing.
	Remove device restart stuff from here.
	Make retransmission timeout shorter and distinct from connection
	timeout.

3.0  22-Apr-86, Edit by VAF
	Phase II of flushing XPORT - use $FAO for output formatting.
	Remove much of the VMS-specific code to IOUTIL module.

2.9  21-Apr-86, Edit by VAF
	Open timeout is now split into Active (short) and Passive (*long*)

2.8  18-Apr-86, Edit by VAF
	Checksum routine shouldn't do byteswap on IP addresses...

2.7  17-Apr-86, Edit by VAF
	Log segment when doing retransmission.
	Increase probe timer to 1 minute, connection timeout to 10 minutes.

2.6  11-Apr-86, Edit by VAF
	Specifiy and do not exceed a minimum retransmission time of 1 sec.
	Set initial retransmission time to 1 sec to speed things up when
	ARP drops initial connection packets.

2.5   8-Apr-86, Edit by VAF
	Make INACTIVATE_TCB and SET_TCB_STATE into routines. The Macros were
	a little obscure to read and didn't save much (these events do not
	happen often enough for the routine-calling overhead to matter).

2.4   7-Apr-86, Edit by VAF
	First shot at more general logging facility.

2.3  24-Mar-86, Edit by VAF
	Start adding ICMP support.
	Many small changes between previous entry and this one.

2.2  17-Mar-86, Edit by VAF
	Redo connection timeouts, add connection probing code.
	Move a lot of code here from USER.BLI.

2.1   7-Mar-86, Edit by VAF
	Phase I of flushing XPORT: replace all $XPO... calls with real RMS
	calls. Phase II will flush all of the XPORT string handling...

2.0  21-Feb-86, Edit by VAF
	Flush "known_hosts" table, add hooks for new hostname module.
	Rename most logicals from TCP$xxx to INET$xxx.
	Flush "myinternetnames" crud, my_internet_adrs, etc.

*** End CMU change log ***

1.0	[10-15-81] stan
	original version

1.2	[5-5-83] stan
	vms operator interface routines

1.3	[5-20-83] stan
	error processor routine.

1.4	[6-12-83] stan
	Support for SMTP (Simple Mail Transfer Protocol) as a weel-known server.

1.5	[6-13-83] stan
	During connection timeout processing: check if process owning connection
	has sub-processes.  If true, then update connection timeout as this
	could be an ftp cmd connection attempting to timeout during a LONG, SLOW
	data transfer.

1.6	[7-15-83] stan
	modified global literal definitions to be byte-size.

1.7	[9-13-83] stan
	rewrote routine "read_known_hosts_file" to clean up code and utilize the
	host-alias table.  Alias no longer utilize an entry in the known-hosts
	table.

1.8	[5-1-85] noelan olson
	Replace obsolete system services.

1.9	[5-30-85] noelan olson
	Use a table to store multiple internet addresses for the purpose of
	gatewaying between networks.
*/


//sbttl "Module Environment & Definition."

#if 0
MODULE TCP(IDENT="8.2",LANGUAGE(BLISS32),
	   ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			   NONEXTERNAL=LONG_RELATIVE),
	   LIST(REQUIRE,ASSEMBLY,OBJECT,BINARY),
	   OPTIMIZE,OPTLEVEL=3,ZIP)=
#endif

// Include System libraries.

#include <starlet.h>	// VMS system definitions
// not yet #include <cmuip/central/include/netxport.h>	// Bliss transportable library defs.
#include <cmuip/central/include/neterror.h>	// Network error codes
#include <cmuip/central/include/netcommon.h>// Common decls
#include "netvms.h"	// Include Local OS Dependent stuff.
#include <cmuip/central/include/nettcpip.h>	// TCP/IP protocols

#include "tcp.h"			// TCP related defintions.
#include "tcpmacros.h"		// Local (tcp) Macro defintions.
#include "structure.h"		// Structure Definitions
#include "snmp.h"			// Simple Network Management Protocol
// not yet #include <tcpmod.h>		// Modification history/version

#include <descrip.h>
#include <ssdef.h>
// not yet? #include <iosbdef.h>



//SBTTL "External Routines"

// MACLIB.MAR

extern int   Time_Stamp(void);
extern  Calc_CheckSum();
extern void     MovByt();
extern  void    SWAPBYTES();
extern  void    CQ_ENQUEUE();
extern  void    CQ_DEQUEUE();
extern  void    CQ_DEQCOPY();

// IOUTIL.BLI

extern  void    FATAL_FAO();
extern  void    OPR_FAO();
extern  void    LOG_FAO();
extern  void    LOG_OUTPUT();
extern  void    ASCII_Dec_Bytes();
extern  void    ASCII_Hex_Bytes();

// USER.BLI

extern void     USER$Post_IO_Status();
extern     USER$Err();
extern void     TCP$KILL_PENDING_REQUESTS();

// TCP_TELNET.BLI

extern  void    TELNET_INPUT();
extern  void    TELNET_OUTPUT();

// TCP_USER.BLI

extern void    TCP$Deliver_User_Data();

// TCP_MECH.BLI

extern void    TCB$Delete();

// MEMGR.BLI

extern     mm$seg_get();
extern void    mm$seg_free();
extern     mm$qblk_get();
extern void    mm$qblk_free();
extern void    mm$uarg_free();
extern     mm$get_mem();

// TCP_SEGIN.BLI

extern  void    SEG$Log_Segment();
extern  void    SEG$Timeout_Syn_Wait_List();

// IP.BLI

extern     IP$Send();
extern     Gen_Checksum();

// NMLOOK.BLI

extern    NML$CANCEL();



//SBTTL "External Literals & data segments"

#define M$INTERNAL 15 // see user.c
#define U$OPEN 1
#define U$CLOSE 2
#define M$CANCEL 14

#if 0
extern signed long LITERAL

// TCP function codes.

    M$INTERNAL:	UNSIGNED(8),
    U$OPEN:	UNSIGNED(8),
    U$CLOSE:	UNSIGNED(8),
    M$Cancel:	UNSIGNED(8);
#endif

// External Data

extern signed long
    default_mss,		// Default MSS
    max_recv_datasize,		// Max segment size
    max_physical_bufsize,	// Max size of device receive buffer
    min_physical_bufsize,	// Minimum size of device send buffer
    log_state,
    act_state;
extern    struct TCP_MIB_struct * TCP_MIB;   // TCP management Information Block


//SBTTL "Global Data Declarations."

unsigned long
CONN_TIMEVAL =CONN_TIMEOUT;
signed long
    TCPIPID,			// current IP ID for TCP

// TCP System Statistics

    TS$UIR = 0,	// user io requests
    TS$ACO = 0,	// active connections opened.
    TS$PCO = 0,	// passive connections opened.
    TS$DBX = 0,	// user data bytes xmitted.
    TS$DBR = 0,	// user data bytes received.
    TS$SX = 0,	// segments sent to IP.
    TS$SR = 0,	// segments received from IP.
    TS$BADSEQ = 0,	// segments dropped by Queue_Network_Data
    TS$ABORT_DROPS  = 0,// Segments dropped because TCB aborted
    TS$QFULL_DROPS  = 0,// Segments dropped because TCB NR queue full
    TS$SEG_BAD_CKSUM = 0, // segs dropped on bad checksum
    TS$DUPLICATE_SEGS = 0, // duplicate segments.
    TS$RETRANS_SEGS = 0,// Retransmitted segments
    TS$RPZ_RXQ = 0,	// Number of times RX queue repacketized
    TS$OORW_SEGS = 0,	// Out Of Recv Window segments
    TS$FUTURE_RCVD = 0,	// Segments in window but beyone RCV.NXT
    TS$FUTURE_DUPS = 0,	// Future segments that were duplicates
    TS$FUTURE_USED = 0,	// Number of future segments used
    TS$FUTURE_DROPPED = 0, // Number of future segments dropped
    TS$SERVERS_FORKED = 0, // well-known servers forked.

    ACK_THRESHOLD  = 1024,  // accumulate resources before window update.
    WINDOW_DEFAULT  = 8192, // receive window.
    TCPTTL	 = 32,	// Default TCP TTL
    TELNET_SERVICE  = 0, // Nonzero if internal TELNET server enabled
    Keep_Alive	 = 0,	// NonZero if Keep alives wanted
    FQ_MAX  = 0;	// Max segments on future queue, per TCB

// I/O Status Block

//    IO_Status: IO_Status_Blk,



// Common macro used by all to send a TCP segment to IP.


#define    SEND_SEG(TCB,SEGADDR,SEGSIZE,DELFLAG,BUFPTR,BUFSIZE,XTIME) \
	TCPIPID = TCPIPID+1; \
	IP$SEND(tcb->local_host,tcb->foreign_host,TCPTOS,TCPTTL, \
		SEGADDR,SEGSIZE,TCPIPID,TCPDF,DELFLAG,TCP_PROTOCOL, \
		BUFPTR,BUFSIZE)
#define    SEND_SEG1(TCB,SEGADDR,SEGSIZE,DELFLAG,BUFPTR,BUFSIZE,XTIME) \
	{ \
	if (XTIME == 0) \
	    tcb->probe_time = PROBE_IVAL + Time_stamp(); \
	else \
	    tcb->probe_time = PROBE_IVAL + XTIME; \
	SEND_SEG(TCB,SEGADDR,SEGSIZE,DELFLAG,BUFPTR,BUFSIZE) \
	}
#define    SEND_SEG0(TCB,SEGADDR,SEGSIZE,DELFLAG,BUFPTR,BUFSIZE,XTIME) \
	{ \
	SEND_SEG(TCB,SEGADDR,SEGSIZE,DELFLAG,BUFPTR,BUFSIZE) \
	}


void TCP$SET_TCB_STATE(struct tcb_structure * tcb, long S)
// Set the State of a TCB, Old state ==> last_state.
    {
    signed long
	LS;

// If changing from Lookup state, kill pending requests
//    NOINT;
    if (((tcb->nmlook_flag) && (tcb->state == CS$NAMELOOK)))
	{
	tcb->nmlook_flag = FALSE;
	NML$CANCEL(tcb, 0, 0);
	};
//    OKINT;

    if ($$LOGF(LOG$TCBSTATE))
	LOG$FAO("!%T TCB !XL state change from !SL to !SL!/",
		0,tcb,tcb->state,S);
    LS = tcb->last_state = tcb->state;
    tcb->state = S;

    // SNMP accounting
    if ((S == CS$SYN_SENT) && ((LS == CS$CLOSED) || (LS == CS$NAMELOOK)))
	TCP_MIB->MIB$tcpActiveOpens = TCP_MIB->MIB$tcpActiveOpens + 1;

    if ((LS == CS$LISTEN) && (S == CS$SYN_RECV))
	TCP_MIB->MIB$tcpPassiveOpens = TCP_MIB->MIB$tcpPassiveOpens + 1;

    if ((S == CS$ESTABLISHED) || (S == CS$CLOSE_WAIT))
	TCP_MIB->MIB$tcpCurrEstab = TCP_MIB->MIB$tcpCurrEstab + 1;

    if ((LS == CS$ESTABLISHED) || (LS == CS$CLOSE_WAIT))
	TCP_MIB->MIB$tcpCurrEstab = TCP_MIB->MIB$tcpCurrEstab - 1;

    if ((S == CS$CLOSED))
	{
	if ((LS == CS$SYN_SENT) || (LS == CS$SYN_RECV))
	   TCP_MIB->MIB$tcpAttemptFails = TCP_MIB->MIB$tcpAttemptFails + 1;
	if ((LS == CS$ESTABLISHED) || (LS == CS$CLOSE_WAIT))
	   TCP_MIB->MIB$tcpEstabResets = TCP_MIB->MIB$tcpEstabResets + 1;
	};
    };

void TCP$INACTIVATE_TCB(struct tcb_structure * tcb,long err)

// Mark a TCB as inactive & set the user error code in the TCB.  When user
// accesses this connection the reason for the inactivation is returned
// instead of connection-does-not-exist error.

    {
    if ((tcb->state != CS$INACTIVE))
	{
        signed long
            MNLQs_Outstanding;

        if ((MNLQs_Outstanding = NML$Cancel(tcb,0,0)) > 0)
            {
            XLOG$FAO(LOG$TCPERR,"!%T TCB !XL inactivated with !ZB// NQE outstanding!/",
                0,tcb,MNLQs_Outstanding);
            XLOG$FAO(LOG$TCP,"!%T TCB !XL inact. with !ZB NQEs outstanding!/",
                0,tcb,MNLQs_Outstanding);
            };

	TCP$SET_TCB_STATE(tcb,CS$INACTIVE);
	XLOG$FAO(LOG$TCBSTATE,
		 "!%T TCB !XL inactivated, reason=!SL!/",0,tcb,err);
	tcb->inactive_code = err;
	tcb->inactive_timeout = Time_Stamp() + MAX_SEG_LIFETIME/2;
	}
    else
	XLOG$FAO(LOG$TCPERR,"!%T TCB !XL inactive in TCP$Inactivate_TCB!/",0,tcb);
    };

//SBTTL "TCP$CLOSE - Close a network connection"
/*

    Initiate close of a TCP connection.
    Returns:
	    0 on success, close in progress, TCB state advanced.
	non-0 on failure, error code to return to user.
*/

    TCP$Send_CTL();

TCP$TCB_CLOSE(struct tcb_structure * * tcbpt)  // check
    {
      struct tcb_structure * tcb;
    tcb = *tcbpt;

// Cancel any name lookup that might be in progess

    NOINT;
    if (tcb->nmlook_flag)
	{
	tcb->nmlook_flag = FALSE;
	NML$CANCEL(tcb, 0, 0);
	};
    OKINT;

    switch (tcb->state)
      {
    case CS$NAMELOOK: case CS$LISTEN: case CS$SYN_SENT:
	{
	TCP$KILL_PENDING_REQUESTS(tcb,NET$_CC); // Error: Connection Closing.
	TCB$Delete(tcb);
	*tcbpt = 0;
	};
    break;

    case CS$SYN_RECV:
	Send_Fin(tcb);
	break;

    case CS$ESTABLISHED:
	{

// If all send data has been sent then send a "FIN" OTHERWISE flag we need
// to send a "FIN".  TCP$Send_Data rtn will see the flag after all data has been
// sent.

	if (tcb->snd_q_count > 0)
	  tcb->pending_close = TRUE;
	else
	    {
	    Send_FIN(tcb);
	    TCP$SET_TCB_STATE(tcb,CS$FIN_WAIT_1);
	    };
	};
	break;

    case CS$FIN_WAIT_1: case CS$CLOSING: case CS$FIN_WAIT_2: case CS$LAST_ACK:
      //~~~	RETURN 0;		// Close in progress
      return NET$_CC;		//~~~ Already closed
      break;

    case CS$TIME_WAIT:
	return NET$_CC;		// Already closed
	break;

    case CS$CLOSE_WAIT:
	{

// If user data remains then flag a FIN needs to be sent after ALL the
// user data has been sent.

	if (tcb->snd_q_count > 0)
	    {
	    tcb->pending_close = TRUE;
	    TCP$SET_TCB_STATE(tcb,CS$CLOSING);
	    }
	else
	    {
	    Send_FIN(tcb);
 	    TCP$SET_TCB_STATE(tcb,CS$LAST_ACK);
	    };
	}
	break;
    default:
     return 0;
     };
    }


//SBTTL "Send_TCP_Options - Set TCP options when sending SYN segment"

#define    SEND_OPT_SIZE 4		// Total number of bytes of options we set
#define    SEND_OPT_OFFSET (SEND_OPT_SIZE/4) // TCP header offset for it

void Send_TCP_Options(tcb,Seg)
     struct tcb_structure * tcb;
     struct segment_structure * Seg;
{
  extern IP_IsLocal();
  struct tcp$opt_block * OPTR;

// Point at the start of the TCP data

    OPTR = &Seg->sh$data;

// Insert the maximum segment size option

    OPTR->tcp$opt_kind = TCP$OPT_KIND_MSS;
    OPTR->tcp$opt_length = TCP$OPT_LENGTH_MSS;
//    OPTR->TCP$OPT_DWORD = max_recv_datasize;
    if (IP_IsLocal (tcb->foreign_host) == -1)
      OPTR->tcp$opt_dword = default_mss;
    else
      OPTR->tcp$opt_dword = max_recv_datasize;

    SWAPWORD(OPTR->tcp$opt_dword);

// Insert other options here. Be sure to update SEND_OPTION_SIZE and to make
// sure that the maximum TCP header size is not exceeded.

    };

//SBTTL "Compute_RTT: Compute Round Trip Time"
/*

Function:

	Check if "ACK" segment acks any segments currently on the retransmission
	queue. If so then delete the segment from the queue.

Inputs:

	TCB = TCB pointer.

Implicit Inputs:

	tcb->round_trip_time & tcb->calculated_rto.

Outputs:

	Time for next retransmission

Side Effects:

	None.

*/

signed long	 TCP$Compute_RTT(struct tcb_structure * tcb)
    {

#define 	ALPHA   90			// Smoothing constant, per RFC973 pg 41
#define 	BETA   150			// Delay variance factor
#define 	ALPHACOMP   (100 - ALPHA)	// "Compliment" of ALPHA (i.e. 1 - ALPHA)
#define 	MINRTT   5			// Minimum reasonable RTT
#define 	MINSRTT   10			// Minimum smoothed RTT

    unsigned long
	newrto	,
	newrtt	,
	crtt	,
	oldrtt	 ;

// Compute round trip time for adaptive retransmission.

    oldrtt = tcb->round_trip_time;
    crtt = MAXU((Time_Stamp() - tcb->xmit_start_time), MINRTT);
//    crtt = Time_Stamp() - tcb->xmit_start_time ;

// Compute smoothed round trip time, see RFC793 (TCP) page 41 for details.

    if ((tcb->rx_count == 0))
	{
	newrtt = (((oldrtt * ALPHA) + (crtt * ALPHACOMP)) / 100) ;
	}
    else
	{
	newrtt = oldrtt ;
	} ;

    newrto = (BETA * newrtt) / 100;
    tcb->round_trip_time = newrtt;
    tcb->calculated_rto = MINU(MAX_RT_TIMEOUT,MAXU(newrto,MIN_RT_TIMEOUT));

    XLOG$FAO(LOG$TCP,
	    "!%T Compute_RTT: TCB=!XL, Prev SRTT=!UL, Cur RTT=!UL, New SRTT=!UL, RTO=!UL!/",
 		 0, tcb, oldrtt, crtt,
		tcb->round_trip_time, tcb->calculated_rto);

    return (newrto) ;

    }

//SBTTL "Check Retransmission Queue"
/*

Function:

	Scan this TCB's retransmission list.  If any segments have timed out
	then retransmit the segment. If a segment has been on the queue too
	long, then timeout the connection.
	N.B. Currently only considers the first entry on the RX queue.

Inputs:

	TCB = TCB pointer.

Outputs:

	Time of next pending retransmission (returns very large value if no
	segments on retransmission queue)

Side Effects:

	Segments on the retransmission queue maybe retransmitted.
	TCB may be inactivated if RX timeout has occurred.
*/

 void    Build_Header();

TCP$Check_Rexmit_Queue(struct tcb_structure * tcb)
    {
    unsigned long
	now	,
	nxtime	,
      delta;
signed long	tmp;

    now = Time_Stamp();		// Get the time.

// See if anything is on the queue

    if ((tcb->srx_q_count > 0) || (tcb->rx_ctl != 0))
	{

// See if this connection has timed-out.

	if (tcb->rx_timeout < now)
	    {			// Max RX exceeded - conn. timeout
	    XLOG$FAO(LOG$TCBSTATE,"!%T TCB !XL killed by RX timeout!/",0,tcb);
	    TCP$KILL_PENDING_REQUESTS(tcb,NET$_CTO);
	    TCP$Inactivate_TCB(tcb,NET$_CTO);
	    return tcb->inactive_timeout;
	    };

// See if it is time to retransmit.

	if (tcb->rx_timer < now)
	    {
	    signed long
		optsize,
		EOLF,SYNF,ACKF,FINF,
		Datasize,Dataptr,
		SEQsize,segsize,
	      bufsize,Buf;
		struct segment_structure * Seg;

// Set segment initial transmit time if this is the first time

	    if (tcb->rx_count == 0)
		{
		tcb->xmit_start_time = now;
		tcb->max_eff_data_size = tcb->max_seg_data_size ;
		} ;

	    tcb->rx_count = tcb->rx_count + 1;
//	    if (tcb->rx_count > 0)
//		{
//		tcb->max_eff_data_size = tcb->max_eff_data_size / tcb->rx_count ;
//		tcb->max_eff_data_size = MAX(tcb->max_eff_data_size, 512) ;
//		} ;

	    TS$RETRANS_SEGS = TS$RETRANS_SEGS+1;

// Compute new retrans time out based on old round trip time plus fudge factor

//	    Delta = tcb->round_trip_time + 
//		    ( (tcb->round_trip_time * tcb->rx_count) / 2);
//	    Delta = MAXU(MINU(MAX_RT_TIMEOUT,Delta),MIN_RT_TIMEOUT);
//	    Delta = MINU(MAXU(TCP$Compute_RTT(tcb),MIN_RT_TIMEOUT) *
//			 tcb->rx_count, MAX_RT_TIMEOUT) ;
	    delta = MINU(MAXU(TCP$Compute_RTT(tcb),MIN_RT_TIMEOUT),
			MAX_RT_TIMEOUT) ;
	    now = Time_Stamp();
	    tcb->rx_timer = now + delta;
	    XLOG$FAO(LOG$TCBCHECK,
		     "!%T Check_rexmit TCB !XL delta !UL!/",0,tcb, delta);

// Now, build and send a segment.
// Calculate buffer size & get control flag values

	    SYNF = (tcb->rx_ctl == M$SYN) || (tcb->rx_ctl == M$SYN_ACK);
	    ACKF = tcb->rx_ctl == M$SYN_ACK;
	    FINF = (tcb->rx_ctl == M$FIN);
	    if (SYNF)
	      optsize = SEND_OPT_SIZE;
	    else
	      optsize = 0;
//	    Datasize = MIN(tcb->srx_q_count,tcb->max_seg_data_size);
	    Datasize = MIN(tcb->srx_q_count, tcb->max_eff_data_size);
	    segsize = Datasize + optsize + TCP_HEADER_SIZE;

// Select network buffer size & allocate it

	    bufsize = DEVICE_HEADER+IP_HDR_BYTE_SIZE+segsize;
	    if (bufsize <= min_physical_bufsize) // check
	      bufsize = min_physical_bufsize;
	    else { 
	      if (bufsize <= max_physical_bufsize)
		bufsize = max_physical_bufsize;
	      else
		bufsize = segsize + DEVICE_HEADER + IP_HDR_BYTE_SIZE;
	    }
	    Buf = mm$seg_get(bufsize);
	    Seg = Buf + DEVICE_HEADER + IP_HDR_BYTE_SIZE;
	    Dataptr = Seg->sh$data;

// Set control flags and options for the segment

	    Seg->sh$control_flags = 0;
	    Seg->sh$c_ack = ACKF;
	    if (SYNF)
		{
		SYNF = Seg->sh$c_syn = TRUE;
		Send_TCP_Options(tcb,Seg);
		Dataptr = Dataptr + SEND_OPT_SIZE;
		};
	    if (FINF)
		Seg->sh$c_fin = TRUE;
	    EOLF = tcb->snd_push_flag &&
//		   ((tcb->snd_pptr - tcb->rx_seq) > 0);
		   (tcb->snd_pptr > tcb->rx_seq);

// Now, insert the retransmission data, if any.

	    XLOG$FAO(LOG$TCP,
		"!%T !UL RX TCB !XL,PTR=!XL,RXQ=!XL/!XL,CNT=!SL,CTL=!SL,new RXTO=!SL!/",
		0,now,tcb,tcb->srx_q_deqp,tcb->srx_q_base,tcb->srx_q_end,
		tcb->rx_count,tcb->rx_ctl,tcb->rx_timer);
	    if (Datasize > 0)
		{
		  CQ_DEQCOPY(&tcb->srx_q_queue,Dataptr,Datasize);
		TCP_MIB->MIB$tcpRetransSegs = TCP_MIB->MIB$tcpRetransSegs + 1;
		};
// Build the rest of the header

	    Build_Header(tcb,Seg,segsize,Datasize,tcb->rx_seq,EOLF,
			 FALSE,optsize / 4);

// Send the segment to IP

	    Send_Seg1(tcb,Seg,segsize,TRUE,Buf,bufsize);

	    return tcb->rx_timer;	// Next time retransmit for this TCB
	    };
//	RETURN tcb->rx_timer;	// Next time retransmit for this TCB
	};

//    RETURN %X"7FFFFFFF";	// Nothing on retransmission queue
    return (now + (20*CSEC));	// Nothing on retransmission queue

    };

//SBTTL "Queue Segment for ReTransmission."
/*

Function:

	Place segment on this TCB's retranmission queue.  Actually a Queue
	block is enqueued which in turn points to the segment & contains
	the timeout & last sequence # of the segment.  If the segment has
	never been transmitted then force the segment to time-out when it
	reaches the head of the RT queue.

Inputs:

	TCB = points to current TCB
	Buf = segment buffer ptr, segment is sub-structure of buffer.
	bufsize = buffer size in bytes.
	Seg = pointer to segment structure
	size = size in bytes of segment
	Xmit Count = # of times segment has been transmitted.

Implicit Inputs:

	tcb->Snd_Nxt has been updated to point to next byte past end of this
	segment.  Used to flag end of segment field in queue block, aids in
	ACKing the segment on the ReTransmission queue.

Outputs:

	None.

Side Effects:

	Segment is on this TCB's retransmission queue (RT_Qhead) & ready
	to be sent directly to IP should the timeout value expires.
	Segment round trip timer is started.
*/

void Rexmit_Enqueue(tcb,SEQsize,CTLTYPE)
     struct tcb_structure * tcb;
    {
#if 0
    REGISTER
	struct Queue_Blk_Structure(QB_RT_Fields) * QB;
#endif
    unsigned long
	now;

    now = Time_Stamp();		// get current time

// If this is the first data item on queue, set retransmit pointer

    XLOG$FAO(LOG$TCP,/*%STRING(*/
	     "!%T RX-ENQ: Time=!UL, TCB=!XL, RXTO=!SL, MXTO=!SL!/",
	     "!%T RX-ENQ: PTR=!XL/!XL, SEQ=!XL, SIZ=!SL, CNT=!SL, CTL=!SL, RXC=!SL!/"/*)*/,
	     0,now,tcb,tcb->rx_timer,tcb->rx_timeout,
	     0,tcb->srx_q_deqp,tcb->snd_q_deqp,tcb->rx_seq,SEQsize,
	     tcb->srx_q_count,tcb->rx_ctl, tcb->rx_count);

    if ((tcb->srx_q_count == 0))
	{
	tcb->srx_q_deqp = tcb->snd_q_deqp;

// If really the first thing on the queue, set sequence # and timers.
//~~~ Questionable code here, using RX_CTL seems like a kluge
// (Should probably use rx_count - HWM)

//	if (tcb->rx_ctl == 0)
//	if ((tcb->rx_seq < tcb->snd_nxt))
	if (tcb->rx_count == 0)
	    {
	    tcb->rx_seq = tcb->snd_nxt;
	    tcb->rx_count = 1;
	    tcb->xmit_start_time = now;	// Start round-trip-timer
	    tcb->rx_timer = now + tcb->calculated_rto;
	    tcb->rx_timeout = now + RX_TIMEVAL; // Max time on RX queue
	    };
	};

// Add data count for the new seg and set flags

//    tcb->rx_timeout = now + RX_TIMEVAL; // Max time on RX queue
    tcb->srx_q_count = tcb->srx_q_count + SEQsize;
    tcb->rx_ctl = CTLTYPE;
XLOG$FAO(LOG$TCP,/*%STRING(*/
	     "!%T RX-ENQ: Time=!UL, TCB=!XL, RXTO=!SL, MXTO=!SL!/",
	 "!%T RX-ENQ: PTR=!XL/!XL, SEQ=!XL, SIZ=!SL, CNT=!SL, CTL=!SL, RXC=!SL!/"/*)*/,
	     0,now,tcb,tcb->rx_timer,tcb->rx_timeout,
	     0,tcb->srx_q_deqp,tcb->snd_q_deqp,tcb->rx_seq,SEQsize,
	     tcb->srx_q_count,tcb->rx_ctl, tcb->rx_count);
    };

//SBTTL "Debug rtn: Display Specified TCB"
/*

Function:

	display contents of a TCB
	N.B. LOG_TCBDUMP is assumed to be on in log_state.
Inputs:

	TCB = address of a TCB data structure.

Outputs:

	none.

Side Effects:

	None.
*/
void TCP$Dump_TCB ( struct tcb_structure * tcb )
    {
      char fhstr_str[20], lhstr_str[20];
    unsigned long
	now	,
      rxtime	;
    struct dsc$descriptor
      stastr,
      fhstr={dsc$w_length:20, dsc$a_pointer:fhstr_str},
      lhstr={dsc$w_length:20, dsc$a_pointer:lhstr_str};

    if (tcb->srx_q_count > 0)
	{
	now = Time_Stamp();
	rxtime = tcb->rx_timer;
	}
    else
	{
	rxtime = 0;
	now = 0;
	};
    switch (tcb->state) {
	case CS$CLOSED: stastr.dsc$a_pointer = "Closed(CMU)."; break;
	case CS$LISTEN: stastr.dsc$a_pointer = "Listen."; break;
	case CS$SYN_SENT: stastr.dsc$a_pointer = "SYN_SENT."; break;
	case CS$SYN_RECV: stastr.dsc$a_pointer = "SYN_RECV."; break;
	case CS$ESTABLISHED: stastr.dsc$a_pointer = "Established."; break;
	case CS$FIN_WAIT_1: stastr.dsc$a_pointer = "FIN_Wait_1."; break;
	case CS$FIN_WAIT_2: stastr.dsc$a_pointer = "FIN_Wait_2."; break;
	case CS$TIME_WAIT: stastr.dsc$a_pointer = "Time_Wait."; break;
	case CS$CLOSE_WAIT: stastr.dsc$a_pointer = "Close_Wait."; break;
	case CS$CLOSING: stastr.dsc$a_pointer = "Closing."; break;
	case CS$LAST_ACK: stastr.dsc$a_pointer = "Last_ACK."; break;
	case CS$RESET: stastr.dsc$a_pointer = "Reset(CMU)."; break;
	case CS$INACTIVE: stastr.dsc$a_pointer = "Inactive(CMU)."; break;
	case CS$NAMELOOK: stastr.dsc$a_pointer = "NameLook(CMU)."; break;
    default: stastr.dsc$a_pointer = "Unknown state."; break;
	  }
    stastr.dsc$w_length=strlen(stastr.dsc$a_pointer);

    ASCII_Dec_Bytes(fhstr,4,tcb->foreign_host,fhstr.dsc$w_length);
    ASCII_Dec_Bytes(lhstr,4,tcb->local_host,lhstr.dsc$w_length);

    LOG$FAO(/*%STRING*/("!_PID:!_!XL!_State:!AS(!XL)!/",
		    "!_FH:!_!AS!_FP:!_!XL (!UL)!/",
		    "!_LH:!_!AS!_LP:!_!XL (!UL)!/",
		    "!_SND.WND: !UL!_RCV.WND: !UL!/",
		    "!_SND.NXT: !XL (!UL)!_RCV.NXT: !XL (!UL)!/",
		    "!_SND.UNA: !XL (!UL)!/",
		    "!_RX time: !UL (now + !UL)!/"),
		    tcb->user_id,stastr,tcb->state,
		    fhstr,tcb->foreign_port,tcb->foreign_port,
		    lhstr,tcb->local_port,tcb->local_port,
		    tcb->snd_wnd,tcb->rcv_wnd,
		    tcb->snd_nxt,tcb->snd_nxt,tcb->rcv_nxt,tcb->rcv_nxt,
		    tcb->snd_una,tcb->snd_una,rxtime,rxtime-now);
    };



//SBTTL "Service-Connections - Look for things to do."
/*

Function:

	Examine a known connections & look for things which need to be
	done.  

Inputs:

	None.

Outputs:

	Next time that Service_Connections needs to be called.

Side Effects:

	Data may be sent or delivered to the user.  Connection may be
	deleted due to timeouts.  Segment retransmission queue is examined.
*/

TCP$Send_Data();
void    TCP$Enqueue_ACK();
void    TCP$Send_ACK();
void    Do_Probe();

Check_TCB ( struct tcb_structure * tcb , signed long Idx , unsigned long now, unsigned long nxtime ) // check switch if
    {

    if ($$LOGF(LOG$TCBCHECK+LOG$TCBDUMP))
	LOG$FAO("!%T Servicing TCB !XL (now=!XL,nxtime=!XL)!/",0,tcb,
		now,nxtime); // check .now ..nxtime

// TCB sanity check
    if ((tcb == 0))
	return 0 ;

    if ($$LOGF(LOG$TCBDUMP))
	TCP$Dump_TCB(tcb);

 // If connection is inactive & the user has not accessed the connection
 // within the timeout period then delete the connection.

    if ((tcb->state == CS$INACTIVE) &&
	(tcb->inactive_timeout < now))
	{
	TCP$KILL_PENDING_REQUESTS(tcb,tcb->inactive_code);
	XLOG$FAO(LOG$TCBSTATE,"!%T Deleting inactive connection !XL!/",
		 0,tcb);
	TCB$Delete(tcb);
	} else {

 // If the connection has not received any packets at all, despite our
 // probes for too long, then we inactivate it here.

	  if ((tcb->state != CS$INACTIVE) && // check () on || and &&
     ((tcb->connection_timeout < now) && (tcb->connection_timeout != 0)) ||
	      ((tcb->user_timeout != 0) && (tcb->user_timeout < now)))
	{
	TCP$KILL_PENDING_REQUESTS(tcb,NET$_CTO);
	switch (tcb->state) {
	case CS$SYN_SENT: case CS$SYN_RECV: case CS$ESTABLISHED: case CS$FIN_WAIT_1:
	 case CS$FIN_WAIT_2: case CS$CLOSE_WAIT:
	    Send_RST(tcb);
	}
	if ($$LOGF(LOG$TCBSTATE))
	    {
	    if ((tcb->connection_timeout < now) && 
		(tcb->connection_timeout != 0))
		LOG$FAO("!%T Conn !XL inactivated - conn timeout!/",
			0,tcb);
	    else
		LOG$FAO("!%T Conn !XL inactivated - user timeout!/",
			0,tcb);
	    };
	TCP$KILL_PENDING_REQUESTS(tcb,NET$_CTO);
	TCP$Inactivate_TCB(tcb,NET$_CTO);
	} else {

// Is there a pending IO function being held until a network event occurs?
// IF so, check the timer & clobber(inactivate) the connection.

	  if (tcb->pending_io && (tcb->function_timer < now))
	{
	tcb->pending_io = FALSE;
	if (tcb->curr_user_function == M$INTERNAL)
	    {

// For internal TCB"s (i.e. TVT"s), just call the timeout handler.

	    (tcb->timeout_routine)(tcb);
	    }
	else
	    {
	    if (tcb->argblk != 0)
		USER$Err(tcb->argblk,NET$_FTO);

// Process According to TCP function requested.

	    switch (tcb->curr_user_function) {
	    case U$OPEN: case U$CLOSE: case M$CANCEL:
		{
		TCP$KILL_PENDING_REQUESTS(tcb,NET$_FTO);
 
// RESET just in case.

		switch (tcb->state) {
		case CS$SYN_SENT: case CS$SYN_RECV: case CS$ESTABLISHED: case CS$FIN_WAIT_1: case CS$FIN_WAIT_2: case CS$CLOSE_WAIT:
		    Send_RST(tcb);
		}
		XLOG$FAO(LOG$TCBSTATE,
		    "!%T Function timeout: FCN=!XL, TCB=!XL!/",
		     0,tcb->curr_user_function,tcb);
		TCP$Inactivate_TCB(tcb,NET$_FTO); // Inactivate connection
		};
	    };
	    };
	} else {
 
// Test for time-wait timeout.  If true then delete the connection.
 
    if ((tcb->state == CS$TIME_WAIT) &&
	(tcb->time_wait_timer < now))
	{
	TCP$KILL_PENDING_REQUESTS(tcb,NET$_TWT);
	XLOG$FAO(LOG$TCBSTATE,"!%T Time-wait expired, conn=!XL!/",
		 0,tcb);
	TCB$Delete(tcb);
	} else {
 
// Connection is valid, Check retransmission queue & see if we can move
// some data. Better check SYN-wait queue also.

#if 0 
    [OTHERWISE]: 
#endif
	{
	unsigned long
		delay,
		min_time,
		rx_time ;

//	XLOG$FAO(LOG$DEBUG,
//		 "!%T Rexmit_Queue : count = !SL!/",
//		    0,tcb->srx_q_count);
	rx_time = TCP$Check_Rexmit_Queue(tcb);
	XLOG$FAO(LOG$DEBUG,
		 "!%T TCP$Check_Rexmit_Queue : RVal = !XL!/",
		    0,rx_time);

	switch (tcb->state) {
	  case CS$CLOSE_WAIT:
	    {	// Try to send some data
	    XLOG$FAO(LOG$TCBCHECK,
		     "!%T Sending data for TCB !XL!/",0,tcb);
	    TCP$Send_Data(tcb);
	    };
	    break;
	case CS$ESTABLISHED: // check duplicate from above and below 
	    {
	    {	// Try to send some data
	    XLOG$FAO(LOG$TCBCHECK,
		     "!%T Sending data for TCB !XL!/",0,tcb);
	    TCP$Send_Data(tcb);
	    };
	    if ((tcb->ack_timer < now))
		{
		XLOG$FAO(LOG$TCP,
			 "!%T Sending spontaneous ACK, TCB=!XL!/",
			 0,tcb);
//		TCP$Send_ACK(tcb);
		TCP$Enqueue_ACK(tcb);
		};
	    if ((tcb->probe_time < now) && Keep_Alive)
		DO_Probe(tcb); // Time to send another probe
	    };
	    if (tcb->is_tvt)
		{
		TELNET_INPUT(tcb);
		TELNET_OUTPUT(tcb);
		}
	    else
		TCP$Deliver_User_Data(tcb);
	    break;
	case CS$FIN_WAIT_1: case CS$FIN_WAIT_2:
	    {	// Try to receive some data
	    if (tcb->is_tvt)
		{
		TELNET_INPUT(tcb);
		TELNET_OUTPUT(tcb);
		}
	    else
		TCP$Deliver_User_Data(tcb);
	    };
	    break;
	};

	XLOG$FAO(LOG$DEBUG,
		 "!%T TCP$Check_Rexmit_Queue : RX_Time = !XL!/",
		    0,rx_time);
	XLOG$FAO(LOG$DEBUG,
		 "!%T TCP$Check_Rexmit_Queue : delay = !XL!/",
		    0,tcb->snd_delay_timer);
	XLOG$FAO(LOG$DEBUG,
		 "!%T TCP$Check_Rexmit_Queue : nxtime = !XL!/",
		 0,nxtime); // check ..nxtime

	if ((tcb->state != CS$LISTEN) && (tcb->snd_q_count > 0))
	    {
	    delay = MINU((tcb->round_trip_time / 2), BASE_RT_TIMEOUT) ;
	    delay = MAXU(delay, 33) ;
	    min_time = now + delay ;
//	    IF ((tcb->snd_delay_timer < now) && (tcb->snd_q_count > 0))THEN
	    if ((tcb->snd_q_count > tcb->max_eff_data_size))
		min_time = tcb->snd_delay_timer ;
	    min_time = MAXU(min_time, now) ;
	    min_time = MINU(rx_time, min_time) ;
//	    nxtime = MINU(..nxtime,rx_time);
	    nxtime = MINU(nxtime, min_time); // check ..nxtime
	    XLOG$FAO(LOG$DEBUG,
		     "!%T TCP$Check_Rexmit_Queue : nxtime = !XL!/",
		     0,nxtime); // check ..nxtime

	    if (tcb->pending_ack)
		{
		XLOG$FAO(LOG$TCP,
		         "!%T Sending pending ACK, TCB=!XL!/",0,tcb);
//		TCP$Send_ACK(tcb); // Give the window update/ACK
		TCP$Enqueue_ACK(tcb); // Give the window update/ACK
		}
	    }
	};
	};
	}
	}
	}
    return 1;
    }


//SBTTL "Service-Connections - Look for things to do."
/*

Function:

	Examine all known connections & look for things which need to be
	done.  This rtn is the 3rd major processing rtn called from TCP
	mainline (Start-Network).

Inputs:

	None.

Outputs:

	Next time that Service_Connections needs to be called.

Side Effects:

	Data may be sent or delivered to the user.  Connection may be
	deleted due to timeouts.  Segment retransmission queue is examined.
*/

TCP$Service_Connections (void)
{
  extern	VTCB_Scan();
  register unsigned long now;
  register struct tcb_structure * tcb;
  unsigned long nxtime;
  signed long count;
  //  struct _iosbdef ios;

// Examine ALL known Connections.

    now = Time_Stamp();
    nxtime = now+DAYSEC;	// Far into the future....

    if ($$LOGF(LOG$TCBCHECK))
	LOG$FAO("!%T !UL Servicing all TCBs, !/",0,now);

    count = VTCB_Scan ( Check_TCB , now , nxtime );
    XLOG$FAO(LOG$DEBUG,"!%T Service_connections: now=!XL nxtime=!XL!/",0,
	     now,nxtime); 

    if ($$LOGF(LOG$TCBCHECK))
	LOG$FAO("!%T !UL Serviced !SL TCBs,!/",0,now,count);


// Examine Syn wait list for entries that have timed out.

    SEG$Timeout_Syn_Wait_List(now);

// Return calculated next time a TCB needs to be serviced

    return nxtime;
    };

//SBTTL "TCP$SEND_ENQUEUE - Copy send data from user send queue to circular buffer"

void TCP$SEND_ENQUEUE(tcb,bufcount,buf,pushf)
	struct tcb_structure * tcb;
	signed long * bufcount, *buf; // check my adds
    {
    signed long
	usedcount,
	lbptr,
	lbcount,
	copycount;
    lbptr = *buf; // check for both these
    lbcount = *bufcount;

// Compute amount that the queue can take, enqueue it.

    usedcount = tcb->snd_q_count + tcb->srx_q_count;
    copycount = MIN(lbcount,tcb->snd_q_size-usedcount);
    XLOG$FAO(LOG$TCP,"!%T SEND-ENQ EQ=!XL,DQ=!XL,RX=!XL,SNQ=!XL/!XL,CNT=!SL,SIZ=!SL,PSH=!SL!/",
	     0,tcb->snd_q_enqp,tcb->snd_q_deqp,tcb->srx_q_deqp,
	     tcb->snd_q_base,tcb->snd_q_end,usedcount,copycount, pushf);
#if 0
    // check wait
    $$KCALL(CQ_ENQUEUE,&tcb->snd_q_queue,lbptr,copycount);
#endif

// Update user pointer and counter

    bufcount = lbcount - copycount;
    buf = lbptr + copycount;

// if PUSH was set, then advance the send Push pointer to end of this buffer

    if (pushf)
	{
	tcb->snd_push_flag = TRUE;
	tcb->snd_pptr = tcb->snd_nxt + tcb->snd_q_count;
	};
    }

//SBTTL "SEND DATA: Segmentize data & send segment over the network."
/*

Function:
	Read user data from send queue (SND_Qhead) & segmentize it.  Send queue
	is structured in the following format:
	Each element of the queue is a queue_block.  Each queue_Block(QB)
	contains information about the user data buffer it points to 
	(size, start address, End Of Letter & Urgent flags).

Inputs:

	TCB = TCB pointer

Implicit Inputs:

	tcb->asm_bc = # of bytes in current user send buffer left. 1st
			element in tcb->snd_qhead.
	tcb->asm_ptr = address of user data in current send buffer.
	tcb->max_seg_data_size = max # of data bytes per segment receiver
				 will accept.

Outputs:

	0 if data not sent because of accumulation strategy, else count of
	data octets sent.

Side Effects:

	As much of the users data is sent with respect to the size of
	the send window.

*/

TCP$SEND_DATA(struct tcb_structure * tcb)
{
  struct segment_structure * seg;
  struct queue_blk_structure(qb_send_fields) * qb;
  unsigned long
    now,
    delay;
  signed long
    seqsize,
    delay_flag	 = 0,
    send_flag	 = 0,
    useable,
    bufptr,
    bufsize,
    segsize,
    dcount,
    eol,
    urg;

// Get time of day

    now = Time_Stamp() ;

// If there is anything on the retransmission queue, then don't try to queue
// anything else up ("Nagle Algorithm")
//    if (((tcb->snd_nxt > tcb->snd_una) && (! tcb->de_nagle)))
//	RETURN 0;

    if (tcb->srx_q_count != 0)
	return 0 ;
    if ((tcb->snd_delay_timer == 0))
	tcb->snd_delay_timer = now ;
//    if ((tcb->snd_delay_timer < now))
//	{
//	// Timer still active, but maybe we can send anyway
//	IF ((tcb->snd_nxt > tcb->snd_una) AND
//	    (! tcb->de_nagle) AND
//	    ((tcb->snd_q_count + tcb->srx_q_count) < tcb->max_eff_data_size)) THEN
//		RETURN 0 ;		// Nope
//	} ;

    useable = tcb->snd_una + tcb->snd_wnd - tcb->snd_nxt ;
    if ((MINU(tcb->snd_q_count, useable) > tcb->max_eff_data_size))
	send_flag = 1 ;
    if (((tcb->snd_nxt == tcb->snd_una) &&
	 (MINU(tcb->snd_q_count, useable) >= (tcb->snd_max_wnd / 2))))
	send_flag = 1 ;
    if (((tcb->snd_nxt == tcb->snd_una) && (tcb->snd_push_flag) &&
	 (tcb->snd_q_count <= useable)))
	send_flag = 1 ;
    if ((tcb->snd_delay_timer >= now))
	send_flag = 1 ;
    if ((tcb->snd_nxt > tcb->snd_una))
	send_flag = 1 ;
    if ((! tcb->de_nagle))
	send_flag = 1 ;
    if ((tcb->snd_q_count >= tcb->max_eff_data_size))
	send_flag = 1 ;
    if (((tcb->snd_nxt > tcb->snd_una) && (! send_flag)))
	return 0 ;		// Nope

// If we received a ICMP Source Quench, then see if it's time to try to
// send more data.

    if ((tcb->squench))
	{
	if ((now < tcb->squench_timer))
		{			//  Still in Source Quench mode
		return 0 ;
		}
	else
		{
		tcb->squench = FALSE ;	//  Reset Source Quench flag
		} ;
	} ;

// Generate TCP segments from circular queue until all send requests are
// exhausted or the SEND WINDOW goes to 0.

    dcount = 0;
    while ((tcb->snd_q_count > 0) && (tcb->snd_wnd > 0))
	{
	eol = urg = FALSE;

// How much user data is avail to send? Up to tcb->max_seg_data_size
// Make sure it fits within the send-window.

//	seqsize = MIN(tcb->snd_q_count,tcb->max_seg_data_size);
	seqsize = MIN(tcb->snd_q_count, tcb->max_eff_data_size);
	seqsize = MINU(seqsize,tcb->snd_wnd);

// get a buffer large enough to contain: device header, IP, TCP headers and
// data. Round to nearest big buffer size.

	segsize = seqsize + TCP_HEADER_SIZE;
	bufsize = segsize + DEVICE_HEADER + IP_HDR_BYTE_SIZE;

	if (bufsize <= min_physical_bufsize)
	    bufsize = min_physical_bufsize;
	else {
	  if (bufsize <= max_physical_bufsize)
	    bufsize = max_physical_bufsize;
	  else
	    bufsize = segsize + DEVICE_HEADER + IP_HDR_BYTE_SIZE;
	}

// Get buffer, point at segment, clear flags

	bufptr = mm$seg_get(bufsize);
	seg = bufptr+DEVICE_HEADER+IP_HDR_BYTE_SIZE;
	seg->sh$control_flags = 0;

// Set PUSH and advance PUSH pointer

	if (tcb->snd_push_flag)
//	    if ((tcb->snd_pptr - tcb->snd_nxt) > 0)
	    if ((tcb->snd_pptr > tcb->snd_nxt))
		{
		eol = TRUE;
//		if ((tcb->snd_nxt + seqsize) - tcb->snd_pptr > 0)
		if (((tcb->snd_nxt + seqsize) > tcb->snd_pptr))
		    tcb->snd_push_flag = FALSE;
		};

//!~~~ Set URG and urgent pointer here?
// Set retransmission info
	
	ReXmit_Enqueue(tcb,seqsize,0);

// Remove from the send circular queue into the segment

	XLOG$FAO(LOG$TCP,
	   "!%T Sending for TCB !XL,SEQ count=!SL,DQ=!XL,EQ=!XL,SNQ=!XL/!XL!/",
	   0,tcb,seqsize,tcb->snd_q_deqp,tcb->snd_q_enqp,
	   tcb->snd_q_base,tcb->snd_q_end);
	XLOG$FAO(LOG$TCP,
	   "!%T Sending for TCB !XL, EFF = !SL!/",
	   0, tcb, tcb->max_eff_data_size);
	CQ_Dequeue(tcb->snd_q_queue,seg->sh$data,seqsize);

// Deduct from the window and add to statistics

	tcb->snd_wnd = tcb->snd_wnd - seqsize;
	TS$DBX = TS$DBX + seqsize;

// Send the segement to IP (internet Protocol handler).

	Build_Header(tcb,seg,segsize,seqsize,tcb->snd_nxt,eol,urg,0);

// Advance SND_NXT for this segment

	tcb->snd_nxt = tcb->snd_nxt + seqsize;

// Send the segment to IP for transmission.

	Send_Seg1(tcb, seg, segsize, TRUE, bufptr, bufsize);
	TS$SX = TS$SX + 1;
	dcount = dcount+seqsize;
	TCP_MIB->MIB$tcpOutSegs = TCP_MIB->MIB$tcpOutSegs + 1;

// If we got here by virtue of expired timer, and not enough left to send
// another full sized packet, or window is too small then exit to avoid
// sending runts.
// (Nice in theory, but it doesn't work...)
//	IF (((Delay_Flag) AND
//	     (tcb->max_seg_data_size > tcb->snd_q_count)) OR
//	     (tcb->snd_wnd < tcb->snd_q_count)) THEN
//		EXITLOOP ;
};			//WHILE

// If circular queue is not full, then insert anything we have onto it.
//!~~~ Warning: we count on the RX queue being empty here (known above)

    if (tcb->snd_q_count < tcb->snd_q_size)
	{
	qb = tcb->snd_qhead;
	while (qb != tcb->snd_qhead)
	    {
	    signed long
	      nqb;
	    struct user_send_args * uargs;

// Enqueue as much data as possible from this user buffer

	    TCP$Send_Enqueue(tcb,qb->sn$size,qb->sn$data,qb->sn$eol);

// If the user buffer still has data in it, then we ran out of queue space.

	    if (qb->sn$size != 0)
	      break; // check

// Otherwise, we're done with this buffer. Post the I/O, dequeue the block, and
// advance to the next entry on the queue.

	    uargs = qb->sn$uargs;
	    User$Post_IO_Status(uargs,SS$_NORMAL,qb->sn$size,0,0);
	    mm$uarg_free(uargs);
	    nqb = qb->sn$next;
	    REMQUE(qb,qb);
	    mm$qblk_free(qb);
	    qb = nqb;
	    };
	};

// Pending Close & ALL user data sent?
// If true then send a FIN.

    if ((tcb->pending_close) && (tcb->snd_q_count == 0))
	{
	tcb->pending_close = FALSE;
	Send_FIN(tcb);

	if (tcb->state == CS$ESTABLISHED)
	  TCP$SET_TCB_STATE(tcb,CS$FIN_WAIT_1);
	else

// Technically, this violates the spec, but we got this way when a CLOSE was
// done in CLOSE-WAIT, so we should really be in LAST-ACK (RFC793 is ambiguous
// on this point).

	    if (tcb->state == CS$CLOSING)
		TCP$SET_TCB_STATE(tcb,CS$LAST_ACK);
	};

    // Reset delay timer for next time
    now = Time_Stamp() ;
//    Delay = (tcb->round_trip_time / 10) ;
    delay = (tcb->round_trip_time / 2);
    delay = MINU(delay, 100) ;
    delay = MAXU(delay, 10) ;
    tcb->snd_delay_timer = now + delay ;

    return dcount;
}


//SBTTL "Send-ACK: Check if we can piggyback ACK or must send a lone ACK"
/*

Function:

	Transmit an ACK.  Check to see if we can send a piggyback ACK on
	an out going data segment or must we send a lone ACK control
	segment.

Inputs:

	TCB - TCB pointer.

Outputs:

	None.

Side Effects:

	An "ACK" control segment maybe sent if there is no data to send
	for this connection or the send window is zero.
*/


void TCP$Enqueue_ACK(struct tcb_structure * tcb)
    {
    unsigned long
	delay,
	now;

// Queue an ACK up for short term delivery

    XLOG$FAO(LOG$TCP,"!%T TCP$Enqueue_ACK TCB !XL!/",
	  0, tcb);

    tcb->pending_ack = TRUE ;

// Get Time of Day

    now = Time_Stamp() ;

    if (((now >= tcb->delayed_ack_timer) ||
	((tcb->old_rcv_nxt + (2 * tcb->max_eff_data_size)) >= tcb->rcv_nxt)))
	{
	TCP$Send_ACK(tcb) ;
//	$DCLAST(ASTADR = TCP$Send_ACK,
//		ASTPRM = TCB);
//	tcb->delayed_ack_timer = Now + DELAYED_ACK_INTERVAL;
	delay = MINU((tcb->round_trip_time / 2), DELAYED_ACK_INTERVAL) ;
	delay = MAXU(delay, 20);
	tcb->delayed_ack_timer = now + delay ;
	} ;
//    else
//	{
//	if (((Now + Delayed_ACK_Interval) (tcb[Delayed_ACK_Timer)))
//		{
//		} ;
//	} ;

    }


//SBTTL "Send-ACK: Check if we can piggyback ACK or must send a lone ACK"
/*

Function:

	Transmit an ACK.  Check to see if we can send a piggyback ACK on
	an out going data segment or must we send a lone ACK control
	segment.

Inputs:

	TCB - TCB pointer.

Outputs:

	None.

Side Effects:

	An "ACK" control segment maybe sent if there is no data to send
	for this connection or the send window is zero.
*/


void TCP$SEND_ACK(struct tcb_structure * tcb)
    {

// If user data needs to be sent then piggyback the ACK.
// Otherwise send a ACK control segment.

XLOG$FAO(LOG$TCP,"!%T TCP$Send_ACK, Rcv window: !UL, ACK number: !XL (!UL)!/",
	  0, tcb->rcv_wnd, tcb->rcv_nxt, tcb->rcv_nxt);

    if (tcb->snd_q_count == 0)
      TCP$Send_CTL(tcb,M$ACK);
    else
	{

// Data available to send, is the send window greater than 0?  Must be
// or TCP$Send_Data will not attempt to send any.

	if ((tcb->state == CS$ESTABLISHED) && (tcb->snd_wnd > 0))
	    {
	    if (TCP$Send_Data(tcb) == 0)
	      TCP$Send_CTL(tcb,M$ACK);
	    }
	else
	    TCP$Send_CTL(tcb,M$ACK); // Send an ACK control segment.
	}
    }

//SBTTL "SEND CTL: Send a Control Segment"
/*

Function:

	Send a control segment to IP.
	Control segments are always sent regardless of the size of the
	send window because we need to force the other TCP to send us it's
	current window size.  Case of foreign TCP closes window we need
	to become aware when it opens back up.

Inputs:

	TCB = Points at current TCB
	Type = Control Message identifier(M$xxx type).

Outputs:

	None.

Side Effects:

	TCP Control segment (Control_Seg_Size) is send by IP over the
	network.  If control-seg is an ACK then clear pending_ack flag in
	the TCB.

*/
TCP$SEND_CTL(struct tcb_structure * tcb,long type)
    {
      struct segment_structure * seg;
    signed long
	bufsize,
	bufptr,
	segsize  = TCP_HEADER_SIZE,
	option_offset = 0,
	seqspace = 1;	// Sequence Space occupied by this segment.
				// assume protected control segment (SYN or FIN)

// Fix segment size if doing SYN - need to send standard options

    if ((type == M$SYN) || (type == M$SYN_ACK))
	{
	segsize = segsize + SEND_OPT_SIZE; // Include room for our options
	option_offset = SEND_OPT_OFFSET; // And indicate how long they are
	};

// Get a buffer large enough to contain tcp header, data, ip header and device
// header.  Index into buffer & overlay segment header structure.

    bufsize = DEVICE_HEADER + IP_HDR_BYTE_SIZE + segsize;
    if (bufsize <= min_physical_bufsize)
      bufsize = min_physical_bufsize;
    else
	if (bufsize <= max_physical_bufsize)
	    bufsize = max_physical_bufsize;
    bufptr = mm$seg_get(bufsize);	// Carve out a control segment structure.
    seg = bufptr + DEVICE_HEADER + IP_HDR_BYTE_SIZE; // point at segment start.

// build the segment

    seg->sh$control_flags = 0;		// Clear control Flags.
    switch (type)
      {
    case M$ACK:
	{
	seg->sh$c_ack = TRUE;
	seqspace = 0;
	tcb->pending_ack = FALSE;		// clear pending ack flag.
	};
	break;
    case M$RST:
	{
	seg->sh$c_rst = TRUE;
	seqspace = 0;
	};
	break;
// make this a SYN control segment with max receive data size option present.
// could be syn_ack.

    case M$SYN: case M$SYN_ACK:
	{
	seg->sh$c_syn = TRUE;
	if (type == M$SYN_ACK)
	    seg->sh$c_ack = TRUE;

// include max data size we will receive option.

	Send_TCP_Options(tcb,seg);
	};
	break;
    case M$FIN:
	seg->sh$c_fin = TRUE;
      }

// If this segment occupies sequence number space (i.e. SYN or FIN), then
// queue it up for retransmission.

    if (seqspace > 0)
	Rexmit_Enqueue(tcb,0,type);

// 2 false parameters are for EOL & Urgent, not on control segments.

    Build_Header(tcb,seg,segsize,seqspace,tcb->snd_nxt,
		 FALSE,FALSE,option_offset);

// Advance SND_NXT for FIN/SYN

    tcb->snd_nxt = tcb->snd_nxt + seqspace;

// Only ACKable segments update the probe timer.

    TS$SX = TS$SX + 1;		// track segments transmitted.
    TCP_MIB->MIB$tcpOutSegs = TCP_MIB->MIB$tcpOutSegs + 1;
    if (seqspace > 0)
      Send_Seg1(tcb, seg, segsize, TRUE, bufptr, bufsize);
    else
      Send_Seg0(tcb, seg, segsize, TRUE, bufptr, bufsize);
    }

//%SBTTL "Send a probe packet"
/*
Function:

	Called periodically for connections which are idle to make sure
	that they are still alive. We send an empty packet which is
	intended to generate some ACK response. Currently, we will send
	an unacceptable segment with SYN and ACK on with a bogus sequence
	number. According to the TCP spec, such a segment should either
	generate an ACK with the correct sequence numbers or should generate
	an RST if the connection does not exist.
*/

void DO_PROBE(struct tcb_structure * tcb)
    {
      const long
	segsize = TCP_HEADER_SIZE;
      const long
	bufsize = DEVICE_HEADER+IP_HDR_BYTE_SIZE+TCP_HEADER_SIZE;
#define BUFSIZE DEVICE_HEADER+IP_HDR_BYTE_SIZE+TCP_HEADER_SIZE
      static long
	buf[(BUFSIZE+3)/4];
      const struct segment_structure *
	seg = buf+DEVICE_HEADER+IP_HDR_BYTE_SIZE ; // check

// Fill in the segment

    seg->sh$source_port = tcb->local_port;
    seg->sh$dest_port = tcb->foreign_port;
    seg->sh$seq = (tcb->snd_nxt ^ -1);
    seg->sh$control_flags = 0;
    seg->sh$c_ack = TRUE;
    seg->sh$c_syn = TRUE;
    seg->sh$ack = tcb->rcv_nxt;
    seg->sh$data_offset = TCP_DATA_OFFSET;
    seg->sh$window = tcb->rcv_wnd;
    seg->sh$urgent = 0;
    seg->sh$checksum = 0;

    if ($$LOGF(LOG$TCP))
	{
	LOG$FAO("!%T Sending TCB inactivity probe!/",0);
	SEG$Log_Segment(seg,segsize,FALSE,FALSE);
	};

// Byteswap header and do checksum

    seg->sh$seq = ROT(seg->sh$seq,16);
    seg->sh$ack = ROT(seg->sh$ack,16);

    SwapBytes(TCP_HEADER_SIZE/2,seg);
    seg->sh$checksum = Gen_Checksum(segsize,seg,tcb->local_host,
				    tcb->foreign_host,TCP_PROTOCOL);
// Finally, send the packet.

    TCP_MIB->MIB$tcpOutSegs = TCP_MIB->MIB$tcpOutSegs + 1;
    Send_Seg1(tcb, seg, segsize, FALSE, buf, bufsize);
    }


//%SBTTL "Build Header: Fill in TCP segment header."
/*

Function:

	Fill in the TCP segment header fields & generate the segment
	checksum.

Inputs:

	TCB = points at current TCB
	seg = points at segment structure.
	Length = Length of TCP header & data in bytes.
	SeqSpace = Amount of segment space contained in this segment.
	EOL = End Of Letter boolean.
	URG = Urgent boolean.
	options = # of 4 byte options fields present. 0 = none.

Outputs:

	none.

Side Effects:

	TCB segment space counters are updated.
*/

void Build_Header(tcb,seg,length,seqspace,seqstart,
		     eol,urg,optoff)
     struct tcb_structure * tcb;
     struct segment_structure * seg;
    {
    signed long
	swap_wordSize;

    seg->sh$source_port = tcb->local_port;
    seg->sh$dest_port = tcb->foreign_port;
    seg->sh$seq = seqstart;

// Is this a synchronized connection?  True implies ack field is valid.

    if (tcb->is_synched)
	{
	seg->sh$c_ack = TRUE;
	tcb->pending_ack = FALSE;
	};

// special case reset segs for ack bit.

    if ((seg->sh$c_rst))
	{
	if (seg->sh$seq == 0)
	  seg->sh$c_ack = TRUE;
	else
	  seg->sh$c_ack = FALSE;
	};

// If ack control is asserted then update spontaneous ack timer.
// Also, update last RCV_NXT/RCV_WND that were ACK'ed

    if ((seg->sh$c_ack))
	{
	tcb->ack_timer = Time_Stamp() + ACK_INTERVAL;
	tcb->old_rcv_nxt = tcb->rcv_nxt;
	tcb->old_rcv_wnd = tcb->rcv_wnd;
	};

    seg->sh$ack = tcb->rcv_nxt;
    seg->sh$data_offset = TCP_DATA_OFFSET + optoff;
    seg->sh$window = tcb->rcv_wnd;

// Process EOL(End Of Letter) & Urgent flags.

    if (eol)
	seg->sh$c_eol = TRUE;

    if (urg)
	{
	seg->sh$c_urg = TRUE;
	seg->sh$urgent = (tcb->snd_nxt + seqspace) - 1;
	};

    if ($$LOGF(LOG$TCP))
	SEG$Log_Segment(seg,length,FALSE,FALSE);

// Generate the Segment Checksum, Includes TCP Pseudo Header.

    seg->sh$checksum = 0;	// For checksum routine.
//   XLOG$FAO(LOG$TCP,"Sending SEQ #: !XL (!UL)!/",Seg->SH$SEQ,Seg->SH$SEQ);

// Swap bytes within TCP header words(16-bits) so checksum is correct for
// order in which the bytes are transmitted.

    seg->sh$seq = ROT(seg->sh$seq,16);
    seg->sh$ack = ROT(seg->sh$ack,16); // swap 16-bit words in 32-bit fullwords

// Swap the bytes in the header. Note that options are always in network byte
// order and thus need not be swapped.

    SwapBytes(TCP_HEADER_SIZE/2,seg);

// generate the actual checksum now that all the bytes are in the order of
// transmission.

    seg->sh$checksum = Gen_Checksum(length,seg,tcb->local_host,
				    tcb->foreign_host,TCP_PROTOCOL);
    }


