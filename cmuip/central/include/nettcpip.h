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

	NetTCPIP

Facility:

	A system wide definition file.  This file describes the various
	structures and protocol headers used in the protocols of the
	TCP/IP suite.

Author:

	Writen by Bruce R. Miller, borrowing from IP.BLI and Structure.REQ

Modification History:

16-Aug-90 Converted to C

*/

    /* Define IP-based protocols */
#define ICMP_PROTOCOL	 1
#define TCP_PROTOCOL	 6
#define UDP_PROTOCOL	17


/* This is sooooooooooooo ugly!  Someone should clean it up. (not me) */

/* TCP Port Service Multiplexer */
#define TCP_PORT_TCPMUX			   1
/* Remote Job Entry */
#define TCP_PORT_RJE			   5
/* Echo */
#define TCP_PORT_ECHO			   7
/* Discard */
#define TCP_PORT_DISCARD		   9
/* Active Users */
#define TCP_PORT_USERS			  11
/* Daytime */
#define TCP_PORT_DAYTIME		  13	
/* Quote of the Day */
#define TCP_PORT_QUOTE			  17
/* Character Generator */
#define TCP_PORT_CHARGEN		  19	
/* File Transfer [Default Data] */
#define TCP_PORT_FTP_DATA		  20	
/* File Transfer [Control] */
#define TCP_PORT_FTP			  21
/* Telnet */
#define TCP_PORT_TELNET			  23
/* Simple Mail Transfer */
#define TCP_PORT_SMTP			  25
/* NSW User System FE */
#define TCP_PORT_NSW_FE			  27
/* MSG ICP */
#define TCP_PORT_MSG_ICP		  29	
/* MSG Authentication */
#define TCP_PORT_MSG_AUTH		  31	
/* Display Support Protocol */
#define TCP_PORT_DSP			  33
/* private printer server */
#define TCP_PORT_Prv_PS			  35
/* Time */
#define TCP_PORT_TIME			  37
/* Resource Location Protocol */
#define TCP_PORT_RLP			  39
/* Graphics */
#define TCP_PORT_GRAPHICS		  41	
/* Host Name Server */
#define TCP_PORT_NAMESERVER		  42	
/* Who Is */
#define TCP_PORT_NICNAME		  43	
/* MPM FLAGS Protocol */
#define TCP_PORT_MPM_FLAGS		  44	
/* Message Processing Module [recv] */
#define TCP_PORT_MPM			  45
/* MPM [default send] */
#define TCP_PORT_MPM_SND		  46	
/* NI FTP */
#define TCP_PORT_NI_FTP			  47
/* Login Host Protocol */
#define TCP_PORT_LOGIN			  49
/* IMP Logical Address Maintenance */
#define TCP_PORT_LA_MAINT		  51	
/* Domain Name Server */
#define TCP_PORT_DOMAIN			  53
/* ISI Graphics Language */
#define TCP_PORT_ISI_GL			  55
/* private terminal access */
#define TCP_PORT_Prv_TA			  57
/* private file service */
#define TCP_PORT_Prv_FS			  59
/* NI MAIL */
#define TCP_PORT_NI_MAIL		  61	
/* VIA Systems - FTP */
#define TCP_PORT_VIA_FTP		  63	
/* TACACS-Database Service */
#define TCP_PORT_TACACS_DS		  65	
/* Bootstrap Protocol Server */
#define TCP_PORT_BOOTPS			  67
/* Bootstrap Protocol Client */
#define TCP_PORT_BOOTPC			  68
/* Trivial File Transfer */
#define TCP_PORT_TFTP			  69
/* Remote Job Service */
#define TCP_PORT_NETRJS_1		  71	
/* Remote Job Service */
#define TCP_PORT_NETRJS_2		  72	
/* Remote Job Service */
#define TCP_PORT_NETRJS_3		  73	
/* Remote Job Service */
#define TCP_PORT_NETRJS_4		  74	
/* any private dial out service */
#define TCP_PORT_Prv_DOS		  75	
/* any private RJE service */
#define TCP_PORT_Prv_RJE		  77	
/* Finger */
#define TCP_PORT_FINGER			  79
/* HOSTS2 Name Server */
#define TCP_PORT_HOSTS2_NS		  81	
/* MIT ML Device */
#define TCP_PORT_MIT_ML_DEV1		  83	
/* MIT ML Device */
#define TCP_PORT_MIT_ML_DEV2		  85	
/* private terminal link */
#define TCP_PORT_Prv_TL			  87
/* SU/MIT Telnet Gateway */
#define TCP_PORT_SU_MIT_TG		  89	
/* MIT Dover Spooler */
#define TCP_PORT_MIT_DOV		  91	
/* Device Control Protocol */
#define TCP_PORT_DCP			  93
/* SUPDUP */
#define TCP_PORT_SUPDUP			  95
/* Swift Remote Vitural File Protocol */
#define TCP_PORT_SWIFT_RVF		  97	
/* TAC News */
#define TCP_PORT_TACNEWS		  98	
/* Metagram Relay */
#define TCP_PORT_METAGRAM		  99	
/* NIC Host Name Server */
#define TCP_PORT_HOSTNAME		 101	
/* ISO-TSAP */
#define TCP_PORT_ISO_TSAP		 102	
/* X400 */
#define TCP_PORT_X400			 103
/* X400-SND */
#define TCP_PORT_X400_SND		 104	
/* Mailbox Name Nameserver */
#define TCP_PORT_CSNET_NS		 105	
/* Remote Telnet Service */
#define TCP_PORT_RTELNET		 107	
/* Post Office Protocol - Version 2 */
#define TCP_PORT_POP2			 109
/* Post Office Protocol - Version 3 */
#define TCP_PORT_POP3			 110
/* SUN Remote Procedure Call */
#define TCP_PORT_SUNRPC			 111
/* Authentication Service */
#define TCP_PORT_AUTH			 113
/* Simple File Transfer Protocol */
#define TCP_PORT_SFTP			 115
/* UUCP Path Service */
#define TCP_PORT_UUCP_PATH		 117	
/* Network News Transfer Protocol */
#define TCP_PORT_NNTP			 119
/* Encore Expedited Remote Proc. Call */
#define TCP_PORT_ERPC			 121
/* Network Time Protocol */
#define TCP_PORT_NTP			 123
/* Locus PC-Interface Net Map Server */
#define TCP_PORT_LOCUS_MAP		 125	
/* Locus PC-Interface Conn Server */
#define TCP_PORT_LOCUS_CON		 127	
/* Password Generator Protocol */
#define TCP_PORT_PWDGEN			 129
/* CISCO FNATIVE */
#define TCP_PORT_CISCO_FNA		 130	
/* CISCO TNATIVE */
#define TCP_PORT_CISCO_TNA		 131	
/* CISCO SYSMAINT */
#define TCP_PORT_CISCO_SYS		 132	
/* Statistics Service */
#define TCP_PORT_STATSRV		 133	
/* INGRES-NET Service */
#define TCP_PORT_INGRES_NET		 134	
/* Location Service */
#define TCP_PORT_LOC_SRV		 135	
/* PROFILE Naming System */
#define TCP_PORT_PROFILE		 136	
/* NETBIOS Name Service */
#define TCP_PORT_NETBIOS_NS		 137	
/* NETBIOS Datagram Service */
#define TCP_PORT_NETBIOS_DGM		 138	
/* NETBIOS Session Service */
#define TCP_PORT_NETBIOS_SSN		 139	
/* EMFIS Data Service */
#define TCP_PORT_EMFIS_DATA		 140	
/* EMFIS Control Service */
#define TCP_PORT_EMFIS_CNTL		 141	
/* Britton-Lee IDM */
#define TCP_PORT_BL_IDM			 142
/* Interim Mail Access Protocol v2 */
#define TCP_PORT_IMAP2			 143
/* NewS */
#define TCP_PORT_NEWS			 144
/* UAAC Protocol */
#define TCP_PORT_UAAC			 145
/* ISO-IP0 */
#define TCP_PORT_ISO_TP0		 146	
/* ISO-IP */
#define TCP_PORT_ISO_IP			 147
/* CRONUS_SUPPORT */
#define TCP_PORT_CRONUS			 148
/* AED 512 Emulation Service */
#define TCP_PORT_AED_512		 149	
/* SQL-NET */
#define TCP_PORT_SQL_NET		 150	
/* HEMS */
#define TCP_PORT_HEMS			 151
/* Background File Transfer Program */
#define TCP_PORT_BFTP			 152
/* SGMP */
#define TCP_PORT_SGMP			 153
/* NETSC */
#define TCP_PORT_NETSC_PROD		 154	
/* NETSC */
#define TCP_PORT_NETSC_DEV		 155	
/* SQL Service */
#define TCP_PORT_SQLSRV			 156
/* KNET/VM Command/Message Protocol */
#define TCP_PORT_KNET_CMP		 157	
/* PCMail Server */
#define TCP_PORT_PCMail_SRV		 158	
/* NSS-Routing */
#define TCP_PORT_NSS_Routing		 159	
/* SGMP-TRAPS */
#define TCP_PORT_SGMP_TRAPS		 160	
/* SNMP */
#define TCP_PORT_SNMP			 161
/* SNMPTRAP */
#define TCP_PORT_SNMPTRAP		 162	
/* CMIP/TCP Manager */
#define TCP_PORT_CMIP_Manage		 163	
/* CMIP/TCP Agent */
#define TCP_PORT_CMIP_Agent		 164	
/* Xerox */
#define TCP_PORT_XNS_Courier		 165	
/* Sirius Systems */
#define TCP_PORT_S_Net			 166
/* NAMP */
#define TCP_PORT_NAMP			 167
/* RSVD */
#define TCP_PORT_RSVD			 168
/* SEND */
#define TCP_PORT_SEND			 169
/* Network PostScript */
#define TCP_PORT_Print_SRV		 170	
/* Network Innovations Multiplex */
#define TCP_PORT_Multiplex		 171	
/* Network Innovations CL/1 */
#define TCP_PORT_CL_1			 172
/* Xyplex */
#define TCP_PORT_Xyplex_MUX		 173	
/* MAILQ */
#define TCP_PORT_MAILQ			 174
/* VMNET */
#define TCP_PORT_VMNET			 175
/* GENRAD-MUX */
#define TCP_PORT_GENRAD_MUX		 176	
/* X Display Manager Control Protocol */
#define TCP_PORT_XDMCP			 177
/* NextStep Window Server */
#define TCP_PORT_NextStep		 178	
/* Border Gateway Protocol */
#define TCP_PORT_BGP			 179
/* Intergraph */
#define TCP_PORT_RIS			 180
/* Unify */
#define TCP_PORT_Unify			 181
/* Unisys-Cam */
#define TCP_PORT_Unisys_Cam		 182	
/* OCBinder */
#define TCP_PORT_OCBinder		 183	
/* OCServer */
#define TCP_PORT_OCServer		 184	
/* Remote-KIS */
#define TCP_PORT_Remote_KIS		 185	
/* KIS Protocol */
#define TCP_PORT_KIS			 186
/* Application Communication Interface */
#define TCP_PORT_ACI			 187
/* MUMPS */
#define TCP_PORT_MUMPS			 188
/* Queued File Transport */
#define TCP_PORT_QFT			 189
/* Gateway Access Control Protocol */
#define TCP_PORT_GACP			 190
/* Prospero */
#define TCP_PORT_Prospero		 191	
/* OSU Network Monitoring System */
#define TCP_PORT_OSU_NMS		 192	
/* Spider Remote Monitoring Protocol */
#define TCP_PORT_SRMP			 193
/* Internet Relay Chat Protocol */
#define TCP_PORT_IRC			 194
/* DNSIX Network Level Module Audit */
#define TCP_PORT_DN6_NLM_AUD		 195	
/* DNSIX Session Mgt Module Audit Redirect */
#define TCP_PORT_DN6_SMM_RED		 196	
/* Directory Location Service */
#define TCP_PORT_DLS			 197
/* Directory Location Service Monitor */
#define TCP_PORT_DLS_Mon		 198	
/* AppleTalk Routing Maintenance */
#define TCP_PORT_AT_RMTP		 201	
/* AppleTalk Name Binding */
#define TCP_PORT_AT_NBP			 202
/* AppleTalk Unused */
#define TCP_PORT_AT_3			 203
/* AppleTalk Echo */
#define TCP_PORT_AT_ECHO		 204	
/* AppleTalk Unused */
#define TCP_PORT_AT_5			 205
/* AppleTalk Zone Information */
#define TCP_PORT_AT_ZIS			 206
/* AppleTalk Unused */
#define TCP_PORT_AT_7			 207
/* AppleTalk Unused */
#define TCP_PORT_AT_8			 208
/* Survey Measurement */
#define TCP_PORT_SUR_MEAS		 243	
/* LINK */
#define TCP_PORT_LINK			 245
/* Display Systems Protocol */
#define TCP_PORT_DSP3270		 246	


/*    ! UDP ports (RFC1060) */

/* Echo */
#define UDP_PORT_ECHO			   7	
/* Discard */
#define UDP_PORT_DISCARD		   9
/* Active Users */
#define UDP_PORT_USERS			  11
/* Daytime */
#define UDP_PORT_DAYTIME		  13	
/* Quote of the Day */
#define UDP_PORT_QUOTE			  17
/* Character Generator */
#define UDP_PORT_CHARGEN		  19	
/* Time */
#define UDP_PORT_TIME			  37
/* Host Name Server */
#define UDP_PORT_NAMESERVER		  42	
/* Who Is */
#define UDP_PORT_NICNAME		  43	
/* Domain Name Server */
#define UDP_PORT_DOMAIN			  53
/* Bootstrap Protocol Server */
#define UDP_PORT_BOOTPS			  67
/* Bootstrap Protocol Client */
#define UDP_PORT_BOOTPC			  68
/* Trivial File Transfer */
#define UDP_PORT_TFTP			  69
/* Finger */
#define UDP_PORT_FINGER			  79
/* SUN Remote Procedure Call */
#define UDP_PORT_SUNRPC			 111
/* Authentication Service */
#define UDP_PORT_AUTH			 113
/* Simple File Transfer Protocol */
#define UDP_PORT_SFTP			 115
/* Network Time Protocol */
#define UDP_PORT_NTP			 123
/* Password Generator Protocol */
#define UDP_PORT_PWDGEN			 129
/* Location Service */
#define UDP_PORT_LOC_SRV		 135	
/* SNMP */
#define UDP_PORT_SNMP			 161
/* SNMPTRAP */
#define UDP_PORT_SNMP_TRAP		 162	
/* CMIP/TCP Manager */
#define UDP_PORT_CMIP_Manage		 163	
/* CMIP/TCP Agent */
#define UDP_PORT_CMIP_Agent		 164	
/* Border Gateway Protocol */
#define UDP_PORT_BGP			 179
/* Internet Relay Chat Protocol */
#define UDP_PORT_IRC			 194

	/* Define more UDP ports */
#define UDP_PORT_NFS			2049

// Max bytesize of IP header with options.
// *** WARNING ***
//	MUST be a multiple of 4.

#define    IP_HDR_BYTE_SIZE 20

// Device dependent header (bytes).  Currently covers NSC hyperchannel
// message-proper & interlan ethernet.
// *** WARNING ***
//	MUST be a multiple of 4.
//////HACK////// What a hack//////  Get rid of this.  Calculate it at run time//
#define    DEVICE_HEADER	68

#define TCP_DATA_OFFSET 5			// in 32-bit words.
#define TCP_HEADER_SIZE (TCP_DATA_OFFSET*4)	// in bytes.
//    Default_Data_Size = 1392,			// default: max size of user
#define DEFAULT_DATA_SIZE 536			// default: max size of user
						// data per segment.

// segment option parameters

#define OPT$SEG_DATASIZE 2			// MSS option value
#define OPT$MAX_RECV_DATASIZE 1392 		// MSS default value


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

	NetTCPIP

Facility:

	A system wide definition file.  This file describes the various
	structures and protocol headers used in the protocols of the
	TCP/IP suite.

Author:

	Writen by Bruce R. Miller, borrowing from IP.BLI and Structure.REQ

Modification History:

1.2  11-Nov-1991	Henry W. Miller		USBR
	Changed SH$ACK and SH$SEG to unsigned long as per RFC1122.

1.1c 29-Mar-1991	Henry W. Miller		USBR
	Defined IPH$Fragmentation_Data.

1.1b 07-Mar-1991	Henry W. Miller		USBR
	Added additional ICMP parameters, including RFC1122.

1.1a 13-Jan-1991	Henry W. Miller		USBR
	Make ICMPTTL and IPTTL configurable variables.

1.1  11-Nov-1990	Bruce R. Miller		CMU NetDev
	Added all the port definitions from RFC #1060 (Assigned Numbers).

*/

// not yet #include <cmuip/central/netinet/netxport> // Get the BLISS field def. library.

    // Define IP-based protocols
#define    ICMP_Protocol        1    // ICMP protocol code
#define    TCP_Protocol         6    // TCP protocol
#define    UDP_Protocol         17   // UDP protocol code

#define    TCP_PORT_TCPMUX	     1	// TCP Port Service Multiplexer
#define    TCP_PORT_RJE	     5	// Remote Job Entry
#define    TCP_PORT_ECHO	     7	// Echo
#define    TCP_PORT_DISCARD	     9	// Discard
#define    TCP_PORT_USERS	    11	// Active Users
#define    TCP_PORT_DAYTIME	    13	// Daytime
#define    TCP_PORT_QUOTE	    17	// Quote of the Day
#define    TCP_PORT_CHARGEN	    19	// Character Generator
#define    TCP_PORT_FTP_DATA	    20	// File Transfer [Default Data]
#define    TCP_PORT_FTP	    21	// File Transfer [Control]
#define    TCP_PORT_TELNET	    23	// Telnet
#define    TCP_PORT_SMTP	    25	// Simple Mail Transfer
#define    TCP_PORT_NSW_FE	    27	// NSW User System FE
#define    TCP_PORT_MSG_ICP	    29	// MSG ICP
#define    TCP_PORT_MSG_AUTH	    31	// MSG Authentication
#define    TCP_PORT_DSP	    33	// Display Support Protocol
#define    TCP_PORT_Prv_PS	    35	// private printer server
#define    TCP_PORT_TIME	    37	// Time
#define    TCP_PORT_RLP	    39	// Resource Location Protocol
#define    TCP_PORT_GRAPHICS	    41	// Graphics
#define    TCP_PORT_NAMESERVER	    42	// Host Name Server
#define    TCP_PORT_NICNAME	    43	// Who Is
#define    TCP_PORT_MPM_FLAGS	    44	// MPM FLAGS Protocol
#define    TCP_PORT_MPM	    45	// Message Processing Module [recv]
#define    TCP_PORT_MPM_SND	    46	// MPM [default send]
#define    TCP_PORT_NI_FTP	    47	// NI FTP
#define    TCP_PORT_LOGIN	    49	// Login Host Protocol
#define    TCP_PORT_LA_MAINT	    51	// IMP Logical Address Maintenance
#define    TCP_PORT_DOMAIN	    53	// Domain Name Server
#define    TCP_PORT_ISI_GL	    55	// ISI Graphics Language
#define    TCP_PORT_Prv_TA	    57	// private terminal access
#define    TCP_PORT_Prv_FS	    59	// private file service
#define    TCP_PORT_NI_MAIL	    61	// NI MAIL
#define    TCP_PORT_VIA_FTP	    63	// VIA Systems - FTP
#define    TCP_PORT_TACACS_DS	    65	// TACACS-Database Service
#define    TCP_PORT_BOOTPS	    67	// Bootstrap Protocol Server
#define    TCP_PORT_BOOTPC	    68	// Bootstrap Protocol Client
#define    TCP_PORT_TFTP	    69	// Trivial File Transfer
#define    TCP_PORT_NETRJS_1	    71	// Remote Job Service
#define    TCP_PORT_NETRJS_2	    72	// Remote Job Service
#define    TCP_PORT_NETRJS_3	    73	// Remote Job Service
#define    TCP_PORT_NETRJS_4	    74	// Remote Job Service
#define    TCP_PORT_Prv_DOS	    75	// any private dial out service
#define    TCP_PORT_Prv_RJE	    77	// any private RJE service
#define    TCP_PORT_FINGER	    79	// Finger
#define    TCP_PORT_HOSTS2_NS	    81	// HOSTS2 Name Server
#define    TCP_PORT_MIT_ML_DEV1    83	// MIT ML Device
#define    TCP_PORT_MIT_ML_DEV2    85	// MIT ML Device
#define    TCP_PORT_Prv_TL	    87	// private terminal link
#define    TCP_PORT_SU_MIT_TG	    89	// SU/MIT Telnet Gateway
#define    TCP_PORT_MIT_DOV	    91	// MIT Dover Spooler
#define    TCP_PORT_DCP	    93	// Device Control Protocol
#define    TCP_PORT_SUPDUP	    95	// SUPDUP
#define    TCP_PORT_SWIFT_RVF	    97	// Swift Remote Vitural File Protocol
#define    TCP_PORT_TACNEWS	    98	// TAC News
#define    TCP_PORT_METAGRAM	    99	// Metagram Relay
#define    TCP_PORT_HOSTNAME	   101	// NIC Host Name Server
#define    TCP_PORT_ISO_TSAP	   102	// ISO-TSAP
#define    TCP_PORT_X400	   103	// X400
#define    TCP_PORT_X400_SND	   104	// X400-SND
#define    TCP_PORT_CSNET_NS	   105	// Mailbox Name Nameserver
#define    TCP_PORT_RTELNET	   107	// Remote Telnet Service
#define    TCP_PORT_POP2	   109	// Post Office Protocol - Version 2
#define    TCP_PORT_POP3	   110	// Post Office Protocol - Version 3
#define    TCP_PORT_SUNRPC	   111	// SUN Remote Procedure Call
#define    TCP_PORT_AUTH	   113	// Authentication Service
#define    TCP_PORT_SFTP	   115	// Simple File Transfer Protocol
#define    TCP_PORT_UUCP_PATH	   117	// UUCP Path Service
#define    TCP_PORT_NNTP	   119	// Network News Transfer Protocol
#define    TCP_PORT_ERPC	   121	// Encore Expedited Remote Proc. Call
#define    TCP_PORT_NTP	   123	// Network Time Protocol
#define    TCP_PORT_LOCUS_MAP	   125	// Locus PC-Interface Net Map Server
#define    TCP_PORT_LOCUS_CON	   127	// Locus PC-Interface Conn Server
#define    TCP_PORT_PWDGEN	   129	// Password Generator Protocol
#define    TCP_PORT_CISCO_FNA	   130	// CISCO FNATIVE
#define    TCP_PORT_CISCO_TNA	   131	// CISCO TNATIVE
#define    TCP_PORT_CISCO_SYS	   132	// CISCO SYSMAINT
#define    TCP_PORT_STATSRV	   133	// Statistics Service
#define    TCP_PORT_INGRES_NET	   134	// INGRES-NET Service
#define    TCP_PORT_LOC_SRV	   135	// Location Service
#define    TCP_PORT_PROFILE	   136	// PROFILE Naming System
#define    TCP_PORT_NETBIOS_NS	   137	// NETBIOS Name Service
#define    TCP_PORT_NETBIOS_DGM   138	// NETBIOS Datagram Service
#define    TCP_PORT_NETBIOS_SSN   139	// NETBIOS Session Service
#define    TCP_PORT_EMFIS_DATA	   140	// EMFIS Data Service
#define    TCP_PORT_EMFIS_CNTL	   141	// EMFIS Control Service
#define    TCP_PORT_BL_IDM	   142	// Britton-Lee IDM
#define    TCP_PORT_IMAP2	   143	// Interim Mail Access Protocol v2
#define    TCP_PORT_NEWS	   144	// NewS
#define    TCP_PORT_UAAC	   145	// UAAC Protocol
#define    TCP_PORT_ISO_TP0	   146	// ISO-IP0
#define    TCP_PORT_ISO_IP	   147	// ISO-IP
#define    TCP_PORT_CRONUS	   148	// CRONUS_SUPPORT
#define    TCP_PORT_AED_512	   149	// AED 512 Emulation Service
#define    TCP_PORT_SQL_NET	   150	// SQL-NET
#define    TCP_PORT_HEMS	   151	// HEMS
#define    TCP_PORT_BFTP	   152	// Background File Transfer Program
#define    TCP_PORT_SGMP	   153	// SGMP
#define    TCP_PORT_NETSC_PROD	   154	// NETSC
#define    TCP_PORT_NETSC_DEV	   155	// NETSC
#define    TCP_PORT_SQLSRV	   156	// SQL Service
#define    TCP_PORT_KNET_CMP	   157	// KNET/VM Command/Message Protocol
#define    TCP_PORT_PCMail_SRV	   158	// PCMail Server
#define    TCP_PORT_NSS_Routing   159	// NSS-Routing
#define    TCP_PORT_SGMP_TRAPS	   160	// SGMP-TRAPS
#define    TCP_PORT_SNMP	   161	// SNMP
#define    TCP_PORT_SNMPTRAP	   162	// SNMPTRAP
#define    TCP_PORT_CMIP_Manage   163	// CMIP/TCP Manager
#define    TCP_PORT_CMIP_Agent	   164	// CMIP/TCP Agent
#define    TCP_PORT_XNS_Courier   165	// Xerox
#define    TCP_PORT_S_Net	   166	// Sirius Systems
#define    TCP_PORT_NAMP	   167	// NAMP
#define    TCP_PORT_RSVD	   168	// RSVD
#define    TCP_PORT_SEND	   169	// SEND
#define    TCP_PORT_Print_SRV	   170	// Network PostScript
#define    TCP_PORT_Multiplex	   171	// Network Innovations Multiplex
#define    TCP_PORT_CL_1	   172	// Network Innovations CL/1
#define    TCP_PORT_Xyplex_MUX	   173	// Xyplex
#define    TCP_PORT_MAILQ	   174	// MAILQ
#define    TCP_PORT_VMNET	   175	// VMNET
#define    TCP_PORT_GENRAD_MUX	   176	// GENRAD-MUX
#define    TCP_PORT_XDMCP	   177	// X Display Manager Control Protocol
#define    TCP_PORT_NextStep	   178	// NextStep Window Server
#define    TCP_PORT_BGP	   179	// Border Gateway Protocol
#define    TCP_PORT_RIS	   180	// Intergraph
#define    TCP_PORT_Unify	   181	// Unify
#define    TCP_PORT_Unisys_Cam	   182	// Unisys-Cam
#define    TCP_PORT_OCBinder	   183	// OCBinder
#define    TCP_PORT_OCServer	   184	// OCServer
#define    TCP_PORT_Remote_KIS	   185	// Remote-KIS
#define    TCP_PORT_KIS	   186	// KIS Protocol
#define    TCP_PORT_ACI	   187	// Application Communication Interface
#define    TCP_PORT_MUMPS	   188	// MUMPS
#define    TCP_PORT_QFT	   189	// Queued File Transport
#define    TCP_PORT_GACP	   190	// Gateway Access Control Protocol
#define    TCP_PORT_Prospero	   191	// Prospero
#define    TCP_PORT_OSU_NMS	   192	// OSU Network Monitoring System
#define    TCP_PORT_SRMP	   193	// Spider Remote Monitoring Protocol
#define    TCP_PORT_IRC	   194	// Internet Relay Chat Protocol
#define    TCP_PORT_DN6_NLM_AUD   195	// DNSIX Network Level Module Audit
#define    TCP_PORT_DN6_SMM_RED   196	// DNSIX Session Mgt Module Audit Redirect
#define    TCP_PORT_DLS	   197	// Directory Location Service
#define    TCP_PORT_DLS_Mon	   198	// Directory Location Service Monitor
#define    TCP_PORT_AT_RMTP	   201	// AppleTalk Routing Maintenance
#define    TCP_PORT_AT_NBP	   202	// AppleTalk Name Binding
#define    TCP_PORT_AT_3	   203	// AppleTalk Unused
#define    TCP_PORT_AT_ECHO	   204	// AppleTalk Echo
#define    TCP_PORT_AT_5	   205	// AppleTalk Unused
#define    TCP_PORT_AT_ZIS	   206	// AppleTalk Zone Information
#define    TCP_PORT_AT_7	   207	// AppleTalk Unused
#define    TCP_PORT_AT_8	   208	// AppleTalk Unused
#define    TCP_PORT_SUR_MEAS	   243	// Survey Measurement
#define    TCP_PORT_LINK	   245	// LINK
#define    TCP_PORT_DSP3270	   246	// Display Systems Protocol


    // UDP ports (RFC1060)
#define    UDP_PORT_ECHO	     7	// Echo
#define    UDP_PORT_DISCARD	     9	// Discard
#define    UDP_PORT_USERS	    11	// Active Users
#define    UDP_PORT_DAYTIME	    13	// Daytime
#define    UDP_PORT_QUOTE	    17	// Quote of the Day
#define    UDP_PORT_CHARGEN	    19	// Character Generator
#define    UDP_PORT_TIME	    37	// Time
#define    UDP_PORT_NAMESERVER	    42	// Host Name Server
#define    UDP_PORT_NICNAME	    43	// Who Is
#define    UDP_PORT_DOMAIN	    53	// Domain Name Server
#define    UDP_PORT_BOOTPS	    67	// Bootstrap Protocol Server
#define    UDP_PORT_BOOTPC	    68	// Bootstrap Protocol Client
#define    UDP_PORT_TFTP	    69	// Trivial File Transfer
#define    UDP_PORT_FINGER	    79	// Finger
#define    UDP_PORT_SUNRPC	   111	// SUN Remote Procedure Call
#define    UDP_PORT_AUTH	   113	// Authentication Service
#define    UDP_PORT_SFTP	   115	// Simple File Transfer Protocol
#define    UDP_PORT_NTP	   123	// Network Time Protocol
#define    UDP_PORT_PWDGEN	   129	// Password Generator Protocol
#define    UDP_PORT_LOC_SRV	   135	// Location Service
#define    UDP_PORT_SNMP	   161	// SNMP
#define    UDP_PORT_SNMPTRAP	   162	// SNMPTRAP
#define    UDP_PORT_CMIP_Manage   163	// CMIP/TCP Manager
#define    UDP_PORT_CMIP_Agent	   164	// CMIP/TCP Agent
#define    UDP_PORT_BGP	   179	// Border Gateway Protocol
#define    UDP_PORT_IRC	   194	// Internet Relay Chat Protocol

    // Define more UDP ports
#define    UDP_PORT_NFS 		 2049



//SBTTL 'IP header structure definition'

// This section defines the Internet Protcol (IP) header.  This protocol
// header is placed in front of the protocol and data from higher layers.

struct ip_header
  {
    union {
      unsigned char     iph$type_service		;
      unsigned       iph$swap_ihl		 : 4;  // check
    };
    unsigned char     iph$total_length		[2];
    unsigned char     iph$ident			[2];
    union {
      unsigned char     iph$fragmentation_data	[2];
      struct {
	unsigned       iph$fragment_offset	 : 13;
	union {
	  unsigned       iph$flags		 	 : 3;
	  struct {
	    unsigned           iph$mf		 : 1;	// More fragments flag
	    unsigned           iph$df		 : 1;	// Don't fragment
	    unsigned           iph$unused		 : 1;	// Unused bit
	  };
	};
      };
    };
    unsigned char     iph$ttl			;
    unsigned char     iph$protocol		;
    unsigned char     iph$checksum		[2];
    unsigned char     iph$source			[4];
    unsigned char     iph$dest			[4];
  };

#define    IP_SIZE   sizeof(struct IP_Header)

#define    IP_HDR_SHWD_SIZE	 10	//IP header size in 16 bit words
#define    IP_HDR_SWAP_SIZE	 6	//Number of words of IP header to byteswap
#define    IP_HDR_WD_SIZE	 5	//IP header size in 32 bit words
#define    IP_VERSION		 4	//IP version number from DoD spec
//#define    IPTTL  60			// Time-to-live
#define    IPTOS  0			// Type of service
#define    IPDF  1			// Don't fragment flag

// Max bytesize of IP header with options.
// *** WARNING ***
//	MUST be a multiple of 4.

#define    IP_HDR_BYTE_SIZE   20

// Device dependent header (bytes).  Currently covers NSC hyperchannel
// message-proper & interlan ethernet.
// *** WARNING ***
//	MUST be a multiple of 4.
//////HACK////// What a hack//////  Get rid of this.  Calculate it at run time//
#define    DEVICE_HEADER	 68



//SBTTL 'ICMP message definitions'    

// ICMP message types
#define    ICM_EREPLY  0		// Echo reply
#define    ICM_DUNREACH  3		// Destination Unreachable
#define	ICM_DUNR_NET  0	// (network unreachable)
#define	ICM_DUNR_HOST  1	// (host unreachable)
#define	ICM_DUNR_PROTOCOL  2	// (protocol unreachable)
#define	ICM_DUNR_PORT  3	// (port unreachable)
#define	ICM_DUNR_FRAGMENT  4	// (fragmentation needed)
#define	ICM_DUNR_SRCROUTE  5	// (source route failed)
#define	ICM_DUNR_NET_UNK  6	// (network unknown)
#define	ICM_DUNR_HOST_UNK  7	// (host unknown)
#define	ICM_DUNR_HOST_ISO  8	// (host isolated)
#define	ICM_DUNR_NET_FORBID  9	// (network forbidden)
#define	ICM_DUNR_HOST_FORBID  10	// (host forbiden)
#define	ICM_DUNR_NET_TOS  11		// (net unavailable for TOS)
#define	ICM_DUNR_HOST_TOS  12		// (host unavailable for TOS)
#define    ICM_SQUENCH  4
#define    ICM_REDIRECT  5			// Redirect
#define	ICM_REDIRECT_NET  0		// (Redirect Network)
#define	ICM_REDIRECT_HOST  1		// (Redirect Host)
#define	ICM_REDIRECT_TOS_NET  2	// (Redirect TOS & Network)
#define	ICM_REDIRECT_TOS_HOST  3	// (Redirect TOS & Host)
#define    ICM_ECHO  8
#define    ICM_TEXCEED  11
#define	ICM_TEXCEED_INTRANS  0    // TTL exceeded while in transit
#define	ICM_TEXCEED_FRAG_REASS  1 // Fragment reassambly time exceeded
#define    ICM_PPROBLEM  12
#define    ICM_TSTAMP  13
#define    ICM_TSREPLY  14
#define    ICM_IREQUEST  15
#define    ICM_IREPLY  16
#define    ICM_AMREQUEST  17
#define    ICM_AMREPLY  18
#define    ICM_MIN  ICM_EREPLY
#define    ICM_MAX  ICM_AMREPLY

// This section defines the Internet Control Message Protcol (ICMP) header.

struct icmp_header
  {
    union {
      struct {
	unsigned char 	icm$type	;	// ICMP packet type
	unsigned char 	icm$code	;	// Type-specific code
	unsigned char 	icm$cksum	[2];	// Checksum of ICMP portion
      };
      unsigned char     icm$ext1	[4];	// Blanket for first longword.
    };

// Type-specific fields:
    union {
      unsigned char     icm$var	[4];	// Type-specific data

// Echo request and reply (0,8)
      struct {
	unsigned char icm$e_id[2];	// Echo - identifier
	unsigned char icm$e_seq[2];	// Echo - sequence number
      };

// Types with an unused VAR field (3,4,11)
      unsigned char icm$typ_unused[4];	// Unused.  Must be zero.

// Redirect messages (5)
      unsigned char icm$r_gwy[4];	// Redirect - new gateway address

// Parameter Problem messages (12)
      struct {
	unsigned char icm$p_ptr;
	unsigned char icm$p_unusd[3];
      };

// timestamp request and reply (13,14)

      struct {
	unsigned char icm$t_id[2];	// Timestamp - identifier
	unsigned char icm$t_seq[2];	// Timestamp - sequence number
      };

// Information request and reply (15,16)

      struct {
	unsigned char icm$i_id[2];	// Information - identifier
	unsigned char icm$i_seq[2];	// Information - sequence number
      };

// Information request and reply (17,18)

      struct {
	unsigned char icm$a_id[2];	// Address Mask - identifier
	unsigned char icm$a_seq[2];	// Address Mask - sequence number
      };
    };

    unsigned char     icm$data	[0];	// Type-specific additional data


  };

#define    ICMP_SIZE   sizeof(struct icmp_header)
#define    ICMP_HEADER_SIZE   ICMP_SIZE*4

#define    MAX_ICMP_DATA_SIZE  512	// Max ICMP data size
//    ICMPTTL = 60,		// Time-to-live
#define    ICMPTOS  0		// Type of service
#define    ICMPDF  1			// Don't fragment flag



//SBTTL 'UDP message definitions'    

// This section defines the User Datagram Protcol structures.

// Define the structure of a UDP packet (after IP header)

struct udpkt_structure
    {
unsigned char     up$source_port	[2]; // UDP source port
unsigned char     up$dest_port	[2]; // UDP destination port
unsigned char     up$length		[2]; // UDP length (including UDP header)
unsigned char     up$checksum		[2]; // UDP checksum (including pseudo-header)
unsigned char     up$data		[0];  // UDP data start
    };

#define    UDPKT_LENGTH  sizeof(struct udpkt_structure)
#define    UDP_HEADER_SIZE  UDPKT_LENGTH*4



//SBTTL 'TCP message definitions'    

// This section defines the Transmission Control Protcol structures.


//SBTTL 'TCP Segment definition.'

/*
 This definition covers the entire TCP segment structure which is sent
 to IP.  For field definitions see the DARPA TCP specification.


	=========================================================
	//							//
	+			TCP Header			+
	//							//
	---------------------------------------------------------
	//							//
	+			   Data				+
	//							//
	=========================================================

*/

struct segment_structure // check
{
  unsigned short     sh$source_port		;	// Source port
  unsigned short     sh$dest_port		;	// Destination port
  unsigned int     sh$seq			;	// Sequence number
  unsigned int     sh$ack			;	// ACK number
  union {
    unsigned short int     sh$control_flags;	// For Fast flag clear.
    struct {
      unsigned 	sh$data_offset		 : 4;
      union {
	unsigned 	sh$c_all_flags		 : 12;
	struct {
	  unsigned 	sh$c_fin		 : 1;	// FIN (close) control
	  unsigned 	sh$c_syn		 : 1;	// SYN (open) control
	  unsigned 	sh$c_rst		 : 1;	// RESET control
	  unsigned 	sh$c_eol		 : 1;	// PUSH control
	  unsigned 	sh$c_ack		 : 1;	// ACK control (ACK valid)
	  unsigned 	sh$c_urg		 : 1;	// URG control (URG valid)
	  unsigned 	sh$rsvrd		 : 6;	// Unused bits
	};
      };
    };
    struct {
      unsigned 	sh$bs_c_rsvrd1		 : 4;
      unsigned 	sh$bs_data_offset	 : 4;
      unsigned 	sh$bs_c_fin		 : 1;
      unsigned 	sh$bs_c_syn		 : 1;
      unsigned 	sh$bs_c_rst		 : 1;
      unsigned 	sh$bs_c_eol		 : 1;
      unsigned 	sh$bs_c_ack		 : 1;
      unsigned 	sh$bs_c_urg		 : 1;
      unsigned 	sh$bs_c_rsvrd2		 : 2;
    };
  };
  unsigned short int sh$window;	// Window beyond this seq #
  signed short int sh$checksum;	// Segment TCP checksum
  unsigned short int sh$urgent;	// Urgent pointer if URG set
  unsigned char     sh$data			[0];	// Start of segment data
};

// Segment TCP options

struct tcp$opt_block
{
  unsigned char     tcp$opt_kind	;	// Option type
  unsigned char     tcp$opt_length	;	// Option length
  union {
    unsigned char     tcp$opt_data	[0];	// Additional data for the option
    unsigned char       tcp$opt_dbyte	;	// One-byte option data
    unsigned short       tcp$opt_dword;	// Two-byte option data
    unsigned char       tcp$opt_dlong	[4];	// Four-byte option data
    };				//(others require harder processing)
};

#define    TCP$OPT_SIZE  sizeof(struct tcp$opt_block)

#define    TCP$OPT_KIND_EOL  0	// End of options list
#define    TCP$OPT_KIND_NOP  1	// No-OP option
#define    TCP$OPT_KIND_MSS  2	// Maximum-segment-size option

#define    TCP$OPT_LENGTH_MSS  4	// Expected length for MSS option


// Segment sizes in bytes. actually the following macro is used primarily for
// mapping attributes to a dynamic data segment via "ref" pointers.

#define    Seg_Allocation  sizeof(struct segment_structure)

// TCP Segment: header & data sizes.

#define    TCP_DATA_OFFSET  5			// in 32-bit words.
#define    TCP_HEADER_SIZE  TCP_DATA_OFFSET*4	// in bytes.
//    Default_Data_Size = 1392,			// default: max size of user
#define    DEFAULT_DATA_SIZE  536			// default: max size of user
						// data per segment.

// segment option parameters

#define   OPT$SEG_DATASIZE  2			// MSS option value
#define   OPT$MAX_RECV_DATASIZE  1392 		// MSS default value

