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
//TITLE 'Define IPACP TCP related structures'
/*

Module:

	TCP.REQ

Facility:

	Define the TCP related things (but not TCP itself)

Author:

	Bruce R. Miller, CMU Network Development
	Borrowing Heavily from other modules.
	Copyright (c) 1986,1987,1990 Carnegie-Mellon University

Modification history:

	10-Dec-1991	Henry W. Miller		USBR
	Add TCB variable SND_Delay_Timer, to implement RFC1122 SWS
	avoidance algorithim.

	05-Dec-1991	Henry W. Miller		USBR
	Add SND_ACK_Threshold variable to TCB.
	Make TCB[ACK_Size] an unsigned long.

	13-Nov-1991	Henry W. Miller		USBR
	Add Delayed_ACK_Timer to TCB.
	Make flag word in TCB a long.  (We ran out of bits)
	Remove old obsolete crap.

	11-Nov-1991	Henry W. Miller		USBR
	Change SND_WL to unsigned longs as per RFC1122.

	17-Sep-1991	Henry W. Miller		USBR
	Bring retransmission parameters into line with RFC1122.
	Add flag in TCB for disabling Nagle algorithim.

	25-Jan-1991	Henry W. Miller		USBR
	Make WINDOW_DEFAULT and ACK_THRESHOLD configurable variables.

	15-Jan-1991	Henry W. Miller		USBR
	Changed SND_WND, RCV_WND, and Old_RCV_WND to unsigned long values
	as recommended by RFC1122.

	13-Jan-1991	Henry W. Miller		USBR
	Comment out TCPTTL; now a configurable variable.

*/

// not yet #include "netxport.h"
#include "netcommon.h"
#include "netvms.h"
#include "structure.h"
#include <descrip.h>


//SBTTL 'Literals'

// TCB Connection States

    #define CS$CLOSED 0	// fictious, TCB does NOT exist.
    #define CS$LISTEN 1
    #define CS$SYN_SENT 2
    #define CS$SYN_RECV 3
    #define CS$ESTABLISHED 4
    #define CS$FIN_WAIT_1 5
    #define CS$FIN_WAIT_2 6
    #define CS$TIME_WAIT 7
    #define CS$CLOSE_WAIT 8
    #define CS$CLOSING 9
    #define CS$LAST_ACK 10
    #define CS$RESET 11
    #define CS$INACTIVE 12
    #define CS$NAMELOOK 13

// Control Segment type identifiers

    #define M$SYN 1	// Send a SYN control segment
    #define M$FIN 2
    #define M$ACK 3
    #define M$RST 4
    #define M$SYN_ACK 5	// SYN & ack together.

// Misc.

    #define TCPTOS 0			// IP Service type: Datagram,Highest Reliablity
				// Reliability over speed, Speed: High.
    #define TCPDF FALSE		// OK to fragment datagrams
    #define SND_Q_MAX 8		// Max number of buffered user sends.

// Misc Time Values in Hundredths seconds.

    #define MAX_SEG_LIFETIME 2*MINUTE	// seconds.
    #define MIN_RT_TIMEOUT 1*CSEC	// Min allowable retransmission time
    #define MAX_RT_TIMEOUT 4*MINUTE	// Max allowable Retransmission time.
    #define BASE_RT_TIMEOUT 3*CSEC	// Base (initial) Round Trip timer.
    #define PROBE_IVAL MINUTE	// Inactivity probe timer
    #define ACK_INTERVAL MINUTE	// spontaneous ack interval timer.
    #define DELAYED_ACK_INTERVAL 20	// Delayed ACK interval, 200ms
    #define SQUENCH_INTERVAL 2*CSEC	// Source Quench Interval, 2 Seconds
    #define FUNCTION_TIMEOUT 10*CSEC	// User function.
    #define ACTIVE_OPEN_TIMEOUT 30*CSEC// Amount of time for active open to happen
    #define PASSIVE_OPEN_TIMEOUT DAYSEC // Amount of time for passive open
    #define NAMELOOK_TIMEOUT 2*MINUTE// How long to wait for name lookup
    #define CLOSE_TIMEOUT 2*MINUTE	// How long to wait for connection to close
    #define CONN_TIMEOUT 10*MINUTE	// How long before connection is dead
    #define RX_TIMEVAL 4*MINUTE	// Retransmit queue time limit
    #define INACTIVE_TCB_TIMEOUT 5*CSEC // Time which a TCB will remain inactive
				// before being deleted.
    #define DEFAULT_SEND_TIMEOUT 300*CSEC// Time allowed for a user's buffer
				// to be sent.


//SBTTL 'Define a TCB: (Transmission Control Block) data structure.'
/*

Define a TCB: (Transmission Control Block) data structure.

The basic requirements of a TCB are derived from the DOD TCP document, page 19.
The TCB is the data structure which TCP uses to control a specific connection.
All active connections (e.g., NOT closed) have an associated TCB.  
Each TCB can be located two ways:
1) Local_Connection_ID is the address of the TCB
2) Each local port has associated a linked-list of TCB's which use that local
   port as part of a connection (socket-pair).
When the TCB is on the free TCB list (Free_TCB_List, Queue header) the
fields LP_Next & LP_Back are used as forward & backwards queue pointers.
*/

struct  tcb_structure {
    void * lp_next;	// LP queue Forward Link pointer.
    void * lp_back;	// LP queue, Backwards Link.
    unsigned int foreign_host;	// Foreign host address
    unsigned int foreign_port;	// Foreign port
    unsigned int local_host;	// Local address of connection
    unsigned int local_port;	// Local Port #
    void * rf_qhead;	// Network: received "future" seg queue
    void * rf_qtail;
    signed int rf_qcount;	// Count of items on future queue
    void * ur_qhead;	// User: receive Data request queue.
    void * ur_qtail;
    void * snd_qhead;	// User: send Segment request queue
    void * snd_qtail;

//
// Security		= [$SWord],	// not implememted.
// Precedance		= [$SWord],	// not implememted.

// Send sequence number variables
// Bytes(4) gives 32-Bit unsigned integer for mod 32 arith.

#if 0
    $Align(Fullword)
#endif
    unsigned int snd_una;	// send unacknowledged seq #
    unsigned int snd_nxt;	// next send seq #
    unsigned int snd_wnd;	// send window
    unsigned int snd_wl;	// Last window update.
    signed int snd_bs;	// send buffer size
    unsigned int snd_up;	// urgent pointer
    unsigned int snd_max_wnd;	// Maximum Send Window
    unsigned int iss;	// Initial Send sequence number.
    unsigned int snd_pptr;	// Send PUSH pointer

#ifdef __x86_64__
    char dummy1[0] __attribute__((aligned(8)));
#endif

// Send circular queue variables

    CQF$DEF(snd_q_);			// Send data queue
    CQF$DEF(srx_q_);			// Retransmission data queue

// Receive Sequence variables

    unsigned int rcv_nxt;	// next expected sequence #
    unsigned int old_rcv_nxt;	// RCV.NXT last time ACK sent
    unsigned int rcv_wnd;	// window
    unsigned int old_rcv_wnd;	// RCV.WND last time ACK sent
    signed int rcv_bs;	// buffer size in bytes
    unsigned int rcv_up;	// Receive urgent pointer
    unsigned int irs;	// Initial receive sequence #
    unsigned int rcv_duptr;	// Last sequence # delivered to user
    unsigned int rcv_pptr;	// Receive PUSH pointer

// Receive circular queue variables

#ifdef __x86_64__
    char dummy2[0] __attribute__((aligned(8)));
#endif

    CQF$DEF(rcv_q_);			// Receive data queue

// Misc connection information

    void * ucb_adrs;	// UCB address for this TCB
    unsigned int vtcb_index;	// VALID_TCB index for this TCB
    unsigned int user_id;	// Owning process ID
    unsigned long long rx_timer;	// Time to do retransmit for this TCB
    unsigned long long rx_timeout;	// Retransmission time-out for TCB
    signed int rx_count;	// # of retransmissions done
    unsigned int rx_seq;	// Retransmission sequence start
    unsigned int rx_ctl;	// Control segment type
    unsigned long long xmit_start_time;	// Time first item queued on RX timer
  union {
    unsigned long long time_wait_timer;	// Time_Wait Counter.
    unsigned long long ack_timer;	// spontaneous ack timer.
  };
    unsigned long long delayed_ack_timer;	// Delayed ACK timer.
    unsigned long long squench_timer;	// Source Quench timer.
    unsigned int user_timeval;	// Amount of time before idle
    unsigned long long user_timeout;	// Time which connection will be closed
    unsigned long long probe_time;	// time when we need to probe again
    unsigned long long connection_timeout;	// Time which connection will be reset.
    unsigned long long inactive_timeout;	// time which conn will be deleted.
    unsigned long long function_timer;	// User function timeout.
    void * tvtdata;	// Pointer to TVT data block if IS_TVT
    void (* timeout_routine)();	// For TVT's, internal timeout routine
    void * argblk;	// Points at user's i/o request argblk.
    unsigned int round_trip_time;	// Time required for segment to be ACKed
    unsigned int calculated_rto;	// Calculated retransmission timeout
    unsigned char curr_user_function;	// Current user function being timed.
    unsigned char state;	// Current state of this connection
    unsigned char last_state;	// Last state of this connection.
    unsigned char con_index;	// Connection Table index for local port
    unsigned short int process_io_chan;	// IO channel for this connection.
    unsigned short int max_seg_data_size;	// max size data receiver will accept
    unsigned short int max_eff_data_size;	// max effective size data receiver will accept
    unsigned short int duplicate_segs;	// duplicate segments.
    unsigned short int oorw_segs;	// Out Of Recv Window segments.
    signed int inactive_code;	// Reason TCB was set inactive.
    unsigned int ack_size;	// # of data bytes to be ack'ed.
    unsigned int snd_ack_threshold;	// Number of bytes before window update
    unsigned long long snd_delay_timer;	// Time to wait before forcing send
  union {
    unsigned int tcb$flags;	// Reserve some space for flags
    struct {
	unsigned active_open : 1;	// Active open performed.
	unsigned open_nowait : 1;	// Wait-mode of OPEN in progress
	unsigned close_nowait : 1;	// Wait-mode of CLOSE in progress
	unsigned pending_close : 1;	// FIN segment needs to be sent.
	unsigned pending_ack : 1;	// ACK segment needs to be sent.
	unsigned pending_io : 1;	// ARGBLK pts at an user IO request.
	unsigned data_2_send : 1;	// User data needs to be sent.
	unsigned eof : 1;	// TCB Closing, All data delivered.
	unsigned is_aborted : 1;	// TCB is aborted - drop incoming data
	unsigned snd_push_flag : 1;	// Push pending on SEND
	unsigned rcv_push_flag : 1;	// Push seen on receive
	unsigned nmlook_flag : 1;	// Name or address lookup in progress
	unsigned is_synched : 1;	// OK to send ACKs, connection synched
	unsigned fin_rcvd : 1;	// Valid FIN received in FIN-WAIT-2
	unsigned is_tvt : 1;	// TCB is a TVT (virtual terminal)
	unsigned de_nagle : 1;	// Turn off Nagle algorithim
	unsigned squench : 1;	// Source Quench received
	unsigned nbwrite : 1;	// Network Buffer busy
	unsigned tcp_df : 1;	// Don't Fragment bit
    };
  };
    signed short int foreign_hnlen;	// Length of foreign host name
    unsigned char foreign_hname[MAX_HNAME];	// Foreign host name
};

// Set the size of one TCB

    #define TCB_SIZE sizeof(struct tcb_structure)		// in BLISS Fullwords.

//MESSAGE(%NUMBER(tcb_size),' longwords per tcb')

//SBTTL 'Connection Table definitions.'
/*

The connection table is the data structure used to identify unique network
connections.  Each element in the table consists of a TCB list (Queue Header)
& the local port number.  For every connection that has the same
local-port you will find the TCB (Transmission Control Blk) in the TCB list
for this local-port.  The primary reason behind this structure is to facilate
connection OPEN processing in the verification that a requested connection
pair (Foreign_Socket & Local_Socket) represent a unique connection.  We can't
have duplicates or ambiguities.  If the local_port field is (-1) then this
connection table entry is available.  As one might have guessed there is a
limit to the number of unique local-ports that are active at any given time.
I suspect as time flows the size of this table will have to be increased.
See the literals & macro definitions following.  The actual global data
declaration is in TCP.BLI.

*/


struct connection_table_structure {
    void * CN$TCB_List;	// This port's TCB list. Queue head prt.
    void * CN$TCB_Tail;	// Queue tail ptr.
    int CN$Local_Port;	// Local Port #.
};

    #define CONECT_TBL_SIZE 4		// # of blocks in connection blockvector
    #define INITIAL_TCBS 2			// Initial size of Valid TCB table
    #define CN$BLK_SIZE sizeof(struct connection_table_structure)	// Size in longs
    #define CN$BLK_BLEN 10			// Size in bytes

//ASSIGN(tblsize,%NUMBER(conect_tbl_size)*%NUMBER(cn$blk_size))
//MESSAGE(%NUMBER(tblsize),' Longwords allocated to Connection Table')


//SBTTL 'Definition of structure for WKS server process table & SYN-wait list'

struct  wks_structure {
    int WKS$Port;
    struct dsc$descriptor WKS$Process;		// ASCID descriptor.
    struct dsc$descriptor WKS$IMAGE;
      int WKS$Stat;
    struct dsc$descriptor WKS$Priv;
    void * WKS$Quotas;
    struct dsc$descriptor WKS$Input;		// ASCID descriptor.
    struct dsc$descriptor WKS$Output;		// ASCID descriptor.
    struct dsc$descriptor WKS$Error;		// ASCID descriptor.
    int WKS$Prior;
    int WKS$MaxSrv;
    int WKS$SYN_Qcount;
    void * WKS$SYN_Qhead;
  void * WKS$SYN_Qtail;
};

#define WKS_BLOCK_SIZE sizeof(struct  wks_structure)
#define WKS_TABLE_SIZE 20
