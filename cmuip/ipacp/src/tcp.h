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

#include "netxport.h"
#include "netcommon.h"
#include "netvms.h"
#include "structure.h"


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

    #define Max_Seg_LifeTime 2*Minute	// seconds.
    #define Min_RT_TimeOut 1*Csec	// Min allowable retransmission time
    #define Max_RT_TimeOut 4*Minute	// Max allowable Retransmission time.
    #define Base_RT_TimeOut 3*CSEC	// Base (initial) Round Trip timer.
    #define PROBE_IVAL Minute	// Inactivity probe timer
    #define ack_interval Minute	// spontaneous ack interval timer.
    #define Delayed_ACK_Interval 20	// Delayed ACK interval, 200ms
    #define SQUENCH_Interval 2*CSEC	// Source Quench Interval, 2 Seconds
    #define Function_TimeOut 10*Csec	// User function.
    #define Active_Open_Timeout 30*Csec// Amount of time for active open to happen
    #define Passive_Open_Timeout DaySec // Amount of time for passive open
    #define NameLook_Timeout 2*Minute// How long to wait for name lookup
    #define Close_Timeout 2*Minute	// How long to wait for connection to close
    #define CONN_TIMEOUT 10*Minute	// How long before connection is dead
    #define RX_TIMEVAL 4*Minute	// Retransmit queue time limit
    #define Inactive_TCB_TimeOut 5*Csec // Time which a TCB will remain inactive
				// before being deleted.
    #define Default_Send_timeOut 300*Csec// Time allowed for a user's buffer
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
    void * LP_Next;	// LP queue Forward Link pointer.
    void * LP_Back;	// LP queue, Backwards Link.
    unsigned int Foreign_Host;	// Foreign host address
    unsigned long Foreign_Port;	// Foreign port
    unsigned int Local_Host;	// Local address of connection
    unsigned long Local_Port;	// Local Port #
    void * RF_Qhead;	// Network: received "future" seg queue
    void * RF_Qtail;
    signed long RF_Qcount;	// Count of items on future queue
    void * UR_QHead;	// User: receive Data request queue.
    void * UR_QTail;
    void * SND_QHead;	// User: send Segment request queue
    void * SND_QTail;

//
// Security		= [$SWord],	// not implememted.
// Precedance		= [$SWord],	// not implememted.

// Send sequence number variables
// Bytes(4) gives 32-Bit unsigned integer for mod 32 arith.

#if 0
    $Align(Fullword)
#endif
    unsigned long SND_UNA;	// send unacknowledged seq #
    unsigned long SND_NXT;	// next send seq #
    unsigned long SND_WND;	// send window
    unsigned long SND_WL;	// Last window update.
    signed long SND_BS;	// send buffer size
    unsigned long SND_UP;	// urgent pointer
    unsigned long SND_MAX_WND;	// Maximum Send Window
    unsigned long ISS;	// Initial Send sequence number.
    unsigned long SND_Pptr;	// Send PUSH pointer

// Send circular queue variables

    CQF$DEF(SND_Q_);			// Send data queue
    CQF$DEF(SRX_Q_);			// Retransmission data queue

// Receive Sequence variables

    unsigned long RCV_NXT;	// next expected sequence #
    unsigned long Old_RCV_NXT;	// RCV.NXT last time ACK sent
    unsigned long RCV_WND;	// window
    unsigned long Old_RCV_WND;	// RCV.WND last time ACK sent
    signed long RCV_BS;	// buffer size in bytes
    unsigned long RCV_UP;	// Receive urgent pointer
    unsigned long IRS;	// Initial receive sequence #
    unsigned long RCV_DUptr;	// Last sequence # delivered to user
    unsigned long RCV_Pptr;	// Receive PUSH pointer

// Receive circular queue variables

    CQF$DEF(RCV_Q_);			// Receive data queue

// Misc connection information

    void * UCB_ADRS;	// UCB address for this TCB
    unsigned long VTCB_INDEX;	// VALID_TCB index for this TCB
    unsigned long User_ID;	// Owning process ID
    unsigned long RX_Timer;	// Time to do retransmit for this TCB
    unsigned long RX_Timeout;	// Retransmission time-out for TCB
    signed long RX_Count;	// # of retransmissions done
    unsigned long RX_SEQ;	// Retransmission sequence start
    unsigned long RX_CTL;	// Control segment type
    unsigned long Xmit_Start_Time;	// Time first item queued on RX timer
    unsigned long Time_Wait_Timer;	// Time_Wait Counter.
//    $OVERLAY(time_wait_timer)
    unsigned long ACK_Timer;	// spontaneous ack timer.
//    $CONTINUE
    unsigned long Delayed_ACK_Timer;	// Delayed ACK timer.
    unsigned long SQUENCH_Timer;	// Source Quench timer.
    unsigned long User_Timeval;	// Amount of time before idle
    unsigned long User_Timeout;	// Time which connection will be closed
    unsigned long Probe_time;	// time when we need to probe again
    unsigned long Connection_TimeOut;	// Time which connection will be reset.
    unsigned long Inactive_Timeout;	// time which conn will be deleted.
    unsigned long Function_Timer;	// User function timeout.
    void * TVTDATA;	// Pointer to TVT data block if IS_TVT
    void * Timeout_Routine;	// For TVT's, internal timeout routine
    void * ArgBlk;	// Points at user's i/o request argblk.
    unsigned long Round_Trip_Time;	// Time required for segment to be ACKed
    unsigned long Calculated_RTO;	// Calculated retransmission timeout
    unsigned char Curr_User_Function;	// Current user function being timed.
    unsigned char State;	// Current state of this connection
    unsigned char Last_State;	// Last state of this connection.
    unsigned char Con_Index;	// Connection Table index for local port
    unsigned short int Process_IO_Chan;	// IO channel for this connection.
    unsigned short int Max_seg_data_size;	// max size data receiver will accept
    unsigned short int Max_eff_data_size;	// max effective size data receiver will accept
    unsigned short int Duplicate_segs;	// duplicate segments.
    unsigned short int OORW_segs;	// Out Of Recv Window segments.
    signed long Inactive_Code;	// Reason TCB was set inactive.
    unsigned long ACK_size;	// # of data bytes to be ack'ed.
    unsigned long SND_ACK_Threshold;	// Number of bytes before window update
    unsigned long SND_Delay_Timer;	// Time to wait before forcing send
  union {
    unsigned long TCB$Flags;	// Reserve some space for flags
    struct {
	unsigned Active_Open : 1;	// Active open performed.
	unsigned Open_NoWait : 1;	// Wait-mode of OPEN in progress
	unsigned Close_NoWait : 1;	// Wait-mode of CLOSE in progress
	unsigned Pending_Close : 1;	// FIN segment needs to be sent.
	unsigned Pending_ACK : 1;	// ACK segment needs to be sent.
	unsigned Pending_IO : 1;	// ARGBLK pts at an user IO request.
	unsigned Data_2_Send : 1;	// User data needs to be sent.
	unsigned EOF : 1;	// TCB Closing, All data delivered.
	unsigned Is_Aborted : 1;	// TCB is aborted - drop incoming data
	unsigned SND_Push_Flag : 1;	// Push pending on SEND
	unsigned RCV_Push_Flag : 1;	// Push seen on receive
	unsigned NMLook_Flag : 1;	// Name or address lookup in progress
	unsigned IS_Synched : 1;	// OK to send ACKs, connection synched
	unsigned FIN_RCVD : 1;	// Valid FIN received in FIN-WAIT-2
	unsigned IS_TVT : 1;	// TCB is a TVT (virtual terminal)
	unsigned DE_Nagle : 1;	// Turn off Nagle algorithim
	unsigned SQUENCH : 1;	// Source Quench received
	unsigned NBWRITE : 1;	// Network Buffer busy
	unsigned TCP_DF : 1;	// Don't Fragment bit
    };
  };
    signed short int Foreign_Hnlen;	// Length of foreign host name
    unsigned char Foreign_Hname[MAX_HNAME];	// Foreign host name
};

// Set the size of one TCB

    #define TCB_Size sizeof(struct tcb_structure);		// in BLISS Fullwords.

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


struct CN$Fields {
    void * CN$TCB_List;	// This port's TCB list. Queue head prt.
    void * CN$TCB_Tail;	// Queue tail ptr.
    long CN$Local_Port;	// Local Port #.
};

    #define Conect_Tbl_Size 4,		// # of blocks in connection blockvector
    #define Initial_TCBs 2,			// Initial size of Valid TCB table
    #define CN$Blk_Size sizeof(struct CN$Fields),	// Size in longs
    #define CN$Blk_BLen 10;			// Size in bytes

//ASSIGN(tblsize,%NUMBER(conect_tbl_size)*%NUMBER(cn$blk_size))
//MESSAGE(%NUMBER(tblsize),' Longwords allocated to Connection Table')


//SBTTL 'Definition of structure for WKS server process table & SYN-wait list'

struct  wk_server_structure {
    long int WKS$Port;
    unsigned long long WKS$Process;		// ASCID descriptor.
    unsigned long long WKS$IMAGE;
      long int WKS$Stat;
    unsigned long long WKS$Priv;
    void * WKS$Quotas;
    unsigned long long WKS$Input;		// ASCID descriptor.
    unsigned long long WKS$Output;		// ASCID descriptor.
    unsigned long long WKS$Error;		// ASCID descriptor.
    long int WKS$Prior;
    long int WKS$MaxSrv;
    long int WKS$SYN_Qcount;
    void * WKS$SYN_Qhead;
  void * WKS$SYN_Qtail;
};

#define WKS_Block_Size sizeof(struct  wk_server_structure)
    #define WKS_Table_Size 20
