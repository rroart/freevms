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
//TITLE "CONFIG - Configure the network ACP"
/*

Module:

	CONFIG  - Network ACP configuration

Facility:

	Configure the the ACP according the information extracted from the
	configuration file "INET$CONFIG", generally "config.txt".

Abstract:

	Handle the aspects of physical network device management.
	Here you will find the table(dev_config) which IP accesses to send
	datagrams.  Also device configuration and initialization routines are
	located here.  The init routine here should not be confused with the
	actual device init routines found in the driver modules.  Device init
	here implies the routine which will call all device init routines for
	those devices which have been properly configured during the device
	configuration phase of start-up. Other initializations handled by this
	module include memory manager block preallocation, gateway definition,
	name server definition, and the settings of various internal control
	variables such as "IP_FORWARDING' and 'LOG_STATE".

Author:

	Original version by Stan Smith, Tim Fallon Fall/Winter, 1982.
	This version by Vince Fuller, CMU-CSD, Spring/Summer, 1986
	Copyright (c) 1986,1987, Vince Fuller and Carnegie-Mellon University

Modification History:

*** Begin USBR change log ***

6,6	24-Dec-1991	Henry W. Miller 	USBR
	Make LOG_THRESHOLD and ACT_THRESHOLD configurable variables.

6.5d	28-Jan-1990	Henry W. Miller 	USBR
	Make WINDOW_DEFAULT and ACK_THRESHOLD configurable variables.

6.5c	13-Jan-1990	Henry W. Miller 	USBR
	Make ICMPTTL, IPTTL, TCPTTL, UDPTTL configurable variables.

*** Begin CMU change log ***

6.5b	12-Nov-1990	Bruce R. Miller		CMU NetDev
	Added the RPC keyword to configure RPC services.
	Added AUTH keyword to do UIC mapping (for NFS)

6.5a	28-Aug-1990     Henry W. Miller USBR
	Added DEFTTL configuration variable.

6.5	21-Sep-90	Bruce R. Miller		CMU NetDev
	Modifications from Mark Berryman, SAIC.COM
	Added code for proxy ARPs and restored clone device code.

6.5	02-Feb-1990	Bruce R. Miller		CMU NetDev
	Removed compile-time device support.
	Added run-time device support.

6.5	20-Oct-1989	Bruce R. Miller		CMU NetDev
	Added Activity Logging stuff.  Copied from normal logging stuff.

6.3	13-MAR-1989	Dale Moore	CMU-CS
	Added Keep Alive stuff.  (I could have sworn that I added
	this stuff before, but I musta lost it.)

6.2B 7-Oct-88, Edit by Robert Smart
	Fix test for number of interfaces (John Mann fix).

6.2A 21-Jun-88, Edit by Peter Dewildt
	Add X25 driver support
	Add SLIP driver support
	Add DECNET driver support

6.2  29-Oct-87, Edit by VAF
	Flush TMPJNL and PRMJNL privs - they no longer exist in VMS 4.6.

6.1  31-Jul-87, Edit by VAF
	Change network config file to include device type field, as distinct
	from device name field. Add TELNET_SERVICE variable.

6.0  23-Mar-87, Edit by VAF
	Add variable MAX_RECV_DATASIZE.
	Replace three sizes of packet buffers with two.

5.9  24-Feb-87, Edit by VAF
	Add support for mailbox name server (MBX_SERVER keyword). Also
	reorganized the parsing of WKS stuff.

5.8  19-Feb-87, Edit by VAF
	Flush support for GREEN and DOMAIN.

5.7  17-Feb-87, Edit by VAF
	Make improvements to the error reporting.

5.6  12-Feb-87, Edit by VAF
	Add support for domain servers.

5.5   9-Feb-87, Edit by VAF
	Add support for specifying process priviliges in WKS entries.

5.4   6-Feb-87, Edit by VAF
	Change references to DEUNA/DEQNA to be "DEC Ethernet". Add support for
	VAXstation-2000 "ES" device (same as XE/XQ).

5.3   5-Feb-87, Edit by VAF
	Add support for network access control.

5.2  12-Nov-86, Edit by VAF
	Add support for WKS records in the config file.

5.1  11-Nov-86, Edit by VAF
	Add routine to set various parameter variables in the config file.
	Currently supported are IP_FORWARDING and FQ_MAX.

5.0  12-Sep-86, Edit by VAF
	Add support for "cloned" devices.

4.9   8-Aug-86, Edit by VAF
	Change name server initialization code to call GREEN_CONFIG.

4.8   4-Aug-86, Edit by VAF
	Add code to initialize name server table.

4.7  23-Jun-86, Edit by VAF
	Add code to initialize IP forwarding state.

4.6  29-Apr-86, Edit by VAF
	Put device checking timers in here.

4.5  21-Apr-86, Edit by VAF
	Phase II of flushing XPORT - Use $FAO instead of XPORT string stuff.

4.4  18-Apr-86, Edit by VAF
	Add new I/O utilities (IOUTIL.BLI module)
	Add EN (3MB ethernet) device
	Report device status after init routine called

4.3  15-Apr-86, Edit by VAF
	Support for variable-length hardware addresses.
	Read physical address as one string of Hex digits.
	Make a byte-swapped copy of the IP address for ARP's use.

4.2   8-Mar-86, Edit by VAF
	Logging flag values are in hex now.

4.1   6-Mar-86, Edit by VAF
	Rewrite to flush all traces of XPORT.

4.0  21-Feb-86, Edit by VAF
	Give each network interface its own IP address and network mask.
	Reorganize initialization code for easier modification.
	Don't preset "dev_config table", flush "NL_xxx" routines.
	Add support for gateways

*** End CMU change log ***

1.0 [1-5-82] tim fallon

	Device configuration was buried inside of IP as we only had one
	network device at the time (hyperchannel).

2.0 [4-10-82] stan smith, tim fallon.

	Moved device configuration out of IP into this module.  Idea was to
	make IP device independent; such that IP accessed all network device
	driver routines (not to be confused with vms kernel level device drivers)
	in the same fashion (same calling conventions via dev_config table).
	Devices are configured (dev_config initialized) by macro invocations
	during devconfig.bli compilation.  Actual reason behind all this change
	was that ethernet were comming to tek and we needed to be able to support
	multiple network devices (hyperchannel and ethernet) from the same
	network acp.

2.1 [4-20-83] stan smith

	Developed routine "configure_network_devices", one no longer has to
	recompile devconfig.bli to include a new ethernet or hyperchannel
	device.  Dev_config table is now initialized from the file logical
	name "tcp$network_device_file".  Each non-comment line of the file
	describes a network device (vms name, used in $assign system service;
	physical device address and device dependent bits).

3.0 [4-28-83] stan smith

	Expanded device configuration to include memory mgmt config. Routine
	"configure_network_Devices" has been renamed to configure_acp.  Format
	of "devconfig.txt" now "config.txt" file has been expanded.  First
	field is now a keyword field, processing is dispatched via this
	keyword.

3.1 [8-2-85] Noelan Olson

	Continue to run if no network devides were configured.  Favor to Stan.

3.11 [11-18-85] Rick Watson U.Texas

	Added ARP transponder code for Deuna device
*/

#if 0
MODULE CONFIG(IDENT="6.6",ZIP,OPTIMIZE,
	      ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			      NONEXTERNAL=LONG_RELATIVE),
	      LIST(NOEXPAND,NOREQUIRE,ASSEMBLY,	OBJECT,BINARY))
#endif

extern 	void OPR_FAO(long, ...);
extern 	void ERROR_FAO(long, ...);
extern 	void FATAL_FAO(long, ...);

#include	<starlet.h>	// VMS system definitions
     // not yet #include "CMUIP_SRC:[CENTRAL]NETXPORT";	// String descriptor stuff
#include "netvms.h"		// Special VMS definitions
#include "structure.h"	// Data structures
#include	"tcpmacros.h"	// ACP-wide macros
#include "snmp.h"		// MIB definitions
#include <cmuip/central/include/netcommon.h> // Common Defs
#include <cmuip/central/include/netconfig.h> // Device configuration defs

#include <ssdef.h>
#include <descrip.h>
#include <fabdef.h>
#include <rabdef.h>
#include <prcdef.h>
#include <prvdef.h>
#include <pqldef.h>

#include <asm/uaccess.h>

#ifndef NOKERNEL
#define sys$open exe$open
#define sys$close exe$close
#define sys$get exe$get
#define sys$exit exe$exit
#define sys$connect exe$connect
#define sys$disconnect exe$disconnect
#endif

#if 0
extern  void    OPR_FAO();
#endif
extern  void    send_2_operator();
extern     LIB$GET_VM ();
extern  void    SWAPBYTES();
extern     GET_IP_ADDR();
extern     GET_DEC_NUM();
extern     GET_HEX_BYTES();
extern     GET_HEX_NUM();
extern  void    ASCII_DEC_BYTES();
extern  void    ASCII_HEX_BYTES();

#if 0
    STR[] = CH$PTR(UPLIT(%ASCIZ %STRING(%REMAINING))) %,
    STRMOVE(THESTR,DPTR) =		// Strmove - STRMOVE(STRING,BP)
	CH$MOVE(sizeof(THESTR),STR(THESTR),DPTR) %,
    STREQL(THESTR,LITSTR) =		// Compare to literal string
	CH$EQL(sizeof(LITSTR),STR(LITSTR),sizeof(LITSTR),THESTR) %,
#endif
#define    STREQLZ(THESTR,LITSTR) 	/* Compare to literal string (ASCIZ) */ \
	(0==strncmp(THESTR,LITSTR,sizeof(LITSTR))) // or other, or min/max?

#if 0
	CH$EQL(sizeof(LITSTR)+1,STR(LITSTR),sizeof(LITSTR)+1,THESTR)
#endif

#define    INIT_DYNDESC(D) \
	{ \
	((struct dsc$descriptor*)&D)->dsc$w_length = 0; \
	((struct dsc$descriptor*)&D)->dsc$b_dtype = DSC$K_DTYPE_T; \
	((struct dsc$descriptor*)&D)->dsc$b_class =  DSC$K_CLASS_D; \
	((struct dsc$descriptor*)&D)->dsc$a_pointer = 0; \
	};


// Module-wide definitions

#define     RECLEN   512
#define     STRLEN   128
#define     STRSIZ   STRLEN // was: CH$ALLOCATION(STRLEN)

// Define file reading blocks

static char     CFBUF [RECLEN];

static signed long
    linptr,
  linlen;

static struct _fabdef    CFFAB_ = { fab$l_fna : "INET$CONFIG", fab$b_fns : 11, fab$b_rfm : FAB$C_VAR }; // check was fnm
static struct _fabdef * CFFAB = & CFFAB_;
static struct _rabdef    CFRAB_ = { rab$l_fab : &CFFAB_,
			    rab$l_ubf : CFBUF,
			    rab$w_usz : RECLEN,
				rab$b_rac : RAB$C_SEQ }; // check
static struct _rabdef * CFRAB = & CFRAB_;

 void    config_err();
 void    Init_Device();
 void    init_memgr();
 void    Init_Gateway();
 void    Init_NameServer();
 void    init_logging();
 void    init_activity_logging();
 void    init_forwarding();
 void    init_variable();
 void    init_mbxresolver();
 void    Init_WKS();
 void    Init_RPC();
 void    Init_Auth();
 void    init_local_host();
    GETFIELD();
    PARSE_NULLFIELD();
 void    SKIPTO();
 void    SKIPWHITE();



// Here we set up the IPACP_Interface.  It provides entry points,
// literals and global pointers to any modules which may be loaded
// at run-time.  See NETCONFIG.REQ for a complete explaination of
// this structure.

 IPACP_Info_Structure IPACP_Int_, * IPACP_Int=&IPACP_Int_;

// N.B. : make sure everything that needs to be defined, like the
// MAX_Physical_BufSize, *is* defined by the time this routine is called...

CNF$Define_IPACP_Interface (void)
    {
    extern
	// pointer to IPACP AST_in_progress flag (from MAIN.BLI)
	ast_in_progress,
	// IPACP nap control (from MAIN.BLI)
	sleeping,

	// IPACP Maximum Physical Buffer Size
	max_physical_bufsize,

	log_state;

	// IAPCP receive callback (from IP.BLI)
extern	ip$receive();
	// IPACP self-address recognition (from IP.BLI)
extern 	ip$isme();

	// Interrupt blocking routines (from WHERE???)
extern 	void MAIN$NOINT();
extern 	void MAIN$OKINT();

	// Error reporting routines (from CONFIG.BLI)
extern 	void CNF$Device_Error();

	// Memory allocation routines (from MEMGR.BLI)
extern 	void mm$seg_get();
extern 	void mm$seg_free();
extern 	void mm$qblk_free();

	// Formatted event logging routines (from IOUTIL.BLI)
extern 	void LOG_FAO();
extern 	void QL_FAO();

    // IAPCP receive callback.
    IPACP_Int ->  ACPI$IP_Receive 	= ip$receive;

    // pointer to IPACP sleeping flag
    IPACP_Int ->  ACPI$Sleeping  	= sleeping;

    // pointer to IPACP AST_in_progress flag
    IPACP_Int ->  ACPI$AST_in_progress  	= &ast_in_progress;

    // Interrupt blocking routines
    IPACP_Int ->  ACPI$NOINT 		= MAIN$NOINT;
    IPACP_Int ->  ACPI$OKINT 		= MAIN$OKINT;

    // Error reporting routines
    IPACP_Int ->  ACPI$Device_Error 	= CNF$Device_Error;

    // IPACP self-address recognition
    IPACP_Int ->  ACPI$IP_ISME 		= ip$isme;

    // Memory allocation routines
    IPACP_Int ->  ACPI$Seg_Get 		= mm$seg_get;
    IPACP_Int ->  ACPI$Seg_Free 		= mm$seg_free;
    IPACP_Int ->  ACPI$QBlk_Free 	= mm$qblk_free;

    // Provide event logging entry points
    IPACP_Int ->  ACPI$LOG_STATE 	= log_state;	// pointer
    IPACP_Int ->  ACPI$LOG_FAO 		= LOG_FAO;
    IPACP_Int ->  ACPI$QL_FAO 		= QL_FAO;
    IPACP_Int ->  ACPI$OPR_FAO 		= OPR_FAO;
    IPACP_Int ->  ACPI$ERROR_FAO 	= ERROR_FAO;
    IPACP_Int ->  ACPI$FATAL_FAO 	= FATAL_FAO;

    // IPACP max physical buffer size
    IPACP_Int ->  ACPI$MPBS  		= max_physical_bufsize;

    return IPACP_Int;
    }



/* Here we initialize all of the variable that can be set by the config
   file.
*/

void init_vars (void)
    {
    extern
	struct IP_group_MIB_struct * IP_group_MIB;

    IP_group_MIB->IPMIB$ipForwarding = 2;	// Just a host, no forwarding
    }


//Sbttl "Device Configuration table definitions"
/*

Here we configure the known network devices.  Each entry in the dev_config
table represents a known network device (eg, hyperchannel, ethernet etc).
If your system doesn't have the specified device, then leave the table entry
alone or replace the nonexistant entry with one of your own choosing.  All
the device_init macro does is to place the device initialization and send
routine addresses in the appropriate device-type entry.
See the DC_Fields portion of STRUCTURE.DEF for details.

*/

void No_Check(ndx)

// Dummy routine for non-checked devices.

    {
    return;
    }

static signed long
    dev_attn;

// gotta declare this PSect on the quad boundary so-as we can use
// self relative queue instructions on it...

#if 0
PSECT
    GLOBAL = quads(ALIGN(3));
#endif

signed long
    dev_count;
Device_Configuration_Entry dev_config_tab[DC_Max_Num_Net_Devices]; /* check PSECT(quads) ALIGN(3), */



cnf$device_stat ( Inx, RB_A )
//
// Dump out a Device configuration entry.
//
    {
      d$dev_dump_return_blk * RB = RB_A;

    if ((Inx < 0) || (Inx > DC_Max_Num_Net_Devices-1) ||
	(dev_config_tab[Inx].dc_valid_device == 0))
	return -1;

    // fill in the simple stuff...
    RB->du$dev_address = dev_config_tab[Inx].dc_ip_address;
    RB->du$dev_netmask = dev_config_tab[Inx].dc_ip_netmask;
    RB->du$dev_network = dev_config_tab[Inx].dc_ip_network;
    RB->du$dev_pck_xmit = 0;
    RB->du$dev_pck_recv = 0;

    // Start a new block so we can use BIND to simplify the code.
    {
      struct dsc$descriptor * DevNam  = &dev_config_tab[Inx].dc_devname ;
      struct dsc$descriptor * DevSpec = &dev_config_tab[Inx].dc_devspec;
    signed long
	StrLen;

    // Copy device name
    StrLen = DEVNAM_MAX_SIZE;
    if (StrLen > DevNam->dsc$w_length)
	StrLen = DevNam->dsc$w_length;
    RB->du$devnam_len = StrLen;
    CH$MOVE ( StrLen , DevNam->dsc$a_pointer , RB->du$devnam_str );

    // Copy device-specific field
    StrLen = DEVSPEC_MAX_SIZE;
    if (StrLen > DevSpec->dsc$w_length)
	StrLen = DevSpec->dsc$w_length;
    RB->du$devspec_len = StrLen;
    CH$MOVE ( StrLen , DevSpec->dsc$a_pointer , RB->du$devspec_str );
    }

    return D$DEV_DUMP_BLKSIZE;
    }


cnf$device_list ( RB )
//
// Dump out the list of valid devices.
//
	 long * RB; // check
    {
      signed long I,
	RBIX;

    RBIX = 1;
    for (I=0;I<=DC_Max_Num_Net_Devices-1;I++)
	if (dev_config_tab[I].dc_valid_device != 0)
	    {
	    RB[RBIX] = I;
	    RBIX = RBIX + 1;
	    };
    // First element of list is the count.
    RB[0] = RBIX - 1;

    // return total size in bytes.
    return RBIX * sizeof(long); // check 
    }


//SBTTL "Configure Network ACP"

/*
Function:

	Read the IPACP configuration file, "INET$CONFIG", containing all info
	about the ACP initialization parameters, including: devices, memory
	management parameters, logging state, variables, well-known services,
	etc. Each line of the file contains a keyword followed by the values
	associated with the keyword. Lines beginning in ";" are ignored as
	comment lines.

Side Effects:

	Initializes the DEV_CONFIG table, the globals for memory management,
	and calls all of the configuration routines in other modules.
*/

void CNF$Configure_ACP (void)
    {
      signed long
	RC,
	cfield [STRSIZ],
	cflen,
	cptr;
      mm_segment_t fs;

// Initialize all the variables before they are set by the config script

    init_vars();

// OPEN the file "config.TXT" & read/decode network device data into blockvector
// dev_config and memory management info.

#ifndef NOKERNEL
    fs = get_fs();
    set_fs(get_ds());

    long prev_xqp_fcb = get_xqp_prim_fcb();
    long prev_x2p_fcb = get_x2p_prim_fcb();
#endif
    if (BLISSIFNOT(BLISSIF(RC = sys$open(CFFAB,0,0)) &&
	    BLISSIF(RC = sys$connect(CFRAB,0,0))))
	{
	$DESCRIPTOR(dsc, "Unable to access INET$CONFIG:");
	send_2_operator(&dsc);
	sys$exit(RC);
	};

// Extract information from each non-comment line of the file

    dev_count = 0;		// No devices

#ifndef NOKERNEL 
#define RMS_WORKAROUND
#endif
#ifdef RMS_WORKAROUND
    struct file * filp;
    long xqp_fcb = get_xqp_prim_fcb();
    long x2p_fcb = get_x2p_prim_fcb();
    if (xqp_fcb!=prev_xqp_fcb)
      filp=xqp_fcb;
    else
      filp=x2p_fcb;
    int offs = 0;
    // temp workaround: with ods2 it read an empty second block
    while ((offs=CFRAB->rab$w_rsz=rms_kernel_read(filp,offs,CFBUF,512))>0 && CFBUF[0])
#else
    while (sys$get(CFRAB,0,0)&1)
#endif
	{
	signed long
	    chr;
	linptr = CH$PTR(CFRAB->rab$l_ubf,0);
	linlen = CFRAB->rab$w_rsz;

// if (1st char of line is a "!" then is it a comment & we ignore it.

#if 1	
	chr = CH$RCHAR(CH$PTR(CFRAB->rab$l_ubf,0));
#else
	int d=CH$PTR(CFRAB->rab$l_ubf);  // check
	chr = CH$RCHAR(d);
#endif
	if ((chr != '!') && (chr != ';'))
	    {
	    if (linlen > (RECLEN-1))
		linlen = RECLEN-1;
	    cptr = CH$PLUS(linptr,linlen);
	    CH$WCHAR_A(0,cptr);		// Make ASCIZ

// Read the first field (category keyword) and dispatch to the appro processing
// routine.

// roart: had to add a label and goto, since $get here read all
	    again:

	    cflen = GETFIELD(cfield);
	    cptr = CH$PTR(cfield,0);
	    if (STREQLZ(cptr,"DEVICE_INIT"))
		    Init_Device();
		else if (STREQLZ(cptr,"MEMGR_INIT"))
		    init_memgr();
		else if (STREQLZ(cptr,"GATEWAY"))
		    Init_Gateway();
		else if (STREQLZ(cptr,"NAME_SERVER"))
		    Init_NameServer();
		else if (STREQLZ(cptr,"LOGGING"))
		    init_logging();
		else if (STREQLZ(cptr,"ACTIVITY"))
		    init_activity_logging();
		else if (STREQLZ(cptr,"IP_FORWARDING"))
		    init_forwarding();
		else if (STREQLZ(cptr,"VARIABLE"))
		    init_variable();
		else if (STREQLZ(cptr,"MBX_RESOLVER"))
		    init_mbxresolver();
		else if (STREQLZ(cptr,"WKS"))
		    Init_WKS();
		else if (STREQLZ(cptr,"RPC"))
		    Init_RPC();
		else if (STREQLZ(cptr,"AUTH"))
		    Init_Auth();
		else if (STREQLZ(cptr,"LOCAL_HOST"))
		    init_local_host();
		else
		    config_err(ASCID("Unknown keyword"));
	    };
	if ( *(char*)linptr == '\n') {
	  linptr++;
	  if (*(char*)linptr != 0)
	    goto again;
	}
	};

    sys$disconnect(CFRAB,0,0);
    sys$close(CFFAB,0,0);

#ifndef NOKERNEL
    set_fs(fs);
#endif

// make sure we did some device configuration here as networks without devices
// are not really very interesting.

    if (dev_count <= 0)
	ERROR$FAO("No network devices detected in INET$CONFIG");
    }

void config_err(EMSG)
//
// Handle error in configuration file. Give the error message and exit.
//
    {
    FATAL$FAO("CONFIG - !AS in line:!/!_!AD",
	      EMSG,CFRAB->rab$w_rsz,CFRAB->rab$l_ubf);
    }

void Init_Device (void)

// Handle a DEVICE_INIT entry in the INET$CONFIG file.
// Parses the device description and adds to DEV_CONFIG table.

    {
      //extern 	LIB$CALLG();
extern 	LIB$FIND_IMAGE_SYMBOL();
extern 	STR$APPEND		();
extern 	STR$CASE_BLIND_COMPARE	();
extern 	STR$COPY_DX		();
 Device_Info_Structure * devinfo;
 Device_Configuration_Entry * dev_config;
 int (*Image_Init)();
 signed long I,
//	tmp,
	rc,
	ipaddr,
	ipmask,
	argv[1],
	devtype[STRSIZ],
	devtlen,
	devstr[STRSIZ],
	devslen,
devspec[STRSIZ],
	devidx;		// device index into devconfig.
struct dsc$descriptor dev_desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :devstr}, * dev_desc=&dev_desc_;
struct dsc$descriptor	devtyp_desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :devtype}, * devtyp_desc=&devtyp_desc_;
struct dsc$descriptor	dev_spec_desc_  = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :devtype}, * dev_spec_desc = &dev_spec_desc_;

// skip over delimiter.

    SKIPTO(':');

// Dev_config table overflow?
// Table index = 0 to DC_Max_Num_Net_Devices-1.

    devidx = dev_count;
    if (dev_count > DC_Max_Num_Net_Devices-1 )
      FATAL$FAO("Too many network devices in INET$CONFIG, max = DC_Max_Num_Net_Devices");
    dev_count = dev_count+1;
    dev_config = dev_config_tab[devidx].dc_begin;

// Clear all flags

    dev_config->dc_flags = 0;
    dev_config->dc_clone_dev = -1;

// Get device driver type

    devtyp_desc -> dsc$w_length  = GETFIELD(devtyp_desc -> dsc$a_pointer );
//    devtlen = GETFIELD(devtype);
    SKIPTO(':');
   
//    cptr = CH$PTR(devtype);

//!!HACK!!// For now, we just include the ethernet code inorderto test
// it more easily (all symbols included, plus code is now in main image PSECT)


    $DESCRIPTOR(ether,"ETHER");
    if ((STR$CASE_BLIND_COMPARE(devtyp_desc,&ether) == 0))
	{
extern	    drv$transport_init();

	Image_Init = drv$transport_init;
	}
// Get name of shared-image IP-transport
    else if ((rc = LIB$FIND_IMAGE_SYMBOL(
		devtyp_desc,
		ASCID("DRV$TRANSPORT_INIT"),
		&Image_Init,
		ASCID("CMUIP_ROOT:[SYSLIB].EXE")
		) != SS$_NORMAL))
	{
	Image_Init = 0;
	Signal(rc);
	config_err(ASCID("Trouble accessing device support image."));
	};

    if ((Image_Init != 0))
	{	// Set up the vector
	devinfo = LIB$CALLG(argv,Image_Init);

	dev_config->dc_rtn_Init = devinfo->DI$Init;
	dev_config->dc_rtn_Xmit = devinfo->DI$Xmit;
	dev_config->dc_rtn_Dump = devinfo->DI$Dump;
	dev_config->dc_rtn_check = devinfo->DI$Check;
	};

// Get vms device name, used
// by the device init routine to $assign the device.
    dev_desc -> dsc$w_length  = GETFIELD(dev_desc -> dsc$a_pointer );
//    $INIT_DYNDESC( dev_config ->dc_devname );
    INIT_DYNDESC( dev_config ->dc_devname );
    STR$COPY_DX (&dev_config ->dc_devname, dev_desc);
    $DESCRIPTOR(colon,":");
    STR$APPEND (&dev_config ->dc_devname, &colon);
// skip over field terminator.
    SKIPTO(':');

// Get vms device specific information, used by the device go receive
// and initiate connections.
    dev_desc -> dsc$w_length  = GETFIELD(dev_desc -> dsc$a_pointer );
//    $INIT_DYNDESC( dev_config->dc_devspec );
    INIT_DYNDESC( dev_config ->dc_devspec );
    STR$COPY_DX (&dev_config ->dc_devspec, dev_desc);
// skip over field terminator.
    SKIPTO(':');

// Set hardware address/device dependant words
//    IF GET_HEX_BYTES(dev_config->dc_phy_size,linptr,
//		     CH$PTR(dev_config->dc_phy_addr)) < 0 THEN
//	config_err(ASCID("Bad device address"));
//    SKIPTO(':');

// Get device IP address

    if (GET_IP_ADDR(&linptr,&ipaddr) < 0)
	config_err(ASCID("Bad device IP address"));
    dev_config->dc_ip_address = ipaddr;
    SKIPTO(':');

// Get device IP network mask

    if (GET_IP_ADDR(&linptr,&ipmask) < 0)
	config_err(ASCID("Bad device IP mask"));
    dev_config->dc_ip_netmask = ipmask;

// Set device IP network number

    dev_config->dc_ip_network = ipmask & ipaddr;

// mark the device as valid so initialization routine will execute the device
// init rtn.

    dev_config->dc_valid_device = TRUE;

// Initialize all of the other fields for this device

    dev_config->dc_Send_Qhead = &dev_config->dc_Send_Qhead;
    dev_config->dc_send_Qtail = &dev_config->dc_Send_Qhead;
    dev_config->dc_online = FALSE;

// See if this device name is a duplicate.

    if (devidx > 0)
	for (I=0;I<=(devidx-1);I++)
	    {
	    // If match, then mark this device as a clone of the it.
	    if (STR$CASE_BLIND_COMPARE(&dev_config_tab[I].dc_devname,
			&dev_config_tab[devidx].dc_devname) == 0)
		{
		dev_config->dc_is_clone = TRUE;
		dev_config->dc_clone_dev = I;
		break;
		};
	    };
    }

void Init_Gateway (void)
//!!HACK!!// Make gateway struct dynamic.
// Handle a GATEWAY entry in the INET$CONFIG file.
// Parses gateway description and adds to GWY_TABLE
// Gateway entries are of the form:
//   GATEWAY:<gwy_name>:<gwy_address>:<gwy-network>:<gwy-netmask>
// Where (these are also the names of the GWY_TABLE fields):
//   gwy_name	Name of the gateway (for debugging purposes)
//   gwy_address	IP address of gateway *must be locally-connected*
//   gwy_network	Network number served by gateway (0 means any/default)
//   gwy_netmask	AND mask for comparing network number
//   gwy_status	Gateway status. Initialized to "up" (nonzero) by this code.

    {
extern	void ip$gwy_config();
    signed long
	GWYname [STRSIZ],
	GWYaddr,
	GWYnet,
	GWYnetmask,
	tmp;
struct dsc$descriptor 	GWY_Name_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :GWYname}, * GWY_Name_Desc = &GWY_Name_Desc_;

// Skip delimiter

    SKIPTO(':');

// First, get the gateway name.

    GWY_Name_Desc -> dsc$w_length  = GETFIELD(GWYname);

// Next, the gateway address

    SKIPTO(':');
    if (GET_IP_ADDR(&linptr,&GWYaddr) < 0)
	config_err(ASCID("Bad gateway address"));

// Next, the network number behind the gateway

    SKIPTO(':');
    if (GET_IP_ADDR(&linptr,&GWYnet) < 0)
	config_err(ASCID("Bad gateway network number"));

// Next, the network mask for that network

    SKIPTO(':');
    if (GET_IP_ADDR(&linptr,&GWYnetmask) < 0)
	config_err(ASCID("Bad gateway mask"));

// Tell IP about this gateway

    ip$gwy_config(GWY_Name_Desc,GWYaddr,GWYnet,GWYnetmask);
    }

void Init_NameServer (void)

// Handle a Name_Server entry in the INET$CONFIG file.
// Parses Name Server description and adds to NS_TABLE
// Name Server entries are of the form:
//   NAME_SERVER:<NS_name>:<NS_address>
// Where (these are also the names of the NS_TABLE fields):
//   NS_name	Name of the gateway (for debugging purposes)
//   NS_address	IP address of gateway *must be locally-connected*
//   NS_status	Gateway status. Initialized to "up" (nonzero) by this code.

    {
    signed long
	NSname [STRSIZ],
	NSnlen,
	NSaddr,
	tmp;

// Skip delimiter

    SKIPTO(':');

// First, get the name server name

    NSnlen = GETFIELD(NSname);

// Next, the name server address

    SKIPTO(':');
    if (GET_IP_ADDR(&linptr,&NSaddr) < 0)
	config_err(ASCID("Bad name server address"));

// Add this entry to the name server database

    OPR$FAO("%IPACP: Obsolete keyword NAME_SERVER found in INET$CONFIG");
    }

void init_memgr (void)
//
// Handle MEMGR-INIT entry in the INET$CONFIG file.
// Specifies memory-manager initialization parameters.
//
    {
    extern unsigned char
	qblk_count_base,
	uarg_count_base ,
	min_seg_count_base ,
	max_seg_count_base ;

// Get # of Queue blocks to preallocate

    SKIPTO(':');
    if (GET_DEC_NUM(&linptr,&qblk_count_base) < 0)
	config_err(ASCID("Bad integer value"));

// Get # of UARG blocks to preallocate

    SKIPTO(':');
    if (GET_DEC_NUM(&linptr,&uarg_count_base) < 0)
	config_err(ASCID("Bad integer value"));

// Get # of minimum-size packet buffers to preallocate

    SKIPTO(':');
    if (GET_DEC_NUM(&linptr,&min_seg_count_base) < 0)
	config_err(ASCID("Bad integer value"));

// get # of maximum-size packet buffers to preallocate

    SKIPTO(':');
    if (GET_DEC_NUM(&linptr,&max_seg_count_base) < 0)
	config_err(ASCID("Bad integer value"));
    }



void init_logging (void)

// Set initial logging state.

    {
extern	void LOG_CHANGE();
    signed long
	logstate;

// Skip terminator

    SKIPTO(':');

// Get log state value

    if (GET_HEX_NUM(&linptr,&logstate) < 0)
	config_err(ASCID("Bad hex value for log state"));

// Set log state

    LOG_CHANGE(logstate);
    }



void init_activity_logging (void)

// Set initial activity logging state.

    {
extern	void ACT_CHANGE();
    signed long
	logstate;

// Skip terminator

    SKIPTO(':');

// Get log state value

    if (GET_HEX_NUM(&linptr,&logstate) < 0)
	config_err(ASCID("Bad hex value for activity log state"));

// Set log state

    ACT_CHANGE(logstate);
    }



//SBTTL "Initialize IP forwarding state"

void init_forwarding (void)
    {
    signed long
	ipstate;
    extern
	struct IP_group_MIB_struct * IP_group_MIB;

// Skip terminator

    SKIPTO(':');

// Get forwarding state value

    if (GET_HEX_NUM(&linptr,&ipstate) < 0)
	config_err(ASCID("Bad hex value for forwarding state"));

// Set state for IP module

    IP_group_MIB->IPMIB$ipForwarding = ipstate;
    }



//SBTTL "Initialize various configuration variables"

void init_variable (void)
    {
    extern
	fq_max,
	syn_wait_count,
	access_flags,
	max_recv_datasize,
	default_mss,
	telnet_service,
	rpc_service,
	snmp_service,
	keep_alive,
	retry_count,
	max_local_ports,
	max_conn,
	max_gateways,
	window_default,
	ack_threshold,
	icmpttl,
	ipttl,
	tcpttl,
	udpttl,
	act_threshold,
    log_threshold;
extern	struct IP_group_MIB_struct * IP_group_MIB;
    signed long
	varname [STRSIZ],
	varlen,
	varptr,
	varval;

// Skip terminator

    SKIPTO(':');

// Get the variable name

    varlen = GETFIELD(varname);
    varptr = CH$PTR(varname,0);

// Skip the terminator

    SKIPTO(':');

// Get the variable value

    if (GET_DEC_NUM(&linptr,&varval) < 0)
	config_err(ASCIDNOT("Bad variable value"));

// Check the variable name & set it.
//!!HACK!!// Document these!
	if (STREQLZ(varptr,"IP_FORWARDING"))
	    IP_group_MIB->IPMIB$ipForwarding = varval;
	else if (STREQLZ(varptr,"FQ_MAX"))
	    fq_max = varval;
	else if (STREQLZ(varptr,"SYN_WAIT_COUNT"))
	    syn_wait_count = varval;
	else if (STREQLZ(varptr,"ACCESS_FLAGS"))
	    access_flags = varval;
	else if (STREQLZ(varptr,"MAX_TCP_DATASIZE"))
	    max_recv_datasize = varval;
	else if (STREQLZ(varptr,"DEFAULT_MSS"))
	    default_mss = varval;
	else if (STREQLZ(varptr,"TELNET_SERVICE"))
	    telnet_service = varval;
	else if (STREQLZ(varptr,"RPC_SERVICE"))
	    rpc_service = varval;
	else if (STREQLZ(varptr,"SNMP_SERVICE"))
	    snmp_service = varval;
	else if (STREQLZ(varptr,"KEEP_ALIVE"))
	    keep_alive = varval;
	else if (STREQLZ(varptr,"RETRY_COUNT"))
	    retry_count = varval;
	else if (STREQLZ(varptr,"MAX_LOCAL_PORTS"))
	    max_local_ports = varval;
	else if (STREQLZ(varptr,"MAX_CONN"))
	    max_conn = varval;
	else if (STREQLZ(varptr,"MAX_GATEWAYS"))
	    max_gateways = varval;
	else if (STREQLZ(varptr,"ICMPTTL"))
	    icmpttl = varval;
	else if (STREQLZ(varptr,"IPTTL"))
	    ipttl = varval;
	else if (STREQLZ(varptr,"TCPTTL"))
	    tcpttl = varval;
	else if (STREQLZ(varptr,"UDPTTL"))
	    udpttl = varval;
	else if (STREQLZ(varptr,"WINDOW_DEFAULT"))
	    window_default = varval;
	else if (STREQLZ(varptr,"ACK_THRESHOLD"))
	    ack_threshold = varval;
	else if (STREQLZ(varptr,"ACT_THRESHOLD"))
	    act_threshold = varval;
	else if (STREQLZ(varptr,"LOG_THRESHOLD"))
	    log_threshold = varval;
    else
	    config_err(ASCID("Unknown variable name"));
    }



//SBTTL "Process definitions"
/*
    Define some keyword tables for parsing options to $CREPRC
*/

// Macro for defining two-word privilege bit

#define    PRVIX(BYT) BYT

#define     PVSIZE   2
#define     PVIX   0
#define     PVAL   1


// Define the STATUS keywords

char *	 STATNAMES[] = {
	ASCIDNOT("SSRWAIT"),PRC$M_SSRWAIT,
	ASCIDNOT("SSFEXCU"),PRC$M_SSFEXCU,
	ASCIDNOT("PSWAPM"),PRC$M_PSWAPM,
	ASCIDNOT("NOACNT"),PRC$M_NOACNT,
	ASCIDNOT("BATCH"),PRC$M_BATCH,
	ASCIDNOT("HIBER"),PRC$M_HIBER,
	ASCIDNOT("IMGDMP"),PRC$M_IMGDMP,
	ASCIDNOT("NOUAF"),PRC$M_NOUAF,
	ASCIDNOT("NETWRK"),PRC$M_NETWRK,
	ASCIDNOT("DISAWS"),PRC$M_DISAWS,
	ASCIDNOT("DETACH"),PRC$M_DETACH,
	ASCIDNOT("INTER"),PRC$M_INTER,
	ASCIDNOT("NOPASSWORD"),PRC$M_NOPASSWORD};

// Define the PRIVILEGE keywords

char * PRIVNAMES[] = {
	ASCIDNOT("*"),-1,
	ASCIDNOT("ALLSPOOL"),PRVIX(PRV$V_ALLSPOOL),
	ASCIDNOT("BUGCHK"),PRVIX(PRV$V_BUGCHK),
	ASCIDNOT("BYPASS"),PRVIX(PRV$V_BYPASS),
	ASCIDNOT("CMEXEC"),PRVIX(PRV$V_CMEXEC),
	ASCIDNOT("CMKRNL"),PRVIX(PRV$V_CMKRNL),
	ASCIDNOT("DETACH"),PRVIX(PRV$V_DETACH),
	ASCIDNOT("DIAGNOSE"),PRVIX(PRV$V_DIAGNOSE),
	ASCIDNOT("DOWNGRADE"),PRVIX(PRV$V_DOWNGRADE),
	ASCIDNOT("EXQUOTA"),PRVIX(PRV$V_EXQUOTA),
	ASCIDNOT("GROUP"),PRVIX(PRV$V_GROUP),
	ASCIDNOT("GRPNAM"),PRVIX(PRV$V_GRPNAM),
	ASCIDNOT("GRPPRV"),PRVIX(PRV$V_GRPPRV),
	ASCIDNOT("LOG_IO"),PRVIX(PRV$V_LOG_IO),
	ASCIDNOT("MOUNT"),PRVIX(PRV$V_MOUNT),
	ASCIDNOT("NETMBX"),PRVIX(PRV$V_NETMBX),
	ASCIDNOT("NOACNT"),PRVIX(PRV$V_NOACNT),
	ASCIDNOT("OPER"),PRVIX(PRV$V_OPER),
	ASCIDNOT("PFNMAP"),PRVIX(PRV$V_PFNMAP),
	ASCIDNOT("PHY_IO"),PRVIX(PRV$V_PHY_IO),
	ASCIDNOT("PRMCEB"),PRVIX(PRV$V_PRMCEB),
	ASCIDNOT("PRMGBL"),PRVIX(PRV$V_PRMGBL),
	ASCIDNOT("PRMMBX"),PRVIX(PRV$V_PRMMBX),
	ASCIDNOT("PSWAPM"),PRVIX(PRV$V_PSWAPM),
	ASCIDNOT("READALL"),PRVIX(PRV$V_READALL),
	ASCIDNOT("SECURITY"),PRVIX(PRV$V_SECURITY),
	ASCIDNOT("SETPRI"),PRVIX(PRV$V_SETPRI),
	ASCIDNOT("SETPRV"),PRVIX(PRV$V_SETPRV),
	ASCIDNOT("SHARE"),PRVIX(PRV$V_SHARE),
	ASCIDNOT("SHMEM"),PRVIX(PRV$V_SHMEM),
	ASCIDNOT("SYSGBL"),PRVIX(PRV$V_SYSGBL),
	ASCIDNOT("SYSLCK"),PRVIX(PRV$V_SYSLCK),
	ASCIDNOT("SYSNAM"),PRVIX(PRV$V_SYSNAM),
	ASCIDNOT("SYSPRV"),PRVIX(PRV$V_SYSPRV),
	ASCIDNOT("TMPMBX"),PRVIX(PRV$V_TMPMBX),
	ASCIDNOT("UPGRADE"),PRVIX(PRV$V_UPGRADE),
	ASCIDNOT("VOLPRO"),PRVIX(PRV$V_VOLPRO),
	ASCIDNOT("WORLD"),PRVIX(PRV$V_WORLD)};


// Define process quotas

struct QUOTA
{
char  QTA$TYPE;	// Quota type
unsigned long    QTA$VALUE;	// Quota value
 };

#define     QUOTA_SIZE   $FIELD_SET_SIZE
#define     QUOTA_BLEN   5

#define     MAXQUOTA   15		// Max number of quotas to specify

// Define a keyword table for parsing the process quotas.

char * QUOTANAMES[] = {
	ASCIDNOT("ASTLM"),	PQL$_ASTLM,
	ASCIDNOT("BIOLM"),	PQL$_BIOLM,
	ASCIDNOT("BYTLM"),	PQL$_BYTLM,
	ASCIDNOT("CPULM"),	PQL$_CPULM,
	ASCIDNOT("DIOLM"),	PQL$_DIOLM,
	ASCIDNOT("ENQLM"),	PQL$_ENQLM,
	ASCIDNOT("FILLM"),	PQL$_FILLM,
	ASCIDNOT("JTQUOTA"),PQL$_JTQUOTA,
	ASCIDNOT("PGFLQUOTA"),PQL$_PGFLQUOTA,
	ASCIDNOT("PRCLM"),	PQL$_PRCLM,
	ASCIDNOT("TQELM"),	PQL$_TQELM,
	ASCIDNOT("WSDEFAULT"),PQL$_WSDEFAULT,
	ASCIDNOT("WSEXTENT"),PQL$_WSEXTENT,
	  ASCIDNOT("WSQUOTA"),PQL$_WSQUOTA};

KEY_VALUE(KEYTAB,KEYLEN,KEYSTR)
//
// Get keyword value from keyword string.
//
	long * KEYTAB;
    {
      long I;
    struct dsc$descriptor * CURSTR;

    for ( I = 0;  I<= (KEYTAB[-1]-1); I+=2 )
	{
	CURSTR = KEYTAB[I];
	if (CH$EQL(KEYLEN,KEYSTR,
		  CURSTR->dsc$w_length,CURSTR->dsc$a_pointer))
	    return KEYTAB[I+1];
	};
    config_err(ASCID("Bad keyword field"));
    return -1;
    }

PARSE_PRCPRIVS(PRVBLK)
//
// Parse a comma-separated list of privilege keywords.
// Returns the number of keywords seen.
//
	long PRVBLK[2];
    {
    signed long
	PRIVCNT,
	 *PRIVPTR,
	PRIVBUF[STRSIZ],
	PLEN,
	CHR;

// Initialize to "no privileges"

    PRVBLK[0] = 0;
    PRVBLK[1] = 0;
    PRIVCNT = 0;

// Loop, parsing keywords.

    while (TRUE)
	{
	PLEN = GETFIELD(PRIVBUF);
	if (PLEN == 0)
	    break;
	PRIVCNT = PRIVCNT + 1;
	PRIVPTR = KEY_VALUE(PRIVNAMES,PLEN,CH$PTR(PRIVBUF,0));
	if (PRIVPTR == -1)
	    {
	    PRVBLK[0] = -1;
	    PRVBLK[1] = -1;
	    }
	else
	  PRVBLK[PRIVPTR[PVIX]] = PRVBLK[PRIVPTR[PVIX]] | PRIVPTR[PVAL]; // check
	SKIPWHITE();

// Check for end of field separator

	CHR = CH$RCHAR_A(linptr);
	if (CHR == ':')
	    break;
	};

// Return count of fields found

    return PRIVCNT;
    }


PARSE_PRCQUOTAS(QLIST,QMAX)
//
// Parse a comma-separated list of quota values.
// Returns the count of quotas seen.
//
    {
	struct QUOTA * qptr;
    signed long
	QUOTBUF [STRSIZ],
	QUOTLEN,
	QUOTCNT,
	QUOTYPE,
	QUOTVAL,
	CHR;

// Initialize to none

    QUOTCNT = 0;
    CH$FILL(0,QMAX*QUOTA_BLEN,QLIST);
    qptr = QLIST;

// Loop, parsing keywords.

    while (TRUE)
	{
	QUOTLEN = GETFIELD(QUOTBUF);
	if (QUOTLEN == 0)
	    break;

// Parse the keyword

	QUOTYPE = KEY_VALUE(QUOTANAMES,QUOTLEN,CH$PTR(QUOTBUF,0));
	SKIPWHITE();

// Parse the separator - must be an equal sign

	CHR = CH$RCHAR_A(linptr);
	if (CHR != '=')
	    config_err(ASCID("Missing = in quota description"));
	SKIPWHITE();

// Parse the quota value

	if (GET_DEC_NUM(&linptr,&QUOTVAL) < 0)
	    config_err(ASCID("Missing or bad quota value"));
	SKIPWHITE();

// Set the value in the quotalist

	qptr->QTA$TYPE = QUOTYPE;
	qptr->QTA$VALUE = QUOTVAL;

// Increment to next entry, checking for overflow

	QUOTCNT = QUOTCNT + 1;
	if (QUOTCNT >= (QMAX-1))
	    config_err(ASCID("Exceeded max number of quota keywords"));
	qptr = qptr + QUOTA_BLEN;
	SKIPWHITE();

// Check for terminator

	CHR = CH$RCHAR_A(linptr);
	if (CHR == ':')
	    break;
	};

// Put the terminator entry in the list

    qptr->QTA$TYPE = PQL$_LISTEND;
    qptr->QTA$VALUE = 0;

// Return the number of entries in the list.

    return QUOTCNT+1;
    }

PARSE_PRCSTATUS (void)
//
// Parse a comma-separated list of process status flags.
// Returns the values OR'ed together.
//
    {
    signed long
	STATVAL,
	STATBUF[STRSIZ],
	STATLEN,
	CHR;

// Initialize value to all off

    STATVAL = 0;

// Loop, parsing the flags

    while (TRUE)
	{
	STATLEN = GETFIELD(STATBUF);
	if (STATLEN == 0)
	    break;
	STATVAL = STATVAL | KEY_VALUE(STATNAMES,STATLEN,CH$PTR(STATBUF,0));
	SKIPWHITE();
	CHR = CH$RCHAR_A(linptr);
	if (CHR == ':')
	    break;
	};

// Finall, return the complete value

    return STATVAL;
    }

//SBTTL "Init_WKS - Add a WKS entry"
		     
void Init_WKS (void)

// Handle a WKS entry in the INET$CONFIG file.
// Parses WKS description and calles Seg$WKS_Config to add the info.
// WKS entries are of the form:
//   WKS:<port>:<procname>:<imagename>:<status>:<privs>:<priority>:
// (cont) <queue-limit>:<maxsrv>

    {
extern	STR$COPY_DX();
extern	void seg$wks_config();
    signed long
	CHR,LPTR,
	WKSprname [STRSIZ],
	WKSimname [STRSIZ],
	QUOTAS[MAXQUOTA*QUOTA_BLEN],
	WKSInput [STRSIZ],
	WKSOutput[STRSIZ],
	WKSError[STRSIZ],
	WKSport,
	WKSstat,
	wkspriv[2],
	WKSprior,
	WKSqlim,
	WKSmaxsrv;
struct dsc$descriptor WKSprname_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :WKSprname}, *WKSprname_Desc=&WKSprname_Desc;
struct dsc$descriptor WKSimname_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :WKSimname}, *WKSimname_Desc=&WKSimname_Desc_;
struct dsc$descriptor Quota_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :QUOTAS}, *Quota_Desc=&Quota_Desc_;
struct dsc$descriptor WKSInput_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :WKSInput}, *WKSInput_Desc=&WKSInput_Desc_;
struct dsc$descriptor WKSOutput_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :WKSOutput}, *WKSOutput_Desc=&WKSOutput_Desc_;
struct dsc$descriptor WKSError_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :WKSError}, *WKSError_Desc=&WKSError_Desc_;

// Skip delimiter
    SKIPTO(':');

// First, get the port number
    if (GET_DEC_NUM(&linptr,&WKSport) < 0)
	config_err(ASCID("Bad WKS port value"));
    SKIPTO(':');

// Next, process name
    WKSprname_Desc -> dsc$w_length  = GETFIELD(WKSprname);
    SKIPTO(':');

// Next, image name - N.B. image name may not have ":" in it.
    WKSimname_Desc = GETFIELD(WKSimname);
    SKIPTO(':');

// Next, $CREPRC status flags. Comma-separated list of keywords.
    WKSstat = PARSE_PRCSTATUS();

// Next, $CREPRC priviliges. Comma-separated list of keywords.
    PARSE_PRCPRIVS(wkspriv);

// Parse the process quotas
    if (PARSE_NULLFIELD())
      SKIPTO(':');
    else
	Quota_Desc->dsc$w_length = PARSE_PRCQUOTAS(QUOTAS,MAXQUOTA)*QUOTA_BLEN;

// Parse the process INPUT file
    if (PARSE_NULLFIELD())
	{
	  $DESCRIPTOR(nla0,"_NLA0:");	
SKIPTO(':');
STR$COPY_DX ( WKSInput_Desc , &nla0 );
	}
    else
	{
	WKSInput_Desc -> dsc$w_length  = GETFIELD(WKSInput);
SKIPTO(':');
	};

// Parse the process OUTPUT file
    if (PARSE_NULLFIELD())
	{
	  $DESCRIPTOR(nla0,"_NLA0:");
	SKIPTO(':');
STR$COPY_DX ( WKSOutput_Desc , &nla0 );
	}
    else
	{
	WKSOutput_Desc -> dsc$w_length  = GETFIELD(WKSOutput);
SKIPTO(':');
	};

// Parse the process ERROR stream
    if (PARSE_NULLFIELD())
	{
	SKIPTO(':');
	STR$COPY_DX ( WKSError_Desc , ASCID("_NLA0:") );
	}
    else
	{
	WKSError_Desc -> dsc$w_length  = GETFIELD(WKSError);
	SKIPTO(':');
	};

// Next, priority value
    if (GET_DEC_NUM(&linptr,&WKSprior) < 0)
	config_err(ASCID("Bad priority value"));
    SKIPTO(':');

// Next the number of outstanding SYN's we will allow for this server.
    if (GET_DEC_NUM(&linptr,&WKSqlim) < 0)
	config_err(ASCID("Bad queue limit value"));    

// Now, find out what the maximum number of servers permitted is.  If zero
// or error, then set to unlimited (0).  (This is actually GLBOAL_MAXSRV as
// defined in TCP_SEGIN.BLI.
//
	WKSmaxsrv = 0;
	if ((CH$RCHAR_A(linptr) == ':'))
	    if (GET_DEC_NUM(&linptr,&WKSmaxsrv) < 0)
		WKSmaxsrv = 0;

// Tell the configuration routine in SEGIN about this guy.

    seg$wks_config(WKSport, WKSprname_Desc, WKSimname_Desc, WKSstat,
		wkspriv,WKSprior,WKSqlim,WKSmaxsrv,Quota_Desc,
		WKSInput_Desc,WKSOutput_Desc,WKSError_Desc);
    }

//SBTTL "Init_RPC - Add an RPC entry"
		     
void Init_RPC (void)

// Handle an RPC entry in the INET$CONFIG file.
// Parses RPC description and calles RPC$Config to add the info.
// RPC entries are of the form:
//   RPC:<name>:<prog>:<vers>:<prot>:<port>:<imagename>

    {
extern	STR$COPY_DX	();
extern	RPC$CONFIG();
    signed long
	RC,
	CHR,LPTR,
	RPCprog,
	RPCvers,
	RPCprot,
	RPCport,
	RPCname[STRSIZ],
      RPCimname [STRSIZ];
struct dsc$descriptor RPCname_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :RPCname}, *RPCname_Desc=&RPCname_Desc_;
struct dsc$descriptor RPCimname_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :RPCimname}, *RPCimname_Desc=&RPCimname_Desc_;

// Skip delimiter
    SKIPTO(':');

// First, get the service name
    RPCname_Desc -> dsc$w_length  = GETFIELD(RPCname);
    SKIPTO(':');

// Next, get the program number
    if (GET_DEC_NUM(&linptr,&RPCprog) < 0)
	config_err(ASCID("Bad RPC program number"));
    SKIPTO(':');

// Next, get the version number
    if (GET_DEC_NUM(&linptr,&RPCvers) < 0)
	config_err(ASCID("Bad RPC version number"));
    SKIPTO(':');

// Next, get the protocol number
    if (GET_DEC_NUM(&linptr,&RPCprot) < 0)
	config_err(ASCID("Bad RPC protocol number"));
    SKIPTO(':');

// Next, get the port number
    if (GET_DEC_NUM(&linptr,&RPCport) < 0)
	config_err(ASCID("Bad RPC port number"));
    SKIPTO(':');

// Next, image name - N.B. image name may not have ":" in it.
    RPCimname_Desc = GETFIELD(RPCimname);
//    SKIPTO(':');

// Tell the configuration routine in the RPC module about this guy.

    RC=RPC$CONFIG( RPCname_Desc,RPCprog,RPCvers,RPCprot,RPCport,RPCimname_Desc);
    if (RC < 0) config_err(ASCID("Can not accept RPC config entry"));
    }

//SBTTL "Init_Auth - Add an authorization entry"
		     
void Init_Auth (void)

// Handle an AUTH entry in the INET$CONFIG file.
// Parses AUTH description and calles RPC$Config_Auth to add the info.
// AUTH entries are of the form:
//   AUTH:<[UIC]>:<uid>:<gid>:<hostname>

    {
extern	STR$COPY_DX	();
extern	RPC$CONFIG_AUTH();
    signed long
	RC,
	CHR,LPTR,
	AUTHuic,
	AUTHuid,
	AUTHgid,
      AUTHhostname [STRSIZ];
struct dsc$descriptor AUTHhostname_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :AUTHhostname}, *  AUTHhostname_Desc = & AUTHhostname_Desc_;

// Skip delimiter
    SKIPTO(':');

// First, get the VMS uic number
    if (GET_HEX_NUM(&linptr,&AUTHuic) < 0)
	config_err(ASCID("Bad AUTH uic number"));
    SKIPTO(':');

// Next, get the net uid number
    if (GET_DEC_NUM(&linptr,&AUTHuid) < 0)
	config_err(ASCID("Bad AUTH uid number"));
    SKIPTO(':');

// Next, host name - N.B. host name may not have ":" in it.
    AUTHhostname_Desc = GETFIELD(AUTHhostname);
//    SKIPTO(':');

// Tell the configuration routine in the RPC module about this guy.

    RC=RPC$CONFIG_AUTH(AUTHuic, AUTHuid, AUTHgid, AUTHhostname_Desc);
    if ((RC < 0)) config_err(ASCID("Can not accept AUTH config entry"));
    }

//SBTTL "Init_MBXResolver - Define the system name resolver process"

void init_mbxresolver (void)
//
// Define the system name resolver process.
// MBX_RESOLVER:<image>:<priority>:<flags>:<privs>:<quotas>
//
    {
extern	void NML$CONFIG();
    signed long
	imagename [STRSIZ],
	STATFLAGS,
	PRIORITY,
	PRIVS[2],
	QUOTAS[MAXQUOTA*QUOTA_BLEN];
struct dsc$descriptor ImageName_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :imagename}, *ImageName_Desc = &ImageName_Desc_;
struct dsc$descriptor Quota_Desc_ = {
				dsc$w_length :0,
				dsc$b_dtype :DSC$K_DTYPE_Z,
				dsc$b_class :DSC$K_CLASS_Z,
				dsc$a_pointer :QUOTAS}, * Quota_Desc = &Quota_Desc;

// Skip delimiter

    SKIPTO(':');

// Parse the image name

    ImageName_Desc -> dsc$w_length  = GETFIELD(imagename);
    SKIPTO(':');

// Parse the priority value

    if (GET_DEC_NUM(&linptr,&PRIORITY) < 0)
	config_err(ASCID("Bad priority value"));
    SKIPTO(':');

// Parse the process status values

    STATFLAGS = PARSE_PRCSTATUS();

// Parse the process privileges

    PARSE_PRCPRIVS(PRIVS);

// Parse the process quotas

    Quota_Desc -> dsc$w_length  = PARSE_PRCQUOTAS(QUOTAS,MAXQUOTA) * QUOTA_BLEN;

// Call the name resolver configuration routine

    NML$CONFIG(ImageName_Desc, PRIORITY, STATFLAGS, PRIVS,
	       Quota_Desc);
    }

//SBTTL "Add an entry to the local hosts list"

void init_local_host (void)
//
// Format is Local_Host:<ip-address>:<ip-mask>
// Reads the data and calls USER$ACCESS_CONFIG(ipaddr,ipmask)
//
    {
extern	void user$access_config();
    signed long
	hostaddr,
	hostmask;

// First, read the host address

    SKIPTO(':');
    if (GET_IP_ADDR(&linptr,&hostaddr) < 0)
	config_err(ASCID("Bad host address"));

// Then, the host mask

    SKIPTO(':');
    if (GET_IP_ADDR(&linptr,&hostmask) < 0)
	config_err(ASCID("Bad host mask"));

// Got the info. Call routine to add to the list

    user$access_config(hostaddr,hostmask);
    }

//SBTTL "Parsing utility routines"

void SKIPTO(TCHR)

// Skip characters past a given character in line buffer.

    {
    signed long
	CHR;
    while ((CHR = CH$RCHAR_A(linptr)) != 0)
	if (CHR == TCHR)
	    return;
    config_err(ASCID("SKIPTO failure (EOL)"));
    }

void SKIPWHITE (void)

// Skip over whitespace in the current record.

    {
    signed long
        CHR,LPTR;
    LPTR = linptr;
    while ((CHR = CH$RCHAR_A(LPTR)) != 0)
	{
	if (CHR != ' ')
	    return;
	linptr = LPTR;
	};
	config_err(ASCID("SKIPWHITE failure (EOL)"));
    }

GETFIELD(FLDADR)
    
// Read a field the current record (linptr). A field is defined as the a
// sequence of non-whitespace characters up to whitespace or a field
// delimiter (currently ",', '=' and ':"). Returns field size on success
// with the field text (case-folded) at FLDADR.
// Returns 0 on failure (end of record).

    {
    signed long
        PTR,CHR,CNT,DSTPTR;

    SKIPWHITE();
    PTR = linptr;
    DSTPTR = CH$PTR(FLDADR,0);
    CNT = 0;
    while (((CHR = CH$RCHAR_A(PTR)) != 0) &&
	  (CHR != ',') && (CHR != ':') && (CHR != ' ') &&
	   (CHR != '='))
	{
	linptr = PTR;
	CNT = CNT+1;
	if ((CHR >= 'a') && (CHR <= 'z'))
	    CHR = CHR-('a'-'A');
	CH$WCHAR_A(CHR,DSTPTR);
	};
    CH$WCHAR_A(0,DSTPTR);
    if (CNT == 0)
	config_err(ASCID("Bad or null field found"));
    return CNT;
    }

PARSE_NULLFIELD (void)
    
// Read a field the current record (linptr). A field is defined as the a
// sequence of non-whitespace characters up to whitespace or a field
// delimiter (currently ",', '=' and ':"). Returns field size on success
// with the field text (case-folded) at FLDADR.
// Returns 0 on failure (end of record).

    {
    signed long
        PTR,CHR;

    SKIPWHITE();
    PTR = linptr;

    if (((CHR = CH$RCHAR_A(PTR)) != 0) &&
	  (CHR != ',') && (CHR != ':') && (CHR != ' ') &&
    (CHR != '=')) return 0;
    else return 1;

    }

//Sbttl "Initialize Network Devices as described in dev_config."
/*

Function:

	Initialize all known network devices from dev_config table.

Inputs:

	None.

Implicit Inputs:

	dev_config table entries.

Outputs:
]
	None.

Side Effects:

	if the dev_config table entry is marked as a valid entry then
	the device init routine is called.  This routine will assign the
	device & place the IO channel in the dev_config table entry dc_io_chan.
*/


void CNF$Net_Device_Init (void)
    {
    extern
	retry_count,
	max_physical_bufsize;
    signed long
	J,
      cdev;
	struct Device_Configuration_Entry * dev_config;

    dev_attn = 0;		// No devices need attention
    for (J=0;J<=(dev_count-1);J++)
	{
	if (dev_config_tab[J].dc_valid_device)
	    {
DESC$STR_ALLOC(oprmsg,80);
DESC$STR_ALLOC(ipstr,20);
		DESC$STR_ALLOC(phystr,50);
		struct dsc$descriptor * devnam;
	    struct dsc$descriptor
      stastr;

// Initialize this device

	    if (dev_config_tab[J].dc_is_clone)
     cdev = dev_config_tab[J].dc_clone_dev;
	    else
		{
		cdev = J;
		(dev_config_tab[J].dc_rtn_Init)(dev_config_tab[J].dc_begin,
				IPACP_Int,
				retry_count,
				max_physical_bufsize);
		};

// And tell the operator the status.

	    devnam = &dev_config_tab[J].dc_devname;
//	    ASCII_Hex_Bytes(phystr,dev_config_tab[cdev].dc_phy_size,
//			    dev_config[cdev,dc_phy_addr],
//			    phystr->dsc$w_length);
	    ASCII_DEC_BYTES(ipstr,4,dev_config_tab[J].dc_ip_address,
			    &ipstr->dsc$w_length);
	    if (dev_config_tab[cdev].dc_online)
     stastr = ASCID("Online");
	    else
		stastr = ASCID("Offline");

	    OPR$FAO("Net device !SL is !AS, IP=!AS (!AS)",
		    J,devnam,ipstr,&stastr);

	    dev_config_tab[cdev].dcmib_ifIndex = cdev;

	    };
	};
    }


//SBTTL	"Check devices needing attention"

 void    CNF$Check_Devices();

static signed long
    CHECKTIME [2] ={-20000000,-1}; // 2 seconds in the future

void CNF$Check_Sched (void)
    {
    sys$setimr(	0,  CHECKTIME,
		CNF$Check_Devices, 0, 0);
    }

void CNF$Device_Error (void)
    {
    if (dev_attn == 0)
	CNF$Check_Sched();		// Schedule a check
    dev_attn = dev_attn+1;	// And bump count of wedged devices
    }

void CNF$Check_Devices (void)
    {
    register i;
    for (i=0;i<=(dev_count-1);i++)
	if (dev_config_tab[i].dc_valid_device && 
	   (! dev_config_tab[i].dc_online) &&
	   (! dev_config_tab[i].dc_is_clone))
	     // Try to restart
	    dev_attn = dev_attn + (dev_config_tab[i].dc_rtn_check)(
						dev_config_tab[i].dc_begin);

    if (dev_attn != 0)
	CNF$Check_Sched();		// Reschedule if there is still a problem
    }



//
// A simple routine to return our local IP address.  By using this, we
// cut down on external access to the device configuration table.
//
CNF$Get_Local_IP_addr (void)
    {
      return dev_config_tab[0].dc_ip_address;
    }
