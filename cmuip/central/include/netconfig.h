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
!++
! NETCONFIG.H	Copyright (c) 1990	Carnegie Mellon University
! 
! Description:
!
!	Supporting declarations for IP Transport device modules
!
! Written By:	Bruce R. Miller		CMU Network Development
! Date:		09-Aug-1990 (Thursday)
!
! Modifications:
!--
*/

#ifndef _CMU_TYPES
#define _CMU_TYPES

typedef union {
    struct {short int w1,w2,w3,w4; } w;
    struct {int l1,l2; } l;
    } quadword;

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

/* 'Network Device Configuration blockvector Entry Fields.' */

/*
Here we define the fields of the device configuration table.  Each entry
describes a network device plus the assigned IO channel # after device
initialization.  One entry per device.  The fields are initialized by the
invocation of the "device_init" macro (see dec_config definition in
devconfig.bli) at compile time and from the file devconfig.txt during acp initialization.
!!!HACK!!! fix this comment to mention the run-time loadable images.
*/

typedef struct {
  unsigned char dc_begin[0];
    quadword	dc_devname;
    quadword	dc_devspec;
    int		*dc_dev_interface;
    ulong	dc_ip_address;		/* Device IP address */
    ulong	dc_ip_netmask;		/* Device IP network mask */
    ulong	dc_ip_network;		/* Device IP network value */
    int		(*dc_rtn_Init)();
    int		(*dc_rtn_Xmit)();
    int		(*dc_rtn_Dump)();
    int		(*dc_rtn_check)();
    ulong	*dc_Send_Qhead;
    ulong	*dc_send_Qtail;

  ulong  dc_flags;
    unsigned	dc_valid_device:1;
    unsigned	dc_online:1;
    unsigned	dc_is_clone:1;

  unsigned long int dc_clone_dev;

    /* MIB data */
    ulong	dcmib_ifIndex;		/* Interface index number */
  quadword	dcmib_ifDescr;		/* Interface description string */
    ulong	dcmib_ifType;		/* device type */
    ulong	dcmib_ifMTU;		/* Max Transfer Unit */
    ulong	dcmib_ifSpeed;		/* Bandwidth (bits/second) */
    ulong	dcmib_ifPAsize;		/* size of Physical Addr */
    ulong	*dcmib_ifPhysAddress;	/* pntr to Physical Addr */
    ulong	dcmib_ifAdminStatus;	/* Desired status of interface */
    ulong	dcmib_ifOperStatus;	/* Current status of interface */
    ulong	dcmib_ifLastState;	/* time of last state change */
    ulong	dcmib_ifInOctets;	/* Octets received */
    ulong	dcmib_ifInUcastPkts;	/* Unicasts delivered */
    ulong	dcmib_ifInNUcastPkt;	/* non-Unicasts delivered */
    ulong	dcmib_ifInDiscards;	/* good but discarded pckts */
    ulong	dcmib_ifInErrors;	/* bad and discarded */
    ulong	dcmib_ifInUnknownProtos;/* Unknown protocol fields */
    ulong	dcmib_ifOutOctets;	/*  */
    ulong	dcmib_ifOutUcastPkts;	/*  */
    ulong	dcmib_ifOutNUcastPkts;	/*  */
    ulong	dcmib_ifOutDiscards;	/*  */
    ulong	dcmib_ifOutErrors;	/*  */
    ulong	dcmib_ifOutQLen;
    } Device_Configuration_Entry;

#define DC_ENTRY_SIZE sizeof(Device_Configuration_Entry)

/*
Where:
!!!HACK!!! Update this table!
dc_devname	= String descriptor for the device name.
dc_phy_addr	= Physical device address (bytestream)
dc_ip_address	= IP address for this interface
dc_ip_netmask	= IP network mask for this interface
dc_dev_interface= address of the controller-specific info block
dc_init_rtn	= Address of routine to initialize the device.
dc_Xmit_rtn	= Address of routine to transmit packets.
dc_dump_rtn	= Address of routine to do dump specific functions.
dc_send_qhead	= Transmit queue head
dc_send_qtail	= Transmit queue tail.
dc_recv_qhead	= Receive queue head(Ethernet only).
dc_recv_qtail	=   "       "   tail    "       "
dc_restart_time	= How long to leave the ether card shutdown.
dc_restart_count= Number of times the card has been restarted.
dc_retry_count	= Current number of consecutive errors from card.
dc_ARP_buffer   = Pointer to buffer for ARP processing
dc_io_chan	= VMS IO channel assigned to this device.
dc_ARP_chan	= VMS IO channel assigned for ARP on this device.
dc_ARP_block	= Pointer to ARP information block for device.
dc_phy_size	= Length of physical addresses on device.
dc_clone_dev	= Device index of real device (for "cloned" devices only).
dc_valid_device = On if valid dev_config table entry set up during runtime by
		  device initialization routine.
dc_online	= On if device was initialized correctly by it's own device
		  init routine (and is currently available).
dc_doswap	= On if ARP protocol requires byteswap of multi-byte values.
dc_need_2_free	= On if buffer free pending during restart (DEUNA/DEQNA only).
dc_IO_queued	= On if I/O in progress during restart (DEUNA/DEQNA only).
dc_XE_started	= On if device successfully started (DEUNA/DEQNA only).
dc_is_clone	= On if device is a clone (multiple IP address).
*/

#define DC_Max_Num_Net_Devices 4	/* size of table. */



/*%SBTTL     'Net Send Queue structure definition'*/
/****************************************************************************

Output segments are placed on the Net Send Q by the Send_IP routine.
Input datagrams are also placed on this Q by the IP_Receive routine
if they are destined for another host.  These store-and-forward packets
include the IP header as well as the segment from higher protocol layers.
The Net_Send routine is called to empty the Net_Send_Queue.

 ****************************************************************************/

typedef struct {
    ulong	*NSQ$Next;
    ulong	*NSQ$Last;
    ulong	*NSQ$Driver;
    ulong	*NSQ$Data;
    ushort	NSQ$Datasize;
    ulong	NSQ$IP_Dest;
    char	*NSQ$Del_Buf;
    ushort	NSQ$Del_buf_size;
    ushort	NSQ$Type;
    uchar	NSQ$Retry;
    uchar	NSQ$Delete;
    } Net_Send_Queue_Element;

/* Q head structure definition */

typedef struct {
    char	*NS$Qhead;
    char	*NS$Qtail;
    } QH_Net_send;



/*!!!HACK!!! Why are there no comments here? (besides this one?)*/

typedef struct {
    int		(*DI$Init)();
    int		(*DI$Xmit)();
    int		(*DI$Dump)();
    int		(*DI$Check)();
    int		(*DI$ARP_Dump)();
    } Device_Info_Structure;



	/*
	Here we describe the IPACP Information block.  This structure
	is assembled by the IPACP to describe itself to other modules
	who wish to interact with it.  It consists of entry points,
	literals, and pointers to global parameters.
	*/

typedef struct {
    /* IAPCP receive callback. */
    int		(*ACPI$IP_Receive)();

    /* pointer to IPACP AST flag */
    int		*ACPI$AST_in_progress;

    /* pointer to IPACP nap control flag */
    int		*ACPI$Sleeping;

    /* Interrupt blocking routines */
    int		(*ACPI$NOINT)();
    int		(*ACPI$OKINT)();

    /* Error reporting routine */
    int		(*ACPI$Device_Error)();

    /* IPACP self-address recognition */
    int		(*ACPI$IP_ISME)();

    /* Memory allocation routines */
    int		(*ACPI$Seg_Get)();
    int		(*ACPI$Seg_Free)();
    int		(*ACPI$QBlk_Free)();

    /* Formatted event logging routines */
    int		(*ACPI$LOG_STATE)();
    int		(*ACPI$QL_FAO)();
    int		(*ACPI$LOG_FAO)();
    int		(*ACPI$OPR_FAO)();
    int		(*ACPI$ERROR_FAO)();
    int		(*ACPI$FATAL_FAO)();

    /* IPACP max physical buffer size */
    ushort	ACPI$MPBS;
    } IPACP_Info_Structure;

#undef word short
#undef uword unsigned short	
#undef ushort unsigned short
#undef ulong unsigned long
#undef uchar unsigned char
