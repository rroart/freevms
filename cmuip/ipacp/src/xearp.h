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
// Definitions for Address Resolution Protocol
// Written by: Vince Fuller, April 1986
// Copyright (c) 1986, Vince Fuller and Carnegie-Mellon University
//
//	10-Feb-1992	Henry W. Miller		USBR
//	Set MAX_RCV_BUF back down to 8 ; was eating up too much NPAGEDYN
//
//	5-Aug-1991	Henry W. Miller		USBR
//	Set MAX_RCV_BUF to 16.
//
// not yet LIBRARY 'CMUIP_SRC:[central]NETXPORT';		// VMS specifics

#define     AR$OP_REQUEST   1		// Request function
#define     AR$OP_REPLY   2		// Reply function

// Definitions of AR$HRD (hardware) fields

#define     AR$HRD_ETHER   1		// 10MB ethernet
#define     AR$HRD_3MB   2		// 3MB ethernet
#define     AR$HRD_AMRAD   3		// Amateur Radio AX.25
#define     AR$HRD_PRONET   4		// proNET
#define     AR$HRD_CHAOS   5		// CHAOSnet

// Definition of AR$PRO (protocol) fields

#define     AR$PRO_PUP   %X'200'	// PUP
#define     AR$PRO_IP   %X'800'	// IP
#define     AR$PRO_IP_3MB   %O'1001'	// IP (3MB Experimental Ethernet)

// Address lengths

#define     ARP_HDW_LEN   8		// Max length of hardware addresses
#define     ARP_IP_LEN   4		// Length of IP addresses
#define     ARP_MAXADRS   ARP_HDW_LEN*2+ARP_IP_LEN*2

struct arp_PKT
{
  unsigned short     ar$hrd	;	// Hardware address space
  unsigned short     ar$pro	;	// Protocol address space
  unsigned char     ar$hln	;	// Byte length of hardware addresses
  unsigned char     ar$pln	;	// Byte length of protocol addresses
  unsigned short     ar$op	;	// ARP opcode
  unsigned char     ar$xtra	[0];// Addresses
};

#define     ARP_HLEN   sizeof(struct arp_PKT)
#define     ARP_LEN   ARP_HLEN+ARP_MAXADRS
#if 0
MACRO ARP_PKT = BLOCK[ARP_LEN] FIELD(ARP_FIELDS) %;
#endif

#define     ARP_MAX_LEN   ARP_LEN	// To export to device drivers

// Header portion of an ARP receive buffer

 struct xe_arp_structure 
 {
   unsigned short     ar_chan ;	// vms channel for arp
   unsigned short     ar_ios0 ;	// iosb for arp
   unsigned short     ar_ios1 ;
   unsigned short     ar_ios2 ;
   unsigned short     ar_ios3 ;
   unsigned char     phys$1  [6];	// physical address 1 (destination)
   union {
     unsigned char     phys$2  [6];	// physical address 2 (source)
     struct {
       unsigned long 	ar_phy_src1 ;
       unsigned short 	ar_phy_src2 ;
     };
   };
   unsigned short     ar_pro  ;	// hardware protocol number
   unsigned char     ar_data [0];// Offset of ARP data
 };


#define     XE_ARP_HLEN  sizeof(struct xe_arp_structure)
#define     XE_ARP_LEN   XE_ARP_HLEN+ARP_MAX_LEN
#if 0
 MACRO
   XE_ARP_STRUCTURE = BLOCK[XE_ARP_LEN] FIELD(XE_ARP_FIELDS*/;
#endif

#define     MAX_RCV_BUF   8

					      
