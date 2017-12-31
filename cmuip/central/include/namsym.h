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
// not yet #include 'CMUIP_SRC:[CENTRAL]NETXPORT';
#include <cmuip/central/include/netcommon.h>

// Define name resolver constants


#define     INASIZ   4
#define     STRLEN   128

// Define the well-known mailbox names

#if 0
MACRO
NAMRES_MBX = 'MBX$NAMRES' %,
IPACP_MBX = 'MBX$IPACP' %;
#endif

// Define the format of a mailbox IOSB

struct MBX$IOSB
{
    short     MI$STATUS	;	// Status of the transfer
    short     MI$COUNT	;	// Byte count
    long     MI$PID	;// Sending process ID
};

#define     MBX$IOSB_SIZE   sizeof (struct MBX$IOSB)
#if 0
MACRO
MBX$IOSB = BLOCK[MBX$IOSB_SIZE] FIELD(MBX$IOSB_FIELDS) %;
#endif

// Define the format of a mailbox message


#define     MSGMAX   512		// Max size mailbox message
#define     MBNMAX   10		// Max length of return mailbox name

struct MAIL$MSG
{
    short     MSG$MBXNLN	;	// Length of return mailbox name (0 if none)
    char    MSG$MBXNAM[MBNMAX]; // MBXnnn for return message
    char     MSG$DATA	[0];// Start of mailbox data
};

#define     MAIL$MSG_SIZE   sizeof (struct MAIL$MSG)
#define     MAIL$MSG_BLEN   2 + MBNMAX
#define     MAX_MAIL$MSG_SIZE   (MSGMAX/4)
#if 0
MACRO
MAIL$MSG = BLOCK[MAIL$MSG_SIZE] FIELD(MAIL$MSG_FIELDS) %,
     MAX_MAIL$MSG = BLOCK[MAX_MAIL$MSG_SIZE] FIELD(MAIL$MSG_FIELDS) %;
#endif

// Define the different types of requests and their replies.


#define     NLRQ$CONTROL   0		// Name resolver control message
#define     NLRQ$NMLOOK   1		// Name-lookup
#define     NLRQ$ADLOOK   2		// Address-lookup
#define     NLRQ$RRLOOK   3		// Address-lookup
#define     NLRQ$PRUNE   4		// Address-lookup
#define     NLRQ$MIN   NLRQ$CONTROL	// Minimum request type
#define     NLRQ$MAX   NLRQ$PRUNE	// Max request type

#define     NLRP$CONTROL   NLRQ$CONTROL
#define     NLRP$NMLOOK   NLRQ$NMLOOK
#define     NLRP$ADLOOK   NLRQ$ADLOOK
#define     NLRP$RRLOOK   NLRQ$RRLOOK
#define     NLRP$PRUNE   NLRQ$PRUNE
#define     NLRP$ERROR   -1		// Message type for error
#define     NLRP$MIN   NLRP$ERROR
#define     NLRP$MAX   NLRP$PRUNE

// Define the common portions of the message

struct RQ$DEFAULT
{
    short     RQ$TYPE	;	// Request type
    unsigned short     RQ$ID	;// Request identifier
};

#define     RQ$DEFAULT_SIZE   sizeof (struct RQ$DEFAULT)
#if 0
MACRO
RQ$DEFAULT = BLOCK[RQ$DEFAULT_SIZE] FIELD(RQ$DEFAULT_FIELDS) %;
#endif

struct RPLY$DEFAULT
{
    short     RPLY$TYPE	;	// Reply type (matches request type)
    unsigned short     RPLY$ID	;// Reply ID
};

#define     RPLY$DEFAULT_SIZE   sizeof (struct RPLY$DEFAULT)
#if 0
MACRO
RPLY$DEFAULT = BLOCK[RPLY$DEFAULT_SIZE] FIELD(RPLY$DEFAULT_FIELDS) %;
#endif

// Define message format for resolver control messages

struct RQ$CONTROL
{
    short     RQCN$TYPE	;	// Request type
    unsigned short     RQCN$ID	;	// Request ID
    unsigned short     RQCN$CCODE	;	// Control code
    long     RQCN$CVALUE	;// Control value
};

#define     RQ$CONTROL_SIZE   sizeof (struct RQ$CONTROL)
#define     RQ$CONTROL_BLEN   10
#if 0
MACRO
RQ$CONTROL = BLOCK[RQ$CONTROL_SIZE] FIELD(RQ$CONTROL_FIELDS) %;
#endif

struct RPLY$CONTROL
{
    short     RPCN$TYPE	;	// Reply type
    unsigned short     RPCN$ID	;	// Reply ID
    long     RPCN$STATUS	;// Return status from operation
};

#define     RPLY$CONTROL_SIZE   sizeof (struct RPLY$CONTROL)
#define     RPLY$CONTROL_BLEN   8
#if 0
MACRO
RPLY$CONTROL = BLOCK[RPLY$CONTROL_SIZE] FIELD(RPLY$CONTROL_FIELDS) %;
#endif

#define     CNRQ$LOG   1		// Set resolver log parameter
#define     CNRQ$EXIT   2		// Request resolver exit
#define     CNRQ$INIT   3		// Request resolver reinitialize
#define     CNRQ$START   4		// NAMRES started (sent to IPACP)
#define     CNRQ$STOP   5		// NAMRES stopping (sent to IPACP)
#define     CNRQ$DUMP   6		// Dump DDB to NamRes log.
#define     CNRQ$MIN   CNRQ$LOG
#define     CNRQ$MAX   CNRQ$DUMP


// Define mesasge format for error messages (reply only)

struct RPLY$ERROR
{
    short     RPER$TYPE	;	// Reply type
    unsigned short     RPER$ID	;	// Reply ID
    long     RPER$ECODE	;// Error code
};

#define     RPLY$ERROR_SIZE   sizeof (struct RPLY$ERROR)
#define     RPLY$ERROR_BLEN   RPLY$ERROR_SIZE*4
#if 0
MACRO
RPLY$ERROR = BLOCK[RPLY$ERROR_SIZE] FIELD(RPLY$ERROR_FIELDS) %;
#endif

// Name lookup request

struct RQ$NMLOOK
{
    short     RQNM$TYPE	;	// Request type
    unsigned short     RQNM$ID	;	// Request identifier
    short     RQNM$NAMLEN	;	// Length of the name to lookup
    char     RQNM$NAMSTR	[0];// Name string
};

#define     RQ$NMLOOK_SIZE   sizeof (struct RQ$NMLOOK)
#define     RQ$NMLOOK_BLEN   6
#if 0
MACRO
RQ$NMLOOK = BLOCK[RQ$NMLOOK_SIZE] FIELD(RQ$NMLOOK_FIELDS) %;
#endif

struct RPLY$NMLOOK
{
    short     RPNM$TYPE	;	// Reply type
    unsigned short     RPNM$ID	;	// Reply ID
    short     RPNM$ADRCNT	;	// Count of addresses
    char    RPNM$ADRLST[4*MAX_HADDRS]; // Address list
    short     RPNM$NAMLEN	;	// Length of official name
    char     RPNM$NAMSTR	[0];// Official name string
};

#define     RPLY$NMLOOK_SIZE   sizeof (struct RPLY$NMLOOK)
#define     RPLY$NMLOOK_BLEN   6+(4*MAX_HADDRS)+2
#if 0
MACRO
RPLY$NMLOOK = BLOCK[RPLY$NMLOOK_SIZE] FIELD(RPLY$NMLOOK_FIELDS) %;
#endif

// Address lookup request

struct RQ$ADLOOK
{
    short     RQAD$TYPE	;	// Request type
    unsigned short     RQAD$ID	;	// Request identifier
    long     RQAD$ADDR	;// IP address to lookup
};

#define     RQ$ADLOOK_SIZE   sizeof (struct RQ$ADLOOK)
#define     RQ$ADLOOK_BLEN   8
#if 0
MACRO
RQ$ADLOOK = BLOCK[RQ$ADLOOK_SIZE] FIELD(RQ$ADLOOK_FIELDS) %;
#endif

struct RPLY$ADLOOK
{
    short     RPAD$TYPE	;	// Reply type
    unsigned short     RPAD$ID	;	// Reply identifier
    short     RPAD$NAMLEN	;	// Name length
    char     RPAD$NAMSTR	[0];// Name string
};

#define     RPLY$ADLOOK_SIZE   sizeof (struct RPLY$ADLOOK)
#define     RPLY$ADLOOK_BLEN   6
#if 0
MACRO
RPLY$ADLOOK = BLOCK[RPLY$ADLOOK_SIZE] FIELD(RPLY$ADLOOK_FIELDS) %;
#endif

// Resource Record lookup request

struct RQ$RRLOOK
{
    short     RQRR$TYPE	;	// Request type
    unsigned short     RQRR$ID	;	// Request identifier
    unsigned short     RQRR$RRTYPE	;
    short     RQRR$NAMLEN	;	// Length of the name to lookup
    char     RQRR$NAMSTR	[0];// Name string
};

#define     RQ$RRLOOK_SIZE   sizeof (struct RQ$RRLOOK)
#define     RQ$RRLOOK_BLEN   6
#if 0
MACRO
RQ$RRLOOK = BLOCK[RQ$RRLOOK_SIZE] FIELD(RQ$RRLOOK_FIELDS) %;
#endif

struct RPLY$RRLOOK
{
    short     RPRR$TYPE	;	// Reply type
    unsigned short     RPRR$ID	;	// Reply ID
    short     RPRR$RDLEN	;	// RR size
    //  short     RPRR$NAMLEN	;	// Length of the name to lookup
    char     RPRR$RDATA	[0];// Address list
};

#define     RPLY$RRLOOK_SIZE   sizeof (struct RPLY$RRLOOK )
#define     RPLY$RRLOOK_BLEN   6
#if 0
MACRO
RPLY$RRLOOK = BLOCK[RPLY$RRLOOK_SIZE] FIELD(RPLY$RRLOOK_FIELDS) %;
#endif

// Prune request

struct RQ$PRUNE
{
    short     RQPR$TYPE	;	// Request type
    unsigned short     RQPR$ID	;	// Request identifier
    short     RQPR$NAMLEN	;	// Length of the name to lookup
    char     RQPR$NAMSTR	[0];// Name string
};

#define     RQ$PRUNE_SIZE   sizeof (struct RQ$PRUNE)
#define     RQ$PRUNE_BLEN   6
#if 0
MACRO
RQ$PRUNE = BLOCK[RQ$PRUNE_SIZE] FIELD(RQ$PRUNE_FIELDS) %;
#endif

struct RPLY$PRUNE
{
    short     RPPR$TYPE	;	// Reply type
    unsigned short     RPPR$ID	;// Reply ID
};

#define     RPLY$PRUNE_SIZE   sizeof (struct RPLY$PRUNE)
#define     RPLY$PRUNE_BLEN   4
#if 0
MACRO
RPLY$PRUNE = BLOCK[RPLY$PRUNE_SIZE] FIELD(RPLY$PRUNE_FIELDS) %;
#endif

