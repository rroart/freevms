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
//TITLE "Input/Output Utilities"
//++
//
// Module:
//
//	IOUTIL
//
// Facility:
//
//	Input and Output utility routines
//
// Abstract:
//
//	Provides a standard library of routines for input and output of
//	addresses as decimal and hexidecimal strings.
//
// Author:
//
//	Vince Fuller, CMU-CSD, April 1986
//	Copyright (c) 1986,1987, Vince Fuller and Carnegie-Mellon University
//
// Modification history:
//
// 2.2	7-Jan-1992	John Clement
//	Added LOG$Flush for immediate output
//
// 2.1	24-Dec-1991	Henry W. Miller		USBR
//	In LOG_OUTPUT and ACT_OUTPUT, use configurable variables LOG_THRESHOLD
//	and ACT_THRESHOLD to decide when to $FLUSH.  After all, we are
//	having a drought.
//
// 2.0D	05-Aug-1991	Henry W. Miller		USBR
//	In LOG_OUTPUT and ACT_OUTPUT, only $FLUSH() when a threshold of
//	2048 bytes has been hit.  Should speed up logging considerably.
//	Please excuse disgusting imagery.
//
// 2.0C	09-Jul-1991	Henry W. Miller		USBR
//	Added LIB for VMS 5.4.
//
// 2.0B	25-Mar-1991	Henry W. Miller		USBR
//	In LOG_OPEN(), print out FAB or RAB STV if error.
//	Define INIT_DYNDESC macro locally - $INIT_DYNDESC not working for
//	some reason!!
//
// 2.0  20-Oct-1989	Bruce R. Miller		CMU NetDev
//	Added code for doing activity file logging by basically
//	duplicating the LOG file code.
//
// 1.9  19-Nov-87, Edit by VAF
//	Use new $ACPWAKE macro.
//
// 1.8  24-Mar-87, Edit by VAF
//	Call RESET_PROCNAME to reset process name in exit handler.
//
// 1.7  24-Feb-87, Edit by VAF
//	Move QL_FAO and message queue management routines in here.
//	Fix a couple of misuses of LOG_STATE.
//
// 1.6  17-Feb-87, Edit by VAF
//	Fix a bunch of problems with error logging.
//
// 1.5  16-Feb-87, Edit by VAF
//	Fix ERROR_FAO/FATAL_FAO to add time+date and EOL before writing to
//	the log file.
//
// 1.4   9-Feb-87, Edit by VAF
//	Flush Error_Processor. New error handling routines are ERROR_FAO and
//	FATAL_FAO. TCPMACROS updated to use these for references to old macros.
//
// 1.3   6-Feb-87, Edit by VAF
//	Change exiting message.
//
// 1.2  30-Sep-86, Edit by VAF
//	Have exception handler send message to opr in addition to logging it
//	to the log file.
//	Have error handler always append message to log file.
//	Have exit handler close log file if it is open.
//
// 1.1  30-May-86, Edit by VAF
//	Get rid of PRINT_MSG routine - make synonymous with OPR_FAO.
//
//--


#if 0
MODULE IOUTIL(IDENT="2.2",LANGUAGE(BLISS32),
	      ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			      NONEXTERNAL=LONG_RELATIVE),
	      LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	      OPTIMIZE,OPTLEVEL=3,ZIP)=
#endif

#include <starlet.h>
//LIBRARY "SYS$LIBRARY:LIB";			// JC
//not yet#include "CMUIP_SRC:[CENTRAL]NETXPORT";
#include <cmuip/central/include/netcommon.h>
#include "tcpmacros.h"
#include "structure.h"

#include "netvms.h"

#include <ssdef.h>
#include <descrip.h>
#include <fabdef.h>
#include <rabdef.h>
#include <opcdef.h>
#include <chfdef.h>

#include <stdarg.h>



#define APPCHR(CHR,DPTR,DCNT,OCNT) \
    if ((DCNT=DCNT-1) > 0) \
	{ \
	OCNT = OCNT+1; \
	CH$WCHAR_A(CHR,DPTR); \
	}

#define    INIT_DYNDESC(D) \
	{\
	D->dsc$w_length	= 0;\
	D->dsc$b_dtype	= DSC$K_DTYPE_T;\
	D->dsc$b_class	= DSC$K_CLASS_D;\
	D->dsc$a_pointer	= 0;\
	};

signed long
	act_threshold	 = 512,
	log_threshold	 = 512 ;


void APPEND_DEC(DPTR,DCNT,NUM,OUTCNT)

// Append a decimal value to a string
//   DPTR - Address of pointer to destination string (updated on return)
//   DCNT - Address of count of bytes remaining in destination string (updated)
//   NUM - Value to output
//   OUTCNT - Address of count of bytes output (updated)

    {
    signed long
	DIV,DIG,REM,VAL,FLAG;
    if (NUM == 0)
	{
	APPCHR('0',DPTR,DCNT,OUTCNT);
	return;
	};
    DIV = 1000000000;			// Highest pwr of 10 in 32 bits
    VAL = NUM;
    if (VAL < 0)
	{
	VAL = -1*VAL;
	APPCHR('-',DPTR,DCNT,OUTCNT);
	};
    FLAG = 0;
    while (DIV > 0)
	{
	DIG = VAL/DIV;
	REM = VAL % DIV;
	DIV = DIV/10;
	if ((DIG != 0) || (FLAG != 0))
	    {
	    FLAG = FLAG+1;
	    APPCHR(DIG+'0',DPTR,DCNT,OUTCNT);
	    };
	VAL = REM;
	};
    }

void ASCII_DEC_BYTES(DESC,COUNT,SOURCE,LEN)

// Write a string of decimal bytes to a string descriptor.

     struct dsc$descriptor * DESC;
    {
      signed long I,
	CPTR,CURBYTE,DPTR,DCNT,OUTCNT;
    OUTCNT = 0;
    CPTR = CH$PTR(SOURCE,0);
    DCNT = DESC->dsc$w_length;
    DPTR = CH$PTR(DESC->dsc$a_pointer,0);
    for (I=(COUNT-1);I>=0;I--)
	{
	CURBYTE = CH$RCHAR_A(CPTR);
	APPEND_DEC(DPTR,DCNT,CURBYTE,OUTCNT);
	if (I != 0)
	    APPCHR('.',DPTR,DCNT,OUTCNT);
	};
    if (LEN != 0)
	LEN = MIN(OUTCNT,DESC->dsc$w_length);
    }

APPEND_HEX(DPTR,DCNT,NUM,OUTCNT,SIZE)

// Append a hexidecimal value to a string

    {
      signed long I,
	DIG,VAL;
    VAL = ROT(NUM,(8-SIZE)*4); // Position first digit
    for (I=(SIZE-1);I>=0;I--)
	{
	VAL = ROT(VAL,4);	// Rotate highest order 4 bits to lowest
	DIG = VAL&0xf;	// Get the digit
	if (DIG <= 9)
	  DIG = '0'+DIG;
	else
	    DIG = 'A'+DIG-10;
	APPCHR(DIG,DPTR,DCNT,OUTCNT);
	}
    }

void ASCII_HEX_BYTES(DESC,COUNT,SOURCE,LEN)

// Write a string of hexidecimal bytes to a string descriptor.

     struct dsc$descriptor * DESC;
    {
      signed long I,
	CPTR,CURBYTE,DPTR,DCNT,OUTCNT;
    CPTR = CH$PTR(SOURCE,0);
    DCNT = DESC->dsc$w_length;
    DPTR = CH$PTR(DESC->dsc$a_pointer,0);
    OUTCNT = 0;
    for (I=(COUNT-1);I>=0;I--)
	{
	CURBYTE = CH$RCHAR_A(CPTR);
	APPEND_HEX(DPTR,DCNT,CURBYTE,OUTCNT,2);
	if (I != 0)
	    APPCHR('-',DPTR,DCNT,OUTCNT);
	};
    if (LEN != 0)
	LEN = MIN(OUTCNT,DESC->dsc$w_length);
    }


    GET_DEC_NUM();

GET_IP_ADDR(CPTR,VAL)

// Convert an text internet address (a.b.c.d) into binary form.
// CPTR contains the address of a pointer to the text.
// VAL is the address of where to put the value.
// Returns -1 on failure, or terminating character (GEQ 0)
// N.B. Assumes that Internet addresses are 4 bytes long.

     char ** CPTR;
    {
      signed long I,
    	DPTR,NVAL,CHR;
    DPTR = CH$PTR(VAL,0);
    for (I=3;I>=0;I--)
	{
	if ((CHR = GET_DEC_NUM(CPTR,&NVAL)) < 0)
	    return -1;
	CH$WCHAR_A(NVAL,DPTR);
	if (I != 0)
	    if (CH$RCHAR_A(*CPTR) != '.')
		return -1;
	};
    return CHR;
    }

GET_DEC_NUM(CPTR,VAL)

// Read a decimal number from a string into binary form.
// CPTR is the address of a string pointer to the numeric text.
// VAL is the address of where to put the value.
// Returns:
//   -1 on failure, first non-blank character is not numeric
//   >=0 on success, returning the terminating character.
// CPTR is updated to point at the terminating character.
// Currently only handles unsigned decimal values.

     char ** CPTR;
     int * VAL;
    {
    signed long
    	CHR,RVAL,LPTR;
    LPTR = *CPTR;
    do {
      CHR = CH$RCHAR_A(LPTR);
    } while (CHR == ' ');
    if ((CHR < '0') || (CHR > '9'))
	return -1;
    RVAL = 0;
    while ((CHR >= '0') && (CHR <= '9'))
	{
	RVAL = RVAL*10+(CHR-'0');
	*CPTR = LPTR;
	CHR = CH$RCHAR_A(LPTR);
	};
    *VAL = RVAL;
    return CHR;
    }


    GET_HEX_NUM();

GET_HEX_BYTES(NUMBYT,CPTR,DEST)

// Read a hexidecimal byte string.
// Returns -1 on failure, or terminating character. CPTR updated to point
// at the terminating character.
// Octets must be separated by the character "-"

    {
      signed long I,
	CVAL,
	LPTR,
	TCHR,
	DPTR;
    DPTR = CH$PTR(DEST,0);
    for (I=(NUMBYT-1);I>=0;I--)
	{
	if ((TCHR=GET_HEX_NUM(CPTR,CVAL)) < 0)
	    return -1;
	CH$WCHAR_A(CVAL,DPTR);
	if (I != 0)
	    if (CH$RCHAR_A(CPTR) != '-')
		return -1;
	};
    return TCHR;
    }

GET_HEX_NUM(INPTR,VAL)

// Read a hexidecimal number from a string into binary form.
// CPTR is the address of a string pointer to the numeric text.
// VAL is the address of where to put the value.
// Returns:
//   -1 on failure, first non-blank character is not numeric
//   >=0 on success, returning the terminating character.
// INPTR is updated to point to the termiating character.

    {
    signed long
    	CHR,RVAL,NCHR,CPTR;
    CPTR = INPTR;
    do {
      CHR = CH$RCHAR_A(CPTR);
    } while (CHR == ' ');
    RVAL = 0;
    NCHR = 0;
    while ((0 == 0))
	{
	signed long
	    CVAL;
	if ((CHR >= '0') && (CHR <= '9'))
	  CVAL = CHR-'0';
	else
	    if ((CHR >= 'a') && (CHR <= 'f'))
	      CVAL = CHR-'a'+10;
	    else
		if ((CHR >= 'A') && (CHR <= 'F'))
		  CVAL = CHR-'A'+10;
		else
		    break;
	NCHR = NCHR+1;
	RVAL = RVAL^4+VAL;
	INPTR = CPTR;
	CHR = CH$RCHAR_A(CPTR);
	};
    if (NCHR == 0)
	return -1;
    VAL = RVAL;
    return CHR;
    }


//SBTTL "Log file handling routines"

struct _fabdef    LOGFAB_ = {   fab$l_fna : "INET$LOG:",
				// not yet ; buggy gcc 		fab$b_fac : FAB$M_PUT,
				// not yet	fab$b_shr : FAB$M_GET,
				// not yet	fab$l_fop : (FAB$M_SQO),
				fab$b_rfm : FAB$C_STMLF,			// JC
				/* not yet fab$b_org : FAB$C_SEQ */ }, *LOGFAB=&LOGFAB_;
struct _rabdef    LOGRAB_ = { rab$l_fab: &LOGFAB_ }, *LOGRAB=&LOGRAB_;

signed long
    log_state  = 0;

#define    TRUE (0 == 0)
#define    FALSE (0 == 1)

 void    OPR_FAO();

LOG_OPEN (void)

// Open the log/trace file for debug & trace recording.
// Output: LOGFAB setup for stream output.
// Returns: TRUE if successfully opened.

    {
    signed long
	RC;
    RC = exe$create( LOGFAB);
    if (! RC)
	{
	OPR$FAO("Log file $CREATE failed, RC = !XL, STV = !XL",
	    RC, LOGFAB->fab$l_stv);
	return FALSE;
	};
    RC = exe$connect( LOGRAB);
    if (! RC)
	{
	OPR$FAO("Log file $CONNECT failed, RC = !XL, STV = !XL",
	    RC, LOGRAB->rab$l_stv);
	return FALSE;
	};
    return TRUE;
    }

LOG_CLOSE (void)
    {
    if (log_state != 0)
	{
	exe$disconnect( LOGRAB);
	exe$close( LOGFAB);
	return TRUE;
	}
    else
	return FALSE;
    }

 void    LOG_FAO();

void LOG_CHANGE(STATE)
    {
    if (STATE != 0)
	{			// He wants it open now
	if (log_state == 0)
	    {		// Not open - open it now
	    if (LOG_OPEN())
		{
		log_state = STATE;
		LOG$FAO("!%T Logging enabled!/",0);
		};
	    };
	LOG$FAO("!%T Log event mask set to !XL!/",0,STATE);
	}
    else
	{			// He wants it closed
	if (log_state != 0)
	    {		// It's open - close it now
	    LOG$FAO("!%T Logging disabled!/",0);
	    LOG_CLOSE();
	    log_state = STATE;	// Set new log state
	    };
	};
    }

void LOG_OUTPUT(OUTDESC)

// Output a string to the log file.
// OUTDESC is the address of a string descriptor.

	struct dsc$descriptor * OUTDESC;
    {
    static
	logcount=0 ;
    if (log_state != 0)
	{
	signed long
	    RC;

	LOGRAB->rab$w_rsz = OUTDESC->dsc$w_length;
	LOGRAB->rab$l_rbf = OUTDESC->dsc$a_pointer;
	logcount = logcount + OUTDESC->dsc$w_length ;

	RC = exe$put( LOGRAB);
//!!HACK!!// Take out this Flush!
	if (( 	(logcount > log_threshold)
	   ||	(log_state && LOG$FLUSH) ))	// JC
	 {
	    RC = exe$flush(LOGRAB);
	    logcount = 0 ;
	    } ;
	};

    }

void LOG_FAO(CSTR)

// Do output to log file using $FAO to format parameters.

    {
    signed long
      RC;
	DESC$STR_ALLOC(OUTDESC,1000);

    RC = exe$faol(CSTR,
	       OUTDESC->dsc$w_length,
	       OUTDESC,
		  /*AP+*/8);
    if (RC)
      LOG_OUTPUT(OUTDESC);
    else
	OPR$FAO("LOG_FAO failure, error code is !XL",RC);
    }

void LOG_Time_Stamp (void)

// Output to the LOG device/file the current time.  Char string ends
// in a space (no crlf).
// VMS specific code.
// Exit:	none

    {
    LOG$FAO("!%T ",0);
    }



//SBTTL "Activity file handling routines"

struct _fabdef    ACTFAB_ = { fab$l_fna : "INET$ACTIVITY:",
			      // not yet ; buggy gcc fab$b_fac : FAB$M_PUT,
			      // not yet fab$b_shr : FAB$M_GET,
			      // not yet fab$l_fop : (FAB$M_SQO),
			      /* not yet fab$b_org : FAB$C_SEQ*/}, *ACTFAB=&ACTFAB_;
struct _rabdef     ACTRAB_ = {rab$l_fab : &ACTFAB_}, *ACTRAB = &ACTRAB_;

signed long
    act_state  = 0;

ACT_OPEN (void)

// Open the activity file for event recording.
// Output: ACTFAB setup for stream output.
// Returns: TRUE if successfully opened.

    {
    signed long
	RC;
    RC = exe$create( ACTFAB);
    if (! RC)
	{
	OPR$FAO("Activity file $CREATE failed, RC = !XL",RC);
	return FALSE;
	};
    RC = exe$connect(ACTRAB);
    if (! RC)
	{
	OPR$FAO("Activity file $CONNECT failed, RC = !XL",RC);
	return FALSE;
	};
    return TRUE;
    }

ACT_CLOSE (void)
    {
    if (act_state != 0)
	{
	exe$disconnect(ACTRAB);
	exe$close(ACTFAB);
	return TRUE;
	}
    else
	return FALSE;
    }

 void    ACT_FAO();

void ACT_CHANGE(STATE)
    {
    if (STATE != 0)
	{			// e wants it open now
	if (act_state == 0)
	    {		// Not open - open it now
	    if (ACT_OPEN())
		{
		act_state = STATE;
		ACT$FAO("!%T Logging enabled!/",0);
		};
	    };
	ACT$FAO("!%T Log event mask set to !XL!/",0,STATE);
	}
    else
	{			// e wants it closed
	if (act_state != 0)
	    {		// It's open - close it now
	    ACT$FAO("!%T Logging disabled!/",0);
	    ACT_CLOSE();
	    act_state = STATE;	// Set new log state
	    };
	};
    }

void ACT_OUTPUT(OUTDESC)

// Output a string to the activity log file.
// OUTDESC is the address of a string descriptor.

	struct dsc$descriptor * OUTDESC;
    {
    static
	ACTCOUNT	 = 0 ;
    if (act_state != 0)
	{
	signed long
	    RC;
	ACTRAB->rab$w_rsz = OUTDESC->dsc$w_length;
	ACTRAB->rab$l_rbf = OUTDESC->dsc$a_pointer;
	ACTCOUNT = ACTCOUNT + OUTDESC->dsc$w_length ;

	RC = exe$put(ACTRAB);
	if ((ACTCOUNT > act_threshold))
	    {
	    RC = exe$flush(ACTRAB);
	    ACTCOUNT = 0 ;
	    } ;
	};
    }

void ACT_FAO(CSTR)

// Do output to activity log file using $FAO to format parameters.

    {
    signed long
      RC;
	DESC$STR_ALLOC(OUTDESC,1000);

    RC = exe$faol(CSTR,
	       OUTDESC->dsc$w_length,
	       OUTDESC,
		  /*AP+*/8);
    if (RC)
      ACT_OUTPUT(OUTDESC);
    else
	OPR$FAO("ACT_FAO failure, error code is !XL",RC);
    }

void ACT_Time_Stamp (void)

// Output to the activity log device/file the current time.  Char string ends
// in a space (no crlf).
// VMS specific code.
// Exit:	none

    {
    ACT$FAO("!%T ",0);
    }

//sbttl "Send messages to the Central VMS operator"
/*

Function:

	Send messages to the operators console & those terminals defined
	as operators.  used by network device interface code to tell the
	world about devices going offline etc.  Message sent to operator
	is prefixed with network name as derrived from "tcp$network_name"
	logical name.

Inputs:

	Text = address of mesage descriptor(vms string descriptor).

Implicit Inputs:

	MYname = initialized string-desc for network name.
Outputs:

	lbc (low bit clear) = success
	otherwise $sndopr error return.

Side Effects:

	operator terminals will receive the xmitted messages.
	if message_length > 128-size(tcp$network_name) then message will
	be truncated.
*/

send_2_operator(TEXT)
	struct dsc$descriptor * TEXT;
    {
extern struct dsc$descriptor *	myname;
    static
      Request_ID = 0;
#define	MAXCHR 1024
    signed long
	MSGLEN,
      PTR;
    struct dsc$descriptor *MSG;
	struct _opcdef * MSGBUF ;
    char *MSGTEXT = &MSGBUF->opc$l_ms_text;
    signed long
	NAMPTR = myname->dsc$a_pointer,
	NAMLEN = myname->dsc$w_length;

    MSGBUF->opc$b_ms_type = OPC$_RQ_RQST;
    MSGBUF->opc$b_ms_target = OPC$M_NM_CENTRL;
    MSGBUF->opc$l_ms_rqstid = Request_ID;
    Request_ID = Request_ID + 1;
    MSGLEN = TEXT->dsc$w_length;
    if (MSGLEN > MAXCHR)
	MSGLEN = MAXCHR;
    CH$MOVE(MSGLEN,TEXT->dsc$a_pointer,MSGTEXT);
    MSG->dsc$w_length = 8+MSGLEN;
    MSG->dsc$b_class = DSC$K_CLASS_Z;
    MSG->dsc$b_dtype = DSC$K_DTYPE_Z;
    MSG->dsc$a_pointer = MSGBUF;
    return exe$sndopr(MSG, 0);
    }

void OPR_FAO(CSTR) 

// Send a message to the VMS operator, using $FAO for output formatting.

    {
    signed long
      RC;
    DESC$STR_ALLOC(OUTDESC,1000);
	DESC$STR_ALLOC(OPRDESC,1000);

    RC = exe$faol(CSTR,
	        OUTDESC->dsc$w_length,
	       OUTDESC,
		  /*AP+*/8);
    if (! RC)
	exe$exit( RC);

// Reformat for console output

    RC = exe$fao(ASCID("IPACP: !AS"),OPRDESC->dsc$w_length,OPRDESC,OUTDESC);
    if (! RC)
	exe$exit( RC);
    send_2_operator(OPRDESC);
    }

signed long
    PRINT_MSG = OPR_FAO;	// Synonym for host table module to use

//SBTTL "Error processing routines - ERROR_FAO, FATAL_FAO"

void ERROR_FAO(CSTR)
//
// Send a message to the console & log the error (OPR_FAO + LOG_FAO)
//
    {
    signed long
	RC,
      OLDSTATE;
    DESC$STR_ALLOC(OUTDESC,1000);
    DESC$STR_ALLOC(OPRDESC,1000);
	DESC$STR_ALLOC(LOGDESC,1000);

// Format the message string

    RC = exe$faol(CSTR,
	       OUTDESC->dsc$w_length,
	       OUTDESC,
		  /*AP+*/8);
    if (! RC)
	{
	OPR$FAO("ERROR_FAO failure, RC = !XL",RC);
	exe$exit( RC);
	};

// Format and send message to the operator

    RC = exe$fao(ASCID("?IPACP: !AS"),OPRDESC->dsc$w_length,OPRDESC,OUTDESC);
    if (! RC)
	exe$exit( RC);
    send_2_operator(OPRDESC);

// Format the message for logging - add time+date and EOL

    RC = exe$fao(ASCID("!%T !AS!/"),LOGDESC->dsc$w_length,LOGDESC,0,OUTDESC);
    if (! RC)
	exe$exit( RC);

// Make sure we are logging something & log it

    OLDSTATE = log_state;
    LOG_CHANGE(0x80000000 || log_state);
    LOG_OUTPUT(LOGDESC);
    LOG_CHANGE(OLDSTATE);
    }


void FATAL_FAO(CSTR)
//
// Same as above, except also exit the ACP.
//
    {
    signed long
	RC,
      OLDSTATE;
    DESC$STR_ALLOC(OUTDESC,1000);
    DESC$STR_ALLOC(OPRDESC,1000);
	DESC$STR_ALLOC(LOGDESC,1000);

// Format the output string

    RC = exe$faol(CSTR,
	       OUTDESC->dsc$w_length,
	       OUTDESC,
		  /*AP+*/8);
    if (! RC)
	{
	OPR$FAO("FATAL_FAO failure, RC = !XL",RC);
	exe$exit( RC);
	};

// Format & send message to the operator

    RC = exe$fao(ASCID("?IPACP: !AS"),OPRDESC->dsc$w_length,OPRDESC,OUTDESC);
    if (! RC)
	exe$exit( RC);
    send_2_operator(OPRDESC);

// Format it for logging

    RC = exe$fao(ASCID("!%T !AS!/"),LOGDESC->dsc$w_length,LOGDESC,
	      0,OUTDESC);

// Make sure we are logging something & log it

    OLDSTATE = log_state;
    LOG_CHANGE(0x80000000 || log_state);
    LOG_OUTPUT(LOGDESC);
    LOG_CHANGE(OLDSTATE);
    exe$exit( SS$_ABORT);
    }

//SBTTL "Queued message processing"
/*
    In order to debug timing-related problems, it is often necessary to log
    information from within AST routines. However, the very act of doing this
    logging can alter timing significantly. To avoid this problem, AST routine
    log messages are queued and later written in non-AST context.
*/

struct QB$ERRMSG
{
void *     EMQ$NEXT	;	// Next item on queue
void *     EMQ$LAST	;	// Previous item on queue
long long    EMQ$MDSC	;	// Message descriptor
    };

#define    QB$ERRMSG_SIZE sizeof(struct QB$ERRMSG)
#if 0
MACRO
    QB$ERRMSG = BLOCK->QB$ERRMSG_SIZE FIELD(QB$ERRMSG_FIELDS) %;
#endif

struct QH$ERRHDR 
{
void *     EM$QHEAD	;
void *    EM$QTAIL;
    };

#define ERRHDR_SIZE sizeof(struct QH$ERRHDR)
#if 0
MACRO
    QH$ERRHDR = BLOCK->QH$ERRHDR_SIZE FIELD(QH$ERRHDR_FIELDS) %;
#endif

static struct QH$ERRHDR
    ERR_MSG_Q_ = { EM$QHEAD : &ERR_MSG_Q_,
		   EM$QTAIL: & ERR_MSG_Q_},*ERR_MSG_Q=&ERR_MSG_Q_;

void QL_FAO(CSTR)
//
// Format and queue an error message using $FAO and the message queue.
//
    {
    extern
	sleeping;
    extern	mm$qblk_get();
    extern void	mm$qblk_free ();
    extern	LIB$SYS_FAOL ();
    signed long
	RC;
    struct dsc$descriptor * MDSC;
    struct queue_blk_structure(QB$ERRMSG) * QB;

// Make sure logging is enabled

    if (log_state == 0)
	return;

// Allocate a queue block for the message

    QB = mm$qblk_get();
    MDSC = QB->EMQ$MDSC;
//    $INIT_DYNDESC(MDSC);
    INIT_DYNDESC(MDSC);

// Format the message

    RC = LIB$SYS_FAOL(CSTR, MDSC->dsc$w_length, MDSC, /*AP+*/8);
    if (! RC)
	{
	OPR$FAO("QL_FAO failure, RC = !XL",RC);
	mm$qblk_free(QB);
	return;
	};

// Insert the entry onto the queue

    INSQUE(QB,ERR_MSG_Q->EM$QTAIL);

// If the ACP is sleeping, issue a wakeup so messages will be written at the
// next interval.

    $ACPWAKE;
    }

void CHECK_ERRMSG_Q (void)
//
// Write all of the messages pending on the error message queue.
// Called from main TCP processing loop after all useful work has been done.
//
    {
      extern      void	mm$qblk_free ();
extern	STR$FREE1_DX ();
 struct queue_blk_structure(QB$ERRMSG) * QB;
	struct dsc$descriptor * MDSC;

// Scan the error message queue, writing each entry to the log file.

    while (REMQUE(ERR_MSG_Q->EM$QHEAD,QB) != EMPTY_QUEUE)
	{
	MDSC = QB->EMQ$MDSC;
	LOG_OUTPUT(MDSC);
	STR$FREE1_DX(MDSC);
	mm$qblk_free(QB);
	};
    }

//Sbttl "VMS Exit Handler"
/*

Function:

	Double check that all user IO has been posted otherwise user process
	will hang in MWAIT waiting for the outstanding IO to complete.  Catch
	is the system dynamic memory used in the user's IO request (IRP).

Inputs:

	None.

Outputs:

	None.

Side Effects:

	All user IO is posted with the TCP error "TCP is Exiting".
*/

void Exit_Handler (void)
    {
      extern      void	user$purge_all_io ();
extern void	RESET_PROCNAME() ;

    ERROR$FAO("Exit handler: Exit requested, cleaning up...");

    user$purge_all_io();
    if (log_state != 0)
	LOG_CLOSE();
    if (act_state != 0)
	ACT_CLOSE();
    OPR$FAO("Network offline - ACP exiting.");

// Re-enable resource wait mode (for debugging)

    exe$setrwm(0);

    RESET_PROCNAME();
    }

//Sbttl "VMS Exception Handler"
/*

Function:

	Catch those nasty Exceptions which might might cause TCP to crash
	& forget about user IO requests thus leaving the user jobs stuck
	in MWAIT state waiting for their IO to complete.

Inputs:

	None.

Outputs:

	SS$_Resignal, indicate we want to bomb.

Side Effects:

	All user IO is posted with the TCP error "TCP is Exiting".
*/

Exception_Handler(SIG,MECH)
     struct  _chfdef1 * SIG;
    {
extern void	user$purge_all_io ();

    ERROR$FAO("Exception handler: signal name !XL",SIG->chf$l_sig_name);
    user$purge_all_io();
    exe$flush( LOGRAB);
    exe$flush( ACTRAB);
    return(SS$_RESIGNAL);
    }
