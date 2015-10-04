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
    This version by Vince Fuller, CMU-CSD, Spring/Summer, 1986
    Copyright (c) 1986,1987, Vince Fuller and Carnegie-Mellon University

Modification History:

6.7a    09-Jul-1991 Henry W. Miller     USBR
    Added STARLET for VMS 5.4.

*** Begin CMU change log ***

6.7  16-Jan-1991, Bruce R. Miller   CMU Network Development
    Added the idea of logging flag "groups" in order to handle
    new clusters of logging flags.  We were running out of
    flags, plus we need to handle ACTIVITY logging.

6.7  06-Feb-1990, Bruce R. Miller   CMU Network Development
    Changed interface to transport device-specific structures.

6.7  27-Nov-1989, Bruce R. Miller   CMU Network Development
    Added equal support for TCP, UDP, ICMP, and IP.
    This module now looks at the ACP Arg Blk to determain
    a request's protocol.  Removed all u$udp$xxxx functions
    calls.  Now all function requests are sent with a protocol
    field in the arg blk and use the u$xxxx function code.

6.7  20-Oct-1989, Bruce R. Miller   CMU Network Development
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
    TCP$KILL_PENDING_REQUESTS.
    Add new counters, make available to dump function.

4.1  25-Jun-86, Edit by VAF
    Make the UCB hold a TCB "index" (index into VALID_TCB table) and don't
    ever let the user look at real TCB addresses.
    Change and simplify "local conn id" validation in all user routines.
    Don't use index 0 of VALID_TCB table.

4.0  23-Jun-86, Edit by VAF
    Start adding support for UCB extension to hold TCB pointer.

3.9  12-Jun-86, Edit by VAF
    Do buffering of user sends here not in SEND_DATA routine. It may cost
    a little extra buffer copying, but it will probably speed things up.

3.8  11-Jun-86, Edit by VAF
    In USER$Purge_All_IO call TCP$KILL_PENDING_REQUESTS - don't duplicate
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
    In NET$SEND, don't call SEND_DATA - it will be done soon enough.

3.2  22-Apr-86, Edit by VAF
    Phase II of flushing XPORT - use $FAO for doing output formatting.

3.1  21-Apr-86, Edit by VAF
    Make all connections wait for open by default.
    Add a new bit to the open call - OP$NoWait - for immediate return

3.0  19-Apr-86, Edit by VAF
    Flush call to SEND_DATA in main user processing routine.
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

#if 0
MODULE USER(IDENT="6.7a",LANGUAGE(BLISS32),
            ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
                            NONEXTERNAL=LONG_RELATIVE),
            LIST(NOREQUIRE,ASSEMBLY,OBJECT,BINARY),
            OPTIMIZE,OPTLEVEL=3,ZIP)=
#endif

// not yet #include <cmuip/central/include/netxport.h"  // BLISS transportablity package
//LIBRARY <starlet.h>   // VMS system definitions ** Not STARLET **
#include <starlet.h>
// not yet #include "SYS$LIBRARY:LIB";  // VMS system definitions ** Not STARLET **
#include <cmuip/central/include/neterror.h> // Network error messages
#include <cmuip/central/include/netcommon.h>    // Various VMS specifics
#include "netvms.h"     // Various VMS specifics
#include "structure.h"      // TCB & Segment Structure definitions
//LIBRARY "tcp.h"           // TCP related definitions
#include "cmuip.h" // needed before tcpmacros.h
#include "tcpmacros.h"      // Include local macros
#include <cmuip/central/include/netconfig.h> // Transport devices interface

#include <ssdef.h>
#include <descrip.h>
#include <jpidef.h>
#include <prvdef.h>

#ifndef NOKERNEL
#define sys$faol exe$faol
#define sys$fao exe$fao
#define sys$getjpiw exe$getjpiw
#define sys$asctoid exe$asctoid
#define sys$find_held exe$find_held
//#define sys$finish_rdb exe$finish_rdb
#endif

//*** N.B. Special UCB extensions used by IP device driver  ***
//*** Take care to always match definitions in IPDRIVER.MAR ***
//*** Referenced by: TCP_USER.BLI, UDP.BLI, ICMP.BLI        ***


#define    UCB$Q_DDP    ucb$q_devdepend // check
#define    UCB$L_CBID   ucb$l_devdepnd2 // Control Block associated with UCB
#define    UCB$L_EXTRA  ucb$l_devdepnd3 // Extra longword for later expansion

                extern signed long
                log_state,
                Time_2_Exit,
                mypid;          // maclib.mar
extern struct dsc$descriptor  Local_Name;
extern Device_Configuration_Entry dev_config_tab[];

// Memory manager dynamic allocation counts (defined in memgr.bli)

extern signed long
qb_gets,
ua_gets,
min_gets,
max_gets,
qb_max,
ua_max,
min_max,
max_max,

// TCP statistics counters (see tcp.bli global defs).

ts$uir,
ts$aco,
ts$pco,
ts$dbx,
ts$dbr,
ts$sr,
ts$sx,
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

// Rtns from MEMGR.BLI

extern     mm$uarg_free();

// IOUTIL.BLI

extern  void    ACT_OUTPUT();
extern     LOG_CHANGE();
extern     ACT_CHANGE();
extern  void    LOG_OUTPUT();
extern  void    LOG_TIME_STAMP();
extern  void    ACT_FAO();
extern  void    LOG_FAO();
#if 0
extern  void    OPR_FAO();
#endif

// Routines From:  MACLIB.MAR

extern     void set_ip_device_offline();
extern     user_requests_avail();
extern      VMS_IO$POST();
extern     void MOVBYT();
extern     void SwapBytes();

// NMLOOK.BLI

extern  void    NML$GETNAME();
extern  void    NML$GETALST();
extern  void    NML$GETRR();
extern  void    NML$CANCEL();
extern  void    NML$STEP();

// IP.BLI

extern     ip_islocal();


// User Network(TCP) I/O Request arg blk Function Codes.
// *** warning ***
// user function MUST be a dense list as they are used to generate a case table.

// Important//  If you change these definitions, you must also change
// also change the corresponding definitions in the IPDRIVER as well as
// m$cancel in maclib.mar.

#define     M$UNUSED      0

#define     U$OPEN    1
#define     U$SEND    2
#define     U$RECV    3
#define     U$CLOSE   4
#define     U$STATUS      5
#define     U$ABORT   6
#define     U$INFO    7

#define     U$MAX_TCP_FUNCTION   7  // for case limit

// GTHST (Get host info) function

#define       U$GTHST     8
#define     GTH_LCLHST   0
#define     GTH_NMLOOK   1
#define     GTH_ADLOOK   2
#define     GTH_RRECLK   3
#define     GTH_MIN   GTH_LCLHST
#define     GTH_MAX   GTH_RRECLK

#define     U$MAX_USER_FUNCTION   8 // for case limit

// Privileged ACP Maintenance Functions.

#define     M$DUMP    9
#define     M$EXIT    10
// extra, obsolete function.
#define     M$DEBUG   11
#define     M$EVENT   12
#define     M$SNMP    13

#define     M$CANCEL      14

// Special, internal routine function (for TVT processing)

#define     M$INTERNAL    15



//SBTTL "Data for checking network access"

// Define block for storing allowed hosts.

struct ACCESS_LIST
{
    int   AC$HOST;
    int    AC$MASK;
};
#define    ACCESS_MAX 20

#if 0
LITERAL
ACCESS_SIZE = $FIELD_SET_SIZE,
MACRO
ACCESS_LIST = BLOCKVECTOR[ACCESS_MAX,ACCESS_SIZE] FIELD(ACCESS_FIELDS) %;

#define    ACF$PRIVPORT = 0,1,0 %,  // Check for privileged ports
#define    ACF$ARPAHOST = 1,1,0 %,  // Check ARPANET_ACCESS for nonlocal hosts
#define    ACF$ALLOPENS = 2,1,0 %;  // Check INTERNET_ACCESS for any open
#endif

#define     ACF_PRIVPORT   1        // Bitmask for above...
#define     ACF_ARPAHOST   2        // "
#define     ACF_ALLOPENS   4        // "

signed long
access_flags  = ACF_PRIVPORT; // Flags for access checks to do

static signed long
ACHOST_COUNT  = 0,  // Count of hosts
ARPANET_ID  = 0,    // Identifier for ARPANET_ACCESS
INTERNET_ID  = 0;   // Identifier for INTERNET_ACCESS
struct ACCESS_LIST * ACHOSTS ;  // List of local hosts

#define    ARPANET_STRING ASCID2(14, "ARPANET_ACCESS")
#define    INTERNET_STRING ASCID2(15, "INTERNET_ACCESS")

//Sbttl "Request VMS to Post users IO request."
/*

Function:

    Request VMS to post the user's IO function thus completing the
    VMS IO request.  We need to change mode to Kernel to access some
    of the VMS IO data strctures.

Inputs:

    IOSB    IO Status Block Address.
    IRP IRP address
    UCB UCB address

Outputs:

    None.

Side Effects:

    Process access mode is changed to KERNEL & back to user before
    this routine exits.

*/


void IO$POST (long long * IOSB, struct user_default_args * UArg)
{
    signed long
    IRP;

    XLOG$FAO(LOG$USER,"!%T IO$POST : Uarg=!XL IRP=!XL UCB=!XL!/",0,
             UArg, UArg->ud$irp_adrs , UArg->ud$ucb_adrs );

    IRP = UArg->ud$irp_adrs;
    UArg->ud$irp_adrs = 0;

    if (IRP == 0)
    {
        OPR$FAO("!%T No IRP, UCB=!XL Proto=!XB funct=!XB !/",0,
                UArg->ud$ucb_adrs,UArg->ud$protocol,UArg->ud$funct);
        return FALSE;
    };

    $$KCALL(VMS_IO$POST,IOSB,IRP,UArg->ud$ucb_adrs);
}



//SBTTL "POST User IO Status"

/*

Function:

    Return I/O Status to the user processes.  Used for TCP functions
    which actually transfer data between user & TCP (SEND, Receieive
    status, dump).

Inputs:

    IRP = Address of User's IO Request Packet (VMS structure).
    UCB = Address of Unit Control Blk (VMS Structure).
    VMS_Return_Code = SS$_NORMAL etc.
    TCP_Err, When VMS_Return_Code != SS$_NORMAL this is the error feild
    Bytes_Xfered = # of bytes read/written
    IO_Tag = IO request identifier (receive only)
    URG = Urgent data present (send only)
    EOL = End Of Letter (send only).

Outputs:

    None.

Side Effects:

    User's IRP is queued to VMS IO post-processing rtns.

*/


void user$post_io_status (UARG,STATUS,NBYTES,
                          FLAGS,ICMCODE)
{
    netio_status_block IOSB_,* IOSB = & IOSB_;

// Fill in Network IO status Block

    if (STATUS != SS$_NORMAL)
    {
        IOSB->nsb$status = SS$_ABORT;
        IOSB->net_status.nsb$xerror = STATUS;
    }
    else
    {
        IOSB->nsb$status = SS$_NORMAL;
        IOSB->nsb$byte_count = NBYTES;
        IOSB->net_status.flags.nsb$flags = FLAGS;
        IOSB->net_status.nsb$icmp_code = ICMCODE;
    };

// Queue IRP to IO post-processor

    IO$POST(IOSB, UARG);

}


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

USER$Err (struct user_default_args * Arg, long Err)
{
    netio_status_block IOSB_ , * IOSB = & IOSB_;

// Fill in Network IO status Block

    IOSB->nsb$status = SS$_ABORT;
    IOSB->nsb$byte_count = 0;
    IOSB->net_status.nsb$xerror = Err;

// Make sure we have an IRP!

    if (Arg->ud$irp_adrs == 0)
    {
        if ($$LOGF(LOG$USER))
            LOG$FAO("!%T No IRP, UArg = !XL UCB=!XL Proto=!XB funct=!XB !/",0,
                    Arg,Arg->ud$ucb_adrs,Arg->ud$protocol,Arg->ud$funct);
        return FALSE;
    };
// Queue IRP to VMS I/O post-processor

    IO$POST(IOSB, Arg);
    mm$uarg_free(Arg);          // Release user TCP arg block.

// If logging is enabled then output the user error message to the log file.

    if ($$LOGF(LOG$USER))
        LOG$FAO("!%T User error return, RC = !XL!/",0,Err);

    return TRUE;
}


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

void user$post_function_ok(struct user_default_args * Arg)
{
    signed long
    IRP;
    netio_status_block IOSB_, * IOSB = &IOSB_;

// Fill in Network IO status Block

    IOSB->nsb$status = SS$_NORMAL;
    IOSB->nsb$byte_count = 0;
    IOSB->net_status.nsb$xstatus = 0;

// Queue IRP to IO post-processor

    IO$POST(IOSB,Arg);
    mm$uarg_free(Arg);      // Release user arg block.
}

//SBTTL "Give info about a connection"

// Common routine used by TCP and UDP to return connection info
// (local/foreign host numbers,names and ports)

void user$net_connection_info(struct user_info_args * uargs,
                              long Lcl_Host,long Frn_Host,long Lcl_Port,long Frn_Port,
                              long Frn_Name,long Frn_Nlen)
{
    connection_info_return_block CS_,* CS= &CS_;

// Verify buffer size

    if (uargs->if$buf_size < CONNECTION_INFO_BYTESIZE)
        {
            USER$Err(uargs,NET$_BTS);// Buffer Too Small error.
            return;
        };

// Fill in Connection Information return argument block.
// Clear Information buffer first.

    CH$FILL(/*%CHAR*/(0),CONNECTION_INFO_BYTESIZE,CS);

// Do Foreign Host Name (ASCIZ string).

    CH$MOVE(Frn_Nlen,Frn_Name,CH$PTR(CS->ci$foreign_host,0));
    CS->ci$fhost_name_size = Frn_Nlen;

// Local host name

    CH$MOVE(Local_Name.dsc$w_length,Local_Name.dsc$a_pointer,
            CH$PTR(CS->ci$local_host,0));
    CS->ci$lhost_name_size = Local_Name.dsc$w_length;

// Local and foreign port numbers.

    CS->ci$local_port = (Lcl_Port & 0xFFFF) ;
    CS->ci$foreign_port = (Frn_Port & 0xFFFF) ;


// Local and remote internet addresses

    CS->ci$local_internet_adrs = Lcl_Host;
    CS->ci$remote_internet_adrs = Frn_Host;

// Copy block to user/system I/O buffer.

    $$KCALL(MOVBYT,CONNECTION_INFO_BYTESIZE,CS,uargs->if$data_start);

// Return the Connection Status to the user by posting the IO request.

    user$post_io_status(uargs,SS$_NORMAL,CONNECTION_INFO_BYTESIZE,0,0);
    mm$uarg_free(uargs);        // relese user arg block.
}

//SBTTL "Derive an integer Clock base"

// get a portion of the 64-bit time to use as a clock based factor in
// time based calculations.

//Entry:    none

//Exit: returns clock based integer.

user$clock_base (void)
{
    signed int
    Now[2];

    sys$gettim(Now);
    return ((Now[0]>>20)+(Now[1]<<12)) & 0x7FFF; // check
}

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

user$get_local_port(Pbase)
long * Pbase;
{
    signed long
    rval;

    *Pbase = *Pbase+1; // check
    rval = *Pbase % USER_LP_END; // check
    if (rval < USER_LP_START)
        rval = rval+USER_LP_START;
    return rval & 0x7FFF;
}

void    ACCESS_INIT();

void user$init_routines (void)
{
    TCP_User_LP = user$clock_base();
    UDP_User_LP = user$clock_base();
    ACCESS_INIT();
}



//SBTTL "Net$Debug - Debug maintenance call."

/*
Function:

    Maintenance level user call.  Sets the IPACP system wide debug level.
    Used to control volume of trace infor placed into IPACP log file.
    IF Global "LOG_State" > 0 then the log file is open & logging enabled.
    Otherwise the LOG file is closed.

Inputs:

    uargs = IPACP user argument block.

Outputs:

    None.

Side Effects:

    Debug level is reset.

*/

void Net$Debug(struct debug_args * uargs)
{
    switch (uargs->de$group)
    {
    case 0:
        LOG_CHANGE(uargs->de$level);
        break;
    case 1:
        ACT_CHANGE(uargs->de$level);
        break;
    };

    user$post_function_ok(uargs);
}



//SBTTL "Net$Event - Activity logging maintenance call."

/*
Function:
    Maintenance level user call.  Used to append messages to the end
    of the INET$ACTIVITY log file.

Inputs:
    uargs = IPACP user argument block.

Outputs:
    None.

Side Effects:
    Activity file is updated

*/

void Net$Event(struct event_args * uargs)
{
    extern  mm$get_mem(), mm$free_mem();
    signed long
    RC,
    Buffer;

    if ((RC=mm$get_mem(&Buffer,uargs->ev$buf_size)) != SS$_NORMAL)
    {
        USER$Err(uargs,RC);
        return;
    };

    $$KCALL(MOVBYT, uargs->ev$buf_size, uargs->ev$data_start, Buffer);

    ACT$FAO("!%D (PID:!XW) [!AD]!/", 0,
            ((long)uargs->ev$pid)&0xffff, uargs->ev$buf_size, Buffer); // check pid

    user$post_function_ok(uargs);
    mm$free_mem(Buffer,uargs->ev$buf_size);
}



//SBTTL "Net$SNMP - Simple Network Management Protocol routine."

/*
Function:
    Maintenance level user call.  Used to manipulate the IPACP

Inputs:
    uargs = IPACP user argument block.

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

void net$snmp(struct snmp_args * uargs)
{
    extern  SNMP$USER_INPUT(),
            mm$get_mem(), mm$free_mem();
#define RBBYTES D$User_Return_Blk_Max_Size
#define RBSIZE (RBBYTES+3)/4        // Largest dump block, in alloc units
    char * In_Buff;
    signed long
    RC,
    Error = FALSE,
    Now[2],         // time as in now.
    One[2] = {1,0}, // QuadWord of val 1.
             bufsize  = 0,
             RB[RBSIZE];

// Fetch the input data from kernal space.

    if ((RC=mm$get_mem(&In_Buff,uargs->snmp$wbuf_size)) != SS$_NORMAL)
    {
        USER$Err(uargs,RC);
        return;
    };

    $$KCALL(MOVBYT, uargs->snmp$wbuf_size, uargs->snmp$data_start, In_Buff);

// Determine which Dump Directive we have.

    /*
        switch (uargs->snmp$function)
        {

    // Return the dynamic memory allocation counts, # of times the free list for the
    // specified data structure was empty.  Used to figure out how many free list
    // elements to pre-allocate.

        case SNMP$C_Get;
        case SNMP$C_GetNext:
        case SNMP$C_Store:
        {
        bufsize = uargs->snmp$rbuf_size;
        Error = SNMP$USER_INPUT(In_Buff,uargs->snmp$wbuf_size,
                       RB+4,bufsize)
        };
        break;

        case SNMP$C_Kill:
    */

    if ((uargs->snmp$function == 4))
    {
        extern      tcp$kill();

        XLOG$FAO(LOG$USER,"!%T Kill !XL (bsize=!XL)!/",0,
                 In_Buff[0],uargs->snmp$wbuf_size);
        bufsize = 10;

        RC = tcp$kill(In_Buff[0]); // nu? what are you waiting for? kill!!!
        if (RC != SS$_NORMAL) Error = USER$Err(uargs,RC);

        CH$MOVE(bufsize,UPLIT("abcdefghij"),RB+4); // First long is size
    }

    /*
        default:
        Error = USER$Err(uargs,NET$_IFC); // Illegal Function code.
        };

    */
    else
        Error = USER$Err(uargs,NET$_IFC); // Illegal Function code.

// Did we have an Error or Illegal Dump directive code?

    mm$free_mem(In_Buff,uargs->snmp$wbuf_size);

    if (! Error)
    {

// Check to see if user buffer is large enough to hold requested data.
// If not return error: Buffer TOO small.

        if (uargs->snmp$rbuf_size < (bufsize + 4))
            USER$Err(uargs,NET$_BTS);   // user's buffer is TOO small.
        else
        {

// Copy local data into user's IO request buffer, Build movbyt arg list.

            RB[0] = bufsize;
            $$KCALL(MOVBYT,bufsize+4,RB,uargs->snmp$data_start);

// Post the user's IO request back to the user.

            user$post_io_status(uargs,SS$_NORMAL,bufsize+4,0,0);
            mm$uarg_free(uargs);    // Release user arg block.
        };
    };
}



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

void    gthst_purge();

void user$purge_all_io (void)
{
    extern  void tcp$purge_all_io();
    extern  void udp$purge_all_io();
    extern  void icmp$purge_all_io();
    extern  void ipu$purge_all_io();
    register
    qb;
    signed long
    expr[2];
    struct user_default_args * uargs;
    struct user_send_args * Sargs;
    netio_status_block * IOSTATUS;

// Set virtual device IP offline.  Prevent further user io.

    $$KCALL(set_ip_device_offline);

// Purge network I/O for all protocols

    tcp$purge_all_io();
    udp$purge_all_io();
    icmp$purge_all_io();
    ipu$purge_all_io();
    gthst_purge();

// check the user request queue again just to be safe.

    expr[0] = 5*TIMER_DELTA;    // 5 seconds in Delta time format.
    expr[1] = -1;
    sys$schdwk(0,0,expr,0);
    sys$hiber();    // check        // Make sure ALL IO has been queued.

// Purge User request queue.
// Special case M$Cancel as the IRP came from the IP: driver cancel routine
// & not from a user process, be sure NOT to post the IO//  USER$Err will delete
// the uargs block.

    while ((uargs=$$KCALL(user_requests_avail)) != FALSE)
    {

// post the user's io request with an error code: tcp is exiting.

        if (uargs->ud$funct != M$CANCEL)
            user$post_io_status(uargs,NET$_TE,0,0,0);
//!!HACK!!// Don't release the Uarg?
    };
}



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

void    GTHST_CANCEL();

user$brk (void)
{
    return    SS$_NORMAL;
}

void VMS$Cancel(struct vms$cancel_args * uargs)
{
    extern  tcp$cancel();
    extern  udp$cancel();
    extern  icmp$cancel();
    extern  ipu$cancel();
    signed long
    ucbptr,
    proto,
    Done;

    ucbptr = uargs->vc$ucb_adrs; // check + UCB$L_EXTRA;
    $$KCALL(MOVBYT,4,ucbptr,&proto);

    XLOG$FAO(LOG$USER,"!%T VMS$Cancel: PID=!XL, Chan=!XL, UCB proto=!XL!/",
             0,uargs->vc$pid,uargs->vc$piochan,proto);

    Done = 0;
    switch (uargs->vc$protocol)
    {
    case U$TCP_PROTOCOL:
        Done = tcp$cancel(uargs);
        break;

    case U$UDP_PROTOCOL:
        Done = udp$cancel(uargs);
        break;

    case U$ICMP_PROTOCOL:
        Done = icmp$cancel(uargs);
        break;

    case U$IP_PROTOCOL:
        Done = ipu$cancel(uargs);
        break;

    default:
        Done = user$brk();
    };

    GTHST_CANCEL(uargs);
    mm$uarg_free(uargs);        // Release IPACP argument block
}

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

void net$dump(struct debug_dump_args * uargs)
{
    extern  CALCULATE_UPTIME();
    extern  TEK$sys_uptime;
    register
    struct queue_blk_structure(qb_ur_fields) * QB;  // queue block pointer.
#define RBBYTES D$User_Return_Blk_Max_Size
#define RBSIZE (RBBYTES+3)/4        // Largest dump block, in alloc units
    signed long
    rc,
    Error = FALSE,
    bufsize,
    count = 0,
    RB[RBSIZE];

// Determine which Dump Directive we have.

    switch (uargs->du$dump_directive)
    {

// Return the dynamic memory allocation counts, # of times the free list for the
// specified data structure was empty.  Used to figure out how many free list
// elements to pre-allocate.

    case DU$DYNAMIC_MEM_ALLOC:
    {
        d$mem_alloc_return_blk * rb = RB;
        extern char
        qblk_count_base,
        uarg_count_base ,
        min_seg_count_base,
        max_seg_count_base,
        qblk_count,
        uarg_count,
        min_seg_count,
        max_seg_count;

        rb->dm$qb = qb_gets;    // queue blocks
        rb->dm$ua = ua_gets;    // User net io argument blks.
        rb->dm$cs = 0;      //~~~ OBSOLETE
        rb->dm$dms = min_gets;  // Minimum (default) size packet buffers
        rb->dm$nm = max_gets;   // Maximum size packet buffers
        rb->dm$qbmx = qb_max;   // queue blocks
        rb->dm$uamx= ua_max;    // User net io argument blks.
        rb->dm$csmx = 0;    //~~~ OBSOLETE
        rb->dm$dmsmx = min_max;// Minimum (default) size buffers.
        rb->dm$nmmx = max_max;  // Maximum size buffer.
        rb->dm$qbal = qblk_count_base;
        rb->dm$uaal = uarg_count_base;
        rb->dm$csal = 0;
        rb->dm$dmsal = min_seg_count_base;
        rb->dm$nmal = max_seg_count_base;
        rb->dm$qbfr = qblk_count;
        rb->dm$uafr = uarg_count;
        rb->dm$csfr = 0;
        rb->dm$dmsfr = min_seg_count;
        rb->dm$nmfr = max_seg_count;
        bufsize = D$MA_BLKSIZE;
    };

    case DU$TCP_STATS:
    {
        d$tcp_stats_return_blk * rb = RB;

        rb->dm$tcpacp_pid       = mypid;
        rb->dm$user_io_requests     = ts$uir;
//!!HACK!!//  // storeForward does not belong here...
        rb->dm$storeforward     = 0;
        rb->dm$active_conects_opened    = ts$aco;
        rb->dm$passive_conects_opened   = ts$pco;
        rb->dm$data_bytes_xmitted   = ts$dbx;
        rb->dm$data_bytes_recved    = ts$dbr;
        rb->dm$segs_xmitted     = ts$sx;
        rb->dm$segs_recved      = ts$sr;
        rb->dm$seg_bad_chksum       = ts$seg_bad_cksum;
        rb->dm$badseq           = ts$badseq;
        rb->dm$duplicate_segs       = ts$duplicate_segs;
        rb->dm$retrans_segs     = ts$retrans_segs;
        rb->dm$rpz_rxq          = ts$rpz_rxq;
        rb->dm$oorw_segs        = ts$oorw_segs;
        rb->dm$future_rcvd      = ts$future_rcvd;
        rb->dm$future_used      = ts$future_used;
        rb->dm$future_dropped       = ts$future_dropped;
        rb->dm$future_dups      = ts$future_dups;
        rb->dm$servers_forked       = ts$servers_forked;

// Compute TCP uptime.
        CALCULATE_UPTIME();
        ch$move(8,TEK$sys_uptime,rb->dm$uptime);

        bufsize = D$TS_BLKSIZE; // byte size of return blk.
    };

// Return all active local-connection-id's otherwise known as the address of the
// connection's TCB.
// 0th element of return vector is the count of valid tcb addresses in the
// return vector (ie, counted vector).

    case DU$LOCAL_CONNECTION_ID:
    {
        void        tcp$connection_list();
        tcp$connection_list(RB);
        bufsize = D$LC_ID_BLKSIZE;
    };

// Dump out a TCB

    case DU$TCB_DUMP:
    {
        extern     tcp$tcb_dump();
        if (tcp$tcb_dump(uargs->du$arg0,RB))
            bufsize = D$TCB_DUMP_BLKSIZE;
        else
            Error = USER$Err(uargs,NET$_CDE);
    };

// Return all UDP connections (as D$Local_Connection_ID above)

    case DU$UDP_CONNECTIONS:
    {
        void        udp$connection_list();
        udp$connection_list(RB);
        bufsize = D$UDP_LIST_BLKSIZE;
    };

// Dump out a UDPCB

    case DU$UDPCB_DUMP:
    {
        extern      udp$udpcb_dump();
        if (udp$udpcb_dump(uargs->du$local_conn_id,RB))
            bufsize = D$UDPCB_DUMP_BLKSIZE;
        else
            Error = USER$Err(uargs,NET$_CDE);
    };

// Return all ICMP connections (as D$Local_Connection_ID above)

    case DU$ICMP_CONNECTIONS:
    {
        extern void         icmp$connection_list();
        icmp$connection_list(RB);
        bufsize = D$ICMP_LIST_BLKSIZE;
    };

// Dump out a ICMPCB

    case DU$ICMPCB_DUMP:
    {
        extern     icmp$icmpcb_dump();
        if (icmp$icmpcb_dump (uargs->du$local_conn_id,RB))
            bufsize = D$ICMPCB_DUMP_BLKSIZE;
        else
            Error = USER$Err(uargs,NET$_CDE);
    };

// Get device-depandent dump from device driver module

    case DU$DEVICE_DUMP:
    {
        if ((uargs->du$device_idx >= 0) &&
                (uargs->du$device_idx <= DC_Max_Num_Net_Devices-1) &&
                (dev_config_tab[uargs->du$device_idx].dc_valid_device))
        {
            // call device dump routine.
            bufsize = uargs->du$buf_size;
            if (bufsize > RBBYTES)
                bufsize = RBBYTES;
            rc = (dev_config_tab[uargs->du$device_idx].dc_rtn_Dump)
                 (uargs->du$device_idx, uargs->du$arg1, uargs->du$arg2,
                  RB, bufsize);
            if (!(rc & 1))
                Error = USER$Err(uargs,NET$_EPD); // error processing dump
        }
        else
            Error = USER$Err(uargs,NET$_BDI); // error: bad device index
    };

// Dump out ARP cache entries.

    case DU$ARP_CACHE:
    {
        signed long
        USIZE,
        RMOD;
//  externAL ROUTINE
//      ARP_DUMP;

        Error = USER$Err(uargs,NET$_IFC); // Illegal Function code.

// Compute size of return block - make multiple of dump block size

//  USIZE = uargs->du$buf_size;
//  if (USIZE > RBBYTES)
//      USIZE = RBBYTES;
//  RMOD = USIZE MOD D$ARP_Dump_Blksize;
//  USIZE = USIZE - RMOD;
//  if (USIZE <= 0)
//      Error = USER$Err(uargs,NET$_BTS)
//  else
//      {
//      bufsize = ARP_DUMP(uargs->du$start_index,RB,USIZE);
//      if (bufsize < 0)
//      Error = USER$Err(uargs,NET$_DAE);
//      };
    };

// Get list of device indexes.

    case DU$DEVICE_LIST:
    {
        extern      cnf$device_list();
        bufsize = cnf$device_list(RB);
    };

    case DU$DEVICE_STAT:
    {
        extern      cnf$device_stat();
        if (uargs->du$buf_size < DC_ENTRY_SIZE)
            Error = USER$Err(uargs,NET$_BTS);
        else if (cnf$device_stat ( uargs->du$arg0, RB ) == -1)
            Error = USER$Err(uargs,NET$_DAE);
        else
            bufsize = D$DEV_DUMP_BLKSIZE;
    };

// Undefined function code - give error

    default:
        Error = USER$Err(uargs,NET$_IFC); // Illegal Function code.
    };

// Did we have an Error or Illegal Dump directive code?

    if (! Error)
    {

// Check to see if user buffer is large enough to hold requested data.
// If not return error: Buffer TOO small.

        if (uargs->du$buf_size < bufsize)
            USER$Err(uargs,NET$_BTS);   // user's buffer is TOO small.
        else
        {

// Copy local data into user's IO request buffer, Build movbyt arg list.

            $$KCALL(MOVBYT,bufsize,RB,uargs->du$data_start);

// Post the user's IO request back to the user.

            user$post_io_status(uargs,SS$_NORMAL,bufsize,0,0);
            mm$uarg_free(uargs);    // Release user arg block.
        };
    };
}

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

void Net$EXIT(struct debug_exit_args * uargs)
{

    XLOG$FAO(LOG$USER,"!%T EXIT requested, User PID: !XL!/",0,uargs->ex$pid);

    user$post_function_ok(uargs);
    Time_2_Exit = TRUE;     // Set global for exit, rtn: start_network.
    $$KCALL(set_ip_device_offline); // mark network device(s) offline.
}

//SBTTL "Network access check routines"

struct GETJPI_BLOCK
{
    short int    BUFLEN;
    short int ITEM;
    void * BUFADR;
    void * RETLEN;
    long int LISTEND;
};
#if 0
LITERAL
GETJPI_SIZE = $FIELD_SET_SIZE;
MACRO
GETJPI_BLOCK = BLOCK->GETJPI_SIZE FIELD(GETJPI_FIELDS) %;
#endif

user$privileged(PID)
//
// Verify the user has privileges to use a well-known local port. User must
// have PHY_IO privilege.
// Returns TRUE if user has the privilege, FALSE otherwise.
//
{
    struct GETJPI_BLOCK JPI_, * JPI = &JPI_;
    signed long
    PRVLEN;
    union _prvdef PRVBUF;

// Fill in GETJPI request block.

    JPI->BUFLEN = 4;
    JPI->ITEM = JPI$_CURPRIV;
    JPI->BUFADR = &PRVBUF;
    JPI->RETLEN = &PRVLEN;
    JPI->LISTEND = 0;

// Request the priviliges for the process.

    if ((sys$getjpiw(0,PID,0,JPI,0,0,0)))
        if (PRVBUF.prv$v_phy_io || PRVBUF.prv$v_setprv)
            return SS$_NORMAL;
    return NET$_NOPRV;
}


check_id(PID,ID)
//
// Check that a user holds a given rights identifier. The identifiers of
// interest to us are ARPANET_ACCESS and ARPANET_WIZARD.
// Returns TRUE if the user has the necessary ID, FALSE otherwise.
//
{
    struct GETJPI_BLOCK JPI_, * JPI=&JPI_;
    signed long
    STATUS,
    UICLEN,
    UICBLK[2],
    RDBCTX,
    CURID;

// Fill in the GETJPI block

    JPI->BUFLEN = 4;
    JPI->ITEM = JPI$_UIC;
    JPI->BUFADR = UICBLK;
    JPI->RETLEN = &UICLEN;
    JPI->LISTEND = 0;

// Retrieve the UIC for the process

    if (! sys$getjpiw(0,PID,0,JPI,0,0,0))
        return FALSE;

// Check the rights database for this user

    UICBLK[1] = 0;
    RDBCTX = 0;
    CURID = ID;
    while ((STATUS = sys$find_held(UICBLK,ID,0,RDBCTX)))
    {
        if (CURID == ID)
        {
            exe$finish_rdb(RDBCTX);
            return TRUE;
        };
    };

// Didn't find it - punt.

    return FALSE;
}


#define WKS$SMTP 25     // Well known port number for SMTP

user$check_access(PID,LCLHST,LCLPRT,FRNHST,FRNPRT)
//
// Main routine to check for network access.
// Returns SS$_NORMAL if access is granted, or error code.
//
{
    long I;

// If no access checking is enabled, then skip this routine

    if (access_flags == 0)
        return SS$_NORMAL;

// If we're checking acess for any network open, then check for INTERNET_ACCESS

    if (access_flags&ACF_ALLOPENS)
        if (! check_id(PID,INTERNET_ID))
            return NET$_NOINA;

// If we're checking access to non-local hosts, then do so

    if (access_flags&ACF_ARPAHOST)
X:
    {

// If the foreign host is in the "local hosts" list, then allow it.

        for (I=(ACHOST_COUNT-1); I>=0; I--)
            if ((FRNHST & ACHOSTS[I].AC$MASK) == ACHOSTS[I].AC$HOST)
                goto leave_x;
        if (! check_id(PID,ARPANET_ID))
            return NET$_NOANA;
    };
leave_x:

// If the local port is privileged, then require special privilege

    if (access_flags&ACF_PRIVPORT)
        if ((((LCLPRT & 0xFFFF) >= WELL_KNOWN_LP_START) &&
                ((LCLPRT & 0xFFFF) <= WELL_KNOWN_LP_END) && FRNPRT == 0) ||
                (FRNPRT == WKS$SMTP))
            if (! user$privileged(PID))
                return NET$_NOPRV;

// Passed all of the tests - let them have access to the network

    return SS$_NORMAL;
}


void user$access_config(HOSTNUM,HOSTMASK)
//
// Add an entry to the list of allowed local hosts. Called by CONFIG when
// LOCAL_HOST entry seen in the config file.
//
{

// Make sure there is room for this entry

    if (ACHOST_COUNT >= ACCESS_MAX)
    {
        DESC$STR_ALLOC(HSTSTR,20);
        extern void         ASCII_DEC_BYTES();

        ASCII_DEC_BYTES(HSTSTR,4,HOSTNUM,HSTSTR->dsc$w_length);
        OPR$FAO("Local hosts list full - not adding entry for !AS",HSTSTR);
    };

// Insert it into the table

    ACHOSTS[ACHOST_COUNT].AC$HOST = HOSTNUM;
    ACHOSTS[ACHOST_COUNT].AC$MASK = HOSTMASK;
    ACHOST_COUNT = ACHOST_COUNT + 1;
}

void ACCESS_INIT (void)
//
// Perform necessary initialzation for host access control. Translates the
// necessary identifiers via $ASCTOID and checks consistancy of switches and
// access list.
//
{

// If access to network check enabled, translate INTERNET_ACCESS rights ID

    if (access_flags&ACF_ALLOPENS)
    {
        if (! sys$asctoid(&INTERNET_STRING,
                          &INTERNET_ID, 0))
        {
            OPR$FAO("% Failed to find identifier !AS - access check disabled",
                    INTERNET_STRING);
            access_flags&=~ACF_ALLOPENS;
        };
    };

// If ARPANET access check enabled, translate ARPANET_ACCESS rights ID and
// verify that some hosts exist in the host list.

    if (access_flags&ACF_ARPAHOST)
    {
        if (! sys$asctoid(&ARPANET_STRING,
                          &ARPANET_ID, 0))
        {
            OPR$FAO("% Failed to find identifier !AS - access check disabled",
                    ARPANET_STRING);
            access_flags&=~ACF_ARPAHOST;
        };
        if (ACHOST_COUNT == 0)
        {
            OPR$FAO("% No local hosts list - ARPANET access check disabled");
            access_flags&=~ACF_ARPAHOST;
        };
    };
}

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

void    GTHST_NMLOOK_DONE();
void    GTHST_ADLOOK_DONE();
void    GTHST_RRLOOK_DONE();

#define    NLBSIZE GTHST_NMLOOK_RET_ARGS_LENGTH
#define    ALBSIZE GTHST_ADLOOK_RET_ARGS_LENGTH
#define    RLBSize 6

void net$gthst(struct gthst_args * uargs)
{
    long IDX;

// Dispatch the GTHST subfunction

    if (uargs->gh$subfunct >= GTH_MIN && uargs->gh$subfunct <= GTH_MAX)
        switch ( uargs->gh$subfunct)
        {

// Local host info - same as name to address-list w/o doing name lookup.

        case GTH_LCLHST:
        {
            signed long
            Args[4];
            gthst_nmlook_block RBLOCK_, * RBLOCK =&RBLOCK_; // check struct gthst_nmlook_args
            long *ADRVEC= &RBLOCK->ghn$adrlst;
            extern
            dev_count;
            extern
            Device_Configuration_Entry dev_config_tab[] ;

// Verify size of return block.

            if (uargs->gh$bufsize < NLBSIZE)
            {
                USER$Err(uargs,NET$_BTS);
                return;
            };

// Get the list of addresses from the configuration table

            for (IDX=0; IDX<=(dev_count-1); IDX++)
                ADRVEC[IDX] = dev_config_tab[IDX].dc_ip_address;
            RBLOCK->ghn$adrcnt = dev_count;

// Copy the name of the local host from the local info

            CH$MOVE(Local_Name.dsc$w_length,Local_Name.dsc$a_pointer,
                    CH$PTR(RBLOCK->ghn$namstr,0));
            RBLOCK->ghn$namlen = Local_Name.dsc$w_length;

// Return data to the user

            $$KCALL(MOVBYT,NLBSIZE,RBLOCK,uargs->gh$data_start);

// And give them a good status reply

            user$post_io_status(uargs,SS$_NORMAL,NLBSIZE,0,0);
            mm$uarg_free(uargs);
        };
        break;

// Name to address-list lookup

        case GTH_NMLOOK:
        {
            struct gthst_nmlook_args * uargs2=uargs;

            if (uargs2->ghn$bufsize < NLBSIZE)
            {
                USER$Err(uargs,NET$_BTS);
                return;
            };

            NML$GETALST(CH$PTR(uargs2->ghn$hstnam,0),uargs2->ghn$hstlen,
                        GTHST_NMLOOK_DONE,uargs2);
        };
        break;

// Address to name lookup

        case GTH_ADLOOK:
        {
            struct gthst_adlook_args * uargs2=uargs;
            signed long
            addr;

            if (uargs2->gha$bufsize < ALBSIZE)
            {
                USER$Err(uargs2,NET$_BTS);
                return;
            };

// User argument is IP address - 0 means local address.

            addr = uargs2->gha$ipaddr;
            if (addr == 0)
                addr = dev_config_tab[0].dc_ip_address;
            NML$GETNAME(addr,GTHST_ADLOOK_DONE,uargs2);
        };
        break;

// Domain resource record query.

        case GTH_RRECLK:
        {
            struct gthst_rrlook_args * uargs2=uargs;

            if (uargs2->grr$bufsize < RLBSize)
            {
                USER$Err(uargs2,NET$_BTS);
                return;
            };

            NML$GETRR(uargs2->grr$rrtype,
                      CH$PTR(uargs2->grr$hstnam,0),uargs2->grr$hstlen,
                      GTHST_RRLOOK_DONE,uargs2);
        };
        break;

// Unknown GTHST function

        default:
            USER$Err(uargs,NET$_IGF);
        };
}

//SBTTL "Name lookup done handler"
/*
    Come here when name GTH_NMLOOK function has completed.
    Called at AST level when NM_GETALST completes for the GTHST function.
    Post the user request with the completion code.
*/

void GTHST_NMLOOK_DONE(uargs,Status,Adrcnt,Adrlst,namlen,Nambuf)
struct gthst_nmlook_args * uargs;
{
    signed long
    Args[4];
    gthst_nmlook_block NLB_, * NLB =&NLB_;

// If an error occurred, give it to the user

    if (! Status)
    {
        USER$Err(uargs,Status);
        return;
    };

// Copy the return data into prototype block

    NLB->ghn$adrcnt = Adrcnt;
    CH$MOVE(Adrcnt*4,Adrlst,NLB->ghn$adrlst);
    NLB->ghn$namlen = namlen;
    CH$MOVE(namlen,Nambuf,NLB->ghn$namstr);

// Copy the block back to the user.

    $$KCALL(MOVBYT,NLBSIZE,NLB,uargs->ghn$data_start);

// And give them a good status reply

    user$post_io_status(uargs,SS$_NORMAL,NLBSIZE,0,0);
    mm$uarg_free(uargs);
}

//SBTTL "Address lookup done handler"
/*
    Same as above, but for GTH_ADLOOK function.
*/

void GTHST_ADLOOK_DONE(uargs,Status,namlen,Nambuf)
struct gthst_adlook_args * uargs;
{
    signed long
    Args[4];
    gthst_adlook_block ALB_, * ALB= &ALB_;

// If an error occurred, give it to the user

    if (! Status)
    {
        USER$Err(uargs,Status);
        return;
    };

// Build the return block

    ALB->gha$namlen = namlen;
    CH$MOVE(namlen,Nambuf,ALB->gha$namstr);

// Return data to the user

    $$KCALL(MOVBYT,ALBSIZE,ALB,uargs->gha$data_start);

// And give them a good status reply

    user$post_io_status(uargs,SS$_NORMAL,ALBSIZE,0,0);
    mm$uarg_free(uargs);
}

//SBTTL "RR lookup done handler"
/*
    Come here when name GTH_RRLOOK function has completed.
    Called at AST level when NM_GETRR completes for the GTHST function.
    Post the user request with the completion code.
*/

void GTHST_RRLOOK_DONE(uargs,Status,RDLen,RData,namlen,Nambuf)
struct gthst_rrlook_args * uargs;
{
    signed long
    Args[4];
    gthst_rrlook_block RLB_, * RLB= &RLB_;

// If an error occurred, give it to the user

    if (! Status)
    {
        USER$Err(uargs,Status);
        return;
    };

// Copy the return data into prototype block

    if (uargs->grr$bufsize < RLBSize + RDLen)
    {
        USER$Err(uargs,NET$_BTS);
        return;
    };

    RLB->grr$rdlen = RDLen;
//    RLB->GRR$NAMLEN = namlen;
//    CH$MOVE(RDLen,RData,RLB->GRR$DATA);
//    CH$MOVE(namlen,Nambuf,RLB->grr$data+.RDLen);

// Copy the block back to the user.

    $$KCALL(MOVBYT,2,RLB,uargs->grr$data_start);
    $$KCALL(MOVBYT,RDLen,RData,uargs->grr$data_start + 2);
//    $$KCALL(MOVBYT,namlen,Nambuf,uargs->grr$data_start + 6 + RDLen);

// And give them a good status reply

    user$post_io_status(uargs,SS$_NORMAL, RLBSize + RDLen,0,0);
    mm$uarg_free(uargs);
}

//SBTTL "GTHST_CANCEL - Cancel GTHST requests for a process"

void    GTHST_CANCEL_ONE();

void GTHST_CANCEL(struct vms$cancel_args * uargs)
//
// Search the list of pending GTHST requests looking for match. If found,
// post it now and delete from the queue.
//
{
    NML$STEP(GTHST_CANCEL_ONE,uargs);
}

void GTHST_CANCEL_ONE(VCUARGS,ASTADR,UARGS)
//
// Check a single entry from the name lookup queue to see if it belongs to
// the process that is doing the cancel. If so, we will cancel it.
//
struct vms$cancel_args * VCUARGS;
struct user_default_args * UARGS;
{

// Do sanity check on AST routine. Only GTHST done routines should be attached
// to GTHST requests.

    if ((ASTADR != GTHST_NMLOOK_DONE) &&
            (ASTADR != GTHST_ADLOOK_DONE) &&
            (ASTADR != GTHST_RRLOOK_DONE))
        return;

// See if the UCB of the cancellor is the same as that of the request. If so,
// cancel the request (i.e. "finish" it with an error).


    if (VCUARGS->vc$ucb_adrs == UARGS->ud$ucb_adrs)
        NML$CANCEL(UARGS,TRUE,NET$_CCAN);
}

void    GTHST_PURGE_ONE();

void gthst_purge (void)
//
// Routine to purge all requests when network ACP exits. Step through all
// requests (with NML$STEP) and punt them.
//
{
    NML$STEP(GTHST_PURGE_ONE,0);
}

void GTHST_PURGE_ONE(COVALUE,ASTADR,UARGS)
//
// Coroutine for NML$STEP/GTHST_PURGE
//
{

// Do sanity check on AST routine. Only GTHST done routines should be attached
// to GTHST requests.

    if ((ASTADR != GTHST_NMLOOK_DONE) &&
            (ASTADR != GTHST_ADLOOK_DONE) &&
            (ASTADR != GTHST_RRLOOK_DONE))
        return;
    NML$CANCEL(UARGS,TRUE,NET$_TE);
}

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


void user$process_user_requests (void)
{

// TCP functions:
    extern  void TCP$OPEN();
    extern  void TCP$CLOSE();
    extern  void TCP$ABORT();
    extern void TCP$SEND ();
    extern  void TCP$RECEIVE();
    extern  void TCP$INFO();
    extern  void TCP$STATUS();

// UDP functions:
    extern  void UDP$OPEN();
    extern  void UDP$CLOSE();
    extern  void udp$abort();
    extern void     UDP$SEND ();
    extern  void UDP$RECEIVE();
    extern  void udp$info();
    extern  void udp$status();

// ICMP functions:
    extern  void ICMP$OPEN();
    extern  void ICMP$CLOSE();
    extern  void icmp$abort();
    extern void     ICMP$SEND ();
    extern  void ICMP$RECEIVE();
    extern  void icmp$info();
    extern  void icmp$status();

// IP functions:
    extern  void IPU$OPEN();
    extern  void IPU$CLOSE();
    extern  void ipu$abort();
    extern  void IPU$SEND ();
    extern  void IPU$RECEIVE();
    extern  void ipu$info();
    extern  void IPU$STATUS();
    register
    struct user_default_args * argblk;

#ifdef __x86_64__
    mm$uarg_free(mm$uarg_get()); // check. avoid syscall at all costs
#endif
    while ((argblk=$$KCALL(user_requests_avail)) != FALSE)
    {
        if ($$LOGF(LOG$USER))
        {
            DESC$STR_ALLOC(funcstr,30);
            struct dsc$descriptor
                    func;
            switch (argblk->ud$funct)
        {
            case U$OPEN:
                func=ASCID("TEK$OPEN");
                break;
            case U$SEND:
                func=ASCID("TEK$SEND");
                break;
            case U$RECV:
                func=ASCID("TEK$RECEIVE");
                break;
            case U$CLOSE:
                func=ASCID("TEK$CLOSE");
                break;
            case U$ABORT:
                func=ASCID("TEK$ABORT");
                break;
            case U$STATUS:
                func=ASCID("TEK$STATUS");
                break;
            case U$INFO:
                func=ASCID("TEK$INFO");
                break;

            case U$GTHST:
                func=ASCID("Net$GTHST");
                break;
            case M$DUMP:
                func=ASCID("Net$DUMP");
                break;
            case M$EXIT:
                func=ASCID("Net$EXIT");
                break;
            case M$DEBUG:
                func=ASCID("Net$Debug");
                break;
            case M$EVENT:
                func=ASCID("Net$Event");
                break;
            case M$SNMP:
                func=ASCID("Net$SNMP");
                break;

            case M$CANCEL:
                func=ASCID("VMS$Cancel");
                break;
            default:
            {
                $DESCRIPTOR(ctr,"?User FCN !SL");
                sys$fao(&ctr,&funcstr->dsc$w_length,funcstr,
                        argblk->ud$funct);
                func = *funcstr;
            };
            };
            LOG$FAO("!%T !AS (Proto:!XB), uargs=!XL,VMSID=!XL,IRP=!XL,UCB=!XL!/",           0, &func, argblk->ud$protocol,
                    argblk, argblk->ud$vms_blk_id,
                    argblk->ud$irp_adrs, argblk->ud$ucb_adrs,
                    argblk->ud$pid);
        };

        if (argblk->ud$funct != M$CANCEL)
            ts$uir = ts$uir + 1; // count arrival of user io request.
        else VMS$Cancel(argblk);

// dispatch to appro user io request handler routine.  Cover both tcp
// and maintenance functions.

        if (argblk->ud$protocol == U$TCP_PROTOCOL)
            // Handle TCP functions first to improve speed.
            if ( argblk->ud$funct >=  U$OPEN && argblk->ud$funct <= U$MAX_TCP_FUNCTION)
                switch ( argblk->ud$funct)
                {
                case U$OPEN:
                    tcp$open(argblk);
                    break;
                case U$SEND:
                    tcp$send(argblk);
                    break;
                case U$RECV:
                    tcp$receive(argblk);
                    break;
                case U$CLOSE:
                    tcp$close(argblk);
                    break;
                case U$ABORT:
                    tcp$abort(argblk);
                    break;
                case U$STATUS:
                    tcp$status(argblk);
                    break;
                case U$INFO:
                    tcp$info(argblk);
                    break;
                default:
                    USER$Err(argblk,NET$_IFC); // Illegal Function Code.

                }
            else if ( argblk->ud$funct >= U$OPEN && argblk->ud$funct <= U$MAX_USER_FUNCTION)
                switch ( argblk->ud$funct)
                {

                case U$OPEN:
                    switch (argblk->ud$protocol)
                    {
                    case U$UDP_PROTOCOL:
                        udp$open(argblk);
                        break;
                    case U$ICMP_PROTOCOL:
                        icmp$open(argblk);
                        break;
                    case U$IP_PROTOCOL:
                        ipu$open(argblk);
                        break;
                    default:
                        USER$Err(argblk,NET$_IPC); // Illegal Protocol Code.
                    };

                case U$SEND:
                    switch (argblk->ud$protocol)
                    {
                    case U$UDP_PROTOCOL:
                        udp$send(argblk);
                        break;
                    case U$ICMP_PROTOCOL:
                        icmp$send(argblk);
                        break;
                    case U$IP_PROTOCOL:
                        ipu$send(argblk);
                        break;
                    default:
                        USER$Err(argblk,NET$_IPC); // Illegal Protocol Code.
                    };

                case U$RECV:
                    switch (argblk->ud$protocol)
                    {
                    case U$UDP_PROTOCOL:
                        udp$receive(argblk);
                        break;
                    case U$ICMP_PROTOCOL:
                        icmp$receive(argblk);
                        break;
                    case U$IP_PROTOCOL:
                        ipu$receive(argblk);
                        break;
                    default:
                        USER$Err(argblk,NET$_IPC); // Illegal Protocol Code.
                    };

                case U$CLOSE:
                    switch (argblk->ud$protocol)
                    {
                    case U$UDP_PROTOCOL:
                        udp$close(argblk);
                        break;
                    case U$ICMP_PROTOCOL:
                        icmp$close(argblk);
                        break;
                    case U$IP_PROTOCOL:
                        ipu$close(argblk);
                        break;
                    default:
                        USER$Err(argblk,NET$_IPC); // Illegal Protocol Code.
                    };

                case U$ABORT:
                    switch (argblk->ud$protocol)
                    {
                    case U$UDP_PROTOCOL:
                        udp$abort(argblk);
                        break;
                    case U$ICMP_PROTOCOL:
                        icmp$abort(argblk);
                        break;
                    case U$IP_PROTOCOL:
                        ipu$abort(argblk);
                        break;
                    default:
                        USER$Err(argblk,NET$_IPC); // Illegal Protocol Code.
                    };

                case U$STATUS:
                    switch (argblk->ud$protocol)
                    {
                    case U$UDP_PROTOCOL:
                        udp$status(argblk);
                        break;
                    case U$ICMP_PROTOCOL:
                        icmp$status(argblk);
                        break;
                    case U$IP_PROTOCOL:
                        ipu$status(argblk);
                        break;
                    default:
                        USER$Err(argblk,NET$_IPC); // Illegal Protocol Code.
                    };

                case U$INFO:
                    switch (argblk->ud$protocol)
                    {
                    case U$UDP_PROTOCOL:
                        udp$info(argblk);
                        break;
                    case U$ICMP_PROTOCOL:
                        icmp$info(argblk);
                        break;
                    case U$IP_PROTOCOL:
                        ipu$info(argblk);
                        break;
                    default:
                        USER$Err(argblk,NET$_IPC); // Illegal Protocol Code.
                    };

                case U$GTHST:
                    net$gthst(argblk);
                    break;

                default:
                {
                    switch (argblk->ud$funct) // check acp maintenance functions
                    {
                    case M$DUMP:
                        net$dump(argblk);
                        break;
                    case M$EXIT:
                        Net$EXIT(argblk);
                        break;
                    case M$DEBUG:
                        Net$Debug(argblk);
                        break;
                    case M$EVENT:
                        Net$Event(argblk);
                        break;
                    case M$SNMP:
                        net$snmp(argblk);
                        break;
//      case M$Cancel:  VMS$Cancel(argblk);
                    case M$CANCEL:
                        SS$_NORMAL;
                    default :
                        USER$Err(argblk,NET$_IFC); // Illegal Function Code.
                    };
                };
                };
#ifdef __x86_64__
        mm$uarg_free(mm$uarg_get()); // check. avoid syscall at all costs
#endif
    };
}

