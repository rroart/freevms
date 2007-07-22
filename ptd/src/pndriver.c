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
//;;	.TITLE	PZDRIVER - Pseudo terminal driver interface
#if 0
	.TITLE	PNDRIVER - Pseudo terminal driver interface
	.IDENT 'V05-038'
#endif
//
// Comment out the following line if NOT building for VMS V4 (in other
// words comment out if you want a V5 driver).
//
//;;	VMS_V4 = 1
 
//++
// FACILITY:
//
//	VAX/VMS Pseudo Terminal Driver interface
//
// ABSTRACT:
//
//	The pseudo terminal consists of two devices.
//	This is the non terminal part of the two devices.
//	(In other words, it's the control side of the device
//	 which is used by the initiating device to communicate
//	 with the process that was created by LOGINOUT...)
//
// AUTHOR:
//
//	19-Nov-1982	Dale Moore	Redid the TW driver for VMS 3.0
//
//	This program has been granted to the public domain by the author.
//
// Revision History:
//
//	Version 'V03-001'
//		DWM	- Added Page seperators
//			- On Last cancel, invoke hangup on TW device
//			- changed PZ_STOP and PZ_STOP2 to return instead
//			  of looping for more.
//			- Changed last cancel to call ioc$reqcom instead of
//			  using macro REQCOM which is a branch ioc$reqcom.
//	Version V03-002	- Changed to Clear word rather than clear byte
//			  in startio routine on word field.
//
//	Version V03-003 (Thu Dec  9 12:42:38 1982) D. Kashtan
//			  Made into a TEMPLATE driver.
//	Version V03-004 (Fri Dec 10 11:40:35 1982) D. Kashtan
//			  Made EXE$... into +EXE$... in FDT dispatch table,
//			  fixing bug that crashed system in SET/SENSE MODE/CHAR
//	Version V03-005	(14-Jun-1983) Dale Moore
//			  Add R4 to calls to IOC$INITIATE.
//			  TTY$STARTIO mucks R4
//	Version V03-006	(12-Jul-1983) Mark London, MIT Plasma Fusion Center
//			- Set terminal to NOBROADCAST when no READ QIO avail-
//			  able so as to allow Broadcasts without hanging up.
//			  (When no QIO available, UCB$M_INT is enabled, and
//			  the Broadcast don't get handled.  The sender of a
//			  Broadcast goes into a wait state until the broadcast
//			  is completed or timed-out, neither or which can
//			  happen.  Setting NOBROADCASTs at least allow the
//			  Broadcast to finish. What is needed is a CTRLS state
//			  that doesn't allow Broadcasts to break through.)
//			- Added MOVC3 instruction for burst data in PN$STARTIO,
//			  which "should" speed up the transfers.
//			- Fixed data transfer problem by raising to fork IPL
//			  while calling PUTNXT in PN$FDTWRITE. NOTE: TWA0 must
//			  be a mailbox to avoid TT reads from timing out.
//
//	Version V04-001 - Doug Davis, Digital Equipment
//			- Most of the changes required for migration to
//			  Version 4.0 relate to the new handling of UCB
//			  creation and deletion. This includes adding 
//			  a CLONEDUCB entry point to the dispatch table,
//			  and "cloning" the UNITINIT routine to handle the
//			  required entry. Also changed the call from 
//			  IOC$CREATE_UCB to IOC$CLONE_UCB, with associated
//			  maintainence of the UCB$V_DELETEUCB bit in the
//			  UCB$L_STS field.
//			- Changes were also incorporated reflecting new
//			  methods of  XON/XOFF flow control.
//			- Although pieces of the original code have been
//			  superceded by these changes ( example - functions
//			  that were performed by Unit_Init for new units
//			  are are now performed by Clone_Init ), most of
//			  the original code was left in place and/or commented
//			  out.
//			
//			NOTE - No subroutines preambles were modified to
//			       reflect these changes.
//
//	Version V04-002	(20-Jan-1985) Mark London, MIT Plasma Fusion Center
//			- Changed test for output characters after call to
//			  UCB$L_TT_PUTNXT and UCB$L_TT_GETNXT.  Output is
//			  indicated in UCB$B_TT_OUTYPE.
//
//	Version V04-003 (24-Jun-1985) Kevin Carosso, Hughes Aircraft Co., S&CG
//			Cleaned this thing up quite a bit.
//			- Got rid of MBX characteristic on the devices.  This
//			  was a holdover to before cloned devices really
//			  existed.
//			- Got rid of the UNIT_INIT routine completely.  This
//			  was replaced by a CLONE_UCB routine.
//			- Leave the PZ template device OFFLINE.  This is what
//			  other TEMPLATE devices do, to indicate that you
//			  really cannot do I/O to the template.
//			- Rewrote the CANCEL_IO routine to issue a DISCONNECT
//			  on the TW device at last deassign of the PZ device.
//			  This causes the TW device to hangup on it's process.
//			  Works quite nicely with VMS V4 connect/disconnect
//			  mechanism.  Also, the devices should never stay
//			  around after last deassign on the PZ, if you want to
//			  reconnect, count on VMS connect/disconnect instead,
//			  it's much less of a security hole.
//			- Got rid of all modem operations.  Improper use tended
//			  to crash the system and they are not necessary.  TW
//			  device is always NOMODEM.  HANGUP works as you want
//			  it to without the modem stuff.
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
//			DEC (bless their little hearts) invented the *#&#$&
//			TU81 and use PTA0: now.
//
//	Version V04-005 (3-Sep-1986) Kevin Carosso, Hughes Aircraft Co., S&CG
//			Fixed bug whereby the sequence ^S followed by ^Y
//			would cause a system hang.  The fix is really in
//			TWDRIVER.
//
//	Version V04-006 (5-Dec-1986) Kevin Carosso, Hughes Aircraft Co., S&CG
//			Fixed the infamous character munging bug.  Turns
//			out that in FDTWRITE we were enabling interrupts
//			after doing the PUTNXT and before checking for
//			a char.  Now check for the character and then
//			ENBINT after we've decided what to do.  I assume
//			UCB$B_TT_OUTYPE field was getting corrupted.
//
//	Version V04-007 (10-JUL-1987) Kevin Carosso, Hughes Aircraft Co., S&CG
//			Fix in TWDRIVER for timeouts.  Don't bother
//			clearing the TIM bit all the time in here now.
//
//			Also, while we're in here, lets make the device
//			acquire "NODE$" prefixes, since mailboxes do.
//
//	Version V04-008	(2-NOV-1987) Kevin Carosso, Network Research Co.
//			In PN$FDTWRITE we were overwriting the status in
//			R0 just before jumping to EXE$ABORTIO when something
//			goes wrong.  Don't POPR into R0, but into R1.  We
//			only care about what we POPR into R3 anyway.
//
//			Fix thanks to Gerard K Newman @ San Diego
//			Supercomputer Center.
//
//	Version V05-001 Digital Equipment Corp.
//			Add support for Symmetric Multiprocessing. 
//
//	Version	V05-002	Digital Equipment Corp.
//			Modify PN$CLONE_INIT to make newly cloned 
//			owned by person requesting new device.
//
//	Version	V05-003	Digital Equipment Corp.
//			Modify PN$CLONE_INIT to change device protection
//			to be S:RWLP,O:RWLP.
//
//	Version	V05-004	Digital Equipment Corp
//			Change TP driver names to TW. (TP driver it turns
//			out, conflicted with the VAX/PSI terminal driver.)
//			TWDRIVER and PZDRIVER are now registered with SQM 
//			so there will be no more conflicts.
//
//	Version	V05-005 Digital Equipment Corp
//			If input stopped then exit PN$FDTWRITE immediately 
//			with reason of SS$_DATAOVERUN.  If we detect input
//			stopping while inserting data return with 
//			SS$_DATAOVERUN and number of bytes inserted.
//
//	Version V05-006	Digital Equipment Corp.
//			Add CODE for sending an AST if PORT XON, PORT XOFF,
//			and PORT SET_LINE routines are called.  Also add 
//			SENSEMODE, and SENSECHAR routine for reading TW device
//			characteristics.
//
//	Version V05-006A Kevin Carosso @ Network Research Co.
//			Conditionalize assembly for VMS V4 or V5.  There
//			is a magic symbol at the top that is commented
//			out for VMS V5.
//
//	Version V05-007 Dale Moore	Carnegie Mellon and
//			Mark Shannon	Carnegie Mellon
//			Changed buffer size from 32 to 256. Changed
//			Instructions that worked with Buffer size from
//			BYTE instructions to WORD instructions.
//			May result in more memory use, but better handle
//			large I/O's.
//
//	Version V05-008 Dale Moore	Carnegie Mellon CS
//			Found place where BISW 	TTY$M_TANK_BURST
//			was suppose to be BISW	#TTY$M_TANK_BURST
//			Thanks to the Gentlemen of Gandalf
//
//	Version V05-009	Dale Moore	Carnegie Mellon
//			Changed to deal with data length as a long instead
//			of word.  Some other stuff cleaned from executables
//			of DEC Windows version.
//
//	Version V05-011 Dale Moore	Carnegie Mellon
//			Updated to conform with what DEC is distributing
//			With DECWindows.  Damn Version Skew.
//
//	Version V05-030	Bruce R. Miller		CMU NetDev
//			Added Bill Glass's fixes to PN$FDTWRITE.  Was
//			returning bad "number of bytes transfered" on
//			request for 0 bytes.  Code also now tests the
//			tank_prempt bit.  New conditions for returning
//			DATAOVERUN.
//			
//	Version V05-031	Bruce R. Miller		CMU NetDev
//			Added SETMODE sub-function to set the ACCPORNAM
//			so that we show up in "SH US/FULL".  Then I
//			commented it out, 'cause I didn't trust it.
//
//	Version V05-032	Bruce R. Miller		CMU NetDev
//			Changed name to PZDriver.  To hell with Digital.
//			I'm sick of worrying about DECWindows everytime
//			I make a change to this code.
//
//	Version V05-033 John M. Clement		Rice University
//			Added code to make Vanilla IO$_SETMODE work
//			Prior to this it returned an error code.
//			It has been tested, but some bits may need
//			to be masked off as they should not be set
//			It relies on the good sense of the caller!!!
//
//	Version V05-034 Henry W. Miller		USBR
//			Changed buffer size to 1024 to handle cut & paste
//			blasts.
//
//	Version V05-035 27-Nov-1991	Henry W. Miller		USBR
//			Merge PZ and TZ UCB extensions together.
//			Make sure UCB$x_STS is referenced properly based
//			upon version of VMS.
//			Use UCB$K_TP_LENGTH instead of UCB$K_TT_LENGTH,
//			They are defined differently in different places.
//
//	Version V05-036 04-Dec-1991	Henry W. Miller		USBR
//			Make driver unloadable.
//
//	Version V05-037 20-Dec-1991	Henry W. Miller		USBR
//			Interlock code while in SET_ACC_POR.
//
//	Version V05-038 27-Jan-1992	John Clement		Rice Univ.
//			Change name to PNDRIVER
//
//--

#if 0
	.PAGE
	.SBTTL	Declarations
 
	.LIBRARY	/SYS$LIBRARY:LIB.MLB/
#endif
//
// External Definitions:
//
#if 0
.NOCROSS
#endif
//	$ACBDEF				// Define ACB
#include <bufiodef.h>
#include <crbdef.h>				// Define CRB
#if 0
#include <candef.h>				// Define cancel codes
#endif
#include <dcdef.h>
#include <ddbdef.h>				// DEFINE DDB
#include <ddtdef.h>				// DEFINE DDT
#include <devdef.h>				// DEVICE CHARACTERISTICS
#include <dyndef.h>				// Dynamic structure definitions
#include <iodef.h>				// I/O Function Codes
#include <irpdef.h>				// IRP definitions
#include <jibdef.h>				// Define JIB offsets
#include <orbdef.h>				// Define ORB offsets
#include <pcbdef.h>				// Define PCB
#include <prdef.h>				// Define PR
#include <ttyucbdef.h>			// Define the TTY fields in the UCB

#include <splcoddef.h>			// Spin lock code definitions

#include <ssdef.h>				// DEFINE System Status
#include <ttdef.h>				// DEFINE TERMINAL TYPES
#include <tt2def.h>				// Define Extended Characteristics
#include <ttydef.h>				// DEFINE TERMINAL DRIVER SYMBOLS
#include <ttyvecdef.h>
#if 0
#include <ttydefs.h>			// DEFINE TERMINAL DRIVER SYMBOLS
#include <ttymacs.h>			// DEFINE TERMINAL DRIVER MACROS
#endif
#include <ucbdef.h>				// DEFINE UCB
#include <vecdef.h>				// DEFINE VECTOR FOR CRB
#if 0
.CROSS
#endif 

#include <system_data_cells.h>
#include <com_routines.h>
#include <ioc_routines.h>
#include <misc_routines.h>
#include <exe_routines.h>
#include <linux/sched.h>
#include <internals.h>

#define DEVICELOCK vmslock
#define DEVICEUNLOCK vmsunlock

//
// Local definitions
//
// QIO Argument list offsets
//
	static int P1 = 0;
static int P2 = 4;
static int P3 = 8;
static int P4 = 12;
static int P5 = 16;
static int P6 = 20;
//
// New device class for control end
//
static int DC$_PZ = 0xFF;
static int DT$_PZ = 0;

//
// Definitions that follow the standard UCB fields for TZ driver
//  This will all probably have to be the same as the standard term
 
struct _tz_ucb {			// Start of UCB definitions
 
  struct _ucb tz$ucb;		// Position at end of UCB
 
  long ucb$l_tz_xucb;	// UCB of corresponding
					//  control/application unit
					// call 
  long ucb$l_tz_xon_ast;	// AST list for XON event notification
  long ucb$l_tz_xoff_ast;	// AST list for XOFF event notification
  long ucb$l_tz_set_ast;	// AST list for notification of SET_LINE 
#define UCB$K_TZ_LEN    (sizeof(struct _ucb)+16);			// Size of UCB
 
//;;	$DEFEND	UCB			// End of UCB definitions
 
//
// Definitions that follow the standard UCB fields in PZ devices
// 

#define BUFFER_SIZE 1024

//
// Length of access port buffer in UCB
//
#define ACCPOR_SIZE 30
 
//;;	$DEFINI UCB			// Start of UCB definitions
//;;	.=UCB$K_LENGTH			// position at end of UCB
//;;	.=UCB$K_TZ_LEN			// position at end of UCB
  long ucb$l_pz_xucb;		// UCB of terminal part of pseudo terminal
  char ucb$t_pz_buffer[BUFFER_SIZE]; // Buffer to store characters to be transmitted

  char ucb$a_rem_host_len;	// Size of remote host ID string.
  char ucb$a_rem_host_str[ACCPOR_SIZE];  // Space for remote host ID string.

#define UCB$K_PZ_LEN (sizeof(struct _tz_ucb))			// Size of UCB

};			// end of UCB definitions
#if 0

	.PAGE
#endif
//
// LOCAL Storage
//
#if 0
	.PSECT $$$105_PROLOGUE
 
	.SBTTL	Standard Tables
#endif
//
// Driver prologue table:
//
#if 0
PN$DPT::
	DPTAB	-			// Driver Prologue table
		END = PN$END,-		// End and offset to INIT's vectors
		UCBSIZE = UCB$K_PZ_LEN,-; Size of UCB
//;;		FLAGS=DPT$M_NOUNLOAD,-		// Don't allow unload
		ADAPTER=NULL,-			// ADAPTER TYPE
		UNLOAD=PZ_UNLOAD,-		// Unload the PZ driver
		NAME	= PNDRIVER		// Name of driver
//;;		NAME	= PZDRIVER		// Name of driver
	DPT_STORE INIT
	DPT_STORE UCB,UCB$W_UNIT_SEED,W,0	// SET UNIT # SEED TO ZERO

#if	defined	VMS_V4
	DPT_STORE UCB,UCB$B_FIPL,B,8 		// FORK IPL
#else
	DPT_STORE UCB,UCB$B_FLCK,B,SPL$C_IOLOCK8 ; FORK LOCK
#endif

#if	defined	VMS_V4
	DPT_STORE UCB,ucb$w_sts,W,-		// TEMPLATE device
			<UCB$M_TEMPLATE>
#else
	DPT_STORE UCB,UCB$L_STS,L,-		// TEMPLATE device
			<UCB$M_TEMPLATE>
#endif
	DPT_STORE UCB,UCB$L_DEVCHAR,L,<-	// Characteristics
			DEV$M_REC!-		//   record oriented
			DEV$M_AVL!-		//   available
			DEV$M_IDV!-		//   input device
			DEV$M_ODV>		//   output device
	DPT_STORE UCB,UCB$L_DEVCHAR2,L, -		// Device characteristics
			<DEV$M_NNM>			// prefix with "NODE$"
	DPT_STORE UCB,UCB$B_DEVCLASS,B,DC$_PZ
	DPT_STORE UCB,UCB$B_DIPL,B,8		// Device IPL = FIPL (no device)
	DPT_STORE DDB,DDB$L_DDT,D,PN$DDT
 
	DPT_STORE REINIT
	DPT_STORE CRB,CRB$L_INTD+VEC$L_INITIAL,D,PN$INITIAL	// Controller
	DPT_STORE END

	.PAGE
	.SBTTL Driver Dispatch table and function decision table
//
// Driver Dispatch table
//
//;;	DDTAB	DEVNAM	= PZ,-			// Device name
	DDTAB	DEVNAM	= PN,-			// Device name
		START	= PN$STARTIO,-		// Start I/O routine
		FUNCTB	= PN$FUNCTAB,-		// The function table
		CANCEL	= PN$CANCEL,-		// the cancel i/o routine
		CLONEDUCB = PN$CLONE_INIT	// Entry when template cloned.
//
// Function Decision table for PZ devices
//
PN$FUNCTAB:
	FUNCTAB	,-			// Legal Functions
		<READLBLK,-
		WRITELBLK,-
		READVBLK,-
		WRITEVBLK,-
		READPBLK,-
		WRITEPBLK,-
		SETMODE,-
		SETCHAR,-
		SENSEMODE,-
		SENSECHAR,-
		>
	FUNCTAB	,-			// Buffered I/O functions
		<READLBLK,-
		WRITELBLK,-
		READVBLK,-
		WRITEVBLK,-
		READPBLK,-
		WRITEPBLK,-
		>
	FUNCTAB	PN$FDTREAD,<READLBLK,READVBLK,READPBLK>
	FUNCTAB PN$FDTWRITE,<WRITELBLK,WRITEVBLK,WRITEPBLK>
	FUNCTAB PN$FDTSET,<SETMODE,SETCHAR>
	FUNCTAB PN$FDTSENSEM,<SENSEMODE>
	FUNCTAB	PN$FDTSENSEC,<SENSECHAR>


	.SBTTL	Local Storage - Name of companion device
#endif
 
	char * TZSTRING="tza"; // ascii,  yes // was: TZA, case prob
#define TZLENGTH 3


//++
// PZ_UNLOAD, Unload the driver
//
// Functional description:
//
//	Usefull for dealocating system buffers with COM$DRVDEALMEM.
//
//	N.B.  See the VMS Device Support manual section on the
//	Driver Unloading Routine before modifying this routine.
//
// Context:
//
//	SYSGEN calls the driver unloading routine at IPL$_POWER.
//	The driver unloading routine cannot lower IPL.
//	The driver unloading routine is called in process context.
//	The driver unloading routine can use all registers.
//
// Inputs:
//
//	R6	- address of the DDB (device data block)
//	R10	- address of the DPT (driver prologue table)
//
// Outputs:
//
//	The driver unloading routine can use all registers.
//
//--

int PZ_UNLOAD()				// Unload device
{
  return SS$_NORMAL;				// Return
}


#if 0
	.PAGE
	.SBTTL	PN$FDTREAD - Function decision routine for PZ control read
#endif
//++
// PN$FDTREAD
//
// Functional Description:
//
//	This routine is called from the function decision table dispatcher
// 	to process a read physical, read logical, read virtual I/O function.
//
//	The function first verifies the caller's parameters, terminating
//	the request with immediate success or error if necessary.
//	A system buffer is allocated and its
//	address is saved in the IRP.  The caller's quota is updated, and
//	the read request is queued to the driver for startup.
//
// Inputs:
//
//	R0,R1,R2	= Scratch
//	R3		= IRP Address
//	R4		= Address of PCB for current process
//	R5		= Device UCB address
//	R6		= Address of CCB
//	R7		= I/O function code
//	R8		= FDT Dispatch addr
//	R9,R10,R11	= Scratch
//	AP		= Address of function parameter list
//			P1(AP) = Buffer Address
//			P2(AP) = Buffer Size
//
// Outputs:
//
//	R0,R1,R2,R11	= Destroyed
//	R3-R10,AP	= Preserved (pickled)
//	IRP$L_SVAPTE(R3)= Address of allocated system buffer
//	IRP$W_BOFF(R3)	= Requested byte count
//
//	System Buffer:
//		LONGWORD/0	= Address of start of data= buff+12
//		LONGWORD/1	= Address of user buffer
//
//--
	static int dummy1;
int PN$FDTREAD(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int sts;
  long *buf,size;
  size=i->irp$l_qio_p2;	// Get buffer Size
  if (size==0)
    //
    // Did he request a read of zero bytes?
    // Is the size zero? If so, go do it easy.
    //
    // Everything is ok
    return exe$finishioc(SS$_NORMAL,i,p,u);		// complete I/O request
  buf = i->irp$l_qio_p1;		// Get buffer Address
  exe_std$readchk(i,p,u,buf,size); // check buf	// Do we have access to the buffer
#ifdef __i386__
  size+=12;		// Add 12 bytes for buffer header
#else
  size+=24;		// Add 12 bytes for buffer header
#endif

  sts=exe_std$debit_bytcnt_alo(size, p, 0, &buf); 	// Verify enough byte quota allocate buffer
  // and charge process for useage

  if (!BLISSIF(sts))			// If error report it
    return	exe_std$abortio(i,p,u,sts);		// complete I/O request

  i->irp$l_svapte = buf;		// Save address of buffer
  i->irp$l_boff = size;		//  and requested byte count
  size=size; //check			// convert to longword count

  struct _bufio * bd=buf;
  bd->bufio$b_type=DYN$C_BUFIO;
  bd->bufio$w_size=size;
#ifdef __i386__
  bd->bufio$ps_pktdata=(long)buf+12;		// Save addr of start of user data
#else
  bd->bufio$ps_pktdata=(long)buf+24;		// Save addr of start of user data
#endif
  bd->bufio$ps_uva32=i->irp$l_qio_p1;		// Save user buffer address in 2nd
					// longword
  return	exe$qiodrvpkt(i, p, u);	// Queue I/O packet to start I/O routine
}

#if 0
.PAGE
.SBTTL	PN$FDTWRITE - Function decision routine for PZ control write
#endif
//++
// PN$FDTWRITE
//
// Functional Description:
//
//	This routine is called from the function decision table dispatcher
// 	to process a write physical, write logical, write virtual I/O
//	function.
//
//	The function first verifies the caller's parameters, terminating
//	the request with immediate success or error if necessary.
//	The routine then immediately start cramming the characters into
//	the associated units typeahead buffer by calling putnxtchr.
//
// Inputs:
//
//	R0,R1,R2	= Scratch
//	R3		= IRP Address
//	R4		= Address of PCB for current process
//	R5		= Device UCB address
//	R6		= Address of CCB
//	R7		= I/O function code
//	R8		= FDT Dispatch addr
//	R9,R10,R11	= Scratch
//	AP		= Address of function parameter list
//		P1(AP)	= Buffer Address
//		P2(AP)	= Buffer Size
//
// Outputs:
//
//	R0,R1,R2	= Destroyed
//	R3-R7,AP	= Preserved (pickled)
//
//
//	N O T E:
//		This routine now assumes that TZ and PZ's fork and DEVICE
//	locks are the same lock.  This allows use to keep from having to 
//	do an extra unecessary lock acquisition.
//	
//
//
// External Routines:
//
//	EXE$ABORTIO - FDT abort io routine
//	Input Parameters:
//		R0 - First longword of IOSB
//		R3 - IRP Address
//		R4 - PCB Address
//		R5 - UCB Address
//
//	EXE$FINISHIOC - FDT finish IO routine
//	Input Parameters:
//		R0 - First longword of IOSB
//		R3 - IRP Address
//		R4 - PCB Address
//		R5 - UCB Address
//
//	EXE$WRITECHK - Check access to buffer
//	Input Parameters:
//		R0 - Address of buffer
//		R1 - Size of buffer
//		R3 - IRP Address
//	Output Parameters:
//		R0,R1,R3 - Preserved
//		R2 - clear
//
//	@UCB$L_TT_PUTNXT(R5) - Port driver input character routine
//	Input Parameters:
//		R3 - character
//		R5 - UCB Address
//	Output Parameters:
//		R3 - if EQL then nothing
//		     if LSS then Burst address to output
//		     if GTR then char to output
//		R5 - UCB Address
//		R1,R2,R4 - trashed
//		R0 - Is this trashed or preserved? Documentation say preserve.
//
//--
int PN$FDTWRITE(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int inp_char;
  char *buf;
  int size=0,send_tot=0,send_chr=0;
  signed int send_num=0;
  struct _tpd_ucb * tpd;
  struct _tz_ucb * tz = u;
  struct _tz_ucb * pz;
  struct _tty_ucb * tty;
  send_chr=0;			// Clear count of characters sent
  size=i->irp$l_qio_p2;		// Get buffer Size
  if (size==0) 			// Is the non zero? If so, do it easy.
    goto	l160;			// Zero size buffer just finish it now

  buf	 = i->irp$l_qio_p1;		// Get buffer Address
  exe_std$writechk(i,p,u,buf,size);		// Do we have access to the buffer
  // No return means no access
  send_tot		 = size;		// Number of characters to send
  pz		 = tz;		// Save away PZ UCB ptr
  //
  //	User request ok.
  //
  //	buf	-> 	Address of characters to input
  //	tz	->	Address of TZ's UCB 
  //	send_num	->	Number of characters to send this time
  //	send_tot	->	Total number of characters to send
  //	send_chr	->	Numbers of characters already sent
  //	pz	->	Address of PZ's UCB
  //
 l20:
  send_num=	send_tot-send_chr;		// Get number of characters send
  if	(send_num<BUFFER_SIZE)		// More data than buffer can hold
    {			// GEQ then use BUFFER_SIZE segment
      if	(send_num<=1)			// Test for one byte only 
	{			// GTR more than one fill up buffer
	  inp_char=	buf[send_chr]; // check	// Move single character into inp_char
	} else { // send_num > 1
	  memcpy(pz->ucb$t_pz_buffer,buf+send_chr,send_num); // check 	// Store data in UCB buffer
	  buf=&pz->ucb$t_pz_buffer;	// Get buffer address
	  inp_char = *buf++;		// Get first character 
	}
    } else { // send_num>=BUFFER_SIZE
      memcpy(pz->ucb$t_pz_buffer,buf+send_chr,BUFFER_SIZE); // check // Store data in UCB buffer
#if BUFFER_SIZE < 255 // check 
      send_num=	BUFFER_SIZE;		// Number of characters in burst
#else
      send_num=	BUFFER_SIZE;		// Number of characters in burst
#endif
      buf=&pz->ucb$t_pz_buffer;	// Get buffer address
      inp_char = *buf++;		// Get first character 
    }

  tz = pz->ucb$l_pz_xucb;		// Get TZ's UCB address

  int savipl = DEVICELOCK(((struct _ucb *)tz)->ucb$l_dlck, ((struct _ucb *)tz)->ucb$b_dipl); // Take device lock for TZ device

#define	TTY$M_TP_XOFF 8
#define	TTY$V_TP_XOFF 3

  tpd=tz;
  if	(TTY$M_TP_XOFF&	// See if XOFFED, if so then
	 tpd->ucb$b_tp_stat) goto l120;	// don't even bother, cause it's full

 l70:
  send_chr++;			// Increment sent character count
  tty=tz;
  signed int CC;
  tty->ucb$l_tt_putnxt(&inp_char,&CC,tty);	// Buffer character
  if (CC<0) {	// check		// LSS burst 
    tty=tz;
    tty->ucb$w_tt_hold|=TTY$M_TANK_BURST;	// Signal burst
  }
  if (CC>0) {   // check 		// GTR single character
    tty=tz;
    tty->tty$b_tank_char = inp_char;		// Store character in tank
    tty->ucb$w_tt_hold|=TTY$M_TANK_HOLD;	// Signal character in tank
  }
  tpd=tz; if	(TTY$M_TP_XOFF&	// See if XOFFED is so then
			 tpd->ucb$b_tp_stat) goto l120;	// stop input and check for echoed data
  send_num--;			// Decrease number to send in burst
  if (send_num>0) {			// LEQ block done see if request done
    inp_char=*buf++;		// Get next character
    goto	l70;			// Send next character
  }

 //
 // See if this request is done or if more to do
 //
  if	(send_chr<send_tot)			// All done
   {			// GEQ done so check for echo

     DEVICEUNLOCK(((struct _ucb *)tz)->ucb$l_dlck, savipl);	// More in request release lock 
     // and go back and get it

     buf	 = i->irp$l_qio_p1;		// Restore users buffer address
     goto	l20;			// 
   }

 //
 // See if need to start up pending read
 //
 l120:
 tty=tz;
 if 	((TTY$M_TANK_PREMPT|	// If anything is in hold
	  TTY$M_TANK_HOLD|	//  start output
	  TTY$M_TANK_BURST)&	//
	 tty->ucb$w_tt_hold)	//
   {			// Otherwise, finish up

     if	(UCB$M_BSY &		// PZ ready to take data 
	 ((struct _ucb *)pz)->ucb$l_sts) {	//
 // Get IRP address
       tz		 = pz;		// Get PZ UCB address
       ioc$initiate(u->ucb$l_irp, u);		// Now start PZ read 
     }
   }

   DEVICEUNLOCK	(((struct _ucb *)pz)->ucb$l_dlck, savipl);	// Done release lock 
   // NOTE PZ & TZ lock are the same

 pz=tz=u;
 tpd=tz;
 tz=	pz->ucb$l_pz_xucb;
 if	(TTY$M_TP_XOFF&	// See if XOFFED report this special
	 tpd->ucb$b_tp_stat) goto l170;	// case
 //
 // Finish up the read
 //
 l150:
 l160:
 {}
 int R=	send_chr<<16;		// Move number of bytes INPUT
 R|=	SS$_NORMAL;		// Everything is just fine
 return exe$finishioc(R,i,p,u);		// Complete the I/O request

 //+
 // Special code to deal with input while xoffed.
 //-
#define	TTY$M_TP_XOFF 8
#define	TTY$V_TP_XOFF 3
#if 0
 ASSUME	TTY$M_TP_XOFF EQ TTY$M_TP_DLLOC+4
   ASSUME  TTY$V_TP_XOFF EQ TTY$V_TP_DLLOC+1
#endif

   l170:          
 if	(send_chr>=send_tot)			// All done?
   goto	l150;			// Yes, a normal termination
 R=	send_chr<<16;		// Move number of bytes INPUT
 R|=SS$_DATAOVERUN;	// Cannot input more data 
 return exe$finishioc(R,i,p,u);		// Complete the I/O request
}

#if 0
.PAGE
.SBTTL	PN$CANCEL - Cancel the IO on the PZ device
#endif
//++
//
// Functional Description:
//
//	This routine is entered to stop io on a PZ unit.  If this is the last
//	deassign on the PZ device, issue a CLASS_DISCONNECT on our associated
//	TZ device to get it away from any processes using it.
//
// Inputs:
//
//	R2 = Negative of the Channel Number,
//		also called channel index number
//	R3 = Current IO package address
//	R4 = PCB of canceling process
//	R5 = UCB Address
//	R8 = CAN$C_CANCEL on CANCEL IO or CAN$C_DASSGN on DEASSIGN
//
// Outputs:
//	Everything should be preserved
//--
int PN$CANCEL(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {					// Cancel PZ usage
  struct _ccb * R6=c;
  struct _acb ** acb_p;
  int R0,R1;
  struct _ucb * tz = u;
  struct _tz_ucb * real_tz;
  struct _tty_ucb * tty;
  signed int index=ctl$gl_ccbbase-c;
  int sts;
  ioc_std$cancelio(index,i,p,u);			// Call the cancel routine
  if	(UCB$M_CANCEL&tz->ucb$l_sts) {	// Branch if not for this guy
    R1=0;
    sts=	SS$_ABORT;			// Status is request canceled
    tz->ucb$l_sts&=~(UCB$M_BSY|UCB$M_CANCEL);			// Clear unit status flags
    ioc$reqcom(sts,R1,tz);			// Complete request
  }
  if	(tz->ucb$l_refc)			// Last Deassign
    return;				// No, just exit
  //
  // Last DEASSIGN we need to get rid of AST's 
  //
  R6			 = index;		// Save the Channel number
  real_tz=tz;
  tz=	real_tz->ucb$l_pz_xucb;		// Switch to TZ UCB
  if (tz==0) goto	l20;				// if not there, skip
  real_tz=tz;
  acb_p=&	real_tz->ucb$l_tz_xon_ast;		// Get XON list head address
  if	(*acb_p)			// Any ast to deliver
    // EQL 0 do not flush it
    com_std$flushattns(p,tz,index,acb_p);		// Flush it
  real_tz= tz; acb_p=&real_tz->ucb$l_tz_xoff_ast;	// Get XOFF list head address
  if	(*acb_p)			// Any ast to deliver
    // EQL 0 do not flush it
    com_std$flushattns(p,tz,index,acb_p);		// Flush it
  real_tz=tz; acb_p=&real_tz->ucb$l_tz_set_ast;		// Get SET_LINE list head address
  if	(*acb_p)			// Any ast to deliver
    // EQL 0 do not flush it
    com_std$flushattns(p,tz,index,acb_p);		// Flush it
  //
  // Do a DISCONNECT on the TZ device.
  //

  real_tz=tz; real_tz->ucb$l_tz_xucb=0;		// Clear backlink to PZ device
  tz->ucb$l_sts|=	UCB$M_DELETEUCB;  // Set it to go bye-bye
  tz->ucb$l_sts&=~UCB$M_ONLINE;	// Mark offline
  tz->ucb$l_sts&=~UCB$M_INT; 	// Don't expect interrupt
  tty=tz;
  R1=	tty->ucb$l_tt_logucb;		// Look at logical term UCB
  if	(((struct _ucb *)R1)->ucb$l_refc==0) {			// See if TZ has any references
    // If so, go and do disconnect
    ioc$delete_ucb();		// if not, delete the UCB
    goto	l20;
  }	
  R0=0;				// indicate that we must hangup
  tty=tz;
  R1=	tty->ucb$l_tt_class;
  ((struct _tt_class *)R1)->class_disconnect();		// Force disconnect
 l20:
  tz=u;
  real_tz=tz;
  real_tz->ucb$l_pz_xucb=0;		// Clear link to deleted TZ
  u->ucb$l_sts|=UCB$M_DELETEUCB;	// Set our own delete bit
  return;
}

#if 0
.PAGE
.SBTTL PN$INITIAL - Initialize Pseudo terminal interface
#endif
 
//++
// PN$INITIAL - Initialize the interface
//
// Functional Description:
//
//	This routine is entered at device connect time and power recovery.
//	There isn't much to do to the device.
//
// Inputs:
//
//	R4 = The devices CSR  (but there is no csr!)
//	R5 = address of IDB
//	R6 = address of DDB
//	R7 = address of CRB
//
// Outputs:
//
//	All registers preserved
//
//--
int PN$INITIAL(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  return;
}

#if 0
.PAGE
.SBTTL	PN$CLONE_INIT - initialize the unit
#endif
//++
// PN$CLONE_INIT - Initialize new PZ device
//
// Functional Description:
//
//	Main thing we do here is clone up an associated terminal device
//	and initialize fields in the two new UCB's.
//
// Inputs:
//
//	R5	= Address of UCB
//
// Outputs:
//
//	All preserved
//--
 
int PN$CLONE_INIT(struct _ucb * u) {
  struct _ddb * R8;
  struct _ddb * R0;
  struct _ucb * R1;
  struct _pcb * p;
  char * R7;
  struct _tpd_ucb * tpd;
  struct _tz_ucb * tz;
  struct _tty_ucb * tty;
  struct _ucb * search_ucb;
  struct _ddb * search_ddb;
  struct _ucb * new;

  //+ ---
  //	Ignore inits on UNIT #0 (the template PZ UCB)
  //- ---
  if	(u->ucb$w_unit==0)		//UNIT #0??
    return;					//Yes: Return
  //No: Initialize

  u->ucb$l_sts&=~UCB$M_DELETEUCB;	// Clear ucbdelete - dec

  //
  // Find the associated device.
  //
  // NOTE: We can't call IOC$SEARCHDEV because it expects the string to
  //	be accessible from the previous access mode. (It executes the
  //	prober instruction with mode=#0). I don't know how to make the
  //	string accessible from the previous access mode cleanly, but I
  //	do know how to move most of IOC$SEARCHDEV into the PZ driver.
  //
  // Get address of i/o database
  // listhead
  // Desired mate = PTY UNIT 0
  // String address for TZA
  // String length
  search_ddb=ioc$gl_devlist;
  int sts=SEARCHDEV(&search_ddb, TZSTRING, TZLENGTH);			// Find the DDB
  if (sts) return;				// Device not found
  sts=	SEARCHUNIT(search_ddb,0,&search_ucb);			// Search for specific unit
  if (sts==0) return;				// NOT FOUND: Return
	
  //
  // Create the PTY, R1 has template UCB of TZ device
  //

  R0	 = u->ucb$l_ddb;		// Find UNIT #0 UCB FOR PZ DEV.
  R0=R0->ddb$ps_ucb;
  // check. is R0 meaningless?
  // R5 = UCB to CLONE
  sts=ioc_std$clone_ucb(search_ucb,&new);			// Clone UCB
 
  R1			 = new;		// Put PTY UCB back into R1
  if (BLISSIFNOT(sts)) {				// WIN!!! (big deal.)
    //+ ---
    //	CREATE_UCB failed, mark our PZ device offline
    //- ---
    u->ucb$l_sts &= ~UCB$M_ONLINE;		// Mark offline
    return;
  //+ ---
  //	PTY UCB created successfully, link the UCBs together
  //- ---
  }
  tz=u; tz->ucb$l_pz_xucb	 = new;		// Store associated UCB
  tz=new; tz->ucb$l_tz_xucb	 = u;		// Store the other one back
  new->ucb$l_pid=0;			// Clear the owner PID in PTY
  new->ucb$l_refc=0;			// Reference count is ZERO
  new->ucb$l_sts&=~UCB$M_DELETEUCB;	// Inhibit deletion
  u->ucb$l_devdepend=new->ucb$w_unit;		// Set associated TZ unit
  // number in PZ devdepend
  //+ ---
  //	Call the PTY unit init routine
  //- ---
  R0	 = new->ucb$l_ddt;		// Get DDT
  R0	 = ((struct _ddt *)R0)->ddt$l_unitinit;		// Get Unit Init Addr in DDT
  if	(R0!=ioc$return)			// Null Address??
    goto	l50;				// No: Call it
#if 0
 // not needed anymore?
 R0	 = new->ucb$l_crb;		// Yes: Look in the CRB
 R0=CRB$L_INTD+VEC$L_UNITINIT(R0);
 if (R0==0)	l100;				// No: Unit init routine
#endif
 
 l50:
 {}
 // R5 = PTY UCB
 void (*fn)();
 fn=R0;
 // try another initline?
 extern int TZ$INITLINE();
 fn=TZ$INITLINE;
 fn(0,new);  // check				// CALL THE UNIT INIT ROUTINE
 p = ctl$gl_pcb;		// Use current PCB
#if 0
 // not yet
 struct _orb * orb	 = new->ucb$l_orb;		// Fetch ORB address
 orb->orb$l_owner = p->pcb$l_uic;		// Set device owner
 orb->orb$b_flags|=ORB$M_PROT_16;	// Indicate using SOGW device protection
 orb->orb$w_prot&=~0xFF;		// Make device S:O:RWLP
#endif

 return;
}

#if 0
.PAGE
.SBTTL	DDB finding Routines
#endif
//++
// SearchDev - Search for device DDB
//
// This routine is called to search the device database for a DDB.
// This is the first step in finding another devices UCB.
//
// This routine copied out of IOC$SEARCHDEV in IOSUBPAGD
//
// Inputs:
//
//	R8 = DDB Head
//	R7 = Address of String
//		String = ddc format: example = /TTA/
//	R4 = Length of string
//
// Outputs:
//
//	R8 = DDB of desired device if EQL, otherwise not found
//	R0 is trashed
//	R1 is trashed
//--
int SEARCHDEV(struct _ddb ** ddb,char * string, int strlen) {				// Search for device name
  char * locstr;
  char loclen;
 l10:	*ddb = (*ddb)->ddb$ps_link;		// Get address of next ddb
  if (*ddb==0) 			// If eql end of list
 				// indicate search failure
    return 1; // inverted error coding
  locstr=&(*ddb)->ddb$t_name;	// Get address of generic device name
  loclen=*locstr++;		// Calculate len of string to compare
  if 	(loclen!=strlen)			// Length of names match?
    goto	l10;			// If neq no
  if (strncmp(locstr,string,strlen))		// Compare device names
    goto	l10;			// If neq names do not match
  return 0;
}

#if 0 
.SBTTL	UCB finding routine
#endif
//++
// SEARCHUNIT - Subroutine to search for UCB given DDB
//
// Given the DDB of a device, get the UCB and run down the ucb list until
// we get the ucb with the desired unit number.  This code is taken out of
// IOC$SEARCHDEV in IOSUBPAGD.
//
// Inputs:
//
//	R8 = DDB of device
//	R6 = unit number of desired UCB
//
// Outputs:
//
//	R1 = UCB of device if NEQ, otherwise not found
//	R0 is trashed
//
//--
int SEARCHUNIT(struct _ddb * ddb, int unit, struct _ucb ** ucb) {				// Search for unit number
  int sts=0;
  struct _ucb dummy;
  int linkoffset=(long)&dummy.ucb$l_link-(long)&dummy;
  *ucb=	(long)&ddb->ddb$ps_ucb-linkoffset;	//check		// Get address of next ucb address
 l10:	*ucb = (*ucb)->ucb$l_link;		// Get Address of next ucb
  if ((*ucb)==0) goto	l40;			// If EQL then end of list
  if	(unit==(*ucb)->ucb$w_unit)	// Unit number match?
    goto	l30;			// If eql yes
  goto	l10;
 l30:	sts=SS$_NORMAL;		// Indicate match
 l40:	return sts;
}

#if 0
.PAGE
.SBTTL	PN$STARTIO - Device Startio routines
#endif
//++
// PN$STARTIO	- Start Input on idle device
//
// Functional Description:
//
//	If after the read FDT routines are done and nobody is doing
//	anything on the device (UCB$V_BSY = 0) then call the start io
//	routine.
//
// Called from:
//
//	Called from any one of five places:
//	- The EXE$QIODRVPKT in the PZ FDT READ routine
//		which calls EXE$INSIOQ which calls IOC$INITIATE
//	- The IOC$REQCOM at the end of this PZ startio routine
//		which calls IOC$INITIATE
//	- The TZ startio routine which calls IOC$INITIATE
//	- The PZ write fdt routine which calls IOC$INITIATE.
//		In case we must echo a character.
//	- The PN$RESUME routine which calls IOC$INITIATE.
//
// Inputs:
//
//	R3 = IRP Address
//	R5 = UCB Address
//		UCB$W_BCNT and UCB$L_SVAPTE are written by IOC$INITIATE
//
// Outputs:
//
//	R5 - UCB Address
//
//--

int PN$STARTIO (struct _irp * i, struct _ucb * u) {
  struct _irp * irp;
  struct _tpd_ucb * tpd;
  struct _tz_ucb * tz;
  struct _tty_ucb * tty;
  struct _ucb * ucb2;
  long chr;
  int len;
#if 0
  .ENABLE LSB
#endif 
     u->ucb$l_svapte=*(long *)u->ucb$l_svapte; // check		// Initialize buffer
  //  pointers
 PZ_OUT_LOOP:
  //
  // Here R5 must point at the PZ device UCB and not at
  //  the UCB of the associated TZ device.
  //
  if	(u->ucb$l_bcnt<=0)			// Any space left in rd packet
   goto	l50;				// No, Completed I/O
  //
  // Switch to terminal UCB
  //
  tz=u;
  u	 = tz->ucb$l_pz_xucb;		// Set to TZ ucb

  //
  // Look for next output in state tank
  //
  // Change Case statement to reflect V4 changes in routines - DEC 
  //
 tty=u; chr=ffs(*(char*)(((long)&tty->ucb$w_tt_hold+1))); // check
 switch 	((long)chr) {			// Dispatch
 case 1: goto	PZ_PREMPT;			// Send Prempt Characte - DEC 
 case 2: goto PZ_STOP;		// Stop output
 case 3: goto PZ_CHAR;			// Char in tank
 case 4: goto PZ_BURST;			// Burst in progress
 }
 //
 // No Pending Data - Look for next character
 //
 u->ucb$l_sts&=~UCB$M_INT; 	// Clear interrupt expected
 //
 // Call class driver for more output
 //
 long cc;  
 ((struct _tty_ucb *)u)->ucb$l_tt_getnxt(&chr,&cc,u);	// Get the next character
 if	(((struct _tty_ucb *)u)->ucb$b_tt_outype==-1)
   	goto	PZ_START_BURST;	// Burst specified
 if (((struct _tty_ucb *)u)->ucb$b_tt_outype==-0) goto PZ_DONE;		// None
 goto	BUFFER_CHAR;		// Buffer the character
 //
 // Output queue exhausted
 PZ_DONE:
 u = ((struct _tz_ucb *)u)->ucb$l_tz_xucb;		// Switch UCBs to PZ UCB
 if	((UCB$M_BSY&		// If not BSY then ignore
	 u->ucb$l_sts)==0) return;	// the char
 irp = u->ucb$l_irp;		// Restore IRP
 if	(irp->irp$l_bcnt==	// Any characters moved
	 u->ucb$l_bcnt)
   return;
   			// Yes complete I/O
 //
 // read buffer exhausted
 //
 l50:	irp = u->ucb$l_irp;		// Restore IRP
 irp->irp$l_iost1=SS$_NORMAL;		// Set successful completetion
 irp->irp$l_bcnt-=u->ucb$l_bcnt;	// Update byte count
		
 irp->irp$l_iost1|=irp->irp$l_bcnt<<16;	// Set in status

 //
 // If we wanted to here we could set the second longword of the device status
 //
 irp->irp$l_iost2=0;		// No status
 int R0 = irp->irp$l_iost1;		// Load IOSB return values
 ioc$reqcom(R0,0,u);
 return SS$_NORMAL;
 //
 // Put the character into the read buffer
 //
 BUFFER_CHAR:
 u = ((struct _tz_ucb *)u)->ucb$l_tz_xucb;		// Switch UCBs to PZ UCB
 if	((UCB$M_BSY&
	 u->ucb$l_sts)==0) goto PZ_OUT_LOOP;	// If no PZ IRP, ignore
 char * c =u->ucb$l_svapte;
 *c=chr; // check	// Add character to buffer
 u->ucb$l_svapte++;	// Bump pointer
 u->ucb$l_bcnt--;		// Show character added
 goto	PZ_OUT_LOOP;		// Go for another char
 //
 // Take care of Burst mode R5 must be TZ UCB
 //
 PZ_START_BURST:
 tty=u;
 tty->ucb$w_tt_hold|=TTY$M_TANK_BURST;	// Signal burst active
		
 //
 // Continue burst
 //
 PZ_BURST:
 ucb2 = ((struct _tz_ucb *)u)->ucb$l_tz_xucb;		// Save PZ UCB in R1
 len=0;			// Initialize output size
 if	(((struct _tty_ucb *)u)->ucb$w_tt_outlen>ucb2->ucb$l_bcnt) // Is buffer too small?
   len = ucb2->ucb$l_bcnt;		// Just output what we can
 else
   len = ((struct _tty_ucb *)u)->ucb$w_tt_outlen;		// Nope, so output all
 
 memcpy(ucb2->ucb$l_svapte,((struct _tty_ucb *)u)->ucb$l_tt_outadr,len);
 // Transfer burst to the buffer
 
 ucb2->ucb$l_svapte+=(long)len;	// Update output pointer
 ucb2->ucb$l_bcnt-=(long)len;	// Update output count
 tty=u;
 tty->ucb$l_tt_outadr+=(long)len;	// Update input pointer
 tty->ucb$w_tt_outlen-=(long)len;	// Update input count
 if (tty->ucb$w_tt_outlen==0) 			// Not the last character
   tty->ucb$w_tt_hold&=~TTY$M_TANK_BURST;	// Reset burst not active
 u = ((struct _tz_ucb *)u)->ucb$l_tz_xucb;		// Swicht UCBs to PZ UCB
 goto	PZ_OUT_LOOP;
 //
 // Get a single char from tt and put in read buffer R5 = TZ UCB
 //
 PZ_CHAR:
 chr = ((struct _tty_ucb *)u)->tty$b_tank_char;		// Get the next byte
 tty=u; tty->ucb$w_tt_hold&=~TTY$M_TANK_HOLD;	// Show tank empty
		
 goto	BUFFER_CHAR;
 //
 // Stop the output R5 = TZ UCB
 //
 // Deleted PZ_STOP2 routine and changed bit clear to byte operation - DEC 
 //
 PZ_STOP:
   u->ucb$l_sts&=~UCB$M_INT;	// Reset output active
 goto	PZ_DONE;			// DON'T go for anymore
					// Or we'll get into an infinite loop
 //
 // Send Xon or Xoff characters, R5 = TZ UCB
 //
 // Changed PZ_XOFF and PZ_XON to be PZ_PREMPT - DEC 
 //
 PZ_PREMPT:
 chr=	((struct _tty_ucb *)u)->ucb$b_tt_prempt;	// Pick up the character
 tty=u; tty->ucb$w_tt_hold&=~TTY$M_TANK_PREMPT;	// Reset Xoff state
 goto	BUFFER_CHAR;
}
#if 0
.DISABLE LSB

.SBTTL	PN$FDTSET - Set up ATTENTION ASTs
#endif
//++
// PN$FDTSET - FUNCTION DECISION ROUTINE FOR SET MODE/CHAR FUNCTIONS
//
// FUNCTIONAL DESCRIPTION:
//
// This routine is the function decision routine for SET MODE/CHAR 
// functions.
//
// P4(AP)	Determines what AST get's setup.
//			1 -> XON AST
//			2 -> XOFF AST
//			3 -> SET_LINE AST
//
// INPUTS:
//
//	R3 = I/O PACKET ADDRESS
//	R4 = PCB ADDRESS OF CURRENT PROCESS
//	R5 = UCB ADDRESS
//	R6 = CCB ADDRESS FOR ASSIGNED UNIT
//	AP = ADDRESS OF ARGUMENT LIST AT USER PARAMETERS
//
// OUTPUTS:
//
//	The function is completed here by "EXE$FINISHIO".
//
// IMPLICIT OUTPUTS:
//
// 	R3,R5 ARE PRESERVED.
//--
int PN$FDTSET(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  char devtype;
  short bufsiz;
  int devdep;
  char * buf;
  int size;
  struct _acb ** acb_p;
  int devdep2 = 0;
  struct _ucb * tz;
  int sts=SS$_NOSUCHDEV;	// Assume no TZ device
  tz=u;
  tz = ((struct _tz_ucb *)tz)->ucb$l_pz_xucb;		// Switch to TZ UCB
  if (tz==0)	goto SET_ABORT;		// No TZ UCB exit
  //;;JC------------------- Removed by J. Clement -------------------------
  //;;JC	CASE	P4(AP),LIMIT=#1,TYPE=B,<- ; Figure out what to set
  //;;JC		SET_XON_AST,-		;
  //;;JC		SET_XOFF_AST,-		;
  //;;JC		SET_LINE_AST,-		;
  //;;JC		SET_ACC_PORT>		;
  //;;JC------------------- Modified by J. Clement -------------------------
  switch (i->irp$l_qio_p4) { 	// Figure out what to set // check
  case 0: goto SET_MODE;		// Page, type ....
  case 1: goto SET_XON_AST;		//
  case 2: goto SET_XOFF_AST;		//
  case 3: goto  SET_LINE_AST;		//
  case 4: goto SET_ACC_PORT;
  }		//
  //;;JC------------------- END Modified by J. Clement -------------------------

 BAD_SET:
  sts=SS$_BADPARAM;	// Assume we were passed something bad.
 SET_ABORT:
  return exe_std$abortio(i,p,u,sts);

  //
  //;;JC------------------- Modified by J. Clement -------------------------
  //	This will set page size, and dev characteristics.
  //
 SET_MODE:
  //
  //	Set the mode
  //
  buf	 = i->irp$l_qio_p1;		// ADDRESS USER BUFFER
  size=i->irp$l_qio_p2;		// GET SIZE ARGUMENT
  if (size>0 && size<8) goto	BAD_SET;// Too small ?
  devtype=((char *)buf)[1];		// Get Type
  bufsiz=((short *)buf)[1];		// Width
  if	(bufsiz==0 || bufsiz>=511)			// Test it
    goto	BAD_SET;		// Too small ? Too big ?
  devdep=((long *)buf)[1];		// Page/characteristics
  if	(size>=12)			// DID HE ASK FOR 2ND ?
    devdep2=((long *)buf)[2];		// Get extended char.
  tz = ((struct _tty_ucb *)tz)->ucb$l_tt_logucb;		// Switch to logical device if one exists
  int savipl = DEVICELOCK(((struct _ucb *)tz)->ucb$l_dlck, ((struct _ucb *)tz)->ucb$b_dipl); //Lock out TZ activity
  // RAISE IPL
  // SAVE CURRENT IPL

  tz->ucb$b_devtype = devtype;		// BUILD TYPE, AND BUFFER SIZE
  tz->ucb$w_devbufsiz = bufsiz;		// Buffer size
  tz->ucb$l_devdepend = devdep;		// Set 1ST CHARACTERISTICS LONGWORD
  if	(size>=12) {			// DID HE ASK FOR 2ND ?
    devdep2&=~TT2$M_DCL_MAILBX;	// Kill bad bits
    tz->ucb$l_devdepnd2 = devdep2;		// Set 2nd CHARACTERISTICS LONGWORD
  }
  DEVICEUNLOCK (((struct _ucb *)tz)->ucb$l_dlck, savipl); // RELEASE INTERLOCK
  // RESTORE IPL

    sts=	SS$_NORMAL;		// Damn, it worked!
  return exe$finishioc(sts,i,p,u);		// Complete request
  //;;JC------------------- END Modified by J. Clement -------------------------


 SET_XON_AST:
  acb_p=&	((struct _tz_ucb *)tz)->ucb$l_tz_xon_ast;	// XON AST list head address
  goto	SET_CMN	;		// Go to common code

 SET_XOFF_AST:
  acb_p=&	((struct _tz_ucb*)tz)->ucb$l_tz_xoff_ast; // XOFF AST list head address
  goto	SET_CMN;			// Go to common code

 SET_LINE_AST:
  acb_p=&	((struct _tz_ucb*)tz)->ucb$l_tz_set_ast;	// SET_LINE AST list head address

 SET_CMN:
  //	i->irp$l_qio_p2=0;		// Remove user param (Is this needed?)
  com_std$setattnast(i,p,u,c,acb_p);	// Insert into AST list
  return exe$finishioc(sts,i,p,u);		// Complete request

 SET_ACC_PORT:

  //;;JC	This code should be interlocked to prevent crashes!!!!
  // We're not really going to do this...
  //;;	JMP	SET_ACC_PORT_FINISH	// Comment out this line if you dare...

  sts=	SS$_BADPARAM;	// Assume we were passed something bad.
  if (((signed long)i->irp$l_qio_p2)<0)			// What size where we given?
    goto	SET_ACC_PORT_FINISH;	// Negative number means ooops...
  if	(i->irp$l_qio_p2>ACCPOR_SIZE)	// Buffer too large?
    goto	SET_ACC_PORT_FINISH;	// ooops...

  tz = ((struct _tz_ucb *)tz)->ucb$l_tz_xucb;		// Switch to PZ UCB
  //	movc5	P2(AP),@P1(AP),#0,#ACCPOR_SIZE,R5->ucb$a_rem_host_str
  memcpy(((struct _tz_ucb *)tz)->ucb$a_rem_host_str,i->irp$l_qio_p1,i->irp$l_qio_p2); // check, zero the rest

  struct _tz_ucb * tz_real = tz;
  tz_real->ucb$a_rem_host_len = i->irp$l_qio_p2;		// It's a counted string.

  // Point to the string.
  tz = ((struct _tz_ucb *)tz)->ucb$l_pz_xucb;		// Switch to TZ UCB

  savipl = DEVICELOCK (((struct _ucb *)tz)->ucb$l_dlck, ((struct _ucb *)tz)->ucb$b_dipl); // Lock out TZ activity
  // RAISE IPL
  // SAVE CURRENT IPL

  struct _tty_ucb * tty;
  tty=tz;
  tty->ucb$l_tt_accpornam   = ((struct _tz_ucb *)tz)->ucb$l_tz_xucb;		// Point at PZ UCB
  struct _tz_ucb * t = ((struct _tz_ucb *)tz)->ucb$l_tz_xucb;
  tty->ucb$l_tt_accpornam=&t->ucb$a_rem_host_len ; //and add offset

  // Set the flag in the UCB TTY extension that indicates the string.
  tty->ucb$w_tt_prtctl|=TTY$M_PC_ACCPORNAM;
  sts=SS$_NORMAL;		// Damn, it worked!

  DEVICEUNLOCK (((struct _ucb *)tz)->ucb$l_dlck, savipl); // RELEASE INTERLOCK
  // RESTORE IPL

 SET_ACC_PORT_FINISH:
  // restore PZ UCB
  return	exe$finishioc(sts,i,p,u);		// Complete request
}


#if 0
.SBTTL	PN$FDTSENSEM - Sense mode routine
#endif
//++
// TTY$FDTSENSEM - SENSE MODE
//
// FUNCTIONAL DESCRIPTION:
//
// This routine passes the the current characteristics for SENSEMODE.
// The buffer returned is a 8 or 12 bytes depending upon users request.
//
// INPUTS:
//
//	R3 = I/O PACKET ADDRESS
//	R4 = CURRENT PCB ADDRESS
//	R5 = UCB ADDRESS
//	R6 = CCB ADDRESS
//	R7 = FUNCTION CODE
//	AP = ARG LIST FROM QIO
//
// OUTPUTS:
//
//	CONTROL IS PASSED TO EXE$ABORTIO ON FAILURE
//	OR COMPLETED VIA EXE$FINISHIO
//
// STATUS RETURNS:
//
//	SS$_NORMAL - SUCCESSFULL
//	SS$_ACCVIO - BUFFER NOT ACCESSIBLE
//
// NOTE:
//	The following code assumes that the DEVICE and FORK lock
//	for the TZ device are the same.  If this changes then this
//	code is broken.
//
//--
int PN$FDTSENSEM (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  struct _ucb * tz=u;
  int devclass;
  int devdep;
  int size, *buf, spec_chr, devdep2, speed, parity;
  VERIFY_SENSE(&size,&buf,i,p,u,c);		// VERIFY USER STORAGE
  tz = ((struct _tz_ucb *)tz)->ucb$l_pz_xucb;		// Switch to TZ UCB
  if (tz==0) {		// Have UCB
    //	If no TZ device abort with SS$_NOSUCHDEV
    // Save error reason
    return exe_std$abortio(i,p,u,SS$_NOSUCHDEV);		// Abort request
  }
  tz = ((struct _tty_ucb *)tz)->ucb$l_tt_logucb;		// Switch to logical device if one exists
  spec_chr=GET_DCL(tz);			// BUILD SPECIAL CHARACTERISTICS

  int savipl = DEVICELOCK (((struct _ucb *)tz)->ucb$l_dlck, ((struct _ucb *)tz)->ucb$b_dipl); // Lock out TZ activity
  // RAISE IPL
  // SAVE CURRENT IPL

  devclass = *(int*)(&tz->ucb$b_devclass);		// BUILD TYPE, AND BUFFER SIZE
  devdep = tz->ucb$l_devdepend;		//RETURN 1ST CHARACTERISTICS LONGWORD
  devdep2=	tz->ucb$l_devdepnd2&~spec_chr; // check //AND 2ND LONGWORD (IF REQUESTED)
  speed= ((struct _tty_ucb *)tz)->ucb$w_tt_speed; // check // RETURN SPEED
  parity= ((struct _tty_ucb *)tz)->ucb$b_tt_parity; // check // RETURN PARITY INFO
  parity=(long)parity&~0xFF000000;		// ZERO HIGH BYTE
  parity = ((struct _tty_ucb*)tz)->ucb$b_tt_crfill;		// AND CR/LF FILL

  DEVICEUNLOCK (((struct _ucb *)tz)->ucb$l_dlck, savipl); // RELEASE INTERLOCK
  // RESTORE IPL

  *buf=devclass;			// RETURN USER DATA
  buf[1]=devdep;		//
  if	(size>=12)			// DID HE ASK FOR 2ND ?
    buf[2]=devdep2;		//
  // RETURN IOSB DATA
  //expanded	goto	CMN_EXIT;		// EXIT RETURNING R0,R1	
  return exe$finishio(speed|SS$_NORMAL,parity,i,p,u);	// check	// COMPLETE REQUEST IOSB WORD 0,1
  
}

#if 0
.SBTTL	PN$FDTSENSEC - Sense char routine
#endif
//++
// TTY$FDTSENSEC - SENSE CHARACTERISTICS
//
// FUNCTIONAL DESCRIPTION:
//
// This routine passes the the permanent characteristics for SENSECHAR.
// The buffer returned is a 8 or 12 bytes depending upon users request.
//
// INPUTS:
//
//	R3 = I/O PACKET ADDRESS
//	R4 = CURRENT PCB ADDRESS
//	R5 = UCB ADDRESS
//	R6 = CCB ADDRESS
//	R7 = FUNCTION CODE
//	AP = ARG LIST FROM QIO
//
// OUTPUTS:
//
//	CONTROL IS PASSED TO EXE$ABORTIO ON FAILURE
//	OR COMPLETED VIA EXE$FINISHIO
//
// STATUS RETURNS:
//
//	SS$_NORMAL - SUCCESSFULL
//	SS$_ACCVIO - BUFFER NOT ACCESSIBLE
//
// NOTE:
//	The following code assumes that the DEVICE and FORK lock
//	for the TZ device are the same.  If this changes then this
//	code is broken.
//--
int PN$FDTSENSEC(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {				// SENSE CHAR
  long devclass;
  long devdep;
  struct _ucb * tz=u;
  int size, *buf, spec_chr, devdep2, speed, parity;
  VERIFY_SENSE(&size,&buf,i,p,u,c);		// VERIFY USER STORAGE
  tz = ((struct _tz_ucb *)tz)->ucb$l_pz_xucb;		// Switch to TZ UCB
  if (tz==0)	{		// Have UCB
    //	If no TZ device abort with SS$_NOSUCHDEV
    // Save error reason
    return exe_std$abortio(i,p,u,SS$_NOSUCHDEV);		// Abort request
  }

  spec_chr=GET_DCL(tz);			// BUILD SPECIAL CHARACTERISTICS

  int savipl = DEVICELOCK (((struct _ucb *)tz)->ucb$l_dlck, ((struct _ucb *)tz)->ucb$b_dipl); // Lock out TZ activity
  // RAISE IPL
  // SAVE CURRENT IPL

  devclass=*(int*)(&((struct _tty_ucb *)tz)->ucb$w_tt_desize) & 0xffffff00; //BUILD TYPE, AND BUFFER SIZE
  devclass|=	DC$_TERM;		// BUILD DEVICE CLASS
  devdep = ((struct _tty_ucb *)tz)->ucb$l_tt_dechar;		//RETURN 1ST CHARACTERISTICS LONGWORD
  devdep2=	((struct _tty_ucb *)tz)->ucb$l_tt_decha1&~spec_chr; // check // AND 2ND LONGWORD (IF REQUESTED)
  speed=((struct _tty_ucb *)tz)->	ucb$w_tt_despee; // check // RETURN SPEED
  parity=((struct _tty_ucb *)tz)->	ucb$b_tt_depari; // RETURN PARITY INFO
  parity	&=~0xFF000000;		// ZERO HIGH BYTE
  parity = ((struct _tty_ucb *)tz)->ucb$b_tt_decrf;		// AND CR/LF FILL

  DEVICEUNLOCK (((struct _ucb *)tz)->ucb$l_dlck, savipl); // RELEASE INTERLOCK
  // RESTORE IPL

  *buf=devclass;			// RETURN USER DATA
  buf[1]=devdep;		//
  if 	(size>=12)			// DID HE ASK FOR 2ND ?
    buf[2]=devdep2;		;
  // RETURN IOSB DATA
  //expanded	goto	CMN_EXIT;		// EXIT RETURNING R0,R1	
  return exe$finishio(speed|SS$_NORMAL,parity,i,p,u);	// check 	// COMPLETE REQUEST IOSB WORD 0,1
  //duplicated

}

//	THIS ROUTINE BUILDS DCL PRIVATE CHARACTERISTICS

int GET_DCL(struct _ucb * ucb) {
  int spec_chr=0;			// INIT RETURN ARGUMENT
  if	(ucb->ucb$l_amb)	// ANY ASSOCIATED MAILBOX?
    spec_chr|=	TT2$M_DCL_MAILBX;	// YES, SO BUILD CHARACTERISTIC
  return spec_chr;
}

  
  //	Common exit path for PN$FDTSENSEM and PN$FDTSENSEC
#if 0
 CMN_EXIT:
  R0=	SS$_NORMAL;
  return EXE$FINISHIO();		// COMPLETE REQUEST IOSB WORD 0,1
  
#endif
  //	If no TZ device abort with SS$_NOSUCHDEV

//	This routine verifies that the user buffer is accessable

int VERIFY_SENSE(int *outsize, int **buf, struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {				// SENSE CHAR
  //
  //	Output:
  //		buf Points to user buffer
  //		outsize Is number of bytes in buffer
  //
  int size;
  *buf	 = i->irp$l_qio_p1;		// ADDRESS USER BUFFER
  // not yet	IFNOWRT	#8,(buf),l20;		// BR IF NO ACCESS TO QUADWORD BUFFER
  *outsize=8;			// INIT DEFAULT ARGUMENT SIZE
  (*buf)[0]=0; (*buf)[1]=0;			// INIT RETURN DATA
  size=i->irp$l_qio_p2;		// GET SIZE ARGUMENT
  if	(size>=12) {			// ROOM FOR SECOND DEVDEPEND SPECIFIED?
    // not yet IFNOWRT	#12,(buf),l20;		// CHECK IF WRITE ACCESS
    (*outsize)=12;			// SAVE ARGUMENT SIZE
    (*buf)[2]=0;			// INIT RETURN FIELD
  }
  return;
 l20:
  // SET ERROR STATUS
  return exe_std$abortio(i,p,u,SS$_ACCVIO);		// ABORT THE IO
}
#if 0

.SBTTL	PN$END - End of driver
PN$END:
// End of driver
.END
#endif

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
#if 0
  myrei();
#endif
}

static struct _fdt pn$fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
//int pn$startio (int a,int b) { };
static void  unsolint (void) { };
static void  cancel (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
int  pn$wrtstartio (int a,int b) { };
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

static struct _ddt pn$ddt = {
  ddt$l_start: PN$STARTIO,
  ddt$l_unsolint: unsolint,
  ddt$l_fdt: &pn$fdt,
  ddt$l_cancel: PN$CANCEL,
  ddt$l_regdump: regdump,
  ddt$l_diagbuf: diagbuf,
  ddt$l_errorbuf: errorbuf,
  ddt$ps_ctrlinit_2: PN$INITIAL,
  ddt$l_unitinit: 0,
  ddt$l_altstart: pn$wrtstartio,
  ddt$l_mntver: mntver,
  ddt$l_cloneducb: PN$CLONE_INIT,
  ddt$w_fdtsize: 0,
  ddt$ps_mntv_sssc: mntv_sssc,
  ddt$ps_mntv_for: mntv_for,
  ddt$ps_mntv_sqd: mntv_sqd,
  ddt$ps_aux_storage: aux_storage,
  ddt$ps_aux_routine: aux_routine
};

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

#if 0
static struct _spl SPIN_PN __cacheline_aligned = { spl$l_own_cpu : 0, spl$l_own_cnt : -1 /*, spl$l_spinlock : 0*/ };
#else
extern struct _spl SPIN_TZ;
#endif

void pn$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ucb->ucb$b_flck=IPL$_IOLOCK8;
  ucb->ucb$b_dipl=IPL$_IOLOCK8;
  ucb->ucb$l_dlck=&SPIN_TZ;

  ucb->ucb$l_devchar = DEV$M_REC | DEV$M_AVL | DEV$M_IDV | DEV$M_ODV;

  ucb->ucb$l_devchar2 = DEV$M_NNM;
  ucb->ucb$b_devclass = DC$_PZ;
  //  ucb->ucb$b_devtype = DT$_TTYUNKN;
  //ucb->ucb$w_devbufsiz = 132;

  // no l_intd set here either

  ucb->ucb$l_devdepend = 99; // just something to fill

  // dropped the mutex stuff

  return;
}

void pn$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ddb->ddb$ps_ddt=&pn$ddt;
  //dpt_store_isr(crb,nl_isr);
  return;
}

int pn$unit_init (struct _idb * idb, struct _ucb * ucb) {
  ucb->ucb$v_online = 0;
  //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

  // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
  // no adp or cram stuff

  // or ints etc
  
  ucb->ucb$v_online = 1;

  return SS$_NORMAL;
}

struct _dpt pn$dpt;
struct _ddb pn$ddb;
struct _ucb pn$ucb;
struct _crb pn$crb;

int pn$init_tables() {
  ini_dpt_name(&pn$dpt, "PNDRIVER");
  ini_dpt_adapt(&pn$dpt, 0);
  ini_dpt_defunits(&pn$dpt, 1);
  ini_dpt_ucbsize(&pn$dpt,sizeof(struct _tz_ucb));
  ini_dpt_struc_init(&pn$dpt, pn$struc_init);
  ini_dpt_struc_reinit(&pn$dpt, pn$struc_reinit);
  ini_dpt_ucb_crams(&pn$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_unload(&pn$dpt, PZ_UNLOAD);
  ini_dpt_end(&pn$dpt);

  ini_ddt_ctrlinit(&pn$ddt, PN$INITIAL);
  ini_ddt_unitinit(&pn$ddt, 0);
  ini_ddt_start(&pn$ddt, PN$STARTIO);
  ini_ddt_cancel(&pn$ddt, ioc_std$cancelio);
  ini_ddt_end(&pn$ddt);

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&pn$fdt,IO$_READLBLK,PN$FDTREAD,1);
  ini_fdt_act(&pn$fdt,IO$_READPBLK,PN$FDTREAD,1);
  ini_fdt_act(&pn$fdt,IO$_READVBLK,PN$FDTREAD,1);
  ini_fdt_act(&pn$fdt,IO$_WRITELBLK,PN$FDTWRITE,1);
  ini_fdt_act(&pn$fdt,IO$_WRITEPBLK,PN$FDTWRITE,1);
  ini_fdt_act(&pn$fdt,IO$_WRITEVBLK,PN$FDTWRITE,1);
  ini_fdt_act(&pn$fdt,IO$_SETMODE,PN$FDTSET,1);
  ini_fdt_act(&pn$fdt,IO$_SETCHAR,PN$FDTSET,1);
  ini_fdt_act(&pn$fdt,IO$_SENSECHAR,PN$FDTSENSEC,1);
  ini_fdt_act(&pn$fdt,IO$_SENSEMODE,PN$FDTSENSEM,1);
  ini_fdt_end(&pn$fdt);

  return SS$_NORMAL;
}

long pn_iodb_vmsinit(void) {
#if 0
  struct _ucb * ucb=&pn$ucb;
  struct _ddb * ddb=&pn$ddb;
  struct _crb * crb=&pn$crb;
#endif 
  struct _ucb * ucb=kmalloc(sizeof(struct _tty_ucb),GFP_KERNEL); //check
  struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
  struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
  unsigned long idb=0,orb=0;

  memset(ucb,0,sizeof(struct _tty_ucb)); // check
  memset(ddb,0,sizeof(struct _ddb));
  memset(crb,0,sizeof(struct _crb));

#if 0
  init_ddb(&pn$ddb,&pn$ddt,&pn$ucb,"dqa");
  init_ucb(&pn$ucb, &pn$ddb, &pn$ddt, &pn$crb);
  init_crb(&pn$crb);
#endif

  init_ddb(ddb,&pn$ddt,ucb,"pna");
  init_ucb(ucb, ddb, &pn$ddt, crb);
  init_crb(crb);

  ucb -> ucb$w_size = sizeof(struct _tty_ucb); // temp placed // check

  ucb -> ucb$w_unit_seed = 1; // was: 0 // check // temp placed
  ucb -> ucb$w_unit = 0; // temp placed

  ucb -> ucb$l_sts |= UCB$M_TEMPLATE; // temp placed

//  ioc_std$clone_ucb(&pn$ucb,&ucb);
  pn$init_tables();
  pn$struc_init (crb, ddb, idb, orb, ucb);
  pn$struc_reinit (crb, ddb, idb, orb, ucb);
  pn$unit_init (idb, ucb);

  insertdevlist(ddb);

  return ddb;

}

long pn_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc) {
  unsigned short int chan;
  struct _ucb * newucb = 0;
  //  ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&pn$ucb*/,&newucb); // check. skip?
  exe$assign(dsc,&chan,0,0,0);


  return newucb;
}

int pn_vmsinit(void) {
  //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

  unsigned short chan0, chan1, chan2;
  $DESCRIPTOR(dsc,"opa0");
  unsigned long idb=0,orb=0;
  struct _ccb * ccb;
  struct _ucb * newucb0,*newucb1,*newucb2;
  struct _ddb * ddb;

  printk(KERN_INFO "dev con here pre\n");

  ddb=pn_iodb_vmsinit();

  /* for the fdt init part */
  /* a lot of these? */

  pn_iodbunit_vmsinit(ddb,1,&dsc);

  printk(KERN_INFO "dev con here\n");

  // return chan0;

}
