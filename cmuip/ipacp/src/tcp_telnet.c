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
	Call TCP$Enqueue_ACK() rather than setting PENDING_ACK.

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

#if 0
MODULE TELNET(IDENT="1.11",LANGUAGE(BLISS32),
	      ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			      NONEXTERNAL=LONG_RELATIVE),
	      LIST(REQUIRE,ASSEMBLY,OBJECT,BINARY),
	      OPTIMIZE,OPTLEVEL=3,ZIP)=
#endif

//!!HACK!!// why does this module make so many refs to AST_in_Progress?

#include<descrip.h> 

#include <starlet.h>	// VMS system definitions
// not yet #include "CMUIP_SRC:[CENTRAL]NETXPORT";	// BLISS common definitions
#include <cmuip/central/include/neterror.h>	// Network error codes
#include "netvms.h"
			// VMS-specific definitions
#include "tcpmacros.h"		// System-wide Macro definitions
#include "structure.h"		// System-wide structure definitions
#include "tcp.h"			// TCP related definitions
#include "telnet.h"		// TELNET protocol definitions

#include<ssdef.h>
#include<iodef.h>
#include<dvidef.h>
#include<iosbdef.h>
#include<msgdef.h>

extern signed long log_state,
    local_name,
    ast_in_progress;

#define     M$INTERNAL	  15

extern     tcp$tcb_close();
extern     tcb$create();
extern  void    tcb$delete();
extern  void    tcp$enqueue_ack();
extern  void    tcp$send_ack();
extern     tcp$send_data();
extern     tcp$tcb_init();
extern     TIME_STAMP();
extern     check_unique_conn();
 void    Conect_Insert();

#define    TVT_OPEN_TIMEOUT 120*CSEC // Amount of time to wait for TVT to open

#define     Telnet$K_Char_AO	  017	// Control-O
#define     Telnet$K_Char_AYT	  024	// Control-T
#define     Telnet$K_Char_Brk	  031	// Control-Y
#define     Telnet$K_Char_EC	  0177	// DEL
#define     Telnet$K_Char_Purge	  0x18	// Control-X
#define     Telnet$K_Char_EL	  025	// Control-U
#define     Telnet$K_Char_IP	  003	// Control-C.



//SBTTL "TELNET definitions"

// Define the structure of a PTY status block

struct PTY$IOSB 
{
unsigned short    PTSB$STATUS;	// $QIO status
unsigned short    PTSB$NBYTES;	// Number of bytes transferred
unsigned short    PTSB$EXTRA1;	// Extra information
unsigned short    PTSB$EXTRA2;	// Extra information
};
#if 0
LITERAL
    PTY$IOSB_SIZE = $FIELD_SET_SIZE;
MACRO
    PTY$IOSB = BLOCK->PTY$IOSB_SIZE FIELD(PTY$IOSB_FIELDS) %;
#endif

// USE_ASTS nonzero means we should do all TCP I/O at AST level and thus it is
// OK to directly call TCP_READ and TCP_WRITE in AST routines.

#define  USE_ASTS 1


//SBTTL "Module data"

struct dsc$descriptor PTY_NAME = ASCID2(8,"INET$PTY");
struct dsc$descriptor AYT_RESPONSE = ASCID2(12,"[definitely]");

// Define the default options state for new TVTs

extern
    Timing_Mark_On(),
    Terminal_Type_On(),
    Terminal_Type_Sub(),
    Window_Size_On(),
    Window_Size_Sub(),
    set_devdep(),				// JC
    LineMode_Sub();

struct OPT$BLOCK  DEFAULT_OPTION_BLOCK[TELNET$K_X_DISPLAY_LOCATION];
#if 0
	PRESET(
	    [OPT$STATE]		= OPT$STATE_OFF,
	    [OPT$CURRENT]	= FALSE,
	    [OPT$PREFER]	= OPT$STATE_OFF),
#endif
  struct OPT$BLOCK    TVT_DEF_LOCAL[TELNET$K_X_DISPLAY_LOCATION] =
{ 
  {
	    /* TELNET$K_BINARY */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_BINARY */ OPT$CURRENT : FALSE,
	    /* TELNET$K_BINARY */ OPT$PREFER : OPT$DONT_CARE,
	    /* TELNET$K_BINARY */ OPT$ON_RTN : set_devdep,		//JC
	    /* TELNET$K_BINARY */ OPT$OFF_RTN : set_devdep,		//JC
				    }, {
	    /* TELNET$K_ECHO */ OPT$STATE : OPT$STATE_OFF,	//JC
	    /* TELNET$K_ECHO */ OPT$CURRENT : FALSE,		//JC
	    /* TELNET$K_ECHO */ OPT$PREFER : OPT$STATE_ON,		//JC
	    /* TELNET$K_ECHO */ OPT$ON_RTN : set_devdep,		//JC
	    /* TELNET$K_ECHO */ OPT$OFF_RTN : set_devdep,		//JC
				    }, {
				      /* reconn */
				    }, {
	    /* TELNET$K_SUPRGA */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_SUPRGA */ OPT$CURRENT : FALSE,
	    /* TELNET$K_SUPRGA */ OPT$PREFER : OPT$STATE_ON,
				    }, {
				    }, {
				    }, {
	    /* TELNET$K_Timing_Mark */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_Timing_Mark */ OPT$CURRENT : FALSE,
	    /* TELNET$K_Timing_Mark */ OPT$ON_RTN : Timing_Mark_On,
	    /* TELNET$K_Timing_Mark */ OPT$PREFER : OPT$DONT_CARE,
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
	    /* TELNET$K_Extended_Ascii */ OPT$STATE : OPT$STATE_OFF,//JC
	    /* TELNET$K_Extended_Ascii */ OPT$CURRENT : FALSE,	//JC
	    /* TELNET$K_Extended_Ascii */ OPT$PREFER : OPT$DONT_CARE,//JC
	    /* TELNET$K_Extended_Ascii */ OPT$ON_RTN : set_devdep,	//JC
	    /* TELNET$K_Extended_Ascii */ OPT$OFF_RTN : set_devdep,	//JC
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
				    }, {
//!//JC	    /* TELNET$K_Window_Size */ OPT$STATE : OPT$STATE_OFF,
//!//JC	    /* TELNET$K_Window_Size */ OPT$CURRENT : FALSE,
//!//JC	    /* TELNET$K_Window_Size */ OPT$PREFER : OPT$STATE_ON,
//!//JC	    /* TELNET$K_Window_Size */ OPT$SUB_RTN : Window_Size_Sub,
				    }, {
				    }, {
				    }, {
	    /* TELNET$K_LineMode */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_LineMode */ OPT$CURRENT : FALSE,
	    /* TELNET$K_LineMode */ OPT$PREFER : OPT$STATE_OFF,
				    }
					}  ;

struct OPT$BLOCK TVT_DEF_REMOTE[TELNET$K_X_DISPLAY_LOCATION] = {
  {
	    /* TELNET$K_BINARY */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_BINARY */ OPT$CURRENT : FALSE,
	    /* TELNET$K_BINARY */ OPT$PREFER : OPT$DONT_CARE,
				    }, {
//!//JC	    /* TELNET$K_ECHO */ OPT$STATE : OPT$STATE_OFF,	//JC
//!//JC	    /* TELNET$K_ECHO */ OPT$CURRENT : FALSE,		//JC
//!//JC	    /* TELNET$K_ECHO */ OPT$PREFER : OPT$DONT_CARE,	//JC
				    }, {
      /* reconn */
				    }, {
	    /* TELNET$K_SUPRGA */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_SUPRGA */ OPT$CURRENT : FALSE,
	    /* TELNET$K_SUPRGA */ OPT$PREFER : OPT$STATE_ON,
				    }, {
      /* size_neg */
				    }, {
				      /* tstatus */
				    }, {
	    /* TELNET$K_Timing_Mark */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_Timing_Mark */ OPT$CURRENT : FALSE,
	    /* TELNET$K_Timing_Mark */ OPT$PREFER : OPT$DONT_CARE,
				    }, {
				      /* remote */
				    }, {
				      /* line */
				    }, {
				      /* page */
				    }, {
				      /* cr */
				    }, {
				      /* tab st */
				    }, {
				      /* tab di */
				    }, {
				      /* ff */
				    }, {
				      /* vert */
				    }, {
				      /* vt */
				    }, {
				      /* lf */
				    }, {
	    /* TELNET$K_Extended_Ascii */ OPT$STATE : OPT$STATE_OFF,//JC
	    /* TELNET$K_Extended_Ascii */ OPT$CURRENT : FALSE,	//JC
	    /* TELNET$K_Extended_Ascii */ OPT$PREFER : OPT$DONT_CARE,//JC
				    }, {
				      /* logout */
				    }, {
				      /* byte */
				    }, {
				      /* data */
				    }, {
				      /* supdup */
				    }, {
				      /* supout */
				    }, {
				      /* loc */
				    }, {
	    /* TELNET$K_Terminal_Type */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_Terminal_Type */ OPT$CURRENT : FALSE,
	    /* TELNET$K_Terminal_Type */ OPT$PREFER : OPT$STATE_ON,	//!// JC
	    /* TELNET$K_Terminal_Type */ OPT$ON_RTN : Terminal_Type_On,
	    /* TELNET$K_Terminal_Type */ OPT$SUB_RTN : Terminal_Type_Sub,
				    }, {
				      /* eor */
				    }, {
				      /* uid */
				    }, {
				      /* out mark */
				    }, {
				      /* loc no */
				    }, {
				      /* 3270 */
				    }, {
				      /* x3 */
				    }, {
	    /* TELNET$K_Window_Size */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_Window_Size */ OPT$CURRENT : FALSE,
	    /* TELNET$K_Window_Size */ OPT$PREFER : OPT$STATE_ON,		//!// JC
	    /* TELNET$K_Window_Size */ OPT$ON_RTN : Window_Size_On,
	    /* TELNET$K_Window_Size */ OPT$SUB_RTN : Window_Size_Sub,
				    }, {
				      /* speed */
				    }, {
	    /* TELNET$K_Toggle_Flow_Control */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_Toggle_Flow_Control */ OPT$CURRENT : FALSE,
	    /* TELNET$K_Toggle_Flow_Control */ OPT$PREFER : OPT$STATE_ON,
				    }, {
	    /* TELNET$K_LineMode */ OPT$STATE : OPT$STATE_OFF,
	    /* TELNET$K_LineMode */ OPT$CURRENT : FALSE,
	    /* TELNET$K_LineMode */ OPT$PREFER : OPT$STATE_OFF,
	    /* TELNET$K_LineMode */ OPT$SUB_RTN : LineMode_Sub,
				    }, {
				      { /* x_disp */ }
}
} ;


//SBTTL "TELNET_CREATE - Initialize a TCP connection for a TVT"
/*
    Creates and initializes a TCB for a new TVT connection. Called from
    segment input processor when a SYN segment has been received for the
    well-known TELNET port (WKS$TELNET).
*/


 void    TELNET_OPEN_TIMEOUT();

TELNET_CREATE(LHOST,LPORT,FHOST,FPORT)
    {
      struct tcb_structure * TCB;
    signed long
	CIDX;

// Check for unique connection. We actually know that the connection is already
// unique, but we do this for the side effect of being inserted in the CONECT
// table.

    NOINT;

    if (check_unique_conn(LPORT,FHOST,FPORT,CIDX) != TRUE)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT create failed - CONECT table full!/",0);
	OKINT;
	return 0;
	};

// Create and initialize a new TCB

    if ((TCB = tcb$create()) == ERROR)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT TCB creation failed!/",0);
	OKINT;
	return 0;
	};

// Perform standard TCB initializations

    tcp$tcb_init(TCB);

// Setup standard TVT TCB fields. Note that segment input processing code will
// setup wild foreign host/port and local host when this routine returns.

    TCB->is_tvt = TRUE;		// This is a TVT
    TCB->tvtdata = 0;		// No TVT data block yet
    TCB->local_host = LHOST;	// Set local host and port
    TCB->local_port = LPORT;
    TCB->foreign_host = FHOST;	// Set foreign host and port
    TCB->foreign_port = FPORT;
    TCB->ucb_adrs = 0;		// No UCB for this, since no user process
    TCB->state = CS$LISTEN;	// Initial state is listening for SYN
    TCB->con_index = CIDX;
    Conect_Insert(TCB,CIDX);	// Insert into connection table

// Setup a handler if the open times-out

    TCB->pending_io = TRUE;
    TCB->curr_user_function = M$INTERNAL;
    TCB->timeout_routine = TELNET_OPEN_TIMEOUT;
    TCB->function_timer = Time_Stamp() + TVT_OPEN_TIMEOUT;

// And return the TCB address

    OKINT;
    return TCB;
    }

void TELNET_OPEN_TIMEOUT(TCB)
	struct tcb_structure * TCB;
    {
//
//   Come here from user function timeout code if a TVT connection does not
//   become established soon enough after we first see the SYN for it. We
//   simply flush the TCB.
//

//~~~ Maybe we should do something better here?

    XLOG$FAO(LOG$TCPERR,"!%T TVT open timeout for TCB x!XL!/",0,TCB);
    tcb$delete(TCB);
    }

//SBTTL "TELNET_OPEN - Finish open of TCP connection for a TVT"
/*
    Finish setup of TELNET connection, creating TVT data block within the
    TCB and performing assigmnent/initialization of the pseudo-terminal
    device associated with this connection. Called from segment input handler
    when TCP connection goes into the ESTABLISHED state.
*/

 void    TVT_NEGOTIATE ();
 void    TCP_READ ();
 void    tcp_add_string ();
 void    TCP_WRITE ();
 void    PTY_READ ();
 void    PTY_WRITE ();
 void    net_to_pty ();
 void    PTY_TO_NET ();
    MBX_READ ();
 void    MBX_READ_DONE ();

void namelook_done(TVT,rc,namlen,name)
//+
//	Fills in the name of the remote port
//-
	struct TVT$BLOCK * TVT;
    {
      struct tcb_structure * TCB;
      unsigned char 	nambuf[100];
      struct dsc$descriptor nam = {
		dsc$w_length : sizeof(nambuf),
		dsc$b_dtype : DSC$K_DTYPE_Z,
		dsc$b_class : DSC$K_CLASS_Z,
		dsc$a_pointer : nambuf};
      unsigned char 	accporbuf[100];
      struct dsc$descriptor accpornam_ = {
		dsc$w_length : sizeof(accporbuf),
		dsc$b_dtype : DSC$K_DTYPE_Z,
		dsc$b_class : DSC$K_CLASS_Z,
		dsc$a_pointer : accporbuf}, *accpornam = &accpornam;
      $DESCRIPTOR(lnm_table,"LNM$SYSTEM_TABLE");
      $DESCRIPTOR(lnm_nam,"TELNET_PASS_PORT");
      struct item_list_3 itm[2]={ {buflen:100, item_code:1, bufaddr: nambuf, &nam.dsc$w_length }, {0,0,0,0} };

    if ((! rc)) return;

    TCB = TVT->TVT$TCB;				// get TCB

    if (exe$trnlnm(0,&lnm_table,			// JC Get logical
		&lnm_nam,				// Pass on port number
		0,					// JC
		itm)					// JC
	== SS$_NORMAL)
      rc = exe$fao(/*%ASCID*/"!AF!AS!UL",
		accpornam->dsc$w_length,accpornam,
		namlen,name,
		nam,
		TCB->foreign_port
			     );
    else
	rc = exe$fao(/*%ASCID*/"!AF",
		accpornam->dsc$w_length,accpornam,
		namlen,name
		);
    XLOG$FAO(LOG$TELNET
	,"!%T Namelook_done: Access port=!AS!/",0
	,accpornam);
//
//	It is better to have the correct name than a truncated on
//
    if (accpornam->dsc$w_length > 30)
	return(SS$_NORMAL);				// JC IF too long skip it
//!//JC	accpornam->dsc$w_length = 30;
    if (rc)
      rc = exe$qiow (0,TVT->TVT$PTY_CHN,IO$_SETMODE,0,
		     accpornam->dsc$a_pointer,	// Buffer
		     accpornam->dsc$w_length,	// Size
		     0,
		     4,0,0,0,0				// Sub-func #4
			);

    }

TELNET_OPEN(TCB)
	struct tcb_structure * TCB;
    {
      extern	LIB$GET_VM_PAGE();
      extern LIB$ASN_WTH_MBX();
      extern LIB$GETDVI();
      extern Print();
      extern Line_Changed_AST();
      extern NML$GETNAME();

      unsigned char 	nambuf[256];
      struct dsc$descriptor nam_ = {
		dsc$w_length : sizeof(nambuf),
		dsc$b_dtype : DSC$K_DTYPE_Z,
		dsc$b_class : DSC$K_CLASS_Z,
		dsc$a_pointer : nambuf}, * nam=&nam;
      unsigned char 	accporbuf[100];
      struct dsc$descriptor  accpornam_ = {
		dsc$w_length : sizeof(accporbuf),
		dsc$b_dtype : DSC$K_DTYPE_Z,
		dsc$b_class : DSC$K_CLASS_Z,
		dsc$a_pointer : accporbuf}, *accpornam=&accpornam_;
      struct TVT$BLOCK * TVT;
      struct _iosb io_stats;
      signed long
	PTYCHAN,
	MBXCHAN,
	Status,
	RC,
	tmp;
      unsigned char TMPBUF[TVT_TTY_BUFLEN];
	  struct dsc$descriptor tmpdsc_, * TMPDSC=&tmpdsc_;
      $DESCRIPTOR(lnm_table,"LNM$SYSTEM_TABLE");
      $DESCRIPTOR(lnm_nam,"TELNET_PASS_PORT");
      $DESCRIPTOR(lnm_nam2,"TELNET_ANNOUNCE");
      struct item_list_3 itm[2]={ {buflen:256, item_code:1, bufaddr: nambuf, &nam->dsc$w_length }, {0,0,0,0} };

// Clear the pending open that we were waiting for.

    TCB->pending_io = FALSE;

// Assign a chunk of memory for the TVT data block

//    RC = LIB$GET_VM(%REF(TVT$SIZE*4),TVT);
    RC = LIB$GET_VM_PAGE(/*%REF*/(((TVT$SIZE * 4) / 512) + 1),&TVT);
    if (! RC)
	{
//~~~ Should we do anything better here?
	XLOG$FAO(LOG$TCPERR,
	  "!%T Telnet_Open: LIB$GET_VM_PAGE failure for TCB=x!XL, RC=x!XL!/"
	  ,0,TCB,RC);
	tcb$delete(TCB);
	return FALSE;
	};

// Clear out the TVT data block

    CH$FILL(0,TVT$SIZE*4,CH$PTR(TVT,0));
    TVT->TVT$TCB = TCB;

// Initialize the options block to the standard initial state

    CH$MOVE(OPT$LSTBLEN,CH$PTR(TVT_DEF_LOCAL,0),CH$PTR(TVT->TVT$LCLOPTS,0));
    CH$MOVE(OPT$LSTBLEN,CH$PTR(TVT_DEF_REMOTE,0),CH$PTR(TVT->TVT$REMOPTS,0));

// Assign the PTY device and start it up.

    RC = LIB$ASN_WTH_MBX(PTY_NAME,
			 /*%REF*/(TVT_MBX_BUFLEN),
			 /*%REF*/(TVT_MBX_BUFLEN),
			 PTYCHAN,
			 MBXCHAN);
    if (! RC)
	{
	    XLOG$FAO(LOG$TCPERR,
		"!%T Telnet_Open:  PTY assign failure for TCB x!XL, RC=x!XL!/"
		,0,TCB,RC);
	    tcb$delete(TCB);
	    return FALSE;
	};


// Associate the TVT with the TCP connection

    TVT->TVT$PTY_CHN = PTYCHAN;
    TVT->TVT$MBX_CHN = MBXCHAN;
    TCB->tvtdata = TVT;

// Fill in the access port as [n.n.n.n]
    if (exe$trnlnm(0,&lnm_table,			// JC Get logical
		&lnm_nam,	// Pass on port number
		0,		// JC
		&itm)				// JC
	== SS$_NORMAL)
#if 0
	RC = exe$fao(/*%ASCID*/"!UB.!UB.!UB.!UB!AS!UL",
		accpornam->dsc$w_length,accpornam,
		.(TCB->foreign_host)< 0,8,0>,
		.(TCB->foreign_host)< 8,8,0>,
		.(TCB->foreign_host)<16,8,0>,
		.(TCB->foreign_host)<24,8,0>,
		nam,
		TCB->foreign_port
		)
    else
	RC = exe$fao(/*%ASCID*/"!UB.!UB.!UB.!UB",
		accpornam->dsc$w_length,accpornam,
		.(TCB->foreign_host)< 0,8,0>,
		.(TCB->foreign_host)< 8,8,0>,
		.(TCB->foreign_host)<16,8,0>,
		.(TCB->foreign_host)<24,8,0>
		);
#else
    { }
#endif
    XLOG$FAO(LOG$TELNET
	,"!%T Telnet_Open: Remote host=!AS!/",0
	,accpornam);
    if (accpornam->dsc$w_length > 30)		// IF too long
	accpornam->dsc$w_length = 30;			// Adjust it
    if (RC)
      RC = exe$qiow (0,TVT->TVT$PTY_CHN,IO$_SETMODE,0,0,0,
		     accpornam->dsc$a_pointer,	// Buffer
		     accpornam->dsc$w_length,	// Size
		     0,
		     4,0,0,0,0				// Sub-func #4
			);

// Fill in the actual name after name resolution
    NML$GETNAME(TCB->foreign_host, namelook_done, TVT);

// Start a receive on the PTY mailbox.

    if (! MBX_READ(TVT))
	{
	tcb$delete(TCB);
	return FALSE;
	};

// Initialize buffer pointers

    TVT->TVT$RD_PTR = CH$PTR(TVT->TVT$RD_BUF,0);
//    TVT->TVT$WR_PTR = CH$PTR(TVT->TVT$WR_BUF);
    TVT->TVT$WR_IPTR = 0 ;
    TVT->TVT$WR_OPTR = 0 ;
    TVT->TVT$NEG_EQP = TVT->TVT$NEG_DQP = CH$PTR(TVT->TVT$NEG_BUF,0);
    TVT->TVT$WR_ICNT = 0 ;
    TVT->TVT$WR_OCNT = 0 ;


//
// Create a banner to send to the user
// The banner is contained in logical TELNET_ANNOUNCE JC
//

    TMPDSC->dsc$b_class = DSC$K_CLASS_Z;
    TMPDSC->dsc$b_dtype = DSC$K_DTYPE_Z;
    TMPDSC->dsc$w_length = sizeof(TMPBUF);
    TMPDSC->dsc$a_pointer = TMPBUF;
//    $FAO(%ASCID"!/!AS VAX/VMS (CMU) TELNET Service!/",
//	TMPDSC->dsc$w_length, TMPDSC, LOCAL_NAME);

    nam->dsc$w_length = sizeof(nambuf);
    if (exe$trnlnm(0,&lnm_table,				// JC Get logical
		&lnm_nam2,		// JC name for banner
		0,		// JC
		&itm)				// JC
	    == SS$_NORMAL)				// JC If got it
	{
	  exe$fao(/*%ASCID*/"!AS!/"				// JC Put into output
		,TMPDSC->dsc$w_length			// JC buffer
		,TMPDSC					// JC
		,nam);					// JC
	tcp_add_string(TVT, TMPDSC) ;
	} ;

// Check for creation of terminal side of connection (like _TZA0:)
// when ever we get input, until this flag is cleared.

    TVT->TVT$DO_PID = 1;

// Stuff the initial <NULL> into the PTY buffer for JOBCTL

//    CH$WCHAR(%CHAR(0),CH$PTR(TVT->TVT$WR_BUF));

    net_to_pty(TVT, CH_NUL) ;

// Queue up the options that we want

    TVT_NEGOTIATE(TVT);

// Write the buffered data to the PTY and to the network
// We assume here that the initial banner can always fit in the network queue.
    TCP_WRITE(TVT);

    PTY_WRITE(TVT);

    PTY_READ(TVT);

// And give success return.
    return TRUE;
    }

//SBTTL "TELNET_CLOSE - Handle close of TELNET connection"
/*
    Handle normal close of TELNET connection. Flush the TVT data structure.
    Called from TCB$Delete just before TCB is deallocated.
*/

 void    TELNET_CLOSE_DONE();

void TELNET_CLOSE(TCB)
	struct tcb_structure * TCB;
    {
	extern NML$CANCEL();
    signed long
	FHOST  = TCB->foreign_host,
      FPORT  = TCB->foreign_port;
	struct TVT$BLOCK * TVT;

// Log this.

    XLOG$FAO(LOG$TCBSTATE,"!%T TVT TCB x!XL closing, TVT=x!XL!/",
	     0,TCB,TCB->tvtdata);
#if 0
    ACT$FAO("!%D Telnet-in closed to IP addr !UB.!UB.!UB.!UB port=!UW!/",0,
	    FHOST<0,8>,FHOST<8,8>,FHOST<16,8>,FHOST<24,8>,
	    FPORT);
#endif

// Make sure we have a TVT and it isn't already being cancelled.

    TVT = TCB->tvtdata;
    if ((TVT != 0) && (! TVT->TVT$CANCEL))
	{

// Deassign the PTY and mailbox channels.

	NML$CANCEL(TVT,0,0);	// Cancel the name lookup for accpornam

	NOINT;
	TVT->TVT$CANCEL = TRUE;
	exe$dassgn(TVT->TVT$PTY_CHN);
	exe$dassgn(TVT->TVT$MBX_CHN);
	exe$dclast(TELNET_CLOSE_DONE,
		TVT);
	OKINT;
	};
    }

void TELNET_CLOSE_DONE(TVT)
//
// Finish deallocation of a TVT, after all AST's have been delivered.
//
	struct TVT$BLOCK * TVT;
    {
    extern	LIB$FREE_VM();
    extern LIB$FREE_VM_PAGE();

// Just deallocate the TVT structure and return.

//    LIB$FREE_VM(%REF(TVT$SIZE*4),TVT);
    LIB$FREE_VM_PAGE(/*%REF*/(((TVT$SIZE * 4) / 512) + 1),TVT);
    }

//SBTTL "TELNET_INPUT - Do TCP input for TELNET connection"
/*
    Process incoming network data for a TELNET connection, handling any
    necessary option negotiations and passing any user data to the PTY
    associated with this connection. Called from segment input handler when
    TCP receive window becomes non-empty.
*/

void TELNET_INPUT(TCB)
	struct tcb_structure * TCB;
    {

// Call TCP input routine

#ifdef USE_ASTS
    exe$dclast(PTY_WRITE,
	    TCB->tvtdata);
#else
    PTY_WRITE(TCB->tvtdata);
#endif
    }

//SBTTL "TELNET_OUTPUT - Do TCP output for TELNET connection"
/*
    Obtain some data to output to the network whenever the TCP send queue
    becomes non-empty. Normally, the PTY read done AST will put data onto the
    TCP output queue (via TCP_WRITE) whenever it arrives over the PTY. When the
    queue fills, however, it effectively "blocks", without issuing further
    PTY reads, until the queue becomes non-full (i.e. the send window opens).
*/

void TELNET_OUTPUT(TCB)
	struct tcb_structure * TCB;
    {

// Just call TCP write with the TVT's TCB

#ifdef USE_ASTS
    exe$dclast(PTY_READ,
	    TCB->tvtdata);
#else
    PTY_READ(TCB->tvtdata);
#endif
    }

//SBTTL "Routines to interface to the network"

 void    TVT_READ_WILL();
 void    TVT_READ_WONT();
 void    TVT_READ_DO();
 void    TVT_READ_DONT();
void    TVT_READ_SUB ();

void TCP_READ(TVT)
//
// TCP_READ - Read as much data as possible from the network into the PTY write
// buffer. If no write is yet in progress, start a write now.
//
	struct TVT$BLOCK * TVT;
    {
struct OPT$BLOCK * LCLOPTS = &TVT->TVT$LCLOPTS ;
    extern	IS_CNTRLT_GOOD();
    struct tcb_structure * TCB;
    signed long
	Byte_Count,
	Byte_Limit,
	Prev_Char,
	CHRPTR,
	CHRCNT,
	CHWPTR,
	CHR,
	NEGCNT ;

// Empty out PTY, so we can accept more input

//    PTY_READ(TVT) ;

// If the PTY write buffer is busy, then don't run here.

    if (TVT->TVT$PWRITE)
	return;

// Is there any data to write?

    TCB = TVT->TVT$TCB;
    if (TCB->rcv_q_count <= 0)
	return;

// If already doing a net read, then don't run here.

    if (TVT->TVT$NREAD)
	return;

// Indicate that we're doing a network read

    TVT->TVT$NREAD = TRUE;
    NEGCNT = TVT->TVT$NEG_CNT;

// We have some data to write. Copy from network to the PTY buffer
// N.B. Need to be careful of synchonization problem with CQ_xxx routines.

    CHRCNT = TCB->rcv_q_count;
    CHRPTR = TCB->rcv_q_deqp;
//    CHWPTR = TVT->TVT$WR_PTR; // = CH$PTR(TVT->TVT$WR_BUF);
    Byte_Limit = MIN(CHRCNT, (TVT_TTY_BUFLEN - TVT->TVT$WR_BCNT)) ;
    CHR = CH_NUL ;

//   Check to see if there is enough buffer space remaining

//    if ((TVT->TVT$WR_BCNT >= (TVT_TTY_BUFLEN - PTY_BUFFER_SIZE)))
    if ((TVT->TVT$WR_BCNT >= (TVT_TTY_BUFLEN - 2)))
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
//    while (((CHRCNT > 0) && (TVT->TVT$WR_BCNT <= (TVT_TTY_BUFLEN - 1))))
//    while (((CHRCNT > 0) && (! TVT->TVT$GAG)))
    while (((Byte_Limit > 0) && (! TVT->TVT$GAG)))
	{

// Read another character from the network buffer, wrapping pointer as needed

	Prev_Char = CHR ;
	CHR = CH$RCHAR_A(CHRPTR);
	if (CHRPTR >= TCB->rcv_q_end)
	    CHRPTR = TCB->rcv_q_base;
	CHRCNT = CHRCNT - 1;
	Byte_Limit = Byte_Limit - 1 ;

	if ($$LOGF(LOG$TVT))
	    {
	    LOG$FAO("!%T TCB x!XL TCP_READ: CHRCNT = !SL, BCNT = !SL limit = !SL CHR = x!XB!/",
		0, TVT->TVT$TCB, CHRCNT, TVT->TVT$WR_BCNT, Byte_Limit, CHR) ;
	    };

// Handle this character based on the current state

#if 0
	CASE TVT->TVT$NRSTATE FROM TVT$STATE_MIN TO TVT$STATE_MAX OF
	SET
#endif
	  if (TVT->TVT$NRSTATE>=TVT$STATE_MIN && TVT->TVT$NRSTATE<=TVT$STATE_MAX) switch (TVT->TVT$NRSTATE) {
	case TVT$STATE_NORMAL:	// Normal state
X:	    {
	    if (CHR == TELNET$K_IAC)
		{

// Have an IAC - enter IAC state.

		TVT->TVT$NRSTATE = TVT$STATE_IAC;
		goto leave_x;
		};

// If reading subnegotiation string, then append byte to subnegotiation buffer

	    if (TVT->TVT$NR_SB)
		{
		if (TVT->TVT$SUB_CNT < TVT_SUB_BUFLEN)
		    {
		    CH$WCHAR_A(CHR,TVT->TVT$SUB_PTR);
		    TVT->TVT$SUB_CNT = TVT->TVT$SUB_CNT + 1;
		    };
		goto leave_x;
		};

// If we're in BINARY mode, just output the character

	    if (LCLOPTS[Telnet$K_Binary].OPT$STATE)
		{
		net_to_pty(TVT, CHR) ;
		}
	    else
		{

// For non BINARY mode, check for special processing following CR

		if (TVT->TVT$NR_CR)
		    {

// If we had a CR as the last character, then do special processing of char


		      switch (CHR)
			{
		    case CH_NUL:	// Null - fake a LF
			{
//!!HWM			net_to_pty(TVT, CH_LF) ;
			TVT->TVT$NR_CR = FALSE;
			};
			break;

		    case CH_CR:	// Another CR - append it & retain CR state
			{
			net_to_pty(TVT, CHR) ;
			};
		      break;

		    case CH_LF:	// LF after CR - drop LF and reset CR state
			{

//~~~ This code actually violates RFC 854, but is necessary to deal with many
//~~~ shithead UNIX systems which send CR-LF-LF when the user types CR-LF.
//~~~ We should be keeping the LF here.
//~~~ (We are, now.  HWM 4-Nov-91)
//!!			net_to_pty(TVT, CHR) ;
			TVT->TVT$NR_CR = FALSE;
			TVT->TVT$NR_LF = TRUE ;
			};
			break;

		    default: // Funny state - append and reset CR state
			{
			net_to_pty(TVT, CHR) ;
			TVT->TVT$NR_CR = FALSE;
			TVT->TVT$NR_LF = FALSE ;
			};
		    };
		    }
		else
		    {

//!// Didn't have a previous CR. Check for LF now and prepend a CR

//!//JC                    if (CHR == CH_LF)
//!//JC 			{
//!//JC 			TVT->TVT$NR_LF = TRUE ;
//!//JC !!!HWM			net_to_pty(TVT, CH_CR) ;
//!//JC 			if ($$LOGF(LOG$TVT))
//!//JC 			    {
//!//JC 			    LOG$FAO("!%T TCB x!XL TCP_READ: ADD CR, BCNT = !SL!/",
//!//JC 				0, TVT->TVT$TCB, TVT->TVT$WR_BCNT) ;
//!//JC 			    };
//!//JC 			} ;

// Didn't have a previous CR. Check for one now and output the byte.

		    if (CHR == CH_CR)
			{
			TVT->TVT$NR_CR = TRUE ;
			TVT->TVT$NR_LF = FALSE ;
			} ;
		    net_to_pty(TVT, CHR) ;
		    };
		};
	    };
	  leave_x:
 break; 
	case TVT$STATE_IAC:	// IAC - Start negotiation
	    {

// Select the different types of negotiations

	    XLOG$FAO(LOG$TELNEG,"!%T Negotiate !UB/",0,CHR);

	    switch (CHR)
	      {
	    case TELNET$K_IAC:	// Another IAC. Send one IAC to terminal
		{
		net_to_pty(TVT, CHR) ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
		break;

	    case TELNET$K_WILL:	// Remote WILL handle an option
		TVT->TVT$NRSTATE = TVT$STATE_WILL;
		break;

	    case TELNET$K_WONT:	// Remote WONT handle an option
		TVT->TVT$NRSTATE = TVT$STATE_WONT;
		break;

	    case TELNET$K_DO:	// Remote tells us DO an option
		TVT->TVT$NRSTATE = TVT$STATE_DO;
		break;

	    case TELNET$K_DONT:	// Remote tells us DONT do an option
		TVT->TVT$NRSTATE = TVT$STATE_DONT;
		break;

	    case TELNET$K_SB:	// Remote is starting subnegotiation
		{
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		TVT->TVT$NR_SB = TRUE;
		TVT->TVT$SUB_PTR = TVT->TVT$SUB_BUF;
		TVT->TVT$SUB_CNT = 0;
		};
		break;

	    case TELNET$K_SE:	// Remote is finished with subnegotiation
		{
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		TVT->TVT$NR_SB = FALSE;
		TVT->TVT$SUB_PTR = TVT->TVT$SUB_BUF;
		TVT_READ_SUB(TVT);
		};
		break;

	    case TELNET$K_AYT:	// User is nervous.  Comfort them.
		{
		if (! IS_CNTRLT_GOOD(TVT))
		    {
		    tcp_add_string(TVT,AYT_RESPONSE);
		    } ;
		net_to_pty(TVT, Telnet$K_Char_AYT) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
		break;

	    case TELNET$K_BRK:	// I think VMS ignores the break key.
		{
		net_to_pty(TVT, Telnet$K_Char_Brk) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
		break;

	    case TELNET$K_IP:	// Interrupt process
		{
		net_to_pty(TVT, Telnet$K_Char_IP) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
		break;

	    case TELNET$K_AO:	// Abort Output
		{
		net_to_pty(TVT, Telnet$K_Char_AO) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
		break;

	    case TELNET$K_EC:	// Erase Character
		{
		net_to_pty(TVT, Telnet$K_Char_EC) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
		break;

	    case TELNET$K_EL:	// Erase Line
		{
		net_to_pty(TVT, Telnet$K_Char_EL) ;
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
		break;

	    case TELNET$K_EOR:	// End of Record
		{
		net_to_pty(TVT, CH_CR) ;	// Fake it by stuffing a CR
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
		break;
		
	    case TELNET$K_DATA_MARK:		// Data Mark
		{
		net_to_pty(TVT, Telnet$K_Char_Purge) ;	// Purge typeahead
		TVT->TVT$CTRL = TRUE ;
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
		break;
		
	    default:	// Garbage to us
		{
		XLOG$FAO(LOG$TELNEG,"!%T Garbage !UB/",0,CHR);
		TVT->TVT$NRSTATE = TVT$STATE_NORMAL;
		};
	    };
	    };

	case TVT$STATE_WILL:	// Process the WILL according to option state
	    TVT_READ_WILL(TVT,CHR);
	    break;

	case TVT$STATE_WONT:	// Process the WONT
	    TVT_READ_WONT(TVT,CHR);
	    break;

	case TVT$STATE_DO:		// Process the DO
	    TVT_READ_DO(TVT,CHR);
	    break;

	case TVT$STATE_DONT:	// Process the DONT
	    TVT_READ_DONT(TVT,CHR);
	    break;

    default:
#if 0
      // check
	[INRANGE,OUTRANGE]:	// Shouldn't ever get here.
#endif
	    0;
	};

//~~~ Update receive window information.

	TCB->rcv_wnd = TCB->rcv_wnd + 1 ;
	TCB->pending_ack = TRUE;

// Update network receive queue pointer and count.

	TCB->rcv_q_deqp = CHRPTR;
	TCB->rcv_q_count = TCB->rcv_q_count - 1 ;

	if ((TVT->TVT$CTRL))
	    break ;
	};

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB x!XL TCP_READ: BCNT = !SL!/",
		0, TVT->TVT$TCB, TVT->TVT$WR_BCNT) ;
	};

// Indicate that we're no longer doing a network read

    TVT->TVT$NREAD = FALSE;

// If we moved some data, then force a ACK now

    if ((TCB->pending_ack))
	{
	tcp$enqueue_ack(TCB) ;
	} ;

    }


void PTY_TO_NET(TVT, CHR)
//
// PTY_TO_NET - Write a byte of data as to the network buffer.
//
	struct TVT$BLOCK * TVT;
    {
struct tcb_structure * TCB;
    signed long
	CHWMAX,
	CHWCNT,
	CHWPTR ;

// Make sure there is some room in the network output queue

    TCB = TVT->TVT$TCB;
    CHWMAX = TCB->snd_q_size-TCB->snd_q_count-TCB->srx_q_count;

//    if ($$LOGF(LOG$TVT))
//	{
//	LOG$FAO("!%T TCB x!XL PTY_TO_NET: CHWMAX = !SL!/",
//		0, TVT->TVT$TCB, CHWMAX) ;
//	};

    if (CHWMAX <= 0)
	return;

// If already doing a net write, then don't run here.

    if (TVT->TVT$NWRITE)
	return;

// Indicate that we have a write in progress

    TVT->TVT$NWRITE = TRUE;

// Copy bytes from the PTY read buffer to the network, quoting IAC's as needed.

    CHWCNT = 0;
    CHWPTR = CH$PTR(TCB->snd_q_enqp,0);

// Write a character to the output buffer, taking care to wrap the pointer

    CH$WCHAR_A(CHR,CHWPTR);
    if (CHWPTR >= TCB->snd_q_end)
	CHWPTR = TCB->snd_q_base;

// Update the network queue pointer and counter

    TCB->snd_q_enqp = CHWPTR;
    TCB->snd_q_count = TCB->snd_q_count + 1 ;
    TCB->snd_pptr = TCB->snd_nxt + TCB->snd_q_count;
    TCB->snd_push_flag = TRUE;

// If the send queue just became nonempty, then schedule a wakeup so that this
// TCB will be serviced soon.

//    if (TCB->snd_q_count == CHWCNT)
    if (TCB->snd_q_count == TCB->max_eff_data_size)
	$ACPWAKE;

// Indicate that we're not in the network write code any more

    TVT->TVT$NWRITE = FALSE;
    }


void TCP_WRITE(TVT)
//
// TCP_WRITE - Write as much data as possible from the PTY read buffer to the
// network.
//
	struct TVT$BLOCK * TVT;
    {
      struct tcb_structure * TCB;
    unsigned long
      Now;
signed long	CHWMAX,
	CHWCNT,
	CHWPTR,
	CHR;

// Make sure there is some room in the network output queue

    TCB = TVT->TVT$TCB;
    CHWMAX = TCB->snd_q_size-TCB->snd_q_count-TCB->srx_q_count;
    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB !XL TCP_WRITE: CHWMAX = !SL NWRITE=!UB!/",
		0, TVT->TVT$TCB, CHWMAX, TVT->TVT$NWRITE) ;
	};

    if (CHWMAX <= 0)
	{
// Something got jammed - gotta force a write
//	tcp$send_data(TCB) ;
	$ACPWAKE ;
	return;
	} ;
// If already doing a net write, then don't run here.

    if (TVT->TVT$NWRITE)
	return;

// Indicate that we have a write in progress

    TVT->TVT$NWRITE = TRUE;

// Copy bytes from the PTY read buffer to the network, quoting IAC's as needed.


    CHWCNT = 0;
    CHWPTR = CH$PTR(TCB->snd_q_enqp,0);
    while (TRUE)
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

	    if (TVT->TVT$NEG_CNT != 0)
		{

// Get next byte from negotiation buffer and send to net

		CHR = CH$RCHAR_A(TVT->TVT$NEG_DQP);
		if ((TVT->TVT$NEG_CNT = TVT->TVT$NEG_CNT-1) == 0)
		    TVT->TVT$NEG_DQP = CH$PTR(TVT->TVT$NEG_BUF,0);
		}
	    else
		{

// Get a character and check for an IAC that needs to be quoted

		if (TVT->TVT$RD_BCNT <= 0)
		    break;
		CHR = CH$RCHAR_A(TVT->TVT$RD_PTR);
		if (CHR == TELNET$K_IAC)
		    TVT->TVT$NW_IAC = TRUE;
		TVT->TVT$RD_BCNT = TVT->TVT$RD_BCNT - 1;
		};
	    };

// Write a character to the output buffer, taking care to wrap the pointer

	CH$WCHAR_A(CHR,CHWPTR);
	if (CHWPTR >= TCB->snd_q_end)
	    CHWPTR = TCB->snd_q_base;
	if ((CHWCNT = CHWCNT + 1) >= CHWMAX)
	    break;
	};

// Update the network queue pointer and counter

    TCB->snd_q_enqp = CHWPTR;
    TCB->snd_q_count = TCB->snd_q_count + CHWCNT;
    TCB->snd_pptr = TCB->snd_nxt + TCB->snd_q_count;
    TCB->snd_push_flag = TRUE;

// If the send queue just became nonempty, then schedule a wakeup so that this
// TCB will be serviced soon.

    Now = Time_Stamp() ;
    if ($$LOGF(LOG$TVT || LOG$TCP))
	{
	LOG$FAO("!%T TCB !XL TCP_WRITE: SNDQCNT= !SL SRXQCNT=!SL!/",
		0, TVT->TVT$TCB, TCB->snd_q_count, TCB->srx_q_count) ;
	LOG$FAO("!%T TCB !XL TCP_WRITE: NOW= !SL Delay=!SL!/",
		0, TVT->TVT$TCB, Now, TCB->snd_delay_timer) ;
	};

//    if (TCB->snd_q_count == CHWCNT)
    if ((((TCB->snd_q_count + TCB->srx_q_count) >= TCB->max_eff_data_size) ||
	 (Now >= TCB->snd_delay_timer)))
	$ACPWAKE;
//	tcp$send_data(TCB) ;

// Indicate that we're not in the network write code any more

    TVT->TVT$NWRITE = FALSE;
    }



void tcp_add_string(TVT,STRDESC_A)
//
// TCP_ADD_STRING - Write as much data as possible from the supplied string
// to the network write buffer via PTY_TO_NET.
//
	struct TVT$BLOCK * TVT;
    {
	struct dsc$descriptor * STRDESC = STRDESC_A;
	struct tcb_structure * TCB;
	signed long
	CHWMAX,
	CHWCNT,
	CHWPTR,
	CHRCNT,
	CHRPTR,
	CHR;

    CHRCNT = STRDESC->dsc$w_length;
    CHRPTR = CH$PTR(STRDESC->dsc$a_pointer,0);

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB !XL TCP_ADD_STRING: CNT !UL STR !AS!/",
	    0, TVT->TVT$TCB, CHRCNT, STRDESC) ;
	};

// Make sure there is some room in the PTY read queue

    if (TVT->TVT$RD_BCNT >= (TVT_TTY_BUFLEN-CHRCNT))
	return;

    CHWPTR = TVT->TVT$RD_PTR = CH$PTR(TVT->TVT$RD_BUF,0);

    while ((CHRCNT > 0))
	{
	CHR = CH$RCHAR_A(CHRPTR);
	PTY_TO_NET(TVT, CHR) ;
	CHRCNT = CHRCNT - 1;
	};

    }

void net_to_pty(TVT, CHR)
//
// NET_TO_PTY - Move a byte from the net buffer to PTY buffer and force a
// write, if possible
//
	struct TVT$BLOCK * TVT;
    {
struct PTY$IOSB * IO_Status = &TVT->TVT$WR_IOSB ;
    signed long
	Byte_Count,
	Bytes_Remaining,
	TT_WR_PTR,
	RC;

// Empty PTY buffers

//    PTY_READ(TVT) ;

//   Check to see if there is enough buffer space remaining

//    if ((TVT->TVT$WR_BCNT >= (TVT_TTY_BUFLEN - PTY_BUFFER_SIZE)))
    if ((TVT->TVT$WR_BCNT >= (TVT_TTY_BUFLEN - 2)))
	{
	TVT->TVT$GAG = TRUE ;
	}
    else
	{
	TVT->TVT$GAG = FALSE ;
	} ;

    if ((TVT->TVT$WR_BCNT >= (TVT_TTY_BUFLEN - 1)))
	{
	if ($$LOGF(LOG$TVT))
	    {
	    LOG$FAO("!%T TCB x!XL NET_TO_PTY: CHR = x!XB, PTY Buffer full!/",
		0, TVT->TVT$TCB, CHR) ;
	    };
	return ;
	} ;

//   Stuff character in PTY write buffer, update pointers

    TT_WR_PTR = CH$PTR(TVT->TVT$WR_BUF, TVT->TVT$WR_IPTR) ;
    CH$WCHAR_A(CHR, TT_WR_PTR) ;
    TVT->TVT$WR_ICNT = TVT->TVT$WR_ICNT + 1 ;
    TVT->TVT$WR_BCNT = TVT->TVT$WR_BCNT + 1 ;
    TVT->TVT$WR_IPTR = TVT->TVT$WR_IPTR + 1 ;

    if ((TVT->TVT$WR_IPTR >= TVT_TTY_BUFLEN))
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

    }



//SBTTL "Routines to interface to the PTY"

void PTY_SET_OWNER_PID(TVT)
//
// PTY_SET_OWNER_PID : Fill in the User_ID field of the associated TCB
//	by interrogating the appropriate device for the TELNET session.
//
// While we're here, we call POKEADDR to give it the information we would
// like (also to the VMS accounting file).
//
	struct TVT$BLOCK * TVT;
   {
     struct tcb_structure * TCB;
     unsigned char 	devstr[20];
     struct dsc$descriptor devnam_, *devnam=&devnam;
//!!	devsln,
     unsigned char 	ptynambuf[20];
     struct dsc$descriptor ptynam_ = {
		dsc$w_length : sizeof(ptynambuf),
		dsc$b_dtype : DSC$K_DTYPE_Z,
		dsc$b_class : DSC$K_CLASS_Z,
		dsc$a_pointer : ptynambuf}, *ptynam=&ptynam_;
signed long
	RC,
	Unit_Number,
	Owner_PID,
	Owner_UIC,
  FHost,FPort;
    extern	PokeAddr();
 struct item_list_3 Item_List[3];
 $DESCRIPTOR(lnm_table,"LNM$PROCESS_TABLE");
 $DESCRIPTOR(lnm_nam,"INET$PTY_TERM");
 struct item_list_3 itm[2]={ {buflen:20, item_code:1, bufaddr: ptynambuf, &ptynam->dsc$w_length }, {0,0,0,0} };

// point at the TCB for this TVT.
    TCB = TVT->TVT$TCB;
    FHost = TCB->foreign_host;

// fill in the owner pid. First get the unit number of the pty device,
// then build a request for the owner pid of the device associated with it.

    Item_List[0].item_code=DVI$_UNIT;
    Item_List[0].bufaddr=&Unit_Number;
    Item_List[1].item_code=0; // check

    RC = exe$getdviw (0,TVT->TVT$PTY_CHN,0,Item_List);
    if (RC)
	{
	  Item_List[0].item_code=DVI$_PID;
	  Item_List[0].bufaddr=&Owner_PID;
	  Item_List[1].item_code=DVI$_OWNUIC;
	  Item_List[1].bufaddr=&Owner_UIC;
	  Item_List[2].item_code=0; // check

// make a descriptor to hold the device name:

	devnam->dsc$b_dtype = DSC$K_DTYPE_Z;
	devnam->dsc$b_class = DSC$K_CLASS_Z;
	devnam->dsc$w_length = sizeof(devstr);
	devnam->dsc$a_pointer = devstr;

// generate this device name string using $FAO:

	ptynam->dsc$w_length = sizeof(ptynambuf);

	RC = exe$trnlnm(0, &lnm_table,
		&lnm_nam,		// JC
		0,		// JC
		&itm);				// JC
	if (! RC) TVT->TVT$DO_PID = 0;		// Cancel
	if (RC == SS$_NORMAL)
	    RC = exe$fao(/*%ASCID*/"_!ASA!UL:",devnam,devnam,ptynam,Unit_Number);
	    if (RC)
		RC = exe$getdviw (0,0,devnam,Item_List,0);

#if 0
    xlog$fao(LOG$TELNET,"!%T PTY_Set_owner_PID: TTY_TERM="!AS"!/",0,devnam);
#endif

// free the string descriptor, we don't need it any more.
//	LIB$FREE_VM(devsln,devstr);

// check return status from the $GETDVIW & $FAO calls.
	if (RC && (Owner_PID != 0))
	    {

	    // finally, we know the PID, so we can fill in the owner field
	    // of the appropriate TCB.
	    TCB->user_id=Owner_PID;
	    TVT->TVT$DO_PID = 0;

	    // Also, set up the information in the remote process'
	    // P1 space...
	    PokeAddr(Owner_PID,  TCB->foreign_host, TCB->foreign_port);

	    // Note the connection in the activity log file.
#if 0
	    ACT$FAO(
		"!%D Telnet-in (PID:x!XW UIC:!%U) <!UB.!UB.!UB.!UB/!UW>!/",0,
		.Owner_PID<0,16,0>, Owner_UIC,
		.FHost<0,8>,FHost<8,8>,FHost<16,8>,FHost<24,8>,
		TCB->foreign_port);
#endif
//
//	Now set any delayed device dependent
//
	    set_devdep(TVT);				// JC
	    PTY_WRITE(TVT);				// JC Write after Hold off
	    };
	};
    }



 void    PTY_READ_DONE();

void PTY_READ(TVT)
//
// PTY_READ - Initiate a read on the PTY device, if there is room in the PTY
// read buffer.
//
	struct TVT$BLOCK * TVT;
    {
      struct tcb_structure * TCB;
    signed long
	Byte_Count,
	RC;

// Make sure there is some room in the network output queue

    TCB = TVT->TVT$TCB;
    Byte_Count = TCB->snd_q_size-TCB->snd_q_count-TCB->srx_q_count;

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
	return;
    TVT->TVT$PREAD = TRUE;

// Compute realistic byte count

    Byte_Count = MIN(Byte_Count, TVT_TTY_BUFLEN) ;

// Initiate a read on the PTY device

    RC = exe$qio(0,
		 TVT->TVT$PTY_CHN,
		 IO$_READVBLK,
		 TVT->TVT$RD_IOSB,
		 PTY_READ_DONE,
		 TVT,
		 TVT->TVT$RD_BUF,
		 Byte_Count,0,0,0,0);
    if (! RC)
	{
	XLOG$FAO(LOG$TCPERR,
		 "!%T TVT PTY read $QIO failure for TCB x!XL, RC=x!XL!/",
		 0,TVT->TVT$TCB,RC);
	tcb$delete(TVT->TVT$TCB);
	};
    }

void PTY_READ_DONE(TVT)
//
// AST routine when PTY read operation finishes. Attempt to write the new data
// to the network, via TCP_WRITE.
//
	struct TVT$BLOCK * TVT;
    {
struct PTY$IOSB * IOSB = &TVT->TVT$RD_IOSB;
 struct tcb_structure * TCB;
    signed long
	Byte_Count,
	CHR,
	RC;

// Make sure the TVT is still valid

    if (TVT->TVT$CANCEL)
	return;

// Check the status of the read. SS$_CANCEL/SS$_ABORT are ignored.

    RC = IOSB->PTSB$STATUS;
    if ((RC == SS$_CANCEL) || (RC == SS$_ABORT))
	return;
    if (! RC)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT read error for TCB x!XL, RC=x!XL!/",
		 0,TVT->TVT$TCB,RC);
	tcb$delete(TVT->TVT$TCB);
	return;
	};

//    AST_IN_PROGRESS = TRUE;
    NOINT ;

// Clear read-in-progress and set number of bytes read.

    TVT->TVT$RD_BCNT = IOSB->PTSB$NBYTES;
    TVT->TVT$RD_PTR = CH$PTR(TVT->TVT$RD_BUF,0);
    TVT->TVT$PREAD = FALSE;

//    AST_IN_PROGRESS = FALSE;
    OKINT;

// check the User_ID field of the TCB. IF zero, we want to fill it in
// with the process ID of the connected TELNET session.
    TCB = TVT->TVT$TCB;
    if (TVT->TVT$DO_PID)
	{

//JC ----------------- Kludge -------------------------
//  The followin test is to make sure that we have an initial login prompt
//  This assumes that the prompt is longer than 7 but less than 15.
//  The actual length is 12, but what the heck give DEC some leeway.
//  Once we have a prompt it is save to do other operations.
//  In particular data written to PTY before the prompt is "promptly" lost!!!
//JC ----------------- Kludge -------------------------

	if ((TVT->TVT$RD_BCNT > 7) &&		// JC Kludge
	   (TVT->TVT$RD_BCNT < 15))			// JC Kludge
							// JC Kludge
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

//    while ((TVT->TVT$RD_BCNT > 0))
//	{
//	CHR = CH$RCHAR_A(TVT->TVT$RD_PTR) ;
//	PTY_TO_NET(TVT, CHR) ;
//	TVT->TVT$RD_BCNT = TVT->TVT$RD_BCNT - 1 ;
//	} ;

    if (! TVT->TVT$NWRITE)
	{
#ifdef USE_ASTS
	TCP_WRITE(TVT);
	Byte_Count = TCB->snd_q_size-TCB->snd_q_count-TCB->srx_q_count;
	if (((TVT->TVT$RD_BCNT > 0) || (Byte_Count > 0)))
	    {
	    exe$dclast(PTY_READ,
		TVT);
	    } ;
#else
	$ACPWAKE;
#endif
	} ;

    }


 void    PTY_WRITE_DONE();

void PTY_WRITE(TVT)
//
// PTY_WRITE - Initiate a write to the PTY device, if there is data in the PTY
// write buffer.
//
	struct TVT$BLOCK * TVT;
    {
      struct PTY$IOSB * IO_Status = &TVT->TVT$WR_IOSB;
    signed long
	Byte_Count,
	Bytes_Remaining,
	TT_WR_PTR,
	RC;
    unsigned char PTY_Char[8];

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB x!XL PTY_WRITE: OCNT = !SL, OPTR = !SL!/",
		0, TVT->TVT$TCB, TVT->TVT$WR_OCNT, TVT->TVT$WR_OPTR);
	};

// If already doing a PTY write, then don't run here.

    if (TVT->TVT$PWRITE)
	return;

    if (TVT->TVT$DO_PID)
     {		//!// JC hold off till proc ready
	if (TVT->TVT$HOLD) return;
	TVT->TVT$HOLD = 1;
	};

// Fill the TCP buffer

	TCP_READ(TVT) ;

// If buffer empty, then don't run here.

    if ((TVT->TVT$WR_BCNT == 0))
	return;

// Indicate that there is now a write in progress

    TVT->TVT$PWRITE = TRUE;

//   Get size of typeahead buffer
//    (This code doesn"t really work - TYPEAHDCNT apparently doesn"t work
//    for PTY's...)
//    (Have to put chan first?  What a crock...  -HWM)
//    RC = $QIOW(
//	    CHAN   = TVT->TVT$PTY_CHN,
//	    FUNC   = IO$_SENSEMODE+IO$M_TYPEAHDCNT,
//	    IOSB   = TVT->TVT$WR_IOSB,
//	    P1     = PTY_Char,
//	    P2     = 8);
//    if (! RC)
//	{
//	XLOG$FAO(LOG$TCPERR,
//		 "!%T TVT PTY sensemode $QIO failure for TCB x!XL, RC=x!XL!/",
//		 0, TVT->TVT$TCB, RC);
//	} ;
//
//    Bytes_Remaining = PTY_Char->IO$V_TYPEAHDCNT ;
//
//    if ($$LOGF(LOG$TVT))
//	{
//	LOG$FAO("!%T TCB x!XL PTY_WRITE: Bytes remaining = !SL, Status = !SL!/",
//		0, TVT->TVT$TCB, Bytes_Remaining, IO_STATUS->PTSB$STATUS) ;
//	};

//   Compute realistic byte count

    if ((TVT->TVT$WR_OPTR < TVT->TVT$WR_IPTR))
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
	    return ;
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
		Byte_Count, TVT->TVT$WR_BUF) ;
	};

// Initiate the write on the PTY

//    opr$fao("pty_write !SL bytes (x!XL,x!XL)",TVT->TVT$WR_BCNT,TVT->TVT$WR_BUF,TVT->TVT$WR_PTR);
    RC = exe$qio(0,
		 TVT->TVT$PTY_CHN,
		 IO$_WRITEVBLK,
		 TVT->TVT$WR_IOSB,
		 PTY_WRITE_DONE,
		 TVT,
		 TT_WR_PTR,
		 Byte_Count,0,0,0,0);
    if (! RC)
	{
	XLOG$FAO(LOG$TCPERR,
		 "!%T TVT PTY write $QIO failure for TCB x!XL, RC=x!XL!/",
		 0,TVT->TVT$TCB,RC);
//	if (IO_Status->PTSB$EXTRA1 != SS$_DATAOVERUN)
	    tcb$delete(TVT->TVT$TCB);
	TVT->TVT$PWRITE = FALSE;
	return;
	};

    }

void PTY_WRITE_DONE(TVT)
//
// PTY_WRITE_DONE - AST routine when PTY write operation finishes. Try to get
// more data for the pty via TCP_READ.
//
	struct TVT$BLOCK * TVT;
    {
      struct PTY$IOSB * IOSB = &TVT->TVT$WR_IOSB;
    signed long
	bytes_left,
	bytes_written,
      RC;
	struct tcb_structure * TCB;

// Make sure the TVT is still valid

    if (TVT->TVT$CANCEL)
	{
	return;
	} ;

// Check the status of the operation.

    RC = IOSB->PTSB$STATUS;
    if (RC == SS$_CANCEL)
	{
	return;
	} ;

    TCB = TVT->TVT$TCB;
    if (! RC)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT write error for TCB x!XL, RC=x!XL!/",
		 0,TCB,RC);
	if (RC != SS$_DATAOVERUN)
	    {
	    tcb$delete(TVT->TVT$TCB);
	    return;
	    };
	};

//    AST_IN_PROGRESS = TRUE;
//    NOINT ;

//   Update counters and pointers

    bytes_written = IOSB->PTSB$NBYTES;
    bytes_left = TVT->TVT$WR_BCNT - bytes_written ;
    TVT->TVT$WR_OPTR = TVT->TVT$WR_OPTR + bytes_written ;
    TVT->TVT$WR_OCNT = TVT->TVT$WR_OCNT + bytes_written ;
    TVT->TVT$WR_BCNT = TVT->TVT$WR_BCNT - bytes_written ;
    if ((TVT->TVT$WR_OPTR >= TVT_TTY_BUFLEN))
	{
	TVT->TVT$WR_OPTR = 0 ;
	} ;

//    if ((bytes_left > 0))
//	{
//    opr$fao("!!!pty_overun!!// !SL bytes",bytes_left,TVT->TVT$WR_BUF,TVT->TVT$WR_PTR);
//	CH$MOVE(bytes_left, TVT->TVT$WR_BUF + bytes_left, TVT->TVT$WR_BUF);
//	TVT->TVT$WR_PTR = TVT->TVT$WR_BUF + bytes_left;
//	} ;

// Indicate write no longer in progress and check for more data to send

    TVT->TVT$PWRITE = FALSE;
//    AST_IN_PROGRESS = FALSE;
//    OKINT ;

    if ($$LOGF(LOG$TVT))
	{
	LOG$FAO("!%T TCB x!XL PTY_WRITE_DONE: #bytes left=!SL, written = !SL!/",
		0, TVT->TVT$TCB, bytes_left, bytes_written) ;
	};

// Empty PTY buffers

    PTY_READ(TVT) ;

//    if ((! TVT->TVT$NREAD) && (TCB->rcv_q_count > 0))
#ifdef USE_ASTS
//	TCP_READ(TVT);
//	$DCLAST(ASTADR = PTY_WRITE,
//		ASTPRM = TVT);
//	PTY_WRITE(TVT) ;
#else
	$ACPWAKE;
#endif

    }

//SBTTL "Mailbox handling routines"

MBX_READ(TVT)
//
// MBX_READ - Initiate a read on the PTY's associated mailbox.
//
     struct TVT$BLOCK * TVT;
{
  signed long
    RC;

// Issue the read $QIO

    RC = exe$qio(0, 
		 TVT->TVT$MBX_CHN,
		 IO$_READVBLK,
		 TVT->TVT$MBX_IOSB,
		 MBX_READ_DONE,
		 TVT,
		 TVT->TVT$MBX_BUF,
		 TVT_MBX_BUFLEN, 0,0,0,0);
    if (! RC)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT MBX Read failure for TCB x!XL, RC=x!XL!/",
		 0,TVT->TVT$TCB,RC);
	return FALSE;
	};
    return TRUE;
    }

void MBX_READ_DONE(TVT)
//
// Here when we receive a message on the mailbox associated with the PTY. If we
// receive the terminal hangup signal (i.e. PTY has gone away).
//
	struct TVT$BLOCK * TVT;
    {
      struct _iosb * IOSB = &TVT->TVT$MBX_IOSB;
      struct _iosb * MBLOCK = &TVT->TVT$MBX_BUF; // check
    signed long
	MTYPE,
	RC;

// Make sure the TVT is still valid

    if (TVT->TVT$CANCEL)
	return;

// Check the status - ignore cancel/abort

    RC = IOSB->iosb$w_status;
    if (RC == SS$_ABORT)
	return;
    if (RC == SS$_CANCEL)
	return;

// Check for null status - just reqeueue the read

//    AST_IN_PROGRESS = TRUE;
//    NOINT ;
    if (RC == 0)
	{
	MBX_READ(TVT);
//	AST_IN_PROGRESS = FALSE;
//	OKINT ;
	return;
	};

// If we got an error, we have a problem. Abort.

    if (! RC)
	{
	XLOG$FAO(LOG$TCPERR,"!%T TVT MBX read failure for TCB x!XL, RC=x!XL!/",
		 0,TVT->TVT$TCB,RC);
	tcb$delete(TVT->TVT$TCB);
//	AST_IN_PROGRESS = FALSE;
//	OKINT ;
	return;
	};

// Get the message type and dispatch it.

    MTYPE = MBLOCK->iosb$w_status; // check
    switch (MTYPE)
      {
	case MSG$_TRMHANGUP:
	    {
	    XLOG$FAO(LOG$TCPERR,"!%T TVT hangup signal for TCB x!XL!/",
		     0,TVT->TVT$TCB);
	    tcp$tcb_close(TVT->TVT$TCB);
//	    AST_IN_PROGRESS = FALSE;
//	    OKINT ;
	    return;
	    };
break;
	};

// If we didn't handle it, it was ignored. Requeue the read.

//    AST_IN_PROGRESS = FALSE;
//    OKINT ;
    MBX_READ(TVT);
    }

//SBTTL "Option negotiation routines"

void Set_State_ON (TVT, OPTBLK)
     struct TVT$BLOCK * TVT;
	struct OPT$BLOCK * OPTBLK;
    {

    if (OPTBLK->OPT$STATE != OPT$STATE_ON)
	{
	OPTBLK->OPT$STATE = OPT$STATE_ON;
	if ((OPTBLK->OPT$ON_RTN != 0))
	  (OPTBLK->OPT$ON_RTN)(TVT);
	}
    }

void Set_State_OFF (TVT, OPTBLK)
     struct TVT$BLOCK * TVT;
	struct OPT$BLOCK * OPTBLK;
    {

    if (OPTBLK->OPT$STATE != OPT$STATE_OFF)
	{
	OPTBLK->OPT$STATE = OPT$STATE_OFF;
	if ((OPTBLK->OPT$OFF_RTN != 0))
	  (OPTBLK->OPT$OFF_RTN)(TVT);
	}
    }



 void    TVT_SEND();

void TVT_READ_WILL(TVT,OPTION)
//
// Received a WILL for an option. Handle according to the option table.
//
	struct TVT$BLOCK * TVT;
    {
      struct OPT$BLOCK * REMOPTS = &TVT->TVT$REMOPTS;
      struct OPT$BLOCK * OPTBLK;
      if (OPTION <= TELNET$K_MAXOPT)
	OPTBLK = REMOPTS[OPTION].OPT$BASE;
      else
	OPTBLK = DEFAULT_OPTION_BLOCK;

// Reset TVT read state

    XLOG$FAO(LOG$TELNEG,"!%T READ_Will !UB!/",0,OPTION);
    TVT->TVT$NRSTATE = TVT$STATE_NORMAL;

// If currently doing the option, then turn it on

    if (OPTBLK->OPT$CURRENT)
	{
	OPTBLK->OPT$CURRENT = FALSE;
	Set_State_ON(TVT, OPTBLK) ;
	}
    else

// Not currently doing negotiation. If we should turn it on, then do so.

	if (! OPTBLK->OPT$STATE)
	    {
	    if ((OPTBLK->OPT$PREFER == OPT$STATE_ON) ||
		(OPTBLK->OPT$PREFER == OPT$DONT_CARE))
		{
//!//JC		OPTBLK [OPT$STATE] = OPT$STATE_ON;
		Set_State_ON(TVT, OPTBLK);		// JC
		TVT_SEND(TVT,TELNET$K_DO,OPTION);
		}
	    else
		TVT_SEND(TVT,TELNET$K_DONT,OPTION);
	    }
    }

void TVT_READ_WONT(TVT,OPTION)
//
// Received a WONT for an option. Handle according to the option table.
//
	struct TVT$BLOCK * TVT;
    {
      struct OPT$BLOCK * REMOPTS = &TVT->TVT$REMOPTS;
      struct OPT$BLOCK * OPTBLK;
	    if (OPTION <= TELNET$K_MAXOPT)
	      OPTBLK = REMOPTS[OPTION].OPT$BASE;
	    else
	      OPTBLK = DEFAULT_OPTION_BLOCK;

// Reset TVT read state

    XLOG$FAO(LOG$TELNEG,"!%T READ_Wont !UB!/",0,OPTION);
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
//!//JC		OPTBLK->OPT$STATE = OPT$STATE_OFF;
		Set_State_OFF(TVT, OPTBLK);		// JC
		TVT_SEND(TVT,TELNET$K_DONT,OPTION);
		}
    }



void TVT_READ_DO(TVT,OPTION)
//
// Received a DO for an option. Handle according to the option table.
//
	struct TVT$BLOCK * TVT;
    {
      struct OPT$BLOCK * LCLOPTS = &TVT->TVT$LCLOPTS;
      struct OPT$BLOCK * OPTBLK;
	    if (OPTION <= TELNET$K_MAXOPT)
	      OPTBLK = LCLOPTS[OPTION].OPT$BASE;
	    else
	      OPTBLK = DEFAULT_OPTION_BLOCK;

// Reset TVT read state

    XLOG$FAO(LOG$TELNEG,"!%T READ_Do !UB!/",0,OPTION);
    TVT->TVT$NRSTATE = TVT$STATE_NORMAL;

// If currently doing the option, then turn it on

    if (OPTBLK->OPT$CURRENT)
	{
	OPTBLK->OPT$CURRENT = FALSE;
//!//JC	OPTBLK->OPT$STATE = OPT$STATE_ON;	// Redundant
	Set_State_ON(TVT, OPTBLK);		// JC Must set on
	}
    else

// Not currently doing negotiation. If we should turn it on, then do so.

	if (! OPTBLK->OPT$STATE)
	    {
	    if ((OPTBLK->OPT$PREFER == OPT$STATE_ON) ||
		(OPTBLK->OPT$PREFER == OPT$DONT_CARE)) 
		{
//!//JC		OPTBLK [OPT$STATE] = OPT$STATE_ON;
		Set_State_ON(TVT, OPTBLK);		// JC
		TVT_SEND(TVT,TELNET$K_WILL,OPTION);
		}
	    else
		TVT_SEND(TVT,TELNET$K_WONT,OPTION);
	    };
    }



void TVT_READ_DONT(TVT,OPTION)
//
// Received a DONT for an option. Handle according to the option table.
//
	struct TVT$BLOCK * TVT;
    {
      struct OPT$BLOCK * LCLOPTS = &TVT->TVT$LCLOPTS;
      struct OPT$BLOCK * OPTBLK;
      if (OPTION <= TELNET$K_MAXOPT)
	OPTBLK = LCLOPTS[OPTION].OPT$BASE;
      else
	OPTBLK = DEFAULT_OPTION_BLOCK;

// Reset TVT read state

    XLOG$FAO(LOG$TELNEG,"!%T READ_Dont !UB!/",0,OPTION);
    TVT->TVT$NRSTATE = TVT$STATE_NORMAL;

// If currently doing the option, then turn it off

    if (OPTBLK->OPT$CURRENT)
	{
	OPTBLK->OPT$CURRENT = FALSE;
//!//JC	OPTBLK->OPT$STATE = OPT$STATE_OFF;
	Set_State_OFF(TVT, OPTBLK);		// JC
	}
    else

// Not currently doing negotiation. If we should turn it off, do so.
// We must not prevent the remote host from reverting back to NVT

	{
	if (OPTBLK->OPT$STATE)
		{
//!//JC		OPTBLK->OPT$STATE = OPT$STATE_OFF;
//!//JC		OPTBLK->OPT$STATE = OPT$STATE_OFF;
		Set_State_OFF(TVT, OPTBLK);		// JC
		TVT_SEND(TVT,TELNET$K_WONT,OPTION);
		} ;
	} ;
    }



void TVT_SEND(TVT,OPR,OPTION)
//
// Send a TVT option negotiation. We queue the bytes needed to implement the
// option negotiation in the option buffer, and attempt a network write if
// none is currently happening.
//
	struct TVT$BLOCK * TVT;
    {
#define	TVT_OPTION_LEN 3

// Make sure there is room for the bytes we need to send.

    XLOG$FAO(LOG$TELNEG,"!%T Send !UB!/",0,OPTION);

    if (TVT->TVT$NEG_CNT >= (TVT_NEG_BUFLEN-TVT_OPTION_LEN))
	return;

// Insert the option negotiation bytes into the buffer

    if (TVT->TVT$NEG_CNT == 0)
	TVT->TVT$NEG_EQP = CH$PTR(TVT->TVT$NEG_BUF,0);
    CH$WCHAR_A(TELNET$K_IAC,TVT->TVT$NEG_EQP);
    CH$WCHAR_A(OPR,TVT->TVT$NEG_EQP);
    CH$WCHAR_A(OPTION,TVT->TVT$NEG_EQP);
    TVT->TVT$NEG_CNT = TVT->TVT$NEG_CNT + TVT_OPTION_LEN;
    }



void TVT_SEND_SUBOP(TVT,OPTION,DATA_A,size)
//
// Send a TVT suboption negotiation. We queue the bytes needed to implement the
// option negotiation in the option buffer, and attempt a network write if
// none is currently happening.
//
	struct TVT$BLOCK * TVT;
    {
      long i,
	data = DATA_A;
unsigned char
	Char ;

// Make sure there is room for the bytes we need to send.

    XLOG$FAO(LOG$TELNEG,"!%T Send_Subop !UB!/",0,OPTION);
    if (TVT->TVT$NEG_CNT >= (TVT_NEG_BUFLEN-(size+5)))
	return;

// Insert the option negotiation bytes into the buffer

    if (TVT->TVT$NEG_CNT == 0)
	TVT->TVT$NEG_EQP = CH$PTR(TVT->TVT$NEG_BUF,0);

    // Write the suboption header.
    CH$WCHAR_A(TELNET$K_IAC,TVT->TVT$NEG_EQP);
    CH$WCHAR_A(TELNET$K_SB,TVT->TVT$NEG_EQP);
    CH$WCHAR_A(OPTION,TVT->TVT$NEG_EQP);

    // Write the suboption data

//    CH$WCHAR_A(Option$K_Tog_Flow_Cntl_OFF,TVT->TVT$NEG_EQP);
    for (i=0;i<=size-1;i++)
	{
	Char = CH$RCHAR_A(data);
	CH$WCHAR_A(Char,TVT->TVT$NEG_EQP);
	};

    // Write the suboption trailer
    CH$WCHAR_A(TELNET$K_IAC,TVT->TVT$NEG_EQP);
    CH$WCHAR_A(TELNET$K_SE,TVT->TVT$NEG_EQP);

    TVT->TVT$NEG_CNT = TVT->TVT$NEG_CNT + (size+5);
    }



void TVT_READ_SUB(TVT)
//
// Received a Suboption for an option. Handle according to the option table.
//
	struct TVT$BLOCK * TVT;
    {
      struct OPT$BLOCK * OPTS = &TVT->TVT$REMOPTS;
    signed long
      sub_func;
	unsigned char opt;

    opt = CH$RCHAR_A(TVT->TVT$SUB_PTR);
    XLOG$FAO(LOG$TELNEG,"!%T READ_Sub !UB!/",0,opt);
    TVT->TVT$SUB_CNT = TVT->TVT$SUB_CNT -1 ;

     // If everything is kosher, execute the suboption handler.
     if (opt <= TELNET$K_MAXOPT)
	if ((OPTS[opt].OPT$SUB_RTN != 0) &&
	    (OPTS[opt].OPT$STATE == OPT$STATE_ON))
	  (OPTS[opt].OPT$SUB_RTN)(TVT);
    }



void TVT_NEGOTIATE(TVT)
//
// Initiate option negotiation for all of the negotiations that we prefer to be
// in the ON state. Loops through LCLOPTS, sending WILLs and REMOPTS sending
// DOs for them.
//
	struct TVT$BLOCK * TVT;
    {
    signed long
	OPTION;
    struct OPT$BLOCK * LCLOPTS = &TVT->TVT$LCLOPTS;
struct OPT$BLOCK * REMOPTS = &TVT->TVT$REMOPTS;

// First, do local options (offer to WILL them)

    for (OPTION=Telnet$K_MINOPT;OPTION<=TELNET$K_MAXOPT;OPTION++)
	{
	  struct OPT$BLOCK * OPTBLK = &LCLOPTS[OPTION].OPT$BASE;

// If off and we prefer on, send a WILL

	if ((! OPTBLK->OPT$STATE) &&
	    (OPTBLK->OPT$PREFER == OPT$STATE_ON))
	    {
	    OPTBLK->OPT$CURRENT = TRUE;
	    TVT_SEND(TVT,TELNET$K_WILL,OPTION);
	    };

// If on and we prefer it off, send a WONT

	if (OPTBLK->OPT$STATE &&
	    (OPTBLK->OPT$PREFER == OPT$STATE_OFF))
	    {
	    OPTBLK->OPT$CURRENT = TRUE;
	    TVT_SEND(TVT,TELNET$K_WONT,OPTION);
	    };
	};

// Then, do remote options (ask remote to DO them)

    for (OPTION=Telnet$K_MINOPT;OPTION<=TELNET$K_MAXOPT;OPTION++)
	{
	  struct OPT$BLOCK* OPTBLK = &REMOPTS[OPTION].OPT$BASE;

// If off and we prefer on, send a DO

	if ((! OPTBLK->OPT$STATE) &&
	    (OPTBLK->OPT$PREFER == OPT$STATE_ON))
	    {
	    OPTBLK->OPT$CURRENT = TRUE;
	    TVT_SEND(TVT,TELNET$K_DO,OPTION);
	    };

// If on and we prefer it off, send a DONT

	if (OPTBLK->OPT$STATE &&
	    (OPTBLK->OPT$PREFER == OPT$STATE_OFF))
	    {
	    OPTBLK->OPT$CURRENT = TRUE;
	    TVT_SEND(TVT,TELNET$K_DONT,OPTION);
	    };
	};
    }


