#ifndef tcpmacros_h
#define tcpmacros_h

#if 0 
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
%Title 'TCP global Macro definitions'
/*
Module:

	TCPMACROS

Facility:

	Provide some macros for convenient usage by the various modules in the
	TCP/IP network ACP.

Author:

	Original author ?
	This version by Vince Fuller, CMU-CSD, Summer, 1986
	Copyright (c) 1986,1987, Vince Fuller and Carnegie-Mellon University

Change log:

6.7	7-Jan-1992	John Clement		Rice
	Added LOG$FLUSH

6.6	17-Jul-1991	Henry W. Miller		USBR
	Added debugging flags for name lookup and memory allocation.

6.4  24-Mar-88, Edit by VAF
	Enclose X..$FAO macros in BEGIN/END blocks to prevent weird IF/THEN
	block semantics.

6.3  20-Nov-87, Edit by VAF
	General cleanup. Create $KCALL macro to isolate all $CMKRNL calls to
	this macro package. Make logging macros, NOINT/OKINT, etc. check and
	declare necessary externals so modules don't have to. Add $ACPWAKE
	macro. Add LOG$IPERR - IP error logging.

6.2  10-Jun-87, Edit by VAF
	Flush hostname processing from ACP. Change GTHST and OPEN blocks to
	know about host name length.

6.1  18-Feb-87, Edit by VAF
	Conditional compilation of a bunch of stuff (logging, GREEN, etc.)
	Improvements to a bunch of the logging stuff.
*/
#endif

#include<stdarg.h>
#include<descrip.h>

// Define version-specific debugging flags.

#if 0
REQUIRE 'MACROSWI.REQ';		// Compile switches

COMPILETIME NO_CrLf = 1;	// For message macros

FIELD VB$FIELDS =
    SET
    VB$LEN = [0,0,16,0],	// Varying string - current length
    VB$DAT = [0,16,8,0]		// First byte of data
    TES;

MACRO
    REPEAT = DO%,		// Extend bliss for Repeat until lovers.

// Generate a pointer to a character string descriptor quadword.

    ASCII_C(S) = uplit(%CharCount(S),uplit(%ascii S))%,

// Generate a carriage-return Line-feed sequence for use in %string lex function.

    CrLf = %STRING(%Char(13),%Char(10))%,

// String descriptor handling macros

    VBUF(NCHR) = BLOCK[CH$ALLOCATION(NCHR+2)] FIELD(VB$FIELDS) %,
    DESC$VSTR = BLOCK[DESC$STR_LEN,BYTE] %,
    DESC$VSTR_ALLOC(NAME,MAXLEN) =
	%IF NOT %NULL(MAXLEN) %THEN
	    %NAME(NAME,'_BUF') : VBUF(MAXLEN) PRESET([VB$LEN] = 0),
	%FI
	NAME : DESC$VSTR
	%IF NOT %NULL(MAXLEN) %THEN
	    PRESET([DSC$B_CLASS] = DSC$K_CLASS_VS,
		   [DSC$B_DTYPE] = DSC$K_DTYPE_VT,
		   [DSC$W_MAXSTRLEN] = MAXLEN,
		   [DSC$A_POINTER] = %NAME(NAME,'_BUF'))
	%FI
	%,
    DESC$STR_INIT(DESC,STRLEN,STRPTR) =
	BEGIN
	DESC[DSC$B_CLASS] = DSC$K_CLASS_Z;
	DESC[DSC$B_DTYPE] = DSC$K_DTYPE_Z;
	DESC[DSC$W_LENGTH] = CH$ALLOCATION(STRLEN);
	DESC[DSC$A_POINTER] = STRPTR;
	END %,
    DESC$VSTR_INIT(DESC,MAXLEN,STRPTR) =
	BEGIN
	DESC[DSC$B_CLASS] = DSC$K_CLASS_VS;
	DESC[DSC$B_DTYPE] = DSC$K_DTYPE_VT;
	DESC[DSC$W_MAXSTRLEN] = MAXLEN;
	DESC[DSC$A_POINTER] = STRPTR;
	END %,

// Handle string descriptor.
// If argument is a quoted string, return created descriptor
// else, just return the argument.

    $$STR(ARG) =
	%IF %ISSTRING(ARG) %THEN %ASCID ARG %ELSE ARG %FI %,

// Declare something external if it isn't already declared

    $QEXT(NAME,MODIF) =
	%IF NOT %DECLARED(NAME) %THEN
	    EXTERNAL NAME  %IF NOT %NULL(MODIF) %THEN : MODIF %FI;
	%FI
    %,
    $QEXTR(NAME,MODIF) =
	%IF NOT %DECLARED(NAME) %THEN
	    EXTERNAL ROUTINE NAME  %IF NOT %NULL(MODIF) %THEN : MODIF %FI;
	%FI
    %,
#endif
	   
// Write a message to the console operator

static int inline OPR$FAO(char *c, ...) {
  struct dsc$descriptor d;
  d.dsc$w_length=strlen(c);
  d.dsc$a_pointer=c;
  va_list args;
  int argv[18],argc=0;
  va_start(args,c);
  while(argc<18) {
    argv[argc]=va_arg(args,int);
    argc++;
  }
  va_end(args);
  return OPR_FAO(&d,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

static int inline ERROR$FAO(char *c, ...) {
  struct dsc$descriptor d;
  d.dsc$w_length=strlen(c);
  d.dsc$a_pointer=c;
  va_list args;
  int argv[18],argc=0;
  va_start(args,c);
  while(argc<18) {
    argv[argc]=va_arg(args,int);
    argc++;
  }
  va_end(args);
  return ERROR_FAO(&d,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

static int inline WARN$FAO(char *c, ...) {
  struct dsc$descriptor d;
  d.dsc$w_length=strlen(c);
  d.dsc$a_pointer=c;
  va_list args;
  int argv[18],argc=0;
  va_start(args,c);
  while(argc<18) {
    argv[argc]=va_arg(args,int);
    argc++;
  }
  va_end(args);
  return ERROR_FAO(&d,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

static int inline FATAL$FAO(char *c, ...) {
  struct dsc$descriptor d;
  d.dsc$w_length=strlen(c);
  d.dsc$a_pointer=c;
  va_list args;
  int argv[18],argc=0;
  va_start(args,c);
  while(argc<18) {
    argv[argc]=va_arg(args,int);
    argc++;
  }
  va_end(args);
  return FATAL_FAO(&d,argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15],argv[16],argv[17]);
}

#if 0
// Obsolete error processing macros - special cases of above.

    Fatal_Error(S,ERRCODE) =
	%IF %NULL(ERRCODE) %THEN
	    FATAL$FAO(S)
	%ELSE
	    FATAL$FAO(%STRING(S,', EC = //XL'),ERRCODE)
	%FI
	%,
    Warn_Error(S,RETVALUE) =
	%IF %NULL(RETVALUE) %THEN
	    WARN$FAO(S)
	%ELSE
	    WARN$FAO(%STRING(S,', EC = //XL'),RETVALUE)
	%FI
	%,

#endif

// Control segment Send macros

#define    send_syn$ack(TCB) tcp$send_ctl(TCB,M$SYN_ACK)
#define    send_syn(TCB) tcp$send_ctl(TCB,M$SYN)
#define    send_fin(TCB) tcp$send_ctl(TCB,M$FIN)
#define    send_rst(TCB) tcp$send_ctl(TCB,M$RST)

// Queue management macros.

#define    queue_empty(Queue$Head) (((struct si_fields *)Queue$Head)->si_qhead == Queue$Head)
#define    queue_not_empty(Queue$Head) (((struct si_fields *)Queue$Head)->si_qhead != Queue$Head)

// Enable/Disable AST Delivery.
// If AST_IN_Progress then don't bother with AST blocking as AST will block
// delivery of other AST as same access mode.

#define    DISABLE_AST exe$setast(0);

#define    ENABLE_AST exe$setast(1);

// Enable/disable interrupts (AST's). These are the macros that should
// really be used, since they restore the AST state to its previous value
// correctly.

#define    NOINT { \
  extern signed long ast_in_progress; \
  extern signed long intdf; \
	if (! ast_in_progress && (intdf = intdf+1) == 0) \
	    DISABLE_AST \
}
#define    OKINT { \
  extern signed long ast_in_progress; \
  extern signed long intdf; \
	if (! ast_in_progress && (intdf = intdf-1) < 0) \
	    ENABLE_AST \
}

// Wake sleeping ACP from $HIBER call. This is used when an AST routine wants
// the mainline ACP code to do something at non-AST level.

#define    $ACPWAKE \
	{ \
	extern sleeping; \
	if (sleeping) \
	    { \
	    sleeping = FALSE; \
	    exe$wake(); \
	} \
	}

// Macros to do word and longword byte swapping.

#define    SWAPWORD(ADDR) \
	{ \
	char * XX = ADDR;\
	register YY;\
	YY = XX[0];\
	XX[0] = XX[1];\
	XX[1] = YY;\
	}
#define    SWAPLONG(ADDR) \
	{\
	short * XX = ADDR;\
	register  TEMP;\
	TEMP = XX[0]; /* Word swap of TTL*/ \
        XX[0] = XX[1];\
	XX[1] = TEMP;\
        }

// Macro to set $CMKRNL argument list
#include <stdarg.h>

#define $$KCALL KCALL

static int inline $$KCALL(int (*func)(), ...) {
	  va_list args;
	  int argv[16],argc=0;
	  va_start(args,func);
	  while(argc<16) {
	    argv[argc]=va_arg(args,int);
	    argc++;
	  }
	  va_end(args);
          return func(argv[0],argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9],argv[10],argv[11],argv[12],argv[13],argv[14],argv[15]);
}

#if 0
    $$KARGS(ANAME)[NAME] =
	ANAME[%COUNT+1] = NAME; %,

// Macro to setup and call routine via $CMKRNL.
    $$KCALL(RTN)[] =
	%IF %LENGTH EQL 1 %THEN
	    $CMKRNL(ROUTIN=RTN)
	%ELSE
	    BEGIN
	    LOCAL
		CMKARG : VECTOR[%LENGTH];
	    CMKARG[0] = %LENGTH-1;
	    $$KARGS(CMKARG,%REMAINING)
	    $CMKRNL(ROUTIN=RTN,ARGLST=CMKARG);
	    END
	%FI
    %,

#endif
#ifdef LOGSWITCH		// Want the logging macros
//know how to implement these, but delay it   
   // Conditionally do something according to LOG_STATE flags

#define   $$LOGF(logf) \
	((log_state && (logf)) != 0)

#if 0
// Macros for output to log and opr and the activity log.

    LOG$OUT(XSTR) =
	LOG_OUTPUT(%ASCID %STRING(XSTR)) %,
    XLOG$OUT(LOGF) =
	BEGIN
	IF $$LOGF(LOGF) THEN
	    LOG$OUT(%REMAINING)
	END
        %,
    LOG$FAO(CST) =
	BEGIN
	%IF NOT %DECLARED(LOG_FAO) %THEN
	    EXTERNAL ROUTINE LOG_FAO : NOVALUE;
	%FI
	%IF %NULL(%REMAINING) %THEN
	    LOG_FAO(%ASCID %STRING(CST))
	%ELSE
	    LOG_FAO(%ASCID %STRING(CST),%REMAINING)
	%FI
	END
	%,
#endif
#if 0
    XLOG$FAO(LOGF) =
	BEGIN
	IF $$LOGF(LOGF) THEN
	    LOG$FAO(%REMAINING)
        END
        %,
    ACT$OUT(XSTR) =
	LOG_OUTPUT(%ASCID %STRING(XSTR)) %,
    ACT$FAO(CST) =
	BEGIN
	%IF NOT %DECLARED(ACT_FAO) %THEN
	    EXTERNAL ROUTINE ACT_FAO : NOVALUE;
	%FI
	%IF %NULL(%REMAINING) %THEN
	    ACT_FAO(%ASCID %STRING(CST))
	%ELSE
	    ACT_FAO(%ASCID %STRING(CST),%REMAINING)
	%FI
	END
	%,
    QL$FAO(CST) =
	BEGIN
	%IF NOT %DECLARED(QL_FAO) %THEN
	    EXTERNAL ROUTINE QL_FAO : NOVALUE;
	%FI
	%IF %NULL(%REMAINING) %THEN
	    QL_FAO(%ASCID %STRING(CST));
	%ELSE
	    QL_FAO(%ASCID %STRING(CST),%REMAINING);
	%FI
	END
	%,
    XQL$FAO(LOGF) =
	BEGIN
	IF $$LOGF(LOGF) THEN
	    QL$FAO(%REMAINING)
	END
        %;
#endif

#else				// Don't want logging macros - make them null

#define $$LOGF SSLOGF
static inline    $$LOGF(/*LOGF*/)  { } ;		// $$LOGF always fails
static inline    LOG$OUT(/*XSTR*/)  { } ;	// LOG$OUT does nothing
static inline    XLOG$OUT(/*LOGF*/)  { } ;	// XLOG$OUT 
static inline    LOG$FAO(/*CST*/)  { } ;		// LOG$FAO 
static inline    ACT$OUT(/*XSTR*/)  { } ;	// ACT$OUT does nothing
static inline    ACT$FAO(/*CST*/) { } ;		// ACT$FAO 
static inline    XLOG$FAO(/*LOGF*/)  { };	// XLOG$FAO 
static inline    QL$FAO(/*CST*/) { } ;		// QL$FAO 
static inline    XQL$FAO(/*LOGF*/) { } ;	// XQL$FAO 
#endif

// Network logger flag bits - determine what events to log

#define    LOG$PHY	 0x01	// Packet physical headers
#define    LOG$ARP	 0x02	// ARP packet info
#define    LOG$IP	 0x04	// IP packet headers
#define    LOG$TCP	 0x08	// TCP segment info (packet trace)
#define    LOG$TCBDUMP	 0x10	// TCB dump on servicing
#define    LOG$USER	 0x20	// User I/O requests
#define    LOG$TCBSTATE 0x40	// TCB state changes
#define    LOG$TCBCHECK 0x80	// TCB servicing timing
#define    LOG$TCPERR	 0x100	// TCP errors (dropped pkts, etc.)
#define    LOG$ICMP	 0x200	// ICMP activity
#define    LOG$UDP	 0x400	// UDP activity
#define    LOG$TVT	 0x800	// TVT (virtual terminal) activity
#define    LOG$IPERR	 0x1000	// IP errors (bad routing, etc.)
#define    LOG$DEBUG	 0x2000	// Temporary statements
#define    LOG$MEM	 0x4000	// Memory Allocation
#define    LOG$MSG	 0x8000	// Name Lookup
#define    LOG$TELNET	 0x10000	// Log Telnet activity
#define    LOG$TELNEG	 0x20000	// Log Telnet negotiations
#define    LOG$TELERR	 0x40000	// Log Telnet errors
#define    LOG$SNOOP	 0x80000	// SNOOP activity
#define    LOG$FLUSH	 0x100000	// Dump each line

// Define QBLK/Input segment queue flags.

#define    Q$XERCV  1		// XEDRV receive buffer queue
#define    Q$SEGIN  2		// AST segment input queue
#define    Q$TCBNR  4		// TCB net receive queue
#define    Q$TCBFQ  8		// TCB future queue
#define    Q$USRCV  16		// User receive queue
#if 0

// Conditional coding for debugging

%IF QDEBUG %THEN
    MACRO
	XQDEFINE =
	    EXTERNAL ROUTINE
		MEM_INSQUE, MEM_REMQUE; %,
	XINSQUE(QH,QB,QRTN,QID,QVAL) =
	    MEM_INSQUE(QH,QB,QRTN,QID,QVAL) %,
	XREMQUE(QH,QB,QRTN,QID,QVAL) =
	    MEM_REMQUE(QH,QB,QRTN,QID,QVAL) %;
%ELSE
    MACRO
	XQDEFINE = %,
	XINSQUE(QH,QB,QRTN,QID,QVAL) =
	    INSQUE(QH,QB) %,
	XREMQUE(QH,QB,QRTN,QID,QVAL) =
	    REMQUE(QH,QB) %;
%FI

#endif
#endif

#define CH$PTR(X,Y) ((long)X+(long)Y)
#define CH$RCHAR(X) *((char *)X) // check or ++ char?
#define CH$PLUS(X,Y) ((long)X+(long)Y)
//#define CH$WCHAR_A(X,Y) { char * localc=Y; *localc++=X; Y=localc; } // was: *(char*)Y++=X
#define CH$WCHAR_A(X,Y) *(char*)Y++=X
   //#define CH$RCHAR_A(X) *(((char *)X)++)  // not yet? *((*(char **)X)++) 
   static inline char ch$rchar_a(char **c) {
  return *(*c)++;
}
#define CH$RCHAR_A(X) ch$rchar_a(&X)   

#define LIB$CALLG(X,Y) Y()


