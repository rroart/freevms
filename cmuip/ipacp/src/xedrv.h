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
//!!HACK!!// I need a header here!

// not yet LIBRARY 'CMUIP_SRC:[central]NETXPORT';		// VMS specifics

//Sbttl   'XE Driver Literals'


#define     XE_IP_type	  %X'800'	//Ethernet IP packet type
#define     XE_ARP_type   %X'806'	//Ethernet ARP packet type
#define     XE_hdr_len	  14		//Byte len of Ethernet xmit header
#define     XE_MINSIZE	  46		//Minimum length of an Ethernet packet
#define     XE_hdr_offset   0		//Allow this much space before rcv pkts
#define     XE_max_retries   5		//Maximum retries on send errors
#define     XE_FFFF	  %X'FFFF'	//HIgh values
#define     IOS_len	  8		//Byte length of standard QIO IOSB
#define     ASTEFN	  3		//Value of event flag for AST routines
#define     ARPEFN	  5		//Value of event flag for ARP routine
#define     Qhead_len	  8		//Byte length of standard VMS Q header
#define     XE_ADR_SIZE   6		// Byte length of an Ethernet address


#define     XEDMP$ARP_Entry	  1	// Get an ARP cache entry.



// This structure defines the interface information that is specific
// to the EtherNet driver.
//!!HACK!!// Document these fields

struct  XE_Interface_Structure
{
  void *     XEI$dev_config		;
  unsigned long     XEI$IO_Chan			;
  unsigned long     XEI$ARP_IO_Chan		;
  void *     XEI$rcvhdrs			;
  void *     XEI$recv_Qhead		;
  void *     XEI$recv_Qtail		;
  void *     XEI$ARP_Buffer		;
  void *     XEI$ARP_Block		;
  unsigned short     XEI$Phy_Size		;
  unsigned char     XEI$Phy_Addr		[6];
  unsigned long     XEI$restart_time		;
  unsigned short     XEI$restart_count		;
  unsigned short     XEI$retry_count		;
  unsigned short     XEI$max_retry		;
  unsigned short     XEI$MPBS			;
  unsigned short     XEI$curhdr			;
  union {
    unsigned short     XEI$Flags			;
    struct {
      unsigned 	XEI$need_2_free	 : 1;	// XEshutdown buf free pending
      unsigned 	XEI$IO_queued	 : 1;	// XE I/O has been started
      unsigned 	XEI$XE_decnet	 : 1;	// XE DECNET address seen
      unsigned 	XEI$XE_started	 : 1;// XE started at least once
    };
  };
};

#define  XE_Interface_size   $Field_set_size
#if 0
MACRO XE_Interface_Structure = BLOCK[XE_Interface_size] FIELD(XE_Interface_Fields)%;
#endif



//Sbttl   'XE Driver Structures'

// This structure defines an address for an DEC Ethernet device.  It is
// general purpose in that it may be used for either a source or a
// destination address in any format.

struct  XE_addrs_structure
{
  union {
    unsigned long     XE_addr_L0         ;
    struct {
      unsigned short        XE_addr_0        ;
      unsigned short        XE_addr_1        ;
    };
  };
  union {
    unsigned long     XE_addr_L1         ;
    struct {
      unsigned short        XE_addr_2        ;
      unsigned short        XE_addr_3       ;
    };
  };
};

#define  XE_Addr_size   $Field_set_size
#if 0
MACRO XE_addrs_structure = BLOCK[XE_addr_size] FIELD(XE_address)%;
#endif

// This structure defines the VMS I/O status block for the XE_Driver

struct XE_iosb_structure 
{
  unsigned short int     XE$vms_code        ;
  unsigned short int     XE$tran_size       ;
  unsigned short     XE$cmd_status      ;
  unsigned char     XE$error_summary   ;
  unsigned char     XE$iosb_unused2    ;
};

#define  XE_iosb_len   $Field_set_size
#if 0
MACRO XE_iosb_structure = BLOCK[XE_iosb_len] FIELD(XE_iosb)%;
#endif

// Structure of DEC ethernet setup package

struct XE_setup_structure 
{
  unsigned short     XE$c_pcli_bus      ;
  unsigned long     XE$l_buffer_length ;
  unsigned short     XE$c_pcli_bfn      ;
  unsigned long     XE$l_number_buff   ;
  unsigned short     XE$c_pcli_pad      ;
  unsigned long     XE$l_padding       ;
  unsigned short     XE$c_pcli_pty      ;
  unsigned long     XE$l_protocol      ;
  unsigned short     XE$c_pcli_prm      ;
  unsigned long     XE$l_promiscuous   ;
  unsigned short     XE$c_pcli_dch      ;
  unsigned long     XE$l_data_chain    ;
  unsigned short     XE$c_pcli_con      ;
  unsigned long     XE$l_control_mode  ;
  unsigned short     XE$c_pcli_mca      ;
  unsigned short     XE$w_mca_length    ;
  unsigned short     XE$w_mca_mode      ;
  unsigned char     XE$L_mca_address   [XE_ADR_SIZE];
  unsigned char     XE$setup_end       [0];

  // Extended area for use when setting physical address

  unsigned short     XE$c_pcli_pha      ;
  unsigned short     XE$w_pcli_phlen    ;
  unsigned short     XE$w_pcli_phmode   ;
  unsigned char     XE$l_pcli_phaddr   [XE_ADR_SIZE];
  unsigned char     XE$setup_pha_end[0];
};

#define  XE_setup_len   $Field_set_size
#if 0
MACRO XE_setup_structure = BLOCK[XE_setup_len] FIELD(XE_setup)%;
#endif

struct XE_sdesc_structure
{
  unsigned long     XE$setup_length    ;
  void *   XE$setup_address;
};

#define  XE_sdesc_len   $Field_set_size
#if 0
MACRO XE_sdesc_structure = BLOCK[XE_sdesc_len] FIELD(XE_setup_descriptor)%;
#endif

// Structure of DEC ethernet sensing package

// The structure of this critter is copied from the XEDRIVER source.
// The IO$_SENSEMODE function with a P2 parameter always returns every
// parameter that will fit into the provided buffer. If the structure
// of the parameter table in XEDRIVER changes this block will not be
// valid. This is checked by comparing the id codes filled in by the
// QIO of items that are used with their NMA$ counterparts.
//
//$FIELD XE_sense_info =
//    {
#if 0
unsigned short //    XE$id_pcli_acc     ;  !Access mode for protocol type
unsigned long //    XE$val_pcli_acc    ;
unsigned short //    XE$id_pcli_pro     ;  !Protocol selection mode
unsigned long //    XE$val_pcli_pro    ;
unsigned short //    XE$id_pcli_bus     ;  !Buffer size
unsigned long //    XE$val_pcli_bus    ;
unsigned short //    XE$id_pcli_bfn     ;  !Number of buffers
unsigned long //    XE$val_pcli_bfn    ;
unsigned short //    XE$id_pcli_pha     ;  !Physical address
unsigned short //    XE$id_pha_count    ;
unsigned char //    XE$device_addr     [6];  !device address - 6 byte string
unsigned char //    XE$sense_end       ;
//    };
//
//LITERAL XE_sense_len = $Field_set_size;
//MACRO XE_sense_blk = BLOCK[XE_sense_len] FIELD(XE_sense_info)%;
#endif

struct XE_Sense // check
{
  unsigned     XE_Sense_Param:12;//	= [0,  0, 12, 0],
  unsigned XE_Sense_Type:1; // 	= [0, 12,  1, 0],
  unsigned XE_Sense_NotUsed:2;// //	= [0, 13,  2, 0],
  unsigned XE_Sense_Zero:1;//	= [0, 15,  1, 0],
  union {
    // If the Type bit EQL 0
    long XE_Sense_Value; //	= [2,  0, 32, 0],
    // If the Type bit EQL 1
    short XE_Sense_Length; //	= [2,  0, 16, 0],
  };
  unsigned char XE_Sense_String[0]; //	= [4,  0,  0, 0]
};

#define     XE_SS_BYTEOFF   4
#define     XE_TYPE_AND_VALUE   6

#define    XE_Param_Size (Buffer) \
	( Buffer->XE_Sense_Type) ? \
	 (Buffer->XE_Sense_Length + XE_SS_BYTEOFF) : \
	  (XE_TYPE_AND_VALUE)

// Receive buffer Q structure

struct XERCV_QB_structure
{
  void *     XERCV$next		;
  void *     XERCV$last		;
  unsigned short int     XERCV$vms_code	;
  unsigned short int     XERCV$tran_size	;
  unsigned short     XERCV$cmd_status	;
  unsigned char     XERCV$error_summary	;
  unsigned char     XERCV$iosb_unused2	;
  unsigned char     XERCV$data		;
};

#define  XERCV_QB_len   $Field_set_size
#if 0
MACRO XERCV_QB_structure = BLOCK[XERCV_QB_len] FIELD(QB_XE_rcv)%;
#endif

// Structure of ethernet send buffer header

struct XESND_structure 
{
  union {
    unsigned char     XESND$dest	       [6];
    struct {
      unsigned long 	XESND$dst1     ;
      unsigned short 	XESND$dst2     ;
    };
  };
  unsigned short    XESND$type;
};

#define  XESND_len   $Field_set_size
#if 0
MACRO XESND_structure = BLOCK[XESND_len] FIELD(XE_send)%;
#endif 

// Structure of ethernet receive buffer header

struct XERCV_structure  
{
  union {
    unsigned char     XERCV$buf		[16];	// Entire buffer
    struct {
      unsigned char 	XERCV$dst	[6];
      unsigned char 	XERCV$src	[6];
      unsigned short 	XERCV$type	;
      unsigned short 	XERCV$fill	;
    };
  };
};

#define  XERCV_len   $Field_set_size
#if 0
MACRO XERCV_structure = BLOCK[XESND_len] FIELD(XE_recv)%;
#endif
