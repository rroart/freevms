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
//TITLE 'Define RFC 855 TELNET protocol'
/*

Module:

	TELNET.REQ

Facility:

	Define the TELNET Virtual Terminal Protocol as according to RFC 855.

Author:

	Dale Moore, CMU-CSD, April, 1986
	Copyright (c) 1986,1987 Carnegie-Mellon University

Modification history:

1.1  05-Sep-89, Edit by BRM
	Added new options.

1.0  21-Jul-87, Edit by VAF
	Convert from TELNET.R32 used with TELNET user program for use with new
	ACP TELNET facility (change was just to remove extraneous definitions).

2.2  Dec 1991 J. Clement	Rice University		JC
	Added TTY_CHAN for EIGHTBIT, PASTHRU characteristics.
	Moved QCB macros to here.

2.3	16-Dec-1991	Henry W. Miller		USBR
	Change TVT flag word to ULONG - we were running out of bits
	Added TVT$JAM bit.

2.4	7-Jan-1992	John Clement
	Added TVT$HOLD bit
*/

#include <starlet.h>	// VMS system definitions
// not yet #include 'CMUIP_SRC:[CENTRAL]NETXPORT';

#define     TELNET$K_Debug	  0

//++
// Constants from RFC855
//--

#define     TELNET$K_EOR	  239	// End of Record marker
#define     TELNET$K_SE		  240	// End of subnegotiation parameters
#define     TELNET$K_NOP	  241	// No operation
#define     TELNET$K_DATA_MARK	  242	// The data stream portion of a Synch
			 	// This should always be accompanied
				// by a TCP Urgent notification.
#define     TELNET$K_BRK	  243 	// NVT character BRK
#define     TELNET$K_IP		  244	// The function IP Interrupt Process
#define     TELNET$K_AO		  245	// The function AO Abort output
#define     TELNET$K_AYT	  246	// The function AYT Are You There
#define     TELNET$K_EC		  247	// The function EC Erase character
#define     TELNET$K_EL		  248	// The function EL Erase Line
#define     TELNET$K_GA		  249	// The function GA Go Ahead
#define     TELNET$K_SB		  250	// Indicates that what follows is
				// subnegotiation of the indicated
				// option
#define     TELNET$K_WILL	  251	// Indicates the desire to begin
				// performing, or confirmation that
				// you are now performing, the
				// indicated option
#define     TELNET$K_WONT	  252	// Indicates the refusal to perform
			 	// or continue performing, the
			 	// indicated option.
#define     TELNET$K_DO		  253	// Indicates the request that the
			 	// other party perform, or
			 	// confirmation that you are expecting
			 	// the other party to perform, the
			 	// indicated option.
#define     TELNET$K_DONT	  254	// Indicates the demand that the
			 	// other party stop performing,
			 	// or confirmation that you are no
			 	// longer expecting the other party
			 	// to perform, the indicated option.
#define     TELNET$K_IAC	  255	// Data Byte 255.

//++
//   Constants from RFC946.
//--

#define     TELNET$K_TTLFMT	  0


//++
//   The Telnet TCP connection is established between the user's port U
//   and the server's port L.  The server listens on its well known port L
//   for such connections.  Since a TCP connection is full duplex and
//   identified by the pair of ports, the server can engage in many
//   simultaneous connections involving it's port L and different user
//   ports U.
//
//   Port Assignment
//
//      When used for remote user access to service hosts (i.e., remote
//      terminal access) this protocol is assigned server port 23 (27
//      octal).  That is L=23.
//--

#define     Telnet$K_SERVER_PORT	  23

//++
// I'm not sure where these are defined, but they should be defined
// in some telnet options rfc
// for more info see rfc855
//--

#define     Telnet$K_Number_Of_Options    036

#define     Telnet$K_Binary		  000
#define     Telnet$K_Echo		  001
#define     Telnet$K_Reconnect		  002
#define     Telnet$K_SUPRGA		  003
#define     Telnet$K_Size_Negotiation	  004
#define     Telnet$K_Status		  005
#define     Telnet$K_Timing_Mark	  006
#define     Telnet$K_Remote_Trans_Echo	  007
#define     Telnet$K_Line_Width		  008
#define     Telnet$K_Page_Size		  009
#define     Telnet$K_CR_Disposition	  010
#define     Telnet$K_Tab_Stops		  011
#define     Telnet$K_Tab_Disposition	  012
#define     Telnet$K_FF_Disposition	  013
#define     Telnet$K_Vertical_Tabs	  014
#define     Telnet$K_VT_Disposition	  015
#define     Telnet$K_LF_Disposition	  016
#define     Telnet$K_Extended_Ascii	  017
#define     Telnet$K_Logout		  018
#define     Telnet$K_Byte_Macro		  019
#define     Telnet$K_Data_Entry		  020
#define     Telnet$K_SUPDUP		  021
#define     Telnet$K_SUPDUP_Output	  022
#define     Telnet$K_Location		  023
#define     Telnet$K_Terminal_Type	  024
#define     Telnet$K_End_of_Record	  025
#define     Telnet$K_User_Id		  026
#define     Telnet$K_Output_Marking	  027
#define     Telnet$K_Location_Number	  028
#define     Telnet$K_3270_Regime          029
#define     Telnet$K_X3_PAD               030
#define     Telnet$K_Window_Size          031
#define     Telnet$K_Terminal_Speed       032
#define     Telnet$K_Toggle_Flow_Control  033
#define     Telnet$K_LineMode             034
#define     TELNET$K_X_DISPLAY_LOCATION   035
 
#define     Telnet$K_Extended		  255
#define     Telnet$K_MINOPT		  Telnet$K_Binary
#define     TELNET$K_MAXOPT		  TELNET$K_X_DISPLAY_LOCATION

#define     Option$K_Terminal_Type_Is	  0
#define     Option$K_Terminal_Type_Send	  1

#define     Option$K_TTYLOC_Format_64	  0

#define     Option$K_Terminal_Speed_Is	  0
#define     Option$K_Terminal_Speed_Send  1

#define     Option$K_Tog_Flow_Cntl_OFF	  0
#define     Option$K_Tog_Flow_Cntl_ON	  1

#define     Option$K_Linemode_Mode                1
#define     Option$K_Linemode_Forwardmask         2
#define     Option$K_Linemode_SLC                 3

#define     Option$K_Linemode_Mode_EDIT           0
#define     Option$K_Linemode_Mode_TRAPSIG        1
#define     Option$K_Linemode_Mode_ACK            2

#define     Option$K_Linemode_SLC_MaxOpt          18
#define     SLC_SYNCH		  1
#define     SLC_BRK		  2
#define     SLC_IP		  3
#define     SLC_AO		  4
#define     SLC_AYT		  5
#define     SLC_EOR		  6
#define     SLC_ABORT		  7
#define     SLC_EOF		  8
#define     SLC_SUSP		  9
#define     SLC_EC		  10
#define     SLC_EL		  11
#define     SLC_EW		  12
#define     SLC_RP		  13
#define     SLC_LNEXT		  14
#define     SLC_XON		  15
#define     SLC_XOFF		  16
#define     SLC_FORW1		  17
#define     SLC_FORW2		  18

    // These codes go in the first 2 bits of the function byte.
#define     SLC_DEFAULT		  3
#define     SLC_VALUE		  2
#define     SLC_CANTCHANGE	  1
#define     SLC_NOSUPPORT	  0

#if 0
MACRO   // Define the fields in the function modifier byte of the SLC's.

    SLC$F_Level         = 0, 2%,
    SLC$F_FlushOut      = 2, 1%,
    SLC$F_FlushIn       = 3, 1%,
    SLC$F_Ack           = 4, 1%;
#endif

//++						// JC Begin
// These macros describe the characteristics of the PTY terminal
//--
struct _qcbdef {
  unsigned char QCB$B_CLASS;
  unsigned char QCB$B_TYPE;
  unsigned short  QCB$W_PAGE_WIDTH;
  unsigned char   QCB$L_CHARISTICS[3];
  unsigned char  QCB$B_PAGE_LENGTH;
  unsigned long QCB$L_EXTEND_CHAR;
};

#define    QCB$K_SIZE	12				// JC End

// Define the format of a TELNET option block

struct OPT$BLOCK {
  unsigned char     OPT$BASE[0]	;	// Base address of block
  unsigned     OPT$STATE	 : 1;	// Option state (TRUE=on,FALSE=off)
  unsigned     OPT$CURRENT	 : 1;	// Nonzero if currently negotiating the option
  unsigned    OPT$PREFER	: 2;	// What we'd prefer for the option state
#if 0
  $ALIGN(FULLWORD)
#endif
       void (*     OPT$ON_RTN)();		// Routine to turn the option on
       void (*     OPT$OFF_RTN)();		// Routine to turn the option off
       void (*     OPT$SUB_RTN)();	// Routine to handle subnegotiation
};

#define     OPT$SIZE   sizeof(struct OPT$BLOCK)
#define     OPT$BLEN   OPT$SIZE*4

#if 0
MACRO
    OPT$BLOCK = BLOCK[OPT$SIZE] FIELD(OPT$FIELDS)%,
    OPT$LIST = BLOCKVECTOR[TELNET$K_MAXOPT+1,OPT$SIZE] FIELD(OPT$FIELDS)%;
#endif

#define    OPT$LSTBLEN (OPT$SIZE*(TELNET$K_MAXOPT+1))*4


// Values for OPT$PREFER field

#define     OPT$STATE_OFF	  0
#define     OPT$STATE_ON	  1
#define     OPT$DONT_CARE	  2



// Define the format of a TVT data block.

#define     TVT_MBX_BUFLEN   256	// Size of PTY mailbox message buffer
#define     TVT_TTY_BUFLEN   512	// Size of PTY read/write buffers
#define     TVT_NEG_BUFLEN   128	// Size of negotiation output buffer
#define     TVT_SUB_BUFLEN   128	// Size of subnegotiation input buffer
#define     PTY_BUFFER_SIZE   32 	// Size of PTY internal buffer
				// (From PZDRIVER.MAR and TZDRIVER.MAR
				// BUFFER_SIZE parameter)
struct TVT$BLOCK
{
  void *     TVT$TCB;			// Backpointer to owning TCB
  unsigned int     TVT$PTY_CHN	;		// The PTY device channel
  unsigned int     TVT$MBX_CHN	;		// The PTY mailbox device channel
  unsigned char    TVT$NEG_BUF	 [TVT_NEG_BUFLEN]; // Buffer for negotiations
  unsigned short     TVT$NEG_CNT	;		// Number of bytes in buffer
  void *     TVT$NEG_EQP;			// ENQ pointer into negotiation buffer
  void *     TVT$NEG_DQP;			// DEQ pointer into negotiation buffer
  unsigned char    TVT$SUB_BUF	 [TVT_SUB_BUFLEN]; // Buffer for reading subnegotiation
  unsigned short     TVT$SUB_CNT	;		// Number of bytes in buffer
  void *     TVT$SUB_PTR;			// Pointer to current byte
  unsigned char    TVT$RD_BUF	 [TVT_TTY_BUFLEN]; // Buffer for PTY read
  unsigned short     TVT$RD_BCNT	;		// Number of bytes in the buffer
  void *     TVT$RD_PTR;			// Pointer to current byte
  unsigned long long     TVT$RD_IOSB	;		// IOSB for read operation
  unsigned char    TVT$WR_BUF	 [TVT_TTY_BUFLEN]; // Buffer for PTY write
  unsigned short     TVT$WR_BCNT	;		// Number of bytes in the buffer
  long     TVT$WR_ICNT	 ;		// Number of bytes read
  long     TVT$WR_OCNT	 ;		// Number of bytes written
  //    TVT$WR_PTR	= [$ADDRESS],		// Pointer to current byte
  long     TVT$WR_IPTR	 ;		// Index of current byte read
  long     TVT$WR_OPTR	 ;		// Index of current byte written
  unsigned long long     TVT$WR_IOSB	;		// IOSB for write operation
  unsigned char    TVT$MBX_BUF	 [TVT_MBX_BUFLEN];
  unsigned long long     TVT$MBX_IOSB;		// IOSB for mailbox
  unsigned short     TVT$NRSTATE	;		// Current TVT read state
  union {
    unsigned long     TVT$FLAGS	 ;		// Flags describing the TVT
    struct {
      unsigned 	TVT$CANCEL	 : 1;	// TVT is being deassigned
      unsigned 	TVT$PREAD	 : 1;	// PTY read is in progress
      unsigned 	TVT$PWRITE	 : 1;	// PTY write is in progress
      unsigned 	TVT$NREAD	 : 1;	// Network read is in progress
      unsigned 	TVT$NWRITE	 : 1;	// Network write is in progress
      unsigned 	TVT$NR_CR	 : 1;	// Last network character was a CR
      unsigned 	TVT$NR_LF	 : 1;	// Last network character was a LF
      unsigned 	TVT$NR_SB	 : 1;	// Currently reading a subnegotiation
      unsigned 	TVT$NW_IAC	 : 1;	// Need to send an IAC to the network
      unsigned 	TVT$DO_PID	 : 1;	// Keep trying to call 
      unsigned 	TVT$GAG		 : 1;	// PTY buffer too full, must write or die
      unsigned 	TVT$CTRL	 : 1;	// Special control character seen
      unsigned 	TVT$JAM		 : 1;	// Network output currently jammed
      unsigned 	TVT$HOLD	 : 1;	// Hold off PTY_Write until PID created
      unsigned 	TVT$TTSET	 : 1;	// TTY device changed.
    };
  };
  unsigned long long     TVT$TTYLOC		;	// TTYLOC data for the connection
  unsigned int     TVT$TTYTYPE		;	// VMS terminal type
  unsigned int     TVT$TTYDEPEND	;	// VMS terminal mode
  unsigned int     TVT$TTYDEPEND2	;	// VMS terminal mode (the sequel)
  unsigned char    TVT$LCLOPTS	 [OPT$LSTBLEN]; // Local options state
  unsigned char    TVT$REMOPTS	 [OPT$LSTBLEN]; // Remote options state
  unsigned char    TVT$LM_FORWARD_MASK  [32];
  unsigned char    TVT$LM_MODE  [1];
  union {
    unsigned char    TVT$LM_FLAGS  [1];
    struct {
      unsigned 	TVT$LM_Use_FM  : 1;
    };
  };
  unsigned char    TVT$Type_descr	 [DSC$K_Z_BLN];
  unsigned char    TVT$TERMINAL_TYPE	 [16];		// Terminal type string JC Beg
  unsigned char    TVT$KILL_TERMINAL_TYPE	 ;	// Terminal Negotiation count
  unsigned char    TVT$Device_Descr	 [DSC$K_Z_BLN];
  unsigned char    TVT$TTY_DEVSTR	 [20];		// Terminal name string
  unsigned int     TVT$TTY_CHN		;		// The TTY channel
  unsigned long long     TVT$TTY_IOSTAT	;		// The TTY IO status
  unsigned char    TVT$TTY_CHAR	 [QCB$K_SIZE];	// The TTY DevDEP char JC End
  unsigned char    TVT$DATA_END[0];
}; 

#define     TVT$SIZE   sizeof(struct TVT$BLOCK)
#if 0
MACRO
    TVT$BLOCK = BLOCK[TVT$SIZE] FIELD(TVT$FIELDS) %;
#endif

// Literals which define the TVT state

#define     TVT$STATE_NORMAL   0	// Normal (last char not an IAC)
#define     TVT$STATE_IAC   1		// IAC seen (start negotiation)
#define     TVT$STATE_WILL   2		// IAC WILL seen
#define     TVT$STATE_WONT   3		// IAC WONT seen
#define     TVT$STATE_DO   4		// IAC DO seen
#define     TVT$STATE_DONT   5		// IAC DONT seen
#define     TVT$STATE_MIN   TVT$STATE_NORMAL
#define     TVT$STATE_MAX   TVT$STATE_DONT
