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
//	.Title	MACLIB - Macro level VMS I/O Interface Routines.
//	.Ident	'2.4-1'
//
// Comment out the following line if NOT building for VMS V4 (in other
// words comment out if you want a V5 driver).
//
//	VMS_V4 = 1

#if 0
	.Enable	SUP
//	.Enable Debug,TraceBack
	.Disable Debug,TraceBack

	.Library /SYS$LIBRARY:LIB.MLB/

	.PSECT	Locked_Section,LONG
#endif

#define MOVC3(c,s,d) memcpy(d,s,c)

//++
//
// Facility:
//
//	Tektronix Network: BLISS-32 callable VMS I/O System Interface rtns.
//
// Abstract:
//
//	1) MOUNT_ip_DEVICE
//		Mount the VMS pseudo network device, not DEC's "NET" device but
//		one of our own creation.
//		This routine is called by IPACP during startup procedures.  The
//		entire purpose is to make the pseudo device available to all
//		users & let IPACP know about where the ACP queue of user I/O
//		requests lives.
//		VMS data structures for the VCB (Volume Control Blk) & AQB (ACP
//		queue blk) are allocated out of non-paged pool.  The final
//		structure is as follows:
//
//		UCB ==> VCB ==> AQB
//
//		The AQB holds the queue of user I/O request packets (IRP's).
//
//	2) DISMOUNT_IP_DEVICE
//		Dismount the IPACP volume.
//
//	3) USER_REQUESTS_AVAIL
//		Check ACP queue of user I/O requests.
//
//	4) VMS_IO$POST
//		Finish the user's I/O request by handing the IRP to VMS
//		standard IO post processing routines.
//
//	5) MOVBYT
//		Move bytes fast with movc3 hardware instruction.
//
//	6) TIME_STAMP
//		Get time in hundredths of seconds since system boot.
//		Use EXE$GL_ABSTIM.  This means that we really only have
//		a resolution down to the second, but that shouldn't
//		adversely affect anything for IP's purposes.  Previous
//		code had to go into kernel mode to read from processor
//		register, but that was enormous overhead (IP seems to
//		get the time for a lot of stuff while handling segments)
//		and wouldn't work on a uVAX (PR$_TODR processor register
//		is no longer defined in VMS V4 for that reason).
//
//	7) SWAPBYTES
//		Swap bytes within a 16-bit word.
//
//	8) ZERO_BLK
//		Zero block of bytes with movc5 instruction.
//
//	9) CALC_CHECKSUM
//
//		Calculate a checksum for a block of memory.
//
// Author:
//
//	Stan C. Smith	10-4-81
//
// Mode:
//
//	Both Kernel (VMS data structure access) & user mode routines.
//
// Assembly Instructions:
//
//	MACRO MACLIB.MAR
//
// Modification History:
//
// *** Begin CMU change log ***
//
// 2.7	12-Jul-1991	Marc A. Shannon		CMU Group N
//	Fixed problem where VCB$L_RVT was pointing to the wrong place
//	and a reference to "LOGVOLNAM" would crash the system.
//
// 2.6	14-Sep-1990	Bruce R. Miller		CMU Network Development
//	Changed _IP0: to INET$DEVICE logical.
//
// 2.5	27-Nov-1989	Bruce R. Miller		CMU Network Development
//	Removed bias towards TCP processing in order to put TCP, UDP,
//	ICMP, and IP on an equal footing.  Changed m$cancel to be 15.
//
//	24-JAN-1988	Chris HO		USC-UCS
//	fixed V5 locks
//
// 2.4	23-Jan-89	Bruce R. Miller, Pete Neergaard, Marc Shannon.
//	Corrected check for end of UCB chain when mounting and dismounting
//	IP device.  Modified routines Mount_IP_Device, and 
//	Set_IP_Device_Offline.  Previously code would overshoot the end of 
//	the UCB list and attempt to dereference null pointer when executing
//	in SMP context.
//
// 2.3	04-JUN-88	Lon Willett (Utah) and Dale Moore (CMU)
//	Changed Checksuming routines and added additional code
//	comments about checksumming.
//
// 2.2  30-Jul-87, Edit by VAF
//		Implement CQ_DEQUEUE and CQ_ENQUEUE here.
//		Minor cleanup in USER_REQUESTS_AVAIL.
//
// 2.1  23-Jan-87, Edit by VAF
//		Device driver now passes the size of the UARG block so we
//		know how much to copy into the ACP's local copy. Device driver
//		must be version 2.8 or later and ACP version 6.0 or later.
//
// 2.0  23-Jul-86, Edit by VAF
//		Pass protocol code and connection id (both from UCB) in
//		cancel processing.
//
// 1.9  16-Jul-86, Edit by VAF
//		Flush extraneous and confusing definitions.
//
// 1.8   1-JUL-86, Dale Moore
//		Change references from THC to IPDRIVER.
//
// 1.7  22-May-86, Edit by VAF
//		In CANCEL processing, pass UCB address in argument block.
//
// 1.6   7-May-86, Edit by VAF
//		In CANCEL processing, convert internal PID to external PID.
//
//*** End CMU change log ***
//
// 1.1 [2-10-82]	Stan Smith
//		Orginal version
//
// 1.2 [6-10-82] Stan Smith
//		Included device offline testing/setting back when mount occurs.
//		This prevents users from doing IO to a non-existant ACP & hanging
//		the user's process.
//
// 1.3 [8-16-82] Stan Smith
//		Blank filled the ACP volume name "TCPACP" total length is 12 bytes.
//
// 1.4 [3-16-83]	Stans Smith
//		Force dev$m_avl off before we start to mount device "thc".
//		Prevents users from doing IO to a device that's not mounted.
//
// 1.5 [6-11-83] stan
//		During vms io posting (vms_io$post) if the irp is in error then
//		force the io function to be a write thus preventing a useless
//		copy from system buffer to user buffer.  On success, if the
//		io function is a read (irp$v_func=true in irp$w_sts), then
//		set the actual # of bytes received in irp$w_bcnt instead of
//		original requested amount.  save some time.
//--

//.SBTTL	System & Local Symbol definitions

// VMS External Definitions.

#include <arbdef.h>		// Access Rights Block defs.
#include <aqbdef.h>		// ACP Queue Block
	  // not yet #include <chfdef.h>		// Condition Handler Facility.
#include <irpdef.h>		// I/O Request Packet
#include <ucbdef.h>		// Unit Control Block
#include <vcbdef.h>		// Volume Control Block.

#ifndef		VMS_V4
#include <cpudef.h>		// VMS Version 5.0 SMP
#endif
#include <crbdef.h>		// channel request block
#include <dcdef.h>		// device classes and types
#include <ddbdef.h>		// device data block
#include <devdef.h>		// device characteristics
#include <idbdef.h>		// interrupt data block
#include <iodef.h>		// I/O function codes
#include <ipldef.h>		// hardware IPL definitions
#include <ssdef.h>		// system status codes
#include <vecdef.h>		// interrupt vector block
#include <fkbdef.h>		// define fork block offsets
#include <dyndef.h>
#include <pcbdef.h>
	  // not yet PR780DEF

#include <linux/sched.h>

#include <system_data_cells.h>

#include <descrip.h>

#include "netvms.h"

#include "structure.h"

#include <asm/hw_irq.h>
#include <linux/mm.h>

#include <misc.h>

	  // inconsistent naming. avoiding and working around
#define ab$l_pid vc$pid
#define ab$l_ucb_adrs vc$ucb_adrs
#define ab$w_uargsize vc$uargsize
#define ab$b_funct vc$funct
#define ab$l_tcbid vc$conn_id
#define ab$b_protocol vc$protocol
#define ab$w_iochan vc$piochan

#define exe$alononpaged EXE$ALONONPAGED
#define exe$deanonpaged EXE$DEANONPAGED

int EXE$ALONONPAGED(int size, int * addr) {
	  *addr=kmalloc(size,GFP_KERNEL);
	  return SS$_NORMAL;
}

int EXE$DEANONPAGED(int * addr) {
  kfree(addr);
  return SS$_NORMAL;
}

// Local symbols

#define M$CANCEL	14	// IP user function code, Cancel connections for PID.
#define FALSE		0	// Bliss definition of false.

// The following definitions are copied from the Network virtual device
// driver "ip"; Module(IPDRIVER.MAR).
// IPACP argument block fields.  This definition also includes the fields
// definied in the SB block.  The IP ACP reads these fields as arguments
// to a user network I/O request. This block defines a CANCEL request only.

#define ucb$l_tcbid ucb$l_devdepend
#define ucb$l_protocol ucb$l_devdepnd2

#if 0

	$DEFINI	AB			// ARGUMENT BLOCK

// These first fields are general function fields.

$DEF	AB$L_DATA_ADRS			// WITHIN THIS BLOCK, START OF Data
			.BLKL	1
$DEF	AB$L_Users_Buf_Adrs		// user's buffer start address
			.BLKL	1
$DEF	AB$L_Blk_ID			// VMS dynamic block ID fields
			.BLKL	1
$DEF	AB$L_IRP_Adrs			// Associated IRP address
			.BLKL	1
$DEF	AB$L_UCB_Adrs			// Unit Control Blk
			.BLKL	1
$DEF	AB$L_PID			// User's PID
			.BLKL	1
$DEF	AB$W_UARGSIZE			// Size of the UARG block
			.BLKW	1
$DEF	AB$B_Funct			// ACP function code
			.BLKB	1
$DEF	AB$B_Protocol			// ACP Protocol code
			.BLKB	1

// The following are CANCEL specific fields.

//$DEF	AB$W_Protocol			// Protocol code for CANCEL
//			.BLKW	1
$DEF	AB$W_IOchan			// IO channel for CANCEL
			.BLKW	1
$DEF	AB$L_TCBID
			.BLKL	1	// Connection ID for CANCEL
$DEF	AB$SIZE
			.BLKB	0	// Size of this block

	$DEFEND	AB
#endif 

//SBTTL	Local Data Declarations

//Shared_Device::	.ASCID /_ip0:/	// name of shared pseudo device
struct dsc$descriptor Shared_Device =	ASCID2 (11,"INET$DEVICE");	// name of shared pseudo device

// These globals are known to IP.

long ucb_adrs=0;	// adrs of UCB
long acp_qb_adrs=0;
long vcb_adrs=0;
long myuic=0;
long mypid=0;

// Range of pages which get locked in WS because they raise IPL & we
// can't afford any page faults here!

void Begin_Lock(),End_Lock();
struct { long a,b; } Locked_Range={a:	Begin_Lock, b:	End_Lock};

// IO Status block used by user_requests_avail rtn: when an IRP appears
// but has not come from the network pseudo-device driver (ie, IRP$L_SVAPTE
// is 0).  In this case the IRP is turned over to VMS IO post-processing
// via a calls to VMS_IO$POST rtn.  The IOSB is used as an arg to VMS_IO$POST.

long long IOSB=0;

long no_argblk=0;
long funct=0;

//SBTTL	Mount_ip_DEVICE - Mount the Pseudo device "ip".

//++
// MOUNT_ip_DEVICE, Mount Sharable Pseudo Device for all users.
//
// Functional Description:
//
//	Find the UCB (Unit Control Block) & build the VMS data structures
//	which define a mounted volume.  Final structure is:
//
//	UCB ==> VCB ==> AQB
//
//	AQB (ACP Queue Blk) contains the queue list head of user I/O
//	request packets.
//
// Calling Sequence:
//
//	$CMKRNL(routin=Mount_ip_Device);
//
// INPUTS:
//
//	None.
//
// Outputs:
//
//	Appropriate Error return codes.
//	OTHERWISE -  SS$_NORMAL with the volume correctly mounted & VMS is happy.
//
// Side Effects:
//
//	Global locations set:
//
//		UCB_ADRS	- Unit Control Blk address
//		ACP_QB_Adrs	- AQB address
//		VCB_Adrs	- Volume Control Block address.
//
//	Pages containing the Mounting, IO request checking & IO posting
//	are all locked in the working set.  Main reason is IPL is often
//	elevated to IPL_SYNCH & we can't have page-faults that high.
//
//--

//ENTRY	Mount_ip_Device,^M<R2,R3,R4,R5,R6,R7,R8>

void mount_ip_device() {
  int R0,R3,R6,R7,R8,R9;
  struct _vcb * R1;
  struct _aqb * R2;
  struct _pcb * R4 = ctl$gl_pcb;
  struct _ucb * R5;
  struct _ucb * U;
 Begin_Lock:
  // not yet	$LKWSET_S	Locked_Range	// lock us in the working set.	
  R1 = &Shared_Device;	// point @ Pseudo device desc.
  R0 = find_ucb(R1,&U);		// locate the UCB
  R1 = U;
  if ((R0&1)==0)			// OK ?

  // Unable to find UCB,  return error here.

    return SS$_NOSUCHDEV;	// No such device error.

  // found UCB, R1 = UCB address
  // Check if volume is already mounted.

  ucb_adrs = R1;			// save UCB address
  R5 = R1;				// UCB ptr.
  R5->ucb$l_devchar &= ~DEV$M_AVL;	// disallow any user io.
  if	((DEV$M_MNT&R5->ucb$l_devchar)==0)
    goto	Not_Mounted;

  // Volume is mounted, Set globals & leave

  R0 = SS$_VOLINV;		// assume error
  R1 = R5->ucb$l_vcb;	// get VCB address
  if (R1==0)			// OK
    return	Dismount();		// oops, clean up.
  vcb_adrs = R1;		// save VCB address.
  R2 = R1->vcb$l_aqb;	// get AQB address
  if (R2==0) 
    return	Dismount();		// Error: Clean up
  acp_qb_adrs = R2;		// save AQB address

  // Indicate we now own the device, set my PID in ACP queue blk.

  R0 = lock_iodb();

#ifdef VMS_V4
  SETIPL	#IPL$_SYNCH		// synchronize with VMS
#else
    FORKLOCK(R5->ucb$b_flck,-1);			// R5->UCB$B_FLCK
#endif

  R2->aqb$l_acppid = R4->pcb$l_pid;	// set new owner PID.
  mypid = R4->pcb$l_pid;	// save for net$dump rtn.
  R1 = R4->pcb$l_arb;	// adrs of ARB
  // not yet myuic = ((struct _arb *)R1)->arb$l_uic;	// save my UIC.

#ifndef VMS_V4
  FORKUNLOCK(R5->ucb$b_flck,-1);			// R5->ucb$b_flck
#endif
  R0 = unlock_iodb();

#ifdef VMS_V4
  SETIPL	#0				// timeshare
#endif

    all_done:

  // Mark device "ip0:" as ONline, mounted & Available.
  // Check for any cloned ip devices & mark them also.  Case of ACP crash
  // & user's had ip devices assigned. crash sets device offline & clears
  // avail.
  // R5 = UCB adrs

#ifdef VMS_V4
  DSBINT	R5->UCB$B_FIPL			// synch with VMS
#endif
    l10:
#ifndef VMS_V4
#if 0
  DEVICELOCK	-			;
  SAVIPL=-(SP),-			;
  PRESERVE=NO			;
#endif
#endif
  R5->ucb$l_sts |= UCB$M_ONLINE;
  R5->ucb$l_devchar |= DEV$M_AVL|DEV$M_MNT;
  R1 = R5->ucb$l_link;		// Get next UCB
#ifndef VMS_V4
#if 0
  DEVICEUNLOCK -
    NEWIPL=(SP)+, -
    PRESERVE=NO
#endif
#endif
    R5 = R1;
  if (R5) goto	l10;
#ifdef VMS_V4
  ENBINT					// reset IPL
#endif
    R0 = SS$_NORMAL;
 bye:
  return R0;

  // Device is NOT Mounted, Mount it.
  // R5 = UCB address

 Not_Mounted:
  R0 = Build_ACP_QB(R5);		// build & link ACP Queue blk.
  if	(SS$_INSFMEM==R0)		// OK?
    goto	bye;			// EQL means Error.
  R8 = acp_qb_adrs;		// for build_VCB
  R0 = build_vcb(R5,R8);
  if ((R0&1)==1) goto all_done;
  return	Dismount();		// lbs = Error.
}

//SBTTL	LOCK_IODB - Lock the I/O Database

//++
// FUNCTIONAL DESCRIPTION:
//
//	Lock the I/O database mutex
//
// Call Sequence:
//
//	LOCK_IODB();
//
// Inputs:
//
//	None.
//
// Outputs:
//
//	R4 = My PCB address
//
// Side Effects:
//
//	I/O Data Base Mutex is locked.
//	IPL set to IPL$_ASTDEL
//
//--

lock_iodb(int * R4) {
  // not yetR4 = ctl$gl_pcb;		// get my PCB address
  *R4=ctl$gl_pcb;
  return	SCH$IOLOCKW();		// lock & return
}

//SBTTL	UNLOCK_IODB - Unlock the I/O Database

//++
//
// Functional Description
//
//	Unlock the I/O Database mutex
//
// Calling Sequence:
//
//	UNLOCK_IODB();
//
// Inputs:
//
//	None.
//
// Outputs:
//
//	R4 = My PCB address
//
// Side Effects:
//
//	I/O Database mutex is unlocked
//	IPL set to 0
//
//--

unlock_iodb(int * R4) {
  int R0,R1,R2,R3,R5,R6,R7,R8,R9;
  // not yet R4 = ctl$gl_pcb;
  *R4=ctl$gl_pcb;
  R0 = SCH$IOUNLOCK();			// unlock I/O database
#ifdef VMS_V4
  SETIPL	#0				// timeshare
#endif
    return R0;
}

//SBTTL	FIND_UCB - Locate specified Unit Control Block.

//++
//
// Functional Description:
//
//	Search the system list of DDB (Device Data Blocks) trying to
//	match the specified DEVICE.
//
// Calling Sequence
//
//	FIND_UCB();
//
// Inputs:
//
//	R1 = Address of Device descriptor (.ASCID)
//
// Outputs:
//
//	R0 - LBC => error
//	     LBS Then R1 = Address of Desired UCB.
//
//--

int find_ucb(R1, U)
     struct _ucb ** U;
{
  int R0,R2,R3,R4,R5,R6,R7,R8,R9;
  struct return_values r;
  int sts;

  R0 = lock_iodb(&R4);
  sts = ioc$searchdev(&r,R1);		// find the UCB
  *U=r.val1;
  // not yet	-(SP) = R0;		// save return info
  R0 = unlock_iodb(&R4);
  // not yet	MOVQ	(SP)+,R0		// restore info
  return sts;				// return
}

//SBTTL	BUILD_ACP_QB - build the ACP Queue Blk structure

//++
// Functional Description:
//
//	Build the ACP queue block data structure in NON-paged dynamic
//	pool space.  Here we find the queue of IRP's (I/O Request Packets)
//	as set up by the QIO Function Dispatch routines.  ACP QB (AQB)
//	is linked in the system list of ACP Queue blocks.

//
// Inputs:
//
//	R5 = UCB address
//
// Outputs:
//
//	R0 = return Code.
//		SS$_INFSMEM - Allocation Error.
//
// Side Effects:
//
//	May return with SS$_INFSMEM if unable to allocate dynamic
//	non-paged pool.
//	ACP_QB_Adrs = Address of AQB constructed.
//	IO database is locked/unlocked while at IPL_SYNCH when AQB is
//	linked into system AQB list.
//
//--

Build_ACP_QB(R5)
     struct _ucb * R5;
{
  int R0,R3,R6,R7,R9;
  struct _arb * R1;
  struct _aqb * R2, *R8;
  struct _pcb * R4;
  R1 = AQB$C_LENGTH;
  R0 = exe$alononpaged(R1,&R2);		// get chunck of non-paged pool.
  if ((R0&1)==0)				// Error?

  // Return error, unable to allocate non-paged pool.

    return SS$_INSFMEM;

  // Fill in AQB.
  // R2 = address of AQB

  acp_qb_adrs = R2;			// save address
  R2->aqb$w_size = AQB$C_LENGTH;	// set block size.
  R2->aqb$b_type = DYN$C_AQB;	// blk type
  R2->aqb$b_acptype = AQB$K_NET;	// say its a Network AQB
  R2->aqb$b_class = 0;			// ACP class (noclass...)
  R2->aqb$b_status = AQB$M_UNIQUE;	// ACP unique to this device.
#ifdef VMS_V4
  R2->aqb$l_acpqfl = R2;		// set IRP queue ptrs.
  R2->aqb$l_acpqbl = R2;		// & back link.
#else
  // WARNING: V5 change! V5 uses self-relative ACP queues!
  R2->aqb$l_acpqfl = 0;		// Init IRP queue ptrs
  R2->aqb$l_acpqbl = 0;		// & back link.
#endif
  qhead_init(&R2->aqb$l_acpqfl);  // check. fix and change later
  R2->aqb$b_mntcnt = 0;		// init mount count

  // Link this AQB into the system list.  Instert at front of list

  R8 = R2;				// save AQB adrs

  R0 = lock_iodb(&R4);			// lock IO datbase
#ifdef VMS_V4
  SETIPL	#IPL$_SYNCH			// Synchronize with VMS
#else
    FORKLOCK(R5->ucb$b_flck,-1);			;
#endif

  R2->aqb$l_acppid = R4->pcb$l_pid;	// set owner PID
  mypid = R4->pcb$l_pid;		// save for net$dump rtn.
  R1 = ioc$gl_aqblist;		// adrs of system list
  R8->aqb$l_link = R1;		// set forward link
  ioc$gl_aqblist = R8;				// set list head.

  // Save my UIC

  R1 = R4->pcb$l_arb;		// addres ARB
  // not yet myuic = R1->arb$l_uic;		// my uic.
#ifndef	VMS_V4
  FORKUNLOCK(R5->ucb$b_flck,-1);
#endif
  R0 = unlock_iodb(&R4);			// unlock & return
  return R0;
}

//SBTTL	BUILD_VCB - Build A volume Control block.

//++
//
// Functional Description:
//
//	Allocate & build the VMS Volume control block from nonpaged pool.
//	VCB is pointed to by the UCB & VCB points to the AQB.
//	Here we link the entire chain together.
//
// Inputs:
//
//	R5 = UCB address
//	R8 = AQB address.
//
// Outputs:
//
//	LBC(R0) THEN Error
//	LBS(R0) THEN success.
//
// Side Effects:
//
//	UCB ==> VCB ==> AQB.  Chain is initialized.
//	AQB mount count is set to traditional ACP idle count (1).
//	VCB_Adrs = address of Volume-Control-Blk.
//
//--

build_vcb(R5,R8)
     struct _ucb * R5;
     struct _aqb * R8;
{
  int R0,R1,R3,R4,R6,R7,R9;
  struct _vcb * R2;
  R1 = VCB$C_LENGTH;		// size of VCB
  R0 = exe$alononpaged(R1,&R2);		// allocate nonpaged pool.
  if ((R0&1)==0) 				// Error? punt if yes.
    return R0;

  // Fill in the VCB
  // R2 = address of VCB

  vcb_adrs = R2;			// save address.
  R2->vcb$b_type = DYN$C_VCB;	// set blk type
  R2->vcb$w_size = VCB$C_LENGTH;	// set blk size
  // not yet?	R2->vcb$w_trans = 1;		// Traditional ACP idle count
  // not yet?	R2->vcb$w_rvn = 0;			// Clear number of rel. volumes
  R2->vcb$b_status = 0;
  // not yet?	R2->vcb$w_mcount = 1;		// 1 volume mounted.
  memcpy(&R2->vcb$t_volname,"Net ",4);	// set volume name
  memcpy(&R2->vcb$t_volname[4],"Devi",4);	// 2nd part of name.
  memcpy(&R2->vcb$t_volname[8],"ce  ",4);	// blank filled (12 chars total).
  R8->aqb$b_mntcnt++;		// say volume is mounted.

  // Link AQB to VCB

  R2->vcb$l_aqb = R8;		// VCB ==> AQB

  // Link UCB into the UCB$L_RVT to prevent crashes from users requesting
  // DVI$_LOGVOLNAM

  R2->vcb$l_rvt = R5;		// VCB$L_RVT ==> UCB (?)

  // link VCB to UCB

  R5->ucb$l_vcb = R2;		// UCB ==> VCB
  return SS$_NORMAL;
}

//SBTTL	Set ip Device Off-line

//++
//
//Function:
//
//	Mark the ip0 device as offline & unavailable.
//	routine called at Kernel mode access.
//
// Inputs:
//
//	None.
//
// Implicit INputs:
//
//	UCB_adrs = address of ip0 UCB(Unit Control Blk).
//	Access mode = kernel, so we can touch ucb's.
//
// Outputs:
//
//	None.
//
// Side Effects:
//
//	Device "ip0:" is marked as OFFLINE & DEV$M_AVL is cleared.
//	all cloned UCB's for this controller are also marked offline.
//
//--

//Entry	Set_ip_Device_OffLine,^M<R5>
int set_ip_device_offline(UCB_Adrs)
{
  int R0,R1,R2,R3,R4,R6,R7,R8,R9;
  struct _ucb * R5;
  R5 = UCB_Adrs;			// adrs of ip0: (base device).
  if (R5>=0)	goto l15;				// good UCB address? must be system address
#ifdef VMS_V4
  DSBINT	R5->UCB$B_DIPL			// synch UCB access.
#endif
    l10:
#ifndef VMS_V4
#if 0
  DEVICELOCK		-		;
  SAVIPL=-(SP),-			;
  PRESERVE=NO			;
#endif
#endif
  R5->ucb$l_sts &= ~UCB$M_ONLINE;	// for show dev, mark offline.
  R5->ucb$l_devchar &= ~DEV$M_AVL;
  R1 = R5->ucb$l_link;		// get next UCB
#ifndef VMS_V4
#if 0
  DEVICEUNLOCK -
    NEWIPL=(SP)+, -
    PRESERVE=NO
#endif
#endif
    R5 = R1;
  if (R5) goto	l10;
#ifdef VMS_V4
  ENBINT					// restore previous IPL.
#endif
    l15:
  return R0;
}

//SBTTL	DISMOUNT - Dismount the ACP volume & deallocate the data structures.

//++
//
// Functional Description:
//
//	Dismount the volume from the pseudo device.  Deallocate VMS data
//	structures AQB,VCB.  Unhook the AQB from the system list.
//
// Calling Sequence:
//
//	JMP DISMOUNT
//
// Inputs:
//
//	R0 = Error code.
//	R5 = UCB address
//
// Outputs:
//
//	Appro. return code.
//
// Side Effects:
//
//	Routine returns after cleaning up the UCB.
//
//--

int Dismount() {
  int R1,R2,R3,R4,R6,R7,R9;
  struct _ucb * R5;
  struct _aqb * R0,* R8;
 DISMOUNT:
  /*PUSHL	R0*/			// save the return code
#ifdef VMS_V4
  DSBINT	R5->ucb$b_dipl		// Save IPL & set new IPL to Device IPL.
#else
#if 0
    DEVICELOCK		-		;
  SAVIPL=-(SP),-			;
  PRESERVE=NO			;
#endif
#endif
  R5->ucb$l_devchar&=~(DEV$M_MNT|DEV$M_AVL);	// clear Avail & mounted.
  R5->ucb$l_refc = 0;		// no references
  //	R5->ucb$l_ownuic = 0;
  R5->ucb$l_pid = 0;
  R5->ucb$l_vcb = 0;		// clean up UCB link
#ifdef VMS_V4
  ENBINT				// reset IPL level.
#else
#if 0
    DEVICEUNLOCK			-	;
  NEWIPL=(SP)+,		-	;
  PRESERVE=NO
#endif
#endif
    R0 = vcb_adrs;		// get VCB adrs
  if (R0)			// OK ?
    R0 = exe$deanonpaged(R0);	// dealllocate space.

  // Deallocate ACP Queue blk

  R8 = acp_qb_adrs;
  if (R8==0) goto	l100;			// OK?

  // Unhook AQB from system list
  // AQB list is a singly linked list.

  R0 = lock_iodb(&R4);		// lock IO database
#ifdef VMS_V4
  DSBINT	#IPL$_SYNCH		// Save Current IPL & set new IPL.
#else
    FORKLOCK(R5->ucb$b_flck,-1);
#endif
  R1 = ioc$gl_aqblist;	// system AQB list head
  R0 = (R1);			// 1st AQB pointer.
  if 	(R8!=R0)			// 1st AQB?
    goto	l70;			// IF NEQ THEN "NO"

  // 1st AQB in system list.

  (R1) = R8->aqb$l_link;	// link it in
  goto	l90;			// done

  // try next AQB in list

 l70:
  if	(R0->aqb$l_link==R8)	// is this it?
    goto	l80;			// EQl = yes.

  // advance to next AQB

  R0 = R0->aqb$l_link;	// get next link
  goto	l70;			// loop

  //

 l80:
  R0->aqb$l_link = R8->aqb$l_link;	// relink
 l90:
#ifdef VMS_V4
  ENBINT				// restore IPL
#endif
    R0 = R8;			// for deallocation rtn.
  R0 = exe$deanonpaged(R0);
#ifndef VMS_V4
  FORKUNLOCK(R5->ucb$b_flck,-1);
#endif
  R0 = unlock_iodb(&R4);		// unlock IO database.
  // all done

 l100:	/*POPL	r0*/			// get return code
  return R0;
}


//SBTTL	USER REQUESTS AVAIL - Get User I/O requests for IP.

//++
//
// Functional Description:
//
//	Check the ACP queue block list head to see if any user IRP's have
//	been queued to IP.  If IRP's present return the IP argument
//	block pointer or FALSE(0).
//	***** Warning *****
//	If the IPACP argument block function is M$CANCEL then the IRP is NOT
//	from a user process & MUST NOT be posted as with normal IRPs.
//
// Calling Sequence:
//
//	$CMKRNL(routin=User_Requests_AVAIL);
//
// Inputs:
//
//	Kernel Mode.
//	Global	ACP_QB_Adrs is valid, device/volume is mounted.
//
// Outputs:
//
//	R0 = TCP argument block address if IRP's were present
//	OR R0 = False(0) if no IRP's present.
//
// Side Effects:
//
//	If there are IRP's & associated system-buffer(IPACP argblk) then
//	allocate a IP process local argblk & copy the IP arguments from the
//	system buffer to the local buffer.  This is performed so we don't have
//	to be in kernel mode all the time (debug tools avail).  If the IRP
//	has no associated IPACP argument block buffer (ie, IRP$L_SVAPTE = 0)
//	then assume the IRP was sent by VMS & check if IO function was IO$_CLEAN.
//	If TRUE then build a fake IPACP user argument block which indicates to 
//	IPACP that a connection for the specified "PID & IO-chan" should be RESET.
//	VMS will call the ip: IO cancel routine in the case of Control-C
//	image rundown (closing open channels) & when the device is deasigned.
//
//__

//Entry USER_Requests_Avail,^M<R2,R3,R4,R5>
int user_requests_avail()
{
  int R1,R3,R4,R6,R7,R8,R9;
  struct _aqb * R0;
  struct _irp * R2;
  struct vms$cancel_args * R5;
 Try_Again:
  R0 = acp_qb_adrs;		// Network AQB address
  if (((long)R0)>=0) 			// Valid system address? < 0 = OK.
    return 0;
#ifdef VMS_V4
  REMQUE	@R0->aqb$l_acpqfl,R2
#else
    int wasempty=aqempty(R0->aqb$l_acpqfl);
#if 0
  REMQHI	(R0->aqb$l_acpqfl,R2);
#endif
  R2 = R0->aqb$l_acpqfl;
  remque(R2, 0); // change to relative later
#endif
  if (wasempty)

  // Queue was empty, return false(0)

    return 0;

  // Queue contained at least 1 IRP.  Get IPACP argument blk address.
  // R2 = address of IRP

  R0 = R2->irp$l_svapte;	// get address of system IPACP argblk.
  if (R0==0) goto	NO_IPACP_Arg_Blk;		// EQL means we have an error.

  // Copy IPACP argument block from the system buffer (Kernel mode access) to
  // IP local process space.  Entire idea is to execute at user level & only
  // be in Kernel mode when we have to (no debug tools to speak of for kernel
  // mode).  Anyway we allocate an ACP argument block & copy just the ACP argument
  // block, not the data.  When we access the data(send) or copy data to the
  // system buffer(receive) we must be in Kernel mode.
  // R0 = ACP argblk address
  // R2 = IRP address

  R2 = R0;			// save system argblk adrs.
  R0 = mm$uarg_get();		// allocate IP process space
  // Returns in R0

  // copy the argblk system-space ==> IP-Local-space.

  /*PUSHL	R0*/			// Save the argument block address
  R1 = ((struct vms$cancel_args *)R2)->ab$w_uargsize;	// Get fullword count value
  MOVC3	(R1,(R2),(R0));		// Copy data from system to local copy
  /*POPL	R0*/			// Return local copy pointer
  return R0;

  // IRP has no associated system buffer (ACP argument block).  This indicates
  // the IRP did not come from the pseudo-device driver & therefore the IPACP
  // will choke on it.  Check if the function code is IO$_CLEAN.  IF TRUE
  // then the user process has been interrupted (Control C or Y) & we must cancel
  // a Connection for this PID & IO channel.  Build a fake ACP argblk with the
  // IP function M$Cancel which will RESET the connection for the specified PID
  // & channel #. IF the check was false then just return the IRP to VMS IO
  // post-processing with an SS$_NORMAL return code.

 NO_IPACP_Arg_Blk:
  no_argblk++;
  R4 = R2->irp$l_func;	// Get IO function code.
  funct = R4;
  if	(IO$_CLEAN!=R4)		// ACP cancel?
    goto	UR$Post;			// No, post IO as SS$_NORMAL

  // User image is in rundown (exit) state & has canceled IO
  // Build an IPACP user argblk & return it with the IP function code M$CANCEL.
  // This will cancel a connection for the specified PID & channel #.
  // VMS will cancel IO on each channel assigned to the "ip" device.
  // Remember: This fake IRP Must NOT be posted to VMS IO post rtns.  System crash
  // will occur.

  R0 = mm$uarg_get();		// get a IPACP user argument blk.
  R5 = R0;			// save argblk adrs
  //;;	R5->ab$l_pid = R2->irp$l_pid;	// set PID in IPACP argblk.
  R0 = R2->irp$l_pid;	// Transform internal PID
  R0 = exe$ipid_to_epid();	// to external PID format
  R5->ab$l_pid = R0;		// Set in argument block
  R0 = R2->irp$l_ucb;	// Get UCB address
  R5->ab$l_ucb_adrs = R0;	// Set UCB address in argblk
  //	R5->ab$b_protocol = R0->ucb$l_protocol; // Set protocol code
  R5->ab$l_tcbid = ((struct _ucb *)R0)->ucb$l_tcbid;	// Connection ID
  R5->ab$b_funct = M$CANCEL;	// IP function code
  R5->ab$w_uargsize = sizeof(struct vms$cancel_args)/*AB$SIZE*/;	// Size of the argblk
  R5->ab$w_iochan = R2->irp$w_chan;	// include channel #.

  R5->ab$b_protocol = R2->irp$l_extend; ;

  R0 = R2;			// point at Fake IRP.
#if 0
  R0 = com_std$drvdealmem(R2);	// release Fake IRP.
#else
  kfree(R2);
#endif
  R0 = R5;			// return IPACP argblk pointer.
  return R0;

  // Release/post the IO as SS$_NORMAL
  // R2 = IRP address.

 UR$Post:
  /*PUSHL	R2->IRP$L_UCB*/		// UCB address
  /*PUSHL	R2*/			// IRP address
  IOSB = SS$_NORMAL;	// set return status
  /*PUSHAQ	IOSB*/			// address of IOSB
  R0 = VMS_IO$POST(IOSB,R2,R2->irp$l_ucb);
  goto	Try_Again;		// dismiss this & look for more.
}

//SBTTL	VMS_IO$POST - Hand User's IRP to VMS IO Post-processing.

//++
//
// Functional Description:
//
//	Fill in IRP IOST1 & IOST2 fields, insert IRP in IO post-processing
//	queue.  If 1st entry in the queue then request IOPOST software
//	interrupt.
//
// Calling Sequence:
//
//	$CMKRNL(routin=VMS_IO$POST,ArgLst=args);
//
// Inputs:
//
//	Kernel mode.
//	0(AP)	Number of arguments to follow.
//	4(AP)	IOSB address
//	8(AP)	IRP address
//	12(AP)	UCB address
//
// Outputs:
//
//	None - BLISS NOVALUE routine.
//
// Side Effects:
//
//	if the iosb low-bit is clear, indicating an error, then
//	force the io function in irp$w_sts to be a write function.  This
//	eliminates the needless copy of bogus data from the system buffer to
//	the user's buffer.  Force irp$v_func to be "0".
//
//	When the successful function is a read (irp$v_func=true in irp$w_sts),
//	then set irp$w_bcnt to reflect the actual # of bytes received instead
//	of the original requested amount.
//
//	If IRP is only member of IO post process queue then a
//	software interrupt (IOPOST) is requested.
//--

// Local stack (AP) offsets, CALLS sets stack in this fashion.

#if 0
#define IOSB$Adrs	  4
#define IRP$Adrs	  8
#define UCB$Adrs	 12
#endif


//ENTRY	VMS_IO$POST,^M<R2>
int VMS_IO$POST(IOSB$ADRS,IRP$ADRS, UCB$ADRS)
{
  int R3,R4,R5,R6,R7,R8,R9;
  struct _vcb * R0;
  struct _irp * R1;
  struct _ucb * R2;
  R1 = IRP$ADRS;		// get IRP address
  R2 = UCB$ADRS;		// UCB address
  R2->ucb$l_opcnt++;		// increase operation count

  // Adjust volume transaction count.

  R0 = vcb_adrs;		// adrs Volume control blk.
  // not yet	R0->vcb$w_trans--;		// indicate transaction has finished.
  //  R1->irp$l_iost1=IOSB$ADRS;	// set IOST1 & 2
  memcpy(&R1->irp$l_iost1, IOSB$ADRS, 8);	// set IOST1 & 2

  // if lbc (low bit clear) {error indicator} then clear irp$v_func in
  // irp$w_sts to prevent useless copy by kernel mode iopost ast routine

  if ((R1->irp$l_iost1&1)==0) {
    R1->irp$l_sts&=~IRP$M_FUNC; // error - fake a write function.
    			// time to post!
  } else {

  // set actual bytes number transfered if this is a read function

    if	(1==(IRP$M_FUNC&R1->irp$l_sts))
      R1->irp$l_bcnt= ((short *) &R1->irp$l_iost1)[1];  // set bytes to give user.
  }

  // insert IRP into I/O post process queue.

#ifdef VMS_V4
  INSQUE	(R1),@L^IOC$GL_PSBL	// in it goes
#else
    find_cpu_data(&R2);
  int wasempty=aqempty(((struct _cpu *)R2)->cpu$l_psbl);
  INSQUE	((R1),((struct _cpu *)R2)->cpu$l_psbl);	// in it goes
#endif
  if (wasempty) 			// Neq = not 1st in queue

  // 1st IRP in queue, request IOPOST interrupt

    SOFTINT_IOPOST_VECTOR;

  return R0;
}

//End_Lock::		// end of locked pages

//SBTTL	Move Bytes "FAST" with movc3 instruction.

//++
//
// Function Description:
//
//	Move bytes via the movc3 instruction.  Copy same number of
//	bytes from source to destination.
//
// calling Sequence:
//
//	BLISS callable rtn "CALLS" linkage:  MOVBYT(Size,SRC,DEST)
//		Warning:  SRC & DEST are addresses!
// Inputs:
//
//	0(AP)	count of args on stack
//	4(AP)	Number of bytes to move
//	8(AP)	Source address
//	12(AP)	Destination address
//
// Outputs:
//
//	None.
//
// All registers preserved
//
//--

// "AP" pointer offsets, calls arguments

#if 0
#define Size	  4
#define Src	  8
#define Dest	  12
#endif

  //MovByt::
int MOVBYT(Size,Src,Dest) {
  int R0,R1,R2,R3,R4,R5,R6,R7,R8,R9;
  MOVC3	(Size,Src,Dest);
  return R0;
}

//SBTTL	Time_Stamp - Get time in hundredths of a second.

//++
//
// Function:
//
//	Get time in hundredths seconds since Jan of cur year.  Use this time instead
//	of QUAD-word system time since its costs less space & is easier to
//	deal with (eg, comparisons).  Read system global "EXE$GL_ABSTIM".

//
// Calling Sequence:
//
//	CALLS from BLISS standard.
//
// Inputs:
//
//	None.
//
// Outputs:
//
//	R0 = time in hundredths seconds since Jan 1 of current year
//	(longword value).
//
//--

//Entry	Time_Stamp, 0
int Time_Stamp()
{
  int R1,R2,R3,R4,R5,R6,R7,R8,R9;
  long long R0;
  //	R0 = @EXE$GL_ABSTIM;	// Get system interval timer
  //	MULL2	100, R0		// Convert seconds to 100ths
  /*PUSHL	R1*/			// Save R1 from being tromped on
  R0 = exe$gq_systime;	// copy the current time into R0/R1
  R0 &=	0x7fffffffffff;
  // not yet	R0 = R0/100; R0=R0/1000; // Convert with R0 in 100ths
  /*POPL	R1*/
  return 10*(long)(R0>>20);
}


//SBTTL Swap bytes on word boundaries.

//++
//
//Function:
//
//	Routine will swap the low address byte of a word with the
//	high address byte of the word.
//
//Calling Convention:
//
//	VMS CALLS
//
//Inputs:
//
//	0(AP) - Number of arguments on stack.
//	4(AP) - Number of contiguous words to swap bytes} in.
//	8(AP) - Word address of where to start swapping.
//
//Outputs:
//
//	None
//
//Side Effects:
//
//	None
//
//--

#if 0
#define WrdCnt	  4					// AP offset to # of words to swap
#define Start	  8					// start address.
#endif

//ENTRY  SWAPBYTES,^M<>
	int swapbytes(WrdCnt,Start)
{
  int *R0,*R2,*R3,*R4,*R5,*R6,*R7,*R8,*R9;
  unsigned char * R1 = Start;			// starting word address.
 Swp_Loop:
  R0 = *R1;				// low ==> temp
  *R1 = R1[1];			// high ==> low
  R1++;
  *R1++ = R0;			// temp ==> High
  if	(--WrdCnt) goto Swp_Loop;		// decr word's left to do
  return R0;
}

//SBTTL	Zero block of bytes.

//++
//
// Function:
//
//	Zero a block of bytes.
//
// Inputs:
//
//	4(AP)	# of bytes to zero.
//	8(AP)	Starting address.
//
// Outputs:
//
//	None.
//
// Side Effects:
//
//	Consecuative bytes are zeroed.
//
//--

#if 0
#define Count	  0
#define Size	  4
#define Adrs	  8
#endif

//ENTRY	Zero_Blk,^M<R2,R3,R4,R5>
int Zero_Blk(Count,Size,Adrs)
{
  int R0,R1,R2,R3,R4,R5,R6,R7,R8,R9;
  /*	MOVC5	0,@Adrs,0,Size,@Adrs*/
  // not yet
  return R0;
}

//SBTTL	Calculate Checksums

//++
//
// Function:
//
//	Generate a 16-bit one's-complement of the one's complement
//	sum of series of 16 bit words.
//	See TCP DARPA document for details.
//
// Number Systems Lesson:
//
//	One's complement arithemetic is different than Two's complement
//	arithmetic.  Two's complement arithmetic is what is used on the
//	on the VAX.  The positive numbers are the same in one's Complement
//	and two's complement.  For two's complement however, NOT X = - X.
//	The complement of a number is the negative of the number.  This
//	Results in two values for 0, %X'0000' (positive zero) and
//	 %X'FFFF' (negative zero).
//	
//	Addition of two's complement can be performed without regard
//	to the sign of the numbers involved.  This is done by simply
//	adding the binary representations and ignoring any carry.
//		(-5) + (-2) = 1011 + 1110 = 11001 = 1001 = -7
//
//	Addition of one's complement numbers can be performed without
//	regard to the sign of the numbers involved.  This is done by
//	adding the binary representation of the numbers involved and
//	then adding one if a carry occurred.
//		(-1) + 6 = 1110 + 0110 = 10100 = 0100 + 1 = 0101 = 5
//
// Algorithm:
//
//	We treat the data to be checksummed as an arry of 32 bit
//	integers.  We keep a running 32 bit sum of the 32 bit integers.
//
//	Sum = 0;
//	Incr I FROM 0 TO .Size - 1 DO
//		Sum = .Sum + .Data [.I] + Carry;
//
//	We can use two's complement addition to keep this running sum
//	as long as we add with carry. Then we add in any pieces of the
//	data that wasn't a full 32 bits.
//	Once the 32 bit sum is computed, we can fold the top 16 bits
//	into the bottom 16 bits.
//
//	Sum = .Sum <0, 16> + .Sum <16, 16>;
//
//	But this folding may produce yet another carry. So we do it twice.
//
//	If you want you can think of the bits as being in a ring, where
//	any carry (out of the high bits) would be pushed into the low
//	bits.
//
//		       00
//		  15        01
//		14            02
//	       13              03
//	       12              04
//		11            05
//		 10          06
//		   09      07
//		       08
//                     
//
//	We then complement the result of the summing and check for
//	value of 0 and return only -0.
//			
//
// Calling Sequence:
//
//	Standard CALLS:
//	    CALC_CHECKSUM(Byte_Count,Start);
//	    GEN_CHECKSUM(Byte_Count,Start,SrcA,DstA,Ptcl_Type)
//
// Inputs:
//
//	4(AP) : Byte_Count = # of 8-bit bytes to checksum as words.
//	8(AP) : Start = Starting byte address.
//    Additionally, for GEN_CHECKSUM:
//	12(AP): SrcA = Source IP address
//	16(AP): DstA = Destination IP address
//	20(AP): Ptcl_Type = Protocol type of packet
//
// Outputs:
//
//
//	My interpretation of "one's complement sum" is the true sum
//	modulo 2^16-1.  Thus, it will be a value in the range 0 .. 2^16-2,
//	i.e. -0 is not a legal "one's complement sum", but -0 is.
//
//	Thus the "one's complement of the one's complement sum" 
//	will be a value in the range 1 to 2^16-1.  So the IP header
//	checksum and TCP checksum should never be zero.  (UDP specifies
//	the interpretation of putting a zero in the checksum field).
//	Under the robustness principle: accept a value of "0" where
//	it should be "-0".  I don't believe that IP or TCP should
//	skip verifying the checksum (as UDP does) when the checksum
//	field is 0, because checksumming for IP and TCP is not optional.
//
//	The one's complement sum of a good packet will be 0.  Of
//	course the checksum routines return the "one's complement of
//	the one's complement sum", so apply a NOT before comparing
//	against 0. But be aware that this routine returns 16 bits in
//	R0, so you must NOT only 16 bits worth.
//
// Side Effects:
//
//	None.
//
//--

// Argument Point stack offsets.

#if 0
#define Byte_Count	  4
#define Start		  8
#define Srca	  12				// Source IP address
#define   Srca0	  12				// First 16-bit word
#define   Srca1	  14				// Second 16-bit word
#define Dsta	  16				// Destination IP address
#define   Dsta0	  16				// First 16-bit word
#define   Dsta1	  18				// Second 16-bit word
#define PtclT	  20				// Protocol type
#endif

//++
// Gen_Checksum - Generate checksum for UDP and TCP.
// Adds in the protocol fields and the IP addresses, then joins Calc_Checksum.
// N.B. We jump directly into Calc_Checksum, so the offsets for Length and Start
// must be the same and we must make sure to save the same registers that
// Calc_Checksum uses.
//--

// Take checksum from elsewhere

//Entry	Gen_Checksum,^M<R2>
int Gen_Checksum(Byte_Count,Start,Srca,Dsta,ptclt)
{
  int R0,R1,R2,R3,R4,R5,R6,R7,R8,R9;
  R0 = Byte_Count;	// Put byte count in R0
  R2 = R0 >> 2;		// Put fullword count in R2
  // Must use byte count rotated +/- 8 bits
  // Must use protocol rotated +/- 8 bits
  // (Because net stuff wants Big Endian byte order)
  R0<<=8;		// Start with byte count
  R0=R0&0x00ffffff;
  R0=R0+	((ptclt&0xff)<<24);	// Add in protocol (only 8 bits wide)
  R0 +=	Srca;		// Add in source addr
  int C = R0 >> 31;
  R0 +=	Dsta + C;		// Add in dest addr (and carry)
  // Join Calc_Checksum routine
  return Calc_Check0(R0,R2,Byte_Count,Start,Srca,Dsta,ptclt);
}

//++
// Calc_Checksum - one's compliment checksum routine.
//--

//Entry	Calc_Checksum,^M<R2>
int Calc_Checksum(Byte_Count,Start,Srca,Dsta,ptclt)
  {
  int R0,R1,R2,R3,R4,R5,R6,R7,R8,R9;
	R0 = 0;			// Start with a zero checksum
	R2 = Byte_Count;
	R2>>=2;		// Put fullword count in R2
					// (and clear Carry)
	return Calc_Check0(R0,R2,Byte_Count,Start,Srca,Dsta,ptclt);
  }

int Calc_Check0(R0,R2,Byte_Count,Start,Srca,Dsta,ptclt) {	// Point where Gen_Checksum joins in
  int *R1,R3,R4,R5,R6,R7,R8,R9,C=R0>>31;
  // R0 and PSW-Carry contain initial 32 bit checksum
  // R2 contains fullword count
  // Start(AP) is pointer to 1st byte
  // Byte_Count(AP) contains byte count
  R1 = Start;		// starting address.
  if ((--R2)>=0) goto clop;			// enter loop
  goto	Odd_Word;		// (no fullwords)
 clop:	R0 = *(R1++)+C;		// add in next fullword and Carry
  C = R0 >> 31;
  if ((--R2)>=0) goto clop;			// get next one
 Odd_Word:
  // Check for extra word
  if ((Byte_Count&2)==0) goto Odd_Byte;
  // was: R2 =	*( ((short *)R1)++);		// get next word
  R2 =	*(short *)R1;		// get next word
  R1 = (long)R1 +2;
  R0=R0+R2+C;			// add it in (and the Carry)
  C = R0 >> 31;
 Odd_Byte:
  // Check for extra byte
  if ((Byte_Count&1)==0) goto Reduce16;
  // was:	R2 =	*( ((char *)R1)++);		// get next byte
  R2 =	*(char *)R1;		// get next byte
  R1=(long)R1+1;
  R0=R0+R2+C;			// add it in (and the Carry)
  C = R0 >> 31;
 Reduce16:
  // We have the sum modulo 2**32-1 (actually: protocol and bytecount
  // from Gen_Checksum are strange, but are eqv mod 2**16-1)
  // Now reduce mod 2**16-1
  R2=R0>>16;		// extract HO word
  //	INSV	0,16,16,R0		// clear HO word
  R0 = R0 & 0xffff;
  R0=R0+R2+C;			// add HO and LO words (and Carry)
  C = R0 >> 31;
  if (test_and_clear_bit(15,&R0)==0) goto Comp; //bbcc		// clear carry out of LO word
  // (and branch if short word result)
  R0++;			// add carry in
  C = R0 >> 31;
  if (C==0)	goto Comp;		// branch if no more carry
  R0++;			// else add final carry in
  // (this INCW can't produce a carry)
 Comp:	// Complement the word
  R0=~R0;
  if  (R0==0) goto	ZSum;
  return R0;
 ZSum:	// Return 0 as FFFF
  R0=~R0;
  return R0;
}

//.Entry	Gen_Checksum,^M<R3,R4>
//
//	R0 = PtclT;		// Start with the protocol code
//	ADDL	Byte_Count,R0	// Add the length
//// Do a byte swap - isn't there an easier way?
//	EXTV	#8,#8,R0,R1		// D C B A	B
//	INSV	R0,#8,#8,R0		// D C A A	B
//	R0 = R1;			// D C A B	B
//	ROTL	#16,R0,R0		// A B D C	B
//	EXTV	#8,#8,R0,R1		// A B D C	D
//	INSV	R0,#8,#8,R0		// A B C C	D
//	R0 = R1;			// A B C D	D
//	R1 = Srca0;		// Get first word of source addr
//	ADDL	R1,R0			// Add it
//	R1 = Srca1;		// Get second word of source addr
//	ADDL	R1,R0
//	R1 = Dsta0;		// Get first word of dest addr
//	ADDL	R1,R0
//	R1 = Dsta1;		// Get second word of dest addr
//	ADDL	R1,R0
//	BRB	Calc_Check0		// Join Calc_Checksum routine
//
//;++
//// Calc_Checksum - one's compliment checksum routine.
//;--
//
//.Entry	Calc_Checksum,^M<R3,R4>
//
//	R0 = 0;			// Start with a zero checksum
//
//Calc_Check0:				// Point where Gen_Checksum joins in
//
//	R4 = Start;		// starting address.
//	R3 = Byte_Count;
//	TSTL	R3			// anything to do?
//	BGTR	l10;			// >0 = yes.
//	return R0;				// 0 count.
//l10:
//	ASHL	#-1,R3,R3		// convert bytes to words.
//	TSTL	R3			// any words to do?
//	BEQL	Odd_BC			// 0 = 1 byte to do....
//	R1 = 0;			// clean up high word of R1
//Clop:
//	MOVW	(R4)+,R1		// get a word
//	ADDL	R1,R0			// longword arith.
//	SOBGTR	R3,Clop			// more to come?
//
//// Check for ODD byte count
//
//ODD_BC:
//	if ((Byte_Count&1)==0) goto Chk_OVFL
//	MOVZBL	(R4),R1			// get ODD byte padded with 0 byte.
//	ADDL	R1,R0
//
//// Add in any overflow
//
//CHk_OVFL:
//	EXTZV	#16,#16,R0,R1		// extract overflow.
//	BEQL	Comp			// 0 = done
//	BICL2	#^XFFFF0000,R0		// AND to 16-bits.
//	ADDL	R1,R0			// add to checksum
//	BRB	Chk_OVFL
//
//// Complement & mask to 16-bits.
//
//Comp:
//	XORW	#^X0FFFF,R0		// Complement to 16-bits.
//	return R0;

//SBTTL	Circular byte queue manipulation routines

// These routines are written in MACRO for two reasons:
//    1) Speed. They are used a lot and should be as fast as possible.
//    2) Precision. Since the circular queues can be modified at AST level, it
//	it critical that the pointers and counters be updated atomically. We
//	can't really trust the BLISS compiler to take care of that.

// Define the format of the circular queue - must match STRUCTURE.REQ

struct _CQ {			// Circular queue
  char	CQ$QUEUE[0];	// Address of the queue structure
  long	CQ$BASE;	// Base address of queue buffer
  long	CQ$END;	// End address of queue buffer
  short	CQ$SIZE;		// Size of the queue
  short	CQ$COUNT;	// Number of bytes on the queue
  long	CQ$ENQP;	// Pointer to first free byte on queue
  long	CQ$DEQP;	// Pointer to first used byte on queue
};


// CQ_Enqueue(CQ,SRC,Scount)
// Enqueue bytes onto a circular queue. Called via $CMKRNL when enqueueing from
// user's system buffer.

#if 0
#define CQ	  4				// Queue header
#define SRC	  8				// Destination address
#define SCOUNT	  12				// Number of bytes
#endif

//ENTRY CQ_Enqueue,^M<R2,R3,R4,R5,R6,R7>
int cq_enqueue(CQ,SRC,SCOUNT)
{
  int R0,R1,R2,R3,R4,R5,R6,R8,R9;
  struct _CQ * R7;
  R7 = CQ;		// Get queue address
  R6  = R7->CQ$END - R7->CQ$ENQP;	// Find space left to end
  if	(R6>SCOUNT) {		// Does he want all we have till end?
    			// Yes - need two moves, then
    MOVC3	(SCOUNT,SRC,R7->CQ$ENQP); // Do the move
    R1 = (long)SRC + SCOUNT;
    R3 = (long)R7->CQ$ENQP + SCOUNT;
    R7->CQ$ENQP = R3;		// Update the queue pointer
    R7->CQ$COUNT+=SCOUNT; // And update the count
    return R0;
  }
					// Here on pointer-wrap case
  MOVC3	(R6,SRC,R7->CQ$ENQP); // Move till end of queue
  R1 = (long)SRC + R6;
  R3 = (long)R7->CQ$ENQP + R6;
  R3 = R7->CQ$BASE;		// Reset pointer to start of queue
  R6 = SCOUNT - R6;// Compute how much we need from Q base
  if (R6>0) {			// Have anything left?
    MOVC3	(R6,(R1),(R3));		// Yes - finish the copy
    R1 = R1 + R6;
    R3 = R3 + R6;
  }
  R7->CQ$ENQP = R3;		// Update the queue pointer
  R7->CQ$COUNT+=SCOUNT; // And update the count
  return R0;				// And done.
}

// CQ_Dequeue(CQ,Dest,Dcount)
// Dequeue bytes from a cirucular queue. Called via $CMKRNL when dequeuing to
// user's system buffer.

#if 0
#define CQ	  4				// Queue header
#define DEST	  8				// Destination address
#define DCOUNT	  12				// Number of bytes
#endif

//ENTRY CQ_Dequeue,^M<R2,R3,R4,R5,R6,R7>
void cq_dequeue(CQ,DEST ,DCOUNT) 
{
  int R0,R1,R2,R3,R4,R5,R6,R8,R9;
  struct _CQ * R7;
  // check all

  R7 = CQ;		// Get queue address
  R6  = R7->CQ$END - R7->CQ$DEQP;	// Find space left to end
  if	(R6>DCOUNT) {		// Does he want all we have till end?
    			// Yes - need two moves, then
    MOVC3	(DCOUNT,R7->CQ$DEQP,DEST); // Do the move
    R1 = (long)R7->CQ$DEQP+DCOUNT;
    R3 = (long)DEST+DCOUNT;
    R7->CQ$DEQP = R1;		// Update the queue pointer
    R7->CQ$COUNT  -= DCOUNT;	// And update the count
    return R0;
  }
 					// Here on pointer-wrap case
  MOVC3	(R6,R7->CQ$DEQP,DEST); // Move till end of queue
  R1 = (long)R7->CQ$DEQP+R6;
  R3 = (long)DEST+R6;
  R1 = R7->CQ$BASE;		// Reset pointer to start of queue
  R6 = DCOUNT - R6;// Compute count we need from Q base
  if (R6>0) {			// Have anything left?
    MOVC3	(R6,(R1),(R3));		// Yes - finish the copy
    R1 = (long)R1+R6;
    R3 = (long)R3+R6;
  }
  R7->CQ$DEQP = R1;		// Update the queue pointer
  R7->CQ$COUNT  -= DCOUNT;	// And update the count
  return R0;				// And done.
}

// CQ_DeqCopy(CQ,Dest,Dcount)
// Same as CQ_Dequeue, except queue pointer/count is not updated.

#if 0
#define CQ	  4				// Queue header
#define DEST	  8				// Destination address
#define DCOUNT	  12				// Number of bytes
#endif

//ENTRY CQ_DeqCopy,^M<R2,R3,R4,R5,R6,R7>
void cq_deqcopy(CQ,DEST ,DCOUNT) 
{
  int R0,R1,R2,R3,R4,R5,R6,R8,R9;
  struct _CQ * R7;
  // check all
  R7=CQ;		// Get queue address
  R6=R7->CQ$END-R7->CQ$DEQP; // Find space left to end
  if	(R6>DCOUNT) {		// Does he want all we have till end?
    			// Yes - need two moves, then
    MOVC3	(DCOUNT,R7->CQ$DEQP,DEST); // Do the move
    R1 = (long)R7->CQ$DEQP+DCOUNT;
    R3 = (long)DEST+DCOUNT;
    //;;	R7->CQ$DEQP = R1;		// Update the queue pointer
    //;;	R7->CQ$COUNT  -= DCOUNT;	// And update the count
  return R0;
  }
 					// Here on pointer-wrap case
  MOVC3	(R6,R7->CQ$DEQP,DEST); // Move till end of queue
  R1 = (long)R7->CQ$DEQP+R6;
  R3 = (long)DEST+R6;
  R1=	R7->CQ$BASE;		// Reset pointer to start of queue
  R6=	DCOUNT-R6;	// Compute count we need from Q base
  if     	(R6>0) {			// Have anything left?
    MOVC3	(R6,R1,R3);		// Yes - finish the copy
    R1 = (long)R1+R6;
    R3 = (long)R3+R6;
  }
  //;;	R7->CQ$DEQP = R1;		// Update the queue pointer
  //;;	R7->CQ$COUNT  -= DCOUNT;	// And update the count
  return R0;				// And done.
}
//END
