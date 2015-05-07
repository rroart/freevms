#ifndef netcommon_h
#define netcommon_h

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

typedef union
{
    struct
    {
        short int w1,w2,w3,w4;
    } w;
    struct
    {
        int l1,l2;
    } l;
} quadword;

typedef union
{
    struct
    {
        int l1,l2,l3,l4;
    } l;
} octaword;

#define word short
#define uword unsigned short
#define ushort unsigned short
#define ulong unsigned long
#define uchar unsigned char


#endif _CMU_TYPES

#define word short
#define uword unsigned short
#define ushort unsigned short
#define ulong unsigned long
#define uchar unsigned char
#define uint unsigned int

typedef union
{
    unsigned int l;
} in_addr;


/* Name length and address count literals */

#define HOST_NAME_MAX_SIZE  128
#define MAX_HNAME       HOST_NAME_MAX_SIZE
#define MAX_HADDRS      20
/*!!!HACK!!! Get rid of this!!!*/
#define MAX_RRSIZE      1024;

#define MAX_UDPCB   100     /* Max number of UDP connections */
#define MAX_ICMPCB   10     /* Max number of ICMP connections */
#define MAX_IPCB     10     /* Max number of IP connections */

/* Define protocol codes for open */
#define U$TCP_PROTOCOL  0       /* TCP protocol (default) */
#define U$UDP_PROTOCOL  1       /* UDP protocol */
#define U$ICMP_PROTOCOL 2       /* ICMP protocol */
#define U$IP_PROTOCOL   3       /* IP protocol */



/*
    Net I/O Status block (Quadword) field definitions. These are basically the
    same as defined in VMS with the exception of the 2nd longword. The second
    longword contains the ACP error code, if status is SS$_ABORT, or request
    flags and status information if status is SS$_NORMAL. Note that the ICMP
    code field is only defined if the ICMP flag is set, and is only valid for
    special connections which know about ICMP messages.

    !===============================================================!
    +     Bytes Transfered      !   VMS Return Code     +
    !---------------------------------------------------------------!
    +       Unused      !     Flags !   ICMP code   +
    !===============================================================!

*/

typedef struct
{
    uword nsb$status;       /* VMS status return code */
    uword nsb$byte_count;
    union
    {
        uint nsb$xstatus;   /* Second status word */
        struct
        {
            uchar nsb$icmp_code;    /* ICMP mess. code, if ICMP flag set*/
            union
            {
                uchar nsb$flags;        /*  Request flags */
                struct
                {
                    uchar nsb$f_urg:1;  /* URGENT data bit (not implemented) */
                    uchar nsb$f_push:1; /* PUSH data bit */
                    uchar nsb$f_eof:1;  /* EOF (i.e. connect. fully closed) */
                    uchar nsb$f_icmp:1; /* ICMP message bit */
                };
            } flags;
        };
        uint nsb$xerror;    /* ACP error code, if RC = SS$_ABORT */
    } net_status;
} netio_status_block;

#define NSB$URGBIT   1      /* Bit value for URGENT bit in FLAGS field */
#define NSB$PUSHBIT  2      /* Bit value for PUSH bit */
#define NSB$EOFBIT   4      /* Bit value for EOF bit */
#define NSB$ICMPBIT  8      /* Bit value for ICMP bit */



/* %SBTTL 'IP protocols header information block definition.' */

/*
  This is the definition of the block of data describing UDP ICMP and IP
  header information when using the send and receive commands.

 Special adress block for send/receive in non-TCP mode.
*/

typedef struct
{
    uint ipadr$src_host;
    uint ipadr$dst_host;

    union
    {
        uchar ipadr$extension[8];
        struct
        {
            uint ipadr$ext1;
            uint ipadr$ext2;
        };
        struct
        {
            uword ipadr$src_port;
            uword ipadr$dst_port;
        };
        struct
        {
            uchar ipadr$type;
            uchar ipadr$code;
            uword ipadr$cksum;
            uint ipadr$special;
        };
    };
} ipadr$address_block;

#if 0
#define IPADR$ADDRESS_SIZE sizeof(ipadr$address_block)
#endif
#define IPADR$ADDRESS_BLEN sizeof(ipadr$address_block)
#define IPADR$UDP_ADDRESS_BLEN 12

/* NET$INFO - return block structure. */

typedef struct
{
    uchar ci$fhost_name_size;
    uchar ci$lhost_name_size;
    uchar ci$foreign_host[HOST_NAME_MAX_SIZE];
    short fill;
    uint ci$foreign_port;
    // short fill_1;
    uchar ci$local_host[HOST_NAME_MAX_SIZE];
    // short fill_2;
    uint ci$local_port;
    uint ci$local_internet_adrs;
    uint ci$remote_internet_adrs;
} connection_info_return_block;

#if 0
#define CONNECTION_INFO_BLKSIZE sizeof(connection_info_return_block)
#endif
#define CONNECTION_INFO_BYTESIZE sizeof(connection_info_return_block)



/* NET$GTHST - Return block for Name to Address translation */
// check swap with nmlook?
typedef struct
{
    uint gha$adrcnt;        /* Count of addresses */
    ulong gha$adrlst[MAX_HADDRS];   /* Address list */
    uint gha$namlen;        /* Official name length */
    char      gha$namstr[MAX_HNAME];    /* Official name string */
} gthst_adlook_block;

#define GTHST_ADLOOK_RET_ARGS_LENGTH sizeof(gthst_nmlook_block)

/* NET$GTHST - Return block for Address to Name translation */

typedef struct
{
    int ghn$adrcnt;
    char * ghn$adrlst[4*MAX_HADDRS];
    uint ghn$namlen;        /* Host name string length */
    char      ghn$namstr[MAX_HNAME];
} gthst_nmlook_block;

#define GTHST_NMLOOK_RET_ARGS_LENGTH sizeof(gthst_adlook_block)

/* NET$GTHST - Return block for Name to RR translation */

typedef struct
{
    uword grr$rdlen;        /* Resource data length */
    char  grr$rdata[];  /* RData followed by name. */
} gthst_rrlook_block;



/* Dump Directive Codes. */

#define DU$DYNAMIC_MEM_ALLOC     1 /* Memory management counters */
#define DU$TCP_STATS         2 /* TCP statistic counters */
#define DU$LOCAL_CONNECTION_ID   3 /* List of active TCP connections */
#define DU$TCB_DUMP      4 /* Contents of a TCB */
#define DU$HOST_STATS        5 /* obsolete, unused */
#define DU$KNOWN_HOST_INDEX  6 /* obsolete, unused */
#define DU$DEVICE_DUMP       7 /* Device-dependant status */
#define DU$UDP_CONNECTIONS   8 /* List of UDP connections */
#define DU$UDPCB_DUMP        9 /* Contents of a UDPCB */
#define DU$ARP_CACHE        10 /* Contents of ARP cache */
#define DU$ICMP_CONNECTIONS 11 /* List of ICMP connections */
#define DU$ICMPCB_DUMP      12 /* Contents of a IPCB */
#define DU$IP_CONNECTIONS   13 /* List of IP connections */
#define DU$IPCB_DUMP        14 /* Contents of a IPCB */
#define DU$DEVICE_LIST      15 /* List of device indexes */
#define DU$DEVICE_STAT      16 /* Contents of dev_config entry */

/* Dump return argument block formats. */

/* Dump Dynamic Memory allocation stats. */
/* Dump Directive: DU$Dynamic_mem_Alloc  */

typedef struct
{
    int dm$qb;          /* Queue Blocks */
    int dm$ua;          /* UARG Blocks */
    int dm$cs;          /* TCP Control segments */
    int dm$dms;     /* default size max segs */
    int dm$nm;          /* net mesg receive buffer */
    int dm$qbmx;        /* Queue blocks max */
    int dm$uamx;        /* UARG Blocks max */
    int dm$csmx;        /* TCP Control segments max */
    int dm$dmsmx;       /* default size max segs max */
    int dm$nmmx;        /* net mesg receive buffer max */
    int dm$qbal;        /* qblks allocated */
    int dm$uaal;        /* Uarg blks allocated */
    int dm$csal;        /* Control segs allocated */
    int dm$dmsal;       /* Default segs allocated */
    int dm$nmal;        /* net recv bufs allocated */
    int dm$qbfr;        /* QBlks free */
    int dm$uafr;        /* Uarg blks free */
    int dm$csfr;        /* Control segs free */
    int dm$dmsfr;       /* Default segs free */
    int dm$nmfr;        /* net recv bufs free */
} d$mem_alloc_return_blk;

#define D$MEM_ALLOC_RETURN_BLK sizeof(d$mem_alloc_return_blk)
#define D$MA_BLKSIZE sizeof(d$mem_alloc_return_blk)

/* User return dump blk:  TCP stats. */

typedef struct
{
    int dm$tcpacp_pid;
    int dm$user_io_requests;
    int dm$storeforward;
    int dm$active_conects_opened;
    int dm$passive_conects_opened;
    int dm$data_bytes_xmitted;
    int dm$data_bytes_recved;
    int dm$segs_xmitted;
    int dm$segs_recved;
    int dm$seg_bad_chksum;
    int dm$badseq;
    int dm$duplicate_segs;
    int dm$retrans_segs;
    int dm$rpz_rxq;
    int dm$oorw_segs;
    int dm$future_rcvd;
    int dm$future_used;
    int dm$future_dropped;
    int dm$future_dups;
    int dm$servers_forked;
    int dm$uptime[2];
    int dm$arps_received;
} d$tcp_stats_return_blk;

#define D$TS_BLKSIZE sizeof(d$tcp_stats_return_blk)

/* Dump all known local-connection-ids. */

/*
!!!HACK!!! OH MY GOD WHAT AM I DOING! I'VE HARD-WIRED IN 100 INSTEAD OF
!!!HACK!!! MAX_TCB.  DON'T LET THIS CODE GO OUT THE DOOR!!!!!
!!!HACK!!! remember to improve the dumping interface...
*/

#define D$LC_ID_BLKSIZE 100*4 /* byte size. */
typedef
uint D$LC_ID_Return_Blk[D$LC_ID_BLKSIZE/4];

/* Dump a specified TCB */

typedef struct
{
    long dm$tcb_addr;       /* Address of TCB (internal) */
    int dm$user_id;     /* Owning process ID */
    word dm$state;      /* Current TCB state */
    word dm$last_state;     /* */
    int  dm$foreign_host;   /* Foreign host # */
    int dm$foreign_port;    /* Foreign port # */
    int dm$local_host;  /* Local host # */
    int dm$local_port;      /* Local port # */
    int dm$user_recv_qe;    /* # of pending user receives */
    int dm$user_send_qe;    /* # of pending user sends */
    int dm$net_recv_qe; /* # of pending receive buffers */
    int dm$future_qe;       /* # of buffers on future queue */
    int dm$rexmit_qe;       /* # of bytes on RX queue */
    long dm$dasm_ptr;       /* Disassembly pointer */
    int dm$dasm_bc;     /* Dissasembly byte count */
    long dm$asm_ptr;        /* Assembly pointer */
    int dm$asm_bc;      /* Assembly byte count */
    int dm$iss;         /* Initial Send Sequence # */
    int dm$snd_una;     /* # of bytes send but not ACK'ed */
    int dm$snd_nxt;     /* Next Send Sequence # */
    int dm$snd_wnd;     /* Send window count */
    int dm$snd_wl;      /*    DM$SND_BS     = [$SLong], */
    signed int dm$snd_bs;
    int dm$irs;     /* Initial Receive Sequence # */
    int dm$rcv_nxt;     /* Next Receive Sequence # */
    int dm$rcv_wnd;     /* Receive window count */
    int dm$rcv_bs;      /* */
    int dm$conn_timeout;    /* Connection dead timeout */
    int dm$rt_timeout;      /* Retransmission timer */
    int dm$round_trip_time; /* Measured round-trip-time */
} d$tcb_dump_return_blk;

#define D$TCB_DUMP_BLKSIZE sizeof(d$tcb_dump_return_blk)

/* Dump list of udpCB's */

#define D$UDP_LIST_BLKSIZE  MAX_UDPCB*4 /* byte size. */

typedef
uint D$UDP_LIST_RETURN_BLK[D$UDP_LIST_BLKSIZE/4];

/* Dump of a single UDPCB */

typedef struct
{
    char    *du$udpcb_address;      /* Internal UDPCB address */
    int   du$udpcb_foreign_host;    /* Foreign host number */
    uword   du$udpcb_foreign_port;  /* Foreign port number */
    int du$udpcb_local_host;    /* Local host number */
    uword   du$udpcb_local_port;    /* Local port number */
    int    du$udpcb_nr_qcount;      /* Count of items on receive queue */
    int    du$udpcb_ur_qcount;      /* Count of items on user queue */
    union
    {
        uword   du$udpcb_flags;     /* UDPCB flags */
        struct
        {
            uword du$udpcb_wildcard:1;  /* Wildcard foreign host/port */
            uword du$udpcb_raw_mode:1;  /* User wants IP headers (not impl) */
            uword du$udpcb_aborting:1;  /* Connection is closing */
            uword du$udpcb_nmlook:1;    /* Name lookup in progress */
            uword du$udpcb_internal:1;  /* Connection is internal */
        };
    };
    uint du$udpcb_user_id;      /* Owning process */
} d$udpcb_dump_return_blk;

#define  D$UDPCB_DUMP_BLKSIZE sizeof(d$udpcb_dump_return_blk)
#define  D$DEV_DUMP_BLKSIZE sizeof(d$udpcb_dump_return_blk)

/* Dump list of ICMPCB's */

#define D$ICMP_LIST_BLKSIZE MAX_ICMPCB*4    /* byte size. */
typedef
uint D$ICMP_LIST_RETURN_BLK[D$ICMP_LIST_BLKSIZE/4];

/* Dump of a single ICMPCB */

typedef struct
{
    char    *DU$ICMPCB_Address;     /* Internal ICMPCB address */
    int DU$ICMPCB_Foreign_Host; /* Foreign host number */
    int DU$ICMPCB_Local_Host;   /* Local host number */
    int    DU$ICMPCB_NR_Qcount; /* Count of items on receive queue */
    int    DU$ICMPCB_UR_Qcount; /* Count of items on user queue */
    union
    {
        uword   DU$ICMPCB_Flags;        /* ICMPCB flags */
        struct
        {
            uword DU$ICMPCB_Wildcard:1; /* Wildcard foreign host/port */
            uword DU$ICMPCB_Raw_Mode:1; /* User wants IP headers (not impl) */
            uword DU$ICMPCB_Aborting:1; /* Connection is closing */
            uword DU$ICMPCB_NMlook:1;   /* Name lookup in progress */
            uword DU$ICMPCB_Internal:1; /* Connection is internal */
        };
    };
    uint DU$ICMPCB_User_ID;     /* Owning process */
} D$ICMPCB_Dump_Return_Blk;

#define D$ICMPCB_DUMP_BLKSIZE sizeof(D$ICMPCB_Dump_Return_Blk)

/* Dump list of IPCB's */

#define D$IP_List_BlkSize   MAX_IPCB*4  /* byte size. */
typedef
uint D$IP_List_Return_Blk[D$IP_List_BlkSize/4];

/* Dump of a single IPCB */

typedef struct
{
    char    *du$ipcb_address;       /* Internal IPCB address */
    int du$ipcb_foreign_host;   /* Foreign host number */
    int du$ipcb_local_host; /* Local host number */
    int    du$ipcb_nr_qcount;   /* Count of items on receive queue */
    int    du$ipcb_ur_qcount;   /* Count of items on user queue */
    union
    {
        uword   du$ipcb_flags;      /* IPCB flags */
        struct
        {
            uword du$ipcb_wildcard:1;   /* Wildcard foreign host/port */
            uword du$ipcb_raw_mode:1;   /* User wants IP headers (not impl) */
            uword du$ipcb_aborting:1;   /* Connection is closing */
            uword du$ipcb_nmlook:1; /* Name lookup in progress */
            uword du$ipcb_internal:1;   /* Connection is internal */
        };
    };
    uint du$ipcb_user_id;       /* Owning process */
} d$ipcb_dump_return_blk;

/* Define ARP cache dump block format (single entry) *

/* !!!HACK!!! Should this be defined here? */
#define MAX_ARP_HDW_LEN 8

typedef struct
{
    uword du$arp_index;         /* Arp cache "index" */
    uword du$arp_device;        /* Device index */
    uint du$arp_expire;     /* Expiration time */
    uint du$arp_saveqb; /* Saved QB */
    union
    {
        uword du$arp_flags;     /* Flags */
        struct
        {
            uword du$arp_valid:1;   /* Entry is valid */
        };
    };
    uint du$arp_ipaddr;     /* IP address */
    uword du$arp_hwsize;        /* Physical address length */
    uchar du$arp_hwaddr[MAX_ARP_HDW_LEN]; /* Physical address */
} d$arp_dump_return_blk_entry;

#define D$ARP_DUMP_BLKSIZE    sizeof(d$arp_dump_return_blk_entry)
#if 0
#define D$ARP_DUMP_MAXBLKSIZE 512   /* Max size of block */
#define D$ARP_DUMP_MAXBLK (D$ARP_DUMP_MAXBLKSIZE/sizeof(D$ARP_DUMP_RETURN_BLK_ENTRY))
#endif

/* Dump of a single device interface */

#define DEVNAM_MAX_SIZE     20
#define DEVSPEC_MAX_SIZE    20

typedef struct
{
    int du$dev_address; /* Interface IP address */
    int du$dev_netmask; /* Interface IP Network Mask */
    int du$dev_network; /* Interface IP Network */
    uword   du$dev_pck_xmit;    /* Packet's transmitted */
    uword   du$dev_pck_recv;    /* Packet's received */
    uword   du$devnam_len;
    uword   du$devspec_len;
    char    du$devnam_str[DEVNAM_MAX_SIZE];
    char    du$devspec_str[DEVSPEC_MAX_SIZE];
} d$dev_dump_return_blk;

#define D$User_Return_Blk_Max_Size 128 // check random

#undef word short
#undef uword unsigned short
#undef ushort unsigned short
#undef ulong unsigned long
#undef uchar unsigned char
#undef uint unsigned int



/* Function call (FCALL) codes */
#define NCP$C_NULL       0
#define NCP$C_ACP        1
#define NCP$C_POOL       2
#define NCP$C_LINK       3
#define NCP$C_ARP        4
#define NCP$C_INTERFACE      5
#define NCP$C_ROUTE      6
#define NCP$C_IP         7
#define NCP$C_ICMP       8
#define NCP$C_UDP        9
#define NCP$C_TCP       10
#define NCP$C_SNMP      11

/* ACP FCall subfunctions   */
#define NCP$C_ACP_NULL      0
#define NCP$C_ACP_START     1
#define NCP$C_ACP_STOP      2
#define NCP$C_ACP_SET_DEBUG 3
#define NCP$C_ACP_SET_ACTLOG    4

/* POOL FCall subfunctions  */
#define NCP$C_POOL_NULL     0
#define NCP$C_POOL_DUMP     1

/* LINK FCall subfunctions  */
#define NCP$C_LINK_NULL     0
#define NCP$C_LINK_ADD      1

/* INTERFACE FCall subfunctions */
#define NCP$C_INTERFACE_NULL    0
#define NCP$C_INTERFACE_ADD 1

/* ROUTE FCall subfunctions */
#define NCP$C_ROUTE_NULL    0
#define NCP$C_ROUTE_ADD     1

/* IP FCall subfunctions    */
#define NCP$C_IP_NULL       0

/* ICMP FCall subfunctions  */
#define NCP$C_ICMP_NULL     0

/* UDP FCall subfunctions   */
#define NCP$C_UDP_NULL      0

/* TCP FCall subfunctions   */
#define NCP$C_TCP_NULL      0
#define NCP$C_TCP_KILL      1

/* SNMP FCall subfunctions  */
#define NCP$C_SNMP_NULL     0

#endif
