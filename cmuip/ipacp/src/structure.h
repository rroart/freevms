#ifndef structure_h
#define structure_h

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
//TITLE 'Define TCP-System wide Data Structures'
/*

Module:

	STRUCTURE

Facility:

	Provide system-wide data structure definitions.  This file
	generates a BLISS LIBRARY file "STRUCTURE.Lxx".  The structure
	library file is used by all other modules as the data structure
	definition file.  This method was adopted in order that all of
	the dynamic data-structures definitions could be located quickly.
	The only other place you will find data declarations (primarily
	global) is in the TCP.BLI module.

Author:

	Original author Stan C. Smith  Fall 1981
	This version by Vince Fuller, CMU-CSD, Spring/Summer, 1986
	Copyright (c) 1986, 1987, Vince Fuller and Carnegie-Mellon University

Modification History:

*** Begin CMU change log ***

	28-Feb-1990	Bruce R. Miller		CMU Network Development
	Removed much, esp. TCP/IP protocol definitions.

	02-Feb-1990	Bruce R. Miller		CMU Network Development
	Removed structure definition for the device configuration
	table.  It now resides in device_inof.req.

	14-Jun-1989	Bruce R. Miller		CMU Network Development
	Added field definitions for RR (resource record) lookup
	subfunction of the GTHST user function.

	24-OCT-1088	Dale Moore	CMu-CS/RI
	Changed Device name from "Pointer to a quadword" to
	"quadword".

5.6  20-Jul-87, Edit by VAF
	Clean up definitions of circular byte queues.

5.5  23-Mar-87, Edit by VAF
	Flush most of the constants describing buffer sizes as they are now
	mostly dynamic.
	Flush things from the NSQ block that don't belong there.

5.4   3-Mar-87, Edit by VAF
	Flush the I/O request "tags". Requires parallel change to IPDRIVER
	and to "VMS.REQ".

5.3   2-Mar-87, Edit by VAF
	Add definitions for UDP open modes (ADDR + DATA)
	Reorganize GTHST block definitions a little.

5.2  19-Feb-87, Edit by VAF
	Add hostname string length for OPEN and GTHST functions. Set by
	IPDRIVER versions 6.2 and later (previous versions incompatable).

5.1  23-Jan-87, Edit by VAF
	Clean up user request block definitions. General cosmetic changes.
	Add UARGSIZE field to UARG block for use by MACLIB. Set by device
	version 2.8 and later (previous versions incompatable).

5.0  12-Nov-86, Edit by VAF
	More changes as outlined in TCPMOD.REQ. Among other things, a lot of
	structures have been removed from here since they don't need to be
	global any more.

3.0   4-Aug-86, Edit by VAF
	Many, many changes to support changes outlined in TCP.MOD.

2.0  20-Feb-86, Edit by VAF
	Flush "known hosts".
	Add IP address and netmask fields to device configuration entries.
	Add IP gateway table (not fully implemented yet).
	Flush NSQ$ROUTE - consequence of "known hosts".
	Add NSQ$IP_Dest - IP address of immediate destination.
	Add MAX_HNAME - max length of hostname string

*** End CMU change log ***

[Sept 1981] 1.1
	Orginal definitions.
[Jan 1982] 1.2
	net$dump user return blk definitions
[Apr 1983] 1.3
	segment options field & tcb max-seg-size field
[Sept 1983] 1.4
	host alias table.
*/

  // not yet#include "cmuip/central/netxport.h"	// Get the transportablity library
#include "../../central/include/netcommon.h" // System-wide definitions

// Define some standard literals

// Misc Time Values in Hundredths seconds.

#define    TIMER_DELTA  -10*1000*1000	// conversion factor to VMS delta time.
#define    CSEC  100			// Convert seconds to hundredths.
#define    MINUTE  60*CSEC		// Convert minutes to .01 sec
#define    DAYSEC  24*60*60*CSEC	// One day
#define    CSEC_TIMER_DELTA  -100*1000 // Delta time format converter.

// Local Port ranges

#define    WELL_KNOWN_LP_START	 1
#define    WELL_KNOWN_LP_END	 255
#define    COMMON_LP_END	 100
#define    USER_LP_START	 1024	// missing range for bsd4.2 unix.
#define    USER_LP_END		 32767
#define    AOUS_LP_START	 32768// Active Open User specified Local Port
#define    AOUS_LP_END		 65535
#define    LP_START		 1
#define    LP_END		 65535

// Misc.

#define    ERROR  -1			// Return value, indicates error............
#define    NOT_FOUND  -1
#define    OK  0			// Valid TCB pointer: RTN, TCB_OK.
#define    WILD  0			// Indicates a wild-card in Foreign_Socket pair.

#define    EMPTY_QUEUE 3 // REMQUE: unsuccessful removal, queue empty.
#define    QUEUE_EMPTY_NOW 2 // REMQUE: OK removal & queue now empty.
#define    FIRST_QUEUE_ELEM 1 // INSQUE return - inserted 1st element.

#define    tblsize = 0;

//SBTTL 'Define circular byte queue header'
/*
    Circular byte queues are used to keep track of data as it flows between
    the user and the network.
*/

#define    CQF$DEF(N) \
	unsigned char N##queue[0];	/* First address of structure */\
	void * N##base;	/* Base address of queue*/\
	void * N##end;	/* Address of end of queue*/\
	unsigned short int N##size;	/* Size of queue*/\
	unsigned short int N##count;	/* Count of items on queue*/\
	void * N##enqp;	/* Pointer to last byte on queue*/\
	void * N##deqp;	/* Pointer to first byte on queue*/

struct cq_fields 
    {
    CQF$DEF(cq$)		// Define the queue fields
    };

#define    CQ_SIZE sizeof(struct cq_fields)

#define    cq$block struct cq_fields


//SBTTL 'Queue Element Block Definitions.'
/*
 Queue blks are used as queue elements which in turn contain bliss fullwords
used for other purposes.  Case: Send Queue consists of a queue of send Queue
Blks.  Each queue block then contains information & pointers to the user's
send data eg.

		//===============================================//
		+		Queue Forward Link		+
		//-----------------------------------------------//
		+		Queue Backwards link		+
		//-----------------------------------------------//
		+		User data size in bytes		+
		//-----------------------------------------------//
		+		User data pointer		+
		//-----------------------------------------------//
		+		Data Time Out in Secs.		+
		//===============================================//

*/

// Queue Block (Send queue) Fields

struct qb_send_fields
{
  void *     sn$next;		// Forward Queue Link (Flink).
  void *     sn$last;		// Backwards queue link (Blink).
  signed long int     sn$size;	// # of bytes to send.
  void *     sn$data ;		// Start of data buffer.
  void *     sn$uargs;		// Address of user arg blk.
  signed short int     sn$timeout;	// connection timeout (time).
  union {
    signed char     sn$flags;
    struct {
      unsigned 	sn$eol		 : 1;		// End Of Letter Boolean.
      unsigned 	sn$urg		 : 1;		// Urgent data boolean.
      unsigned 	sn$user		 : 1;	// On if seg is in user space
    };
  };
};

#define    QB_SN_SIZE sizeof(struct qb_send_fields)

// Queue Block (ReTransmission Queue Fields).

struct qb_rt_fields
{
  void *     rt$next;		// FLink.
  void *     rt$last;		// Blink.
  void *     rt$buf;		// segment buffer start adrs
  signed short int     rt$bufsize;	// byte size of buffer.
  signed short int     rt$segsize;	// Byte size of segment.
  void *     rt$seg;		// Start address of segment.
  signed int     rt$dest_adrs;		// Destination network adrs.
  signed long     rt$timeout;		// Retransmission timeout
  signed long     rt$maxtimeout;		// expiration time on RX queue
  signed long     rt$start_xmit_time;		// base for round trip calc.
  signed long     rt$xmit_count;		// # of transmissions.
  signed long     rt$seq_end;		// Last sequence # in segment
};

#define    QB_RT_SIZE sizeof(struct qb_rt_fields)

// Network Receive segment queue.

struct qb_nr_fields
{
  void *     nr$next;		// FLINK.
  void *     nr$last;		// BLink.
  signed long int     nr$buf_size;	// byte size of read buffer.
  void *     nr$buf;		// Read buffer start.
  signed long int     nr$size;	// Byte size of TCP segment.
  void *     nr$seg;		// start of TCP segment.
  signed long int     nr$data_size;	// # of data bytes available.
  void *     nr$uptr;		// pointer to new data
  signed long int     nr$ucount;	// count of new data
  signed long int     nr$timeout;
  signed int     nr$src_adrs;	// Internet address.
  signed int     nr$dest_adrs;	// Internet address.
  signed short     nr$src_port;		// SYN wait list: Source Port
  signed short     nr$dest_port;		// SYN wait list: Dest port.
  union {
    signed long     nr$seq_start;		// First usable seqence #
    struct { 			// Reuse for ICMP messages
      signed char 	nr$icm_type;		// ICMP msg type
      signed char 	nr$icm_code;		// ICMP code field
      signed short 	nr$icm_ex;		// ICMP extra data
    };  
    void * 	nr$fragptr;		// IP fragment flag/pointer
  };
  signed long     nr$seq_end;		// Segment's last sequence #.
  signed long     nr$seq_count;		// Count of usable sequence #s
  union {
    signed char     nr$flags;
    struct {
      unsigned 	nr$eol		 : 1;		// End Of Letter boolean
      unsigned 	nr$urg		 : 1;		// Urgent data boolean.
      unsigned 	nr$icmp		 : 1;	// Really an ICMP message
    };
  };
};

#define    QB_NR_SIZE sizeof(struct qb_nr_fields)

// User Receive Data Request queue

struct qb_ur_fields
{
  void *     ur$next;		// FLINK.
  void *     ur$last;		// Blink
  signed long int     ur$size;	// # of bytes requested by user.
  void *     ur$data;		// Start of data buffer.
  union {
    struct {
      void *     ur$irp_adrs;		// IO request adrs.
      void *     ur$ucb_adrs;		// Unit Control Blk adrs.
    };
    struct {
      int ( * 	ur$astadr)();		// AST address for internal conn
      void *	ur$astprm;		// AST param for internal conn
    };
  };
  void *     ur$uargs;		// Address of user arg blk.
};

#define    QB_UR_SIZE sizeof(struct qb_ur_fields)

#define qb_size 0
      // check $MAXLIT(QB_Size,QB_SN_SIZE,QB_RT_SIZE,QB_NR_SIZE,QB_UR_SIZE);

#define	queue_blk_structure(F) F
//MESSAGE(%NUMBER(qb_size),' Longwords allocated per Queue Block')

//SBTTL 'Network Segment Received Queue Definition.'
/*

Segment receive queue consists of Queue Blks which are structured according
to the QB_NR_Field description.  IP places segments on this queue after IP
has verified the datagram.
*/

struct si_fields
{
struct si_fields *     si_qhead;
struct si_fields *     si_qtail;
};

#define    SI_Qheader_Size sizeof(struct si_fields)


//SBTTL '"SYN" Wait Queue Definition.'

// "SYN" wait queue. List of "SYN" segments received waiting for a server
// process to do a "PASSIVE" OPEN.  Queue elements (blocks) are structured
// according to the QB_NR_Fields.

struct sw_fields
{
  void *     sw_qhead;
  void *     sw_qtail;
};


//SBTTL 'User (TCP) I/O request Argument Block definitions.'
/*

User Network I/O requests are fed to TCP via an ACP queue.  The queue
elements are VMS I/O request packets (IRP's) which point to a TCP argument
blocks.  These argument block are formatted according to the following field
definitions.


User default argument block structure.
user argument blocks have at least these arguments.

//===============================================================//
+			Data Start Address			+
//---------------------------------------------------------------//
+			Users Buffer Address			+
//---------------------------------------------------------------//
+		     VMS Dynamic Memory Block ID		+
//---------------------------------------------------------------//
+		     I/O Request Packet Address			+
//---------------------------------------------------------------//
+		     Unit Control Block Address			+
//---------------------------------------------------------------//
+			Process ID				+
//---------------------------------------------------------------//
+						| TCP Function	+
//===============================================================//

*/

#define Static_Fields(NM) \
void *     NM##$data_start;	/* Start of data within this block.*/\
void *     NM##$ubuf_adrs;	/* User buffer address.*/\
int NM$$fill; /* overlap bufiodef */\
signed long     NM##$vms_blk_id;	/* VMS system dynamic block ID.*/\
void *     NM##$irp_adrs;	/* I/O Request Packet Address.*/\
void *     NM##$ucb_adrs;	/* Unit Control Block Address.*/\
void *     NM##$pid;	/* Owning process ID.*/\
unsigned short     NM##$uargsize;	/* Size of this UARG block*/\
signed char     NM##$funct;	/* ACP Function code.*/\
signed char     NM##$protocol;/* ACP Protocol code.*/\
char NM##$b_data[0];

struct user_default_args
{
  Static_Fields(ud);			// Only the standard fields
};

#define    UD_SIZE sizeof(struct user_default_args)

// User Call: OPEN

struct user_open_args
{
  Static_Fields(op);			// Define the standard fields
  unsigned char    op$protohdrblk[0];
  unsigned int     op$src_host;
  unsigned int     op$dst_host;
  unsigned int     op$ext1;	// Protocol header extension info
  unsigned int     op$ext2;	// Protocol header extension info
  union {
    unsigned short     op$flags;	// Open flags
    struct {
      union {
	unsigned 	op$mode		 : 1;	// Open mode, 1=Active, 0=Passive
	unsigned 	op$active_open	 : 1;	// Alias for the above
      };
      // UDP: 1=ADDRMODE 
      unsigned 	op$nowait	 : 1;	// 1=No wait, 0=wait
      unsigned 	op$addr_flag	 : 1;	// 1=Address, 0=String (Foreign_Host)
    };
  };
  signed short     op$timeout;	// Inactivity timeout in seconds.
  signed short     op$piochan;	// user's IO channel
  unsigned short     op$foreign_hlen; 	// Length of foreign host name
  union {
    signed char    op$foreign_host[MAX_HNAME]; // ASCIZ FH name
    signed int      op$foreign_address;	// FH address if OP$ADDR_FLAG set
  };
};

#define    OPEN_ARG_LENGTH sizeof(struct user_open_args)
#define    Max_User_ArgBlk_Size OPEN_ARG_LENGTH	// used by Uarg rtns (MEMGR.BLI)

#define OP$BLK_SIZE sizeof(struct user_open_args)	// Size of open block
#define OP$ALL_SIZE  (OP$BLK_SIZE>IRP$C_LENGTH ? OP$BLK_SIZE : IRP$C_LENGTH)

#define     OP$MODE_PASSIVE   0	// OP$MODE value for TCP passive open
#define     OP$MODE_ACTIVE   1		// OP$MODE value for TCP active open (default)
#define     OP$MODE_UDPDATA   1	// OP$MODE value for UDP data open (default)
#define     OP$MODE_UDPADDR   0	// OP$MODE value for UDP address open


// User Call: SEND

struct user_send_args
{
  Static_Fields(se);			// Define the standard fields
  unsigned char    se$protohdrblk[0];
  unsigned int     se$src_host;
  unsigned int     se$dst_host;
  unsigned int     se$ext1;	// Protocol header extension info
  unsigned int     se$ext2;	// Protocol header extension info
  union {
    unsigned short     se$flags;
    struct {
      unsigned 	se$opm		 : 1;	// Used in "OPEN" processing.
      unsigned 	se$eol		 : 1;	// End Of Letter flag.
      unsigned 	se$urg		 : 1;	// Urgent data sent.
    };
  };
  signed short     se$timeout;	// Send timeout (obsolete)
  void *     se$local_conn_id;	// Connection ID
  signed short     se$buf_size;	// Send buffer size
  signed char    se$data;	// User data start
};

#define    SEND_ARG_LENGTH sizeof(struct user_send_args)
#define SE$BLK_SIZE (&((struct user_send_args *)0)->se$data)

// User Call: RECIEVE

struct user_recv_args
{
  Static_Fields(re);			// Define the standard fields
  unsigned char     re$protohdrblkp[0];
  unsigned int     re$src_host;	// Source Host
  unsigned int     re$dst_host;	// Destination Host
  unsigned int     re$ext1;	// Protocol header extension info
  unsigned int     re$ext2;	// Protocol header extension info
  unsigned short     re$flags;	// No flags defined
  signed short     re$timeout;	// Obsolete, unused
  void *     re$local_conn_id;
  void *     re$alt_io;	// Alternate IO entry point
  void *     re$ph_buff;	// P0 Proto hdr buff pntr
  signed short     re$buf_size;
  signed char    re$data;
};

#define    RECV_ARG_LENGTH sizeof(struct user_recv_args)
#define RE$BLK_SIZE (&((struct user_recv_args *)0)->re$data)

// User Call: CLOSE

struct user_close_args
{
  Static_Fields(cl);			// Define the standard fields
  union {
    unsigned short     cl$flags;
    struct {
      unsigned 	cl$abort	 : 1;	// 1=Abort
      unsigned 	cl$nowait	 : 1;	// 1=No wait, 0=wait
    };
  };
  signed short     cl$timeout;	// Not used.
  void *    cl$local_conn_id;	// Connection ID
};

#define    CLOSE_ARG_LENGTH sizeof(struct user_close_args)
#define CL$BLK_SIZE CLOSE_ARG_LENGTH	// Size of close block
#define CL$ALL_SIZE IRP$C_LENGTH

// User Call: STATUS

struct user_status_args
{
  Static_Fields(st);			// Define the standard fields
  unsigned short     st$flags;	// Not used.
  signed short     st$timeout;	// Not used.
  void *     st$local_conn_id;	// Connection ID
  signed short    st$buf_size;
};

#define    STATUS_ARG_LENGTH sizeof(struct user_status_args)
#define ST$BLK_SIZE STATUS_ARG_LENGTH

// User Status call status-return blk

struct status_return_arg_blk
{
  signed short     sr$bytes_avail;
  signed char     sr$state;
  signed char     sr$last_state;
  signed long int     sr$send_window;
  signed long int     sr$recv_window;
  signed long int     sr$user_id;
};

#define    SR_BLK_SIZE sizeof(struct status_return_arg_blk)

// User Call: Net$INFO

struct user_info_args
{
  Static_Fields(if);			// Define the standard fields
  unsigned short     if$flags;	// Not used.
  signed short     if$timeout;	// Not used.
  void *     if$local_conn_id;	// Connection ID
  signed short     if$buf_size;// Size of returned data buffer
};

#define    INFO_ARG_LENGTH sizeof(struct user_info_args)

// User Call: ABORT

struct user_abort_args
{
  Static_Fields(ab);			// Define the standard fields
  unsigned short     ab$flags;	// None defined
  signed short     ab$timeout;	// Not used.
  void *    ab$local_conn_id;
};

#define    ABORT_ARG_LENGTH sizeof(struct user_abort_args)

// User call: GTHST

#define    GTHST_Static(NM) \
unsigned short 	NM##$flags;	/* Function flags */\
signed short 	NM##$subfunct;	/* GTHST subfunction*/\
signed short 	NM##$bufsize;	/* User buffer size*/\
union { \
unsigned short 	NM##$arg1;	/* Size of argument*/\
unsigned short	NM##$rrtype;	/* only for grr */\
}; \
union { \
unsigned short 	NM##$arg2size;	/* Size of argument*/\
 unsigned short  NM##$hstlen;	\
}

struct gthst_args 
{
  Static_Fields(gh);			// Define the standard fields
  GTHST_Static(gh);			// Define the standard fields
};

#define    GTHST_ARG_LENGTH sizeof(struct U$GTHST_FIELDS)

// Overlays for the various GTHST subfunctions

// Name to address function

struct gthst_nmlook_args 
{
  Static_Fields(ghn);			// Define the standard fields
  GTHST_Static(ghn);			// and the standard GTHST fields
  char ghn$hstnam[HOST_NAME_MAX_SIZE];
};

#define    GTHST_NMLOOK_ARGS_LENGTH sizeof(struct gthst_nmlook_args)

// Address to name subfunction

struct gthst_adlook_args 
{
  Static_Fields(gha);			// Define the standard fields
  GTHST_Static(gha);			// and the standard GTHST fields
  signed int    gha$ipaddr;
};

#define    GTHST_ADLOOK_ARGS_LENGTH sizeof(struct gthst_adlook_args)

// Name to RR function

struct gthst_rrlook_args 
{
  Static_Fields(grr);			// Define the standard fields
  GTHST_Static(grr);			// and the standard GTHST fields
  char grr$hstnam[HOST_NAME_MAX_SIZE];
};

#define    GTHST_RRLOOK_ARGS_LENGTH sizeof(struct GTHST_RRLOOK_Fields)

// Maintenance Call : DUMP

struct debug_dump_args
{
  Static_Fields(du);			// Define the standard fields
  unsigned short     du$dump_directive;	// Dump function code.
  signed short     du$timeout;	// Not used.
  union {
    unsigned long     du$arg0;	// Argument #0
    void * 	du$local_conn_id;	// TCB/UDPCB index, DU$TCB_DUMP
    signed char 	du$device_idx;	// device index, DU$DEVICE_DUMP
    signed short 	du$start_index;	// ARP cache index, DU$ARP_CACHE
  };
  unsigned long     du$arg1;	// Argument #1
  unsigned long     du$arg2;	// Argument #2
  signed short     du$buf_size;	// Size of returned data block
  signed char     du$data;	// Start of return data buffer.
};

#define    Dump_Arg_Length sizeof(struct M$DUMP_Fields)
#define DU$BLK_SIZE (&((struct debug_dump_args *)0)->du$data)
#define DU$MAX_SIZE (DU$BLK_SIZE+8192)	// Maximum buffer size
	
// Maintenance Call : SNMP

struct snmp_args
{
  Static_Fields(snmp);			// Define the standard fields
  unsigned short     snmp$function;	// SNMP sub-function code.
  signed short     snmp$misc;	// Size of returned data block
  signed short     snmp$wbuf_size;	// Size of VAR data block
  signed short     snmp$rbuf_size;	// Size of returned data block
  signed char      snmp$data;	// Start of UArg data buffer.
};

#define    SNMP_Arg_Length sizeof(struct M$SNMP_Fields)
	
// Maintenance Call: EXIT
 
struct debug_exit_args
{
  Static_Fields(ex);			// Define the standard fields
  unsigned short     ex$flags;	// Not used.
  signed short     ex$timeout;	// Not used.
  void *     ex$local_conn_id;	// Not used.
};

#define    Exit_arg_Length sizeof(struct M$EXIT_Fields)

// Maintenance Call: PANIC

struct debug_panic_args
{
  Static_Fields(pa);			// Define the standard fields
  unsigned short     pa$flags;	// Not Used.
  signed short     pa$timeout;	// Not used.
  void *     pa$local_conn_id;	// Not used.
};

#define    Panic_arg_Length sizeof(struct M$PANIC_Fields)

// Maintenance Call: DEBUG

struct debug_args
{
  Static_Fields(de);			// Define the standard fields
  unsigned long     de$level;	// Debug level mask
  unsigned long     de$local_conn_id;	// Connection-ID (NYI)
  unsigned long     de$group;// glag cluster (DEBUG or ACTIVITY)
};

#define    Debug_Arg_Length sizeof(struct M$DEBUG_Fields)

// Maintenance Call: EVENT

struct event_args
{
  Static_Fields(ev);			// Define the standard fields
  signed short     ev$buf_size;	// Event buffer size
  signed char ev$data;	// User data start
};

#define    Event_Arg_Length sizeof(struct M$EVENT_Fields)
#define EV$BLK_SIZE (&((struct event_args *)0)->ev$data)

// VMS cancel IO call to TCPACP.  This argument block is generated by the
// User_requests_avail rtn(module:maclib.bli) in response to the virtual device
// driver "TCH" sending a fake IRP (IO request packet) to the TCPACP.  Only
// item that are of interest/valid are the user's PID & the Process IO cahnnel
// for which the IO is being canceled.  VMS cancels IO on both the $CANCEL
// system service & when the user deasssigns ($DEASGN) the IO channel.

struct vms$cancel_args
{
  Static_Fields(vc);		// Define the standard fields
  signed short     vc$piochan;	// Process IO channel (from IRP)
  signed long    vc$conn_id;	// Connection ID
};

#define    Cancel_Arg_Length sizeof(struct M$VMS_Cancel_Fields)

// End: Structure.Def

#endif
