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
#define ICMP_Protocol	 1
#define TCP_Protocol	 6
#define UDP_Protocol	17


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

