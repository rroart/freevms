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
/*

Module:

	NETCOMMON

Facility:

	Provide system-wide data structure and literal definitions.
	This file is intended to contain definitions which are shared
	by the IPACP, clients, and possibly by NAMRES.

Author:

	Much originally from STRUCTURE.REQ
	This version by Bruce R. Miller, CMU NetDev, 10am 2/22/90
	Copyright (c) 1990, Bruce Miller and CMU.

Modification History:

*/

/* LIBRARY 'CMUIP_SRC:[CENTRAL]NETCONFIG'; */

/* Miscellaneous stuff to make life easier. */

#ifndef _CMU_TYPES
#define _CMU_TYPES

typedef union {
    struct {short int w1,w2,w3,w4; } w;
    struct {int l1,l2; } l;
    } quadword;


#endif _CMU_TYPES

#define word short
#define uword unsigned short	
#define ushort unsigned short
#define ulong unsigned long
#define uchar unsigned char

typedef union {
   unsigned long l;
   } in_addr;
	

/* Name length and address count literals */

#define Host_Name_Max_Size	128
#define MAX_HNAME		Host_Name_Max_Size
#define MAX_HADDRS		20
/*!!!HACK!!! Get rid of this!!!*/
#define MAX_RRSIZE		1024;

#define MAX_UDPCB	100		/* Max number of UDP connections */
#define MAX_ICMPCB	 10		/* Max number of ICMP connections */
#define MAX_IPCB	 10		/* Max number of IP connections */

/* Define protocol codes for open */
#define U$TCP_Protocol	0		/* TCP protocol (default) */
#define U$UDP_Protocol	1		/* UDP protocol */
#define U$ICMP_Protocol	2		/* ICMP protocol */
#define U$IP_Protocol	3		/* IP protocol */



/*
    Net I/O Status block (Quadword) field definitions. These are basically the
    same as defined in VMS with the exception of the 2nd longword. The second
    longword contains the ACP error code, if status is SS$_ABORT, or request
    flags and status information if status is SS$_NORMAL. Note that the ICMP
    code field is only defined if the ICMP flag is set, and is only valid for
    special connections which know about ICMP messages.

	!===============================================================!
	+     Bytes Transfered		!	VMS Return Code		+
	!---------------------------------------------------------------!
	+	    Unused		!     Flags	!   ICMP code	+
	!===============================================================!

*/

typedef struct {
    uword NSB$STATUS;		/* VMS status return code */
    uword NSB$Byte_Count;
    union {
	ulong STATUS;	/* Second status word */
	struct {
	    uchar NSB$ICMP_CODE;	/* ICMP mess. code, if ICMP flag set*/
	    union {
		uchar NSB$FLAGS;		/*  Request flags */
		struct {
		    uchar NSB$F_URG:1;	/* URGENT data bit (not implemented) */
		    uchar NSB$F_PUSH:1;	/* PUSH data bit */
		    uchar NSB$F_EOF:1;	/* EOF (i.e. connect. fully closed) */
		    uchar NSB$F_ICMP:1;	/* ICMP message bit */
		    };
		} flags;
	    };
	ulong ERROR;	/* ACP error code, if RC = SS$_ABORT */
	} X;
    } NetIO_Status_Block;
#define NSB$XERROR X.ERROR

#define NSB$URGBIT   1		/* Bit value for URGENT bit in FLAGS field */
#define NSB$PUSHBIT  2		/* Bit value for PUSH bit */
#define NSB$EOFBIT   4		/* Bit value for EOF bit */
#define NSB$ICMPBIT  8		/* Bit value for ICMP bit */



/* %SBTTL 'IP protocols header information block definition.' */

/*
  This is the definition of the block of data describing UDP ICMP and IP
  header information when using the send and receive commands. 

 Special adress block for send/receive in non-TCP mode.
*/

/* typedef struct {
 *    ulong IPADR$SRC_HOST;
 *    ulong IPADR$DST_HOST;
 *
 *    union {
 *	uchar IPADR$EXTENSION[8];
 *	struct {
 *	    ulong IPADR$EXT1;
 *	    ulong IPADR$EXT2;
 *	    };
 *	struct {
 *	    uword IPADR$SRC_PORT;
 *	    uword IPADR$DST_PORT;
 *	    };
 *	struct {
 *	    uchar IPADR$TYPE;
 *	    uchar IPADR$CODE;
 *	    uword IPADR$CKSUM;
 *	    ulong IPADR$SPECIAL;
 *	    };
 *	};
 *    } IPADR$ADDRESS_BLOCK;
 */

typedef struct {
    ulong IPADR$SRC_HOST;
    ulong IPADR$DST_HOST;

    union {
	uchar IPADR$EXTENSION[8];
	struct {
	    ulong P1;
	    ulong P2;
	    } EXT;
	struct {
	    uword SRC;
	    uword DST;
	    } PORT;
	struct {
	    uchar TYPE;
	    uchar CODE;
	    uword CKSUM;
	    ulong SPECIAL;
	    } ICMP;
	} U ;
    } IPADR$ADDRESS_BLOCK;

#define IPADR$EXT1	U.EXT.P1
#define IPADR$EXT2	U.EXT.P2
#define IPADR$SRC_PORT	U.PORT.SRC
#define IPADR$DST_PORT	U.PORT.DST
#define IPADR$TYPE	U.ICMP.TYPE
#define IPADR$CODE	U.ICMP.CODE
#define IPADR$CKSUM	U.ICMP.CKSUM
#define IPADR$SPECIAL	U.ICMP.SPECIAL

/* NET$INFO - return block structure. */

typedef struct {
    uchar CI$FHost_Name_size;
    uchar CI$LHost_Name_size;
    uchar CI$Foreign_Host[Host_Name_Max_Size];
    short fill;
    ulong CI$Foreign_Port;
/*    short fill_1; 	mlo 17-SEP-1993 15:18:57 not needed */
    uchar CI$Local_Host[Host_Name_Max_Size];
    ulong CI$Local_Port;
/*    short fill_2;	mlo 17-SEP-1993 15:20:20 not needed */
    ulong CI$local_internet_adrs;
    ulong CI$remote_internet_adrs;
    } Connection_Info_Return_Block;



/* NET$GTHST - Return block for Name to Address translation */

typedef struct {
    ulong GHA$NL_ADRCNT;		/* Count of addresses */
    ulong GHA$NL_ADRLST[MAX_HADDRS];	/* Address list */
    ulong GHA$NL_NAMLEN;		/* Official name length */
    char	  GHA$NL_NAMSTR[MAX_HNAME];	/* Official name string */
    } GTHST_NMLOOK_BLOCK;


/* NET$GTHST - Return block for Address to Name translation */

typedef struct {
    ulong GHN$NAMLEN;		/* Host name string length */
    char	  GHN$NAMSTR[MAX_HNAME];
    } GTHST_ADLOOK_BLOCK;

/* NET$GTHST - Return block for Name to RR translation */

typedef struct {
    uword GRR$RL_RDLEN;		/* Resource data length */
    char  GRR$RL_RDATA[];	/* RData followed by name. */
    } GTHST_RRLOOK_BLOCK;



/* Dump Directive Codes. */

#define DU$Dynamic_Mem_Alloc	 1 /* Memory management counters */
#define DU$TCP_Stats		 2 /* TCP statistic counters */
#define DU$Local_Connection_ID	 3 /* List of active TCP connections */
#define DU$TCB_Dump		 4 /* Contents of a TCB */
#define DU$Host_stats		 5 /* obsolete, unused */
#define DU$Known_Host_Index	 6 /* obsolete, unused */
#define DU$device_dump		 7 /* Device-dependant status */
#define DU$UDP_Connections	 8 /* List of UDP connections */
#define DU$UDPCB_Dump		 9 /* Contents of a UDPCB */
#define DU$ARP_Cache		10 /* Contents of ARP cache */
#define DU$ICMP_Connections	11 /* List of ICMP connections */
#define DU$ICMPCB_Dump		12 /* Contents of a IPCB */
#define DU$IP_Connections	13 /* List of IP connections */
#define DU$IPCB_Dump		14 /* Contents of a IPCB */
#define DU$Device_List		15 /* List of device indexes */
#define DU$Device_Stat		16 /* Contents of dev_config entry */

/* Dump return argument block formats. */

/* Dump Dynamic Memory allocation stats. */
/* Dump Directive: DU$Dynamic_mem_Alloc  */

typedef struct {
    long DM$QB;			/* Queue Blocks */
    long DM$UA;			/* UARG Blocks */
    long DM$CS;			/* TCP Control segments */
    long DM$DMS;		/* default size max segs */
    long DM$NM;			/* net mesg receive buffer */
    long DM$QBMX;		/* Queue blocks max */
    long DM$UAMX;		/* UARG Blocks max */
    long DM$CSMX;		/* TCP Control segments max */
    long DM$DMSMX;		/* default size max segs max */
    long DM$NMMX;		/* net mesg receive buffer max */
    long DM$QBAL;		/* qblks allocated */
    long DM$UAAL;		/* Uarg blks allocated */
    long DM$CSAL;		/* Control segs allocated */
    long DM$DMSAL;		/* Default segs allocated */
    long DM$NMAL;		/* net recv bufs allocated */
    long DM$QBFR;		/* QBlks free */
    long DM$UAFR;		/* Uarg blks free */
    long DM$CSFR;		/* Control segs free */
    long DM$DMSFR;		/* Default segs free */
    long DM$NMFR;		/* net recv bufs free */
    } D$Mem_Alloc_Return_blk;

/* User return dump blk:  TCP stats. */

typedef struct {
    long DM$TCPACP_PID;
    long DM$User_IO_Requests;
    long DM$StoreForward;
    long DM$Active_Conects_Opened;
    long DM$Passive_Conects_Opened;
    long DM$Data_Bytes_Xmitted;
    long DM$Data_Bytes_Recved;
    long DM$Segs_Xmitted;
    long DM$Segs_Recved;
    long DM$Seg_Bad_Chksum;
    long DM$BadSeq;
    long DM$Duplicate_Segs;
    long DM$Retrans_Segs;
    long DM$RPZ_RXQ;
    long DM$OORW_Segs;
    long DM$Future_Rcvd;
    long DM$Future_Used;
    long DM$Future_Dropped;
    long DM$Future_Dups;
    long DM$Servers_Forked;
    long DM$Uptime[2];
    long DM$Arps_Received;
    } D$TCP_Stats_Return_Blk;

/* Dump all known local-connection-ids. */

/*
!!!HACK!!! OH MY GOD WHAT AM I DOING! I'VE HARD-WIRED IN 100 INSTEAD OF
!!!HACK!!! MAX_TCB.  DON'T LET THIS CODE GO OUT THE DOOR!!!!!
!!!HACK!!! remember to improve the dumping interface...
*/

#define D$LC_ID_Blksize 100*4 /* byte size. */
typedef
    ulong D$LC_ID_Return_Blk[D$LC_ID_Blksize/4];

/* Dump a specified TCB */

typedef struct {
    long DM$TCB_Addr;		/* Address of TCB (internal) */
    long DM$User_ID;		/* Owning process ID */
    word DM$State;		/* Current TCB state */
    word DM$Last_State;		/* */
    in_addr DM$Foreign_Host;	/* Foreign host # */
    long DM$Foreign_Port;	/* Foreign port # */
    in_addr DM$Local_Host;	/* Local host # */
    long DM$Local_Port;		/* Local port # */
    long DM$User_Recv_QE;	/* # of pending user receives */
    long DM$User_Send_QE;	/* # of pending user sends */
    long DM$Net_Recv_QE;	/* # of pending receive buffers */
    long DM$Future_QE;		/* # of buffers on future queue */
    long DM$Rexmit_QE;		/* # of bytes on RX queue */
    long DM$Dasm_ptr;		/* Disassembly pointer */
    long DM$Dasm_bc;		/* Dissasembly byte count */
    long DM$Asm_ptr;		/* Assembly pointer */
    long DM$Asm_bc;		/* Assembly byte count */
    long DM$ISS;			/* Initial Send Sequence # */
    long DM$SND_UNA;		/* # of bytes send but not ACK'ed */
    long DM$SND_NXT;		/* Next Send Sequence # */
    long DM$SND_WND;		/* Send window count */
    long DM$SND_WL;		/*    DM$SND_BS		= [$SLong], */
    long DM$IRS;		/* Initial Receive Sequence # */
    long DM$RCV_NXT;		/* Next Receive Sequence # */
    long DM$RCV_WND;		/* Receive window count */
    long DM$RCV_BS;		/* */
    long DM$Conn_TimeOut;	/* Connection dead timeout */
    long DM$RT_TimeOut;		/* Retransmission timer */
    long DM$Round_Trip_Time;	/* Measured round-trip-time */
    } D$TCB_Dump_Return_Blk;

/* Dump list of udpCB's */

#define D$UDP_List_BlkSize	MAX_UDPCB*4	/* byte size. */
typedef
    ulong D$UDP_List_Return_Blk[D$UDP_List_BlkSize/4];

/* Dump of a single UDPCB */

typedef struct {
    char    *DU$UDPCB_Address;		/* Internal UDPCB address */
    ulong   DU$UDPCB_Foreign_Host;	/* Foreign host number */
    uword   DU$UDPCB_Foreign_Port;	/* Foreign port number */
    in_addr DU$UDPCB_Local_Host;	/* Local host number */
    uword   DU$UDPCB_Local_Port;	/* Local port number */
    long    DU$UDPCB_NR_Qcount;		/* Count of items on receive queue */
    long    DU$UDPCB_UR_Qcount;		/* Count of items on user queue */
    union {
	uword   DU$UDPCB_Flags;		/* UDPCB flags */
	struct {
	    uword DU$UDPCB_Wildcard:1;	/* Wildcard foreign host/port */
	    uword DU$UDPCB_Raw_Mode:1;	/* User wants IP headers (not impl) */
	    uword DU$UDPCB_Aborting:1;	/* Connection is closing */
	    uword DU$UDPCB_NMlook:1;	/* Name lookup in progress */
	    uword DU$UDPCB_Internal:1;	/* Connection is internal */
	    };
	};
    ulong DU$UDPCB_User_ID;		/* Owning process */
    } D$UDPCB_Dump_Return_Blk;

/* Dump list of ICMPCB's */

#define D$ICMP_List_BlkSize	MAX_ICMPCB*4	/* byte size. */
typedef
    ulong D$ICMP_List_Return_Blk[D$ICMP_List_BlkSize/4];

/* Dump of a single ICMPCB */

typedef struct {
    char    *DU$ICMPCB_Address;		/* Internal ICMPCB address */
    in_addr DU$ICMPCB_Foreign_Host;	/* Foreign host number */
    in_addr DU$ICMPCB_Local_Host;	/* Local host number */
    long    DU$ICMPCB_NR_Qcount;	/* Count of items on receive queue */
    long    DU$ICMPCB_UR_Qcount;	/* Count of items on user queue */
    union {
	uword   DU$ICMPCB_Flags;		/* ICMPCB flags */
	struct {
	    uword DU$ICMPCB_Wildcard:1;	/* Wildcard foreign host/port */
	    uword DU$ICMPCB_Raw_Mode:1;	/* User wants IP headers (not impl) */
	    uword DU$ICMPCB_Aborting:1;	/* Connection is closing */
	    uword DU$ICMPCB_NMlook:1;	/* Name lookup in progress */
	    uword DU$ICMPCB_Internal:1;	/* Connection is internal */
	    };
	};
    ulong DU$ICMPCB_User_ID;		/* Owning process */
    } D$ICMPCB_Dump_Return_Blk;

/* Dump list of IPCB's */

#define D$IP_List_BlkSize	MAX_IPCB*4	/* byte size. */
typedef
    ulong D$IP_List_Return_Blk[D$IP_List_BlkSize/4];

/* Dump of a single IPCB */

typedef struct {
    char    *DU$IPCB_Address;		/* Internal IPCB address */
    in_addr DU$IPCB_Foreign_Host;	/* Foreign host number */
    in_addr DU$IPCB_Local_Host;	/* Local host number */
    long    DU$IPCB_NR_Qcount;	/* Count of items on receive queue */
    long    DU$IPCB_UR_Qcount;	/* Count of items on user queue */
    union {
	uword   DU$IPCB_Flags;		/* IPCB flags */
	struct {
	    uword DU$IPCB_Wildcard:1;	/* Wildcard foreign host/port */
	    uword DU$IPCB_Raw_Mode:1;	/* User wants IP headers (not impl) */
	    uword DU$IPCB_Aborting:1;	/* Connection is closing */
	    uword DU$IPCB_NMlook:1;	/* Name lookup in progress */
	    uword DU$IPCB_Internal:1;	/* Connection is internal */
	    };
	};
    ulong DU$IPCB_User_ID;		/* Owning process */
    } D$IPCB_Dump_Return_Blk;

/* Define ARP cache dump block format (single entry) *

/* !!!HACK!!! Should this be defined here? */
#define MAX_ARP_HDW_LEN 8

typedef struct {
    uword DU$ARP_INDEX;			/* Arp cache "index" */
    uword DU$ARP_DEVICE;		/* Device index */
    ulong DU$ARP_EXPIRE;		/* Expiration time */
    ulong DU$ARP_SAVEQB;	/* Saved QB */
    union {
	uword DU$ARP_FLAGS;		/* Flags */
	struct {
	    uword DU$ARP_VALID:1;	/* Entry is valid */
	    };
	};
    ulong DU$ARP_IPADDR;		/* IP address */
    uword DU$ARP_HWSIZE;		/* Physical address length */
    uchar DU$ARP_HWADDR[MAX_ARP_HDW_LEN]; /* Physical address */
    } D$ARP_Dump_Return_Blk_Entry;

#define D$ARP_Dump_MaxBlksize 512	/* Max size of block */
#define D$ARP_Dump_Maxblk (D$ARP_Dump_MaxBlksize/sizeof(D$ARP_Dump_Return_Blk_Entry))

/* Dump of a single device interface */

#define DevNam_Max_Size		20
#define DevSpec_Max_Size	20

/* typedef struct {
 */
struct D$Dev_Dump_Return_Blk {
    in_addr DU$Dev_Address;	/* Interface IP address */
    in_addr DU$Dev_netmask;	/* Interface IP Network Mask */
    in_addr DU$Dev_network;	/* Interface IP Network */
    uword   DU$Dev_Pck_Xmit;	/* Packet's transmitted */
    uword   DU$Dev_Pck_Recv;	/* Packet's received */
    uword   DU$DevNam_Len;
    uword   DU$DevSpec_Len;
    char    DU$DevNam_Str[DevNam_Max_Size];
    char    DU$DevSpec_Str[DevSpec_Max_Size];
    };

#undef word short
#undef uword unsigned short	
#undef ushort unsigned short
#undef ulong unsigned long
#undef uchar unsigned char



/* Function call (FCALL) codes */
#define NCP$C_NULL		 0
#define NCP$C_ACP		 1
#define NCP$C_POOL		 2
#define NCP$C_LINK		 3
#define NCP$C_ARP		 4
#define NCP$C_INTERFACE		 5
#define NCP$C_ROUTE		 6
#define NCP$C_IP		 7
#define NCP$C_ICMP		 8
#define NCP$C_UDP		 9
#define NCP$C_TCP		10
#define NCP$C_SNMP		11

/* ACP FCall subfunctions	*/
#define NCP$C_ACP_NULL		0
#define NCP$C_ACP_START		1
#define NCP$C_ACP_STOP		2
#define NCP$C_ACP_SET_DEBUG	3
#define NCP$C_ACP_SET_ACTLOG	4

/* POOL FCall subfunctions	*/
#define NCP$C_POOL_NULL		0
#define NCP$C_POOL_DUMP		1

/* LINK FCall subfunctions	*/
#define NCP$C_LINK_NULL		0
#define NCP$C_LINK_ADD		1

/* INTERFACE FCall subfunctions */
#define NCP$C_INTERFACE_NULL	0
#define NCP$C_INTERFACE_ADD	1

/* ROUTE FCall subfunctions	*/
#define NCP$C_ROUTE_NULL	0
#define NCP$C_ROUTE_ADD		1

/* IP FCall subfunctions	*/
#define NCP$C_IP_NULL		0

/* ICMP FCall subfunctions	*/
#define NCP$C_ICMP_NULL		0

/* UDP FCall subfunctions	*/
#define NCP$C_UDP_NULL		0

/* TCP FCall subfunctions	*/
#define NCP$C_TCP_NULL		0
#define NCP$C_TCP_KILL		1

/* SNMP FCall subfunctions	*/
#define NCP$C_SNMP_NULL		0

