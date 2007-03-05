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
/* Define Network User Interface. */

/* LIBRARY 'CMUIP_SRC:[CENTRAL]NETCOMMON';	! Common defs. */

/* Define IP QIO function Codes. */
/* Use predefined codes which are meaningful only to IP. */

#define IP$OPEN		= IO$_Create
#define IP$CLOSE	= IO$_Delete
#define IP$SEND		= IO$_WriteVBlk
#define IP$RECEIVE	= IO$_ReadVBlk
#define IP$ABORT	= IO$_Deaccess
#define IP$STATUS	= IO$_ACPcontrol
#define IP$INFO		= IO$_Modify

#define IP$GTHST IO$_SKIPFILE
#define GTH_LCLHST 0	/* Local host information */
#define GTH_NAMADR 1	/* Name to address translation */
#define GTH_ADRNAM 2	/* Address to name translation */
#define GTH_RRLOOK 3	/* Address to name translation */

/* OPEN modes */

#define Active	1	/* IP - Connection is ACTIVE */
#define Passive	0	/* IP - Connection is PASSIVE */
#define UDPData	1	/* UDP - UDP data only (must match above) */
#define UDPAddr	0	/* UDP - IP address supplied (must match above) */
#define WILD	0	/* Wild port specifier. */
#define Asynch	1	/* $QIO's */
#define Synch	0	/* $QIOW's */

/* Open flag bit positions ** Must match STRUCTURE and IPDRIVER ** */

#define OPF$Mode	1
#define OPF$Nowait	2
#define OPF$Addrfla	4

/* Send flag bit positions ** Must match STRUCTURE and IPDRIVER ** */
/*!!!HACK!!! Not used...*/
#define SND$EOLFlag	1
#define SND$UrgentFlag	2

/* IPSend flag bit positions ** Must match STRUCTURE and IPDRIVER ** */

#define SND$Exact	1
#define SND$NoCkSum	2

/* TCB Connection States */

#define CS$Closed	 0	/* (not a real state) */
#define CS$Listen	 1	/* Waiting for SYN */
#define CS$SYN_Sent	 2	/* SYN sent, waiting for SYN-ACK */
#define CS$SYN_RECV	 3	/* SYN received & sent, waiting for ACK */
#define CS$Established	 4	/* Connection ready to send/receive data */
#define CS$FIN_Wait_1	 5	/* CLOSE done, FIN sent, waiting for ACK */
#define CS$FIN_Wait_2	 6	/* ACK of FIN received, waiting for FIN */
#define CS$Time_Wait	 7	/* FINs sent,received,ACKed */
#define CS$Close_Wait	 8	/* FIN received, waiting for user CLOSE */
#define CS$Closing	 9	/* FINs sent&received, waiting for ACK * */
#define CS$Last_ACK	10	/* FINs sent&received, waiting for ACK ** */
#define CS$Reset	11	/* (not a TCP state) */
#define CS$Inactive	12	/* (not a TCP state) Connection is closed */
#define CS$NameLook	13	/* (not a TCP state) Waiting for name lookup */

/* State only reached via FIN-WAIT-1 (local initiation of close)    */
/** State only reached via CLOSE-WAIT (remote initiation of close)  */
