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

#if 0
MODULE XE_DRIVER(IDENT="5.0a",LANGUAGE(BLISS32),
		 ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
				 NONEXTERNAL=LONG_RELATIVE),
		 LIST(EXPAND,TRACE,NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
		 OPTIMIZE,OPTLEVEL=3,ZIP)=
!!!HACK!!// Add code to shut-down an interface permanently...
!!!HACK!!// Make sure errors are handled correctly.

#endif

//LIBRARY <starlet.h>	// VMS system defintions
// not yet #include "SYS$LIBRARY:LIB";	// VMS system defintions

#include <cmuip/central/include/nettcpip.h>		// Required for UDP check
// not yet#include <cmuip/central/include/netxport.h>		// VMS specifics
#include <cmuip/central/include/netcommon.h>	// CMU-OpenVMS/IP
#include "netvms.h"		// VMS specifics
#include <cmuip/central/include/netconfig.h>	// Device interface specs.
#include <cmuip/central/include/netdevices.h>	// Helpfull macros...

#include "xedrv.h"
#include "xearp.h"

#include <iodef.h>
#include <ssdef.h>
#include <nmadef.h>
#include <descrip.h>
#include <xmdef.h>

// NETMACLIB.OBJ
extern    Time_Stamp();
extern     swapbytes();

// NETDEVICES.OBJ
extern      ASCII_HEX_BYTES();

// XEDRV_ARP.BLI

extern  void    xearp$dev_init();
extern     xearp$check();
extern  void    xearp$input();

void    XE_receive();
    void xe_arprcv();

    char *   XE_BROADCAST = "FFFFFFFFFFFF"; // check

    // Description of this device
#define    XE_description "Digital Ethernet Card"

static signed long
    // String descriptor used to hold description
long    XE_descriptor[2];


//SBTTL "Declare the device information block used to describe entry points."

static 
    // DRV$Device_Info is a list of everything we want the IPACP
    // to know about us...  Initialized by ROUTINE DRV$TRANSPORT_INIT.
     Device_Info_Structure DRV$Device_Info_, * DRV$Device_Info = &DRV$Device_Info_;

    // The IPACP_Interface tells us all about the IPACP.  It gives us
    // entry points, literals and global pointers.  See NETDEVICES.REQ
    // for a complete explaination of this structure.
    // Note: This pointer must be named "IPACP_Interface"
    IPACP_Info_Structure * IPACP_Interface;



 void    XE_Shutdown();

static XE$ERR(XE_Int)
  {
    /*DRV$OPR_FAO(%REMAINING); */ /* check */
    XE_Shutdown(XE_Int,TRUE);
  } 



//SBTTL "Start asynchronous I/O on the DEC ethernet"

XE_StartIO ( struct XE_Interface_Structure * XE_Int)
    {
      signed long I,
      RC;
    struct XERCV_QB_structure * Buff;
	struct xe_arp_structure * ARbuf;
      struct XERCV_structure * rcvhdrs = XE_Int->XEI$rcvhdrs ;
      short XE_chan = XE_Int->xei$io_chan;
	short XAR_chan = XE_Int->xei$arp_io_chan;

// Supply four receive buffers to device controller

    DRV$OPR_FAO ("XEDRV: MPBS = !SL",DRV$MAX_PHYSICAL_BUFSIZE);

    XE_Int->XEI$curhdr = 0;
    for (I=0;I<=(MAX_RCV_BUF-1);I++)
	{	// Get buffer, put on Q and issue IO$_READVBLK function
	Buff = drv$seg_get(DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len));
	INSQUE ( Buff , XE_Int-> XEI$recv_Qtail  );
	Buff = Buff + XE_hdr_offset;
	RC = exe$qio(ASTEFN,XE_chan,IO$_READVBLK,Buff->XERCV$vms_code,
		     XE_receive,  XE_Int,
		  Buff->XERCV$data,
		  DRV$MAX_PHYSICAL_BUFSIZE,
		     0, 0,
		     rcvhdrs[I].XERCV$buf,0);
	if (RC != SS$_NORMAL)
	    {     // Error issuing set receive buffer command
	    XE$ERR(XE_Int,"XE queue read request failure, RC=!XL",RC);
	    return 0;
	    }
	};

// get ARP buffer and issue a receive qio

    ARbuf = drv$seg_get(XE_ARP_LEN*4);
    XE_Int-> xei$arp_buffer  = ARbuf;
    RC = exe$qio(	ARPEFN, XE_Int-> xei$arp_io_chan ,
		IO$_READVBLK,
		ARbuf -> ar_ios0 ,
			xe_arprcv, XE_Int,
			ARbuf->ar_data, ARP_MAX_LEN*4, 0, 0,
			ARbuf->phys$1, 0);
    if (RC != SS$_NORMAL)
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

	struct XE_Interface_Structure * XE_Int;
    {
    signed long
	RC,
	plen;
	struct XE_iosb_structure IOS_ , * IOS = &IOS_;
	struct XE_setup_structure Setup_, * Setup= &Setup_;
	struct XE_sdesc_structure Paramdescr_, * Paramdescr= &Paramdescr_;

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

    Setup ->XE$c_pcli_mca     = NMA$C_PCLI_MCA;
    Setup ->XE$w_mca_length   = 2 + XE_ADR_SIZE;
    Setup ->xe$w_mca_mode     = NMA$C_LINMC_CLR;
    CH$MOVE (XE_ADR_SIZE, XE_BROADCAST, Setup ->XE$L_mca_address);

// If he wants us to set the physical address, then do so.

    if (setflag)
	{
	Setup->XE$c_pcli_pha = NMA$C_PCLI_PHA;
	Setup->XE$w_pcli_phlen = XE_ADR_SIZE+2;
	Setup->XE$w_pcli_phmode = NMA$C_LINMC_SET;
	CH$MOVE(XE_ADR_SIZE,CH$PTR(setaddr),CH$PTR(Setup->XE$l_pcli_phaddr));
	plen = (long)Setup->xe$setup_pha_end - (long)Setup;
	}
    else
	plen = (long)Setup->xe$setup_end - (long)Setup;

// Set up for IP protocol on this channel

    Setup->XE$l_protocol      = XE_IP_type;	// IP
    swapbytes(1,&Setup->XE$l_protocol);

    Paramdescr->xe$setup_length = plen;
    Paramdescr->xe$setup_address = Setup;

// Issue the startup command to controller

    RC = exe$qiow (1, XE_Int->xei$io_chan,
		IO$_SETMODE+IO$M_CTRL+IO$M_STARTUP,IOS,0,0,0,Paramdescr);
    if (!( (RC == SS$_NORMAL) && (IOS->xe$vms_code == SS$_NORMAL) ))
	{
	if (IOS->xe$vms_code == SS$_BADPARAM)
	   XE$ERR(XE_Int,"XE startup failure, RC=!XL,VMS_code=!XL,Param=!XL",
		  RC,IOS->xe$vms_code,((long *)IOS)[1]);
	else
	   XE$ERR(XE_Int,"XE startup failure, RC=!XL,VMS_code=!XL,Xfer size=!SL",
	       RC,IOS->xe$vms_code,IOS->xe$tran_size);
	return 0;
	};

// Modify the setup block for the arp responder

    Setup->XE$l_protocol = XE_ARP_type;
    Setup->xe$w_mca_mode = NMA$C_LINMC_SET;

    swapbytes(1,&Setup->XE$l_protocol);
    Paramdescr->xe$setup_length = (long)Setup->xe$setup_end - (long)Setup;
    Paramdescr->xe$setup_address = Setup;

// Issue the startup command to controller

    RC = exe$qiow (1, XE_Int->xei$arp_io_chan,
		IO$_SETMODE+IO$M_CTRL+IO$M_STARTUP,IOS,0,0,0,Paramdescr);
    if (!( (RC == SS$_NORMAL) && (IOS->xe$vms_code == SS$_NORMAL) ))
	{	// Startup command failed
	XE$ERR(XE_Int,
	       "XE ARP startup failure, RC=!XL,VMS_code=!XL,Xfer size=!SL",
	       RC,IOS->xe$vms_code,IOS->xe$tran_size);
	return 0;
	};

// Everything OK - return TRUE value

    return -1;
    }

XE_SenseDev( struct XE_Interface_Structure * XE_Int,
		     struct xe_addrs_structure * phaddr,
		     struct xe_Addrs_structure * hwaddr,
		     long online)
//
// Read status of device.
//   phaddr	pointer to area to store "physical" (decnet) address
//   hwaddr	pointer to area to store hardware address
//   online	true if device has been started successfully.
//
    {
    signed long
      RC;
//!!HACK!!// What's wrong with XE_sense_blk?
//	Sense: 	XE_sense_blk,	
    char Sense [512];
    struct XE_sdesc_structure * Paramdescr;
	struct XE_iosb_structure * IOS;

// Get info from controller

    Paramdescr->xe$setup_length = sizeof (Sense);
    Paramdescr->xe$setup_address = Sense;

    RC = exe$qiow (1, XE_Int->xei$io_chan,
		IO$_SENSEMODE+IO$M_CTRL, IOS, 0, 0, 0, Paramdescr);
    if (! ((RC == SS$_NORMAL) && (IOS->xe$vms_code)))
	{			// Statistics call failed
	XE$ERR(XE_Int,
	       "XE sense mode QIOW failure, RC=!XL,VMS_Code=!XL,Xfer size=!SL",
	       RC,IOS->xe$vms_code,IOS->xe$tran_size);
	return 0;
	};

    Paramdescr->xe$setup_length = IOS ->xe$tran_size;

    while (Paramdescr ->xe$setup_length > 0)
	{
	  struct XE_Sense*  Param = Paramdescr ->xe$setup_address ;

	Paramdescr ->xe$setup_length = Paramdescr ->xe$setup_length - 
				XE_Param_Size (Param);
	Paramdescr ->xe$setup_address = Paramdescr ->xe$setup_address +
				XE_Param_Size (Param);

	switch (Param->XE_Sense_Param)
	  {
	case NMA$C_PCLI_PHA:
	    CH$MOVE(XE_ADR_SIZE,CH$PTR(Param->XE_Sense_String),
		    CH$PTR(phaddr));
	    break;

	case NMA$C_PCLI_HWA:
	    CH$MOVE(XE_ADR_SIZE,CH$PTR(Param->XE_Sense_String),
		    CH$PTR(hwaddr));
	    break;
	  }
	};

// See if the device is online
//Ignore the timeout bit for DEQNA's

//!!HACK!!// there may be a problem with XE$cmd_status.
    online = (IOS->XE$cmd_status && 0x0FF00) == XM$M_STS_ACTIVE;

// Return success

    return -1;
    }

//SBTTL "Start device - top-level"

xe_startall ( XE_Int , restart )
	struct XE_Interface_Structure * XE_Int;
    {
    signed long
      online;
     Device_Configuration_Entry * dev_config;
    struct xe_addrs_structure * addrs;
	struct xe_addrs_structure * hwaddr;
	struct xe_addrs_structure * phaddr;
	struct xe_addrs_structure * useaddr;

// Point at out current idea of the device address

    addrs = XE_Int-> xei$phy_addr ;

// Try to start the device

    if (! XE_StartDev(XE_Int,0,0))
	return 0;

// Only check for DECNET and set ARP address the first time that we successfully
// start the device up. Otherwise, when (not IF) the first error causes the
// device to be shut-off, we run the risk of wedging everything.

    if (XE_Int->XEI$XE_started)
	{

// If we've been started before, just make sure that our address agrees.
// If not, it probably means that DECNET was slow to get started and we
// need to wait for it.

	if (! XE_SenseDev(XE_Int,phaddr,hwaddr,online))
	    return 0;
	if (XE_Int->XEI$XE_decnet)
	  useaddr = phaddr;
	else
	    useaddr = hwaddr;
	if (CH$NEQ(XE_ADR_SIZE,CH$PTR(useaddr),
		  XE_ADR_SIZE,CH$PTR(XE_Int->xei$phy_addr)))
	    {
	    DRV$OPR_FAO("XE restart failed - address mismatch");
	    XE_Shutdown(XE_Int,TRUE);
	    return 0;
	    };
	}
    else
	{

// Get device address

	if (! XE_SenseDev(XE_Int,phaddr,hwaddr,online))
	    return 0;

// Remember that it was started at least once

	    XE_Int->XEI$XE_started = TRUE;

// If no "physical" (DECNET) address, then restart device and set address

	    if (CH$EQL(XE_ADR_SIZE,CH$PTR(phaddr),
		      XE_ADR_SIZE,CH$PTR(XE_BROADCAST)))
		{
		XE_Shutdown(XE_Int,0);
		if (! XE_StartDev(XE_Int,TRUE,hwaddr))
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

	    if (CH$NEQ(XE_ADR_SIZE,CH$PTR(useaddr),
		      XE_ADR_SIZE,CH$PTR(addrs)))
		{
		if (CH$NEQ(XE_ADR_SIZE,CH$PTR(addrs),
			  XE_ADR_SIZE,CH$PTR(UPLIT(0,0))))
		    {
			DESC$STR_ALLOC(newstr,50);

		    ASCII_Hex_Bytes(newstr,XE_ADR_SIZE,useaddr,
				    newstr->dsc$w_length);
		    DRV$OPR_FAO("XE address mismatch, using address: !AS",newstr);
		    };
		CH$MOVE(XE_ADR_SIZE,CH$PTR(useaddr),CH$PTR(addrs));
		};

// Initialize ARP parameters for this device

//!!HACK!!// Trim this down...
    	xearp$dev_init(XE_Int,AR$HRD_ETHER,XE_IP_type,addrs,XE_ARP_HLEN,1,0);
	};

// Start I/O on the device

    XE_StartIO ( XE_Int );

// Say that this device is finally ready
    dev_config = XE_Int->xei$dev_config;
//    dev_config [dc_online] = online;
    dev_config ->dc_online = TRUE;

    return TRUE;
    }



#define    EMPTY_QUEUE 3

#define    XE_RESTART_TIME 5*100		// How long to leave shutdown

void XE_FreeBufs ( struct XE_Interface_Structure * XE_Int )

// AST routine to deallocate XE read buffers on error.
// Scheduled by shutdown routine.

    {
	struct XERCV_QB_Structure * BUFF;

// Flush IP buffers

    while (REMQUE ( XE_Int-> XEI$recv_Qhead  , BUFF ) != EMPTY_QUEUE)
	drv$seg_free ( DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len + IOS_len) , BUFF );

// Release the ARP buffer

    if ((BUFF=XE_Int->xei$arp_buffer) != 0)
	{
	drv$seg_free ( XE_ARP_LEN*4 , BUFF );
	XE_Int-> xei$arp_buffer  = 0;
	};

// Say that this has been done.

    XE_Int-> XEI$need_2_free  = FALSE;
    }

void XE_Shutdown ( XE_Int , restart )
//
// Shut the device controller down. Issue shutdown command to controller
// if online, set offline, general cleanup.
// 

	struct XE_Interface_Structure * XE_Int;
    {
    signed long
	RC,
      now;
     Device_Configuration_Entry * dev_config;
	 struct XE_iosb_structure * IOS;

// Disallow ASTs

    DRV$NOINT;

    now =  Time_Stamp();

// Set device offline.
    dev_config = XE_Int->xei$dev_config;
    dev_config ->dc_online = FALSE;

// If it should be restarted, then say when

    if (restart != 0)
	{
	XE_Int-> XEI$restart_time  = now + XE_RESTART_TIME;
	drv$device_error();
	}
    else
	XE_Int-> XEI$restart_time  = 0;

// Shutdown the IP channel

    if (XE_Int->xei$io_chan != 0)
	{
	exe$cancel(XE_Int->xei$io_chan);
	RC = exe$qiow( 1, XE_Int->xei$io_chan,
		    IO$_SETMODE+IO$M_CTRL+IO$M_SHUTDOWN,IOS);
	if ((RC != SS$_NORMAL) || (IOS->xe$vms_code != SS$_NORMAL))
	    {
	    DRV$Warn_FAO("XE shutdown QIOW failure, EC = !XL",RC);
	    DRV$Fatal_FAO("XE shutdown failure, EC = !XL",IOS->xe$vms_code);
	    };
	};

// Shutdown the ARP channel

    if (XE_Int->xei$arp_io_chan != 0)
	{
	exe$cancel(XE_Int->xei$arp_io_chan);
	RC = exe$qiow( 1, XE_Int->xei$arp_io_chan,
		    IO$_SETMODE+IO$M_CTRL+IO$M_SHUTDOWN,IOS);
	if ((RC != SS$_NORMAL) || (IOS->xe$vms_code != SS$_NORMAL))
	    {
	    DRV$Warn_FAO("XE ARP shutdown QIOW failure, EC = !XL",RC);
	    DRV$Fatal_FAO("XE ARP shutdown failure, EC = !XL",IOS->xe$vms_code);
	    return;
	    };
	};

// Schedule AST to deallocate all buffers

    if (XE_Int->XEI$IO_queued)
	{
	XE_Int->XEI$IO_queued = FALSE;
	XE_Int->XEI$need_2_free = TRUE;
	exe$dclast( XE_FreeBufs, XE_Int);
	};

// Allow AST's again
    DRV$OKINT;

    }

//SBTTL	"Ethernet driver check routine"
// Routine to call whenever the device is offline and shouldn't be.

XE$CHECK ( Device_Configuration_Entry * dev_config )
    {
    signed long
	now;
	struct XE_Interface_Structure * XE_Int = dev_config-> dc_dev_interface  ;


    now = Time_Stamp();
    if (now > XE_Int->XEI$restart_time)
	{
	if ((! XE_Int->XEI$need_2_free) &&
	   xe_startall(XE_Int,TRUE))
	    {		// Device restarted OK
	    XE_Int->XEI$restart_time = 0;
	    XE_Int->XEI$retry_count = 0;
	    XE_Int->XEI$restart_count = XE_Int->XEI$restart_count + 1;
	    DRV$OPR_FAO("XE (DEC ENET) restarted, count = !SL",
		    XE_Int->XEI$restart_count);
//!!HACK!!// Is it OK to move this to after $ACPWAKE
//	    Dev_attn = Dev_attn-1;
	    DRV$ACPWAKE;		// Special event...
	    // Return -1 to decrement the # of devices needing attention.
	    return -1;
	    }
	else			// Wait a while and try again...
	    XE_Int->XEI$restart_time = now + XE_RESTART_TIME;
	};

// Return 0 since device still needs attention.
    return 0;
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

//!!HACK!!// Should there be XE$Init and XE$Init_Controller?
//!!HACK!!//  XE$Init_Class and XE$Init_Port

void XE$init ( Device_Configuration_Entry * dev_config ,
 		  long IPACP_Int, long max_retry, long MPBS)
    {
extern 	xearp$init();
extern 	LIB$GET_VM();
extern 	LIB$GET_VM_PAGE();
    signed long
	RC,
	rcvhdrs,
	XE_Chan,
	XAR_Chan;
    struct XE_Interface_Structure * XE_Int;

// Setup the global
    IPACP_Interface = IPACP_Int;

// Hold AST's until done here
    DRV$NOINT;

// Initialize the ARP module
    xearp$init();

// Assign Ethernet Controller
    if (! (RC=exe$assign (&dev_config->dc_devname, &XE_Chan, 0, 0, 0)))
         // Ethernet controller assign failed
	{
	DRV$Fatal_FAO("XE $ASSIGN failure (dev=), EC = !XL",
		    dev_config->dc_devname,RC);
	return;
	};

//  Assign the channel for the arp responder
    if (! (RC=exe$assign( &dev_config->dc_devname,&XAR_Chan, 0, 0 ,0)))
         // Ethernet controller assign failed
	{
	DRV$Fatal_FAO("XE $ASSIGN failure (dev=), EC = !XL",
		    dev_config->dc_devname,RC);
	return;
	};

// Allocate and Initialize the XE controller block
    // Allocate VM
    //!!HACK!!// When are we going to deallocate this?  Ever?
//    if (! (LIB$GET_VM(%REF(XE_Interface_size*4),XE_Int)))
      if (! (LIB$GET_VM_PAGE(/*%REF*/(((XE_Interface_size * 4) / 512) + 1),&XE_Int)))
	{	// Couldn't allocate memory for controller block
	DRV$Fatal_FAO("XE LIB$GET_VM failure (dev=), EC = !XL" ,
		    dev_config->dc_devname,RC);
	return;
	};

    // Zero out the memory block
    CH$FILL(/*%CHAR*/(0),XE_Interface_size*4,XE_Int);

    // Fill in the blanks...
    XE_Int->xei$io_chan = XE_Chan;
    XE_Int->xei$arp_io_chan = XAR_Chan;
    XE_Int->XEI$Phy_Size = 6;	// Ether addrs are 6 bytes long (48 bits)
    XE_Int->XEI$max_retry = max_retry;	// Maximum # of conseq. retries allowed
    XE_Int->XEI$MPBS = MPBS;	// Maximum Physical Buffer Size
    XE_Int->XEI$Flags = 0;	// Just making sure...

    // Set-up the receive queue
    XE_Int->XEI$recv_Qhead = XE_Int->XEI$recv_Qhead;
    XE_Int->XEI$recv_Qtail = XE_Int->XEI$recv_Qhead;

    // set double-link between XE_Int and dev_config blocks
    XE_Int-> xei$dev_config  = dev_config;
    dev_config -> dc_dev_interface   = XE_Int;

// Allocate and Initialize the XE receive headers
    // Allocate VM
    //!!HACK!!// When are we going to deallocate this?  Ever?
//    if (! (LIB$GET_VM(%REF(MAX_RCV_BUF*XERCV_LEN*4),rcvhdrs)))
      if (! (LIB$GET_VM_PAGE(/*%REF*/(((MAX_RCV_BUF*XERCV_LEN*4)/512)+1),&rcvhdrs)))
	{	// Couldn't allocate memory for receive headers
	DRV$Fatal_FAO("XE LIB$GET_VM failure (dev=!AS), RC=!XL",
		    dev_config->dc_devname,RC);
	return;
	};

    XE_Int->XEI$rcvhdrs = rcvhdrs;
    XE_Int->XEI$curhdr = 0;	// current ethernet header to use

// Start the device
    xe_startall(XE_Int,FALSE);

// Fill in the dev_config ifTable fields
    {
	struct dsc$descriptor * desc = &dev_config->dcmib_ifDescr;

//    dev_config->dcmib_ifIndex = -1;	// Filled by IPACP

    desc->dsc$w_length = XE_descriptor[0];
    desc->dsc$a_pointer = XE_descriptor[1];

    dev_config->dcmib_ifType = 6;		// EtherNet
    dev_config->dcmib_ifMTU = DRV$MAX_PHYSICAL_BUFSIZE;
    dev_config->dcmib_ifSpeed = 10000000;	// bits/second

    dev_config->dcmib_ifPAsize = XE_Int->XEI$Phy_Size;
    dev_config->dcmib_ifPhysAddress = XE_Int->xei$phy_addr;

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

static    XE_LOG(MSG,IPADDR,HWADDR)
    {
      long * STR_DESC;
      extern	void xearp$log();

    STR_DESC[0] = sizeof(MSG);
    STR_DESC[1] = MSG;

    xearp$log(STR_DESC,IPADDR,6,HWADDR);
    }

void xe$xmit ( Device_Configuration_Entry * dev_config )
    {
      struct XE_iosb_structure * IOS; 
      Net_Send_Queue_Element * QB;
    struct xe_addrs_structure * Addrs;
    struct XESND_structure * Sbuf;
    struct ip_structure * IPHDR;
    signed long
	RC,
	Sen_size,
	ARstat,
	AddrCheck,
	xchan;

	struct  XE_Interface_Structure * XE_Int = &dev_config ->dc_dev_interface  ;

    DRV$NOINT;
// Check if a request is on the Net_send_Q for this device

    if ((REMQUE(dev_config->dc_Send_Qhead,QB)) == EMPTY_QUEUE) // check
	return;			// The Q is empty

    ARstat = 0;			// Assume we will deallocate the packet

// Make sure device is online

    if (! dev_config->dc_online)
	{			// Device is offline
	DRV$XQL_FAO(LOG$PHY,"!%T XE device !XL offline (xmit)!/",0,XE_Int);
	}
    else
X:	{

// Position for Ethernet device header

	Sbuf = QB->NSQ$Data - XE_hdr_len;
	IPHDR = QB->NSQ$Data;
// IPH$TTL in the swapped header is really IPH$Protocol
	if ((IPHDR->iph$ttl == UDP_PROTOCOL) &&
	   (IPHDR->iph$dest == ((! dev_config->dc_ip_netmask) ||
		dev_config->dc_ip_network)))
	    {
	    CH$MOVE(XE_ADR_SIZE, CH$PTR(XE_BROADCAST), CH$PTR(Addrs));
	    ARstat = 1;
	    }
	else
	    ARstat = xearp$check(XE_Int,QB->NSQ$IP_Dest,Addrs,QB);
	if (ARstat > 0)
	    {		// Have an address

// Fill in Ethernet header information

#if 0
	    if ($$LOGF(LOG$PHY))
		XE_LOG("XE IP xmit",QB->NSQ$IP_Dest,Addrs);
#endif

	    CH$MOVE(XE_ADR_SIZE,CH$PTR(Addrs),Sbuf->XESND$dest);
	    Sbuf->XESND$type = XE_IP_type;
	    swapbytes(1,&Sbuf->XESND$type);

	    xchan = XE_Int->xei$io_chan;

// Send packet to controller

//!!HACK!!// What's the EFN for?
	    Sen_size = MAX(QB->NSQ$Datasize,XE_MINSIZE);
	    RC = exe$qiow(1,	xchan,
			IO$_WRITEVBLK,
			  IOS, 0, 0,
			QB->NSQ$Data,
			  Sen_size, 0, 0,
			Sbuf, 0);

// Check for $QIO error

	    if (! (RC))
		{
		XE$ERR(XE_Int,"XE $QIO error (send),RC=!XL",RC);
		goto leave_x;
		};

// Check for device driver error

	    if (IOS->xe$vms_code != SS$_NORMAL)
		{
		XE$ERR(XE_Int,"XE driver error (send),VMS_code=!XL",
		       IOS->xe$vms_code);
		goto leave_x;
		};

// Check for controller error

	    if ((IOS->XE$cmd_status && 0x0FF00) != XM$M_STS_ACTIVE)
		{
		DRV$XLOG_FAO(LOG$PHY,"%T XE command error !XW!/",
				0,IOS->XE$cmd_status);
		goto leave_x;
		};
	    };
	};	// End of block X:
    leave_x:

// Delete buffer and release QBlk if ARP didn't claim them and deletable

    if (ARstat >= 0)
	{
	if (QB->NSQ$Delete)
	    drv$seg_free(QB->NSQ$Del_buf_size,QB->NSQ$Del_Buf);
	drv$qblk_free(QB);
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

void XE_receive ( struct XE_Interface_Structure * XE_Int )
    {
      struct XERCV_QB_structure * Rbuf;
      struct XERCV_QB_structure * NRbuf;
       Device_Configuration_Entry * dev_config;
    signed long
	rcvix,
	RC,
	IRC,
	Error_Flag  = 0;

    struct XERCV_structure * rcvhdrs = XE_Int->XEI$rcvhdrs ;

    dev_config = XE_Int-> xei$dev_config ;

// Set flag indicating interrupt in progress

    DRV$AST_IN_PROGRESS = TRUE;

// If device not online, then give message and punt

    if (! dev_config->dc_online)
	{

//~~	DRV$OPR_FAO("XE receive AST when offline");
	DRV$AST_IN_PROGRESS = FALSE;
	return;
	};

// Get first input packet off of the queue
//!!HACK!!// What if the first packet wasn't the one which $QIO returned?
    REMQUE(XE_Int->XEI$recv_Qhead,Rbuf);
    Rbuf = Rbuf + XE_hdr_offset;
    rcvix = XE_Int->XEI$curhdr;

    if ((RC = Rbuf->XERCV$vms_code) != SS$_NORMAL)
	{
//	Error_Flag = 1;
	switch (Rbuf->XERCV$vms_code)
	  {
	    case SS$_ABORT:
		DRV$OPR_FAO("XE abort, cmd_status=!XW, error_summary=!XB",
  			Rbuf->XERCV$cmd_status,Rbuf->XERCV$error_summary);
		break;

	    case SS$_DATAOVERUN: case SS$_TIMEOUT:
		{
		DRV$OPR_FAO("XE read error (timeout/overrun), RC=!XL",RC);
		DRV$OPR_FAO("DEBUG - bytes received=!XL",Rbuf->XERCV$tran_size);
		};
		break;

	    default :
		DRV$OPR_FAO("XE: VMS Error, cmd_status=!XW, error_summary=!XB",
  			Rbuf->XERCV$cmd_status,Rbuf->XERCV$error_summary);
	  }
	};

    //Ignore the timeout bit for DEQNA's
    if ((Rbuf->XERCV$cmd_status  && 0x0FF00) != XM$M_STS_ACTIVE)
	{
	Error_Flag = 1;
	// Error from board
	DRV$OPR_FAO("XE status error.  Status = !XL",
		     Rbuf->XERCV$cmd_status && 0x0FF00);
	};

    if (Error_Flag)
	{
	if (XE_Int->XEI$retry_count >= XE_Int->XEI$max_retry)
	  XE$ERR(XE_Int,"XE retried !ZB times.",XE_Int->XEI$retry_count);
	else
	    {
	    // Stuff this packet back on the queue for deallocation
	    Rbuf = Rbuf - XE_hdr_offset;
	    INSQUE(Rbuf, XE_Int->XEI$recv_Qtail);
	    XE_Int->XEI$retry_count = XE_Int->XEI$retry_count+1;
	    DRV$AST_IN_PROGRESS = FALSE;
	    return;
	    }
	}
    else
        XE_Int->XEI$retry_count = 0;

// Get another buffer and put it on the receive Q for this device

    XE_Int->XEI$curhdr = XE_Int->XEI$curhdr+1;
    if (XE_Int->XEI$curhdr > (MAX_RCV_BUF-1))
	XE_Int->XEI$curhdr = 0;

    NRbuf = drv$seg_get(DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len));
    INSQUE(NRbuf,XE_Int->XEI$recv_Qtail);
    NRbuf = NRbuf + XE_hdr_offset;
    RC = exe$qio(ASTEFN,XE_Int->xei$io_chan,
	      IO$_READVBLK,
	      NRbuf->XERCV$vms_code, XE_receive, XE_Int,
	      NRbuf->XERCV$data,
	      DRV$MAX_PHYSICAL_BUFSIZE,
0,0,
	      rcvhdrs[XE_Int->XEI$curhdr].XERCV$buf,0);

// Check for recoverable error. This hack is necessary because the DEQNA
// is known to get wedged, and the driver gives back SS$_DEVINACT when this
// happens.

    if (! RC)
	{
	if (RC == SS$_DEVINACT)
	    {
	    // Stuff the packet back on the queue for deallocation
	    Rbuf = Rbuf - XE_hdr_offset;
	    INSQUE(Rbuf, XE_Int->XEI$recv_Qtail);
	    XE$ERR(XE_Int,"XE $QIO read error (dev_inact), RC=!XL",RC);
	    }
	else
	    DRV$Fatal_FAO("Ethernet $QIO queue read error, EC = !XL",RC);
	}
    else
	{

// Send datagram to IP

#if 0
	if ($$LOGF(LOG$PHY))
	    XE_LOG("XE IP receive",0,rcvhdrs[rcvix].XERCV$src);
#endif

	dev_config->dcmib_ifInOctets = dev_config->dcmib_ifInOctets +
		Rbuf->XERCV$tran_size + XE_hdr_len;

	drv$ip_receive(Rbuf-XE_hdr_offset,
		   DRV$MAX_PHYSICAL_BUFSIZE+(Qhead_len+IOS_len),
		   Rbuf->XERCV$data,Rbuf->XERCV$tran_size,dev_config);
	};

    DRV$AST_IN_PROGRESS = FALSE;
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
void xe_arprcv( struct XE_Interface_Structure * XE_Int )
    {
	 Device_Configuration_Entry * dev_config = XE_Int->xei$dev_config;
	struct xe_arp_structure * ARbuf;
    signed long
	usflag,
	tpa,
	I,
	spa,
	Route,
	RC;

// Set flag indicating interrupt in progress

    DRV$AST_IN_PROGRESS = TRUE;

// Check for bogosity

    if (! dev_config->dc_online)
	{
//~~	Send_2_Operator(%ASCID "XE ARP receive AST when offline");
	DRV$AST_IN_PROGRESS = FALSE;
	return;
	};

// check status of receive

    ARbuf = XE_Int->xei$arp_buffer;
    if (((ARbuf->ar_ios0 != SS$_NORMAL) &&
       (ARbuf->ar_ios0 != SS$_DATAOVERUN)))
	// Error from DEC driver
	if (ARbuf->ar_ios0 == SS$_ABORT)
	    DRV$OPR_FAO("XE abort, cmd_status=!XW, error_summary=!XB",
  			ARbuf->ar_ios2,ARbuf->ar_ios3);
	else
	    DRV$Error_FAO("XE ARP receive error (ugh), EC = !XL",ARbuf->ar_ios0);

#if 0
    if ($$LOGF(LOG$PHY))
	XE_LOG("XE ARP receive",0,ARbuf->phys$2);
#endif

// Packet is OK at the hardware level - hand it up to the ARP module

    xearp$input(XE_Int,ARbuf->ar_data);

// restart the arp receive
//!!HACK!!// what's the EFN for?
    RC = exe$qio(ARPEFN,XE_Int->xei$arp_io_chan,
		 IO$_READVBLK,ARbuf->ar_ios0,
		 xe_arprcv, XE_Int,
		 ARbuf->ar_data,ARP_MAX_LEN*4,0,0,
		 ARbuf->phys$1,0);
    if (RC != SS$_NORMAL)
	XE$ERR(XE_Int,"XE ARP read failure, RC = !XL",RC);

    DRV$AST_IN_PROGRESS = FALSE;
    }


//SBTTL "ARP transmit routine"

void xe$arp_xmit(XE_Int,arbuf,arlen,dest)

// Finish packaging of ARP packet and transmit it.
// ARBUF points to the ARP data portion of the packet - header space has been
// reserved before it, if needed.

	struct XE_Interface_Structure * XE_Int;
    {
    signed long
      rc;
	struct XE_IOSB_STRUCTURE * ios;

    DRV$NOINT ;

// 0 for destination means he wants us to do a broadcast.

    if (dest == 0)
	dest = XE_BROADCAST;

// Write the ARP to the network (don't need to build any header info)

    rc = exe$qiow(0,XE_Int->xei$arp_io_chan,IO$_WRITEVBLK,
		ios, 0, 0, arbuf, arlen, 0, 0, dest, 0);
    if (rc != SS$_NORMAL)
	{
	XE$ERR(XE_Int,"XE ARP xmit failure, RC = !XL",rc);
	Signal(rc);
	}
    else
	if ($$LOGF(LOG$PHY))
	    XE_LOG("XE ARP xmit",0,dest);
    DRV$OKINT ;
    }

//SBTTL "Perform device specific DUMP functions"

XE$dump(dev_config, funct, arg, buffer, sizeAdrs)
    {
extern	XE$ARP_DUMP();


    switch (funct)
      {
	case XEDMP$ARP_Entry:
	    {
	      sizeAdrs = XE$ARP_DUMP ( arg, buffer, sizeAdrs); // check .size
	    return SS$_NORMAL;
	    };
	    break;
	default:
	  return 0;
      }
    }



drv$transport_init (void)
// Initialize the transport information/entry vector
// Must be done at run time to avoid ADDRESS fixups...
    {

    // Fill in the EtherNet description string
    XE_descriptor[0] = strlen(XE_description);
    XE_descriptor[1] = XE_description;

    // Provide the XEDRV entry points
    DRV$Device_Info->DI$Init	= XE$init;
    DRV$Device_Info->DI$Xmit	= xe$xmit;
    DRV$Device_Info->DI$Dump	= XE$dump;
    DRV$Device_Info->DI$Check	= XE$CHECK;

    return DRV$Device_Info;
    }

