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
//TITLE "USER - ACP user interface module"
//SBTTL "User Interface Overview"
/*

Module:

	USER

Facility:

	Top-level processing of user I/O requests.

Abstract:

	Here we process the ACP input queue. Basic operation is to Dequeue a
	user IO request, process it & loop until there are no more requests.
	User requests cover the basic network functions user: Connection open,
	close abort, info, status, name lookup, data transmit, data receive.
	There are also a number of privileged maintenance functions which are
	require the PHY_IO (physical I/O capability) to execute. Maintenance
	functions include:  Shutting down the ACP, dumping out of internal
	structures (such as connection blocks, the ARP cache, etc.).

	Note:  the routine Process_User_Requests() is the point in this
	module where IRPs enter the IPACP.

Author:

	Orignal version by Stan C. Smith, Fall 1981
	This version by	Vince Fuller, CMU-CSD, Spring/Summer, 1986
	Copyright (c) 1986,1987, Vince Fuller and Carnegie-Mellon University

Modification History:

6.7a	09-Jul-1991	Henry W. Miller		USBR
	Added STARLET for VMS 5.4.

*** Begin CMU change log ***

6.7  16-Jan-1991, Bruce R. Miller	CMU Network Development
	Added the idea of logging flag "groups" in order to handle
	new clusters of logging flags.  We were running out of
	flags, plus we need to handle ACTIVITY logging.

6.7  06-Feb-1990, Bruce R. Miller	CMU Network Development
	Changed interface to transport device-specific structures.

6.7  27-Nov-1989, Bruce R. Miller	CMU Network Development
	Added equal support for TCP, UDP, ICMP, and IP.
	This module now looks at the ACP Arg Blk to determain
	a request's protocol.  Removed all u$udp$xxxx functions
	calls.  Now all function requests are sent with a protocol
	field in the arg blk and use the u$xxxx function code.

6.7  20-Oct-1989, Bruce R. Miller	CMU Network Development
	Added Net$Event function to allow IP clients to log significant
	activities in a centrally located log file.

6.6   8-Sep-87, Edit by VAF
	Change definition of M$INTERNAL from dangerous 0 value to 25.

6.5  30-Jul-87, Edit by VAF
	Use $$KCALL macro for calling $CMKRNL routines.

6.4  10-Jun-87, Edit by VAF
	In USER$CHECK_ACCESS, check that local port is GEQ Well_Known_LP_Start
	(so that wildcarded local ports are OK).

6.3  23-Mar-87, Edit by VAF
	Use two standard-sized packet buffers instead of three.

6.2   3-Mar-87, Edit by VAF
	Flush obsolete junk from IOSB format. Rename IOSB fields.

6.1   2-Mar-87, Edit by VAF
	Add new GTHST function for obtaining local host info.

6.0  19-Feb-87, Edit by VAF
	Rewrite GTHST to use the new name lookup functions. Flush the GTHST
	request queue since we no longer need it (thanks to NML$STEP).

5.9  18-Feb-87, Edit by VAF
	Move UCB extension literals from TCP_USER.BLI.
	Fix bug in connection STATUS code - was returning the foreign port
	instead of the local port.

5.8  12-Feb-87, Edit by VAF
	Modifications for domain service. Fix bug in privilege checking - open
	of WKS local port is only privileged if foreign port is wild.

5.7   5-Feb-87, Edit by VAF
	Add code for network access checking.

5.6  28-Aug-86, Edit by VAF
	Add dump functions for reading ARP cache.

5.5  13-Aug-86, Edit by VAF
	Add dump functions for listing UDP connections.
	Move TCB dump functions into TCB_User where they belong.

5.4  10-Aug-86, Edit by VAF
	Convert GTHST routines to use green protocol routines.

5.3   9-Aug-86, Edit by VAF
	Remove SET_HOSTS from here - it is more complicated with name servers.
	Get local host name from LOCAL_NAME global, don't do address to name
	translation.

5.2  31-Jul-86, Edit by VAF
	Add Net_Connection_Info UDP/TCP common routine.

5.1  29-Jul-86, Edit by VAF
	Move a couple of routines back in here that will be used by both
	TCP and UDP (SET_HOSTS and GET_USER_LP).

5.0  23-Jul-86, Edit by VAF
	Split-off all TCP specific functions into TCP_USER module.
	Add hooks for UDP implementation.

4.9  17-Jul-86, Edit by VAF
	Debugging code for tracking input segments.

4.8  17-Jul-86, Edit by VAF
	Make CLOSE_TCB take reference to TCB pointer so it can clear it.
	Log foreign ports in NET$OPEN, log TCB address and conn idx.
	Log TCB address in all user functions.

4.7  16-Jul-86, Edit by VAF
	Separate hair for setting host addresses out from INIT_TCB.

4.6  15-Jul-86, Edit by VAF
	Return TS$BADSEQ in stats dump.
	Return future queue count in TCB dump.

4.5  11-Jul-86, Edit by VAF
	Return more memory manager counters.

4.4   9-Jul-86, Edit by VAF
	Return TS$Future_dups now.
	Change local port generation algorithm to start at clock base and
	go incrementally from there.
	Change initial sequence number generation to use clock base in
	upper 16-bits of sequence number.

4.3   7-Jul-86, Edit by VAF
	Fix two bugs - in TCB_OK, check for legal value in VALID_TCB table.
	In NET$RECEIVE - handle LAST-ACK state (give connection closing error).

4.2   1-Jul-86, Edit by VAF
	Add support for "future" segments queue in INIT_TCB and
	TCP$KILL_P}ING_REQUESTS.
	Add new counters, make available to dump function.

4.1  25-Jun-86, Edit by VAF
	Make the UCB hold a TCB "index" (index into VALID_TCB table) and don't
	ever let the user look at real TCB addresses.
	Change and simplify "local conn id" validation in all user routines.
	Don't use index 0 of VALID_TCB table.

4.0  23-Jun-86, Edit by VAF
	Start adding support for UCB extension to hold TCB pointer.

3.9  12-Jun-86, Edit by VAF
	Do buffering of user sends here not in S}_DATA routine. It may cost
	a little extra buffer copying, but it will probably speed things up.

3.8  11-Jun-86, Edit by VAF
	In USER$Purge_All_IO call TCP$KILL_P}ING_REQUESTS - don't duplicate
	all of that effort.

3.7  10-Jun-86, Edit by VAF
	Know about new TCB cells for keeping track of segments and data on
	network/user queues.
	Make TCP$Purge_Send_Queue know about buffered Qblocks.

3.6   6-Jun-86, Edit by VAF
	Add some debugging code.

3.5  22-May-86, Edit by VAF
	Use VMS error message facility.

3.4   8-May-86, Edit by VAF
	Make CLOSE function block user until we get to Time-Wait state.
	User may request immediate-close mode by specifying CL$NOWAIT.
	Add Last_ACK state.
	Make VMS$CANCEL routine initiate a close, not reset connection.

3.3   2-May-86, Edit by VAF
	In NET$S}, don't call S}_DATA - it will be done soon enough.

3.2  22-Apr-86, Edit by VAF
	Phase II of flushing XPORT - use $FAO for doing output formatting.

3.1  21-Apr-86, Edit by VAF
	Make all connections wait for open by default.
	Add a new bit to the open call - OP$NoWait - for immediate return

3.0  19-Apr-86, Edit by VAF
	Flush call to S}_DATA in main user processing routine.
	A lot of code in this module needs work - it shouldn't be diddling tcb
	states and such.

2.9  18-Apr-86, Edit by VAF
	New GET_IP_ADDR routine.

2.8   7-Apr-86, Edit by VAF
	New logging stuff.

2.7   4-Apr-86, Edit by VAF
	GTHST user function - get host information.

2.6   3-Apr-86, Edit by VAF
	Flush all of the UDP stuff. We have to think about how to do it right.

2.5   2-Apr-86, Edit by VAF
	Move some code that belongs here out of SEGIN and TCP.

2.4  31-Mar-86, Edit by VAF
	Add UDP open and UDP close stubs.

2.3  17-Mar-86, Edit by VAF
	Redo connection timeout stuff.
	Fix bug in checksum algorithm.
	Move a bunch of code out of this module (more to be done).

2.2  10-Mar-86, Edit by VAF
	Changs to handle overlapping segments.
	**N.B. the TCB[Dasm_*] crap should be flushed.

2.1   7-Mar-86, Edit by VAF
	New log file handling stuff.

2.0  21-Feb-86, Edit by VAF
	Flush "known_hosts" crud, replace with hostname module.
	Flush "myinternetaddrs" crud, replace with dev_config entries.
	Add Local_Host to TCB - it is determined at connect-open time and
	speficies which interface is used for a connection.
	Other miscellaneous changes in an effort to bring this code into the
	real Internet world.
	Allow open by IP host number ("a.b.c.d" as host name)

*** End CMU change log ***

1.1  [10-1-81] stan smith
	original version.

1.2  [7-15-83] stan
	force byte-size on some external literals.

1.3  [7-28-83] stan
	new net$dump function: one converts a host name to a known_hosts table
	index, other uses the known_hosts index to retreive stats for the
	specified host.
1.4  [9-14-83] stan
	"decode_network_host" now scans host_alias table attempting to match
	host name specified in net$open call.

1.5  [1-24-84] stan
	rtn: retrans_enqueue, place an upper bound on the retransmission timeout
	value.  Round-trip time can become very large, prevent lengthy delays.

1.6  [5-30-85] noelan olson
	Modified to use the table of internet addresses.  Must use the proper
	one to calculate checksum when gatewaying between networks.

1.61  Rick Watson U.Texas
	Find available port for user.
*/


//SBTTL "Module Definition"

MODULE USER(IDENT="6.7a",LANGUAGE(BLISS32),
	    ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			    NONEXTERNAL=LONG_RELATIVE),
	    LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
	    OPTIMIZE,OPTLEVEL=3,ZIP)=
{

#include "CMUIP_SRC:[CENTRAL]NETXPORT";	// BLISS transportablity package
!LIBRARY "SYS$LIBRARY:STARLET";	// VMS system definitions ** Not STARLET **
#include "SYS$LIBRARY:LIB";	// VMS system definitions ** Not STARLET **
#include "CMUIP_SRC:[central]NETERROR";	// Network error messages
#include "CMUIP_SRC:[CENTRAL]NETCOMMON";	// Various VMS specifics
#include "CMUIP_SRC:[CENTRAL]NETVMS";		// Various VMS specifics
#include "STRUCTURE";		// TCB & Segment Structure definitions
!LIBRARY "TCP";			// TCP related definitions
#include "TCPMACROS";		// Include local macros
#include "CMUIP_SRC:[central]NETCONFIG"; // Transport devices interface

!*** N.B. Special UCB extensions used by IP device driver	***
!*** Take care to always match definitions in IPDRIVER.MAR	***
!*** Referenced by: TCP_USER.BLI, UDP.BLI, ICMP.BLI		***

signed long LITERAL
    UCB$Q_DDP	= $BYTEOFFSET(UCB$Q_DEVDEP}),
    UCB$L_CBID	= UCB$Q_DDP,	// Control Block associated with UCB
    UCB$L_EXTRA	= UCB$Q_DDP+4;	// Extra longword for later expansion

extern signed long
    Log_State,
    Time_2_Exit,
    MYPID,			// maclib.mar
    Local_Name : DESC$STR,
    DEV_Config_Tab: Device_Configuration_Table,

// Memory manager dynamic allocation counts (defined in memgr.bli)

    QB_GETS,
    UA_GETS,
    MIN_GETS,
    MAX_GETS,
    QB_MAX,
    UA_MAX,
    MIN_MAX,
    MAX_MAX,

// TCP statistics counters (see tcp.bli global defs).

    TS$UIR,
    TS$ACO,
    TS$PCO,
    TS$DBX,
    TS$DBR,
    TS$SR,
    TS$SX,
    ts$duplicate_segs,
    ts$retrans_segs,
    ts$rpz_rxq,
    ts$oorw_segs,
    ts$future_rcvd,
    ts$future_used,
    ts$future_dropped,
    ts$future_dups,
    ts$seg_bad_cksum,
    ts$badseq,
    ts$servers_forked;

extern

// Rtns from MEMGR.BLI

    MM$UArg_Free: NOVALUE,

// IOUTIL.BLI

 VOID    ACT_OUTPUT,
    LOG_CHANGE,
    ACT_CHANGE,
 VOID    LOG_OUTPUT,
 VOID    LOG_TIME_STAMP,
 VOID    ACT_FAO,
 VOID    LOG_FAO,
 VOID    OPR_FAO,

// Routines From:  MACLIB.MAR

    Set_IP_device_OFFline: NOVALUE,
    User_Requests_Avail,
    VMS_IO$POST: NOVALUE,
    MovByt: NOVALUE,
    SwapBytes: NOVALUE,
    Time_Stamp,

// NMLOOK.BLI

 VOID    NML$GETNAME,
 VOID    NML$GETALST,
 VOID    NML$GETRR,
 VOID    NML$CANCEL,
 VOID    NML$STEP,

// IP.BLI

    IP_ISLOCAL;


// User Network(TCP) I/O Request arg blk Function Codes.
// *** warning ***
// user function MUST be a dense list as they are used to generate a case table.

// Important//  If you change these definitions, you must also change
// also change the corresponding definitions in the IPDRIVER as well as
// m$cancel in maclib.mar.

signed long LITERAL
    M$UNUSED	= 0: UNSIGNED(8),

    U$OPEN	= 1: UNSIGNED(8),
    U$S}	= 2: UNSIGNED(8),
    U$RECV	= 3: UNSIGNED(8),
    U$CLOSE	= 4: UNSIGNED(8),
    U$STATUS	= 5: UNSIGNED(8),
    U$ABORT	= 6: UNSIGNED(8),
    U$INFO	= 7: UNSIGNED(8),

    U$max_TCP_function = 7,	// for case limit

// GTHST (Get host info) function

    U$GTHST	= 8: UNSIGNED(8),
	GTH_LCLHST = 0,
	GTH_NMLOOK = 1,
	GTH_ADLOOK = 2,
	GTH_RRECLK = 3,
	GTH_MIN	= GTH_LCLHST,
	GTH_MAX	= GTH_RRECLK,

    U$max_user_function = 8,	// for case limit

// Privileged ACP Maintenance Functions.

    M$DUMP	= 9: UNSIGNED(8),
    M$EXIT	= 10: UNSIGNED(8),
			// extra, obsolete function.
    M$DEBUG	= 11: UNSIGNED(8),
    M$EVENT	= 12: UNSIGNED(8),
    M$SNMP	= 13: UNSIGNED(8),

    M$CANCEL	= 14: UNSIGNED(8),

// Special, internal routine function (for TVT processing)

    M$INTERNAL	= 15: UNSIGNED(8);



//SBTTL "Data for checking network access"

// Define block for storing allowed hosts.

$FIELD ACCESS_FIELDS (void)
    SET
    AC$HOST	= [$BYTES(4)],
    AC$MASK	= [$BYTES(4)]
    TES;
LITERAL
    ACCESS_SIZE = $FIELD_SET_SIZE,
    ACCESS_MAX = 20;
MACRO
    ACCESS_LIST = BLOCKVECTOR[ACCESS_MAX,ACCESS_SIZE] FIELD(ACCESS_FIELDS) %;

MACRO
    ACF$PRIVPORT = 0,1,0 %,	// Check for privileged ports
    ACF$ARPAHOST = 1,1,0 %,	// Check ARPANET_ACCESS for nonlocal hosts
    ACF$ALLOPENS = 2,1,0 %;	// Check INTERNET_ACCESS for any open
BIND
    ACF_PRIVPORT = 1,		// Bitmask for above...
    ACF_ARPAHOST = 2,		// "
    ACF_ALLOPENS = 4;		// "

signed long
    ACCESS_FLAGS  = ACF_PRIVPORT; // Flags for access checks to do

static signed long
    ACHOST_COUNT  = 0,	// Count of hosts
    ACHOSTS : ACCESS_LIST,	// List of local hosts
    ARPANET_ID  = 0,	// Identifier for ARPANET_ACCESS
    INTERNET_ID  = 0;	// Identifier for INTERNET_ACCESS
BIND
    ARPANET_STRING = %ASCID"ARPANET_ACCESS",
    INTERNET_STRING = %ASCID"INTERNET_ACCESS";

//Sbttl "Request VMS to Post users IO request."
/*

Function:

	Request VMS to post the user's IO function thus completing the
	VMS IO request.  We need to change mode to Kernel to access some
	of the VMS IO data strctures.

Inputs:

	IOSB	IO Status Block Address.
	IRP	IRP address
	UCB	UCB address

Outputs:

	None.

Side Effects:

	Process access mode is changed to KERNEL & back to user before
	this routine exits.

*/


IO$POST (IOSB, struct User_Default_Args * UArg): NOVALUE (void)
    {
    signed long
	IRP;

    XLOG$FAO(LOG$USER,"!%T IO$POST : Uarg=!XL IRP=!XL UCB=!XL!/",0,
		.UArg, UArg->UD$IRP_Adrs , UArg->UD$UCB_Adrs );

    IRP = UArg->UD$IRP_Adrs;
    UArg->UD$IRP_Adrs = 0;

    if (IRP == 0)
	{
	OPR$FAO("!%T No IRP, UCB=!XL Proto=!XB funct=!XB !/",0,
		UArg->UD$UCB_Adrs,UArg->UD$Protocol,UArg->UD$funct);
	return FALSE
	};

    $$KCALL(VMS_IO$POST,IOSB,IRP,UArg->UD$UCB_Adrs);
    };



//SBTTL "POST User IO Status"

/*

Function:

	Return I/O Status to the user processes.  Used for TCP functions
	which actually transfer data between user & TCP (S}, Receieive
	status, dump).

Inputs:

	IRP = Address of User's IO Request Packet (VMS structure).
	UCB = Address of Unit Control Blk (VMS Structure).
	VMS_Return_Code = SS$_Normal etc.
	TCP_Err, When VMS_Return_Code != SS$_Normal this is the error feild
	Bytes_Xfered = # of bytes read/written
	IO_Tag = IO request identifier (receive only)
	URG = Urgent data present (send only)
	EOL = End Of Letter (send only).

Outputs:

	None.

Side Effects:

	User's IRP is queued to VMS IO post-processing rtns.

*/


USER$POST_IO_STATUS (UARG,STATUS,NBYTES,
VOID 				   FLAGS,ICMCODE) (void)
    {
    signed long
!	IRP,
	IOSB :  NetIO_Status_Block;

// Fill in Network IO status Block

    if (STATUS != SS$_NORMAL)
	{
	IOSB->NSB$STATUS = SS$_ABORT;
	IOSB->NSB$XERROR = STATUS;
	}
    else
	{
	IOSB->NSB$STATUS = SS$_NORMAL;
	IOSB->NSB$BYTE_COUNT = NBYTES;
	IOSB->NSB$FLAGS = FLAGS;
	IOSB->NSB$ICMP_CODE = ICMCODE;
	};

// Queue IRP to IO post-processor

    IO$POST(IOSB, UARG)

    };


//SBTTL "USER Err - Return Errors to users process"

/*

Function:

	Return IPACP related errors to the user's process.  A network
	I/O status block is filled out.  Status block is similar to
	the VMS IO status block (surprize).

Inputs:

	Arg = Address of argument block.  Actually is the system
	      buffer (IRP$L_SVAPTE) pointed at by IRP.
	Err = Error code to be returned to user.

Outputs:

	Always TRUE.

Side Effects:

	IRP is queued to VMS IO post-processing.
	User argblk (ARG) is released to memory manager.

*/

USER$ERR (struct User_Default_Args * Arg,Err)
    {
    signed long
	IOSB : NETIO_STATUS_BLOCK;

// Fill in Network IO status Block

    IOSB->NSB$STATUS = SS$_ABORT;
    IOSB->NSB$BYTE_COUNT = 0;
    IOSB->NSB$XERROR = ERR;

// Make sure we have an IRP!

    if (ARG->UD$IRP_Adrs == 0)
	{
	if ($$LOGF(LOG$USER))
	    LOG$FAO("!%T No IRP, UArg = !XL UCB=!XL Proto=!XB funct=!XB !/",0,
		.ARG,ARG->UD$UCB_Adrs,ARG->UD$Protocol,ARG->UD$funct);
	return FALSE
	};
// Queue IRP to VMS I/O post-processor

    IO$POST(IOSB, ARG);
    MM$UArg_Free(Arg);			// Release user TCP arg block.

// If logging is enabled then output the user error message to the log file.

    if ($$LOGF(LOG$USER))
	LOG$FAO("!%T User error return, RC = !XL!/",0,Err);
	    
    return TRUE;
    };


//Sbttl "POST USER FUNCTION OK - Give em a "YOU DONE GOOD" "
/*

Function:

	Return to the requesting user a sucessful status on the
	requested IP function.  Returns a network IO status blk
	to the user.

Inputs:

	Arg = IPACP argument block.

Outputs:

	None.

Side Effects:

	IRP is queued to VMS IO posting routines.

*/

USER$POST_FUNCTION_OK(struct User_Default_Args * Arg): NOVALUE (void)
    {
    signed long
	IRP,
	IOSB : NetIO_Status_Block;

// Fill in Network IO status Block

    IOSB->NSB$STATUS = SS$_Normal;
    IOSB->NSB$BYTE_COUNT = 0;
    IOSB->NSB$XSTATUS = 0;

// Queue IRP to IO post-processor

    IO$POST(IOSB,Arg);
    MM$UArg_Free(Arg);		// Release user arg block.
    };

//SBTTL "Give info about a connection"

// Common routine used by TCP and UDP to return connection info
// (local/foreign host numbers,names and ports)

USER$Net_Connection_Info(struct User_Info_Args * Uargs,
			     Lcl_Host,Frn_Host,Lcl_Port,Frn_Port,
void 			     Frn_Name,Frn_Nlen) (void)
    {
    signed long
	CS: Connection_Info_Return_Block;

// Verify buffer size

    if (Uargs->IF$Buf_Size LSS Connection_Info_ByteSize)
	{
	USER$Err(Uargs,NET$_BTS);// Buffer Too Small error.
	RETURN;
	};

// Fill in Connection Information return argument block.
// Clear Information buffer first.

    CH$FILL(%CHAR(0),Connection_Info_ByteSize,CS);

// Do Foreign Host Name (ASCIZ string).

    CH$MOVE(Frn_nlen,Frn_name,CH$PTR(CS->CI$Foreign_Host));
    CS->CI$FHost_Name_Size = Frn_nlen;

// Local host name

    CH$MOVE(Local_Name->DSC$W_LENGTH,Local_Name->DSC$A_POINTER,
	    CH$PTR(CS->CI$Local_Host));
    CS->CI$LHost_Name_Size = Local_Name->DSC$W_LENGTH;

// Local and foreign port numbers.

    CS->CI$Local_Port = (Lcl_Port && %X"FFFF") ;
    CS->CI$Foreign_Port = (Frn_Port && %X"FFFF") ;
  
  
// Local and remote internet addresses

    CS->CI$Local_internet_adrs = Lcl_Host;
    CS->CI$Remote_internet_adrs = Frn_Host;

// Copy block to user/system I/O buffer.

    $$KCALL(MOVBYT,Connection_Info_ByteSize,CS,Uargs->IF$Data_Start);
  
// Return the Connection Status to the user by posting the IO request.

    User$Post_IO_Status(Uargs,SS$_NORMAL,Connection_Info_ByteSize,0,0);
    MM$UArg_Free(Uargs);		// relese user arg block.
    };

//SBTTL "Derive an integer Clock base"

// get a portion of the 64-bit time to use as a clock based factor in
// time based calculations.

!Entry:	none

!Exit:	returns clock based integer.

USER$Clock_Base (void)
    {
    signed long
	Now: VECTOR->2;

    $GETTIM(TimAdr=Now);
    return (NOW->0^-20+NOW->1^12) && %x"7FFF";
    };

//SBTTL "Allocate a USER Local Port"
/*

Function:

	Allocate a user local port for a connection.  Port is clock-based.

Inputs:

	None.

Outputs:

	Valid user local port.

Side Effects:

	If new local port is > local-port space end then wrap it around
	& start at the beginning.
*/


signed long
    TCP_User_LP,
    UDP_User_LP;

USER$GET_LOCAL_PORT(Pbase)
    {
    signed long
	rval;

    Pbase = ..Pbase+1;
    rval = ..Pbase MOD User_LP_End;
    if (rval LSS User_LP_Start)
	rval = rval+User_LP_Start;
    return rval && %X"7FFF";
    };

FORWARD ROUTINE
 VOID    ACCESS_INIT;

USER$INIT_ROUTINES : NOVALUE (void)
    {
    TCP_User_LP = USER$Clock_Base();
    UDP_User_LP = USER$Clock_Base();
    ACCESS_INIT();
    };



//SBTTL "Net$Debug - Debug maintenance call."

/*
Function:

	Maintenance level user call.  Sets the IPACP system wide debug level.
	Used to control volume of trace infor placed into IPACP log file.
	IF Global "LOG_State" > 0 then the log file is open & logging enabled.
	Otherwise the LOG file is closed.

Inputs:

	Uargs = IPACP user argument block.

Outputs:

	None.

Side Effects:

	Debug level is reset.

*/

Net$Debug(struct Debug_Args * UArgs): NOVALUE (void)
    {
    SELECT (UARGS->DE$GROUP) OF
	SET
	[0] : LOG_CHANGE(UARGS->DE$LEVEL);
	[1] : ACT_CHANGE(UARGS->DE$LEVEL);
	TES;

    USER$Post_Function_OK(Uargs);
    };



//SBTTL "Net$Event - Activity logging maintenance call."

/*
Function:
	Maintenance level user call.  Used to append messages to the end
	of the INET$ACTIVITY log file.

Inputs:
	Uargs = IPACP user argument block.

Outputs:
	None.

Side Effects:
	Activity file is updated

*/

Net$Event(struct Event_Args * UArgs): NOVALUE (void)
    {
    EXTERNAL ROUTINE
	MM$Get_Mem, MM$Free_Mem;
    signed long
	RC,
	Buffer;

    if ((RC=MM$Get_Mem(Buffer,UArgs->EV$Buf_Size)) != SS$_NORMAL)
	{
	USER$ERR(Uargs,RC);
	RETURN
	};

    $$KCALL(MOVBYT, UArgs->EV$Buf_Size, UArgs->EV$Data_Start, Buffer);

    ACT$FAO("!%D (PID:!XW) [!AD]!/", 0,
	    (UArgs->EV$PID)<0,16,0>, UArgs->EV$Buf_Size, Buffer);

    USER$Post_Function_OK(Uargs);
    MM$Free_Mem(Buffer,UArgs->EV$Buf_Size);
    };



//SBTTL "Net$SNMP - Simple Network Management Protocol routine."

/*
Function:
	Maintenance level user call.  Used to manipulate the IPACP

Inputs:
	Uargs = IPACP user argument block.

Outputs:
	None.

Side Effects:
	System is, um, manipulated

*/

//Sbttl "NET$SNMP - Service an SNMP request"

/*

Function:

	Used to debug TCP by allowing a privileged user to examine TCP during
	execution.  The Dump directive indicates which/what type of a dump we
	will take.  (Please excuse the disgusting imagery).

Inputs:

	User argument blk formated according to Debug_Dump_args field definition.


Implicit Inputs:

	User must be privileged.

Output:

	User"s IO is posted back to the user.  If no errors then the user"s
	buffer is filled with requested dump data.
*/

NET$SNMP(struct SNMP_Args * Uargs): NOVALUE (void)
    {
    EXTERNAL ROUTINE
	SNMP$USER_INPUT,
	MM$Get_Mem, MM$Free_Mem;
    LITERAL
	RBBYTES = D$User_Return_Blk_Max_Size,
	RBSIZE = (RBBYTES+3)/4;		// Largest dump block, in alloc units
    signed long
	RC,
	struct VECTOR * In_Buff [,BYTE],
	Error = False,
	Now: VECTOR->2,			// time as in now.
	One: VECTOR->2 Initial(1,0),	// QuadWord of val 1.
	BufSize  = 0,
	RB: VECTOR->RBSIZE;

// Fetch the input data from kernal space.

    if ((RC=MM$Get_Mem(In_Buff,UArgs->SNMP$WBuf_Size)) != SS$_NORMAL)
	{
	USER$ERR(Uargs,RC);
	RETURN
	};

    $$KCALL(MOVBYT, UArgs->SNMP$WBuf_Size, UArgs->SNMP$Data_Start, In_Buff);

// Determine which Dump Directive we have.

/*
    SELECTONE Uargs->SNMP$Function OF
    SET

// Return the dynamic memory allocation counts, # of times the free list for the
// specified data structure was empty.  Used to figure out how many free list
// elements to pre-allocate.

    [SNMP$C_Get,
     SNMP$C_GetNext,
     SNMP$C_Store]:
	{
	Bufsize = UArgs->SNMP$RBuf_Size;
	Error = SNMP$USER_INPUT(In_Buff,UArgs->SNMP$WBuf_Size,
			       RB+4,Bufsize)
	};
    [SNMP$C_Kill]:
*/

if ((Uargs->SNMP$Function == 4))
	{
	EXTERNAL ROUTINE
	    TCP$KILL;

	XLOG$FAO(LOG$USER,"!%T Kill !XL (bsize=!XL)!/",0,
		 In_Buff->0,Uargs->SNMP$WBuf_Size);
	Bufsize = 10;

	RC = TCP$KILL(In_Buff->0); // nu? what are you waiting for? kill!!!
	if (RC != SS$_NORMAL) Error = USER$Err(Uargs,RC);

	CH$MOVE(BufSize,UPLIT(%ASCII "abcdefghij"),RB+4); // First long is size
	}
/*
    [Otherwise]:
	Error = USER$Err(Uargs,NET$_IFC); // Illegal Function code.
    TES;

*/
else
	Error = USER$Err(Uargs,NET$_IFC); // Illegal Function code.

// Did we have an Error or Illegal Dump directive code?

    MM$Free_Mem(In_Buff,UArgs->SNMP$WBuf_Size);

    if (NOT Error)
	{

// Check to see if user buffer is large enough to hold requested data.
// If not return error: Buffer TOO small.

	if (Uargs->SNMP$RBuf_Size LSS (Bufsize + 4))
	    USER$Err(Uargs,NET$_BTS)	// user's buffer is TOO small.
	else
	    {

// Copy local data into user's IO request buffer, Build movbyt arg list.

	    RB->0 = Bufsize;
	    $$KCALL(MOVBYT,Bufsize+4,RB,Uargs->SNMP$Data_Start);

// Post the user's IO request back to the user.

	    User$Post_IO_Status(Uargs,SS$_Normal,BufSize+4,0,0);
	    MM$UArg_Free(Uargs);	// Release user arg block.
	    End;
	};
    };



//Sbttl "Purge/Post All Remaining User IO Requests."
/*

Function:

	Post all remaining user IO requests back to the respective users.
	Routine is generally called before IPACP exits, idea is NOT to hang
	any user processes.  Virtual device "IP" is set to the offline state.

Inputs:

	None.

Outputs:

	None.

Side Effects:

	Take IP device offline and call protocol-specific routines to kill
	requests associated with all connections. Purge anything that remains
	on the user request queue. In any case, since the ACP is going to die
	soon don't bother to delete dynamic data structures, can end up calling
	the memory-mangler recursively. oops.

*/

FORWARD ROUTINE
 void    GTHST_Purge;

USER$Purge_All_IO : NOVALUE (void)
    {
    EXTERNAL ROUTINE
	TCP$Purge_All_IO : NOVALUE,
	UDP$Purge_All_IO : NOVALUE,
	ICMP$Purge_All_IO : NOVALUE,
	IPU$Purge_All_IO : NOVALUE;
    REGISTER
	qb;
    signed long
	struct User_Default_Args * Uargs,
	struct User_Send_Args * Sargs,
	IOSTATUS:  NetIO_Status_Block,
	EXPR: Vector->2;

// Set virtual device IP offline.  Prevent further user io.

    $$KCALL(Set_IP_Device_OffLine);

// Purge network I/O for all protocols

    TCP$Purge_All_IO();
    UDP$Purge_All_IO();
    ICMP$Purge_All_IO();
    IPU$Purge_All_IO();
    GTHST_Purge();

// check the user request queue again just to be safe.

    Expr->0 = 5*Timer_Delta;	// 5 seconds in Delta time format.
    Expr->1 = -1;
    $SCHDWK(Daytim=EXPR);
    $HIBER;			// Make sure ALL IO has been queued.

// Purge User request queue.
// Special case M$Cancel as the IRP came from the IP: driver cancel routine
// & not from a user process, be sure NOT to post the IO//  USER$Err will delete
// the uargs block.

    WHILE (Uargs=$$KCALL(User_Requests_Avail)) != false DO
	{

// post the user's io request with an error code: tcp is exiting.

	if (Uargs->UD$Funct neq M$Cancel)
	    User$Post_IO_Status(Uargs,NET$_TE,0,0,0);
!!!HACK!!// Don't release the Uarg?
	};
    };



//Sbttl "VMS$Cancel - Connection Cancel initiated by VMS."
/*

Function:

	Cancel(ABORT) connection associated with a given PID & IO channel #.
	Action is initiated by "IP" driver cancel IO routine.  Driver sends
	the ACP a "FAKE" IRP which communicates the process PID & channel #
	for which the IO is being cancelled.  See side effects below.

Inputs:

	VMS cancel request arg block (VMS$Cancel_args)
	Arg block contains requesting process PID & channel #.

Outputs:

	None.

Side Effects:

	All pending IO requests are returned to the user process with the
	VMS return code of SS$_ABORT & the TCP Error code ER$CCAN (Connection
	canceled).  Generally this routine is called in response to VMS image
	run-down routines doing a $CANCEL system service on an open I/O
	chanel.  Virtual device driver builds a TCP arg block with the
	"M$CANCEL" function code.
	***** Warning *****
	Do NOT post the IO on this I/O request as it did NOT come from a user
	process but instead came from the IP: driver cancel IO routine.
	Maclib.mar(user-requests-avail) routine takes care of the VMS dynamic
	memory management, all we have to worry about is the User-argument
	block.

*/

FORWARD ROUTINE
 VOID    GTHST_CANCEL;

User$Brk (void)
    {
    SS$_NORMAL
    };

void VMS$Cancel(struct VMS$Cancel_Args * Uargs) (void)
    {
    EXTERNAL ROUTINE
	TCP$Cancel,
	UDP$Cancel,
	ICMP$Cancel,
	IPU$Cancel;
    signed long
	ucbptr,
	proto,
	Done;

    ucbptr = Uargs->VC$UCB_Adrs + UCB$L_EXTRA;
    $$KCALL(MOVBYT,4,ucbptr,proto);

    XLOG$FAO(LOG$USER,"!%T VMS$Cancel: PID=!XL, Chan=!XL, UCB proto=!XL!/",
	     0,Uargs->VC$PID,Uargs->VC$PIOchan,Proto);

    Done = 0;
    SELECTONE Uargs->VC$Protocol OF
    SET
    [U$TCP_Protocol]:
	Done = TCP$Cancel(Uargs);

    [U$UDP_Protocol]:
	Done = UDP$Cancel(Uargs);

    [U$ICMP_Protocol]:
	Done = ICMP$Cancel(Uargs);

    [U$IP_Protocol]:
	Done = IPU$Cancel(Uargs);

    [OTHERWISE]:
	Done = USER$BRK();
    TES;

    GTHST_CANCEL(Uargs);
    MM$UArg_Free(Uargs);		// Release IPACP argument block
    };

//Sbttl "NET$Dump - Dump the TCB blocks to a user process"

/*

Function:

	Used to debug TCP by allowing a privileged user to examine TCP during
	execution.  The Dump directive indicates which/what type of a dump we
	will take.  (Please excuse the disgusting imagery).

Inputs:

	User argument blk formated according to Debug_Dump_args field definition.


Implicit Inputs:

	User must be privileged.

Output:

	User"s IO is posted back to the user.  If no errors then the user"s
	buffer is filled with requested dump data.
*/

NET$DUMP(struct Debug_Dump_Args * Uargs): NOVALUE (void)
    {
    EXTERNAL ROUTINE
	CALCULATE_UPTIME;
    EXTERNAL
	TEK$sys_uptime;
    REGISTER
	struct Queue_Blk_Structure * QB(QB_UR_Fields);	// queue block pointer.
    LITERAL
	RBBYTES = D$User_Return_Blk_Max_Size,
	RBSIZE = (RBBYTES+3)/4;		// Largest dump block, in alloc units
    signed long
	RC,
	Error = False,
	BufSize,
	count = 0,
	RB: VECTOR->RBSIZE;

// Determine which Dump Directive we have.

    SELECTONE Uargs->DU$Dump_Directive OF
    SET

// Return the dynamic memory allocation counts, # of times the free list for the
// specified data structure was empty.  Used to figure out how many free list
// elements to pre-allocate.

    [DU$Dynamic_Mem_Alloc]:
	{
	Map
	    RB: D$Mem_Alloc_Return_Blk;
	EXTERNAL
	    QBLK_Count_base : UNSIGNED BYTE,
	    Uarg_Count_base : UNSIGNED BYTE,
	    MIN_Seg_Count_base : UNSIGNED BYTE,
	    MAX_Seg_Count_base : UNSIGNED BYTE,
	    QBLK_Count : UNSIGNED BYTE,
	    Uarg_Count : UNSIGNED BYTE,
	    MIN_Seg_Count : UNSIGNED BYTE,
	    MAX_Seg_Count : UNSIGNED BYTE;

	rb->dm$qb = qb_gets;	// queue blocks
	rb->dm$ua = Ua_gets;	// User net io argument blks.
	rb->dm$cs = 0;		!~~~ OBSOLETE
	rb->dm$dms = MIN_Gets;	// Minimum (default) size packet buffers
	rb->dm$nm = MAX_Gets;	// Maximum size packet buffers
	rb->dm$qbmx = qb_max;	// queue blocks
	rb->dm$uamx= Ua_max;	// User net io argument blks.
	rb->dm$csmx = 0;	!~~~ OBSOLETE
	rb->dm$dmsmx = MIN_max;// Minimum (default) size buffers.
	rb->dm$nmmx = MAX_max;	// Maximum size buffer.
	rb->dm$qbal = QBLK_Count_Base;
	rb->dm$uaal = Uarg_Count_Base;
	rb->dm$csal = 0;
	rb->dm$dmsal = MIN_Seg_Count_Base;
	rb->dm$nmal = MAX_Seg_Count_base;
	rb->dm$qbfr = QBLK_Count;
	rb->dm$uafr = Uarg_Count;
	rb->dm$csfr = 0;
	rb->dm$dmsfr = MIN_Seg_Count;
	rb->dm$nmfr = MAX_Seg_Count;
	Bufsize	= D$MA_BLKsize;
	};

    [DU$TCP_stats]:
	{
	MAP
	    RB: D$TCP_Stats_Return_Blk;

	rb->dm$tcpacp_pid		= mypid;
	rb->dm$user_io_requests		= ts$uir;
!!!HACK!!//  // storeForward does not belong here...
	rb->dm$storeForward		= 0;
	rb->dm$active_conects_opened	= ts$aco;
	rb->dm$passive_conects_opened	= ts$pco;
	rb->dm$data_bytes_xmitted	= ts$dbx;
	rb->dm$data_bytes_recved	= ts$dbr;
	rb->dm$segs_xmitted		= ts$sx;
	rb->dm$segs_recved		= ts$sr;
	rb->dm$Seg_Bad_Chksum		= ts$seg_bad_cksum;
	rb->dm$badseq			= ts$badseq;
	rb->dm$duplicate_segs		= ts$duplicate_segs;
	rb->dm$retrans_segs		= ts$retrans_segs;
	rb->dm$rpz_rxq			= ts$rpz_rxq;
	rb->dm$oorw_segs		= ts$oorw_segs;
	rb->dm$future_rcvd		= ts$future_rcvd;
	rb->dm$future_used		= ts$future_used;
	rb->dm$future_dropped		= ts$future_dropped;
	rb->dm$future_dups		= ts$future_dups;
	rb->dm$Servers_Forked		= ts$servers_forked;

// Compute TCP uptime.
	CALCULATE_UPTIME();
	ch$move(8,TEK$sys_uptime,rb->dm$uptime);

	BufSize = D$TS_BlkSize;	// byte size of return blk.
	};

// Return all active local-connection-id's otherwise known as the address of the
// connection's TCB.
// 0th element of return vector is the count of valid tcb addresses in the
// return vector (ie, counted vector).

    [DU$Local_Connection_ID]:
	{
	EXTERNAL ROUTINE
void 	    TCP$Connection_List;
	TCP$Connection_List(RB);
	BufSize = D$lc_id_BlkSize;
	};

// Dump out a TCB

    [DU$TCB_Dump]:
	{
	EXTERNAL ROUTINE
	   TCP$TCB_Dump;
	if (TCP$TCB_Dump(Uargs->DU$ARG0,RB))
	    BufSize = D$TCB_Dump_BLKsize
	else
	    Error = USER$Err(Uargs,NET$_CDE)
	};

// Return all UDP connections (as D$Local_Connection_ID above)

    [DU$UDP_Connections]:
	{
	EXTERNAL ROUTINE
void 	    UDP$Connection_List;
	UDP$Connection_List(RB);
	Bufsize = D$UDP_List_Blksize;
	};

// Dump out a UDPCB

    [DU$UDPCB_Dump]:
	{
	EXTERNAL ROUTINE
	    UDP$UDPCB_Dump;
	if (UDP$UDPCB_Dump(Uargs->DU$Local_Conn_ID,RB))
	    Bufsize = D$UDPCB_Dump_Blksize
	else
	    Error = USER$Err(Uargs,NET$_CDE);
	};

// Return all ICMP connections (as D$Local_Connection_ID above)

    [DU$ICMP_Connections]:
	{
	EXTERNAL ROUTINE
void 	    ICMP$Connection_List;
	ICMP$Connection_List(RB);
	Bufsize = D$ICMP_List_Blksize;
	};

// Dump out a ICMPCB

    [DU$ICMPCB_Dump]:
	{
	EXTERNAL ROUTINE
	   ICMP$ICMPCB_Dump;
	if (ICMP$ICMPCB_DUMP (Uargs->DU$Local_Conn_ID,RB))
	    Bufsize = D$ICMPCB_Dump_Blksize
	else
	    Error = USER$Err(Uargs,NET$_CDE);
	};

// Get device-depandent dump from device driver module

    [du$device_dump]:
	{
	IF (uargs->du$device_idx geq 0) AND
	   (uargs->du$device_idx leq DC_max_num_net_Devices-1) AND
	   (Dev_Config_Tab[uargs->du$device_idx,dc_valid_Device]) THEN
	    {		// call device dump routine.
	    bufsize = Uargs->DU$Buf_Size;
	    if (bufsize > RBBYTES)
		bufsize = RBBYTES;
	    rc = (Dev_config_tab[uargs->du$device_idx, dc_rtn_Dump])
		    (uargs->du$device_idx, uargs->du$ARG1, uargs->du$ARG2,
		     rb, bufsize);
	    if (NOT rc)
		Error = USER$Err(uargs,NET$_epd); // error processing dump
	    }
	else
	    Error = USER$Err(uargs,NET$_bdi); // error: bad device index
	};

// Dump out ARP cache entries.

    [DU$ARP_CACHE]:
	{
	signed long
	    USIZE,
	    RMOD;
!	EXTERNAL ROUTINE
!	    ARP_DUMP;

	Error = USER$Err(Uargs,NET$_IFC); // Illegal Function code.

// Compute size of return block - make multiple of dump block size

!	USIZE = Uargs->DU$Buf_Size;
!	if (USIZE > RBBYTES)
!	    USIZE = RBBYTES;
!	RMOD = USIZE MOD D$ARP_Dump_Blksize;
!	USIZE = USIZE - RMOD;
!	if (USIZE <= 0)
!	    Error = USER$Err(Uargs,NET$_BTS)
!	else
!	    {
!	    Bufsize = ARP_DUMP(Uargs->DU$Start_Index,RB,USIZE);
!	    if (Bufsize LSS 0)
!		Error = USER$Err(Uargs,NET$_DAE);
!	    };
	};

// Get list of device indexes.

    [DU$Device_List]:
	{
	EXTERNAL ROUTINE
	    CNF$Device_list;
	Bufsize = CNF$Device_list(RB);
	};

    [DU$Device_Stat]:
	{
	EXTERNAL ROUTINE
	    CNF$Device_stat;
	if (Uargs->DU$Buf_Size LSS DC_Entry_Size*4)
	    Error = USER$Err(Uargs,NET$_BTS)
	else if (CNF$Device_stat ( Uargs->DU$ARG0, RB ) == -1)
	    Error = USER$Err(Uargs,NET$_DAE)
	else
	    Bufsize = D$Dev_dump_blksize
	};

// Undefined function code - give error

    [Otherwise]:
	Error = USER$Err(Uargs,NET$_IFC); // Illegal Function code.
    TES;

// Did we have an Error or Illegal Dump directive code?

    if (NOT Error)
	{

// Check to see if user buffer is large enough to hold requested data.
// If not return error: Buffer TOO small.

	if (Uargs->DU$Buf_Size LSS Bufsize)
	    USER$Err(Uargs,NET$_BTS)	// user's buffer is TOO small.
	else
	    {

// Copy local data into user's IO request buffer, Build movbyt arg list.

	    $$KCALL(MOVBYT,Bufsize,RB,Uargs->DU$Data_Start);

// Post the user's IO request back to the user.

	    User$Post_IO_Status(Uargs,SS$_Normal,BufSize,0,0);
	    MM$UArg_Free(Uargs);	// Release user arg block.
	    End;
	};
    };

//SBTTL "Net$Exit - Orderly shutdown of IP ACP"
/*
Function:

	Used as a privileged call to force an orderly shutdown of the ACP.

Inputs:

	User must have vms (phy_io) privilege.

Outputs:

	None

Side Effects:

	global "time_2_exit" set true, seen in tcp.bli mainline.
	IP: device and all clones are set offline.
*/

Net$EXIT(struct Debug_EXIT_Args * Uargs): NOVALUE (void)
    {

    XLOG$FAO(LOG$USER,"!%T EXIT requested, User PID: !XL!/",0,Uargs->EX$PID);

    USER$Post_Function_OK(Uargs);
    Time_2_Exit = True;		// Set global for exit, rtn: start_network.
    $$KCALL(Set_IP_device_OFFline); // mark network device(s) offline.
    };

//SBTTL "Network access check routines"

$FIELD GETJPI_FIELDS (void)
    SET
    BUFLEN	= [$Short_Integer],
    ITEM	= [$Short_Integer],
    BUFADR	= [$Address],
    RETLEN	= [$Address],
    LIST}	= [$Long_Integer]
    TES;
LITERAL
    GETJPI_SIZE = $FIELD_SET_SIZE;
MACRO
    GETJPI_BLOCK = BLOCK->GETJPI_SIZE FIELD(GETJPI_FIELDS) %;

User$Privileged(PID)
!
// Verify the user has privileges to use a well-known local port. User must
// have PHY_IO privilege.
// Returns TRUE if user has the privilege, FALSE otherwise.
!
    {
    signed long
	JPI : GETJPI_BLOCK,
	PRVBUF : $BBLOCK->8,
	PRVLEN;

// Fill in GETJPI request block.

    JPI->BUFLEN = 4;
    JPI->ITEM = JPI$_CURPRIV;
    JPI->BUFADR = PRVBUF;
    JPI->RETLEN = PRVLEN;
    JPI[LIST}] = 0;

// Request the priviliges for the process.

    if (($GETJPIW(PIDADR=PID,ITMLST=JPI)))
	if (PRVBUF->PRV$V_PHY_IO || PRVBUF->PRV$V_SETPRV)
	    return SS$_NORMAL;
    return NET$_NOPRV;
    };


Check_ID(PID,ID)
!
// Check that a user holds a given rights identifier. The identifiers of
// interest to us are ARPANET_ACCESS and ARPANET_WIZARD.
// Returns TRUE if the user has the necessary ID, FALSE otherwise.
!
    {
    signed long
	STATUS,
	JPI : GETJPI_BLOCK,
	UICBLK : VECTOR->2,
	UICLEN,
	RDBCTX,
	CURID;

// Fill in the GETJPI block

    JPI->BUFLEN = 4;
    JPI->ITEM = JPI$_UIC;
    JPI->BUFADR = UICBLK;
    JPI->RETLEN = UICLEN;
    JPI[LIST}] = 0;

// Retrieve the UIC for the process

    if (NOT $GETJPIW(PIDADR=PID,ITMLST=JPI))
	return FALSE;

// Check the rights database for this user

    UICBLK->1 = 0;
    RDBCTX = 0;
    CURID = ID;
    WHILE (STATUS = $FIND_HELD(	HOLDER	= UICBLK,
				ID	= CURID,
				CONTXT	= RDBCTX)) DO
        {
	if (CURID == ID)
	    {
	    $FINISH_RDB(CONTXT = RDBCTX);
	    return TRUE;
	    };
	};

// Didn't find it - punt.

    return FALSE;
    };


LITERAL WKS$SMTP = 25;		// Well known port number for SMTP

USER$CHECK_ACCESS(PID,LCLHST,LCLPRT,FRNHST,FRNPRT)
!
// Main routine to check for network access.
// Returns SS$_NORMAL if access is granted, or error code.
!
    {
    LABEL
	X;

// If no access checking is enabled, then skip this routine

    if (ACCESS_FLAGS == 0)
	return SS$_NORMAL;

// If we're checking acess for any network open, then check for INTERNET_ACCESS

    if (ACCESS_FLAGS<ACF$ALLOPENS>)
	if (NOT CHECK_ID(PID,INTERNET_ID))
	    return NET$_NOINA;

// If we're checking access to non-local hosts, then do so

    if (ACCESS_FLAGS<ACF$ARPAHOST>)
X:	{

// If the foreign host is in the "local hosts" list, then allow it.

	DECR I FROM (ACHOST_COUNT-1) TO 0 DO
	    if ((FRNHST && ACHOSTS[I,AC$MASK]) == ACHOSTS[I,AC$HOST])
		LEAVE X;
	if (NOT CHECK_ID(PID,ARPANET_ID))
	    return NET$_NOANA;
	};

// If the local port is privileged, then require special privilege

    if (ACCESS_FLAGS<ACF$PRIVPORT>)
	IF (((LCLPRT && %X"FFFF") GEQ Well_Known_LP_Start) AND
	    ((LCLPRT && %X"FFFF") <= Well_Known_LP_End) && FRNPRT == 0) OR
	   (FRNPRT == WKS$SMTP) THEN
	    if (NOT User$Privileged(PID))
		return NET$_NOPRV;

// Passed all of the tests - let them have access to the network

    return SS$_NORMAL;
    };


USER$ACCESS_CONFIG(HOSTNUM,HOSTMASK) : NOVALUE (void)
!
// Add an entry to the list of allowed local hosts. Called by CONFIG when
// LOCAL_HOST entry seen in the config file.
!
    {

// Make sure there is room for this entry

    if (ACHOST_COUNT GEQ ACCESS_MAX)
	{
	signed long
	    DESC$STR_ALLOC(HSTSTR,20);
	EXTERNAL ROUTINE
VOID 	    ASCII_DEC_BYTES;

	ASCII_DEC_BYTES(HSTSTR,4,HOSTNUM,HSTSTR->DSC$W_LENGTH);
	OPR$FAO("Local hosts list full - not adding entry for !AS",HSTSTR);
	};

// Insert it into the table

    ACHOSTS[ACHOST_COUNT,AC$HOST] = HOSTNUM;
    ACHOSTS[ACHOST_COUNT,AC$MASK] = HOSTMASK;
    ACHOST_COUNT = ACHOST_COUNT + 1;
    };


ACCESS_INIT : NOVALUE (void)
!
// Perform necessary initialzation for host access control. Translates the
// necessary identifiers via $ASCTOID and checks consistancy of switches and
// access list.
!
    {

// If access to network check enabled, translate INTERNET_ACCESS rights ID

    if (ACCESS_FLAGS<ACF$ALLOPENS>)
	{
	IF NOT $ASCTOID(NAME	= INTERNET_STRING,
			ID	= INTERNET_ID) THEN
	    {
	    OPR$FAO("% Failed to find identifier !AS - access check disabled",
		    INTERNET_STRING);
	    ACCESS_FLAGS<ACF$ALLOPENS> = 0;
	    };
	};

// If ARPANET access check enabled, translate ARPANET_ACCESS rights ID and
// verify that some hosts exist in the host list.

    if (ACCESS_FLAGS<ACF$ARPAHOST>)
	{
	IF NOT $ASCTOID(NAME	= ARPANET_STRING,
			ID	= ARPANET_ID) THEN
	    {
	    OPR$FAO("% Failed to find identifier !AS - access check disabled",
		    ARPANET_STRING);
	    ACCESS_FLAGS<ACF$ARPAHOST> = 0;
	    };
	if (ACHOST_COUNT == 0)
	    {
	    OPR$FAO("% No local hosts list - ARPANET access check disabled");
	    ACCESS_FLAGS<ACF$ARPAHOST> = 0;
	    };
	};
    };

//SBTTL "NET$GTHST: Get host information"
/*
Function:

    Retrieve information from the Namserver. 

Inputs:

    GTHST subfunction code and function-specific arguments.

Outputs:

    Function-specific data posted to user.

Side effects:

	Results of query may be cached in the Name Server.
*/

FORWARD ROUTINE
 VOID    GTHST_NMLOOK_DONE,
 VOID    GTHST_ADLOOK_DONE,
 VOID    GTHST_RRLOOK_DONE;

LITERAL
    NLBSIZE = GTHST_NMLOOK_RET_ARGS_LENGTH*4,
    ALBSIZE = GTHST_ADLOOK_RET_ARGS_LENGTH*4,
    RLBSize = 6;

void NET$GTHST(struct GTHST_Args * Uargs) (void)
    {

// Dispatch the GTHST subfunction

    CASE Uargs->GH$Subfunct FROM GTH_MIN TO GTH_MAX OF
    SET

// Local host info - same as name to address-list w/o doing name lookup.

    [GTH_LCLHST]:
	{
	signed long
	    RBLOCK : GTHST_NMLOOK_RET_ARGS,
	    Args: VECTOR->4;
	BIND
	    ADRVEC = RBLOCK->GHN$ADRLST : VECTOR;
	EXTERNAL
	    DEV_COUNT,
	    DEV_CONFIG_TAB : DEVICE_CONFIGURATION_TABLE;

// Verify size of return block.

	if (Uargs->GH$BUFSIZE LSS NLBSIZE)
	    {
	    USER$Err(Uargs,NET$_BTS);
	    RETURN;
	    };

// Get the list of addresses from the configuration table

	INCR IDX FROM 0 TO (DEV_COUNT-1) DO
	    ADRVEC[IDX] = DEV_CONFIG_TAB[IDX,DC_IP_ADDRESS];
	RBLOCK->GHN$ADRCNT = DEV_COUNT;

// Copy the name of the local host from the local info

	CH$MOVE(Local_Name->DSC$W_LENGTH,Local_Name->DSC$A_POINTER,
		CH$PTR(RBLOCK->GHN$NAMSTR));
	RBLOCK->GHN$NAMLEN = Local_Name->DSC$W_LENGTH;

// Return data to the user

	$$KCALL(MOVBYT,NLBSIZE,RBLOCK,Uargs->GH$Data_Start);

// And give them a good status reply

	User$Post_IO_Status(Uargs,SS$_Normal,NLBSIZE,0,0);
	MM$UArg_Free(Uargs);
	};

// Name to address-list lookup

    [GTH_NMLOOK]:
	{
	MAP
	    struct GTHST_NMLOOK_ARGS * Uargs;

	if (Uargs->GHN$BUFSIZE LSS NLBSIZE)
	    {
	    USER$Err(Uargs,NET$_BTS);
	    RETURN;
	    };

	NML$GETALST(CH$PTR(Uargs->GHN$HSTNAM),Uargs->GHN$HSTLEN,
		    GTHST_NMLOOK_DONE,Uargs);
	};

// Address to name lookup

    [GTH_ADLOOK]:
	{
	MAP
	    struct GTHST_ADLOOK_ARGS * Uargs;
	signed long
	    addr;

	if (Uargs->GHA$BUFSIZE LSS ALBSIZE)
	    {
	    USER$Err(Uargs,NET$_BTS);
	    RETURN;
	    };

// User argument is IP address - 0 means local address.

	addr = Uargs->GHA$IPADDR;
	if (addr == 0)
	    addr = Dev_Config_Tab[0,dc_IP_address];
	NML$GETNAME(addr,GTHST_ADLOOK_DONE,Uargs);
        };

// Domain resource record query.

    [GTH_RRECLK]:
	{
	MAP
	    struct GTHST_RRLOOK_ARGS * Uargs;

	if (Uargs->GRR$BUFSIZE LSS RLBSize)
	    {
	    USER$Err(Uargs,NET$_BTS);
	    RETURN;
	    };

	NML$GETRR(Uargs->GRR$RRTYPE,
		    CH$PTR(Uargs->GRR$HSTNAM),Uargs->GRR$HSTLEN,
		    GTHST_RRLOOK_DONE,Uargs);
	};

// Unknown GTHST function

    [OUTRANGE]:
	USER$Err(Uargs,NET$_IGF);
    TES;
    };

//SBTTL "Name lookup done handler"
/*
    Come here when name GTH_NMLOOK function has completed.
    Called at AST level when NM_GETALST completes for the GTHST function.
    Post the user request with the completion code.
*/

GTHST_NMLOOK_DONE(Uargs,Status,Adrcnt,Adrlst,Namlen,Nambuf) : NOVALUE (void)
    {
    signed long
	Args: VECTOR->4;
    MAP
	struct GTHST_NMLOOK_ARGS * Uargs;
    signed long
	NLB : GTHST_NMLOOK_RET_ARGS;

// If an error occurred, give it to the user

    if (NOT Status)
	{
	USER$Err(Uargs,Status);
	RETURN;
	};
    
// Copy the return data into prototype block

    NLB->GHN$ADRCNT = Adrcnt;
    CH$MOVE(Adrcnt*4,Adrlst,NLB->GHN$ADRLST);
    NLB->GHN$NAMLEN = Namlen;
    CH$MOVE(Namlen,Nambuf,NLB->GHN$NAMSTR);

// Copy the block back to the user.

    $$KCALL(MOVBYT,NLBSIZE,NLB,Uargs->GHN$Data_Start);

// And give them a good status reply

    User$Post_IO_Status(Uargs,SS$_Normal,NLBSIZE,0,0);
    MM$UArg_Free(Uargs);
    };

//SBTTL "Address lookup done handler"
/*
    Same as above, but for GTH_ADLOOK function.
*/

GTHST_ADLOOK_DONE(Uargs,Status,Namlen,Nambuf) : NOVALUE (void)
    {
    signed long
	Args: VECTOR->4;
    MAP
	struct GTHST_ADLOOK_ARGS * Uargs;
    signed long
	ALB : GTHST_ADLOOK_RET_ARGS;

// If an error occurred, give it to the user

    if (NOT Status)
	{
	USER$Err(Uargs,Status);
	RETURN;
	};

// Build the return block

    ALB->GHA$NAMLEN = namlen;
    CH$MOVE(Namlen,Nambuf,ALB->GHA$NAMSTR);

// Return data to the user

    $$KCALL(MOVBYT,ALBSIZE,ALB,Uargs->GHA$Data_Start);

// And give them a good status reply

    User$Post_IO_Status(Uargs,SS$_Normal,ALBSIZE,0,0);
    MM$UArg_Free(Uargs);
    };

//SBTTL "RR lookup done handler"
/*
    Come here when name GTH_RRLOOK function has completed.
    Called at AST level when NM_GETRR completes for the GTHST function.
    Post the user request with the completion code.
*/

GTHST_RRLOOK_DONE(Uargs,Status,RDLen,RData,Namlen,Nambuf) : NOVALUE (void)
    {
    signed long
	Args: VECTOR->4;
    MAP
	struct GTHST_RRLOOK_ARGS * Uargs;
    signed long
	RLB : GTHST_RRLOOK_RET_ARGS;

// If an error occurred, give it to the user

    if (NOT Status)
	{
	USER$Err(Uargs,Status);
	RETURN;
	};
    
// Copy the return data into prototype block

    if (Uargs->GRR$BUFSIZE LSS RLBSize + RDLen)
	{
	USER$Err(Uargs,NET$_BTS);
	RETURN;
	};

    RLB->GRR$RDLEN = RDLen;
//    RLB->GRR$NAMLEN = Namlen;
//    CH$MOVE(RDLen,RData,RLB->GRR$DATA);
//    CH$MOVE(Namlen,Nambuf,RLB->GRR$DATA+.RDLen);

// Copy the block back to the user.

    $$KCALL(MOVBYT,2,RLB,Uargs->GRR$Data_Start);
    $$KCALL(MOVBYT,RDLen,Rdata,Uargs->GRR$Data_Start + 2);
//    $$KCALL(MOVBYT,Namlen,Nambuf,Uargs->GRR$Data_Start + 6 + RDLen);

// And give them a good status reply

    User$Post_IO_Status(Uargs,SS$_Normal, RLBSize + RDLen,0,0);
    MM$UArg_Free(Uargs);
    };

//SBTTL "GTHST_CANCEL - Cancel GTHST requests for a process"

FORWARD ROUTINE
 VOID    GTHST_CANCEL_ONE;

Void GTHST_CANCEL(struct VMS$CANCEL_ARGS * Uargs) (void)
!
// Search the list of pending GTHST requests looking for match. If found,
// post it now and delete from the queue.
!
    {
    NML$STEP(GTHST_CANCEL_ONE,Uargs);
    };

GTHST_CANCEL_ONE(VCUARGS,ASTADR,UARGS) : NOVALUE (void)
!
// Check a single entry from the name lookup queue to see if it belongs to
// the process that is doing the cancel. If so, we will cancel it.
!
    {
    MAP
	struct VMS$CANCEL_ARGS * VCUARGS,
	struct User_Default_Args * UARGS;

// Do sanity check on AST routine. Only GTHST done routines should be attached
// to GTHST requests.

    IF (ASTADR != GTHST_NMLOOK_DONE) AND
	(ASTADR != GTHST_ADLOOK_DONE) AND
	(ASTADR != GTHST_RRLOOK_DONE) THEN
	RETURN;

// See if the UCB of the cancellor is the same as that of the request. If so,
// cancel the request (i.e. "finish" it with an error).


    if (VCUARGS->VC$UCB_ADRS == UARGS->UD$UCB_ADRS)
	NML$CANCEL(UARGS,TRUE,NET$_CCAN);
    };

FORWARD ROUTINE
 VOID    GTHST_PURGE_ONE;

GTHST_PURGE : NOVALUE (void)
!
// Routine to purge all requests when network ACP exits. Step through all
// requests (with NML$STEP) and punt them.
!
    {
    NML$STEP(GTHST_PURGE_ONE,0);
    };

GTHST_PURGE_ONE(COVALUE,ASTADR,UARGS) : NOVALUE (void)
!
// Coroutine for NML$STEP/GTHST_PURGE
!
    {

// Do sanity check on AST routine. Only GTHST done routines should be attached
// to GTHST requests.

    IF (ASTADR != GTHST_NMLOOK_DONE) AND
	(ASTADR != GTHST_ADLOOK_DONE) AND
	(ASTADR != GTHST_RRLOOK_DONE) THEN
	RETURN;
    NML$CANCEL(UARGS,TRUE,NET$_TE);
    };

//SBTTL "Process User Requests."
/*

Function:

	Process user IP function requests.  User requests are read from the
	ACP input queue.  All request processing is handled by the appro rtn.
	Requests maybe queued for later processing as in the case of a receive
	requests & no data-bearing segments have arrived for that connection.
	If user is waiting for a local event flag to be set, then he is still
	waiting until the IO request has been posted via IO$POST rtn.
	VMS access mode MUST be KERNAL to touch VMS IO data structures.

Inputs:

	None.

Implicit Inputs:

	VMS pseudo-device has been mounted so an ACP queue block exists.
	See module: MACLIB.MAR for device mounts.

Outputs:

	None.

Side Effects:

	User requests are processed & returned to the user.

IMPORTANT NOTE// :

	When adding new functions to the IPACP, remember to change
	and recompile the function codes in *all* modules.  Don't
	forget about changing m$cancel in maclib.mar//  It's defined
	both there and in user.bli.

*/


USER$Process_User_Requests : NOVALUE (void)
    {
    EXTERNAL ROUTINE

// TCP functions:
	TCP$OPEN : NOVALUE,
	TCP$CLOSE : NOVALUE,
	TCP$ABORT : NOVALUE,
	TCP$S} : NOVALUE,
	TCP$RECEIVE : NOVALUE,
	TCP$INFO : NOVALUE,
	TCP$STATUS : NOVALUE,

// UDP functions:
	UDP$OPEN : NOVALUE,
	UDP$CLOSE : NOVALUE,
	UDP$ABORT : NOVALUE,
	UDP$S} : NOVALUE,
	UDP$RECEIVE : NOVALUE,
	UDP$INFO : NOVALUE,
	UDP$STATUS : NOVALUE,

// ICMP functions:
	ICMP$OPEN : NOVALUE,
	ICMP$CLOSE : NOVALUE,
	ICMP$ABORT : NOVALUE,
	ICMP$S} : NOVALUE,
	ICMP$RECEIVE : NOVALUE,
	ICMP$INFO : NOVALUE,
	ICMP$STATUS : NOVALUE,

// IP functions:
	IPU$OPEN : NOVALUE,
	IPU$CLOSE : NOVALUE,
	IPU$ABORT : NOVALUE,
	IPU$S} : NOVALUE,
	IPU$RECEIVE : NOVALUE,
	IPU$INFO : NOVALUE,
	IPU$STATUS : NOVALUE;
    REGISTER
	struct User_Default_Args * ArgBlk;

    WHILE (ARGBLK=$$KCALL(User_requests_Avail)) != False DO
	{
	if ($$LOGF(LOG$USER))
	    {
	    signed long
		DESC$STR_ALLOC(funcstr,30),
		func;
	    SELECTONE ArgBlk->UD$Funct OF
	    SET
	    [U$OPEN]:	func=%ASCID"TEK$OPEN";
	    [U$S}]: 	func=%ASCID"TEK$S}";
	    [U$RECV]:	func=%ASCID"TEK$RECEIVE";
	    [U$CLOSE]:	func=%ASCID"TEK$CLOSE";
	    [U$ABORT]:	func=%ASCID"TEK$ABORT";
	    [U$STATUS]:	func=%ASCID"TEK$STATUS";
	    [U$INFO]:	func=%ASCID"TEK$INFO";

	    [U$GTHST]:  func=%ASCID"Net$GTHST";
	    [M$DUMP]:	func=%ASCID"Net$DUMP";
	    [M$EXIT]:	func=%ASCID"Net$EXIT";
	    [M$DEBUG]:	func=%ASCID"Net$Debug";
	    [M$EVENT]:	func=%ASCID"Net$Event";
	    [M$SNMP]:	func=%ASCID"Net$SNMP";

	    [M$Cancel]:	func=%ASCID"VMS$Cancel";
	    [OTHERWISE]:
		    {
		    $FAO(%ASCID"?User FCN !SL",funcstr->DSC$W_LENGTH,funcstr,
			 argblk->UD$Funct);
		    func = funcstr;
		    };
	    TES;
	    LOG$FAO("!%T !AS (Proto:!XB), Uargs=!XL,VMSID=!XL,IRP=!XL,UCB=!XL!/",		    0, func, Argblk->UD$Protocol,
		    Argblk, Argblk->UD$VMS_BLK_ID,
		    Argblk->UD$IRP_Adrs, Argblk->UD$UCB_Adrs,
		    Argblk->UD$PID);
	    };

	if (Argblk->ud$funct != M$Cancel)
	    ts$uir = ts$uir + 1 // count arrival of user io request.
	else VMS$Cancel(ArgBlk);

// dispatch to appro user io request handler routine.  Cover both tcp
// and maintenance functions.

	if (Argblk->ud$protocol == U$TCP_Protocol)
	   // Handle TCP functions first to improve speed.
	   CASE argBlk->ud$funct FROM u$open TO u$max_TCP_function OF
	   SET
	   [u$open]:	TCP$OPEN(ArgBlk);
	   [u$send]: 	TCP$S}(ArgBlk);
	   [u$recv]:	TCP$Receive(ArgBlk);
	   [u$close]:	TCP$CLOSE(ArgBlk);
	   [u$abort]:	TCP$ABORT(ArgBlk);
	   [u$status]:	TCP$Status(ArgBlk);
	   [u$info]:	TCP$Info(ArgBlk);
	   [OUTRANGE]:	USER$Err(ArgBlk,NET$_IFC); // Illegal Function Code.

	   TES
	else
	CASE argBlk->ud$funct FROM u$open TO u$max_user_function OF
	SET

	[u$open]:
	    SELECTONE Argblk->ud$protocol OF
	    SET
	    [U$UDP_Protocol] :	UDP$OPEN(ArgBlk);
	    [U$ICMP_Protocol] :	ICMP$OPEN(ArgBlk);
	    [U$IP_Protocol] :	IPU$OPEN(ArgBlk);
	    [OTHERWISE]:USER$Err(ArgBlk,NET$_IPC); // Illegal Protocol Code.
	    TES;

	[u$send]:
	    SELECTONE Argblk->ud$protocol OF
	    SET
	    [U$UDP_Protocol] :	UDP$S}(ArgBlk);
	    [U$ICMP_Protocol] :	ICMP$S}(ArgBlk);
	    [U$IP_Protocol] :	IPU$S}(ArgBlk);
	    [OTHERWISE]:USER$Err(ArgBlk,NET$_IPC); // Illegal Protocol Code.
	    TES;

	[u$recv]:
	    SELECTONE Argblk->ud$protocol OF
	    SET
	    [U$UDP_Protocol] :	UDP$Receive(ArgBlk);
	    [U$ICMP_Protocol] :	ICMP$Receive(ArgBlk);
	    [U$IP_Protocol] :	IPU$Receive(ArgBlk);
	    [OTHERWISE]:USER$Err(ArgBlk,NET$_IPC); // Illegal Protocol Code.
	    TES;

	[u$close]:
	    SELECTONE Argblk->ud$protocol OF
	    SET
	    [U$UDP_Protocol] :	UDP$CLOSE(ArgBlk);
	    [U$ICMP_Protocol] :	ICMP$CLOSE(ArgBlk);
	    [U$IP_Protocol] :	IPU$CLOSE(ArgBlk);
	    [OTHERWISE]:USER$Err(ArgBlk,NET$_IPC); // Illegal Protocol Code.
	    TES;

	[u$abort]:
	    SELECTONE Argblk->ud$protocol OF
	    SET
	    [U$UDP_Protocol] :	UDP$ABORT(ArgBlk);
	    [U$ICMP_Protocol] :	ICMP$ABORT(ArgBlk);
	    [U$IP_Protocol] :	IPU$ABORT(ArgBlk);
	    [OTHERWISE]:USER$Err(ArgBlk,NET$_IPC); // Illegal Protocol Code.
	    TES;

	[u$status]:
	    SELECTONE Argblk->ud$protocol OF
	    SET
	    [U$UDP_Protocol] :	UDP$Status(ArgBlk);
	    [U$ICMP_Protocol] :	ICMP$Status(ArgBlk);
	    [U$IP_Protocol] :	IPU$Status(ArgBlk);
	    [OTHERWISE]:USER$Err(ArgBlk,NET$_IPC); // Illegal Protocol Code.
	    TES;

	[u$info]:
	    SELECTONE Argblk->ud$protocol OF
	    SET
	    [U$UDP_Protocol] :	UDP$Info(ArgBlk);
	    [U$ICMP_Protocol] :	ICMP$Info(ArgBlk);
	    [U$IP_Protocol] :	IPU$Info(ArgBlk);
	    [OTHERWISE]:USER$Err(ArgBlk,NET$_IPC); // Illegal Protocol Code.
	    TES;

	[u$gthst]:	Net$GTHST(argblk);

	[OUTRANGE]:
	    {
	    SELECTONE argBlk->ud$funct OF // check acp maintenance functions
	    SET
	    [M$DUMP]:	Net$DUMP(ArgBlk);
	    [M$EXIT]:	Net$EXIT(ArgBlk);
	    [M$DEBUG]:	Net$Debug(ArgBlk);
	    [M$EVENT]:	Net$Event(ArgBlk);
	    [M$SNMP]:	Net$SNMP(ArgBlk);
!	    [M$Cancel]:	VMS$Cancel(ArgBlk);
	    [M$Cancel]:	SS$_NORMAL;
	    [OTHERWISE]:USER$Err(ArgBlk,NET$_IFC); // Illegal Function Code.
	    TES;
	    };
	TES;
	};
    };
}
ELUDOM
