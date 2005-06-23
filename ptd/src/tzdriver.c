//
//	****************************************************************
//
//		Copyright (c) 1992, Carnegie Mellon University
//
//		All Rights Reserved
//
//	Permission  is  hereby  granted   to  use,  copy,  modify,  and
//	distribute  this software  provided  that the  above  copyright
//	notice appears in  all copies and that  any distribution be for
//	noncommercial purposes.
//
//	Carnegie Mellon University disclaims all warranties with regard
//	to this software.  In no event shall Carnegie Mellon University
//	be liable for  any special, indirect,  or consequential damages
//	or any damages whatsoever  resulting from loss of use, data, or
//	profits  arising  out of  or in  connection  with  the  use  or
//	performance of this software.
//
//	****************************************************************
//
#if 0
	.TITLE	TZDRIVER - Pseudo terminal driver for Front End and ethernet
	.IDENT	'V06-003'
#endif
//
// Comment out the following line if NOT building for VMS V4 (in other
// words comment out if you want a V5 driver).
//
//;;	VMS_V4 = 1
 
//++
// FACILITY:
//
//	VAX/VMS TERMINAL DRIVER
//
// ABSTRACT:
//
//	PSEUDO TERMINAL ROUTINES.
//
// AUTHOR:
//
//	19-NOV-1982	Dale Moore (dwm) at CMU
//		Redone for VMS V3.0
//
//	Version V06-003	27-Nov-1991	Henry W. Miller		USBR
//			Changed title to TZDRIVER.
//			Make sure UCB$x_STS is referenced properly depending
//			upon version of VMS.
//			Merge PZ and TZ UCB extensions together.
//			Use UCB$K_TP_LENGTH instead of UCB$K_TT_LENGTH,
//			they are defined differently in different places.
//			In TZ$RESUME, return via RSB.  I don't think any
//			further processing is required.  Also clear UCB$INT_M,
//			rather then set, to allow I/O to proceed.
//
//	Version V06-002	Henry W. Miller		USBR
//			Changed buffer size to 1024 to handle cut and paste
//			blasts.
//
//	Version V06-001	Bruce R. Miller		CMU NetDev
//			Changed name to PZDriver.  To hell with Digital.
//			I'm sick of worrying about DECWindows everytime
//			I make a change to this code.
//
//	This program has been granted to the public domain by the author.
//
// MODIFICATIONS:
//
//	V05-004 Kevin Carosso @ Network Research Co.
//		Conditionalize assembly for VMS V4 or V5.  There
//		is a magic symbol at the top that is commented
//		out for VMS V5.
//
//	V05-004	Digital Equipment Corp.	24-May-1988
//		Add code to delivered ASTs if PORT XON, PORT XOFF, or 
//		PORT SET_LINE routines are called.
//
//	V05-003	Digital Equipment Corp.	13-May-1988
//		1) Add hooks to indicate if line is XOFFed.
//		2) Set TTY$M_PC_NOMODEM bit in UCB$W_TT_PRTCTL.
//		3) Set device to HOSTSYNC.
//
//	V05-002	Digital Equipment Corp.	15-Feb-1988
//		Add TTY$M_PC_MULTISESSION bit to UCB$W_TT_PRTCTL so that
//		this device is not counted as a user in LOGIN.
//
//---------- Previous history ---------------------
//
//		DWM	- Added .Page above each .sbttl
//			- added modem transition routines in disco and init
//			- removed $ACBDEF external def's
//			- restored timeout to resume
//			- added ioc$initiate call in xoff and xon routines.
//	Version V03-002 - Changed TP startio to clear TIM in UCB$W_STS
//	Version V03-003 - Changed TW$RESUME to not set timeout bit in sts
//
//	Version V03-004 (Thu Dec  9 12:43:17 1982) D. Kashtan
//			  Made into a TEMPLATE driver.
//			  (***WARNING*** -- LOOK AT COMMENTS FOR ROUTINE
//			   TW$SET_LINE.  THERE IS IMPORTANT INFORMATION
//			   ABOUT CHANGES TO VMS THAT WILL AFFECT THE TWDRIVER)
//	Version V03-005 - (14-Jun-1983 )Dale Moore
//			  Add R4 to masks on calls to ioc$initiate.
//			  TTY$STARTIO clobber R4.
//	Version V03-006	- Change TW$XON and TW$XOFF routines from
//			  unconditionally calling IOC$INITIATE.
//			  IOC$INITIATE will branch to PY$STARTIO,
//			  Which will call TTY$GETNEXTCHAR (UCB$L_TT_GETNEXT),
//			  Which will branch to EOLSEEN,
//			  which calls TTY$READONE,
//			  which calls the PORT_XOFF routine which is TW$XOFF.
//			  This cycle was eating up all of the kernel stack.
//	Version V03-007 - (12-Jul-1983) Mark London
//			  Set default of TW to TERM to allow REPLY/USERS.
//			  Make TW owned and not a TEMPLATE in order that
//			  SHO TERM TWA0 does not cause a crash (SHOW attempts
//			  an ASSIGN to the device.)
//	Verison V04-001 - ( 9-Nov-1984 ) L. Bellamy and D. Davis
//			  In order of appearance:
//
//			  Added Object Rights Block offset definitions.
//			  Add TEMPLATE bit to UCB$W_STS.
//			  Add ORB definitions in prologue.
//			  Modify to allow vector definitions using VEC macro.
//			  Use CLASS_CTRL_INIT and CLASS_UNIT_INIT to locate
//			   vector table.
//			  Update powerfail routines.
//			  Use new methodology for XON/XOFF flow control.
//			  Get rid of all the SET_LINE stack manipulation
//			  since the terminal driver does not do anything
//			  malicious as suggested.
//
//			  NOTE - Comments in subroutine preambles not altered
//			  to reflect changes in most places.
//
//	Version V04-003 (24-Jun-1985) Kevin Carosso, Hughes Aircraft Co., S&CG
//			Cleaned this thing up quite a bit.
//			- Got rid of MBX characteristic on the devices.  This
//			  was a holdover to before cloned devices really
//			  existed.
//			- Leave the TW template device OFFLINE.  This is what
//			  other TEMPLATE devices do, to indicate that you
//			  really cannot do I/O to the template.  Also left
//			  it UNAVAILABLE, since the only way it is supposed to
//			  get used is by cloning in the PY device.
//			- Got rid of all modem operations.  Improper use tended
//			  to crash the system and they are not necessary.  TW
//			  device is always NOMODEM.  HANGUP works as you want
//			  it to without the modem stuff.
//			- Setup forced characteristics and default permanent
//			  characteristics.  Forced are: NOAUTOBAUD, and NODMA.
//			  Default is HANGUP.
//			- Fixed up TW$DISCONNECT to properly send a message
//			  to PY device's associated mailbox only if we are
//			  actually doing a hangup.  It used to do it every
//			  time.  Changed the message to MSG$_TRMHANGUP.
//			- Got rid of the BRDCST on/off stuff.  It doesn't seem
//			  to be necessary any more.  It also had a bug in it
//			  somewhere that caused the terminal to start off
//			  NOBRDCST when it shouldn't.
//			- General house-cleaning.  Got rid of commented out
//			  lines from VMS V3 version.  Fixed up typos in 
//			  comments.
//
//	Version V04-004 (10-Feb-1986) Kevin Carosso, Hughes Aircraft Co., S&CG
//			Changed all references to PTDRIVER to TPDRIVER because
//			DEC (bless their little hearts) invented the %*%#$%
//			TU81 and use PTA0: now.
//
//	Version V04-005 (3-Sep-1986) Kevin Carosso, Hughes Aircraft Co., S&CG
//			Fixed bug whereby the sequence ^S followed by ^Y
//			would cause a system hang.  Apparently the class
//			driver calls the port RESUME routine when doing
//			an ABORT and again when canceling the ^S.  Calling
//			RESUME twice like this is bad.  In general, the
//			port RESUME routine should not restart the output.
//			In this driver, the RESUME and STOP routines aren't
//			needed, so I got rid of them completely.
//
//			Now on ^Y the ^S state is not cleared.  You still
//			need to type ^Q to get your output (including the
//			*INTERRUPT* echo string.  I thought this was another
//			bug, but it's how my DHU-11 based terminals act
//			and makes sense, because only a ^Q should really
//			cancel a ^S.
//
//			Also removed the TEMPLATE bit from the TWA0: template
//			device since it was allowing people to $ASSIGN channels
//			and get unhappy (lonely) TW devices that could crash
//			the system.  Now, the only way to clone TW devices
//			is through the code in PYDRIVER, hence by assigning
//			a channel to PYA0:.  You can still $ASSIGN to TWA0:,
//			but since the device is offline that shouldn't hurt
//			anything.  This fixed crashes caused by KERMIT and
//			other things attempting to use TWA0:.  You now get
//			an error "DEVOFFLINE".
//
//			Fixed a bug that caused a system crash from SHOW
//			DEVICE/FULL of TWA0:.  The ACL_QUEUE bit was set
//			in the ORB but the ACL queue was invalid (was zeroed).
//			I got rid of the bit, now all is well.
//
//	Version V04-006 (5-Dec-1986) Kevin Carosso, Hughes Aircraft Co., S&CG
//			Fixed the infamous character munging bug.  The fix
//			is really in PYDRIVER.
//
//	Version V04-007 (10-JUL-1987) Kevin Carosso, Hughes Aircraft Co., S&CG
//			In TW$INITLINE make sure to tell the class driver
//			never to time out.  This fix from Forrest Kenney
//			at DEC.
//
//			Also, while we're in here, lets make the device
//			acquire "NODE$" prefixes, since all the other
//			terminal drivers do.
//
//	Version V04-008	(2-NOV-1987) Kevin Carosso, Network Research Co.
//			Only a version change here to keep in sync with
//			TWDRIVER which has the real change.
//
//			Fix thanks to Gerard K Newman @ San Diego
//			Supercomputer Center.
//
//	Version V05-001 (21-Dec-1987) Digital Equipment Corp.
//			Add support for Symmetric Multiprocessing. 
//
//	Version V05-002 Digital Equipment Corp.
//			Change TP driver names to TW. (TP driver it turns
//			out, conflicted with the VAX/PSI terminal driver.)
//			TWDRIVER and PYDRIVER are now registered with SQM 
//			so there will be no more conflicts.
//
//	Version V05-003 Dale Moore	Carnegie Mellon University
//			Added some fixes gleaned from DECWindows version.
//--

#if 0
	.PAGE
	.SBTTL	Declarations
 
	.LIBRARY	/SYS$LIBRARY:LIB.MLB/
#endif 
//
// EXTERNAL DEFINITIONS:
//
#if 0
.NOCROSS
#endif
#include <acbdef.h>				// AST control block definitions
#include <crbdef.h>				// DEFINE CRB
#include <dcdef.h>				// DEVICE CLASSES AND TYPES
#include <ddbdef.h>				// DEFINE DDB
#include <ddtdef.h>				// DEFINE DDT
#include <devdef.h>				// DEVICE CHARACTERISTICS
#include <dptdef.h>
#include <dyndef.h>				// Dynamic structure definitions
#include <idbdef.h>				// DEFINE IDB OFFSETS
#include <iodef.h>				// I/O Function Codes
#include <irpdef.h>				// IRP definitions
#include <msgdef.h>				// Message types
#include <orbdef.h>				// Define Object's Rights Block offsets

#include <splcoddef.h>			// Spinlock code definitions

#include <ttdef.h>				// DEFINE TERMINAL TYPES
#include <tt2def.h>				// Define Extended Characteristics
#if 0
#include <ttydefs.h>				// DEFINE TERMINAL DRIVER SYMBOLS
#endif
#include <ttydef.h>			// DEFINE TERMINAL DRIVER SYMBOLS
#include <ttyucbdef.h>
#if 0
#include <ttymacs.h>			// DEFINE TERMINAL DRIVER MACROS
#endif
#include <ucbdef.h>				// DEFINE UCB
#include <vecdef.h>				// DEFINE VECTOR FOR CRB
#include <ttyvecdef.h>				// DEFINE VECTOR FOR CRB

#if 0
.CROSS
#endif

#include <system_data_cells.h>

#include <linux/kernel.h>

#define TTY$GW_DEFBUF tty$gw_defbuf
#define TTY$GL_DEFCHAR tty$gl_defchar
#define TTY$GL_DEFCHAR2 tty$gl_defchar2
#define TTY$GB_DEFSPEED tty$gb_defspeed
#define TTY$GB_RSPEED tty$gb_rspeed
#define TTY$GB_PARITY tty$gb_parity
 
#define BUFFER_SIZE 1024
 
//
// LOCAL DEFINITIONS
//
static int DT$_TZ = 0xFF;

//
// Length of access port buffer in UCB
//
#define ACCPOR_SIZE 30
 
//
// Definitions that follow the standard UCB fields for TZ driver
//  This will all probably have to be the same as the standard term
 
struct _tz_ucb {			// Start of UCB definitions
 
  struct _ucb tz$ucb;		// Position at end of UCB
 
  long ucb$l_tz_xucb;	// UCB of corresponding
					//  control/application unit
  long ucb$l_tz_xon_ast;	// AST list for XON event notification
  long ucb$l_tz_xoff_ast;	// AST list for XOFF event notification
  long ucb$l_tz_set_ast;	// AST list for notification of SET_LINE 
#define	UCB$K_TZ_LEN	(sizeof(struct _ucb)+16);		// Size of UCB
 
//;;	$DEFEND	UCB			// End of UCB definitions
//
// Definitions that follow the standard UCB fields in PZ devices
//

//;;	$DEFINI UCB			// Start of UCB definitions
//;;	.=UCB$K_LENGTH			// position at end of UCB
//;;	.=UCB$K_TZ_LEN			// position at end of UCB
  long ucb$l_pz_xucb;		// UCB of terminal part of pseudo terminal
  char ucb$t_pz_buffer[BUFFER_SIZE]; // Buffer to store characters to be transmitted
char ucb$a_rem_host_len;	// Size of remote host ID string.
  char ucb$a_rem_host_str[ACCPOR_SIZE]; // Space for remote host ID string.

#define	UCB$K_PZ_LEN (sizeof(struct _tz_ucb))			// Size of UCB

};			// end of UCB definitions
 
//
// LOCAL STORAGE
//
#if 0
	.PSECT	$$$105_PROLOGUE

	.PAGE
	.SBTTL	Standard Tables
#endif 
//
// Driver prologue table:
//
#if 0
TZ$DPT::
	DPTAB	-			// DRIVER PROLOGUE TABLE
		END=TZ_END,-		// End and offset to INIT's vectors
		UCBSIZE=UCB$K_TZ_LEN,- 	// SIZE OF UCB
		FLAGS=DPT$M_NOUNLOAD,-	// Do not allow unload
		ADAPTER=NULL,-		// ADAPTER TYPE
		NAME=TZDRIVER,-		// NAME OF DRIVER
		VECTOR=PORT_VECTOR
	DPT_STORE INIT
	DPT_STORE UCB,UCB$W_UNIT_SEED,W,0	// SET UNIT # SEED TO ZERO

#if	defined	VMS_V4
	DPT_STORE UCB,UCB$B_FIPL,B,8 		// FORK IPL
#else
	DPT_STORE UCB,UCB$B_FLCK,B,SPL$C_IOLOCK8 ; FORK LOCK
#endif

	DPT_STORE UCB,UCB$L_DEVCHAR,L,<-; CHARACTERISTICS
			DEV$M_REC!-	;
			DEV$M_IDV!-	;
			DEV$M_ODV!-	;
			DEV$M_TRM!-	// Terminal Device
			DEV$M_CCL>
	DPT_STORE UCB,UCB$L_DEVCHAR2,L,<-		// Device characteristics
			DEV$M_RTT!-			// remote terminal
			DEV$M_NNM>			// prefix with "NODE$"
	DPT_STORE UCB,UCB$B_DEVCLASS,B,DC$_TERM		;
	DPT_STORE UCB,UCB$B_TT_DETYPE,B,TT$_UNKNOWN	// TYPE
	DPT_STORE UCB,UCB$W_TT_DESIZE,@W,TTY$GW_DEFBUF	// BUFFER SIZE
	DPT_STORE UCB,UCB$L_TT_DECHAR,@L,TTY$GL_DEFCHAR	// DEFAULT CHARACTERS
	DPT_STORE UCB,ucb$l_tt_decha1,@L,TTY$GL_DEFCHAR2; DEFAULT CHARACTERS
	DPT_STORE UCB,UCB$W_TT_DESPEE,@B,TTY$GB_DEFSPEED; DEFAULT SPEED
	DPT_STORE UCB,UCB$W_TT_DESPEE+1,@B,TTY$GB_RSPEED; DEFAULT SPEED
	DPT_STORE UCB,UCB$B_TT_DEPARI,@B,TTY$GB_PARITY	// DEFAULT PARITY
	DPT_STORE UCB,UCB$B_TT_PARITY,@B,TTY$GB_PARITY	// DEFAULT PARITY
	DPT_STORE UCB,UCB$B_DEVTYPE,B,TT$_UNKNOWN	// TYPE
	DPT_STORE UCB,UCB$W_DEVBUFSIZ,@W,TTY$GW_DEFBUF	// BUFFER SIZE
	DPT_STORE UCB,UCB$L_DEVDEPEND,@L,TTY$GL_DEFCHAR	// DEFAULT CHARACTERS
	DPT_STORE UCB,UCB$L_TT_DEVDP1,@L,TTY$GL_DEFCHAR2; Default Characters
	DPT_STORE UCB,UCB$W_TT_SPEED,@B,TTY$GB_DEFSPEED	// DEFAULT SPEED
	DPT_STORE UCB,UCB$W_TT_SPEED+1,@B,TTY$GB_RSPEED	// DEFAULT SPEED
	DPT_STORE UCB,UCB$B_DIPL,B,8			// DEV IPL (no device)
	DPT_STORE UCB,UCB$L_TT_WFLINK,L,0	// Zero write queue.
	DPT_STORE UCB,UCB$L_TT_WBLINK,L,0	// Zero write queue.
	DPT_STORE UCB,UCB$L_TT_RTIMOU,L,0	// Zero read timed out disp.
//
// Added ORB definitions 
//
//	DPT_STORE ORB, ORB$B_FLAGS, B, <ORB$M_PROT_16>
//	DPT_STORE ORB, ORB$W_PROT, @W, TTY$GW_PROT
//	DPT_STORE ORB, ORB$L_OWNER, @L, TTY$GL_OWNUIC
 
	DPT_STORE DDB,DDB$L_DDT,D,TZ$DDT
 
	DPT_STORE REINIT
	DPT_STORE CRB,CRB$L_INTD+VEC$L_INITIAL,D,TZ$INITIAL; CONTROLLER INIT
	DPT_STORE CRB,CRB$L_INTD+VEC$L_UNITINIT,D,TZ$INITLINE; UNIT INIT
	DPT_STORE END
 
	DDTAB	DEVNAM	= TZ,-		// Dummy TZ port Driver Dispatch table
		START	= 0,-
		FUNCTB	= 0
 
	.PSECT $$$115_DRIVER,LONG
 
// The associated class driver uses this table to command the port driver.
// The address of the table is contained in the terminal UCB extension area.
// The offset definitions are defined by the ttydefs.
#endif

	  void SET_FORCED_CHARS(struct _ucb * ucb);

//
// TZ specific dispatch table
//
	  struct _tt_port port_vector;

void TZ$NULL()				// Null port routine
{ }
 

#if 0
	.PAGE
	.SBTTL	TZ$INITIAL - Initialize pseudo terminal interface
#endif
//++
// TZ$INITIAL - INITIALIZE INTERFACE
//
// FUNCTIONAL DESCRIPTION:
//
// This routine is entered at device CONNECT time and power recovery.
// All we do is connect ourselves up to the class driver.
//
// INPUTS:
//
//	R4 = ADDRESS OF THE UNIT CSR
//	R5 = IDB OF UNIT
//	R8 = ADDRESS OF THE UNIT CRB
//
// OUTPUTS:
//
//	R0, R1, R2, R3 are destroyed.
//
// IMPLICIT INPUTS:
//
//	IPL = IPL$_POWER
//
//--
	void TZ$INITIAL(R8)
     struct _crb * R8;
{
 
  //	CLASS_CTRL_INIT TZ$DPT,PORT_VECTOR // check. do not need relocs?
  extern struct _dpt tz$dpt;
  tz$dpt.dpt$ps_vector=&port_vector; // does CLASS... do this? already done?
  R8->crb$b_tt_type=DT$_TZ;
}

#if 0
	.PAGE
	.SBTTL	TZ$INITLINE - RESET INDIVIDUAL LINE
#endif
//++
// TZ$INITLINE - RESET pseudo terminal control state
//
// FUNCTIONAL DESCRIPTION:
//
// This routine performs a simple unit initialization.
//
//
// INPUTS:
//
//	R5 = UCB ADDRESS
//
// OUTPUTS:
//
//	R2,R5 ARE PRESERVED.
//--
void TZ$INITLINE(struct _idb * idb, struct _ucb * ucb)				// RESET SINGLE LINE
{
  int R3,R4,R6,R7,R8,R9;
  struct _tt_class * R0;
  struct _ucb * R1 = 0;
  struct _tty_ucb * tty;
  struct _tt_class * R2;
  R0=&port_vector; // check TZ$VEC?		// Set TZ port vector table 
  CLASS_UNIT_INIT(ucb,R0);			
  if	(ucb->ucb$w_unit==0)	// Skip initialization of TEMPLATE
    	return;			// Unit #0 = Template: Skip everything!
  if	(UCB$M_POWER&ucb->ucb$l_sts)	// Skip if powerfail recovery
    goto l20;
  R1	 = ((struct _tty_ucb *)ucb)->ucb$l_tt_logucb;		// Look at logical term UCB
  if (R1 &&				// If none, then has no refs

      R1->ucb$l_refc)		// See if TZ has any references
    goto	l20;				// If so don't reinit ucb
  SET_FORCED_CHARS(ucb);		// Set required characteristics
  tty=ucb;
  tty->ucb$l_tt_decha1|=TT2$M_HANGUP;	// Set default characteristics
  R1	 = tty->ucb$l_tt_class;		// Address class vector table

  tty=ucb;
  tty->ucb$w_tt_prtctl|=	TTY$M_PC_NOTIME|// Tell class driver not to time out,
    TTY$M_PC_NOMODEM|	-	// do not allow modem processing,
    TTY$M_PC_MULTISESSION;	// and do not count as user 

  ucb->ucb$l_devdepend &= ~TT$M_MODEM;		// Do not allow modem bit to ever be  
  tty->ucb$l_tt_dechar &= ~TT$M_MODEM;		// set.
  ucb->ucb$l_devdepend |= TT$M_HOSTSYNC;		// Always set device to HOSTSYNC
  tty->ucb$l_tt_dechar |= TT$M_HOSTSYNC;		//
  R2=R1;
  R2->class_setup_ucb(tty);		// Init ucb fields
 l20:
  if	((UCB$M_POWER&ucb->ucb$l_sts)==0) return;	// Powerfail handler
  R0	 = tty->ucb$l_tt_class;
  R2=R1;
  R2->class_powerfail(ucb);
  
  return;
}
 
 

//++
// TZ$SET_LINE - Used to Reset SPEED and UCB
//
// FUNCTIONAL DESCRIPTION:
//
//	Called whenever someone tries to set terminal modes/characteristics.
//	All we do is reset anything that we think should never be changed.
//
// INPUTS:
//
//	R5 = UCB ADDRESS of TZ
//
// OUTPUTS:
//
//	none
//--
void TZ$SET_LINE(struct _ucb * ucb) {
  unsigned long ** acb_p;
  struct _tz_ucb * tz = ucb;
  struct _tty_ucb * tty = ucb;
  acb_p=&tz->ucb$l_tz_set_ast;		// Get list head address
  if	(*acb_p)				// See if list is empty
    			// Empty do not deliver ASTs
    com_std$delattnast(acb_p, ucb);		// Delliver all ASTs 
  SET_FORCED_CHARS(ucb);
}

void SET_FORCED_CHARS(struct _ucb * ucb) {
  struct _tty_ucb * tty = ucb;
  //
  // This little routine sets certain required characteristics.  It is called by
  // the INITLINE code to set them at the outset and by the SETLINE code to reset
  // them unconditionally if someone tries to set them.
  //
  ucb->ucb$l_devdepnd2 &= ~(TT2$M_DMA | TT2$M_AUTOBAUD);
  tty->ucb$l_tt_decha1 &= ~(TT2$M_DMA | TT2$M_AUTOBAUD);
		
  return;
}
 
#if 0

 
	.Page
#endif
//++
// TZ$DISCONNECT - SHUT OFF UNIT
//
// FUNCTIONAL DESCRIPTION:
//
// This routine is used when for some reason the unit must be disconnected.
// This can be at hangup or last deassign.  If the PZ device has an associated
// mailbox, signal an MSG$_TRMHANGUP in it.
//
// Although we are disconnecting a virtual device, we don't do anything
// more than send a hangup message because we want to allow the device to
// be reusable.  It's really only if the control device (PZ) goes away
// that we mark the TZ offline and delete it's UCB.  That code's all in
// PZDRIVER.
//
// INPUTS:
//
//	R0 = (0 for hangup, 1 for nohangup)
//	R5 = UCB ADDRESS of TZ
//
// OUTPUTS:
//
//	R3,R4 ARE USED.
//--

	static int indent_dummy;
void TZ$DISCONNECT(struct _ucb * ucb, int r0) {
  int R3;
  if (r0) return;			// If no hangup, skip all this.
 
  struct _tz_ucb * tz = 0; // check
  ucb = tz->ucb$l_tz_xucb;		// Get PZ UCB
  if (ucb) {			// If disconnected, ignore
    R3 = ucb->ucb$l_amb;		// Load Associated Mailbox of PZ UCB
    if (R3) {			// If EQL then no mailbox
      int R4 =	MSG$_TRMHANGUP;	// Load Message Type
      exe_std$sndevmsg();		// Send the message
    }
  }
 return;
}
 
 

#if 0
	.PAGE
	.SBTTL	TZ START I/O ROUTINE
#endif
//++
// TZ$STARTIO - START I/O OPERATION ON TZ
//
// FUNCTIONAL DESCRIPTION:
//
//	This routine is entered from the device independent terminal startio
//	routine to enable output interrupts on an idle TZ unit
//
// INPUTS:
//
//	R3 =	Character	AND	CC = Plus (N EQL 0)
//	R3 =	Address		AND	CC = Negative (N EQL 1)
//
//	R5 = UCB ADDRESS
//
// OUTPUTS:
//
//	R5 = UCB ADDRESS
//--
	static int dummy2;

void TZ$STARTIO(int R3, struct _ucb * u, signed int CC) {				// START I/O ON UNIT
  struct _ucb * ucb = u;
  struct _tty_ucb * tty=ucb;
  if (CC==1) {			// Single character
    tty->tty$b_tank_char = R3;		// Save output character
    tty->ucb$w_tt_hold|=TTY$M_TANK_HOLD;	// Signal charater in tank
  } else {
    tty->ucb$w_tt_hold|=TTY$M_TANK_BURST;	// Signal burst active
  }

  //
  // Here we must do something to notify our mate device that
  // there is data to pick up
  //
  ucb = ((struct _tz_ucb *)ucb)->ucb$l_tz_xucb;		// Switch to PZ UCB
  if (ucb) {			// PZ is disconnected: skip

    FORKLOCK();	// Take out PZ device FORK LOCK
#if 0
    LOCK=ucb$b_flck(ucb), -	;
    SAVIPL=-(SP),	-	;
    PRESERVE=NO
#endif
      if	(UCB$M_BSY&		// If the device isn't busy,
		 ucb->ucb$l_sts) 	// then dont do i/o

	ioc$initiate(ucb->ucb$l_irp, ucb);		// IOC$INITIATE needs IRP addr

    FORKUNLOCK();			// Release PZ drvice FORK LOCK
#if 0
    LOCK=ucb$b_flck(ucb), -	;
    NEWIPL=(SP)+,	-	;
    PRESERVE=NO,	-	;
    CONDITION=RESTORE	;
#endif
		
  return;
  } else {
  //
  // Come here if we have no PZ control device to send stuff to.  Just
  // suck all the data we can out of the class driver and throw it away.
  //
    ucb=u;
    tty=ucb;
    tty->ucb$w_tt_hold&= ~(	TTY$M_TANK_HOLD|	// Nothing in progress now
				TTY$M_TANK_BURST|
				TTY$M_TANK_PREMPT);
  }
  do {
    ucb->ucb$l_sts&=~UCB$M_INT; 
    tty=ucb;
    long chr, cc;
    tty->ucb$l_tt_getnxt(&chr,&cc,ucb);
  } while (tty->ucb$b_tt_outype);

  return;
 
}

#if 0
	.PAGE
	.SBTTL	Port Routines Stop,Resume,XON,XOFF
#endif
//++
// TZ$XOFF -	Send Xoff
// TZ$XON -	Send Xon
// TZ$ABORT -	Abort current output
//
// Functional Description:
//
//	These routines are used by the terminal class driver to
//	control output on the port
//
// Inputs:
//
//	R5 = UCB Address
//
// Outputs:
//
//	R5 = UCB Address
//--

	  void TZ$XON_XOFF_CMN(struct _ucb * ucb, struct _acb ** acb_p, int R3);

	  static int	TTY$M_TP_XOFF = 8;
static int	TTY$V_TP_XOFF = 3;
#if 0
ASSUME	TTY$M_TP_XOFF EQ TTY$M_TP_DLLOC+4
ASSUME  TTY$V_TP_XOFF EQ TTY$V_TP_DLLOC+1
#endif

void TZ$XOFF(void * vec, int R3, struct _ucb *ucb) {
  struct _tz_ucb * tz=ucb;
  struct _acb * acb_p=&tz->ucb$l_tz_xoff_ast ; //Get address of AST list
  if	(R3!=0x13)		// Is XOFF
    return TZ$XON_XOFF_CMN(ucb,acb_p,R3);		// NEQ not XONN use common rotuine
  struct _tpd_ucb * tpd = ucb;
  tpd->ucb$b_tp_stat|=TTY$M_TP_XOFF;	// Set XOFF bit in UCB
  TZ$XON_XOFF_CMN(ucb,acb_p,R3);		// Do common processing
}

void TZ$XON (void * vec, int R3, struct _ucb * ucb) {
  struct _tz_ucb * tz=ucb;
  struct _acb ** acb_p=&tz->ucb$l_tz_xon_ast ; //Get address of AST list
  if (R3!=0x11)		// Is it XON
    return	TZ$XON_XOFF_CMN(ucb,acb_p,R3);		// NEQ not XONN use common rotuine
  struct _tpd_ucb * tpd = ucb;
  tpd->ucb$b_tp_stat&=~TTY$M_TP_XOFF;	// Clear XOFF bit in UCB
  ;
  TZ$XON_XOFF_CMN(ucb,acb_p,R3);
}

void TZ$CONT (struct _ucb * ucb);

//
// Schedule xon/xoff to be sent, and changed schedule bit mask 
//
void TZ$XON_XOFF_CMN(struct _ucb * ucb, struct _acb ** acb_p, int R3) {			// Common XONN/XOFF code
  struct _tty_ucb * tty=ucb;
  long * R0;
  short * s;
  if	(*acb_p) {		// See if AST present
    // EQL no AST to deliver
    R0		 = acb_p;		// Copy list address

  l10:
    R0=*R0;			// Get next entry
    if (R0==0) goto	l20;			// All done so deliver ASTs
    //	MOVZBW	R3,ACB$L_KAST+4(R0)	// Store new param value
    s=(short*)R0+3; // check
    *s=R3;
    // was: MOVZBW	R3,ACB$L_KAST+6(R0)	// Store new param value
    goto	l10;			// Look for another entry
  l20:
    com_std$delattnast(acb_p, ucb);	// Deliver the ASTs
  }
  ucb->ucb$l_sts|=UCB$M_INT;
  tty->ucb$w_tt_hold|=TTY$M_TANK_PREMPT;	// Schedule xon
  tty->ucb$b_tt_prempt		 = R3;		// Save character
  TZ$CONT(ucb);
}

//
// Resume any active output
//
void TZ$RESUME (struct _ucb * ucb) {
//;;	BISL	#UCB$M_INT,ucb->ucb$l_sts
  ucb->ucb$l_sts&=~UCB$M_INT;
  struct _tty_ucb * tty;
  tty=ucb;
  tty->ucb$w_tt_hold&=~TTY$M_TANK_STOP;	// No longer stopped

  return;				// Go, mon, go.
}

void TZ$CONT (struct _ucb * ucb) {
  struct _tt_class * R4; // check
  // was:	MOVL	R4,-(SP)
  struct _tty_ucb * tty;
  tty=ucb;
  R4=tty->ucb$l_tt_class;
  R4->class_fork();
  // was:	MOVL	(SP)+,R4
  return;
}

void TZ$FORK(struct _ucb * ucb) {
  struct _tz_ucb * tz;
  tz=ucb;
  ucb = tz->ucb$l_tz_xucb;		// Switch to PZ UCB
  if (ucb==0) return;			// PZ is disconnected: skip

  FORKLOCK();	// Take out PZ device FORK LOCK
#if 0
  LOCK=ucb$b_flck(ucb), -	;
  SAVIPL=-(SP),	-	;
  PRESERVE=NO
#endif

    if (UCB$M_BSY&		// If the device isn't busy,
	ucb->ucb$l_sts)	// then dont do i/o
      // Get IRP address
      ioc$initiate(ucb->ucb$l_irp, ucb);		// IOC$INITIATE needs IRP addr

  FORKUNLOCK();	// Release PZ device FORK LOCK
#if 0
  LOCK=ucb$b_flck(ucb), -	;
  NEWIPL=(SP)+,	-	;
  PRESERVE=NO,	-	;
  CONDITION=RESTORE	;
#endif

  return;
}

//
// Stop any currently active output
//
void TZ$STOP (struct _tty_ucb * ucb) {
  ucb->ucb$w_tt_hold|=TTY$M_TANK_STOP;	// Make PZ_OUT_LOOP stop
  return;
}

//
// Abort any port currently active
//
void TZ$ABORT(struct _tty_ucb * ucb) {
  ucb->ucb$w_tt_hold&=~TTY$M_TANK_BURST;	// reset burst active
 l10:
  return;
}
 
#if 0
TZ_END:				// End of driver
 
	.END
#endif

	static int dummy3;
struct _tt_port port_vector = {
  //
  // Added port vector table using VEC macros 
  //
  //	    $VECINI	TZ:TZ$NULL
  port_startio:TZ$STARTIO,
  port_disconnect:TZ$DISCONNECT,
  port_set_line:TZ$SET_LINE,
  port_ds_set:TZ$NULL,
  port_xon:TZ$XON,
  port_xoff:TZ$XOFF,
  port_stop:TZ$STOP,
  port_stop2:TZ$NULL,
  port_abort:TZ$ABORT,
  port_resume:TZ$RESUME,
  port_set_modem:TZ$NULL,
  port_glyphload:TZ$NULL,
  port_maint:TZ$NULL,
  port_forkret:TZ$FORK,
  port_start_read:TZ$NULL,
  port_middle_read:TZ$NULL,
  port_end_read:TZ$NULL,
  port_cancel:TZ$NULL,
};
 
#include<crbdef.h>
#include<cdtdef.h>
#include<dcdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<fdtdef.h>
#include<pdtdef.h>
#include<idbdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ddbdef.h>
#include<ipldef.h>
#include<dyndef.h>
#include<ssdef.h>
#include<iodef.h>
#include<devdef.h>
#include<system_data_cells.h>
#include<ipl.h>
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>
#include<ftucbdef.h>
#include<ftrddef.h>

#include<linux/blkdev.h>

static struct _irp * globali;
static struct _ucb * globalu;

static void  startio3 (struct _irp * i, struct _ucb * u) { 
  ioc$reqcom(SS$_NORMAL,0,u);
  return;
};

static void  startio2 (struct _irp * i, struct _ucb * u) { 
  u->ucb$l_fpc=startio3;
  exe$iofork(i,u);
  return;
}

static void ubd_intr2(int irq, void *dev, struct pt_regs *unused)
{
  struct _irp * i;
  struct _ucb * u;
  void (*func)();

  if (intr_blocked(20))
    return;
  regtrap(REG_INTR,20);
  setipl(20);
  /* have to do this until we get things more in order */
  i=globali;
  u=globalu;

  func=u->ucb$l_fpc;
  func(i,u);
  myrei();
}

static struct _fdt tz$fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
//int tz$startio (int a,int b) { };
static void  unsolint (void) { };
static void  cancel (void) { };
static void  ioc_std$cancelio (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
int  tz$wrtstartio (int a,int b) { };
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

static struct _ddt tz$ddt = { // Dummy TZ port Driver Dispatch table
  ddt$l_start: 0,
  ddt$l_fdt: 0,
};

int tz$fdtread(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int tz$fdtwrite(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void tz$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ucb->ucb$b_flck=IPL$_IOLOCK8;
  ucb->ucb$b_dipl=IPL$_IOLOCK8;

  ucb->ucb$l_devchar = DEV$M_REC | DEV$M_IDV | DEV$M_ODV | DEV$M_TRM | DEV$M_CCL;

  ucb->ucb$l_devchar2 = DEV$M_RTT | DEV$M_NNM;
  ucb->ucb$b_devclass = DC$_TERM;

  struct _tty_ucb * tty=ucb;

  tty->ucb$b_tt_detype=TT$_UNKNOWN;	// TYPE
  tty->ucb$w_tt_desize=TTY$GW_DEFBUF;	// BUFFER SIZE
  tty->ucb$l_tt_dechar=TTY$GL_DEFCHAR;	// DEFAULT CHARACTERS
  tty->ucb$l_tt_decha1=TTY$GL_DEFCHAR2; // DEFAULT CHARACTERS
  tty->ucb$w_tt_despee=TTY$GB_DEFSPEED<<8; // DEFAULT SPEED
  tty->ucb$w_tt_despee|=TTY$GB_RSPEED; //DEFAULT SPEED
  tty->ucb$b_tt_depari=TTY$GB_PARITY;	// DEFAULT PARITY
  tty->ucb$b_tt_parity=TTY$GB_PARITY;	// DEFAULT PARITY

  ucb->ucb$b_devtype = DT$_TTYUNKN; // TYPE
  ucb->ucb$w_devbufsiz = TTY$GW_DEFBUF; // BUFFER SIZE
  ucb->ucb$l_devdepend = TTY$GL_DEFCHAR; // DEFAULT CHARACTERS

  ucb->ucb$l_tt_devdp1=TTY$GL_DEFCHAR2; // Default Characters
  tty->ucb$w_tt_speed=TTY$GB_DEFSPEED<<8;	// DEFAULT SPEED
  tty->ucb$w_tt_speed|=TTY$GB_RSPEED;	// DEFAULT SPEED
  ucb->ucb$b_dipl=8;			// DEV IPL (no device)
  tty->ucb$l_tt_wflink=0;	// Zero write queue.
  tty->ucb$l_tt_wblink=0;	// Zero write queue.
  tty->ucb$l_tt_rtimou=0;	// Zero read timed out disp.

  // dropped the mutex stuff

  return;
}

void tz$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ddb->ddb$ps_ddt=&tz$ddt;
  //dpt_store_isr(crb,nl_isr);
  return;
}

int tz$unit_init (struct _idb * idb, struct _ucb * ucb) {
  ucb->ucb$v_online = 0;
  //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

  // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
  // no adp or cram stuff

  // or ints etc
  
  ucb->ucb$v_online = 1;

  TZ$INITLINE(0, ucb); // temp  placed

  return SS$_NORMAL;
}

struct _dpt tz$dpt;
struct _ddb tz$ddb;
struct _ucb tz$ucb;
struct _crb tz$crb;

int tz$init_tables() {
  ini_dpt_name(&tz$dpt, "TZDRIVER");
  ini_dpt_adapt(&tz$dpt, 0);
  ini_dpt_defunits(&tz$dpt, 1);
  ini_dpt_ucbsize(&tz$dpt,sizeof(struct _tz_ucb));
  ini_dpt_struc_init(&tz$dpt, tz$struc_init);
  ini_dpt_struc_reinit(&tz$dpt, tz$struc_reinit);
  ini_dpt_ucb_crams(&tz$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_flags(&tz$dpt, DPT$M_NOUNLOAD);
  ini_dpt_adptype(&tz$dpt, 0);
  ini_dpt_vector(&tz$dpt, port_vector);
  ini_dpt_end(&tz$dpt);

  ini_ddt_ctrlinit(&tz$ddt, TZ$INITIAL);
  ini_ddt_unitinit(&tz$ddt, TZ$INITLINE);
  // check. should be 0?  ini_ddt_start(&tz$ddt, TZ$STARTIO);
  ini_ddt_cancel(&tz$ddt, ioc_std$cancelio);
  ini_ddt_end(&tz$ddt);

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&tz$fdt,IO$_READLBLK,tz$fdtread,1);
  ini_fdt_act(&tz$fdt,IO$_READPBLK,tz$fdtread,1);
  ini_fdt_act(&tz$fdt,IO$_READVBLK,tz$fdtread,1);
  ini_fdt_act(&tz$fdt,IO$_WRITELBLK,tz$fdtwrite,1);
  ini_fdt_act(&tz$fdt,IO$_WRITEPBLK,tz$fdtwrite,1);
  ini_fdt_act(&tz$fdt,IO$_WRITEVBLK,tz$fdtwrite,1);
  ini_fdt_end(&tz$fdt);

  return SS$_NORMAL;
}

int tz_iodb_vmsinit(void) {
#if 0
  struct _ucb * ucb=&tz$ucb;
  struct _ddb * ddb=&tz$ddb;
  struct _crb * crb=&tz$crb;
#endif 
  struct _ucb * ucb=kmalloc(sizeof(struct _tz_ucb),GFP_KERNEL);
  struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
  struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
  unsigned long idb=0,orb=0;

  bzero(ucb,sizeof(struct _tz_ucb));
  bzero(ddb,sizeof(struct _ddb));
  bzero(crb,sizeof(struct _crb));

#if 0
  init_ddb(&tz$ddb,&tz$ddt,&tz$ucb,"dqa");
  init_ucb(&tz$ucb, &tz$ddb, &tz$ddt, &tz$crb);
  init_crb(&tz$crb);
#endif

  init_ddb(ddb,&tz$ddt,ucb,"tza");
  init_ucb(ucb, ddb, &tz$ddt, crb);
  init_crb(crb);

  ucb -> ucb$w_size = sizeof(struct _tz_ucb); // temp placed

  ucb -> ucb$w_unit_seed = 1; // was: 0 // check // temp placed
  ucb -> ucb$w_unit = 0; // temp placed

  ucb -> ucb$l_sts |= UCB$M_TEMPLATE; // temp placed

//  ioc_std$clone_ucb(&tz$ucb,&ucb);
  tz$init_tables();
  tz$struc_init (crb, ddb, idb, orb, ucb);
  tz$struc_reinit (crb, ddb, idb, orb, ucb);
  tz$unit_init (idb, ucb);

  insertdevlist(ddb);

  return ddb;

}

int tz_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc) {
  unsigned short int chan;
  struct _ucb * newucb = 0;
  // ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&tz$ucb*/,&newucb); // check. skip?
  exe$assign(dsc,&chan,0,0,0);
  registerdevchan(MKDEV(TTYAUX_MAJOR,unitno),chan);


  return newucb;
}

int tz_vmsinit(void) {
  //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

  unsigned short chan0, chan1, chan2;
  $DESCRIPTOR(dsc,"opa0");
  unsigned long idb=0,orb=0;
  struct _ccb * ccb;
  struct _ucb * newucb0,*newucb1,*newucb2;
  struct _ddb * ddb;

  printk(KERN_INFO "dev con here pre\n");

  ddb=tz_iodb_vmsinit();

  /* for the fdt init part */
  /* a lot of these? */

  tz_iodbunit_vmsinit(ddb,1,&dsc);

  printk(KERN_INFO "dev con here\n");

  TZ$INITIAL(&tz$crb); // check
  // return chan0;

}

int tz$fdtread(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  printk("should not be in tz$read\n");
}

int tz$fdtwrite(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  printk("should not be in tz$write\n");
}

