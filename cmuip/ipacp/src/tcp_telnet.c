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
//TITLE "TCP_TELNET - TELNET Virtual Terminal service"
/*
Facility:

	TCP_TELNET - Run incoming TELNET service under TCP (RFC 854)

Abstract:

	Supports incoming TELNET virtual terminal traffic by routing TCP
	data between pseudo-terminals and the rest of the IPACP.

Author:

	Vince Fuller, CMU-CSD, Summer, 1987
	Copyright (c) 1987, Vince Fuller and Carnegie-Mellon University

Module Modification History:

1.11
	7-Jan-1992	John Clement
	Put in hold off in PTY_WRITE until process is available on
	Terminal side.  And fixed LOG$TVT write in PTY_Write.

1.10
	3-Dec-1991	John Clement		Rice University (JC)

	Binary_On, Binary_Off added to turn on/off EIGHTBIT,PASTHRU
	Extended_ASCII_On,Off added to trun on/off EIGHTBIT

1.9	14-Nov-1991	Henry W. Miller		USBR
	Rework TELNET server logic.  Changes are too numerous to list, but
	highlights are:
	TELNET_INPUT() and TELNET_OUTPUT() now call the PTY I/O routines
	rather than the TCP I/O routines.
	PTY_READ() and PTY_WRITE() now call the TCP I/O routines, rather
	than vice versa.
	New routine NET_TO_PTY() now fills the PTY buffer from the TCP buffer,
	and handles buffer wrapping and overflow properly.
	PTY_WRITE() now handles buffer wrapping and overflow properly.
	Call TCP$Enqueue_ACK() rather than setting P}ING_ACK.

1.8	13-Nov-1991 Edit by John M. Clement(JC)	Rice University
	Modified to make Negotiations work.

1.7	15-Oct-1991	Henry W. Miller		USBR
	Rearrange logging code in PTY_WRITE() so we can see how many bytes
	would have been written if the $QIO fails.

1.6	18-Jul-1991	Henry W. Miller		USBR
	Use LIB$GET_VM_PAGE and LIB$FREE_VM_PAGE rather then LIB$GET_VM
	and LIB$FREE_VM.

1.5	28-Jan-1991	Henry W. Miller		USBR
	Make ACK_THRESHOLD a configurable variable.

1.4	04-Sep-1989	Bruce R. Miller         CMU Network Development
	Adding support for LineMode option


1.3	15-Dec-1988, Edit by Simon Hackett, University of Adelaide, Australia
	Routine PTY_SET_OWNER_PID to allow identification of the PID of
	a TELNET session.

1.2	20-Feb-1989	Bruce R. Miller		CMU Network Development
	Added support for several new options: Timing-Mark, Window-Size,
	Terminal-Type, Remote-Flow-control.

1.2	01-Feb-1989	Bruce R. Miller		CMU Network Development
	Added routine TCP_ADD_STRING() to send a string over the net.
	Added AYT (Are you there?) recognition & response.

1.2	09-JUN-1988	Dale Moore	CMU-CS/RI
	Modified to return to State Normal upon receiving IAC <silly> or
	IAC <unknown>.  Also be willing to negotiate Supress GA in
	both directions.

1.1  19-Nov-87, Edit by VAF
	$ACPWAKE macro is now in TCPMACROS.REQ.

1.0  31-Jul-87, Edit by VAF
	Initial version. Loosely based on Dale Moore's TELNET_SERVER.

*/

MODULE TELNET(IDENT="1.11",LANGUAGE(BLISS32),
	      ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			      NONEXTERNAL=LONG_RELATIVE),
	      LIST(REQUIRE,ASSEMBLY,OBJECT,BINARY),
	      OPTIMIZE,OPTLEVEL=3,ZIP)=
{

!!!HACK!!// why does this module make so many refs to AST_in_Progress?

#include "SYS$LIBRARY:STARLET";	// VMS system definitions
#include "CMUIP_SRC:[CENTRAL]NETXPORT";	// BLISS common definitions
#include "CMUIP_SRC:[CENTRAL]NETERROR";	// Network error codes
#include "CMUIP_SRC:[CENTRAL]NETVMS";			// VMS-specific definitions
#include "TCPMACROS";		// System-wide Macro definitions
#include "STRUCTURE";		// System-wide structure definitions
#include "TCP";			// TCP related definitions
#include "TELNET";		// TELNET protocol definitions

extern signed long
    LOCAL_NAME,
    AST_IN_PROGRESS;

extern signed long LITERAL
    M$INTERNAL : UNSIGNED(8);

extern
    TCP$TCB_CLOSE,
    TCB$Create,
 void    TCB$Delete,
 void    TCP$Enqueue_Ack,
 void    TCP$Send_Ack,
    TCP$Send_Data,
    TCP$TCB_Init,
    TIME_STAMP,
    Check_Unique_Conn,
 void    Conect_Insert;

LITERAL
    TVT_OPEN_TIMEOUT = 120*CSEC; // Amount of time to wait for TVT to open

LITERAL		!
    Telnet$K_Char_AO	= %O"17",	// Control-O
    Telnet$K_Char_AYT	= %O"24",	// Control-T
    Telnet$K_Char_Brk	= %O"31",	// Control-Y
    Telnet$K_Char_EC	= %O"177",	// DEL
    Telnet$K_Char_Purge	= %X"18",	// Control-X
    Telnet$K_Char_EL	= %O"25",	// Control-U
    Telnet$K_Char_IP	= %O"03";	// Control-C.



//SBTTL "TELNET definitions"

// Define the structure of a PTY status block

$FIELD PTY$IOSB_FIELDS (void)
    SET
    PTSB$STATUS	= [$UWORD],	// $QIO status
    PTSB$NBYTES	= [$UWORD],	// Number of bytes transferred
    PTSB$EXTRA1	= [$UWORD],	// Extra information
    PTSB$EXTRA2	= [$UWORD]	// Extra information
    TES;
LITERAL
    PTY$IOSB_SIZE = $FIELD_SET_SIZE;
MACRO
    PTY$IOSB = BLOCK->PTY$IOSB_SIZE FIELD(PTY$IOSB_FIELDS) %;

// USE_ASTS nonzero means we should do all TCP I/O at AST level and thus it is
// OK to directly call TCP_READ and TCP_WRITE in AST routines.

COMPILETIME
    USE_ASTS = 1;

//SBTTL "Module data"

BIND
    PTY_NAME = %ASCID"INET$PTY",
    AYT_RESPONSE = %ASCID"[definitely]";

// Define the default options state for new TVTs

extern
    Timing_Mark_On,
    Terminal_Type_On,
    Terminal_Type_Sub,
    Window_Size_On,
    Window_Size_Sub,
    Set_DEVDEP,				// JC
    LineMode_Sub;

static signed long
    DEFAULT_OPTION_BLOCK : OPT$BLOCK
	PRESET(
	    [OPT$STATE]		= OPT$STATE_OFF,
	    [OPT$CURRENT]	= FALSE,
	    [OPT$PREFER]	= OPT$STATE_OFF),
    TVT_DEF_LOCAL : OPT$LIST
	PRESET(
	    [TELNET$K_BINARY, OPT$STATE]	= OPT$STATE_OFF,
	    [TELNET$K_BINARY, OPT$CURRENT]	= FALSE,
	    [TELNET$K_BINARY, OPT$PREFER]	= OPT$DONT_CARE,
	    [TELNET$K_BINARY, OPT$ON_RTN]	= Set_DEVDEP,		!JC
	    [TELNET$K_BINARY, OPT$OFF_RTN]	= Set_DEVDEP,		!JC

	    [TELNET$K_ECHO, OPT$STATE]		= OPT$STATE_OFF,	!JC
	    [TELNET$K_ECHO, OPT$CURRENT]	= FALSE,		!JC
	    [TELNET$K_ECHO, OPT$PREFER]		= OPT$STATE_ON,		!JC
	    [TELNET$K_ECHO, OPT$ON_RTN]		= Set_DEVDEP,		!JC
	    [TELNET$K_ECHO, OPT$OFF_RTN]	= Set_DEVDEP,		!JC

	    [TELNET$K_SUPRGA, OPT$STATE]	= OPT$STATE_OFF,
	    [TELNET$K_SUPRGA, OPT$CURRENT]	= FALSE,
	    [TELNET$K_SUPRGA, OPT$PREFER]	= OPT$STATE_ON,

	    [TELNET$K_Timing_Mark, OPT$STATE]	= OPT$STATE_OFF,
	    [TELNET$K_Timing_Mark, OPT$CURRENT]	= FALSE,
	    [TELNET$K_Timing_Mark, OPT$ON_RTN]	= Timing_Mark_On,
	    [TELNET$K_Timing_Mark, OPT$PREFER]	= OPT$DONT_CARE,

	    [TELNET$K_Extended_Ascii, OPT$STATE]	= OPT$STATE_OFF,!JC
	    [TELNET$K_Extended_Ascii, OPT$CURRENT]	= FALSE,	!JC
	    [TELNET$K_Extended_Ascii, OPT$PREFER]	= OPT$DONT_CARE,!JC
	    [TELNET$K_Extended_Ascii, OPT$ON_RTN]	= Set_DEVDEP,	!JC
	    [TELNET$K_Extended_Ascii, OPT$OFF_RTN]	= Set_DEVDEP,	!JC

!!!JC	    [TELNET$K_Window_Size, OPT$STATE]	= OPT$STATE_OFF,
!!!JC	    [TELNET$K_Window_Size, OPT$CURRENT]	= FALSE,
!!!JC	    [TELNET$K_Window_Size, OPT$PREFER]	= OPT$STATE_ON,
!!!JC	    [TELNET$K_Window_Size, OPT$SUB_RTN] = Window_Size_Sub,

	    [TELNET$K_LineMode, OPT$STATE]	= OPT$STATE_OFF,
	    [TELNET$K_LineMode, OPT$CURRENT]	= FALSE,
	    [TELNET$K_LineMode, OPT$PREFER]	= OPT$STATE_Off),

    TVT_DEF_REMOTE : OPT$LIST
	PRESET(
	    [TELNET$K_BINARY, OPT$STATE]	= OPT$STATE_OFF,
	    [TELNET$K_BINARY, OPT$CURRENT]	= FALSE,
	    [TELNET$K_BINARY, OPT$PREFER]	= OPT$DONT_CARE,

!!!JC	    [TELNET$K_ECHO, OPT$STATE]		= OPT$STATE_OFF,	!JC
!!!JC	    [TELNET$K_ECHO, OPT$CURRENT]	= FALSE,		!JC
!!!JC	    [TELNET$K_ECHO, OPT$PREFER]		= OPT$DONT_CARE,	!JC

	    [TELNET$K_SUPRGA, OPT$STATE]	= OPT$STATE_OFF,
	    [TELNET$K_SUPRGA, OPT$CURRENT]	= FALSE,
	    [TELNET$K_SUPRGA, OPT$PREFER]	= OPT$STATE_ON,

	    [TELNET$K_Timing_Mark, OPT$STATE]	= OPT$STATE_OFF,
	    [TELNET$K_Timing_Mark, OPT$CURRENT]	= FALSE,
	    [TELNET$K_Timing_Mark, OPT$PREFER]	= OPT$DONT_CARE,

	    [TELNET$K_Extended_Ascii, OPT$STATE]	= OPT$STATE_OFF,!JC
	    [TELNET$K_Extended_Ascii, OPT$CURRENT]	= FALSE,	!JC
	    [TELNET$K_Extended_Ascii, OPT$PREFER]	= OPT$DONT_CARE,!JC

	    [TELNET$K_Terminal_Type, OPT$STATE]		= OPT$STATE_OFF,
	    [TELNET$K_Terminal_Type, OPT$CURRENT]	= FALSE,
	    [TELNET$K_Terminal_Type, OPT$PREFER]	= OPT$STATE_ON,	!!// JC
	    [TELNET$K_Terminal_Type, OPT$ON_RTN]	= Terminal_Type_On,
	    [TELNET$K_Terminal_Type, OPT$SUB_RTN]	= Terminal_Type_Sub,

	    [TELNET$K_Window_Size, OPT$STATE]	= OPT$STATE_OFF,
	    [TELNET$K_Window_Size, OPT$CURRENT]	= FALSE,
	    [TELNET$K_Window_Size, OPT$PREFER]	= OPT$STATE_ON,		!!// JC
	    [TELNET$K_Window_Size, OPT$ON_RTN]	= Window_Size_On,
	    [TELNET$K_Window_Size, OPT$SUB_RTN]	= Window_Size_Sub,

	    [TELNET$K_Toggle_Flow_Control, OPT$STATE]	= OPT$STATE_OFF,
	    [TELNET$K_Toggle_Flow_Control, OPT$CURRENT]	= FALSE,
	    [TELNET$K_Toggle_Flow_Control, OPT$PREFER]	= OPT$STATE_ON,

	    [TELNET$K_LineMode, OPT$STATE]	= OPT$STATE_OFF,
	    [TELNET$K_LineMode, OPT$CURRENT]	= FALSE,
	    [TELNET$K_LineMode, OPT$PREFER]	= OPT$STATE_OFF,
	    [TELNET$K_LineMode, OPT$SUB_RTN]	= LineMode_Sub);



//SBTTL "TELNET_CREATE - Initialize a TCP connection for a TVT"
/*
    Creates and initializes a TCB for a new TVT connection. Called from
    segment input processor when a SYN segment has been received for the
    well-known TELNET port (WKS$TELNET).
*/

FORWARD ROUTINE
 VOID    TELNET_OPEN_TIMEOUT;

TELNET_CREATE(LHOST,LPORT,FHOST,FPORT)
    {
    signed long
	STRUCT TCB_STRUCTURE * TCB,
	CIDX;

// Check for unique connection. We actually know that the connection is already
// unique, but we do this for the side effect of being inserted in the CONECT
// table.

    NOINT;

    if (Check_Unique_Conn(LPORT,FHOST,FPORT,CIDX) != TRUE)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT create failed - CONECT table full!/",0);
	OKINT;
	RETURN 0;
	};

// Create and initialize a new TCB

    if ((TCB = TCB$Create()) == Error)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT TCB creation failed!/",0);
	OKINT;
	RETURN 0;
	};

// Perform standard TCB initializations

    TCP$TCB_Init(TCB);

// Setup standard TVT TCB fields. Note that segment input processing code will
// setup wild foreign host/port and local host when this routine returns.

    TCB->IS_TVT = TRUE;		// This is a TVT
    TCB->TVTDATA = 0;		// No TVT data block yet
    TCB->Local_Host = LHOST;	// Set local host and port
    TCB->Local_Port = LPORT;
    TCB->Foreign_Host = FHOST;	// Set foreign host and port
    TCB->Foreign_Port = FPORT;
    TCB->UCB_ADRS = 0;		// No UCB for this, since no user process
    TCB->STATE = CS$LISTEN;	// Initial state is listening for SYN
    TCB->CON_INDEX = CIDX;
    Conect_Insert(TCB,CIDX);	// Insert into connection table

// Setup a handler if the open times-out

    TCB->Pending_IO = TRUE;
    TCB->Curr_User_Function = M$INTERNAL;
    TCB->Timeout_Routine = TELNET_OPEN_TIMEOUT;
    TCB->Function_Timer = Time_Stamp() + TVT_Open_Timeout;

// And return the TCB address

    OKINT;
    RETURN TCB;
    };

TELNET_OPEN_TIMEOUT(TCB) : NOVALUE (void)
    {
!
//   Come here from user function timeout code if a TVT connection does not
//   become established soon enough after we first see the SYN for it. We
//   simply flush the TCB.
!
    MAP
	STRUCT TCB_STRUCTURE * TCB;

!~~~ Maybe we should do something better here?

    XLOG$FAO(LOG$TCPERR,"!%T TVT open timeout for TCB x!XL!/",0,TCB);
    TCB$Delete(TCB);
    };

//SBTTL "TELNET_OPEN - Finish open of TCP connection for a TVT"
/*
    Finish setup of TELNET connection, creating TVT data block within the
    TCB and performing assigmnent/initialization of the pseudo-terminal
    device associated with this connection. Called from segment input handler
    when TCP connection goes into the ESTABLISHED state.
*/

FORWARD ROUTINE
 VOID    TVT_NEGOTIATE,
 VOID    TCP_READ,
 VOID    TCP_ADD_STRING,
 VOID    TCP_WRITE,
 VOID    PTY_READ,
 VOID    PTY_WRITE,
 VOID    NET_TO_PTY,
 VOID    PTY_TO_NET,
    MBX_READ,
 VOID    MBX_READ_DONE;

namelook_done(tvt,rc,namlen,name) : NOVALUE (void)
!+
!	Fills in the name of the remote port
!-
    {
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	STRUCT TCB_STRUCTURE * TCB,
	nambuf : VECTOR[100,BYTE],
	nam	: $BBLOCK->DSC$K_S_BLN PRESET (
		[DSC$W_LENGTH] = %ALLOCATION(nambuf),
		[DSC$B_DTYPE] = DSC$K_DTYPE_Z,
		[DSC$B_CLASS] = DSC$K_CLASS_Z,
		[DSC$A_POINTER] = nambuf),
	accporbuf : VECTOR[100,BYTE],
	accpornam : $BBLOCK->DSC$K_S_BLN PRESET (
		[DSC$W_LENGTH] = %ALLOCATION(accporbuf),
		[DSC$B_DTYPE] = DSC$K_DTYPE_Z,
		[DSC$B_CLASS] = DSC$K_CLASS_Z,
		[DSC$A_POINTER] = accporbuf);

    if ((NOT RC)) RETURN;

    TCB = TVT->TVT$TCB;				// get TCB

    IF $TRNLOG(						// JC Get logical
		LOGNAM=%ASCID"TELNET_PASS_PORT",	// Pass on port number
		RSLLEN=nam->DSC$W_LENGTH,		// JC
		RSLBUF=nam)				// JC
	    == SS$_Normal
    THEN
	RC = $FAO(%ASCID"!AF!AS!UL",
		accpornam->DSC$W_LENGTH,accpornam,
		.namlen,name,
		nam,
		TCB->Foreign_PORT
		)
    else
	RC = $FAO(%ASCID"!AF",
		accpornam->DSC$W_LENGTH,accpornam,
		.namlen,name
		);
    XLOG$FAO(LOG$TELNET
	,"!%T Namelook_done: Access port=!AS!/",0
	,accpornam);
!
!	It is better to have the correct name than a truncated on
!
    if (accpornam->DSC$W_LENGTH > 30)
	RETURN(SS$_NORMAL);				// JC IF too long skip it
!!!JC	accpornam->DSC$W_LENGTH = 30;
    if (RC)
	RC = $QIOW (CHAN=TVT->TVT$PTY_CHN,FUNC=IO$_SETMODE,
			P1=accpornam->DSC$A_POINTER,	// Buffer
			P2=accpornam->DSC$W_LENGTH,	// Size
			P4=4				// Sub-func #4
			);

    };

TELNET_OPEN(TCB)
    {
    EXTERNAL ROUTINE
	LIB$GET_VM_PAGE : BLISS ADDRESSING_MODE(GENERAL),
	LIB$ASN_WTH_MBX : BLISS ADDRESSING_MODE(GENERAL),
	LIB$GETDVI	: BLISS ADDRESSING_MODE(GENERAL),
	Print,
	Line_Changed_AST;
   EXTERNAL ROUTINE
	NML$GETNAME;
    MAP
	STRUCT TCB_STRUCTURE * TCB;
    signed long
	nambuf : VECTOR[256,BYTE],
	nam	: $BBLOCK->DSC$K_S_BLN PRESET (
		[DSC$W_LENGTH] = %ALLOCATION(nambuf),
		[DSC$B_DTYPE] = DSC$K_DTYPE_Z,
		[DSC$B_CLASS] = DSC$K_CLASS_Z,
		[DSC$A_POINTER] = nambuf),
	accporbuf : VECTOR[100,BYTE],
	accpornam : $BBLOCK->DSC$K_S_BLN PRESET (
		[DSC$W_LENGTH] = %ALLOCATION(accporbuf),
		[DSC$B_DTYPE] = DSC$K_DTYPE_Z,
		[DSC$B_CLASS] = DSC$K_CLASS_Z,
		[DSC$A_POINTER] = accporbuf),
	struct TVT$BLOCK * TVT,
	PTYCHAN,
	MBXCHAN,
	Status,
	RC,
	IO_STATS : BLOCK[8,BYTE],
	tmp : SIGNED LONG,
	TMPBUF : BLOCK[TVT_TTY_BUFLEN, BYTE],
	TMPDSC : BLOCK[DSC$K_Z_BLN,BYTE];

// Clear the pending open that we were waiting for.

    TCB->Pending_IO = FALSE;

// Assign a chunk of memory for the TVT data block

//    RC = LIB$GET_VM(%REF(TVT$SIZE*4),TVT);
    RC = LIB$GET_VM_PAGE(%REF(((TVT$SIZE * 4) / 512) + 1),TVT);
    if (NOT RC)
	{
!~~~ Should we do anything better here?
	XLOG$FAO(LOG$TCPERR,
	  "!%T Telnet_Open: LIB$GET_VM_PAGE failure for TCB=x!XL, RC=x!XL!/"
	  ,0,TCB,RC);
	TCB$DELETE(TCB);
	RETURN FALSE;
	};

// Clear out the TVT data block

    CH$FILL(0,TVT$SIZE*4,CH$PTR(TVT));
    TVT->TVT$TCB = TCB;

// Initialize the options block to the standard initial state

    CH$MOVE(OPT$LSTBLEN,CH$PTR(TVT_DEF_LOCAL),CH$PTR(TVT->TVT$LCLOPTS));
    CH$MOVE(OPT$LSTBLEN,CH$PTR(TVT_DEF_REMOTE),CH$PTR(TVT->TVT$REMOPTS));

// Assign the PTY device and start it up.

    RC = LIB$ASN_WTH_MBX(PTY_NAME,
			 %REF(TVT_MBX_BUFLEN),
			 %REF(TVT_MBX_BUFLEN),
			 PTYCHAN,
			 MBXCHAN);
    if (NOT RC)
	{
	    XLOG$FAO(LOG$TCPERR,
		"!%T Telnet_Open:  PTY assign failure for TCB x!XL, RC=x!XL!/"
		,0,TCB,RC);
	    TCB$DELETE(TCB);
	    RETURN FALSE;
	};


// Associate the TVT with the TCP connection

    TVT->TVT$PTY_CHN = PTYCHAN;
    TVT->TVT$MBX_CHN = MBXCHAN;
    TCB->TVTDATA = TVT;

// Fill in the access port as [n.n.n.n]
    IF $TRNLOG(						// JC Get logical
		LOGNAM=%ASCID"TELNET_PASS_PORT",	// Pass on port number
		RSLLEN=nam->DSC$W_LENGTH,		// JC
		RSLBUF=nam)				// JC
	    == SS$_Normal
    THEN						// JC If got it
	RC = $FAO(%ASCID"!UB.!UB.!UB.!UB!AS!UL",
		accpornam->DSC$W_LENGTH,accpornam,
		.(TCB->Foreign_Host)< 0,8,0>,
		.(TCB->Foreign_Host)< 8,8,0>,
		.(TCB->Foreign_Host)<16,8,0>,
		.(TCB->Foreign_Host)<24,8,0>,
		nam,
		TCB->Foreign_PORT
		)
    else
	RC = $FAO(%ASCID"!UB.!UB.!UB.!UB",
		accpornam->DSC$W_LENGTH,accpornam,
		.(TCB->Foreign_Host)< 0,8,0>,
		.(TCB->Foreign_Host)< 8,8,0>,
		.(TCB->Foreign_Host)<16,8,0>,
		.(TCB->Foreign_Host)<24,8,0>
		);
    XLOG$FAO(LOG$TELNET
	,"!%T Telnet_Open: Remote host=!AS!/",0
	,accpornam);
    if (accpornam->DSC$W_LENGTH > 30)		// IF too long
	accpornam->DSC$W_LENGTH = 30;			// Adjust it
    if (RC)
	RC = $QIOW (CHAN=TVT->TVT$PTY_CHN,FUNC=IO$_SETMODE,
			P1=accpornam->DSC$A_POINTER,	// Buffer
			P2=accpornam->DSC$W_LENGTH,	// Size
			P4=4				// Sub-func #4
			);

// Fill in the actual name after name resolution
    NML$GETNAME(TCB->Foreign_Host, namelook_done, TVT);

// Start a receive on the PTY mailbox.

    if (NOT MBX_READ(TVT))
	{
	TCB$DELETE(TCB);
	RETURN FALSE;
	};

// Initialize buffer pointers

    TVT->TVT$RD_PTR = CH$PTR(TVT->TVT$RD_BUF);
//    TVT->TVT$WR_PTR = CH$PTR(TVT->TVT$WR_BUF);
    TVT->TVT$WR_IPTR = 0 ;
    TVT->TVT$WR_OPTR = 0 ;
    TVT->TVT$NEG_EQP = TVT->TVT$NEG_DQP = CH$PTR(TVT->TVT$NEG_BUF);
    TVT->TVT$WR_ICNT = 0 ;
    TVT->TVT$WR_OCNT = 0 ;


!
// Create a banner to send to the user
// The banner is contained in logical TELNET_ANNOUNCE JC
!

    TMPDSC->DSC$B_CLASS = DSC$K_CLASS_Z;
    TMPDSC->DSC$B_DTYPE = DSC$K_DTYPE_Z;
    TMPDSC->DSC$W_LENGTH = %ALLOCATION(TMPBUF);
    TMPDSC->DSC$A_POINTER = TMPBUF;
//    $FAO(%ASCID"!/!AS VAX/VMS (CMU) TELNET Service!/",
!	TMPDSC->DSC$W_LENGTH, TMPDSC, LOCAL_NAME);

    NAM [DSC$W_LENGTH] = %ALLOCATION(nambuf);
    IF $TRNLOG(						// JC Get logical
		LOGNAM=%ASCID"TELNET_ANNOUNCE",		// JC name for banner
		RSLLEN=nam->DSC$W_LENGTH,		// JC
		RSLBUF=nam)				// JC
	    == SS$_Normal THEN				// JC If got it
	{
	$FAO(%ASCID"!AS!/"				// JC Put into output
		,TMPDSC->DSC$W_LENGTH			// JC buffer
		,TMPDSC					// JC
		,NAM);					// JC
	TCP_Add_String(TVT, TMPDSC) ;
	} ;

// Check for creation of terminal side of connection (like _TZA0:)
// when ever we get input, until this flag is cleared.

    TVT->TVT$DO_PID = 1;

// Stuff the initial <NULL> into the PTY buffer for JOBCTL

//    CH$WCHAR(%CHAR(0),CH$PTR(TVT->TVT$WR_BUF));

    NET_TO_PTY(TVT, CH_NUL) ;

// Queue up the options that we want

    TVT_NEGOTIATE(TVT);

// Write the buffered data to the PTY and to the network
// We assume here that the initial banner can always fit in the network queue.
    TCP_WRITE(TVT);

    PTY_WRITE(TVT);

    PTY_READ(TVT);

// And give success return.
    TRUE
    };

//SBTTL "TELNET_CLOSE - Handle close of TELNET connection"
/*
    Handle normal close of TELNET connection. Flush the TVT data structure.
    Called from TCB$Delete just before TCB is deallocated.
*/

FORWARD ROUTINE
 VOID    TELNET_CLOSE_DONE;

TELNET_CLOSE(TCB) : NOVALUE (void)
    {
    MAP
	STRUCT TCB_STRUCTURE * TCB;
    EXTERNAL ROUTINE
	NML$CANCEL;
    signed long
	FHOST  = TCB->Foreign_Host,
	FPORT  = TCB->Foreign_Port,
	struct TVT$BLOCK * TVT;

// Log this.

    XLOG$FAO(LOG$TCBSTATE,"!%T TVT TCB x!XL closing, TVT=x!XL!/",
	     0,TCB,TCB->TVTDATA);
    ACT$FAO("!%D Telnet-in closed to IP addr !UB.!UB.!UB.!UB port=!UW!/",0,
	    FHOST<0,8>,FHOST<8,8>,FHOST<16,8>,FHOST<24,8>,
	    FPORT);

// Make sure we have a TVT and it isn't already being cancelled.

    TVT = TCB->TVTDATA;
    if ((TVT != 0) && (NOT TVT->TVT$CANCEL))
	{

// Deassign the PTY and mailbox channels.

	NML$CANCEL(TVT,0,0);	// Cancel the name lookup for accpornam

	NOINT;
	TVT->TVT$CANCEL = TRUE;
	$DASSGN(CHAN = TVT->TVT$PTY_CHN);
	$DASSGN(CHAN = TVT->TVT$MBX_CHN);
	$DCLAST(ASTADR = TELNET_CLOSE_DONE,
		ASTPRM = TVT);
	OKINT;
	};
    };

TELNET_CLOSE_DONE(TVT) : NOVALUE (void)
!
// Finish deallocation of a TVT, after all AST's have been delivered.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    EXTERNAL ROUTINE
	LIB$FREE_VM		: BLISS ADDRESSING_MODE(GENERAL),
	LIB$FREE_VM_PAGE	: BLISS ADDRESSING_MODE(GENERAL);

// Just deallocate the TVT structure and return.

//    LIB$FREE_VM(%REF(TVT$SIZE*4),TVT);
    LIB$FREE_VM_PAGE(%REF(((TVT$SIZE * 4) / 512) + 1),TVT);
    };

//SBTTL "TELNET_INPUT - Do TCP input for TELNET connection"
/*
    Process incoming network data for a TELNET connection, handling any
    necessary option negotiations and passing any user data to the PTY
    associated with this connection. Called from segment input handler when
    TCP receive window becomes non-empty.
*/

TELNET_INPUT(TCB) : NOVALUE (void)
    {
    MAP
	STRUCT TCB_STRUCTURE * TCB;

// Call TCP input routine

//IF USE_ASTS %THEN
    $DCLAST(ASTADR = PTY_WRITE,
	    ASTPRM = TCB->TVTDATA);
//else
    PTY_WRITE(TCB->TVTDATA);
//FI
    };

//SBTTL "TELNET_OUTPUT - Do TCP output for TELNET connection"
/*
    Obtain some data to output to the network whenever the TCP send queue
    becomes non-empty. Normally, the PTY read done AST will put data onto the
    TCP output queue (via TCP_WRITE) whenever it arrives over the PTY. When the
    queue fills, however, it effectively "blocks", without issuing further
    PTY reads, until the queue becomes non-full (i.e. the send window opens).
*/

TELNET_OUTPUT(TCB) : NOVALUE (void)
    {
    MAP
	STRUCT TCB_STRUCTURE * TCB;

// Just call TCP write with the TVT's TCB

//IF USE_ASTS %THEN
    $DCLAST(ASTADR = PTY_READ,
	    ASTPRM = TCB->TVTDATA);
//else
    PTY_READ(TCB->TVTDATA);
//FI
    };

//SBTTL "Routines to interface to the network"

FORWARD ROUTINE
 VOID    TVT_READ_WILL,
 VOID    TVT_READ_WONT,
 VOID    TVT_READ_DO,
 VOID    TVT_READ_DONT,
 VOID    TVT_READ_SUB ;

TCP_READ(TVT) : NOVALUE (void)
!
// TCP_READ - Read as much data as possible from the network into the PTY write
// buffer. If no write is yet in progress, start a write now.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	LCLOPTS = TVT->TVT$LCLOPTS : OPT$LIST ;
    EXTERNAL ROUTINE
	IS_CNTRLT_GOOD;
    signed long
	STRUCT TCB_STRUCTURE * TCB,
	Byte_Count,
	Byte_Limit,
	Prev_Char,
	CHRPTR,
	CHRCNT,
	CHWPTR,
	CHR,
	NEGCNT ;
    LABEL
	X;

// Empty out PTY, so we can accept more input

//    PTY_READ(TVT) ;

// If the PTY write buffer is busy, then don't run here.

    if (TVT->TVT$PWRITE)
	RETURN;

// Is there any data to write?

    TCB = TVT->TVT$TCB;
    if (TCB->RCV_Q_COUNT <= 0)
	RETURN;

// If already doing a net read, then don't run here.

    if (TVT->TVT$NREAD)
	RETURN;

// Indicate that we're doing a network read

    TVT->TVT$NREAD = TRUE;
    NEGCNT = TVT->TVT$NEG_CNT;

// We have some data to write. Copy from network to the PTY buffer
// N.B. Need to be careful of synchonization problem with CQ_xxx routines.

    CHRCNT = TCB->RCV_Q_COUNT;
    CHRPTR = TCB->RCV_Q_DEQP;
//    CHWPTR = TVT->TVT$WR_PTR; // = CH$PTR(TVT->TVT$WR_BUF);
    Byte_Limit = MIN(CHRCNT, (TVT_TTY_BUFLEN - TVT->TVT$WR_BCNT)) ;
    CHR = CH_NUL ;

//   Check to see if there is enough buffer space remaining

//    if ((TVT->TVT$WR_BCNT GEQ (TVT_TTY_BUFLEN - PTY_BUFFER_SIZE)))
    if ((TVT->TVT$WR_BCNT GEQ (TVT_TTY_BUFLEN - 2)))
	{
	TVT->TVT$GAG = TRUE ;
	}
    else
	{
	TVT->TVT$GAG = FALSE ;
	} ;

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB x!XL TCP_READ: CHRCNT = !SL, BCNT = !SL limit = !SL!/",
		0, TVT->TVT$TCB, CHRCNT, TVT->TVT$WR_BCNT, Byte_Limit) ;
	};

    TVT->TVT$CTRL = FALSE ;
//    WHILE ((CHRCNT > 0) && (TVT->TVT$WR_BCNT <= (TVT_TTY_BUFLEN - 1))) DO
//    WHILE ((CHRCNT > 0) && (NOT TVT->TVT$GAG)) DO
    WHILE ((Byte_Limit > 0) && (NOT TVT->TVT$GAG)) DO
	{

// Read another character from the network buffer, wrapping pointer as needed

	Prev_Char = CHR ;
	CHR = CH$RCHAR_A(CHRPTR);
	if (CHRPTR GEQ TCB[RCV_Q_}])
	    CHRPTR = TCB->RCV_Q_BASE;
	CHRCNT = CHRCNT - 1;
	Byte_Limit = Byte_Limit - 1 ;

	if ($$LOGF(LOG$TVT))
	    {
	    LOG$FAO("!%T TCB x!XL TCP_READ: CHRCNT = !SL, BCNT = !SL limit = !SL CHR = x!XB!/",
		0, TVT->TVT$TCB, CHRCNT, TVT->TVT$WR_BCNT, Byte_Limit, CHR) ;
	    };

// Handle this character based on the current state

	CASE TVT->TVT$NRSTATE FROM TVT$STATE_MIN TO TVT$STATE_MAX OF
	SET
	[TVT$STATE_NORMAL]:	// Normal state
X:	    {
	    if (CHR == TELNET$K_IAC)
		{

// Have an IAC - enter IAC state.

		TVT->TVT$NRSTATE = TVT$STATE_IAC;
		LEAVE X;
		};

// If reading subnegotiation string, then append byte to subnegotiation buffer

	    if (TVT->TVT$NR_SB)
		{
		if (TVT->TVT$SUB_CNT LSS TVT_SUB_BUFLEN)
		    {
		    CH$WCHAR_A(CHR,TVT->TVT$SUB_PTR);
		    TVT->TVT$SUB_CNT = TVT->TVT$SUB_CNT + 1
		    };
		LEAVE X;
		};

// If we're in BINARY mode, just output the character

	    if (LCLOPTS[TELNET$K_BINARY,OPT$STATE])
		{
		Net_To_PTY(TVT, CHR) ;
		}
	    else
		{

// For non BINARY mode, check for special processing following CR

		if (TVT->TVT$NR_CR)
		    {

// If we had a CR as the last character, then do special processing of char


		    SELECTONE CHR OF
		    SET
		    [CH_NUL]:	// Null - fake a LF
			{
!!!HWM			Net_To_PTY(TVT, CH_LF) ;
			TVT->TVT$NR_CR = FALSE;
			};

		    [CH_CR]:	// Another CR - append it & retain CR state
			{
			Net_To_PTY(TVT, CHR) ;
			};

		    [CH_LF]:	// LF after CR - drop LF and reset CR state
			{

!~~~ This code actually violates RFC 854, but is necessary to deal with many
!~~~ shithead UNIX systems which send CR-LF-LF when the user types CR-LF.
!~~~ We should be keeping the LF here.
!~~~ (We are, now.  HWM 4-Nov-91)
!!!			Net_To_PTY(TVT, CHR) ;
			TVT->TVT$NR_CR = FALSE;
			TVT->TVT$NR_LF = TRUE ;
			};

		    [OTHERWISE]: // Funny state - append and reset CR state
			{
			Net_To_PTY(TVT, CHR) ;
			TVT->TVT$NR_CR = FALSE;
			TVT->TVT$NR_LF = FALSE ;
			};
		    TES;
		    }
		else
		    {

!!// Didn't have a previous CR. Check for LF now and prepend a CR

!!!JC                    if (CHR == CH_LF)
!!!JC 			{
!!!JC 			TVT->TVT$NR_LF = TRUE ;
!!!JC !!!HWM			Net_To_PTY(TVT, CH_CR) ;
!!!JC 			if ($$LOGF(LOG$TVT))
!!!JC 			    {
!!!JC 			    LOG$FAO("!%T TCB x!XL TCP_READ: ADD CR, BCNT = !SL!/",
!!!JC 				0, TVT->TVT$TCB, TVT->TVT$WR_BCNT) ;
!!!JC 			    };
!!!JC 			} ;

// Didn't have a previous CR. Check for one now and output the byte.

		    if (CHR == CH_CR)
			{
			TVT->TVT$NR_CR = TRUE ;
			TVT->TVT$NR_LF = FALSE ;
			} ;
		    Net_To_PTY(TVT, CHR) ;
		    };
		};
	    };

	[TVT$STATE_IAC]:	// IAC - Start negotiation
	    {

// Select the different types of negotiations

	    XLOG$FAO(LOG$TELNEG,"!%T Negotiate !UB/",0,CHR);

	    SELECTONE CHR OF
	    SET
	    [TELNET$K_IAC]:	// Another IAC. Send one IAC to terminal
		{
		Net_To_PTY(TVT, CHR) ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};

	    [TELNET$K_WILL]:	// Remote WILL handle an option
		TVT->TVT$NRSTATE = TVT$STATE_WILL;

	    [TELNET$K_WONT]:	// Remote WONT handle an option
		TVT->TVT$NRSTATE = TVT$STATE_WONT;

	    [TELNET$K_DO]:	// Remote tells us DO an option
		TVT->TVT$NRSTATE = TVT$STATE_DO;

	    [TELNET$K_DONT]:	// Remote tells us DONT do an option
		TVT->TVT$NRSTATE = TVT$STATE_DONT;

	    [TELNET$K_SB]:	// Remote is starting subnegotiation
		{
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		TVT->TVT$NR_SB = TRUE;
		TVT->TVT$SUB_PTR = TVT->TVT$SUB_BUF;
		TVT->TVT$SUB_CNT = 0;
		};

	    [TELNET$K_SE]:	// Remote is finished with subnegotiation
		{
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		TVT->TVT$NR_SB = FALSE;
		TVT->TVT$SUB_PTR = TVT->TVT$SUB_BUF;
		TVT_READ_SUB(TVT)
		};

	    [TELNET$K_AYT]:	// User is nervous.  Comfort them.
		{
		if (NOT IS_CNTRLT_GOOD(TVT))
		    {
		    TCP_ADD_STRING(TVT,AYT_RESPONSE);
		    } ;
		Net_To_PTY(TVT, Telnet$K_Char_AYT) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL
		};

	    [TELNET$K_Brk]:	// I think VMS ignores the break key.
		{
		Net_To_PTY(TVT, Telnet$K_Char_Brk) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL
		};

	    [TELNET$K_IP]:	// Interrupt process
		{
		Net_To_PTY(TVT, Telnet$K_Char_IP) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL
		};

	    [TELNET$K_AO]:	// Abort Output
		{
		Net_To_PTY(TVT, Telnet$K_Char_AO) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};

	    [TELNET$K_EC]:	// Erase Character
		{
		Net_To_PTY(TVT, Telnet$K_Char_EC) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};

	    [TELNET$K_EL]:	// Erase Line
		{
		Net_To_PTY(TVT, Telnet$K_Char_EL) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};

	    [TELNET$K_EOR]:	// End of Record
		{
		Net_To_PTY(TVT, CH_CR) ;	// Fake it by stuffing a CR
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};

	    [TELNET$K_Data_Mark]:		// Data Mark
		{
		Net_To_PTY(TVT, Telnet$K_Char_PURGE) ;	// Purge typeahead
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};

	    [OTHERWISE]:	// Garbage to us
		{
		XLOG$FAO(LOG$TELNEG,"!%T Garbage !UB/",0,CHR);
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
	    TES;
	    };

	[TVT$STATE_WILL]:	// Process the WILL according to option state
	    TVT_READ_WILL(TVT,CHR);

	[TVT$STATE_WONT]:	// Process the WONT
	    TVT_READ_WONT(TVT,CHR);

	[TVT$STATE_DO]:		// Process the DO
	    TVT_READ_DO(TVT,CHR);

	[TVT$STATE_DONT]:	// Process the DONT
	    TVT_READ_DONT(TVT,CHR);

	[INRANGE,OUTRANGE]:	// Shouldn't ever get here.
	    0;
	TES;

!~~~ Update receive window information.

	TCB->RCV_WND = TCB->RCV_WND + 1 ;
	TCB[P}ING_ACK] = TRUE;

// Update network receive queue pointer and count.

	TCB->RCV_Q_DEQP = CHRPTR;
	TCB->RCV_Q_COUNT = TCB->RCV_Q_COUNT - 1 ;

	if ((TVT->TVT$CTRL))
	    EXITLOOP ;
	};

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB x!XL TCP_READ: BCNT = !SL!/",
		0, TVT->TVT$TCB, TVT->TVT$WR_BCNT) ;
	};

// Indicate that we're no longer doing a network read

    TVT->TVT$NREAD = FALSE;

// If we moved some data, then force a ACK now

    if ((TCB[P}ING_ACK]))
	{
	TCP$Enqueue_Ack(TCB) ;
	} ;

    };


VOID PTY_TO_NET(TVT, CHR) (void)
!
// PTY_TO_NET - Write a byte of data as to the network buffer.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	STRUCT TCB_STRUCTURE * TCB,
	CHWMAX,
	CHWCNT,
	CHWPTR ;

// Make sure there is some room in the network output queue

    TCB = TVT->TVT$TCB;
    CHWMAX = TCB->SND_Q_SIZE-TCB->SND_Q_COUNT-TCB->SRX_Q_COUNT;

//    if ($$LOGF(LOG$TVT))
!	{
!	LOG$FAO("!%T TCB x!XL PTY_TO_NET: CHWMAX = !SL!/",
!		0, TVT->TVT$TCB, CHWMAX) ;
!	};

    if (CHWMAX <= 0)
	RETURN;

// If already doing a net write, then don't run here.

    if (TVT->TVT$NWRITE)
	RETURN;

// Indicate that we have a write in progress

    TVT->TVT$NWRITE = TRUE;

// Copy bytes from the PTY read buffer to the network, quoting IAC's as needed.

    CHWCNT = 0;
    CHWPTR = CH$PTR(TCB->SND_Q_ENQP);

// Write a character to the output buffer, taking care to wrap the pointer

    CH$WCHAR_A(CHR,CHWPTR);
    if (CHWPTR GEQU TCB[SND_Q_}])
	CHWPTR = TCB->SND_Q_BASE;

// Update the network queue pointer and counter

    TCB->SND_Q_ENQP = CHWPTR;
    TCB->SND_Q_COUNT = TCB->SND_Q_COUNT + 1 ;
    TCB->SND_PPTR = TCB->SND_NXT + TCB->SND_Q_COUNT;
    TCB->SND_PUSH_FLAG = TRUE;

// If the send queue just became nonempty, then schedule a wakeup so that this
// TCB will be serviced soon.

//    if (TCB->SND_Q_COUNT == CHWCNT)
    if (TCB->SND_Q_COUNT == TCB->MAX_EFF_DATA_SIZE)
	$ACPWAKE;

// Indicate that we're not in the network write code any more

    TVT->TVT$NWRITE = FALSE;
    };


TCP_WRITE(TVT) : NOVALUE (void)
!
// TCP_WRITE - Write as much data as possible from the PTY read buffer to the
// network.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	STRUCT TCB_STRUCTURE * TCB,
	Now	:	UNSIGNED,
	CHWMAX,
	CHWCNT,
	CHWPTR,
	CHR;

// Make sure there is some room in the network output queue

    TCB = TVT->TVT$TCB;
    CHWMAX = TCB->SND_Q_SIZE-TCB->SND_Q_COUNT-TCB->SRX_Q_COUNT;
    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB !XL TCP_WRITE: CHWMAX = !SL NWRITE=!UB!/",
		0, TVT->TVT$TCB, CHWMAX, TVT->TVT$NWRITE) ;
	};

    if (CHWMAX <= 0)
	{
// Something got jammed - gotta force a write
!	TCP$Send_Data(TCB) ;
	$ACPWAKE ;
	RETURN;
	} ;
// If already doing a net write, then don't run here.

    if (TVT->TVT$NWRITE)
	RETURN;

// Indicate that we have a write in progress

    TVT->TVT$NWRITE = TRUE;

// Copy bytes from the PTY read buffer to the network, quoting IAC's as needed.


    CHWCNT = 0;
    CHWPTR = CH$PTR(TCB->SND_Q_ENQP);
    WHILE TRUE DO
	{

// Do we need to send a doubled IAC?

	if (TVT->TVT$NW_IAC)
	    {
	    CHR = TELNET$K_IAC;
	    TVT->TVT$NW_IAC = FALSE;
	    }
	else
	    {

// If there are negotiation bytes to send, then do them first.

	    if (TVT->TVT$NEG_CNT NEQU 0)
		{

// Get next byte from negotiation buffer and send to net

		CHR = CH$RCHAR_A(TVT->TVT$NEG_DQP);
		if ((TVT->TVT$NEG_CNT = TVT->TVT$NEG_CNT-1) == 0)
		    TVT->TVT$NEG_DQP = CH$PTR(TVT->TVT$NEG_BUF);
		}
	    else
		{

// Get a character and check for an IAC that needs to be quoted

		if (TVT->TVT$RD_BCNT <= 0)
		    EXITLOOP;
		CHR = CH$RCHAR_A(TVT->TVT$RD_PTR);
		if (CHR == TELNET$K_IAC)
		    TVT->TVT$NW_IAC = TRUE;
		TVT->TVT$RD_BCNT = TVT->TVT$RD_BCNT - 1;
		};
	    };

// Write a character to the output buffer, taking care to wrap the pointer

	CH$WCHAR_A(CHR,CHWPTR);
	if (CHWPTR GEQU TCB[SND_Q_}])
	    CHWPTR = TCB->SND_Q_BASE;
	if ((CHWCNT = CHWCNT + 1) GEQ CHWMAX)
	    EXITLOOP;
	};

// Update the network queue pointer and counter

    TCB->SND_Q_ENQP = CHWPTR;
    TCB->SND_Q_COUNT = TCB->SND_Q_COUNT + CHWCNT;
    TCB->SND_PPTR = TCB->SND_NXT + TCB->SND_Q_COUNT;
    TCB->SND_PUSH_FLAG = TRUE;

// If the send queue just became nonempty, then schedule a wakeup so that this
// TCB will be serviced soon.

    Now = Time_Stamp() ;
    if ($$LOGF(LOG$TVT || LOG$TCP))
	{
	LOG$FAO("!%T TCB !XL TCP_WRITE: SNDQCNT= !SL SRXQCNT=!SL!/",
		0, TVT->TVT$TCB, TCB->SND_Q_COUNT, TCB->SRX_Q_COUNT) ;
	LOG$FAO("!%T TCB !XL TCP_WRITE: NOW= !SL Delay=!SL!/",
		0, TVT->TVT$TCB, Now, TCB->SND_Delay_Timer) ;
	};

//    if (TCB->SND_Q_COUNT == CHWCNT)
    IF (((TCB->SND_Q_COUNT + TCB->SRX_Q_COUNT) GEQ TCB->MAX_EFF_DATA_SIZE) OR
	(Now GEQU TCB->SND_Delay_Timer)) THEN
	$ACPWAKE;
!	TCP$Send_Data(TCB) ;

// Indicate that we're not in the network write code any more

    TVT->TVT$NWRITE = FALSE;
    };



TCP_ADD_STRING(TVT,STRDESC_A) : NOVALUE (void)
!
// TCP_ADD_STRING - Write as much data as possible from the supplied string
// to the network write buffer via PTY_TO_NET.
!
    {
    BIND
	STRDESC = STRDESC_A : $BBLOCK;
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	STRUCT TCB_STRUCTURE * TCB,
	CHWMAX,
	CHWCNT,
	CHWPTR,
	CHRCNT,
	CHRPTR,
	CHR;

    CHRCNT = STRDESC->DSC$W_LENGTH;
    CHRPTR = CH$PTR(STRDESC->DSC$A_POINTER);

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB !XL TCP_ADD_STRING: CNT !UL STR !AS!/",
	    0, TVT->TVT$TCB, CHRCNT, STRDESC) ;
	};

// Make sure there is some room in the PTY read queue

    if (TVT->TVT$RD_BCNT GEQ (TVT_TTY_BUFLEN-.CHRCNT))
	RETURN;

    CHWPTR = TVT->TVT$RD_PTR = CH$PTR(TVT->TVT$RD_BUF);

    WHILE (CHRCNT > 0) DO
	{
	CHR = CH$RCHAR_A(CHRPTR);
	PTY_TO_NET(TVT, CHR) ;
	CHRCNT = CHRCNT - 1;
	};

    };

VOID NET_TO_PTY(TVT, CHR) (void)
!
// NET_TO_PTY - Move a byte from the net buffer to PTY buffer and force a
// write, if possible
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	IO_Status = TVT->TVT$WR_IOSB : PTY$IOSB ;
    signed long
	Byte_Count,
	Bytes_Remaining,
	TT_WR_PTR,
	RC;

// Empty PTY buffers

//    PTY_READ(TVT) ;

//   Check to see if there is enough buffer space remaining

//    if ((TVT->TVT$WR_BCNT GEQ (TVT_TTY_BUFLEN - PTY_BUFFER_SIZE)))
    if ((TVT->TVT$WR_BCNT GEQ (TVT_TTY_BUFLEN - 2)))
	{
	TVT->TVT$GAG = TRUE ;
	}
    else
	{
	TVT->TVT$GAG = FALSE ;
	} ;

    if ((TVT->TVT$WR_BCNT GEQ (TVT_TTY_BUFLEN - 1)))
	{
	if ($$LOGF(LOG$TVT))
	    {
	    LOG$FAO("!%T TCB x!XL NET_TO_PTY: CHR = x!XB, PTY Buffer full!/",
		0, TVT->TVT$TCB, CHR) ;
	    };
	RETURN ;
	} ;

//   Stuff character in PTY write buffer, update pointers

    TT_WR_PTR = CH$PTR(TVT->TVT$WR_BUF, TVT->TVT$WR_IPTR) ;
    CH$WCHAR_A(CHR, TT_WR_PTR) ;
    TVT->TVT$WR_ICNT = TVT->TVT$WR_ICNT + 1 ;
    TVT->TVT$WR_BCNT = TVT->TVT$WR_BCNT + 1 ;
    TVT->TVT$WR_IPTR = TVT->TVT$WR_IPTR + 1 ;

    if ((TVT->TVT$WR_IPTR GEQ TVT_TTY_BUFLEN))
	{
	TVT->TVT$WR_IPTR = 0 ;
	} ;

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB x!XL NET_TO_PTY: CHR = x!XB, ICNT = !SL, IPTR = !SL!/",
		0, TVT->TVT$TCB, CHR, TVT->TVT$WR_ICNT, TVT->TVT$WR_IPTR) ;
	};

//   Compute realistic byte count

    Byte_Count = MIN(TVT->TVT$WR_BCNT, TVT_TTY_BUFLEN) ;

// Do logging...

    if ($$LOGF(LOG$TVT))
	{
	TT_WR_PTR = CH$PTR(TVT->TVT$WR_BUF, TVT->TVT$WR_IPTR) ;
	LOG$FAO("!%T TCB x!XL NET_TO_PTY: #bytes=!SL, Byte Count = !SL,Data=[!AD]!/",
		0,TVT->TVT$TCB, TVT->TVT$WR_BCNT, Byte_Count,
		TVT_TTY_BUFLEN, TVT->TVT$WR_BUF);
	};

    };



//SBTTL "Routines to interface to the PTY"

PTY_SET_OWNER_PID(TVT) : NOVALUE (void)
!
// PTY_SET_OWNER_PID : Fill in the User_ID field of the associated TCB
!	by interrogating the appropriate device for the TELNET session.
!
// While we're here, we call POKEADDR to give it the information we would
// like (also to the VMS accounting file).
!
   {
   MAP
	struct TVT$BLOCK * TVT;
   signed long
	STRUCT TCB_STRUCTURE * TCB,
	devstr : VECTOR[20,Byte],
	devnam : $BBLOCK->DSC$K_S_BLN,
!!!	devsln,
	ptynambuf : VECTOR[20,BYTE],
	ptynam : $BBLOCK->DSC$K_S_BLN PRESET (
		[DSC$W_LENGTH] = %ALLOCATION(ptynambuf),
		[DSC$B_DTYPE] = DSC$K_DTYPE_Z,
		[DSC$B_CLASS] = DSC$K_CLASS_Z,
		[DSC$A_POINTER] = ptynambuf),
	RC,
	Unit_Number,
	Owner_PID,
	Owner_UIC,
	FHost,FPort,
	Item_List		: $ITMLST_DECL (Items = 3);
    EXTERNAL ROUTINE
	PokeAddr : ADDRESSING_MODE(GENERAL);

// point at the TCB for this TVT.
    TCB = TVT->TVT$TCB;
    FHost = TCB->Foreign_Host;

// fill in the owner pid. First get the unit number of the pty device,
// then build a request for the owner pid of the device associated with it.

    $ITMLST_INIT (ITMLST = Item_List,
	(ITMCOD = DVI$_UNIT, BUFADR = Unit_Number));

    RC = $GETDVIW (ITMLST = Item_List,CHAN=TVT->TVT$PTY_CHN);
    if (RC)
	{
	$ITMLST_INIT(ITMLST = Item_List,
	    (ITMCOD = DVI$_PID, BUFADR = Owner_Pid),
	    (ITMCOD = DVI$_OWNUIC, BUFADR = Owner_UIC));

// make a descriptor to hold the device name:

	devnam->DSC$B_DTYPE = DSC$K_DTYPE_Z;
	devnam->DSC$B_CLASS = DSC$K_CLASS_Z;
	devnam->DSC$W_LENGTH = %ALLOCATION(devstr);
	devnam->DSC$A_POINTER = devstr;

// generate this device name string using $FAO:

	ptynam->DSC$W_LENGTH = %ALLOCATION(ptynambuf);

	RC = $TRNLOG(
		LOGNAM=%ASCID"INET$PTY_TERM",		// JC
		RSLLEN=ptynam->DSC$W_LENGTH,		// JC
		RSLBUF=ptynam);				// JC
	if (NOT RC) TVT->TVT$DO_PID = 0;		// Cancel
	if (RC == SS$_Normal)
	    RC = $FAO(%ASCID"_!ASA!UL:",devnam,devnam,ptynam,Unit_Number);
	    if (RC)
		RC = $GETDVIW (ITMLST = Item_List, DEVNAM = devnam);

    xlog$fao(LOG$TELNET,"!%T PTY_Set_owner_PID: TTY_TERM="!AS"!/",0,devnam);

// free the string descriptor, we don't need it any more.
!	LIB$FREE_VM(devsln,devstr);

// check return status from the $GETDVIW & $FAO calls.
	if (RC && (Owner_Pid != 0))
	    {

	    // finally, we know the PID, so we can fill in the owner field
	    // of the appropriate TCB.
	    TCB->User_ID=.Owner_Pid;
	    TVT->TVT$DO_PID = 0;

	    // Also, set up the information in the remote process'
	    // P1 space...
	    PokeAddr(Owner_Pid,  TCB->Foreign_Host, TCB->Foreign_Port);

	    // Note the connection in the activity log file.
	    ACT$FAO(
		"!%D Telnet-in (PID:x!XW UIC:!%U) <!UB.!UB.!UB.!UB/!UW>!/",0,
		.Owner_PID<0,16,0>, Owner_UIC,
		.FHost<0,8>,FHost<8,8>,FHost<16,8>,FHost<24,8>,
		TCB->Foreign_Port);
!
!	Now set any delayed device dependent
!
	    Set_DEVDEP(TVT);				// JC
	    PTY_Write(TVT);				// JC Write after Hold off
	    };
	};
    };



FORWARD ROUTINE
 VOID    PTY_READ_DONE;

PTY_READ(TVT) : NOVALUE (void)
!
// PTY_READ - Initiate a read on the PTY device, if there is room in the PTY
// read buffer.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	STRUCT TCB_STRUCTURE * TCB,
	Byte_Count,
	RC;

// Make sure there is some room in the network output queue

    TCB = TVT->TVT$TCB;
    Byte_Count = TCB->SND_Q_SIZE-TCB->SND_Q_COUNT-TCB->SRX_Q_COUNT;

    XLOG$FAO(LOG$TVT,
	"!%T TVT PTY_read: TCB x!XL, BC=!SL, RDCNT=!SL, NGCNT=!SL!/",
	0, TVT->TVT$TCB, Byte_Count, TVT->TVT$RD_BCNT, TVT->TVT$NEG_CNT);

// Empty out the TCP buffers

    if (((TVT->TVT$NEG_CNT > 0) || (TVT->TVT$RD_BCNT > 0)))
	{
	TCP_WRITE(TVT) ;
	} ;

// Ignore this if there is already a PTY read in progress.

    if (TVT->TVT$PREAD)
	RETURN;
    TVT->TVT$PREAD = TRUE;

// Compute realistic byte count

    Byte_Count = MIN(Byte_Count, TVT_TTY_BUFLEN) ;

// Initiate a read on the PTY device

    RC = $QIO(
	    CHAN	= TVT->TVT$PTY_CHN,
	    FUNC	= IO$_READVBLK,
	    IOSB	= TVT->TVT$RD_IOSB,
	    ASTADR	= PTY_READ_DONE,
	    ASTPRM	= TVT,
	    P1		= TVT->TVT$RD_BUF,
	    P2		= Byte_Count);
    if (NOT RC)
	{
	XLOG$FAO(LOG$TCPERR,
		 "!%T TVT PTY read $QIO failure for TCB x!XL, RC=x!XL!/",
		 0,TVT->TVT$TCB,RC);
	TCB$DELETE(TVT->TVT$TCB);
	};
    };

PTY_READ_DONE(TVT) : NOVALUE (void)
!
// AST routine when PTY read operation finishes. Attempt to write the new data
// to the network, via TCP_WRITE.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	IOSB = TVT->TVT$RD_IOSB : PTY$IOSB;
    signed long
        STRUCT TCB_STRUCTURE * TCB,
	Byte_Count,
	CHR,
	RC;

// Make sure the TVT is still valid

    if (TVT->TVT$CANCEL)
	RETURN;

// Check the status of the read. SS$_CANCEL/SS$_ABORT are ignored.

    RC = IOSB->PTSB$STATUS;
    if ((RC == SS$_CANCEL) || (RC == SS$_ABORT))
	RETURN;
    if (NOT RC)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT read error for TCB x!XL, RC=x!XL!/",
		 0,TVT->TVT$TCB,RC);
	TCB$DELETE(TVT->TVT$TCB);
	RETURN;
	};

//    AST_IN_PROGRESS = TRUE;
    NOINT ;

// Clear read-in-progress and set number of bytes read.

    TVT->TVT$RD_BCNT = IOSB->PTSB$NBYTES;
    TVT->TVT$RD_PTR = CH$PTR(TVT->TVT$RD_BUF);
    TVT->TVT$PREAD = FALSE;

//    AST_IN_PROGRESS = FALSE;
    OKINT;

// check the User_ID field of the TCB. IF zero, we want to fill it in
// with the process ID of the connected TELNET session.
    TCB = TVT->TVT$TCB;
    if (TVT->TVT$DO_PID)
	{

!JC ----------------- Kludge -------------------------
//  The followin test is to make sure that we have an initial login prompt
//  This assumes that the prompt is longer than 7 but less than 15.
//  The actual length is 12, but what the heck give DEC some leeway.
//  Once we have a prompt it is save to do other operations.
//  In particular data written to PTY before the prompt is "promptly" lost!!!
!JC ----------------- Kludge -------------------------

	IF (TVT->TVT$RD_BCNT > 7) AND		// JC Kludge
	   (TVT->TVT$RD_BCNT LSS 15)			// JC Kludge
	THEN						// JC Kludge
		PTY_SET_OWNER_PID(TVT);
	};

// Print debug info

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB x!XL PTY_READ_DONE: #bytes=!SL,Data=[!AD]!/",
		0, TVT->TVT$TCB, TVT->TVT$RD_BCNT,
		TVT->TVT$RD_BCNT, TVT->TVT$RD_BUF);
	};

// Give this data to the network.

//    WHILE (TVT->TVT$RD_BCNT > 0) DO
!	{
!	CHR = CH$RCHAR_A(TVT->TVT$RD_PTR) ;
!	PTY_TO_NET(TVT, CHR) ;
!	TVT->TVT$RD_BCNT = TVT->TVT$RD_BCNT - 1 ;
!	} ;

    if (NOT TVT->TVT$NWRITE)
	{
//IF USE_ASTS %THEN
	TCP_WRITE(TVT);
	Byte_Count = TCB->SND_Q_SIZE-TCB->SND_Q_COUNT-TCB->SRX_Q_COUNT;
	if (((TVT->TVT$RD_BCNT > 0) || (Byte_Count > 0)))
	    {
	    $DCLAST(ASTADR = PTY_READ,
		ASTPRM = TVT);
	    } ;
//else
	$ACPWAKE;
//FI
	} ;

    };

FORWARD ROUTINE
 VOID    PTY_WRITE_DONE;

PTY_WRITE(TVT) : NOVALUE (void)
!
// PTY_WRITE - Initiate a write to the PTY device, if there is data in the PTY
// write buffer.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	IO_Status = TVT->TVT$WR_IOSB : PTY$IOSB;
    signed long
	Byte_Count,
	Bytes_Remaining,
	TT_WR_PTR,
	PTY_Char	: BLOCK[8,BYTE],
	RC;

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB x!XL PTY_WRITE: OCNT = !SL, OPTR = !SL!/",
		0, TVT->TVT$TCB, TVT->TVT$WR_OCNT, TVT->TVT$WR_OPTR);
	};

// If already doing a PTY write, then don't run here.

    if (TVT->TVT$PWRITE)
	RETURN;

    IF TVT->TVT$DO_PID
    THEN {		!!// JC hold off till proc ready
	if (TVT->TVT$HOLD) RETURN;
	TVT->TVT$HOLD = 1;
	};

// Fill the TCP buffer

	TCP_READ(TVT) ;

// If buffer empty, then don't run here.

    if ((TVT->TVT$WR_BCNT == 0))
	RETURN;

// Indicate that there is now a write in progress

    TVT->TVT$PWRITE = TRUE;

//   Get size of typeahead buffer
//    (This code doesn"t really work - TYPEAHDCNT apparently doesn"t work
//    for PTY's...)
//    (Have to put chan first?  What a crock...  -HWM)
//    RC = $QIOW(
!	    CHAN   = TVT->TVT$PTY_CHN,
!	    FUNC   = IO$_SENSEMODE+IO$M_TYPEAHDCNT,
!	    IOSB   = TVT->TVT$WR_IOSB,
!	    P1     = PTY_Char,
!	    P2     = 8);
//    if (NOT RC)
!	{
!	XLOG$FAO(LOG$TCPERR,
!		 "!%T TVT PTY sensemode $QIO failure for TCB x!XL, RC=x!XL!/",
!		 0, TVT->TVT$TCB, RC);
!	} ;
!
//    Bytes_Remaining = PTY_Char->IO$V_TYPEAHDCNT ;
!
//    if ($$LOGF(LOG$TVT))
!	{
!	LOG$FAO("!%T TCB x!XL PTY_WRITE: Bytes remaining = !SL, Status = !SL!/",
!		0, TVT->TVT$TCB, Bytes_Remaining, IO_STATUS->PTSB$STATUS) ;
!	};

//   Compute realistic byte count

    if ((TVT->TVT$WR_OPTR LSS TVT->TVT$WR_IPTR))
	{
	// OPTR trailing IPTR, expected case, compute distance
	Byte_Count = (TVT->TVT$WR_IPTR - TVT->TVT$WR_OPTR) ;
	}
    else
	{
	// OPTR equals IPTR, nothing else to do here
	if ((TVT->TVT$WR_OPTR == TVT->TVT$WR_IPTR))
	    {
	    TVT->TVT$PWRITE = FALSE ;
	    RETURN ;
	    } ;
	// IPTR trailing OPTR, IPTR wrapped, compute linear size remaining
	Byte_Count = TVT_TTY_BUFLEN - TVT->TVT$WR_OPTR ;
	} ;

//    Byte_Count = MIN(Byte_Count, PTY_BUFFER_SIZE) ;
    TT_WR_PTR = CH$PTR(TVT->TVT$WR_BUF, TVT->TVT$WR_OPTR) ;

    if (TVT->TVT$DO_PID) Byte_Count = 1;		// JC For hold off

// Do logging...

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB x!XL PTY_WRITE: #bytes=!SL, Byte Count = !SL,Data=[!AD]!/",
		0,TVT->TVT$TCB, TVT->TVT$WR_BCNT, Byte_Count,
		.Byte_Count, TVT->TVT$WR_BUF) ;
	};

// Initiate the write on the PTY

//    opr$fao("pty_write !SL bytes (x!XL,x!XL)",TVT->TVT$WR_BCNT,TVT->TVT$WR_BUF,TVT->TVT$WR_PTR);
    RC = $QIO(
	    FUNC   = IO$_WRITEVBLK,
	    CHAN   = TVT->TVT$PTY_CHN,
	    IOSB   = TVT->TVT$WR_IOSB,
	    ASTADR = PTY_WRITE_DONE,
	    ASTPRM = TVT,
	    P1     = TT_WR_PTR,
	    P2     = Byte_Count);
    if (NOT RC)
	{
	XLOG$FAO(LOG$TCPERR,
		 "!%T TVT PTY write $QIO failure for TCB x!XL, RC=x!XL!/",
		 0,TVT->TVT$TCB,RC);
!	if (IO_Status->PTSB$EXTRA1 != SS$_DATAOVERUN)
	    TCB$DELETE(TVT->TVT$TCB);
	TVT->TVT$PWRITE = FALSE;
	RETURN;
	};

    };

PTY_WRITE_DONE(TVT) : NOVALUE (void)
!
// PTY_WRITE_DONE - AST routine when PTY write operation finishes. Try to get
// more data for the pty via TCP_READ.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	IOSB = TVT->TVT$WR_IOSB : PTY$IOSB;
    signed long
	bytes_left,
	bytes_written,
	RC,
	STRUCT TCB_STRUCTURE * TCB;

// Make sure the TVT is still valid

    if (TVT->TVT$CANCEL)
	{
	RETURN;
	} ;

// Check the status of the operation.

    RC = IOSB->PTSB$STATUS;
    if (RC == SS$_CANCEL)
	{
	RETURN;
	} ;

    TCB = TVT->TVT$TCB;
    if (NOT RC)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT write error for TCB x!XL, RC=x!XL!/",
		 0,TCB,RC);
	if (RC != SS$_DATAOVERUN)
	    {
	    TCB$DELETE(TVT->TVT$TCB);
	    RETURN
	    };
	};

//    AST_IN_PROGRESS = TRUE;
//    NOINT ;

//   Update counters and pointers

    bytes_written = IOSB->PTSB$NBYTES;
    bytes_left = TVT->TVT$WR_BCNT - Bytes_written ;
    TVT->TVT$WR_OPTR = TVT->TVT$WR_OPTR + bytes_Written ;
    TVT->TVT$WR_OCNT = TVT->TVT$WR_OCNT + Bytes_Written ;
    TVT->TVT$WR_BCNT = TVT->TVT$WR_BCNT - Bytes_Written ;
    if ((TVT->TVT$WR_OPTR GEQ TVT_TTY_BUFLEN))
	{
	TVT->TVT$WR_OPTR = 0 ;
	} ;

//    if ((bytes_left > 0))
!	{
//    opr$fao("!!!pty_overun!!// !SL bytes",bytes_left,TVT->TVT$WR_BUF,TVT->TVT$WR_PTR);
!	CH$MOVE(bytes_left, TVT->TVT$WR_BUF + bytes_left, TVT->TVT$WR_BUF);
!	TVT->TVT$WR_PTR = TVT->TVT$WR_BUF + bytes_left;
!	} ;

// Indicate write no longer in progress and check for more data to send

    TVT->TVT$PWRITE = FALSE;
//    AST_IN_PROGRESS = FALSE;
//    OKINT ;

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB x!XL PTY_WRITE_DONE: #bytes left=!SL, written = !SL!/",
		0, TVT->TVT$TCB, bytes_left, Bytes_written) ;
	};

// Empty PTY buffers

    PTY_READ(TVT) ;

//    if ((NOT TVT->TVT$NREAD) && (TCB->RCV_Q_COUNT > 0))
//IF USE_ASTS %THEN
!	TCP_READ(TVT);
!	$DCLAST(ASTADR = PTY_WRITE,
!		ASTPRM = TVT);
!	PTY_WRITE(TVT) ;
//else
	$ACPWAKE;
//FI

    };

//SBTTL "Mailbox handling routines"

MBX_READ(TVT)
!
// MBX_READ - Initiate a read on the PTY's associated mailbox.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	RC;

// Issue the read $QIO

    RC = $QIO(FUNC	= IO$_READVBLK,
	      CHAN	= TVT->TVT$MBX_CHN,
	      IOSB	= TVT->TVT$MBX_IOSB,
	      ASTADR	= MBX_READ_DONE,
	      ASTPRM	= TVT,
	      P1	= TVT->TVT$MBX_BUF,
	      P2	= TVT_MBX_BUFLEN);
    if (NOT RC)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT MBX Read failure for TCB x!XL, RC=x!XL!/",
		 0,TVT->TVT$TCB,RC);
	RETURN FALSE;
	};
    RETURN TRUE;
    };

MBX_READ_DONE(TVT) : NOVALUE (void)
!
// Here when we receive a message on the mailbox associated with the PTY. If we
// receive the terminal hangup signal (i.e. PTY has gone away).
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	IOSB = TVT->TVT$MBX_IOSB : $BBLOCK,
	MBLOCK = TVT->TVT$MBX_BUF : $BBLOCK;
    signed long
	MTYPE,
	RC;

// Make sure the TVT is still valid

    if (TVT->TVT$CANCEL)
	RETURN;

// Check the status - ignore cancel/abort

    RC = IOSB[0,0,16,0];
    if (RC EQLU SS$_ABORT)
	RETURN;
    if (RC EQLU SS$_CANCEL)
	RETURN;

// Check for null status - just reqeueue the read

//    AST_IN_PROGRESS = TRUE;
//    NOINT ;
    if (RC EQLU 0)
	{
	MBX_READ(TVT);
!	AST_IN_PROGRESS = FALSE;
!	OKINT ;
	RETURN;
	};

// If we got an error, we have a problem. Abort.

    if (NOT RC)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT MBX read failure for TCB x!XL, RC=x!XL!/",
		 0,TVT->TVT$TCB,RC);
	TCB$DELETE(TVT->TVT$TCB);
!	AST_IN_PROGRESS = FALSE;
!	OKINT ;
	RETURN;
	};

// Get the message type and dispatch it.

    MTYPE = MBLOCK[0,0,16,0];
    SELECTONE MTYPE OF
	SET
	[MSG$_TRMHANGUP]:
	    {
	    XLOG$FAO(LOG$TCPERR,"!%T TVT hangup signal for TCB x!XL!/",
		     0,TVT->TVT$TCB);
	    TCP$TCB_CLOSE(TVT->TVT$TCB);
!	    AST_IN_PROGRESS = FALSE;
!	    OKINT ;
	    RETURN;
	    };
	TES;

// If we didn't handle it, it was ignored. Requeue the read.

//    AST_IN_PROGRESS = FALSE;
//    OKINT ;
    MBX_READ(TVT);
    };

//SBTTL "Option negotiation routines"

Void Set_State_ON (TVT, OPTBLK) (void)
    {
    MAP
	struct TVT$BLOCK * TVT,
	struct OPT$BLOCK * OPTBLK;

    if (OPTBLK->OPT$STATE != OPT$STATE_ON)
	{
	OPTBLK->OPT$STATE = OPT$STATE_ON;
	if ((OPTBLK->OPT$ON_RTN != 0))
	    (OPTBLK->OPT$ON_RTN)(TVT)
	}
    };

Void Set_State_OFF (TVT, OPTBLK) (void)
    {
    MAP
	struct TVT$BLOCK * TVT,
	struct OPT$BLOCK * OPTBLK;

    if (OPTBLK->OPT$STATE != OPT$STATE_OFF)
	{
	OPTBLK->OPT$STATE = OPT$STATE_OFF;
	if ((OPTBLK->OPT$OFF_RTN != 0))
	    (OPTBLK->OPT$OFF_RTN)(TVT)
	}
    };



FORWARD ROUTINE
 VOID    TVT_S};

TVT_READ_WILL(TVT,OPTION) : NOVALUE (void)
!
// Received a WILL for an option. Handle according to the option table.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	REMOPTS = TVT->TVT$REMOPTS : OPT$LIST,
	OPTBLK = (
	    if (OPTION LEQU TELNET$K_MAXOPT)
		REMOPTS[OPTION,OPT$BASE]
	    else
		DEFAULT_OPTION_BLOCK) : OPT$BLOCK;

// Reset TVT read state

    XLOG$FAO(LOG$TELNEG,"!%T READ_Will !UB!/",0,Option);
    TVT->TVT$NRSTATE = TVT$STATE_NORMAL;

// If currently doing the option, then turn it on

    if (OPTBLK->OPT$CURRENT)
	{
	OPTBLK->OPT$CURRENT = FALSE;
	Set_State_ON(TVT, OPTBLK) ;
	}
    else

// Not currently doing negotiation. If we should turn it on, then do so.

	if (NOT OPTBLK->OPT$STATE)
	    {
	    IF (OPTBLK->OPT$PREFER == OPT$STATE_ON) OR
	       (OPTBLK->OPT$PREFER == OPT$DONT_CARE) THEN
		{
!!!JC		OPTBLK [OPT$STATE] = OPT$STATE_ON;
		Set_State_ON(TVT, OPTBLK);		// JC
		TVT_S}(TVT,TELNET$K_DO,OPTION);
		}
	    else
		TVT_S}(TVT,TELNET$K_DONT,OPTION);
	    }
    };

TVT_READ_WONT(TVT,OPTION) : NOVALUE (void)
!
// Received a WONT for an option. Handle according to the option table.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	REMOPTS = TVT->TVT$REMOPTS : OPT$LIST,
	OPTBLK = (
	    if (OPTION LEQU TELNET$K_MAXOPT)
		REMOPTS[OPTION,OPT$BASE]
	    else
		DEFAULT_OPTION_BLOCK) : OPT$BLOCK;

// Reset TVT read state

    XLOG$FAO(LOG$TELNEG,"!%T READ_Wont !UB!/",0,Option);
    TVT->TVT$NRSTATE = TVT$STATE_NORMAL;

// If currently doing the option, then turn it off

    if (OPTBLK->OPT$CURRENT)
	{
	OPTBLK->OPT$CURRENT = FALSE;
	Set_State_OFF(TVT, OPTBLK);
	}
    else

// Not currently doing negotiation. If we should turn it off, do so.
// We must not prevent the remote host from reverting back to NVT

	if (OPTBLK->OPT$STATE)
		{
!!!JC		OPTBLK->OPT$STATE = OPT$STATE_OFF;
		Set_State_OFF(TVT, OPTBLK);		// JC
		TVT_S}(TVT,TELNET$K_DONT,OPTION);
		}
    };



TVT_READ_DO(TVT,OPTION) : NOVALUE (void)
!
// Received a DO for an option. Handle according to the option table.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	LCLOPTS = TVT->TVT$LCLOPTS : OPT$LIST,
	OPTBLK = (
	    if (OPTION LEQU TELNET$K_MAXOPT)
		LCLOPTS[OPTION,OPT$BASE]
	    else
		DEFAULT_OPTION_BLOCK) : OPT$BLOCK;

// Reset TVT read state

    XLOG$FAO(LOG$TELNEG,"!%T READ_Do !UB!/",0,Option);
    TVT->TVT$NRSTATE = TVT$STATE_NORMAL;

// If currently doing the option, then turn it on

    if (OPTBLK->OPT$CURRENT)
	{
	OPTBLK->OPT$CURRENT = FALSE;
!!!JC	OPTBLK->OPT$STATE = OPT$STATE_ON;	// Redundant
	Set_State_ON(TVT, OPTBLK);		// JC Must set on
	}
    else

// Not currently doing negotiation. If we should turn it on, then do so.

	if (NOT OPTBLK->OPT$STATE)
	    {
	    IF (OPTBLK->OPT$PREFER == OPT$STATE_ON) OR
	       (OPTBLK->OPT$PREFER == OPT$DONT_CARE) THEN
		{
!!!JC		OPTBLK [OPT$STATE] = OPT$STATE_ON;
		Set_State_ON(TVT, OPTBLK);		// JC
		TVT_S}(TVT,TELNET$K_WILL,OPTION);
		}
	    else
		TVT_S}(TVT,TELNET$K_WONT,OPTION);
	    };
    };



TVT_READ_DONT(TVT,OPTION) : NOVALUE (void)
!
// Received a DONT for an option. Handle according to the option table.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	LCLOPTS = TVT->TVT$LCLOPTS : OPT$LIST,
	OPTBLK = (
	    if (OPTION LEQU TELNET$K_MAXOPT)
		LCLOPTS[OPTION,OPT$BASE]
	    else
		DEFAULT_OPTION_BLOCK) : OPT$BLOCK;

// Reset TVT read state

    XLOG$FAO(LOG$TELNEG,"!%T READ_Dont !UB!/",0,Option);
    TVT->TVT$NRSTATE = TVT$STATE_NORMAL;

// If currently doing the option, then turn it off

    if (OPTBLK->OPT$CURRENT)
	{
	OPTBLK->OPT$CURRENT = FALSE;
!!!JC	OPTBLK->OPT$STATE = OPT$STATE_OFF;
	Set_State_OFF(TVT, OPTBLK);		// JC
	}
    else

// Not currently doing negotiation. If we should turn it off, do so.
// We must not prevent the remote host from reverting back to NVT

	{
	if (OPTBLK->OPT$STATE)
		{
!!!JC		OPTBLK->OPT$STATE = OPT$STATE_OFF;
!!!JC		OPTBLK->OPT$STATE = OPT$STATE_OFF;
		Set_State_OFF(TVT, OPTBLK);		// JC
		TVT_S}(TVT,TELNET$K_WONT,OPTION);
		} ;
	} ;
    };



TVT_S}(TVT,OPR,OPTION) : NOVALUE (void)
!
// Send a TVT option negotiation. We queue the bytes needed to implement the
// option negotiation in the option buffer, and attempt a network write if
// none is currently happening.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    LITERAL
	TVT_OPTION_LEN = 3;

// Make sure there is room for the bytes we need to send.

    XLOG$FAO(LOG$TELNEG,"!%T Send !UB!/",0,Option);

    if (TVT->TVT$NEG_CNT GEQ (TVT_NEG_BUFLEN-TVT_OPTION_LEN))
	RETURN;

// Insert the option negotiation bytes into the buffer

    if (TVT->TVT$NEG_CNT == 0)
	TVT->TVT$NEG_EQP = CH$PTR(TVT->TVT$NEG_BUF);
    CH$WCHAR_A(TELNET$K_IAC,TVT->TVT$NEG_EQP);
    CH$WCHAR_A(OPR,TVT->TVT$NEG_EQP);
    CH$WCHAR_A(OPTION,TVT->TVT$NEG_EQP);
    TVT->TVT$NEG_CNT = TVT->TVT$NEG_CNT + TVT_OPTION_LEN;
    };



TVT_S}_SUBOP(TVT,OPTION,DATA_A,SIZE) : NOVALUE (void)
!
// Send a TVT suboption negotiation. We queue the bytes needed to implement the
// option negotiation in the option buffer, and attempt a network write if
// none is currently happening.
!
    {
    BIND
	DATA = DATA_A;
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	Char : UNSIGNED BYTE;

// Make sure there is room for the bytes we need to send.

    XLOG$FAO(LOG$TELNEG,"!%T Send_Subop !UB!/",0,Option);
    if (TVT->TVT$NEG_CNT GEQ (TVT_NEG_BUFLEN-(Size+5)))
	RETURN;

// Insert the option negotiation bytes into the buffer

    if (TVT->TVT$NEG_CNT == 0)
	TVT->TVT$NEG_EQP = CH$PTR(TVT->TVT$NEG_BUF);

    // Write the suboption header.
    CH$WCHAR_A(TELNET$K_IAC,TVT->TVT$NEG_EQP);
    CH$WCHAR_A(TELNET$K_SB,TVT->TVT$NEG_EQP);
    CH$WCHAR_A(OPTION,TVT->TVT$NEG_EQP);

    // Write the suboption data

//    CH$WCHAR_A(Option$K_Tog_Flow_Cntl_OFF,TVT->TVT$NEG_EQP);
    INCR i FROM 0 TO Size-1 DO
	{
	Char = CH$RCHAR_A(Data);
	CH$WCHAR_A(Char,TVT->TVT$NEG_EQP)
	};

    // Write the suboption trailer
    CH$WCHAR_A(TELNET$K_IAC,TVT->TVT$NEG_EQP);
    CH$WCHAR_A(TELNET$K_SE,TVT->TVT$NEG_EQP);

    TVT->TVT$NEG_CNT = TVT->TVT$NEG_CNT + (Size+5);
    };



TVT_READ_SUB(TVT) : NOVALUE (void)
!
// Received a Suboption for an option. Handle according to the option table.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	OPTS = TVT->TVT$REMOPTS : OPT$LIST;
    signed long
	sub_func,
	opt : UNSIGNED BYTE;

    opt = CH$RCHAR_A(TVT->TVT$SUB_PTR);
    XLOG$FAO(LOG$TELNEG,"!%T READ_Sub !UB!/",0,Opt);
    TVT->TVT$SUB_CNT = TVT->TVT$SUB_CNT -1 ;

     // If everything is kosher, execute the suboption handler.
     if (opt LEQU TELNET$K_MAXOPT)
	IF (OPTS[opt,OPT$SUB_RTN] != 0) AND
	   (OPTS[opt,OPT$STATE] == OPT$STATE_ON) THEN
		(OPTS[opt,OPT$SUB_RTN])(TVT)
    };



TVT_NEGOTIATE(TVT) : NOVALUE (void)
!
// Initiate option negotiation for all of the negotiations that we prefer to be
// in the ON state. Loops through LCLOPTS, sending WILLs and REMOPTS sending
// DOs for them.
!
    {
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	OPTION;
    BIND
	LCLOPTS = TVT->TVT$LCLOPTS : OPT$LIST,
	REMOPTS = TVT->TVT$REMOPTS : OPT$LIST;

// First, do local options (offer to WILL them)

    INCR OPTION FROM TELNET$K_MINOPT TO TELNET$K_MAXOPT DO
	{
	BIND
	    OPTBLK = LCLOPTS[OPTION,OPT$BASE] : OPT$BLOCK;

// If off and we prefer on, send a WILL

	IF (NOT OPTBLK->OPT$STATE) AND
	   (OPTBLK->OPT$PREFER == OPT$STATE_ON) THEN
	    {
	    OPTBLK->OPT$CURRENT = TRUE;
	    TVT_S}(TVT,TELNET$K_WILL,OPTION);
	    };

// If on and we prefer it off, send a WONT

	IF OPTBLK->OPT$STATE AND
	   (OPTBLK->OPT$PREFER == OPT$STATE_OFF) THEN
	    {
	    OPTBLK->OPT$CURRENT = TRUE;
	    TVT_S}(TVT,TELNET$K_WONT,OPTION);
	    };
	};

// Then, do remote options (ask remote to DO them)

    INCR OPTION FROM TELNET$K_MINOPT TO TELNET$K_MAXOPT DO
	{
	BIND
	    OPTBLK = REMOPTS[OPTION,OPT$BASE] : OPT$BLOCK;

// If off and we prefer on, send a DO

	IF (NOT OPTBLK->OPT$STATE) AND
	   (OPTBLK->OPT$PREFER == OPT$STATE_ON) THEN
	    {
	    OPTBLK->OPT$CURRENT = TRUE;
	    TVT_S}(TVT,TELNET$K_DO,OPTION);
	    };

// If on and we prefer it off, send a DONT

	IF OPTBLK->OPT$STATE AND
	   (OPTBLK->OPT$PREFER == OPT$STATE_OFF) THEN
	    {
	    OPTBLK->OPT$CURRENT = TRUE;
	    TVT_S}(TVT,TELNET$K_DONT,OPTION);
	    };
	};
    };
}
ELUDOM

