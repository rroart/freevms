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
//Title "Ethernet device driver"
//Sbttl "Driver overview"
/*

Module:

          XE_driver

Facility:

          DEC Ethernet device driver

Abstract:

          XE_driver provides upper level protocols with access to the DEC
          Ethernet controller device.  This module has three main routines:
	  XE_init, XE_xmit and XE_receive. XE_init is called at initialization
	  time to setup the ethernet device. XE_xmit is called during run time
	  to send network packets to the network.  It is normally called by the
	  IP_send routine but may be called by any other routine in the network
	  module. XE_receive is always initiated by an AST from the DEC device
	  driver within the VMS system.  The AST's are initially setup by the
	  XE_init routine and subsequently setup by XE_receive itself.

Author:

	  Original author Tim Fallon
	  Previous versions by Kevin Carosso and Ned Freed
	  This version by Vince Fuller, CMU-CSD, Spring/Summer, 1986
	  Copyright (c) 1986,1987, Vince Fuller and Carnegie-Mellon University


Modification History:

	09-Aug-1991	Henry W. Miller		USBR
	Numerous changes:

	Search STARLET for VMS 5.4.
	Print out buffer size in decimal in XE_StartIO().
	In XE_StartDEV(), use MAX_RCV_BUF parameter rather than 4.
	Use LIB$GET_VM_PAGE/LIB$FREE_VM_PAGE rather than LIB$GET_VM/
	LIB$FREE_VM.
	Go NOINT/OKINT during critical phase in XE$XMIT() and XE$ARP_XMIT().

*** Begin CMU change log ***

	02-Feb-1990	Bruce R. Miller		CMU NetDev
	Added a Device_Info table so that this module can be compiled
	into a run-time loadable image, instead of having to be
	linked directly with the IPACP.

	01-Sep-1989	Bruce R. Miller		CMU NetDev
	Restructured code to retry a failed operation a number
	of times before failing.  The number of times to retry
	a read is contained in the variable RETRY_COUNT and is
	settable using the VARIABLE command in the internet.config
	file.

	30-Aug-1989	Bruce R. Miller		CMU NetDev
	Special handling of SS$_BADPARAM return code from XE startup $QIO.

	??-Aug-1989	Bruce R. Miller		CMU NetDev
	Added code to handle SS$_ABORT return code from $QIO read.

	03-Jul-1989	Bruce R. Miller		CMU NetDev
	More verbose description of $ASSIGN failures.

	08-FEB-1989	Dale Moore	CMU-CS/RI
	Now receive only Broadcast packets and not all mutlicast
	packets.

	14-NOV-1988	Dale Moore	CMU-CS/RI
	Added changes suggested by Jerry Lotto.  He mentioned
	getting them from Kevin Carrusso.  They involve dealing
	with brain dead DEQNA's. 

4.1  19-Nov-87, Edit by VAF
	Use $ACPWAKE macro to wakeup ACP.

4.0  31-Jul-87, Edit by VAF
	Enable multicast/broadcast on ARP port (needed for newer devices).

3.9  23-Mar-87, Edit by VAF
	Use dynamic global MAX_PHYSICAL_BUFSIZE when allocating receive
	buffers. Simplify calling sequence of IP_RECEIVE.
	Simplify format of NSQ blocks.

3.8  24-Feb-87, Edit by VAF
	Q_MESSAGE has been flushed. Use QL_FAO (QL$FAO,XQL$FAO).
	Keep track of number of XE restarts.

3.7   6-Feb-87, Edit by VAF
	Change references to "DEUNA/DEQNA" to be "DEC Ethernet" or "XE".

3.6  17-Oct-86, Edit by VAF
	Allow XE_ARP_XMIT to fail recoverably... Sigh.

3.5  28-Oct-86, Edit by VAF
	In XE_XMIT, know about new ARP_CHECK -1 return - indicates that packet
	is being held by ARP routines for later retransmission.

3.4  18-Jul-86, Edit by VAF
	In XE_XMIT, take out bogus retry stuff. REMQUE the Qblk at the start
	of the routine.

3.3  15-Jul-86, Edit by VAF
	Make sure in-progress buffer is deallocated on read error.

3.2  19-Jun-86, Edit by VAF
	Pass device index up to IP.

3.1  11-Jun-86, Edit by VAF
	Allow ARP read to fail.
	Only play the "is decnet there" game the first time that the DEQNA
	is successfully started. Typically, if it "goes away" it is because
	DECNET was slow to restart its part of the device.

3.0  29-May-86, Edit by Dale and Vince
	Rewrite sense-mode code. Use hardware address if "physical" address
	is not valid (i.e. decnet isn't running).

2.9  29-May-86, Edit by VAF
	New NOINT/OKINT scheme. Initialize ARP parameters AFTER device is
	started, so that the physical address is correct.

2.8  13-May-86, Edit by VAF
	Redo error/restart stuff to be consistant, correctly deassign read
	buffers, etc.

2.7   9-May-86, Edit by VAF
	Disable ASTs and do $CANCEL in shutdown routine.
	Issue a $WAKE in restart routine.

2.6  29-Apr-86, Edit by VAF
	Don't issue $WAKE calls in here.

2.5  15-Apr-86, Edit by VAF
	Changes for new, general ARP interface.
	Treat addresses as 6-byte long strings, not as two partial words.
	Remove a lot of byteswapping of addresses.

2.4   7-Apr-86, Edit by VAF
	New logging stuff. Log physical address in receive AST routines.
	Use Q_MESSAGE not LOG_OUTPUT when writing log info.

2.3  31-Mar-86, Edit by VAF
	Various changes, fixes to 2.2 change.

2.2   5-Mar-86, Edit by VAF
	Add code in XE_RECEIVE to restart the DEQNA when it gets wedged.

2.1  21-Feb-86, Edit by VAF
	Put ARP origination code in here. *NB* this ARP code is temporary
	and is not very general. It was put here so that I could change
	the interface between IP and the device driver to its eventual final
	state. **This code will be replaced by a more general ARP mechanism
	someday (hopefully, soon) ***
	Flush "known hosts" table.

*** End CMU change log ***

          1.0 - Original release
          1.1 - Removed buffer copies.  Higher level protocols now sends a
                buffer that has space in front to put device control info.
                Also implemented the XE_hdr_offset to allow for different
                device control information on store-and-forward buffers.
	  1.2 - Assign ethernet packet type to be compatiable with berkely
		4.2 unix.
	  2.0 - Converted from Interlan driver to DEUNA/DEQNA driver.
	  2.01 - Add ARP responder (Rick Watson U.Texas)
*/


//Sbttl   "Module definitions"

MODULE XE_DRIVER(IDENT="5.0a",LANGUAGE(BLISS32),
		 ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
				 NONEXTERNAL=LONG_RELATIVE),
		 LIST(EXPAND,TRACE,NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
		 OPTIMIZE,OPTLEVEL=3,ZIP)=
!!!HACK!!// Add code to shut-down an interface permanently...
!!!HACK!!// Make sure errors are handled correctly.

{

!LIBRARY "SYS$LIBRARY:STARLET";	// VMS system defintions
#include "SYS$LIBRARY:LIB";	// VMS system defintions

#include "CMUIP_SRC:[CENTRAL]NETTCPIP";		// Required for UDP check
#include "CMUIP_SRC:[central]NETXPORT";		// VMS specifics
#include "CMUIP_SRC:[central]NETCOMMON";	// CMU-OpenVMS/IP
#include "CMUIP_SRC:[central]NETVMS";		// VMS specifics
#include "CMUIP_SRC:[central]NETCONFIG";	// Device interface specs.
#include "CMUIP_SRC:[central]NETDEVICES";	// Helpfull macros...

#include "XEDRV";
#include "XEARP";

extern

// NETMACLIB.OBJ
    Time_Stamp,
    SwapBytes,

// NETDEVICES.OBJ
     ASCII_HEX_BYTES,

// XEDRV_ARP.BLI

 VOID    XEARP$DEV_INIT,
    XEARP$CHECK,
 VOID    XEARP$INPUT;

FORWARD ROUTINE
 void    XE_Receive,
    XE_ArpRcv :  NOVALUE;

BIND
    XE_BROADCAST = UPLIT (BYTE (REP 6 OF (%X"FF")));

MACRO
    // Description of this device
    XE_description = "Digital Ethernet Card"%;

static signed long
    // String descriptor used to hold description
    XE_descriptor : VECTOR[2];


//SBTTL "Declare the device information block used to describe entry points."

static signed long
    // DRV$Device_Info is a list of everything we want the IPACP
    // to know about us...  Initialized by ROUTINE DRV$TRANSPORT_INIT.
    DRV$Device_Info : Device_Info_Structure;

signed long
    // The IPACP_Interface tells us all about the IPACP.  It gives us
    // entry points, literals and global pointers.  See NETDEVICES.REQ
    // for a complete explaination of this structure.
    // Note: This pointer must be named "IPACP_Interface"
    struct IPACP_Info_Structure * IPACP_Interface;



FORWARD ROUTINE
 void    XE_Shutdown;

MACRO XE$ERR(XE_Int)
    {
    DRV$OPR_FAO(%REMAINING);
    XE_Shutdown(XE_Int,true);
    }
    %;



//SBTTL "Start asynchronous I/O on the DEC ethernet"

XE_StartIO ( struct XE_Interface_Structure * XE_Int)
    {
    signed long
	RC,
	struct XERCV_QB_structure * Buff,
	struct XE_ARP_STRUCTURE * ARbuf;
    BIND
	rcvhdrs = XE_Int->XEI$rcvhdrs : BLOCKVECTOR[MAX_RCV_BUF,XERCV_LEN]
					 FIELD(XE_RECV),
	XE_chan = XE_Int->XEI$IO_Chan,
	XAR_chan = XE_Int->XEI$ARP_IO_chan;

// Supply four receive buffers to device controller

    DRV$OPR_FAO ("XEDRV: MPBS = !SL",DRV$MAX_PHYSICAL_BUFSIZE);

    XE_Int->XEI$curhdr = 0;
    for (I=0;I<=(MAX_RCV_BUF-1);I++)
	{	// Get buffer, put on Q and issue IO$_READVBLK function
	Buff = DRV$Seg_get(DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len));
	INSQUE ( Buff , XE_Int [ XEI$recv_Qtail ] );
	Buff = Buff + XE_hdr_offset;
	RC = $QIO(chan=.XE_chan, EFN=ASTEFN, IOSB=Buff->XERCV$vms_code,
		  Func = IO$_READVBLK,
		  P1 = Buff->XERCV$data,
		  P2 = DRV$MAX_PHYSICAL_BUFSIZE,
		  P5 = rcvhdrs[I,XERCV$buf],
		  astadr = XE_receive, astprm = XE_Int);
	if (RC != SS$_Normal)
	    {     // Error issuing set receive buffer command
	    XE$ERR(XE_Int,"XE queue read request failure, RC=!XL",RC);
	    Return 0;
	    }
	};

// get ARP buffer and issue a receive qio

    ARbuf = DRV$Seg_get(XE_ARP_LEN*4);
    XE_Int [ XEI$ARP_buffer ] = ARbuf;
    RC = $QIO(	chan = XE_Int [ XEI$ARP_IO_chan ], EFN = ARPEFN,
		IOSB = ARbuf [ ar_ios0 ],
		Func = IO$_READVBLK,
		P1 = ARbuf->ar_data, P2=ARP_MAX_LEN*4,
		P5 = ARbuf->phys$1,
		astadr = XE_ArpRcv, astprm = XE_Int);
    if (RC != SS$_Normal)
	{
	XE$ERR(XE_Int,"XE ARP queued read failure, RC=!XL",RC);
	return 0;
	};

// Indicate that I/O has been started

    XE_Int->XEI$IO_queued = TRUE;
    return -1;
    }



//SBTTL "Start the device"

XE_StartDev ( XE_Int , setflag , setaddr )

// This routine initializes the ethernet device to receive IP packets.
// Issue start command to the controller.
// Accepts:
//   XE_INT	EtherNet interface info block
//   SETFLAG	TRUE if device physical address needs to be set
//   SETADDR	If SETFLAG, then physical address to set
// Returns:
//   0 (false) on failure, device not started
//  -1 (true) on success, device ready, reads queued

    {
    MAP
	struct XE_Interface_Structure * XE_Int;
    signed long
	RC,
	IOS: XE_iosb_structure,
	Setup: XE_setup_structure,
	Paramdescr: XE_sdesc_structure,
	plen;

// Build the nasty setup block required by the ethernet device

    Setup->XE$c_pcli_bus      = NMA$C_PCLI_BUS;
    Setup->XE$l_buffer_length = DRV$MAX_PHYSICAL_BUFSIZE;
    Setup->XE$c_pcli_bfn      = NMA$C_PCLI_BFN;
    Setup->XE$l_number_buff   = MAX_RCV_BUF;
    Setup->XE$c_pcli_pad      = NMA$C_PCLI_PAD;
    Setup->XE$l_padding       = NMA$C_STATE_OFF;
    Setup->XE$c_pcli_pty      = NMA$C_PCLI_PTY;
    Setup->XE$c_pcli_prm      = NMA$C_PCLI_PRM;
    Setup->XE$l_promiscuous   = NMA$C_STATE_OFF;
    Setup->XE$c_pcli_dch      = NMA$C_PCLI_DCH;
    Setup->XE$l_data_chain    = NMA$C_STATE_OFF;
    Setup->XE$c_pcli_con      = NMA$C_PCLI_CON;
    Setup->XE$l_control_mode  = NMA$C_LINCN_NOR;


// Change XE$W_mca_mode to NMA$C_LINMC_SET if you want IP broadcasts.

    Setup [XE$c_pcli_mca]     = NMA$C_PCLI_MCA;
    Setup [XE$W_MCA_Length]   = 2 + XE_ADR_SIZE;
    Setup [XE$W_MCA_MODE]     = NMA$C_LINMC_CLR;
    CH$MOVE (XE_ADR_SIZE, XE_BROADCAST, Setup [XE$L_MCA_Address]);

// If he wants us to set the physical address, then do so.

    if (setflag)
	{
	Setup->XE$c_pcli_pha = NMA$C_PCLI_PHA;
	Setup->XE$w_pcli_phlen = XE_ADR_SIZE+2;
	Setup->XE$w_pcli_phmode = NMA$C_LINMC_SET;
	CH$MOVE(XE_ADR_SIZE,CH$PTR(setaddr),CH$PTR(setup->XE$l_pcli_phaddr));
	plen = Setup->XE$setup_pha_end - Setup;
	}
    else
	plen = Setup->XE$setup_end - Setup;

// Set up for IP protocol on this channel

    Setup->XE$l_protocol      = XE_IP_type;	// IP
    Swapbytes(1,Setup->XE$l_protocol);

    Paramdescr->XE$setup_length = plen;
    Paramdescr->XE$setup_address = Setup;

// Issue the startup command to controller

    RC = $QIOW (chan = XE_Int->XEI$IO_chan, EFN = 1, IOSB = ios,
		Func = IO$_SETMODE+IO$M_CTRL+IO$M_STARTUP, P2 = Paramdescr);
    if (NOT( (RC == SS$_Normal) and (IOS->XE$vms_code == SS$_Normal) ))
	{
	if (IOS->XE$vms_code == SS$_BADPARAM)
	   XE$ERR(XE_Int,"XE startup failure, RC=!XL,VMS_code=!XL,Param=!XL",
	       RC,IOS->XE$VMS_Code,IOS[1,0,32,0])
	else
	   XE$ERR(XE_Int,"XE startup failure, RC=!XL,VMS_code=!XL,Xfer size=!SL",
	       RC,IOS->XE$VMS_Code,IOS->XE$Tran_size);
	return 0;
	};

// Modify the setup block for the arp responder

    Setup->XE$l_protocol = XE_ARP_type;
    Setup->XE$W_mca_mode = NMA$C_LINMC_SET;

    Swapbytes(1,Setup->XE$l_protocol);
    Paramdescr->XE$setup_length = Setup->XE$setup_end - Setup;
    Paramdescr->XE$setup_address = Setup;

// Issue the startup command to controller

    RC = $QIOW (chan = XE_Int->XEI$ARP_IO_chan, EFN = 1, IOSB = ios,
		Func = IO$_SETMODE+IO$M_CTRL+IO$M_STARTUP, P2 = Paramdescr);
    if (NOT( (RC == SS$_Normal) && (IOS->XE$vms_code == SS$_Normal) ))
	{	// Startup command failed
	XE$ERR(XE_Int,
	       "XE ARP startup failure, RC=!XL,VMS_code=!XL,Xfer size=!SL",
	       RC,IOS->XE$VMS_Code,IOS->XE$Tran_size);
	return 0;
	};

// Everything OK - return TRUE value

    return -1;
    }

XE_SenseDev( struct XE_Interface_Structure * XE_Int,
		     struct XE_Addrs_structure * phaddr,
		     struct XE_Addrs_structure * hwaddr,
		     online)
!
// Read status of device.
//   phaddr	pointer to area to store "physical" (decnet) address
//   hwaddr	pointer to area to store hardware address
//   online	true if device has been started successfully.
!
    {
    signed long
	RC,
!!!HACK!!// What's wrong with XE_sense_blk?
!	Sense: 	XE_sense_blk,	
	Sense:	$BBLOCK [512],
	Paramdescr: XE_sdesc_structure,
	IOS: XE_iosb_structure;

// Get info from controller

    Paramdescr->XE$setup_length = %ALLOCATION (Sense);
    Paramdescr->XE$setup_address = Sense;

    rc = $QIOW (chan = XE_Int->XEI$IO_chan, EFN = 1, IOSB = ios,
		Func = IO$_SENSEMODE+IO$M_CTRL, P2 = Paramdescr);
    if (NOT ((rc eql ss$_normal) && (IOS->XE$vms_code)))
	{			// Statistics call failed
	XE$ERR(XE_Int,
	       "XE sense mode QIOW failure, RC=!XL,VMS_Code=!XL,Xfer size=!SL",
	       RC,IOS->XE$VMS_code,IOS->XE$tran_size);
	return 0;
	};

    Paramdescr [XE$SETUP_LENGTH] = IOS [XE$Tran_Size];

    while (Paramdescr [XE$SETUP_LENGTH] > 0)
	{
	BIND
	    Param = Paramdescr [XE$SETUP_ADDRESS] : $BBLOCK FIELD (XE_Sense);

	Paramdescr [XE$SETUP_LENGTH] = Paramdescr [XE$SETUP_LENGTH] - 
				XE_Param_Size (Param);
	Paramdescr [XE$SETUP_ADDRESS] = Paramdescr [XE$SETUP_ADDRESS] +
				XE_Param_Size (Param);

	SELECTONEU Param->XE_Sense_Param OF
	SET
	[NMA$C_PCLI_PHA]:
	    CH$MOVE(XE_ADR_SIZE,CH$PTR(Param->XE_Sense_String),
		    CH$PTR(phaddr));

	[NMA$C_PCLI_HWA]:
	    CH$MOVE(XE_ADR_SIZE,CH$PTR(Param->XE_Sense_String),
		    CH$PTR(hwaddr));
	TES
	};

// See if the device is online
!Ignore the timeout bit for DEQNA's

!!!HACK!!// there may be a problem with XE$cmd_status.
    online = (IOS->XE$cmd_status && %X"0FF00") == XM$M_STS_ACTIVE;

// Return success

    return -1;
    }

//SBTTL "Start device - top-level"

XE_StartAll ( XE_Int , restart )
    {
    MAP
	struct XE_Interface_Structure * XE_Int;
    signed long
	online,
	struct Device_Configuration_Entry * dev_config,
	struct XE_addrs_structure * addrs,
	hwaddr: XE_addrs_structure,
	phaddr: XE_addrs_structure,
	struct XE_addrs_structure * useaddr;

// Point at out current idea of the device address

    addrs = XE_Int [ XEI$phy_addr ];

// Try to start the device

    if (NOT XE_StartDev(XE_Int,0,0))
	return 0;

// Only check for DECNET and set ARP address the first time that we successfully
// start the device up. Otherwise, when (not IF) the first error causes the
// device to be shut-off, we run the risk of wedging everything.

    if (XE_Int->XEI$XE_started)
	{

// If we've been started before, just make sure that our address agrees.
// If not, it probably means that DECNET was slow to get started and we
// need to wait for it.

	if (NOT XE_SenseDev(XE_Int,phaddr,hwaddr,online))
	    return 0;
	if (XE_Int->XEI$XE_decnet)
	    useaddr = phaddr
	else
	    useaddr = hwaddr;
	IF CH$NEQ(XE_ADR_SIZE,CH$PTR(useaddr),
		  XE_ADR_SIZE,CH$PTR(XE_Int->XEI$phy_addr)) THEN
	    {
	    DRV$OPR_FAO("XE restart failed - address mismatch");
	    XE_Shutdown(XE_Int,true);
	    return 0;
	    };
	}
    else
	{

// Get device address

	if (NOT XE_SenseDev(XE_Int,phaddr,hwaddr,online))
	    return 0;

// Remember that it was started at least once

	    XE_Int->XEI$XE_started = TRUE;

// If no "physical" (DECNET) address, then restart device and set address

	    IF CH$EQL(XE_ADR_SIZE,CH$PTR(phaddr),
		      XE_ADR_SIZE,CH$PTR(XE_BROADCAST)) THEN
		{
		XE_Shutdown(XE_Int,0);
		if (NOT XE_StartDev(XE_Int,true,hwaddr))
		    return 0;
		useaddr = hwaddr;
		XE_Int->XEI$XE_decnet = FALSE;
		}
	    else
		{		// Remember that we had DECNET
		XE_Int->XEI$XE_decnet = TRUE;
		useaddr = phaddr;
		};

// Check against address specified in CONFIG - report if different

	    IF CH$NEQ(XE_ADR_SIZE,CH$PTR(useaddr),
		      XE_ADR_SIZE,CH$PTR(addrs)) THEN
		{
		IF CH$NEQ(XE_ADR_SIZE,CH$PTR(addrs),
			  XE_ADR_SIZE,CH$PTR(UPLIT(0,0))) THEN
		    {
		    signed long
			DESC$STR_ALLOC(newstr,50);

		    ASCII_Hex_Bytes(newstr,XE_ADR_SIZE,useaddr,
				    newstr->DSC$W_LENGTH);
		    DRV$OPR_FAO("XE address mismatch, using address: !AS",newstr);
		    };
		CH$MOVE(XE_ADR_SIZE,CH$PTR(useaddr),CH$PTR(addrs));
		};

// Initialize ARP parameters for this device

!!!HACK!!// Trim this down...
    	XEARP$DEV_INIT(XE_Int,AR$HRD_ETHER,XE_IP_TYPE,addrs,XE_ARP_HLEN,1,0);
	};

// Start I/O on the device

    XE_StartIO ( XE_Int );

// Say that this device is finally ready
    dev_config = XE_Int [XEI$Dev_config];
//    dev_config [dc_online] = online;
    dev_config [dc_online] = True;

    return True;
    }



LITERAL
    EMPTY_QUEUE = 3 : UNSIGNED(8);

LITERAL
    XE_RESTART_TIME = 5*100;		// How long to leave shutdown

void XE_FreeBufs ( struct XE_Interface_Structure * XE_Int ) (void)

// AST routine to deallocate XE read buffers on error.
// Scheduled by shutdown routine.

    {
    signed long
	struct XERCV_QB_Structure * BUFF;

// Flush IP buffers

    while (REMQUE ( XE_Int [ XEI$recv_Qhead ] , BUFF ) != EMPTY_QUEUE)
	DRV$Seg_Free ( DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len + IOS_len) , BUFF );

// Release the ARP buffer

    if ((BUFF=XE_Int->XEI$ARP_buffer) != 0)
	{
	DRV$Seg_Free ( XE_ARP_LEN*4 , BUFF );
	XE_Int [ XEI$ARP_buffer ] = 0;
	};

// Say that this has been done.

    XE_Int [ XEI$need_2_free ] = FALSE;
    }

void XE_Shutdown ( XE_Int , restart ) (void)
!
// Shut the device controller down. Issue shutdown command to controller
// if online, set offline, general cleanup.
// 

    {
    MAP
	struct XE_Interface_Structure * XE_Int;
    signed long
	RC,
	now,
	struct Device_Configuration_Entry * dev_config,
	IOS : XE_iosb_structure;

// Disallow ASTs

    DRV$NOINT;

    now =  Time_Stamp();

// Set device offline.
    dev_config = XE_Int [XEI$Dev_config];
    dev_config [dc_online] = False;

// If it should be restarted, then say when

    if (restart != 0)
	{
	XE_Int [ XEI$restart_time ] = now + XE_RESTART_TIME;
	DRV$Device_Error();
	}
    else
	XE_Int [ XEI$restart_time ] = 0;

// Shutdown the IP channel

    if (XE_Int->XEI$IO_chan != 0)
	{
	$CANCEL(chan=XE_Int->XEI$IO_chan);
	RC = $QIOW( chan=XE_Int->XEI$IO_chan, EFN=1, IOSB=IOS,
		    Func=IO$_SETMODE+IO$M_CTRL+IO$M_SHUTDOWN);
	if ((RC != SS$_Normal) || (IOS->XE$VMS_CODE != SS$_NORMAL))
	    {
	    DRV$Warn_FAO("XE shutdown QIOW failure, EC = !XL",RC);
	    DRV$Fatal_FAO("XE shutdown failure, EC = !XL",IOS->XE$VMS_CODE);
	    };
	};

// Shutdown the ARP channel

    if (XE_Int->XEI$ARP_IO_chan != 0)
	{
	$CANCEL(chan=XE_Int->XEI$ARP_IO_chan);
	RC = $QIOW( chan=XE_Int->XEI$ARP_IO_chan, EFN=1, IOSB=IOS,
		    Func=IO$_SETMODE+IO$M_CTRL+IO$M_SHUTDOWN);
	if ((RC != SS$_Normal) || (IOS->XE$VMS_CODE != SS$_NORMAL))
	    {
	    DRV$Warn_FAO("XE ARP shutdown QIOW failure, EC = !XL",RC);
	    DRV$Fatal_FAO("XE ARP shutdown failure, EC = !XL",IOS->XE$VMS_CODE);
	    RETURN;
	    };
	};

// Schedule AST to deallocate all buffers

    if (XE_Int->XEI$IO_queued)
	{
	XE_Int->XEI$IO_queued = FALSE;
	XE_Int->XEI$need_2_free = TRUE;
	$DCLAST(astadr = XE_FreeBufs, astprm = XE_Int);
	};

// Allow AST's again
    DRV$OKINT;

    }

//SBTTL	"Ethernet driver check routine"
// Routine to call whenever the device is offline and shouldn't be.

XE$CHECK ( struct Device_Configuration_Entry * dev_config )
								: NOVALUE (void)
    {
    signed long
	now;
    BIND
	XE_Int = dev_config [ dc_dev_interface ] : REF XE_Interface_Structure;


    now = Time_Stamp();
    if (now > XE_Int->XEI$restart_time)
	{
	IF (NOT XE_Int->XEI$need_2_free) AND
	   XE_StartAll(XE_Int,true) THEN
	    {		// Device restarted OK
	    XE_Int->XEI$restart_time = 0;
	    XE_Int->XEI$retry_count = 0;
	    XE_Int->XEI$restart_count = XE_Int->XEI$restart_count + 1;
	    DRV$OPR_FAO("XE (DEC ENET) restarted, count = !SL",
		    XE_Int->XEI$restart_count);
!!!HACK!!// Is it OK to move this to after $ACPWAKE
!	    Dev_attn = Dev_attn-1;
	    DRV$ACPWAKE;		// Special event...
	    // Return -1 to decrement the # of devices needing attention.
	    return -1
	    }
	else			// Wait a while and try again...
	    XE_Int->XEI$restart_time = now + XE_RESTART_TIME;
	};

// Return 0 since device still needs attention.
	0
    }

//Sbttl   "Ethernet driver init routine"
/******************************************************************************

   Function:

      Initialize the DEC Ethernet Device
        1.  Assign the device.
        2.  Issue the startup command to configure, self-test, and put
            the thing online.
	3.  Do an IO$_SENSEMODE to obtain hardware address.
        4.  Check hardware address with configuration file.
        5.  If address is incorrect set it to the configuration file value.
        6.  Get 4 buffers and issue 4 IO$_READVBLK functions with AST's.

   Inputs:

	dev_config : pointer to address of the device configuration entry

   Outputs:

      XE_Int is allocated and initialized.
      Device channel number is stored in XE_Int and online bit is set

*******************************************************************************
*/

!!!HACK!!// Should there be XE$Init and XE$Init_Controller?
!!!HACK!!//  XE$Init_Class and XE$Init_Port

XE$init ( struct Device_Configuration_Entry * dev_config ,
void 		  IPACP_Int, max_retry, MPBS) (void)
    {
    EXTERNAL ROUTINE
	XEARP$INIT,
	LIB$GET_VM : ADDRESSING_MODE(GENERAL),
	LIB$GET_VM_PAGE : ADDRESSING_MODE(GENERAL);
    signed long
	RC,
	rcvhdrs,
	struct XE_Interface_Structure * XE_Int,
	XE_Chan,
	XAR_Chan;

// Setup the global
    IPACP_Interface = IPACP_Int;

// Hold AST's until done here
    DRV$NOINT;

// Initialize the ARP module
    XEARP$INIT();

// Assign Ethernet Controller
    IF NOT (rc=$Assign (devnam = dev_config->dc_devname, chan = XE_chan))
    THEN     // Ethernet controller assign failed
	{
	DRV$Fatal_FAO("XE $ASSIGN failure (dev="!AS"), EC = !XL",
		    dev_config->dc_devname,rc);
	RETURN
	};

//  Assign the channel for the arp responder
    IF NOT (rc=$Assign(devnam = dev_config->dc_devname,chan=XAR_chan))
    THEN     // Ethernet controller assign failed
	{
	DRV$Fatal_FAO("XE $ASSIGN failure (dev="!AS"), EC = !XL",
		    dev_config->dc_devname,rc);
	RETURN
	};

// Allocate and Initialize the XE controller block
    // Allocate VM
    !!!HACK!!// When are we going to deallocate this?  Ever?
//    if (NOT (LIB$GET_VM(%REF(XE_Interface_size*4),XE_Int)))
    if (NOT (LIB$GET_VM_PAGE(%REF(((XE_Interface_size * 4) / 512) + 1),XE_Int)))
	{	// Couldn't allocate memory for controller block
	DRV$Fatal_FAO("XE LIB$GET_VM failure (dev="!AS"), EC = !XL" ,
		    dev_config->dc_devname,rc);
	RETURN
	};

    // Zero out the memory block
    CH$FILL(%CHAR(0),XE_Interface_size*4,XE_Int);

    // Fill in the blanks...
    XE_Int->XEI$IO_chan = XE_chan;
    XE_Int->XEI$ARP_IO_chan = XAR_chan;
    XE_Int->XEI$Phy_Size = 6;	// Ether addrs are 6 bytes long (48 bits)
    XE_Int->XEI$max_retry = max_retry;	// Maximum # of conseq. retries allowed
    XE_Int->XEI$MPBS = MPBS;	// Maximum Physical Buffer Size
    XE_Int->XEI$Flags = 0;	// Just making sure...

    // Set-up the receive queue
    XE_Int->XEI$recv_Qhead = XE_Int->XEI$recv_Qhead;
    XE_Int->XEI$recv_Qtail = XE_Int->XEI$recv_Qhead;

    // set double-link between XE_Int and dev_config blocks
    XE_Int [ XEI$dev_config ] = dev_config;
    dev_config [ dc_dev_interface ]  = XE_Int;

// Allocate and Initialize the XE receive headers
    // Allocate VM
    !!!HACK!!// When are we going to deallocate this?  Ever?
//    if (NOT (LIB$GET_VM(%REF(MAX_RCV_BUF*XERCV_LEN*4),rcvhdrs)))
    if (NOT (LIB$GET_VM_PAGE(%REF(((MAX_RCV_BUF*XERCV_LEN*4)/512)+1),rcvhdrs)))
	{	// Couldn't allocate memory for receive headers
	DRV$Fatal_FAO("XE LIB$GET_VM failure (dev="!AS"), RC=!XL",
		    dev_config->dc_devname,rc);
	RETURN
	};

    XE_Int->XEI$rcvhdrs = rcvhdrs;
    XE_Int->XEI$curhdr = 0;	// current ethernet header to use

// Start the device
    XE_Startall(XE_Int,false);

// Fill in the dev_config ifTable fields
    {
    BIND
	desc = dev_config->dcmib_ifDescr : $BBLOCK[8];

//    dev_config->dcmib_ifIndex = -1;	// Filled by IPACP

    desc->DSC$W_LENGTH = XE_Descriptor[0];
    desc->dsc$A_POINTER = XE_Descriptor[1];

    dev_config->dcmib_ifType = 6;		// EtherNet
    dev_config->dcmib_ifMTU = DRV$MAX_PHYSICAL_BUFSIZE;
    dev_config->dcmib_ifSpeed = 10000000;	// bits/second

    dev_config->dcmib_ifPAsize = XE_Int->XEI$Phy_Size;
    dev_config->dcmib_ifPhysAddress = XE_Int->XEI$Phy_Addr;

    dev_config->dcmib_ifAdminStatus = 2;	// start
//    dev_config->dcmib_ifOperStatus = 2;		// start

    dev_config->dcmib_ifLastState = 0;
    dev_config->dcmib_ifInOctets = 0;
    dev_config->dcmib_ifInUcastPkts = 0;
    dev_config->dcmib_ifInErrors = 0;
    dev_config->dcmib_ifInUnknownProtos = 0;
    dev_config->dcmib_ifOutOctets = 0;
    dev_config->dcmib_ifOutUcastPkts = 0;
    dev_config->dcmib_ifOutNUcastPkts = 0;
    dev_config->dcmib_ifOutDiscards = 0;
    dev_config->dcmib_ifOutErrors = 0;
    dev_config->dcmib_ifOutQLen = 0;
    }

// Ok to take AST's again
    DRV$OKINT;

    }


//Sbttl   "Ethernet driver xmit"
/******************************************************************************

Function:

     This routine is called by the higher level protocol to transmit a
     datagram to the DEC Ethernet controller.  All information about a datagram
     is found on the Net_send_Q for this device.  Each Q entry will be 
     processed and deleted from the Q.

Inputs:

     dev_config - dev_config table entry for this device.
     Net_send_queue on the dev_config table entry for this device

Outputs:

     None.

*******************************************************************************
*/

MACRO
    XE_LOG(MSG,IPADDR,HWADDR)
    {
    signed long
	STR_DESC : VECTOR [2];
    EXTERNAL ROUTINE
	XEARP$LOG : NOVALUE;

    STR_DESC[0] = %CHARCOUNT(MSG);
    STR_DESC[1] = UPLIT(MSG);

    XEARP$LOG(STR_DESC,IPADDR,6,HWADDR);
    } %;

XE$xmit ( struct Device_Configuration_Entry * dev_config )
								: NOVALUE (void)
    {
    LABEL
	X;
    signed long
	RC,
	IOS: XE_IOSB_structure,
	struct BLOCK * QB[] Field(QB_net_send),
	Addrs: XE_addrs_structure,
	struct XESND_Structure * Sbuf,
	struct IP_Structure * IPHDR,
	Sen_size,
	ARstat,
	AddrCheck,
	xchan;
    BIND
	XE_Int = dev_config [ dc_dev_interface ] : REF XE_Interface_Structure;

    DRV$NOINT;
// Check if a request is on the Net_send_Q for this device

    if ((REMQUE(Dev_config->dc_send_Qhead,QB)) == Empty_Queue)
	RETURN;			// The Q is empty

    ARstat = 0;			// Assume we will deallocate the packet

// Make sure device is online

    if (NOT Dev_config->dc_online)
	{			// Device is offline
	DRV$XQL_FAO(LOG$PHY,"!%T XE device !XL offline (xmit)!/",0,XE_Int);
	}
    else
X:	{

// Position for Ethernet device header

	Sbuf = QB->NSQ$Data - XE_hdr_len;
	IPHDR = QB->NSQ$Data;
// IPH$TTL in the swapped header is really IPH$Protocol
	IF (IPHDR->IPH$TTL Eql UDP_Protocol) AND
	   (IPHDR->IPH$Dest Eqlu ((NOT DEV_CONFIG->DC_IP_NETMASK) OR
		DEV_CONFIG->DC_IP_NETWORK)) THEN
	    {
	    CH$MOVE(XE_ADR_SIZE, CH$PTR(XE_Broadcast), CH$PTR(ADDRS));
	    ARstat = 1;
	    }
	else
	    ARstat = XEARP$CHECK(XE_Int,QB->NSQ$IP_DEST,ADDRS,QB);
	if (ARstat > 0)
	    {		// Have an address

// Fill in Ethernet header information

	    if ($$LOGF(LOG$PHY))
		XE_LOG("XE IP xmit",QB->NSQ$IP_DEST,ADDRS);

	    CH$MOVE(XE_ADR_SIZE,CH$PTR(addrs),Sbuf->XESND$dest);
	    Sbuf->XESND$type = XE_IP_type;
	    Swapbytes(1,Sbuf->XESND$Type);

	    xchan = XE_Int->XEI$IO_chan;

// Send packet to controller

!!!HACK!!// What's the EFN for?
	    Sen_size = MAX(QB->NSQ$Datasize,XE_MINSIZE);
	    RC = $QIOW(	CHAN	= xchan,
			EFN	= 1,
			FUNC	= IO$_WRITEVBLK,
			IOSB	= IOS,
			P1	= QB->NSQ$Data,
			P2	= Sen_size,
			P5	= Sbuf);

// Check for $QIO error

	    if (NOT (RC))
		{
		XE$ERR(XE_Int,"XE $QIO error (send),RC=!XL",RC);
		LEAVE X;
		};

// Check for device driver error

	    if (IOS->XE$vms_code != SS$_Normal)
		{
		XE$ERR(XE_Int,"XE driver error (send),VMS_code=!XL",
		       IOS->XE$vms_code);
		LEAVE X;
		};

// Check for controller error

	    if ((IOS->XE$cmd_status && %X"0FF00") != XM$M_STS_ACTIVE)
		{
		DRV$XLOG_FAO(LOG$PHY,"%T XE command error !XW!/",
				0,IOS->XE$cmd_status);
		LEAVE X;
		};
	    };
	};	// End of block X:

// Delete buffer and release QBlk if ARP didn't claim them and deletable

    if (ARstat GEQ 0)
	{
	if (QB->NSQ$delete)
	    DRV$Seg_free(QB->NSQ$del_buf_size,QB->NSQ$del_buf);
	DRV$Qblk_free(QB);
	};
    DRV$OKINT;
    }

//SBTTL   "Ethernet driver recv"
/******************************************************************************

Function:

     This routine is an AST interrupt routine.  It is started when the
     Ethernet controller system driver has received a packet and issues an
     AST to this routine.  The packet is passed to the higher protocol for
     processing.  A new buffer is rented, put on the receive Queue and then
     passed to the device for subsequent packet arrivals.

Inputs:

     XE_Int : address of ethernet interface information block

Outputs:

     Calls IP_Receive to deliver the datagram.

*******************************************************************************
*/

void XE_receive ( struct XE_Interface_Structure * XE_Int ) (void)
    {
    signed long
	rcvix,
	struct XERCV_QB_structure * Rbuf,
	struct XERCV_QB_Structure * Nrbuf,
	struct Device_Configuration_Entry * dev_config,
	RC,
	IRC,
	Error_Flag  = 0;
    BIND
	rcvhdrs = XE_Int->XEI$rcvhdrs : BLOCKVECTOR[MAX_RCV_BUF,XERCV_LEN]
					 FIELD(XE_RECV);

    dev_config = XE_Int [ XEI$dev_config ];

// Set flag indicating interrupt in progress

    DRV$AST_in_Progress = True;

// If device not online, then give message and punt

    if (NOT dev_config->dc_online)
	{

!~~	DRV$OPR_FAO("XE receive AST when offline");
	DRV$AST_in_Progress = False;
	RETURN;
	};

// Get first input packet off of the queue
!!!HACK!!// What if the first packet wasn't the one which $QIO returned?
    REMQUE(XE_Int->XEI$recv_Qhead,Rbuf);
    Rbuf = Rbuf + XE_hdr_offset;
    rcvix = XE_Int->XEI$curhdr;

    if ((RC = Rbuf->XERCV$vms_code) != SS$_Normal)
	{
!	Error_Flag = 1;
	SELECT RBUF->XERCV$VMS_CODE OF
	    SET
	    [SS$_ABORT] :
		DRV$OPR_FAO("XE abort, cmd_status=!XW, error_summary=!XB",
  			RBUF->XERCV$cmd_status,RBUF->XERCV$error_summary);
	    [SS$_DATAOVERUN,SS$_TIMEOUT] :
		{
		DRV$OPR_FAO("XE read error (timeout/overrun), RC=!XL",RC);
		DRV$OPR_FAO("DEBUG - bytes received=!XL",Rbuf->XERCV$tran_size);
		};
	    [OTHERWISE] :
		DRV$OPR_FAO("XE: VMS Error, cmd_status=!XW, error_summary=!XB",
  			RBUF->XERCV$cmd_status,RBUF->XERCV$error_summary);
	    TES
	};

    !Ignore the timeout bit for DEQNA's
    if ((Rbuf->XERCV$cmd_status  && %X"0FF00") != XM$M_STS_ACTIVE)
	{
	Error_Flag = 1;
	// Error from board
	DRV$OPR_FAO("XE status error.  Status = !XL",
		     Rbuf->XERCV$cmd_status && %X"0FF00");
	};

    if (Error_Flag)
	{
	if (XE_Int->XEI$retry_count GEQ XE_Int->XEI$max_retry)
	    XE$ERR(XE_Int,"XE retried !ZB times.",XE_Int->XEI$retry_count)
	else
	    {
	    // Stuff this packet back on the queue for deallocation
	    Rbuf = Rbuf - XE_hdr_Offset;
	    INSQUE(Rbuf, XE_Int->XEI$recv_Qtail);
	    XE_Int->XEI$retry_count = XE_Int->XEI$retry_count+1;
	    DRV$AST_in_Progress = False;
	    RETURN;
	    }
	}
    else
        XE_Int->XEI$retry_count = 0;

// Get another buffer and put it on the receive Q for this device

    XE_Int->XEI$curhdr = XE_Int->XEI$curhdr+1;
    if (XE_Int->XEI$curhdr > (MAX_RCV_BUF-1))
	XE_Int->XEI$curhdr = 0;

    NRbuf = DRV$Seg_get(DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len));
    INSQUE(NRbuf,XE_Int->XEI$recv_Qtail);
    NRbuf = NRbuf + XE_hdr_offset;
    RC = $QIO(chan=XE_Int->XEI$IO_chan, EFN=ASTEFN,
	      Func=IO$_READVBLK,
	      IOSB=NRbuf->XERCV$vms_code, Astadr=XE_Receive, Astprm=.XE_Int,
	      P1=NRbuf->XERCV$data,
	      P2=DRV$MAX_PHYSICAL_BUFSIZE,
	      P5=rcvhdrs[XE_Int->XEI$curhdr,XERCV$buf]);

// Check for recoverable error. This hack is necessary because the DEQNA
// is known to get wedged, and the driver gives back SS$_DEVINACT when this
// happens.

    if (NOT RC)
	{
	if (RC == SS$_DEVINACT)
	    {
	    // Stuff the packet back on the queue for deallocation
	    Rbuf = Rbuf - XE_hdr_Offset;
	    INSQUE(Rbuf, XE_Int->XEI$recv_Qtail);
	    XE$ERR(XE_Int,"XE $QIO read error (dev_inact), RC=!XL",RC);
	    }
	else
	    DRV$Fatal_FAO("Ethernet $QIO queue read error, EC = !XL",RC);
	}
    else
	{

// Send datagram to IP

	if ($$LOGF(LOG$PHY))
	    XE_LOG("XE IP receive",0,rcvhdrs[rcvix,XERCV$src]);

	dev_config->dcmib_ifInOctets = dev_config->dcmib_ifInOctets +
		Rbuf->XERCV$tran_size + XE_hdr_len;

	DRV$IP_Receive(Rbuf-XE_hdr_offset,
		   DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len),
		   Rbuf->XERCV$data,Rbuf->XERCV$tran_size,dev_config);
	};

    DRV$AST_in_Progress = False;
    }


//Sbttl   "Ethernet driver arp recv"
/******************************************************************************

Function:

     This routine is an AST interrupt routine.  It is started when the
     Ethernet controller system driver has received an ARP packet and issues
     an AST to this routine. ARP packet is passed to ARP module for processing.

Inputs:

     XE_Int - Controller info block for this interface

*******************************************************************************
*/
XE_ArpRcv( struct XE_Interface_Structure * XE_Int ): NOVALUE (void)
    {
    BIND
	dev_config = XE_Int->struct Device_Configuration_Entry * XEI$Dev_Config;
    signed long
	struct XE_ARP_STRUCTURE * ARbuf,
	usflag,
	tpa,
	I,
	spa,
	Route,
	RC;

// Set flag indicating interrupt in progress

    DRV$AST_in_Progress = True;

// Check for bogosity

    if (NOT dev_config->dc_online)
	{
!~~	Send_2_Operator(%ASCID "XE ARP receive AST when offline");
	DRV$AST_in_Progress = False;
	RETURN;
	};

// check status of receive

    ARbuf = XE_Int->XEI$ARP_buffer;
    IF ((ARbuf->ar_ios0 != SS$_Normal) AND
       (ARbuf->ar_ios0 != SS$_DATAOVERUN)) THEN
	// Error from DEC driver
	if (ARbuf->ar_ios0 == SS$_ABORT)
	    DRV$OPR_FAO("XE abort, cmd_status=!XW, error_summary=!XB",
  			ARBUF->ar_ios2,ARBUF->ar_ios3)
	else
	    DRV$Error_FAO("XE ARP receive error (ugh), EC = !XL",ARbuf->ar_ios0);

    if ($$LOGF(LOG$PHY))
	XE_LOG("XE ARP receive",0,ARbuf->phys$2);

// Packet is OK at the hardware level - hand it up to the ARP module

    XEARP$INPUT(XE_Int,ARBUF->AR_DATA);

// restart the arp receive
!!!HACK!!// what's the EFN for?
    RC = $QIO(	chan=XE_Int->XEI$ARP_IO_chan, EFN=ARPEFN,
		IOSB=ARbuf->ar_ios0,Func=IO$_READVBLK,
		P1=ARbuf->ar_data, P2=ARP_MAX_LEN*4,
		P5=ARbuf->phys$1,
		astadr=XE_ArpRcv, astprm=.XE_Int);
    if (RC != SS$_Normal)
	XE$ERR(XE_Int,"XE ARP read failure, RC = !XL",RC);

    DRV$AST_in_Progress = False;
    }


//SBTTL "ARP transmit routine"

XE$ARP_XMIT(XE_Int,arbuf,arlen,dest) : NOVALUE (void)

// Finish packaging of ARP packet and transmit it.
// ARBUF points to the ARP data portion of the packet - header space has been
// reserved before it, if needed.

    {
    MAP
	struct XE_Interface_Structure * XE_Int;
    signed long
	rc,
	ios : XE_IOSB_STRUCTURE;

    DRV$NOINT ;

// 0 for destination means he wants us to do a broadcast.

    if (dest == 0)
	dest = xe_broadcast;

// Write the ARP to the network (don't need to build any header info)

    rc = $QIOW(	CHAN=XE_Int->XEI$ARP_IO_chan, FUNC=IO$_WRITEVBLK,
		IOSB=ios, P1=.arbuf, P2=.arlen, P5=.dest);
    if (rc != SS$_NORMAL)
	{
	XE$ERR(XE_Int,"XE ARP xmit failure, RC = !XL",rc);
	Signal(RC)
	}
    else
	if ($$LOGF(LOG$PHY))
	    XE_LOG("XE ARP xmit",0,dest);
    DRV$OKINT ;
    }

//SBTTL "Perform device specific DUMP functions"

XE$dump(dev_config, funct, arg, buffer, sizeAdrs)
    {
    EXTERNAL ROUTINE
	XE$ARP_DUMP;


    SELECTONE funct OF
	SET
	[XEDMP$ARP_Entry] :
	    {
	    sizeAdrs = XE$ARP_DUMP ( arg, buffer, ..sizeAdrs);
	    SS$_NORMAL
	    };
	[Otherwise] :
	    0
	TES
    }



DRV$TRANSPORT_INIT (void)
// Initialize the transport information/entry vector
// Must be done at run time to avoid ADDRESS fixups...
    {

    // Fill in the EtherNet description string
    XE_descriptor[0] = %CHARCOUNT(XE_description);
    XE_descriptor[1] = UPLIT(XE_description);

    // Provide the XEDRV entry points
    DRV$Device_Info->DI$Init	= XE$Init;
    DRV$Device_Info->DI$Xmit	= XE$XMit;
    DRV$Device_Info->DI$Dump	= XE$Dump;
    DRV$Device_Info->DI$Check	= XE$Check;

    DRV$Device_Info
    }


}
ELUDOM

