//
//  ****************************************************************
//
//      Copyright (c) 1992, Carnegie Mellon University
//
//      All Rights Reserved
//
//  Permission  is  hereby  granted   to  use,  copy,  modify,  and
//  distribute  this software  provided  that the  above  copyright
//  notice appears in  all copies and that  any distribution be for
//  noncommercial purposes.
//
//  Carnegie Mellon University disclaims all warranties with regard
//  to this software.  In no event shall Carnegie Mellon University
//  be liable for  any special, indirect,  or consequential damages
//  or any damages whatsoever  resulting from loss of use, data, or
//  profits  arising  out of  or in  connection  with  the  use  or
//  performance of this software.
//
//  ****************************************************************
//
//  .TITLE  IPDRIVER - Internet Protocol Virtual Device Driver
//  .IDENT  /6.6/
//  .Library /sys$library:lib.mlb/
//  .enable sup
//  .disable debug,traceback
//
// FACILITY:
//
//  Internet Protocol Virtual/Sharable network device driver. Similar
//  to DEC's "net:" device.
//
// ABSTRACT:
//
//  This driver provides a sharable virtual device which queues
//  I/O Requests to the IPACP (Internet Protocol ACP).  This driver
//  builds Argument blocks for the ACP user interface handler.
//  Various VMS QIO function codes have been redefined to fit the
//  IP network functionality:  Format of NET$xxxx (VMS counterpart).
//
//  NET$OPEN (IO$_Create)
//      OPEN a network connection.
//  NET$SEND (IO$_WRITELBLK)
//      Transmit data over the network.
//  NET$RECEIVE (IO$_READLBLK)
//      Receive data from the network
//  NET$CLOSE (IO$_Delete)
//      Close a network connection.
//  NET$ABORT (IO$_Deaccess)
//      ABORT a network connection
//  NET$NCP (IO$_ACPCONTROL)
//      Perform an Network Kernal Control operation.
//
// The network IO status is returned in the traditional VMS IO status block.
// Here we redefine some of the fields to reflect network operatons.
// If the VMS return code is SS$_NORMAL then the IO operation completed
// successfully.  Otherwise the Error code will a traditional VMS error code.
// If the error code is not SS$_ABORT then the error was detected in the virtual
// device (IP:) driver & will reflect the error ala VMS (SS$_ACCVIO etc.).
// If the VMS error code is SS$_ABORT then the error was detected in IP &
// the net error code field will reflect the actual error (SS$_Abort is used as
// a VMS error escape).  During the NET$OPEN call, the 2nd bliss fullword
// is the Local-connection_ID if the VMS return code is SS$_NORMAL otherwise
// the net error field reflects the error.  The IO request TAG identifies the
// IO request for those who use asynchronous IO ($qio).  The flags field
// indicates URGENT data & EOL (End Of Letter) conditions. Note that the ICMP
// code field is only defined for special connections which can return ICMP
// messages and only if the ICMP flag is set.
//
//
//  !===============================================================!
//  +     Bytes Transfered      !   VMS Return Code     +
//  !---------------------------------------------------------------!
//  +       Unused      !     Flags !   ICMP code   +
//  !===============================================================!
//
// Privileges Required:
//
//  NETMBX
//  PHY_IO for maintenance functions.
//
// AUTHOR:
//
//  Bruce R. Miller     CMU Network Development
//  Originaly Stan C. Smith     29-Sep-1981
//
// Modification History:
//

//++
// For VAX-VMS V4 systems, uncomment the following line
//--

//;;;;VMS_V4=1


//  .SBTTL  External and local symbol definitions

//
// External symbols
//

#include <aqbdef.h>          // ACP queue block.
#include <ccbdef.h>          // Channel Control Block.
#include <crbdef.h>          // Channel request block
#include <dcdef.h>           // Device classes and types
#include <ddbdef.h>          // Device data block
#include <ddtdef.h>          // Driver Dispatch Table
#include <devdef.h>          // Device characteristics
#include <idbdef.h>          // Interrupt data block
#include <iodef.h>           // I/O function codes
#include <ipldef.h>          // Hardware IPL definitions
#include <irpdef.h>          // I/O request packet
#include <jibdef.h>          // Job Information Block fields.
#include <pcbdef.h>          // Process Control Blk.
#include <ssdef.h>           // System status codes
#include <ucbdef.h>          // Unit control block
#include <vcbdef.h>          // Volume Control block.
#include <vecdef.h>          // Interrupt vector block
#include <dyndef.h>          // 
#include <orbdef.h>          // Object-Rights Block

#include <dptdef.h>
#include <fdtdef.h>
#include <bufiodef.h>

#include <system_data_cells.h>
#include <internals.h>

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/sched.h>

#include <descrip.h>
#include "../../ipacp/src/structure.h"
#include "../../central/include/netcommon.h"
#include "../../central/include/network.h"

#include <queue.h>
#include <com_routines.h>
#include <exe_routines.h>
#include <misc_routines.h>
#include <sch_routines.h>

#include <linux/slab.h>

int getbuf(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb *c, void ** newbuf, int req_size);
int net_r_common(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int val);
int net_close_abort_common(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int val);

// Let's add some fields to the UCB, hmmm?

struct IP_UCB
{
// . = UCB$K_LENGTH

    struct _ucb ucb;
    unsigned long ucb$l_addrmode;

    //$DEF  IP_UCB_K_LENGTH

};

//
// Local symbols
//

//
// Argument list (AP) offsets for device-dependent QIO parameters
//

#define P1   0              // First QIO parameter
#define P2   4              // Second QIO parameter
#define P3   8              // Third QIO parameter
#define P4   12             // Fourth QIO parameter
#define P5   16             // Fifth QIO parameter
#define P6   20             // Sixth QIO parameter

//
// Other constants
//

#define MAX_USIZE        8192   // Maximum size of user buffer
#define FH_NAME_SIZE         128    // max Foreign Host name string size(bytes).

#define ucb$l_cbid   ucb$l_devdepend
#define ucb$l_protocol   ucb$l_devdepnd2
#define ucb$l_addrmode   ucb$l_devdepnd3

#if 0

#define TCP_PROTOCOL         0  // Protocol code for TCP
#define UDP_PROTOCOL         1  // Protocol code for UDP
#define ICMP_PROTOCOL        2  // Protocol code for ICMP
#define IP_PROTOCOL      3  // Protocol code for IP

// Address specification fields (3 longwords) for connectionless protocols.
//
//  !===============================================================!
//  +              Source Host              +
//  !---------------------------------------------------------------!
//  +           Destination Host            +
//  !---------------------------------------------------------------!
//  +   Destination Port    |      Source Port      +
//  !===============================================================!

struct  IPADR
{

    unsigned int IPADR$Src_Host;    // Source host
    unsigned int IPADR$Dst_Host;    // Destination host
    unsigned int IPADR$Ext1;    // Header info longword #1
#define IPADR$UDP_SIZE  12
    unsigned int IPADR$Ext2;    // Header info longword #2
#define IPADR$SIZE  16

};
#endif

// System buffer header fields (3 longwords) Supported by VMS
//
//  !===============================================================!
//  +   Within this blk, start Address of user data     +
//  !---------------------------------------------------------------!
//  +   User's Buffer Address (process address)         +
//  !---------------------------------------------------------------!
//  +   VMS Dynamic data structure header fields        +
//  !---------------------------------------------------------------!
//  +       |       |       | 1st Data Byte +
//  !===============================================================!

#if 0
struct  SB
{

    unsigned long sb$l_data_adrs;       // within this blk, data start adrs
    unsigned long sb$l_users_buf_adrs;  // user's buffer address
    unsigned short int sb$w_type;       // VMS Block type field
    unsigned short int sb$w_size;       // VMS Block size field
    unsigned char sb$b_data;        // start of actual data

};

// UCB device dependant words for IP device
// The UCB has one quadword builtin for device-dependant use. We will use the
// first longword of this to store the connection control block address and the
// second to store the protocol type (TCP, UDP, ICMP, or IP).

// ACP argument block fields.  This definition also includes the fields
// definied in the SB block which must match exactly as they are used by
// VMS to communicate between the user and the device driver.  The ACP
// reads these fields as arguments to a user network I/O request.  See
// NET$OPEN routine for graphic desc.

struct  AB              // Basic argument block
{

    unsigned long AB$L_Data_Adrs;       // WITHIN THIS BLOCK, START OF Data
    unsigned long AB$L_Users_Buf_Adrs;  // user's buffer start address
    unsigned long AB$L_Blk_ID;      // VMS dynamic block ID fields
    unsigned long AB$L_IRP_Adrs;        // Associated IRP address
    unsigned long AB$L_UCB_Adrs;        // Unit Control Blk
    unsigned long AB$L_PID;     // User's PID
    unsigned short int AB$W_UARGSIZE;       // Size of the argument block
    unsigned char AB$B_Funct;       // ACP function code
    unsigned char AB$B_Protocol;        // ACP protocol code
    unsigned char AB$B_Data[0];     // start of User Arguments
    // should be []?            .BLKB   0
    //  AB$COM_END = .          // End of common fields

};


// Define additional fields for OPEN block

struct  OP
{

// . = AB$COM_END

    struct AB OP$AB;

#define OPF$MODE     1          // Mask for mode bit
#define OPF$NOWAIT   2          // Mask for no-wait bit
#define OPF$ADDRFLAG     4          // Mask for address flag bit

// Note about OP$FLAGS:
// OP$MODE also is used in UDP.  A 0 means address headers are in the data buff.

    unsigned int OP$Src_Host;   // Source host
    unsigned int OP$Dst_Host;   // Destination host
    unsigned int OP$Ext1    ;   // Header info longword #1
    unsigned int OP$Ext2    ;   // Header info longword #2

    unsigned short int OP$Flags;    // Open flags
#define OP$M_MODE 1
#define OP$M_NOWAIT 2
#define OP$M_ADDRFLAG 4
#if 0
    _VIELD  OP,0,<-
    <Mode,1,M>,-        // Open mode (1=active,0=passive)
    <NoWait,1,M>,-      // No-wait mode
    <AddrFlag,1,M>,-    // Argument is IP address, not name
    >
#endif
    unsigned short int OP$TimeOut;  // Open timeout
    unsigned short int OP$PIOCHAN;  // Process I/O channel
    unsigned short int OP$Foreign_Hlen; // Length of foreign host name
    unsigned char OP$Foreign_Host[FH_NAME_SIZE];
#define OP$BLK_SIZE sizeof(struct OP)   // Size of open block

};

#define OP$ALL_SIZE  OP$BLK_SIZE        // Set size to allocate
// .IF GREATER <IRP$C_LENGTH-OP$BLK_SIZE>
//#define     OP$ALL_SIZE  IRP$C_Length     // Round up small blocks...
// .ENDC

// Define additional fields for CLOSE/ABORT

struct  CL
{
// . = AB$COM_END
    struct AB CL$AB;

    unsigned short int CL$FLAGS;    // Close flags
    unsigned short int CL$TIMEOUT;  // Close timeout
    unsigned long CL$LCID   ;   // Connection ID
#define CL$BLK_SIZE sizeof(struct CL)   // Size of close block

};
//~~~ This should be MAX(CL$BLK_SIZE,IRP$C_LENGTH)

// Define additional fields for SEND

struct  SE
{

// . = AB$COM_END               // Append to common block
    struct AB SE$AB;

#endif
#define SEF$EOL_BIT  0              // Mask for EOL bit
#define SEF$URGENT_BIT  0           // Mask for Urgent bit
#if 0

    unsigned int SE$Src_Host;   // Source host
    unsigned int SE$Dst_Host;   // Destination host
    unsigned int SE$Ext1    ;   // Header info longword #1
    unsigned int SE$Ext2    ;   // Header info longword #2

    unsigned short int SE$Flags;    // Flags
#define SE_M_XMODE 1
#define SE_M_EOL 2
#define SE_M_URG 4
#if 0
    _VIELD  SE,0,<-
    <XMODE,1,M>,-       // Obsolete
    <EOL,1,M>,-     // End Of Letter
    <URG,1,M>-      // Urgent data present
    >
#endif
    unsigned short int SE$TIMEOUT;  // Send timeout
    unsigned long SE$LCID   ;   // Connection-ID
    unsigned short int SE$BUFSIZE;  // User buffer size
    unsigned char SE$DATA   [MAX_USIZE]; // Start of user data in block
#define SE$MAX_SIZE sizeof(struct SE)   // Max send block size
#define  SE$BLK_SIZE (SE$MAX_SIZE-MAX_USIZE)    // SEND block size

};


// Define additional fields for RECEIVE
// N.B. INFO and STATUS also use this size block.

struct  RE
{

// . = AB$COM_END               // Append to common block
    struct AB RE$AB;

    unsigned int RE$SRC_HOST;   // Source host
    unsigned int RE$DST_HOST;   // Destination host
    unsigned int RE$EXT1    ;   // Header info longword #1
    unsigned int RE$EXT2    ;   // Header info longword #2

    unsigned short int RE$FLAGS;    // Flags
    unsigned short int RE$TIMEOUT;  // Receive timeout
    unsigned long RE$LCID   ;   // Connection-ID
    unsigned long RE$Alt_IO;    // Alt_IO entry point
    unsigned long RE$PH_Buff;   // Proto Header buffer Pntr
    unsigned short int RE$BUFSIZE;  // User buffer size
    unsigned char RE$Data[MAX_USIZE]; // Start of user data in block
#define RE$MAX_SIZE sizeof(struct RE)   // Max READ block
#define RE$BLK_SIZE (RE$MAX_SIZE-MAX_USIZE) // RECEIVE block size

};


// Define additional fields for STATUS and INFO

struct  ST
{

// . = AB$COM_END               // Append to common block
    struct AB ST$AB;

    unsigned short int ST$FLAGS;    // Flags
    unsigned short int ST$TIMEOUT;  // Timeout
    unsigned long ST$LCID   ;   // Connection-ID
    unsigned short int ST$BUFSIZE;  // User buffer size
    unsigned char ST$DATA[MAX_USIZE]; // Start of user data in block
#define ST$MAX_SIZE sizeof(struct ST)   // Max READ block
#define ST$BLK_SIZE (ST$MAX_SIZE-MAX_USIZE) // info/status block size

};

#endif
// Define additional fields for GTHST

struct  GH
{

// . = AB$COM_END               // Append to common block
//  struct AB GH$AB;
    Static_Fields(gh);

    unsigned short int gh$flags;    // Flags (currently unused)
    unsigned short int gh$ghfunct;  // GTHST function code
    unsigned short int gh$bufsize;  // User's buffer size
    unsigned short int gh$arg1  ;   // First argument to GTHST
    unsigned short int gh$arg2size; // Length of second argument
    unsigned char gh$arg2   [FH_NAME_SIZE]; // Second argument to GTHST
    unsigned char gh$udata[MAX_USIZE]; // Area to return data to user
#define GH$MAX_SIZE sizeof(struct GH)   // Max size of GTHST block
#define GH$BLK_SIZE (&((struct GH *)0)->gh$udata)   // Size of basic GTHST block

};

#if 0
// Additional fields for debugging functions
#endif

struct DB
{

// . = AB$COM_END               // Append to common block
//  struct AB DB$AB;
    Static_Fields(db);

    unsigned long db$level; // Debugging level
    unsigned long db$lcid   ;   // Connection ID
    unsigned long db$group; // Which logging flags to modify
#define DB$BLK_SIZE sizeof(struct DB)   // Max DEBUG block size

};

#if 0
//Additional fields for Dump function

struct DU
{

// . = AB$COM_END               // Append to common block
    struct AB DU$AB;

    unsigned short int DU$FUNCT;    // Dump subfunction
    unsigned short int DU$TIMEOUT;  // Timeout (obsolete, unused)
    unsigned long DU$ARG0   ;   // Argument #0
    unsigned long DU$ARG1   ;   // Argument #1
    unsigned long DU$ARG2   ;   // Argument #2
    unsigned short int DU$BUFSIZE;  // User buffer size
    unsigned char DU$DATA   [MAX_USIZE]; // User data area
#define DU$MAX_SIZE sizeof(struct DU)   // Maximum buffer size
};


// Define additional fields for EVENT

struct  EV
{

// . = AB$COM_END               // Append to common block
    struct AB EV$AB;

    unsigned short int EV$BUFSIZE;  // User buffer size
    unsigned char EV$DATA   [MAX_USIZE]; // Start of user data in block
#define EV$MAX_SIZE sizeof(struct EV)   // Max send block size
#define EV$BLK_SIZE (EV$MAX_SIZE-MAX_USIZE) // EVENT block size

};
#endif

//Additional fields for NCP function

struct NCP
{

// . = AB$COM_END               // Append to common block
//  struct AB NCP$AB;
    Static_Fields(ncp);

    unsigned short int ncp$ncpfunct;    // NCP function
    unsigned short int ncp$misc;    // NCP subfunction
    unsigned short int ncp$wbufsize;    // User buffer size
    unsigned short int ncp$rbufsize;    // User buffer size
    unsigned char ncp$data[MAX_USIZE]; // User data area
#define NCP$MAX_SIZE sizeof(struct NCP) // Maximum buffer size
#define NCP$BLK_SIZE (&((struct NCP *)0)->ncp$data) // Basic buffer size

};

// User QIO function codes, func parameter of $qio

#if 0
#define IP$OPEN      IO$_CREATE
#define IP$SEND      IO$_WRITELBLK
#define IP$RECEIVE   IO$_READLBLK
#define IP$CLOSE     IO$_DELETE
#define IP$STATUS    IO$_ACPCONTROL
#define IP$ABORT     IO$_DEACCESS
#define IP$INFO      IO$_MODIFY
#define IP$DUMP      IO$_UNLOAD
#define IP$EXIT      IO$_RELEASE
#define IP$DEBUG     IO$_DIAGNOSE

// GTHST (Get host information) $QIO function code

#define IP$GTHST     IO$_SKIPFILE

// EVENT (Send activity data) $QIO function code

#define IP$EVENT     IO$_WRITECHECK

#define IP$NCP       IO$_RDSTATS
#endif

// IP Argument block function codes
// the following user function codes must form a dense list as they are
// used to generate a case table.

#define U$OPEN       1
#define U$SEND       2
#define U$RECV       3
#define U$CLOSE      4
#define U$STATUS     5
#define U$ABORT      6
#define U$INFO       7

#define U$GTHST      8
#define     U$GH_NAMADR  1      // Domain Name to A-record function
#define     U$GH_ADRNAM  2      // Address to name function
#define     U$GH_RRLOOK  3      // Domain Name to RR function

// Privileged maintenance calls.

#define M$DUMP       9
#define M$EXIT       10
#define M$DEBUG      11
#define M$EVENT      12
#define M$NCP        13
#define M$CANCEL     14

#define du$known_host_index  6      // dump sub-function code.

int ip4$unit_init (struct _idb * idb, struct _ucb * ucb) ;
int ip4$init_tables() ;
int ip_unload(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int unit_init(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int net_open(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int queue_2_acp(struct _irp * i, struct _pcb * p, struct _ucb * u) ;
int net_abort(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int Net_Send(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int send_common(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, struct user_abort_args * buf) ;
int ip_send(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int net_receive(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;               // Receive data from network
int net_status(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;                // Request connection status
int net_dump(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int net_ncp(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int net_gthst(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int net_exit(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int net_debug(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int net_event(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int getdiag(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, void ** newbuf, int req_size) ;
int ip_finishio(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;   // Start processing something the IPACP has posted to us.
int ip_cancel(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int chan) ;               // Cancel an I/O operation
int netacp_alive(struct _ucb * u) ;
int net_close(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;
int net_info(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) ;

static struct _fdt ip4$fdt
#if 0
        =
{
fdt$q_valid:
    IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
fdt$q_buffered:
    IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
}
#endif
;

struct _ddt ip4$ddt=
{
ddt$l_fdt:
    &ip4$fdt
};

#undef ini_fdt_act
extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void ip4$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb)
{
    ucb->ucb$b_flck=IPL$_IOLOCK8;
    ucb->ucb$b_dipl=IPL$_IOLOCK8;

    ucb->ucb$l_devchar = DEV$M_NET | DEV$M_IDV | DEV$M_ODV | DEV$M_MBX;

    ucb->ucb$l_devchar2 = DEV$M_NNM;
    ucb->ucb$b_devclass = DC$_REALTIME;
    ucb->ucb$b_devtype = DT$_TTYUNKN;
    ucb->ucb$w_devbufsiz = 1024;

    ucb->ucb$l_sts = UCB$M_TEMPLATE;
    ucb->ucb$l_devdepend = 99; // just something to fill

    ucb->ucb$w_mb_seed = 0;

    ucb->ucb$l_refc = 0;

    ucb->ucb$l_cbid = 0;

    ucb->ucb$l_protocol = 0xff;

    ddb->ddb$l_acpd = "IP     ";

    // no orb yet

    // dropped the mutex stuff

    return;
}

void ip4$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb)
{
    ddb->ddb$ps_ddt=&ip4$ddt;
    //dpt_store_isr(crb,nl_isr);
    return;
}

int ip4$unit_init (struct _idb * idb, struct _ucb * ucb)
{
    ucb->ucb$v_online = 0;
    //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

    // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
    // no adp or cram stuff

    // or ints etc

    ucb->ucb$v_online = 1;

    return SS$_NORMAL;
}

struct _dpt ip4$dpt;
struct _ddb ip4$ddb;
struct _ucb ip4$ucb;
struct _crb ip4$crb;

int ip4$init_tables()
{
    ini_dpt_name(&ip4$dpt, "IPDRIVER");
    ini_dpt_adapt(&ip4$dpt, 0);
    ini_dpt_defunits(&ip4$dpt, 1);
    ini_dpt_flags(&ip4$dpt, DPT$M_SVP);
    ini_dpt_ucbsize(&ip4$dpt,sizeof(struct _ucb)); // check this
    ini_dpt_struc_init(&ip4$dpt, ip4$struc_init);
    ini_dpt_struc_reinit(&ip4$dpt, ip4$struc_reinit);
    ini_dpt_ucb_crams(&ip4$dpt, 1/*NUMBER_CRAMS*/);
    ini_dpt_unload(&ip4$dpt, ip_unload);
    ini_dpt_end(&ip4$dpt);

    ini_ddt_unitinit(&ip4$ddt, unit_init);
    // not? ini_ddt_start(&ip4$ddt, startio);
    ini_ddt_cancel(&ip4$ddt, ip_cancel);
    ini_ddt_end(&ip4$ddt);

    /* for the fdt init part */
    /* a lot of these? */

    ini_fdt_act(&ip4$fdt,IO$_READLBLK,net_receive,1);
    ini_fdt_act(&ip4$fdt,IO$_READVBLK,net_receive,1); // not abortio?
    ini_fdt_act(&ip4$fdt,IO$_READPBLK,exe_std$abortio,1);
    ini_fdt_act(&ip4$fdt,IO$_WRITELBLK,Net_Send,1);
    ini_fdt_act(&ip4$fdt,IO$_WRITEVBLK,Net_Send,1); // not abortio?
    ini_fdt_act(&ip4$fdt,IO$_WRITEPBLK,exe_std$abortio,1);
    ini_fdt_act(&ip4$fdt,IO$_CREATE,net_open,1);
    ini_fdt_act(&ip4$fdt,IO$_DELETE,net_close,1);
    ini_fdt_act(&ip4$fdt,IO$_ACPCONTROL,net_status,1);
    ini_fdt_act(&ip4$fdt,IO$_MODIFY,net_info,1);
    ini_fdt_act(&ip4$fdt,IO$_DEACCESS,net_abort,1);
    ini_fdt_act(&ip4$fdt,IO$_UNLOAD,net_dump,1);
    ini_fdt_act(&ip4$fdt,IO$_RELEASE,net_exit,1);
    ini_fdt_act(&ip4$fdt,IO$_DIAGNOSE,net_debug,1);
    ini_fdt_act(&ip4$fdt,IO$_SKIPFILE,net_gthst,1);
    ini_fdt_act(&ip4$fdt,IO$_WRITECHECK,net_event,1);
    ini_fdt_act(&ip4$fdt,IO$_RDSTATS,net_ncp,1);
#if 0
    ini_fdt_act(&ip4$fdt,IO$_ACCESS,acp_std$access,1);
    ini_fdt_act(&ip4$fdt,IO$_READLBLK,acp_std$readblk,1);
    ini_fdt_act(&ip4$fdt,IO$_READPBLK,acp_std$readblk,1);
    ini_fdt_act(&ip4$fdt,IO$_READVBLK,acp_std$readblk,1);
    ini_fdt_act(&ip4$fdt,IO$_WRITELBLK,acp_std$writeblk,1);
    ini_fdt_act(&ip4$fdt,IO$_WRITEPBLK,acp_std$writeblk,1);
    ini_fdt_act(&ip4$fdt,IO$_WRITEVBLK,acp_std$writeblk,1);
    ini_fdt_act(&ip4$fdt,IO$_CREATE,acp_std$access,1);
    ini_fdt_act(&ip4$fdt,IO$_DEACCESS,acp_std$deaccess,1);
    ini_fdt_act(&ip4$fdt,IO$_DELETE,acp_std$modify,1);
    ini_fdt_act(&ip4$fdt,IO$_MODIFY,acp_std$modify,1);
    ini_fdt_act(&ip4$fdt,IO$_ACPCONTROL,acp_std$modify,1);
    ini_fdt_act(&ip4$fdt,IO$_MOUNT,acp_std$mount,1);
#endif
    ini_fdt_end(&ip4$fdt);

    return SS$_NORMAL;
}

long ip4_iodb_vmsinit(void)
{
#if 0
    struct _ucb * ucb=&ip4$ucb;
    struct _ddb * ddb=&ip4$ddb;
    struct _crb * crb=&ip4$crb;
#endif
    struct _ucb * ucb=kmalloc(sizeof(struct _ucb),GFP_KERNEL); //check
    struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
    struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
    unsigned long idb=0,orb=0;

    memset(ucb,0,sizeof(struct _ucb)); // check
    memset(ddb,0,sizeof(struct _ddb));
    memset(crb,0,sizeof(struct _crb));

#if 0
    init_ddb(&ip4$ddb,&ip4$ddt,&ip4$ucb,"dqa");
    init_ucb(&ip4$ucb, &ip4$ddb, &ip4$ddt, &ip4$crb);
    init_crb(&ip4$crb);
#endif

    init_ddb(ddb,&ip4$ddt,ucb,"ipa");
    init_ucb(ucb, ddb, &ip4$ddt, crb);
    init_crb(crb);

    ucb -> ucb$w_size = sizeof(struct _ucb); // temp placed // check

    ucb -> ucb$w_unit_seed = 1; // was: 0 // check // temp placed
    ucb -> ucb$w_unit = 0; // temp placed

#if 0
    ucb -> ucb$l_sts |= UCB$M_TEMPLATE; // temp placed
#endif

//  ioc_std$clone_ucb(&ip4$ucb,&ucb);
    ip4$init_tables();
    ip4$struc_init (crb, ddb, idb, orb, ucb);
    ip4$struc_reinit (crb, ddb, idb, orb, ucb);
    ip4$unit_init (idb, ucb);

    insertdevlist(ddb);

    return ddb;

}

long ip4_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc)
{
    unsigned short int chan;
    struct _ucb * newucb = 0;
    //  ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&ip4$ucb*/,&newucb); // check. skip?
#if 0
    exe$assign(dsc,&chan,0,0,0);
    registerdevchan(MKDEV(TTYAUX_MAJOR,unitno),chan);
#endif

    return newucb;
}

int ip4_vmsinit(void)
{
    //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

    unsigned short chan0, chan1, chan2;
    $DESCRIPTOR(dsc,"ipa0");
    unsigned long idb=0,orb=0;
    struct _ccb * ccb;
    struct _ucb * newucb0,*newucb1,*newucb2;
    struct _ddb * ddb;

    printk(KERN_INFO "dev con here pre\n");

    ddb=ip4_iodb_vmsinit();

    /* for the fdt init part */
    /* a lot of these? */

    ip4_iodbunit_vmsinit(ddb,1,&dsc);

    printk(KERN_INFO "dev ip4 here\n");

    // return chan0;

}

//++
// IP_UNLOAD, Unload the driver
//
// Functional description:
//
//  Usefull for dealocating system buffers with COM$DRVDEALMEM.
//
//  N.B.  See the VMS Device Support manual section on the
//  Driver Unloading Routine before modifying this routine.
//
// Context:
//
//  SYSGEN calls the driver unloading routine at IPL$_POWER.
//  The driver unloading routine cannot lower IPL.
//  The driver unloading routine is called in process context.
//  The driver unloading routine can use all registers.
//
// Inputs:
//
//  R6  - address of the DDB (device data block)
//  R10 - address of the DPT (driver prologue table)
//
// Outputs:
//
//  The driver unloading routine can use all registers.
//
//--

int ip_unload(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    // Unload device
    return SS$_NORMAL;
    // Return
}


//  .sbttl  Unit Initialization rtn.

// Bogus unit initialization rtn that sysgen can call even though this is
// a virtual device.  If this rtn is left out sysgen crashes the system on
// a connect cmd.

//
// Functional description:
//
//  The operating system calls this routine after calling the
//  controller initialization routine:
//
//      at system startup
//      during driver loading
//      during recovery from a power failure
//
// Inputs:
//
//  R4  - address of the CSR (controller status register)
//  R5  - address of the UCB (unit control block)
//
// Outputs:
//
//  The routine must preserve all registers except R0-R3.
//
//--

int unit_init(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
//  JSB G^INI$BRK
    return SS$_NORMAL;
}

//  .SBTTL  FDT Routine: "OPEN" a Network Connection.

//++
// OPEN a network connection, QIO func=IP$OPEN
//
// Functional description:
//
//  The user has requested to "OPEN" a network connection.
//  Here we verify the QIO parameters & build an argument block.
//  Finally the IRP (I/O Request Packet) & associated System buffer
//  ( argument block) are queued to the IP ACP.
//
//  OPEN argument structure:
//!!!HACK!!! I think all of the structure definitions in this module are wrong!
//
//  =============================================================
//  +           Data Start - Not used           +
//  !-----------------------------------------------------------!
//  +       User's Buffer Address - Not used.       +
//  !-----------------------------------------------------------!
//  +           VMS Block ID longword.          +
//  !-----------------------------------------------------------!
//  +   IRP Address - Used by ACP for IO postprocessing     +
//  !-----------------------------------------------------------!
//  +   UCB Address - Used by ACP for IO postprocessing     +
//  !-----------------------------------------------------------!
//  +               PID                 +
//  !-----------------------------------------------------------!
//  +   Time Out(secs)       !       Flags  !  Function +
//  !-----------------------------------------------------------!
//  +           Local Port              +
//  !-----------------------------------------------------------!
//  +           Foreign                 +
//  +           Host                    +
//  +           ASCIZ                   +
//  +           Name                    +
//  +       20 chars max (includes null)            +
//  !-----------------------------------------------------------!
//  +           Foreign Port                +
//  !-----------------------------------------------------------!
//  +   Process IO Channel   !   Buffer Size in Bytes       +
//  =============================================================
//
//
// Inputs:
//
//  R0-R2   - scratch registers
//  R3  - address of the IRP (I/O request packet)
//  R4  - address of the PCB (process control block)
//  R5  - address of the UCB (unit control block)
//  R6  - address of the CCB (channel control block)
//  R7  - bit number of the I/O function code
//  R8  - address of the FDT table entry for this routine
//  R9-R11  - scratch registers
//  AP  - address of the 1st function dependent QIO parameter
//
// QIO P1 - P6 Parameters
//
//  P1 = Address of Foreign Host asciz name or 0 for wild-card Host.
//  P2 = Foreign Port integer or 0 for wild-card port.
//  P3 = Local Port integer or 0 to let ACP assign a local port integer.
//  P4 = Open flags, bit 0 is mode
//  P5 = Protocol code, 0 = TCP, 1 = UDP, 2 = ICMP, 3 = IP
//  P6 = Connection time-out in seconds.
//
// Outputs:
//
//  IRP & associated system buffer (IPACP argument block) are queued to
//  the IP ACP.
//
//  The routine must preserve all registers except R0-R2, and
//  R9-R11.
//
// User Error Returns:
//
//  SS$_MEDOFL  = Media is off-line, ACP has died.
//  SS$_ACCVIO  = Unable to access HOST name string.
//  SS$_BADPARAM    = Host name string + null byte > 20 characters.
//  SS$_EXQUOTA = unable to allocate ACP argument blk.
//  SS$_INSFMEM = Same as SS$_EXQUOTA.
//
//--


int net_open(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    struct user_open_args * buf;
    int sts = netacp_alive(u);  // network ACP still running?
    if (sts!=SS$_NORMAL)
        return sts;
    //; jsb G^INI$BRK
    u->ucb$l_addrmode = 0;  // Now we'll know during NET_SEND...

    if (i->irp$l_qio_p5==U$UDP_PROTOCOL // Doing UDP?
            && // Nope, go on.
            // Temp
            (i->irp$l_qio_p4&OPF$Mode)==0)  //
        // No, then all is well.

        // Ok, the user is passing the IP addresses in the data buffer.
        // We are going to make a note of this in the UCB.
        u->ucb$l_addrmode = 1;  // Now we'll know during NET_SEND...

    // If the user is supplying an IP address, figure the length

    if (i->irp$l_qio_p4&OPF$Addrflag)   // Is he specifying an IP address?
    {
        // No - check string length
#if 0
        movzbl  S^#4,R9         // Length of string
        movl    R9,R1           // Size of buffer
        movl    P1(AP),R0       // Buffer address
#endif
        sts = exe_std$writechk (i, p, u, i->irp$l_qio_p1, 4/*i->irp$l_qio_p2*/);        // Check it
        goto    OP$Buf;         // And done

        // Determine length of Foreign Host name for copy operation.
        // Did user specify a wild-card host (eg,0)?

    }
#if 0
    // check
    clrl    R9          // assume a wild-card size.
#endif
    if (i->irp$l_qio_p1==0)         // see if wildcard specified
        goto    OP$Buf;         // wild-card, continue.

    // Check access to specified Foreign Host name.  Since we don't know the
    // actual length we must assume 1 char for access check.

#if 0
    movl    P1(AP),R0       // get address of String.
    movzbl  S^#1,R1         // set size
#endif
    sts = exe_std$writechk (i, p, u, i->irp$l_qio_p1, 1/*i->irp$l_qio_p2*/);        // if error we don't return here.

    // Verify length to be < 128 chars. 127 chars of name + null termination byte.

    if (strlen(i->irp$l_qio_p1)>=FH_NAME_SIZE)

        // String too long, return error

        return exe_std$abortio(i, p, u, SS$_BADPARAM);

    // Found the null char, r0 = chars left in string. Compute actual size + null

#if 0
    subl3   R0,#FH_NAME_SIZE,R9
    incl    R9          // string size is in R9
#endif

    // Allocate a system buffer for the ACP argument block

OP$Buf:
    // default size for buffer.
    sts = getbuf(i,p,u,c,&buf,OP$ALL_SIZE);     // allocate & check/account quotas

    // Fill in OPEN args.
    // R1 = size in bytes
    // R2 = address of system buffer.
    // R9 = Size of foreign host string
    //
    // copy Foreign Host name to ACP arg blk.

#if 0
    pushr   #^M<R1,R2,R3,R4,R5>
    movl    R2,R10      // adrs of system buffer, so movc will be happy.
#endif

    // Initialize area first

#if 0
    movc5   #0,OP$Foreign_Host(R10),#0,#FH_NAME_SIZE,OP$Foreign_Host(R10)
#endif
    memset(&buf->op$foreign_host,0,FH_NAME_SIZE);
    if (i->irp$l_qio_p1 && strlen(i->irp$l_qio_p1)) // Foreign Host name Wild?
        // 0 = yes, do foreign port
        memcpy(&buf->op$foreign_host,i->irp$l_qio_p1,strlen(i->irp$l_qio_p1));

    // Here after copy or if wild

    buf->op$foreign_hlen=0;
    if (i->irp$l_qio_p1)
        buf->op$foreign_hlen=strlen(i->irp$l_qio_p1);   // Set foreign host name length

    // Set foreign Port
    // Local port

    buf->op$ext1=i->irp$l_qio_p3 + (i->irp$l_qio_p2<<16); // check.

    // Open flags (active/passive)

    buf->op$flags=i->irp$l_qio_p4;      // O or 1 value

    // Time Out in seconds

    buf->op$timeout=i->irp$l_qio_p6;

    // Set "OPEN" function code and protocol code

    struct user_abort_args * ab_buf = buf;

    ab_buf->ab$funct=U$OPEN;    // Set function code
    // Get protocol code
    u->ucb$l_protocol=i->irp$l_qio_p5; // Set UCB protocol code
    ab_buf->ab$protocol=i->irp$l_qio_p5;    // Set arg blk protocol code
    ab_buf->ab$uargsize=OP$BLK_SIZE; // Set argument block size

    // Rearrange parameters for a raw IP connection

    if (i->irp$l_qio_p5==U$IP_PROTOCOL)     // Is this an IP connection?
    {
        // Nope, go on.
        buf->op$src_host=i->irp$l_qio_p2;   // Address filter
        buf->op$ext1=i->irp$l_qio_p3;   // Protocol Filter

        // Include Process IO channel which reflects this network connection.

    }
    buf->op$piochan=i->irp$w_chan;

    // Clear ACP window blk pointer.

    c->ccb$l_wind=0;

    // Queue IRP & ACP arg blk to network acp "IPACP"
    // R5 = ucb adrs

    return queue_2_acp(i,p,u);
}

int queue_2_acp(struct _irp * i, struct _pcb * p, struct _ucb * u)
{
    struct _vcb * v = u->ucb$l_vcb; // get address of VCB

#if 0
    // .IF DEFINED VMS_V4
    SetIPL  #IPL$_SYNCH     // synchronize with VMS
    // .ENDC
#endif
    v->vcb$l_trans++;       // include this IRP
    //  movl    #42,IRP$L_EXTEND(R3) // Mark this...
    return  exe$qioacppkt(i,p,u);       // to the ACP......
}

//  .SBTTL  FDT Routine: "CLOSE/ABORT" a Network Connection.

//++
// Close a network connection, QIO func=IP$Close.
// Abort a connection, QIO func=IP$Abort.
//
// Functional description:
//
//  User has requested a network connection be closed.  A ACP argument
//  block is generated.
//
//  CLOSE/Abort argument block structure:
//
//  =============================================================
//  +       Data Start - Not used.              +
//  !-----------------------------------------------------------!
//  +       Users Buffer Address - Not used.        +
//  !-----------------------------------------------------------!
//  +       VMS Block ID longword.              +
//  !-----------------------------------------------------------!
//  +   IRP Address - Used by ACP for IO postprocessing     +
//  !-----------------------------------------------------------!
//  +   UCB Address - Used by ACP for IO postprocessing     +
//  !-----------------------------------------------------------!
//  +           PID                 +
//  !-----------------------------------------------------------!
//  +   Not Used.        !       Flags  !  Function +
//  !-----------------------------------------------------------!
//  +           Local Connection ID         +
//  =============================================================
//
// Inputs:
//
//  R0-R2   - scratch registers
//  R3  - address of the IRP (I/O request packet)
//  R4  - address of the PCB (process control block)
//  R5  - address of the UCB (unit control block)
//  R6  - address of the CCB (channel control block)
//  R7  - bit number of the I/O function code
//  R8  - address of the FDT table entry for this routine
//  R9-R11  - scratch registers
//  AP  - address of the 1st function dependent QIO parameter
//
// QIO Parameters:
//
//  P1 = Close flags
//
// Outputs:
//
//  IRP & associated system buffer(ACP argument block) are queue to
//  the IP ACP.
//
//  The routine must preserve all registers except R0-R2, and
//  R9-R11.
//
// User Error Returns:
//
//  SS$_MEDOFL  = Media off-line, ACP is not running.
//  SS$_BADPARAM    = Invalid Local Connection ID (<= 0).
//  SS$_EXQUOTA = Unable to allocate ACP argument block.
//  SS$_INSFMEM = same as SS$_EXQUOTA.
//
//--

int net_close(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
#if 0
    movw    #U$Close,R10        // set function for common code.
#endif
    //  cmpb    UCB$L_Protocol(R5),#UDP_Protocol
    //  bneq    5$
    //  movw    #UDP$Close,R10
    //5$:

    return net_close_abort_common(i,p,u,c,U$CLOSE);
}

// Do common code/set-up for NET_Close & NET_Abort functions

int net_close_abort_common(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int val)
{

#if 0
DO_AB:
#endif
    struct user_abort_args * buf;
    int sts = netacp_alive(u);      // network ACP still running?
    if (sts!=SS$_NORMAL)
        return sts;

    // Allocate a system buffer(ACP arg blk)

    sts = getbuf(i,p,u,c,&buf,CL$ALL_SIZE);
    struct user_close_args * cl_buf=buf;

    // Setup arguments - R2 points at ACP arg blk.

    cl_buf->cl$local_conn_id=u->ucb$l_cbid;// Copy connection-ID
    cl_buf->cl$flags=i->irp$l_qio_p1;   // Copy flags
    buf->ab$funct=val;  // set ACP function code
    buf->ab$protocol=u->ucb$l_protocol;// Set arg blk protocol code
    buf->ab$uargsize=CL$BLK_SIZE; // Set argblk length
    return queue_2_acp(i,p,u);      // of to the ACP
}

// "ABORT" function.  Set-up is the same as CLOSE, only difference is
// ACP Function code.

int net_abort(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
#if 0
    movw    #U$ABORT,R10        // set for common code
#endif
    //  cmpb    UCB$L_Protocol(R5),#UDP_Protocol // Doing UDP?
    //  bneq    5$          // No.
    //  movw    #UDP$ABORT,R10      // Yes - set UDP code
    //5$:
#if 0
    BRB DO_AB           // merge into common code sequence.
#endif
    return net_close_abort_common(i,p,u,c,U$ABORT);
}

// Handle the Bad Local connection ID Error.

#if 0
Bad_LC_ID:
return exe_std$abortio(i, p, u, SS$_BADPARAM);      // return error to user.
#endif

// .SBTTL   FDT Routines: "ACP Functions: SEND,RECEIVE,STATUS,INFO,DUMP & EVENT".

//++
// SEND data over the network, QIO Func=IP$SEND=IO$_WRITELBLK.
// RECEIVE data from the network, QIO Func=IP$RECEIVE=IO$_READLBLK
// Obtain a connections "STATUS", QIO Func=IP$STATUS=IO$_ACPCONTROL.
// Retrieve Connection Information, QIO Func=IP$INFO=IO$_MODIFY.
// DUMP a portion of IP, QIO Func=IP$DUMP=IO$_UNLOAD.
// Log a significant EVENT (activity), QIO Func=NET$EVENT=IO$_WRITECHECK.
//
// Functional description:
//
//  ACP argument processing & arg block format for (Send,receive,status,
//  Info,dump,event) are all very close so we can process them with one rtn
//  Basic idea is that the user has specified a Data buffer, size &
//  Local-Connection-ID for all four. Dump is a privileged (physical
//  IO) function.
//
//  ACP argument Block structure:
//
//  =============================================================
//  +   Data Start - Points at user data within this blk.    +
//  !-----------------------------------------------------------!
//  +       User's Buffer Address               +
//  !-----------------------------------------------------------!
//  +       VMS Block ID longword.              +
//  !-----------------------------------------------------------!
//  +   IRP Address - Used by ACP for IO postprocessing     +
//  !-----------------------------------------------------------!
//  +   UCB Address - Used by ACP for IO postprocessing     +
//  !-----------------------------------------------------------!
//  +           PID                 +
//  !-----------------------------------------------------------!
//  +   Time Out(secs)       !Flags or dump code!  Function +
//  !-----------------------------------------------------------!
//  +          Local Connection ID              +
//  !-----------------------------------------------------------!
//  +   Tag          !      Data Size       +
//  !-----------------------------------------------------------!
//  +   User's Data or Space reserved for data.         +
//  +   Size in bytes is found in field "Data Size"     +
//  =============================================================
//
//
// Inputs:
//
//  R0-R2   - scratch registers
//  R3  - address of the IRP (I/O request packet)
//  R4  - address of the PCB (process control block)
//  R5  - address of the UCB (unit control block)
//  R6  - address of the CCB (channel control block)
//  R7  - bit number of the I/O function code
//  R8  - address of the FDT table entry for this routine
//  R9-R11  - scratch registers
//  AP  - address of the 1st function dependent QIO parameter
//
// QIO Parameters:
//
//!!!HACK!!! this is wrong!
//
//  P1 = Buffer address
//  P2 = Size of buffer in bytes
//  P3 =
//      Send: EOL flag (Bit 0),  Urgent flag (Bit 1).
//      Recv: Address spec. block (for UDP, ICMP, and IP)
//          Dump: Dump function code.
//  P4 =
//      Send: Address spec. block (for UDP, ICMP, and IP)
//      Dump: Local connection id (adrs of CB to be dumped).
//         adrs of asciz hostname string (known host index dump fcn).
//         Known Network host table index, host stats function.
//
// Outputs:
//
//  IRP & associated system buffer(ACP argument block) are queued to
//  the IP ACP.
//
//  The routine must preserve all registers except R0-R2, and
//  R9-R11.
//
// User Error Returns:
//
// SS$_MEDOFL       = Network ACP not running.
// SS$_BADPARAM     = Invalid Local Connection ID (<= 0).
// SS$_EXQUOTA      = Unable to allocate ACP Arg block or Buffer TOO large.
// SS$_INSFMEM      = Unable to allocate ACP argument block.
// SS$_ACCVIO       = Buffer access violation.
// SS$_ILLCNTRLFUNC = Illegal ACP function code. (Internal Error).
//
//--



int Net_Send(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    struct user_send_args * buf;
    int sts = netacp_alive(u);      // network ACP still running?
    if (sts!=SS$_NORMAL)
        return sts;

    if (u->ucb$l_protocol==U$IP_PROTOCOL) // Doing IP?
        // Nope, go on.
        return  ip_send(i,p,u,c);       // Yep... Treat it special.

    if (u->ucb$l_protocol==U$UDP_PROTOCOL && // Doing UDP?
            // Nope, go on.
            u->ucb$l_addrmode)          // Addr in data buffer?
    {
        // nope
        i->irp$l_qio_p6=i->irp$l_qio_p1;    // P6 = addr buffer
        i->irp$l_qio_p1+=IPADR$UDP_ADDRESS_BLEN;    // bump up buff addr by 3 longs
        i->irp$l_qio_p2-=IPADR$UDP_ADDRESS_BLEN;    // bump dn buff size by 3 longs

        // Check for access to user specified IP addresses, if given.
    }
    if (i->irp$l_qio_p6)            // Was address specified?
        // If not, go on.  Otherwise...
        // get address
        // set size to 4 longs
        sts = exe_std$writechk (i, p, u, i->irp$l_qio_p1, 16/*i->irp$l_qio_p2*/);       // check access, set IRP$W_BCNT.

    // Check for access to user specified data buffer
    if (i->irp$l_qio_p2<=0)     // get the size
        // must be gtr 0

        // Return error on bad size or bad local connection ID.

        return exe_std$abortio(i, p, u, SS$_BADPARAM);

    // Did user request more than we want to give em?

    if (i->irp$l_qio_p2>MAX_USIZE)
        // OK, check buffer access.

        // Requested buffer TOO large, return error.

        return exe_std$abortio(i, p, u, SS$_INSFMEM);

    // Check the access.  Both system rtns (EXE$.....CHK) do NOT return if
    // an error occurs but pass control to QIO error handler.

    // get address
    sts = exe_std$writechk (i, p, u, i->irp$l_qio_p1, i->irp$l_qio_p2);     // check access, set IRP$W_BCNT.

    // Allocate ACP IRP buffer
    // If requested data buffer + ACP args <= IRP size, use IRP size.
    // This is to prevent dynamic non-paged pool fragmentation.
    // R1 = size (bytes) of system buffer needed.


    int len= i->irp$l_qio_p2+SE$BLK_SIZE;   // Fit in IRP size
    if (len<IRP$K_LENGTH)
        len = IRP$K_LENGTH;         // User larger of the two sizes.
    sts = getbuf(i,p,u,c,&buf,len);         // allocate/check quotas.

    // Set:
    // user's buffer address in ACP arg blk.
    // Data size
    // Local Connection ID or Dump fuction code.
    // I/O Request Tag
    // R2 = address of ACP arg block

#if 0
    // Move the data into the Arg Blk.
    pushr   #^M<R1,R2,R3,R4,R5>
    movc3   P2(AP),@P1(AP),SE$DATA(R2)
    popr    #^M<R1,R2,R3,R4,R5>
#endif
    memcpy(&buf->se$data,i->irp$l_qio_p1,i->irp$l_qio_p2);

    if (i->irp$l_qio_p6)                // Was address specified?
    {
        // If not, go on.  Otherwise...
        ipadr$address_block * ip = i->irp$l_qio_p6; // get address
        buf->se$src_host=ip->ipadr$src_host; // Set local host
        buf->se$dst_host=ip->ipadr$dst_host; // Set remote host
        buf->se$ext1=ip->ipadr$ext1;    // Set first header extension
        buf->se$ext2=ip->ipadr$ext2;    // Set second header extension
    }
    else
    {

        // No ADDRESS BLOCK given// clear out proto header fields to ddefault.
        buf->se$src_host=0;     // Clear local host
        buf->se$dst_host=0;     // Clear remote host
        buf->se$ext1=0;         // Clear first header extension
        buf->se$ext2=0;         // Clear second header extension
    }

    // Test / Set End Of Letter (EOL) & Urgent data (URG) Flags in ACP arg blk

    if (test_bit(SEF$EOL_BIT,&i->irp$l_qio_p4))     // Did e set the EOL bit?
        // No? - check urgent bit.
        buf->se$eol=1;      // Set it in the IRP flags

    if (test_bit(SEF$URGENT_BIT,&i->irp$l_qio_p5))      // Did e set the urgent bit?
        buf->se$urg=1;      // Set it in the IRP flags
    //check
    return send_common(i,p,u,c,buf);
}

int send_common(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, struct user_abort_args * buf)
{
    struct user_send_args * se_buf=buf;
    int sts;
    se_buf->se$local_conn_id=u->ucb$l_cbid; // Connection ID
    buf->ab$ubuf_adrs=i->irp$l_qio_p1;
    se_buf->se$buf_size=i->irp$l_qio_p2;    // amount of user data in bytes
    buf->ab$funct=U$SEND;       // Set function code
    buf->ab$uargsize=SE$BLK_SIZE;   // Set argblk length
    buf->ab$protocol=u->ucb$l_protocol; // Set arg blk protocol code


    // Copy data to system buffer
    // Send data starts in User argument block after the send arguments.

Send$Copy_data:
    //!!!HACK!!! Is any of this redundant?
    buf->ab$data_start=&se_buf->se$data;

    return queue_2_acp(i,p,u);
}

int ip_send(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    struct user_send_args * buf;
    int sts;
    // Check for access to user specified IP addresses, if given.

    //  JSB G^INI$BRK   // Execute BPT (BreakPoint) instruction)
    //  movl    P5(AP),R1

    i->irp$l_qio_p5=i->irp$l_qio_p5<<2;         // Convert longs to bytes
    //check

    if (i->irp$l_qio_p6 &&          // Was header specified?
            // If not, abort.  Otherwise...
            //!!!HACK!!! Should be #20 until above hack is fixed
            i->irp$l_qio_p5>=20)        // at least 5 longs?
    {
        // too small, you lose...

        sts = exe_std$writechk (i, p, u, i->irp$l_qio_p1, i->irp$l_qio_p2);     // check access, set IRP$W_BCNT.

        // Check for access to user specified data buffer

l5:
        if (i>i->irp$l_qio_p2<=0)       // get the size
            // must be gtr 0
            return  exe_std$abortio(i, p, u, SS$_BADPARAM); // check

        // Return error on bad size or bad local connection ID.

    }
    else
        return  exe_std$abortio(i, p, u, SS$_BADPARAM); // check

    // Did user request more than we want to give em?

    if (i->irp$l_qio_p2>MAX_USIZE)
        // OK, check buffer access.

        // Requested buffer TOO large, return error.

        return exe_std$abortio(i, p, u, SS$_INSFMEM);

    // Check the access.  Both system rtns (EXE$.....CHK) do NOT return if
    // an error occurs but pass control to QIO error handler.

    // get address
    sts = exe_std$writechk (i, p, u, i->irp$l_qio_p1, i->irp$l_qio_p2);     // check access, set IRP$W_BCNT.

    // Allocate ACP IRP buffer
    // If requested data buffer + ACP args <= IRP size, use IRP size.
    // This is to prevent dynamic non-paged pool fragmentation.
    // R1 = size (bytes) of system buffer needed.

    int len= i->irp$l_qio_p2+SE$BLK_SIZE;   // Fit in IRP size
    len+=i->irp$l_qio_p5;       // Add header size
    if (len<IRP$K_LENGTH)
        len = IRP$K_LENGTH;         // User larger of the two sizes.
    sts = getbuf(i,p,u,c,&buf,len);         // allocate/check quotas.

    // Set:
    // IP specific fields in ACP arg blk.
    // R2 = address of ACP arg block

    //!!!HACK!!! Why am I doing this?  The IPACP can get this data directly
    unsigned long * ip = i->irp$l_qio_p6;   // get address
    unsigned char * ch = ip;
    //check
    buf->se$src_host=ip[3];     // Set local host
    buf->se$dst_host=ip[4];     // Set remote host
    buf->se$ext1=ch[9];         // Set protocol
    buf->se$ext2=i->irp$l_qio_p5; // Set header size

    // Set EXACT and NOCHKSM Flags in ACP arg blk
    buf->se$flags=i->irp$l_qio_p4;

    // Move the data into the Arg Blk.
    memcpy(&buf->se$data,i->irp$l_qio_p6,i->irp$l_qio_p5);
    // Now copy the data...
    struct user_send_args * buf2=((unsigned long)buf+(unsigned long)i->irp$l_qio_p5);
    memcpy(&buf2->se$data,i->irp$l_qio_p1,i->irp$l_qio_p2);

    return  send_common(i,p,u,c,buf);           // return to main routine
}


int net_receive(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)                 // Receive data from network
{
    int sts;
    struct user_recv_args * buf;
    struct user_abort_args * ab_buf;
    struct _bufio * diag_buf;
    int size = 16;          // Size of Addr Block

    if (u->ucb$l_protocol==U$UDP_PROTOCOL)   // Doing UDP?
    {
        // Nope, go on.
        size = 12;              // UDP addr blk only 12 bytes
        if (u->ucb$l_addrmode)          // Addr in data buffer?
        {
            // nope
            i->irp$l_qio_p3=i->irp$l_qio_p1;    // P3 = addr buffer
            i->irp$l_qio_p1+=IPADR$UDP_ADDRESS_BLEN;    // bump up buff addr by 3 longs
            i->irp$l_qio_p2-=IPADR$UDP_ADDRESS_BLEN;        // bump dn buff size by 3 longs
        }
    }

    // Check for access to user specified IP addresses, if given.
    if (i->irp$l_qio_p3)            // Was address specified?
        // If not, go on.  Otherwise...

        // get address
        // set addr blk size to 3 or 4 longs
        sts = exe_std$readchk(i,p,u,i->irp$l_qio_p3,size);      // check access, set IRP$W_BCNT.

#if 0
    movw    #U$Recv,R11     // Receive request code
#endif
    sts = netacp_alive(u);      // network ACP still running?
    if (sts!=SS$_NORMAL)
        return sts;

    // Check for access to user specified data buffer

    if (i->irp$l_qio_p2<=0)     // get the size
        // < 0 gives an error

        // Return error on bad size or bad local connection ID.

        return exe_std$abortio(i, p, u, SS$_BADPARAM);

    if (i->irp$l_qio_p2>MAX_USIZE)      // Make sure he isn't asking too much
        // OK, check buffer access.

        // Requested buffer TOO large, return error.

        return exe_std$abortio(i, p, u, SS$_INSFMEM);

    // Check the access.  Both system rtns (EXE$.....CHK) do NOT return if
    // an error occurs but returns to QIO error handler.
    // ReadCHK sets: IRP$W_BCNT & IRP$V_Func in IRP$W_STS(read function).

    // get address
    sts = exe_std$readchk(i,p,u,i->irp$l_qio_p1,i->irp$l_qio_p2);

    // Allocate a ACP arg block buffer.
    // Requested data buffer + ACP args <= IRP size, use IRP size.
    // This is to prevent dynamic non-paged pool fragmentation.
    // R1 = size (bytes) of system buffer needed.

    int len= i->irp$l_qio_p2+RE$BLK_SIZE;   // Fit in IRP size
    if (len<IRP$K_LENGTH)
        len = IRP$K_LENGTH;         // User larger of the two sizes.

    sts = getbuf(i,p,u,c,&buf,len);         // allocate/check quotas.
    ab_buf=buf;

    // Set:
    // user's buffer address in ACP arg blk.
    // Data size
    // Local Connection ID
    // I/O Request Tag
    // R2 = address of ACP arg block

    buf->re$local_conn_id=u->ucb$l_cbid;    // Connection ID
    ab_buf->ab$ubuf_adrs=i->irp$l_qio_p1;
    buf->re$buf_size=i->irp$l_qio_p2;       // amount of user data in bytes
    ab_buf->ab$funct=U$RECV;        // Set function code
    ab_buf->ab$protocol=u->ucb$l_protocol;// Set arg blk protocol code
    ab_buf->ab$uargsize=RE$BLK_SIZE;    // Set argument block size

    buf->re$ph_buff=0;      // Pass addr to Uargs
    if (i->irp$l_qio_p3)            // Was address specified?
    {
        // If not, go on.  Otherwise...

        // !!!HACK!!! I think this code is UHG-lee!
        sts = getdiag(i,p,u,c,&diag_buf,size);  // allocate diagnostic buffer

        diag_buf->bufio$ps_uva32=i->irp$l_qio_p3; // Point to user's P0 buffer.
        i->irp$l_diagbuf=diag_buf;  // point at system buffer.
        i->irp$l_sts|=IRP$M_DIAGBUF; // Set diagbuff flag in the IRP.
        buf->re$ph_buff=diag_buf->bufio$ps_pktdata; // Pass addr to Uargs
        diag_buf->bufio$w_size=size;
    }

    // Queue IRP & ACP arg blk to network acp "IPACP"

    // We must set the system buffer data start address so that
    // the IPACP won't over-write the Uargs we need for post-processing.
    // !!!HACK!!! This is no longer necessary (w/o Finishio)
    buf->re$data_start=&buf->re$data;   // reset sysbuff data start

    // Now send that puppy off to see the world...
    return queue_2_acp(i,p,u);          // finish up (not really)
}



// Common routines for Info & Status.

int net_status(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)                  // Request connection status
{
    // Status request code
    return net_r_common(i,p,u,c,U$STATUS);      // Go to common code
}

net_info(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)                // Request connection info
{
    // Info request code
    return net_r_common (i,p,u,c,U$INFO);
}

// Info & Status requests join here, with R11 containing the request code.

int net_r_common(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int val)
{
    struct user_status_args * buf;
    struct user_abort_args * ab_buf;
    int sts = netacp_alive(u);      // network ACP still running?
    if (sts!=SS$_NORMAL)
        return sts;

    // Check for access to user specified data buffer

    if (i->irp$l_qio_p2<=0)     // get the size
        // < 0 gives an error

        // Return error on bad size or bad local connection ID.

        return exe_std$abortio(i, p, u, SS$_BADPARAM);

    if (i->irp$l_qio_p2>MAX_USIZE)      // Make sure he isn't asking too much
        // OK, check buffer access.

        // Requested buffer TOO large, return error.

        return exe_std$abortio(i, p, u, SS$_INSFMEM);

    // Check the access.  Both system rtns (EXE$.....CHK) do NOT return if
    // an error occurs but returns to QIO error handler.
    // ReadCHK sets: IRP$W_BCNT & IRP$V_Func in IRP$W_STS(read function).

    // get address
    sts = exe_std$readchk(i,p,u,i->irp$l_qio_p1,i->irp$l_qio_p2);

    // Allocate a ACP arg block buffer.
    // Requested data buffer + ACP args <= IRP size, use IRP size.
    // This is to prevent dynamic non-paged pool fragmentation.
    // R1 = size (bytes) of system buffer needed.

    int len= i->irp$l_qio_p2+ST$BLK_SIZE;   // Fit in IRP size
    if (len<IRP$K_LENGTH)
        len = IRP$K_LENGTH;         // User larger of the two sizes.
    sts = getbuf(i,p,u,c,&buf,len);         // allocate/check quotas.
    ab_buf=buf;

    // Set:
    // user's buffer address in ACP arg blk.
    // Data size
    // Local Connection ID
    // I/O Request Tag
    // R2 = address of ACP arg block

    buf->st$local_conn_id=u->ucb$l_cbid;    // Connection ID
    ab_buf->ab$ubuf_adrs=i->irp$l_qio_p1;
    buf->st$buf_size=i->irp$l_qio_p2;       // amount of user data in bytes
    ab_buf->ab$funct=val;       // Set function code
    ab_buf->ab$protocol=u->ucb$l_protocol; // Set arg blk protocol code
    ab_buf->ab$uargsize=ST$BLK_SIZE;    // Set argument block size

    return queue_2_acp(i,p,u);          // finish up
}


// Network dump debugging function.
// Basically a read function, but a little bit different.

int net_dump(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    struct debug_dump_args * buf;
    struct user_abort_args * ab_buf;
    int sts = netacp_alive(u);      // network ACP still running?
    if (sts!=SS$_NORMAL)
        return sts;

    // Check for access to user specified data buffer

    if (i->irp$l_qio_p2<=0)     // get the size
        // < 0 gives an error

        // Return error on bad size or bad local connection ID.

        return exe_std$abortio(i, p, u, SS$_BADPARAM);

    // Did user request more than we want to give em?

    if (i->irp$l_qio_p2>DU$MAX_SIZE)        // Is he asking for too much?
        // OK, check buffer access.

        // Requested buffer TOO large, return error.

        return exe_std$abortio(i, p, u, SS$_INSFMEM);

    // Check the access.  Both system rtns (EXE$.....CHK) do NOT return if
    // an error occurs but returns to QIO error handler.
    // ReadCHK sets: IRP$W_BCNT & IRP$V_Func in IRP$W_STS(read function).

    // get address
    sts = exe_std$readchk(i,p,u,i->irp$l_qio_p1,i->irp$l_qio_p2);

    // Allocate a ACP arg block buffer.
    // If requested data buffer + ACP args <= IRP size, use IRP size.
    // This is to prevent dynamic non-paged pool fragmentation.
    // R1 = size (bytes) of system buffer needed.

    int len= i->irp$l_qio_p2+DU$BLK_SIZE;   // Fit in IRP size
    if (len<IRP$K_LENGTH)
        len = IRP$K_LENGTH;         // User larger of the two sizes.
    sts = getbuf(i,p,u,c,&buf,len);         // allocate/check quotas.
    ab_buf=buf;

    // Set:
    // user's buffer address in ACP arg blk.
    // Data size
    // Dump function code
    // I/O Request Tag
    // R2 = address of ACP arg block

    ab_buf->ab$ubuf_adrs=i->irp$l_qio_p1;
    buf->du$buf_size=i->irp$l_qio_p2;       // amount of user data in bytes
    buf->du$funct=i->irp$l_qio_p3;      // dump directive
    buf->du$arg0=i->irp$l_qio_p4;       // Argument #0
    buf->du$arg1=i->irp$l_qio_p5;       // Argument #1
    buf->du$arg2=i->irp$l_qio_p6;       // Argument #2
    ab_buf->ab$funct=M$DUMP;        // set ACP function code
    ab_buf->ab$protocol=255; // Set arg blk protocol code
    ab_buf->ab$uargsize=DU$BLK_SIZE;    // set UARG block size
    return queue_2_acp(i,p,u);
}


// Network NCP function.
// Delivery a buffer of commands to the IPACP.  Return the results to
// the client

int net_ncp(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    struct NCP * buf;
    struct user_abort_args * ab_buf;
    int sts = netacp_alive(u);      // network ACP still running?
    if (sts!=SS$_NORMAL)
        return sts;

    if (i->irp$l_qio_p5>=0)     // get the size
    {
        // < 0 gives an error

        // Check for access to user specified return buff, if given.
        if (i->irp$l_qio_p4)            // Was address specified?
            // If not, go on.  Otherwise...
            // get address
            sts = exe_std$readchk(i,p,u,i->irp$l_qio_p4,i->irp$l_qio_p5);       // check access, set IRP$W_BCNT.

        // Check for access to user specified data buffer

        if (i->irp$l_qio_p3>0)      // get the size
            // < 0 gives an error
            return exe_std$abortio(i, p, u, SS$_BADPARAM);

        // Return error on bad size or bad local connection ID.

    }
    else
        return exe_std$abortio(i, p, u, SS$_BADPARAM);

    // Did user request more than we want to give em?

    if (i->irp$l_qio_p3>DU$MAX_SIZE)        // Is he asking for too much?
        // OK, check buffer access.

        // Requested buffer TOO large, return error.

        return exe_std$abortio(i, p, u, SS$_INSFMEM);

    // Check the access.  Both system rtns (EXE$.....CHK) do NOT return if
    // an error occurs but returns to QIO error handler.
    // ReadCHK sets: IRP$W_BCNT & IRP$V_Func in IRP$W_STS(read function).

    // get address
    sts = exe_std$writechk(i, p, u, i->irp$l_qio_p2, i->irp$l_qio_p3);

    // Allocate a ACP arg block buffer.
    // If requested data buffer + ACP args <= IRP size, use IRP size.
    // This is to prevent dynamic non-paged pool fragmentation.
    // R1 = size (bytes) of system buffer needed.

    int len2=i->irp$l_qio_p3+DU$BLK_SIZE;   // Size of input IRP
    int len=i->irp$l_qio_p3+DU$BLK_SIZE;    // Size of output IRP
    if (len<len2)               // Use the larger of the two
        len=len2;               // Use size of input IRP
    if (len<IRP$K_LENGTH)
        len = IRP$K_LENGTH;         // User larger of the two sizes.
    // just pasted

#if 0
    addl3   #DU$BLK_SIZE,P3(AP),R0  // Size of input IRP
    addl3   #DU$BLK_SIZE,P5(AP),R1  // Size of output IRP
    cmpl    R0,R1           // Use the larger of the two
    BLSS    l17;
    movl    R0,R1           // Use size of input IRP
l17:
    cmpl    #IRP$K_LENGTH,R1
    Blss    l20;            // User larger of the two sizes.
    movzbl  #IRP$K_LENGTH,R1
#endif
l20:
    sts = getbuf(i,p,u,c,&buf,len);         // allocate/check quotas.
    ab_buf=buf;

    // Move the data into the Arg Blk.
    memcpy(&buf->ncp$data,i->irp$l_qio_p2,i->irp$l_qio_p3);

    // Set:
    // user's buffer address in ACP arg blk.
    // Data size
    // Dump function code
    // I/O Request Tag
    // R2 = address of ACP arg block

    buf->ncp$ncpfunct=i->irp$l_qio_p1;      // NCP directive
    ab_buf->ab$ubuf_adrs=i->irp$l_qio_p4;
    buf->ncp$wbufsize=i->irp$l_qio_p3;  // amount of user data in bytes
    buf->ncp$rbufsize=i->irp$l_qio_p5;  // amount of user data in bytes
    ab_buf->ab$funct=M$NCP;     // set ACP function code
    ab_buf->ab$protocol=255;        // Set arg blk protocol code
    ab_buf->ab$uargsize=NCP$BLK_SIZE;// set UARG block size
    ab_buf->ab$data_start=&buf->ncp$data;// buffer start
    return queue_2_acp(i,p,u);
}

//.SBTTL    FDT Routines: GTHST - get host info

//++
//
// Functional description:
//
//  Name translation function argument structure:
//
//  =============================================================
//  +   Data start - Points at user data within the blk     +
//  !-----------------------------------------------------------!
//  +           User's Buffer Address.          +
//  !-----------------------------------------------------------!
//  +           VMS Block ID longword.          +
//  !-----------------------------------------------------------!
//  +   IRP Address - Used by ACP for IO postprocessing     +
//  !-----------------------------------------------------------!
//  +   UCB Address - Used by ACP for IO postprocessing     +
//  !-----------------------------------------------------------!
//  +               PID                 +
//  !-----------------------------------------------------------!
//  +      Unused        !  GTHST function  !  Function +
//  !-----------------------------------------------------------!
//  +            Function Arguments             +
//  !-----------------------------------------------------------!
//
// Inputs:
//
//  R0-R2   - scratch registers
//  R3  - address of the IRP (I/O request packet)
//  R4  - address of the PCB (process control block)
//  R5  - address of the UCB (unit control block)
//  R6  - address of the CCB (channel control block)
//  R7  - bit number of the I/O function code
//  R8  - address of the FDT table entry for this routine
//  R9-R11  - scratch registers
//  AP  - address of the 1st function dependent QIO parameter
//
// QIO P1 - P6 Parameters
//
//  P1 = User buffer address
//  P2 = User buffer size
//  P3 = GTHST function code
//  P4 = Function-specific argument
//  P5 = Function-specific argument
//  P6 = Function-specific argument
//
// Outputs:
//
//  IRP & associated system buffer(ACP argument block) are queued to
//  the IP ACP.
//
//  The routine must preserve all registers except R0-R2, and
//  R9-R11.
//
// User Error Returns:
//
//  SS$_MEDOFL  = Network ACP not running
//  SS$_ACCVIO  = Unable to access HOST name string.
//  SS$_BADPARAM    = Host name string + null byte > 128 characters.
//  SS$_EXQUOTA = unable to allocate ACP argument blk.
//  SS$_INSFMEM = Same as SS$_EXQUOTA.
//
//--

int net_gthst(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    struct GH * buf;
    struct user_abort_args * ab_buf;
    int len;

    int sts = netacp_alive(u);      // Make sure ACP is alive
    if (sts!=SS$_NORMAL)
        return sts;

    // Check for access to user data buffer

    if (i->irp$l_qio_p2<0)      // Get the size
        // Good

        // Return error on bad size

        return exe_std$abortio(i, p, u, SS$_BADPARAM);

    // Check size

    if (i->irp$l_qio_p2>GH$MAX_SIZE)
        // OK - check buffer access
        // Buffer too large error
        return exe_std$abortio(i, p, u, SS$_INSFMEM);

    // Check access to buffer

    // User buffer address
    sts = exe_std$readchk(i,p,u,i->irp$l_qio_p1,i->irp$l_qio_p2);       // Check access

    // Buffer ok - now check function-specific arguments

    int size = 4;           // Assume arguments are one longword
    // Get subfunction code
    if (i->irp$l_qio_p3==U$GH_NAMADR)   // Doing name to A-rec?
        goto    l7;         // Check name string.
    if (i->irp$l_qio_p3==U$GH_RRLOOK)   // Doing RR lookup?
        goto    l8;         // No? don't check name string, then.

    // For NAMADR and RRLOOK, P4 has pointer to domain name string

l7:         // Get address of string
    // Minimal size
    sts = exe_std$readchk(i,p,u,i->irp$l_qio_p4,1);

    // Verify length to be < limit

    if (strlen(i->irp$l_qio_p1)>FH_NAME_SIZE)   // Find the null
        // None - punt
        return exe_std$abortio(i, p, u, SS$_BADPARAM);

    // Calculate string length

    // String size into R9
    size = strlen (i->irp$l_qio_p1);

    // If we are doing an RRLook, check the RR-type parameter (P5).

    if (i->irp$l_qio_p3!=U$GH_RRLOOK)   // Doing RR lookup?
        goto    l8;         // No? don't check 2nd arg, then.

#if 0
    movl    P5(AP),R0       // Get RType arg
#endif
    // !!!HACK!!! check bounds here.

    // Allocate a system buffer for the argument block
    // R1 = size (bytes) of system buffer needed.
    // R9 = size of GTHST arguments (in addition to basic block)


    // just pasted
l8:
    len= i->irp$l_qio_p2+GH$BLK_SIZE+size;  // Fit in IRP size
    if (len<IRP$K_LENGTH)
        len = IRP$K_LENGTH;         // User larger of the two sizes.
l9:
    sts = getbuf(i,p,u,c,&buf,len);         // allocate/check quotas.
    ab_buf=buf;

    // Fill in arguments
    // R1 = Size in bytes
    // R2 = address of system buffer
    // R9 = size of second argument
    // R11 = GTHST subfunction code
    // P1 = address of user's buffer
    // P2 = size of user's buffer
    // P4 = Argument #1 (host name or IP address)
    // P5 = Argument #2 (For RR lookup, this is the resource type arguement)
    // P6 = Argument #3 (undefined for all current functions)

    ab_buf->ab$ubuf_adrs=i->irp$l_qio_p1;
    buf->gh$bufsize=i->irp$l_qio_p2;
    ab_buf->ab$funct=U$GTHST;
    ab_buf->ab$protocol=255; // Set arg blk protocol code
    ab_buf->ab$uargsize=GH$BLK_SIZE;
    buf->gh$ghfunct=i->irp$l_qio_p3;
    buf->gh$arg1=42;        // init agr1...
    buf->gh$arg2size=size;  // Set length of argument

    //NAMADR function - copy host name string

    if (i->irp$l_qio_p3==U$GH_NAMADR)   // Name to address translation?
    {
        memcpy(&buf->gh$arg2,i->irp$l_qio_p4,size);
        goto    l20;            // Common finish
    }

    //ADRNAM function

    if (i->irp$l_qio_p3==U$GH_ADRNAM)   // Address to name translation?
    {
        memcpy(&buf->gh$arg2,&i->irp$l_qio_p4,4);
        goto    l20;            // Common finish
    }

    //RRLOOK function - copy domain name string

    if (i->irp$l_qio_p3==U$GH_RRLOOK)   // Address to name translation?
    {
        buf->gh$arg1=i->irp$l_qio_p5;   // Set length of argument
        // address of system buffer
        memcpy(&buf->gh$arg2,i->irp$l_qio_p4,size);
    }
    //  brb l20;            // Common finish

    //Some function we don't know - let the ACP abort it

l18:

    // Done - give block to the ACP

l20:
    return queue_2_acp(i,p,u);
}

//.SBTTL    FDT Routine: EXIT and Debug routines.

//++
//
// All of the following Maintenance ACP functions require the VMS
// Physical I/O privilege EXCEPT Cancel.
//
// EXIT: Force ACP to Cleanup & exit, QIO func=IP$EXIT=IO$_Release.
// DEBUG: set the level, QIO Func=IP$DEBUG=IO$_Diagnose
//
// Functional description:
//
//  Exit function causes IPACP to cleanup(dismount device) & exit.
//  Debug function sets the level of trace information output
//  by IPACP.
//
//
//  EXIT / DEBUG ACP argument block structure:
//
//  =============================================================
//  +       Data Start - Not used.              +
//  !-----------------------------------------------------------!
//  +       Users Buffer Address - Not used.        +
//  !-----------------------------------------------------------!
//  +       VMS Block ID longword.              +
//  !-----------------------------------------------------------!
//  +   IRP Address - Used by ACP for IO postprocessing     +
//  !-----------------------------------------------------------!
//  +   UCB Address - Used by ACP for IO postprocessing     +
//  !-----------------------------------------------------------!
//  +           PID                 +
//  !-----------------------------------------------------------!
//  +   Not Used.        !    Debug Level   !  Function +
//  =============================================================
//
// Inputs:
//
//  R0-R2   - scratch registers
//  R3  - address of the IRP (I/O request packet)
//  R4  - address of the PCB (process control block)
//  R5  - address of the UCB (unit control block)
//  R6  - address of the CCB (channel control block)
//  R7  - bit number of the I/O function code
//  R8  - address of the FDT table entry for this routine
//  R9-R11  - scratch registers
//  AP  - address of the 1st function dependent QIO parameter
//
// QIO Parameters:
//
//  Exit:
//      no P1 - P6 parameters
//  Debug:
//      P1 = Debug level.
//      P2 = Connection ID.
//
// Outputs:
//
//  IRP & associated system buffer(ACP argument block) are queue to
//  the IP ACP.
//
//  The routine must preserve all registers except R0-R2, and
//  R9-R11.
//
// User Error Returns:
//
//  SS$_MEDOFL  = Network ACP is NOT running.
//  SS$_EXQUOTA = Unable to allocate ACP argument block.
//  SS$_INSFMEM = same as SS$_EXQUOTA.
//
//--

int net_exit(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    struct user_abort_args * buf;
    int sts = netacp_alive(u);      // network ACP still running?
    if (sts!=SS$_NORMAL)
        return sts;
    sts = getbuf(i,p,u,c,&buf,IRP$K_LENGTH);        // allocate & check/account quotas
    buf->ab$funct=M$EXIT;   // set ACP function code
    buf->ab$protocol=255; // Set arg blk protocol code
    buf->ab$uargsize=DB$BLK_SIZE;
    return queue_2_acp(i,p,u);
}

// Set the ACP debug level.  Determines the amount of trace data the ACP
// will generate.

int net_debug(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    struct DB * buf;
    struct user_abort_args * ab_buf;
    int sts = netacp_alive(u);      // network ACP still running?
    if (sts!=SS$_NORMAL)
        return sts;
    // default size
    sts = getbuf(i,p,u,c,&buf,IRP$K_LENGTH);
    ab_buf=buf;
    ab_buf->ab$funct=M$DEBUG;   // set ACP function code
    ab_buf->ab$protocol=255;// Set arg blk protocol code
    ab_buf->ab$uargsize=DB$BLK_SIZE;
    buf->db$level=i->irp$l_qio_p1;  // set debug level
    buf->db$lcid=i->irp$l_qio_p2;   // connection-ID (if any)
    buf->db$group=i->irp$l_qio_p3;  // and logging type (ie. DEBUG,ACTIVITY)
    return queue_2_acp(i,p,u);
}



int net_event(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    struct event_args * buf;
    struct user_abort_args * ab_buf;
    int sts = netacp_alive(u);      // network ACP still running?
    if (sts!=SS$_NORMAL)
        return sts;

    // Check for access to user specified data buffer

    if (i->irp$l_qio_p2>0)      // get the size
        goto    l10;            // must be gtr 0

    // Return error on bad size or bad local connection ID.

    return exe_std$abortio(i, p, u, SS$_BADPARAM);

    // Did user request more than we want to give em?

l10:
    if (i->irp$l_qio_p2<=MAX_USIZE)
        goto    l15;            // OK, check buffer access.

    // Requested buffer TOO large, return error.

    return exe_std$abortio(i, p, u, SS$_INSFMEM);

    // Check the access.  Both system rtns (EXE$.....CHK) do NOT return if
    // an error occurs but returns to QIO error handler.

l15:            // get address
    sts = exe_std$writechk (i, p, u, i->irp$l_qio_p1, i->irp$l_qio_p2);     // check access, set IRP$W_BCNT.

    // Allocate ACP IRP buffer
    // If requested data buffer + ACP args <= IRP size, use IRP size.
    // This is to prevent dynamic non-paged pool fragmentation.
    // R1 = size (bytes) of system buffer needed.

    int len= i->irp$l_qio_p2+EV$BLK_SIZE;   // Fit in IRP size
    if (len<IRP$K_LENGTH)
        len = IRP$K_LENGTH;         // User larger of the two sizes.
#if 0
    addl3   #EV$BLK_SIZE,P2(AP),R1  // Fit in IRP size
    cmpl    #IRP$K_LENGTH,R1
    Blss    l20;            // User larger of the two sizes.
    movzbl  #IRP$K_LENGTH,R1
#endif
l20:
    sts = getbuf(i,p,u,c,&buf,len);         // allocate/check quotas.
    ab_buf=buf;

    // Set:
    // user's buffer address in arg blk.
    // Data size
    //
    // R2 = address of ACP arg block

    ab_buf->ab$ubuf_adrs=i->irp$l_qio_p1;
    buf->ev$buf_size=i->irp$l_qio_p2;       // amount of user data in bytes

    ab_buf->ab$funct=M$EVENT;       // Set function code
    ab_buf->ab$protocol=255; // Set arg blk protocol code
    ab_buf->ab$uargsize=EV$BLK_SIZE;    // Set argblk length

    // Copy data to system buffer
    // Event data starts in User argument block after the event arguments.

Event$Copy_data:
    ab_buf->ab$data_start=&buf->ev$data;
    memcpy(&buf->ev$data,i->irp$l_qio_p1,i->irp$l_qio_p2);
    return queue_2_acp(i,p,u);
}


//  .SBTTL  GetBuf - Allocate a System buffer & Check resources.

//++
// Allocate a Nonpaged-Pool system buffer to be used as a ACP argument block.
// Let I/O post-processing rtns deallocate this buffer.
//
// Inputs:
//
//  R0,R2   - scratch registers
//  R1  - Size (in bytes) of requested buffer.
//  R3  - address of the IRP (I/O request packet)
//  R4  - address of the PCB (process control block)
//  R5  - address of the UCB (unit control block)
//  R6  - address of the CCB (channel control block)
//  R7  - bit number of the I/O function code
//  R8  - address of the FDT table entry for this routine
//  R9-R11  - scratch registers
//  AP  - address of the 1st function dependent QIO parameter
//
//
// Outputs:
//
// Routine returns to caller on success Otherwise JMP's to EXE_STD$ABORTIO.
//
//   Success:
//  R1 = Size of allocated system buffer, rounded to next 16 byte boundary.
//  R2 = Address of newly allocated system buffer.
//  IRP$L_SVAPTE - points at buffer
//  IRP$B_BOFF = size of buffer in bytes.
//  Selected ACP Argument block fields are set:
//   AB$L_PID = Pid of requesting process
//   AB$L_IRP_Adrs = address of IRP so ACP can find it again.
//   AB$Data_Start = address of actual data within this block.  Used
//   by VMS I/O post processing rtns to copy (RECEIVE request) data from
//   this system buffer to the user specified buffer.  Address is
//   actually the start of the ACP argument block (portion after the
//   VMS default block header).
//
//  The routine must preserve all registers except R0-R2.
//
// User Error Returns:
//
//  SS$_EXQUOTA = Unable to allocate ACP argument block.
//  SS$_INSFMEM = same as SS$_EXQUOTA.
//
//--

//  .Enabl  LSB

int getbuf(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb *c, void ** newbuf, int req_size)
{
#if 0
    // check. drop this until it gets implemented
    pushr   #^M<R1,R3>
    // .IF  DEFINED VMS_V4
    JSB G^EXE$BufQuoPrc     // Can we afford the buffer?
    // .IF_FALSE
    JSB G^EXE$DEBIT_BYTCNT_ALO
    // .ENDC
    popr    #^M<R1,R3>
    Blbs    R0,l10;

    // Couldn't afford buffer allocation, return error

    popl    r1          // clear JSB return address
    return exe_std$abortio(i, p, u, SS$_);

    // Quota OK

l10:
    // .IF  DEFINED VMS_V4
    pushl   R3          // save IRP address
    JSB G^EXE$ALLOCBUF      // get a buffer
    popl    R3
    Blbs    R0,l20;         // Success?

    // Unable to allocate system buffer

    popl    R1
    return exe_std$abortio(i, p, u, SS$_);

    // Handle accounting chores.
    // R1 = size of system buffer
    // R2 = address
    // charge job for resources

l20:
    movl    PCB$L_JIB(R4),R0    // get Job Information blk adrs
    subl2   R1,JIB$L_BytCnt(R0) // charge it!

    // Set IRP fields
    // .ENDC
#endif
    struct _bufio * buf = kmalloc (req_size,GFP_KERNEL);
    memset(buf, 0, req_size);
    buf->bufio$w_size=req_size;
    buf->bufio$b_type=DYN$C_BUFIO;
    struct user_abort_args * ab_buf = buf;
    i->irp$l_svapte=buf;    // point at system buffer.
    i->irp$l_boff=req_size; // total size of system buffer

    // Set system buffer block header

#if 0
    movab   AB$B_Data(R2),Sb$l_Data_Adrs(R2)
#endif
    buf->bufio$ps_pktdata=&ab_buf->ab$b_data;

    // Set standard ACP arg blk fields

    //  movl    IRP$L_PID(R3),ab$pid(R2)    // include the process PID
#if 0
    movl    IRP$L_PID(R3),R0        // Transform the internal PID
#endif
    ab_buf->ab$pid=exe$ipid_to_epid(i->irp$l_pid);  // to external PID format
    ab_buf->ab$irp_adrs=i;          // IRP address
    ab_buf->ab$ucb_adrs=u;          // unit control blk address.
    ab_buf->ab$funct=0;         // clear function
    ab_buf->ab$protocol=0;
    ab_buf->ab$uargsize=0;          // and argblk size
    *newbuf=buf;
    return SS$_NORMAL;          // return to caller
}

//  .DSABL  LSB

//  .SBTTL  GetDiag - Allocate a System buffer & Check resources.

//++
// Allocate a Nonpaged-Pool system buffer to be used as a diagnostic buffer.
// This diagnostic buffer is used to return the protocol headers.
// Let I/O post-processing rtns deallocate this buffer.
//
// Inputs:
//
//  R0,R2   - scratch registers
//  R1  - Size (in bytes) of requested buffer.
//  R3  - address of the IRP (I/O request packet)
//  R4  - address of the PCB (process control block)
//  R5  - address of the UCB (unit control block)
//  R6  - address of the CCB (channel control block)
//  R7  - bit number of the I/O function code
//  R8  - address of the FDT table entry for this routine
//  R9-R11  - scratch registers
//  AP  - address of the 1st function dependent QIO parameter
//
//
// Outputs:
//
// Routine returns to caller on success Otherwise JMP's to EXE_STD$ABORTIO.
//
//   Success:
//  R1 = Size of allocated system buffer, rounded to next 16 byte boundary.
//  R2 = Address of newly allocated system buffer.
//  IRP$L_DIAGBUF - points at buffer
//  Selected ACP Argument block fields are set:
//   AB$Data_Start = address of actual data within this block.  Used
//   by VMS I/O post processing rtns to copy (RECEIVE request) data from
//   this system buffer to the user specified buffer.  Address is
//   actually the start of the ACP argument block (portion after the
//   VMS default block header).
//
//  The routine must preserve all registers except R0-R2.
//
// User Error Returns:
//
//  SS$_EXQUOTA = Unable to allocate ACP argument block.
//  SS$_INSFMEM = same as SS$_EXQUOTA.
//
//--

//  .Enabl  LSB

int getdiag(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, void ** newbuf, int req_size)
{
    //N.B. We don't debit the diagnostic buffer against the BytCnt since
    // we are not credited upon IO-completion.
    struct _bufio * buf;
    int sts=1;
    // Add in system buffer header size
#if 0
    JSB G^EXE$ALLOCBUF
    with sts=exe$allocbuf
#endif
             buf=kmalloc(req_size+sizeof(struct _bufio), GFP_KERNEL);
    buf->bufio$w_size=req_size+sizeof(struct _bufio);
    buf->bufio$b_type=DYN$C_BUFIO;
    if ((sts&1)==0)

        // Couldn't allocate, return error

        // clear JSB return address
        return exe_std$abortio(i, p, u, sts);

    // Set system buffer block header
    buf->bufio$ps_pktdata=&buf->bufio$pq_uva64; // check

    // Set standard ACP arg blk fields

    *newbuf=buf;
    return SS$_NORMAL;              // return to caller
}

//  .DSABL  LSB

//  .SBTTL  IP-FINISHIO, Finish I/O routine

//++
// IP_FINISHIO, perform special I/O post processing.
//
// Functional description:
//
// Inputs:
//
//  R5  - address of the current IRP (I/O request packet)
//
// Outputs:
//
//  The routine must preserve all registers except R0-R2 and R4.
//
// Function:
//!!!HACK!!! This is no longer used.
//  When a user requests a RECEIVE and provides the IPDriver with
//  a special address buffer, the IPDriver will build an IRP with
//  the address of this routine in the IRP$L_PID field.  When the
//  IOC$IOPOST interrupt attempts to post-process the IRP, it will
//  Look at the IRP$L_PID field and see that the high bit is set,
//  since the driver lives in system space (S0), and it will know
//  to call our routine with a JSB.  After we receive control, we
//  can copy the "protocol header" fields of the UArg system block
//  into the user supplied buffer.  We must return control to
//  IOC$IOPOST with an RSB.
//--

int ip_finishio(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)     // Start processing something the IPACP has posted to us.
{
    int sts;
#if 0
    JSB G^INI$BRK   // Execute BPT (BreakPoint) instruction)

    Pushr   #^M<R3,R5>          // Let's save these...
    // R3 := IRP, R5 := UCB
    MOVL    R5,R3
    MOVL    IRP$L_UCB(R3),R5
#endif

    // We're back!
    i->irp$l_pid=i->irp$l_extend;   // restore orig. PID
#if 0
    MOVL    IRP$L_PID(R3),R0        // for debugging
#endif

    // Let's put the UArg system block in R1
    struct user_recv_args * buf = i->irp$l_svapte;      // Find UArgs.
    if (buf==0)             // Do we still have them?
        goto    l10;                // If not, go on.  Otherwise...

    // Make sure client has supplied a buffer// put it in R0.
#if 0
    //does not make a difference now
    MOVL    RE$PH_Buff(R1),R0       // for debugging
    tstl    R0              // Do we still have them?
    beql    l10;                // If not, go on.  Otherwise...
#endif

    // Skip the transfer for now...
    //  BRW l10;                // Forget the rest...

    //
    //!!!HACK!!! We should check accessability of the buffer here! (maybe?)
    // If it's not accessable, fill in IOSB fields!
    //  movl    RE$Src_Host(R1),IPADR$SRC_HOST(R0) // Set local host
    //  movl    RE$Dst_Host(R1),IPADR$DST_HOST(R0) // Set remote host
    //  movl    RE$EXT1(R1),IPADR$EXT1(R0)  // Set first header extension
    //  movl    RE$EXT2(R1),IPADR$EXT2(R0)  // Set second header extension

l10:
    com$post(i,u);  // post-it.

#if 0
    Popr    #^M<R3,R5>  // restoration
#endif
    return SS$_NORMAL;          // bring it on home, Jimmy.
}


//  .SBTTL  IP-CANCEL, Cancel I/O routine

//++
// IP_CANCEL, CANCELS a Connection for the requesting process.
//
// Functional description:
//
//  Action is to build a fake IRP with the PID of the process & channel #
//  for which the cancel IO was requested.
//  The IRP$W_FUNC field to equal IO$_CLEAN, IRP$W_Chan = IO channel with
//  IRP$L_SVAPTE = 0.  This combination indicates to ACP
//  (User_requests_avail rtn) routine that a connection cancel should be
//  performed for the sepcified PID/Channel #.
//
// Inputs:
//
//  R2  - negated value of the channel index number
//  R3  - address of the current IRP (I/O request packet)
//  R4  - address of the PCB (process control block) for the
//        process canceling I/O
//  R5  - address of the UCB (unit control block)
//
// Outputs:
//
//  Fake IRP is queued to the IPACP process to cancel IO for the specified
//  PID/channel # pair.
//  The routine must preserve all registers except R0-R3.
//
//--

int ip_cancel(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int chan)                 // Cancel an I/O operation
{
    struct _irp * newirp;
    int wasempty;

    // Have we cancelled this connection before? (UCB$V_Cancel will be set).
    // Reason for this test is that BOTH the cancel IO system service ($CANCEL)
    // and $DEASGN call this routine.  If a user process is stopped via a Control-C
    // then when the image gets a forced exit SYSRUNDWN will cancel ALL IO & then
    // deassign all assigned IO channels thus calling this routine twice.  By being
    // aware of the cancel bit in the UCB we can prevent bothering the IPACP a
    // second time.  One must remember that the UCB's for this device are cloned &
    // you get a NEW UCB every time you assign the device "IP:".  This is why we
    // don't step on others with the cancel bit.

#if 0
    // .IF  DEFINED VMS_V4
    if (test_and_clear_bit(UCB$V_Cancel,u->UCB$L_STS)) goto Finish_Cancel; // bbsc
    if (test_bit(DEV$V_AVL,&u->UCB$L_DEVCHAR)==0) goto Finish_Cancel; // bbc
#endif
    // .IF_FALSE
#define UCB$V_CANCEL 3
#define DEV$V_AVL 18 // check this
    if (test_and_clear_bit(UCB$V_CANCEL, u->ucb$l_sts)) // bbcc
        return SS$_NORMAL;
    if (test_bit(DEV$V_AVL, &u->ucb$l_devchar)==0)
        return SS$_NORMAL;

    // First IO cancel, flag as such.

    u->ucb$l_sts|=UCB$M_CANCEL;     // flag IO is cancelled.
    newirp=kmalloc(sizeof(struct _irp),GFP_KERNEL);
    memset(newirp,0, sizeof(struct _irp));

    // Init the IRP, R2 = IRP address.

    newirp->irp$w_size=IRP$C_LENGTH;
    newirp->irp$b_type=DYN$C_IRP;
    newirp->irp$l_svapte=0;     // NO ACP argblk.
    newirp->irp$l_func=IO$_CLEAN;   // Clean up connection.
    newirp->irp$b_pri=4;        // process base priority.
    newirp->irp$l_pid=p->pcb$l_pid; // PID of requestor.
    newirp->irp$w_chan=chan;        // channel #
    newirp->irp$l_ucb=u;        // UCB address
    //  movl    UCB$L_PROTOCOL(R5),IRP$L_WIND(R2)   // protocol
    newirp->irp$l_extend=u->ucb$l_protocol; // protocol

    // Queue Fake IRP to ACP.

#if 0
    movl    R2,R3               // for insertirp.
    movl    UCB$L_VCB(R5),R2        // VCB adrs
    movl    VCB$L_AQB(R2),R2        // asrs of ACP Q listhead.
    // .IF  DEFINED VMS_V4
    exe$insertirp(u,i);         // Q it!
    // .IF_FALSE
    // Warning: V5 change! EXE$INSERTIPR no longer works! ACP queues are
    // now self-relative!
    //  exe$insertirp(u,i);         // Q it!
#endif
    int R0 = 0;

l8:
    wasempty = aqempty(&u->ucb$l_vcb->vcb$l_aqb->aqb$l_acpqfl);
    insque(newirp,&u->ucb$l_vcb->vcb$l_aqb->aqb$l_acpqfl);  // stuff in the queue
#if 0
    insqti  (R3), (R2)          // stuff in the queue
    BGEQU   l9;             // Made it
#endif
    goto l9;  // check interlck risk
    R0++;
    if (R0<900000) goto l8;         // Nope try again
#if 0
    BUG_CHECK   BADQHDR, FATAL
#endif
    panic("badqhdr\n");

l9:
    // .ENDC
    if (wasempty) goto  Finish_Cancel;

    // Wake ACP

#if 0
    // .IF  DEFINED VMS_V4
    JSB G^SCH$WAKE
    // .IF_FALSE
#endif
    vmslock(&SPIN_SCHED,IPL$_SCHED);
    sch$wake(u->ucb$l_vcb->vcb$l_aqb->aqb$l_acppid);
    vmsunlock(&SPIN_SCHED,0);
    // .ENDC

Finish_Cancel:
    return SS$_NORMAL;                  // Return
}

// .sbttl   Netacp_alive, Check if network acp process is alive.

//++
// Netacp_alive - Check if the network ACP is still running.
//
// Functional description:
//
//  check device available bit in ucb.  Bit is cleared if acp exits.
//
// Inputs:
//
//  R3  - address of the current IRP (I/O request packet)
//  R5  - address of the UCB (unit control block)
//
// Outputs:
//
//  returns if acp is running
//  otherwise, pop return address from stack and jmp to abort io rtn.
//  The routine must preserve all registers except R0-R3.
//--

int netacp_alive(struct _ucb * u)
{
    if (u->ucb$l_devchar & DEV$M_AVL)
        return SS$_NORMAL;
    return exe_std$abortio(u->ucb$l_irp, ctl$gl_pcb, u, SS$_MEDOFL);
}

//++
// Label that marks the end of the driver
//--

#if 0
IP_END:             // Last location in driver
.END

#endif
