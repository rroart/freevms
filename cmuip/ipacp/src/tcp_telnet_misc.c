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
//TITLE "TCP_TELNET_MISC - TELNET Virtual Terminal service support routines"
/*
Facility:

	TCP_TELNET - Run incoming TELNET service under TCP (RFC 854)

Abstract:

	A collection of routines which support the TELNET module.
	Includes option-specific code, etc...

Author:

1.0	02-Feb-1989	Bruce R. Miller		CMU Network Development

Module Modification History:

1.1	13-Nov-1991	John M. Clement (JC)		Rice University
	Added many terminal types and fixed the WINDOW_SIZE and TERMINAL_TYPE
	setting routines.  Also removed all calls to SIGNAL
	Calls to SIGNAL are forbidded if we run under IPACP.

	The Terminal name is now parsed by preceding it by "*"
	and doing a wild search.  This allows syntax as VT240 or DEC-VT240
	Indeed even 240 will be understood.

	Set_DEVDEP added to turn on/off 8-bit, PASSALL

	When Terminal type is received only ask for new one if current one
	is different from last one or after 6 tries.

	Fixed a problem in Timing_mark_ON Referred to REMOPTS instead of
	LCLOPTS (JC)
*/

#if 0
MODULE TELNET_MISC(IDENT="1.0",LANGUAGE(BLISS32),
	      ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			      NONEXTERNAL=LONG_RELATIVE),
	      LIST(REQUIRE,ASSEMBLY,OBJECT,BINARY),
	      OPTIMIZE,OPTLEVEL=3,ZIP)=
#endif

#include<descrip.h> 

  // not yet#include "SYS$LIBRARY:LIB";		// JC
//LIBRARY "SYS$LIBRARY:STARLET";
  // not yet #include "CMUIP_SRC:[CENTRAL]NETXPORT";	// BLISS common definitions
#include "telnet.h"		// TELNET protocol definitions
#include "tcpmacros.h"		// Local (tcp) Macro defintions.

#include<ssdef.h>
#include"netvms.h"
#include<ttdef.h>
#include<tt2def.h>
#include<iodef.h>
#include<iosbdef.h>



//++
// This macro describes the terminal translation table entries.
//--
//  Modified by J Clement		JC
//	Changed table structure to include the Device dependent terminal
//	This has been modified to include all known DEC terminal types.
//	Characteristics.
struct ttab$entry
{
  void *    TTab$L_NAME;
  long    TTab$W_TYPE;
  long    TTab$L_DEVDEP;
  long    TTab$L_DEVDEP2;
    };

#define    TTAB$K_SIZE sizeof(struct ttab$entry)
#define    TTAB$K_TABLE_SIZE	39
#if 0
LITERAL
    TTab$K_Size	= $Field_Set_Size,
MACRO
    TTab$Entry = BLOCK->TTab$K_Size FIELD(TTAB$FIELDS)%;
#endif

static struct dsc$descriptor dscs[TTAB$K_TABLE_SIZE+1] = { {0, 0},
							   ASCID2(7,"UNKNOWN"),
							   ASCID2(7,"DEC-FT1"),
							   ASCID2(7,"DEC-FT2"),
							   ASCID2(7,"DEC-FT3"),
							   ASCID2(7,"DEC-FT4"),
							   ASCID2(7,"DEC-FT5"),
							   ASCID2(7,"DEC-FT6"),
							   ASCID2(7,"DEC-FT7"),
							   ASCID2(7,"DEC-FT8"),
							   ASCID2(10,"DEC-TQ_BTS"),	// ????
							   ASCID2(8,"DEC-VT05"),
							   ASCID2(9,"DEC-VK100"),
							   ASCID2(3,"TEK"),
							   ASCID2(7,"DEC-LAX"),
							   ASCID2(8,"DEC-LA34"),
							   ASCID2(8,"DEC-LA36"),
							   ASCID2(8,"DEC-LA38"),
							   ASCID2(8,"DEC-VT5X"),
							   ASCID2(8,"DEC-VT50"),
							   ASCID2(8,"DEC-VT52"),
							   ASCID2(8,"DEC-VT55"),
							   ASCID2(8,"DEC-VT80"),
							   ASCID2(9,"DEC-VT100"),
							   ASCID2(9,"DEC-VT101"),
							   ASCID2(9,"DEC-VT102"),
							   ASCID2(9,"DEC-VT105"),
							   ASCID2(9,"DEC-VT125"),
							   ASCID2(9,"DEC-VT131"),
							   ASCID2(9,"DEC-VT132"),
							   ASCID2(9,"DEC-VT132"),
							   ASCID2(9,"DEC-VT173"),
							   ASCID2(6,"DECPRO"),
							   ASCID2(9,"DEC-VT200"),
							   ASCID2(9,"DEC-VT220"),
							   ASCID2(9,"DEC-VT240"),
							   ASCID2(9,"DEC-VT300"),
							   ASCID2(9,"DEC-VT320"),
							   ASCID2(9,"DEC-VT340"),
							   ASCID2(9,"DEC-VT400") };

static unsigned long
Term_Table [4*TTAB$K_TABLE_SIZE+1]={
TTAB$K_TABLE_SIZE,
TTAB$K_SIZE,
	0,
	0,
&dscs[1],
TT$_UNKNOWN,
0,
0,
&dscs[2],
TT$_FT1,
0,
0,
&dscs[3],
TT$_FT2,
0,
0,
&dscs[4],
TT$_FT3,
0,
0,
&dscs[5],
TT$_FT4,
0,
0,
&dscs[6],
TT$_FT5,
0,
0,
&dscs[7],
TT$_FT6,
0,
0,
&dscs[8],
TT$_FT7,
0,
0,
&dscs[9],
TT$_FT8,
0,
0,
&dscs[10],	// ????
TT$_TQ_BTS,
0,
0,
&dscs[11],
TT$_VT05,
0
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_LFFILL
			+TT$M_SCOPE,
0,
&dscs[12],
TT$_VK100,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_DECCRT,
&dscs[13],
TT$_TEK401X,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_MECHFORM,
0,
&dscs[14],
TT$_LAX,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_MECHFORM,
0,
&dscs[15],
TT$_LA34,
0
			+TT$M_LOWER
			+TT$M_WRAP
			+TT$M_MECHFORM,
0,
&dscs[16],
TT$_LA36,
0
			+TT$M_LOWER
			+TT$M_WRAP,
0,
&dscs[17],
TT$_LA38,
0
			+TT$M_LOWER
			+TT$M_WRAP
			+TT$M_MECHFORM,
0,
&dscs[18],
TT$_VT5X,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0,
&dscs[19],
TT$_VT5X,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0,
&dscs[20],
TT$_VT52,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0,
&dscs[21],
TT$_VT55,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0,
&dscs[22],
TT$_VT80,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
//!!Optional		+TT2$M_AVO
			+TT2$M_DECCRT,
&dscs[23],
TT$_VT100,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
//!!Optional		+TT2$M_AVO
			+TT2$M_DECCRT,
&dscs[24],
TT$_VT101,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
			+TT2$M_DECCRT,
&dscs[25],
TT$_VT102,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_PRINTER
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
&dscs[26],
TT$_VT105,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
//!!Optional		+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
&dscs[27],
TT$_VT125,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_SIXEL
			+TT2$M_PRINTER
			+TT2$M_APP_KEYPAD
			+TT2$M_ANSICRT
			+TT2$M_REGIS
			+TT2$M_AVO
			+TT2$M_DECCRT,
&dscs[28],
TT$_VT131,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
			+TT2$M_BLOCK
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
&dscs[29],
TT$_VT132,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
			+TT2$M_BLOCK
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
&dscs[30],
TT$_VT132,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
			+TT2$M_BLOCK
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
&dscs[31],
TT$_VT173,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_BLOCK
			+TT2$M_AVO
			+TT2$M_EDIT,
&dscs[32],
TT$_PRO_SERIES,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_PRINTER
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
&dscs[33],
TT$_VT200_SERIES,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2,
&dscs[34],
TT$_VT200_SERIES,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2,
&dscs[35],
TT$_VT200_SERIES,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_REGIS
			+TT2$M_SIXEL
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2,
&dscs[36],
TT$_VT300_SERIES,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_PRINTER
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2
			+TT2$M_DECCRT3,
&dscs[37],
TT$_VT300_SERIES,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_PRINTER
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2
			+TT2$M_DECCRT3,
&dscs[38],
TT$_VT300_SERIES,
0
			+TT2$M_REGIS
			+TT2$M_SIXEL
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_PRINTER
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2
			+TT2$M_DECCRT3,
&dscs[39],
TT$_VT400_SERIES,
0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
0
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2
			+TT2$M_DECCRT3
			+TT2$M_DECCRT4
};



Print (control_string, P1)
    {
    extern
	Send_2_Operator();
    signed long
	Status;
    unsigned char Out_Buffer[256];
    unsigned char Out_Desc_d[8];
    struct dsc$descriptor * Out_Desc=&Out_Desc_d;

    Out_Desc->dsc$b_class = DSC$K_CLASS_Z;
    Out_Desc->dsc$b_dtype = DSC$K_DTYPE_Z;
    Out_Desc->dsc$w_length = sizeof (Out_Buffer);
    Out_Desc->dsc$a_pointer = Out_Buffer;
    Status = exe$faol ( // check
	 control_string,
	 Out_Desc->dsc$w_length,
	 Out_Desc,
	 P1);
    if (! Status) return Status;
    return Send_2_Operator(Out_Desc);
    }



void Line_Changed_AST (Parm)
//--
// Functional Description:
//
// 	Called whenever the TW's characteristics are changed by the child proc
//--
    {
      extern
	TVT_SEND_SUBOP();
    signed long
	data =1,
	RC;
    struct _qcbdef PTY_Char;
    struct _iosb io_stats;
      struct TVT$BLOCK *TVT = Parm;
      union _ttdef * Old_Chars = &TVT->TVT$TTYDEPEND;
      union _ttdef * Charistics	= &PTY_Char.QCB$L_CHARISTICS;
      union _tt2def * Extend_Char	= &PTY_Char.QCB$L_EXTEND_CHAR;	// JC

    // Make sure the TVT is still valid
    if (TVT->TVT$CANCEL)
	return;

	RC = exe$qiow (0,
	 TVT->TVT$PTY_CHN,
	 IO$_SENSEMODE,
	io_stats,
		       0,
		       0,
	PTY_Char,
	QCB$K_SIZE,
		       0,0,0,0);
    if (! RC)
        {
            XLOG$FAO(LOG$TELERR
		,"!%T Set_LINE_Change_AST: SENSEMODE status=!UL!/",0,RC);
	}
    else if (! io_stats.iosb$w_status)
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_LINE_Change_AST: SENSEMODE status=!UL!/"
		,0,io_stats.iosb$w_status);
	}
    else
    {
	if (Charistics->TT$V_TTSYNC != Old_Chars->TT$V_TTSYNC)
	    TVT_SEND_SUBOP(TVT,Telnet$K_Toggle_Flow_Control,
		%REF(IF (Charistics->TT$V_TTSYNC)
			THEN Option$K_Tog_Flow_Cntl_ON
			else Option$K_Tog_Flow_Cntl_OFF),
		1);
	Old_Chars = Charistics;
	TVT->TVT$TTYDEPEND2 = Extend_Char;
	};

	RC = exe$qiow (
0,
		TVT->TVT$PTY_CHN,
		IO$_SETMODE,
		io_stats,
  0,
  0,
		Line_Changed_AST,
		TVT,
		0,
		3,
0,
0);
	if (! RC)
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_LINE_Change_AST: SETMODE status=!UL!/",0,RC);
	}
	else if (! io_stats.iosb$w_status)
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_LINE_Change_AST: SETMODE status=!UL!/"
		,0,io_stats.iosb$w_status);
	};
    }


IS_CNTRLT_GOOD (TVT)
//--
// Functional Description:
//
//	Returns 1 if process will respond to ^T with status information.
//
//	Right now it checks to see if the term is in Pasthru/ALL or TTSYNC
//	mode which usually means I'm in EMACS, the only thing I know of
//	Also TELNET !!!!!!!
//	that eats ^T's.
//--
	struct TVT$BLOCK * TVT;
    {
      struct _iosb io_stats;
      struct _qcbdef PTY_Char;
      union _ttdef * Charistics = &PTY_Char.QCB$L_CHARISTICS;
      union _tt2def * Extend_Char = &PTY_Char.QCB$L_EXTEND_CHAR;   // JC

    signed long
	RC,
	Status;

    RC = exe$qiow (
0,
	TVT->TVT$PTY_CHN,
	IO$_SENSEMODE,
	io_stats,
	PTY_Char,
	QCB$K_SIZE,
	  0,0,0,0);
    return ! (
	(Charistics->tt$v_ttsync == 0) ||
	(Charistics->tt$v_passall == 1) ||
	(Extend_Char->tt2$v_pasthru == 1)
	);
    }



void Timing_Mark_On(TVT)
//--
// Functional Description:
//
//	The TM option has been turned on.  Turn it right back off.
//	In the future we shouldn't respond until we think the process
//	has, um, well, processed all of the data before the TM.
//++
	struct TVT$BLOCK * TVT;
    {
    struct OPT$BLOCK *
	LCLOPTS = &TVT->TVT$LCLOPTS;

    LCLOPTS[Telnet$K_Timing_Mark].OPT$STATE = OPT$STATE_OFF;

    }



void Set_PTY_Window_Size (TVT, pag, width)
//--
// Functional Description:
//
//	Set the terminal mode to reflect the new size.
//--
	struct TVT$BLOCK * TVT;
    {
      long pty_chan	= TVT->TVT$PTY_CHN;
      struct _qcbdef PTY_Char;
      union _ttdef * Charistics = &PTY_Char.QCB$L_CHARISTICS;
      union _tt2def * Extend_Char = &PTY_Char.QCB$L_EXTEND_CHAR;   // JC

    struct _iosb io_stats;
    signed long
	Status;

    Status = exe$qiow (
0,
	PTY_CHAN,
	IO$_SENSEMODE,
	io_stats,
  0,0,
	PTY_Char,
	QCB$K_SIZE,
  0,0,0,0);
    if (! Status)
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Window_size: SENSEMODE status=!UL!/",0,status);
	}
    else if (! io_stats.iosb$w_status)
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Window_size: SENSEMODE status=!UL!/"
		,0,io_stats.iosb$w_status);
	}
    else
    {
	if (Width > 0)
		PTY_Char->QCB$W_PAGE_WIDTH = width;
	if (Pag > -1)
		PTY_Char->QCB$B_PAGE_LENGTH = pag;
	Status = exe$qiow (
0,
		PTY_Chan,
		IO$_SETMODE,
		io_stats,
  0,0,
		PTY_Char,
		QCB$K_SIZE,
  0,0,0,0);
	if (! Status)
	{
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Window_size: SETMODE status=!UL!/",0,status);
	}
	else if (! io_stats.iosb$w_status)
	{
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Window_size: SETMODE status=!UL!/",
		0,io_stats.iosb$w_status);
	}
    }
    return SS$_NORMAL;
    }

void Window_Size_Sub (TVT)
//++
// Functional Description:
//
//	We've received new window size information.
//--
	struct TVT$BLOCK * TVT;
    {
    signed long
	Width,
	Length;

    if (TVT->TVT$SUB_CNT LSS 4) return;

    Width = 255 * CH$RCHAR_A(TVT->TVT$SUB_PTR);
    Width = Width + CH$RCHAR_A(TVT->TVT$SUB_PTR);

    Length = 255 * CH$RCHAR_A(TVT->TVT$SUB_PTR);
    Length = Length + CH$RCHAR_A(TVT->TVT$SUB_PTR);

    if ((Width == 0) || (Length == 0)) return(SS$_NORMAL);

    Set_PTY_Window_Size(TVT, Length, Width);

    SS$_NORMAL
    }


void Set_PTY_Term_Type (TVT, type, devdep) (void)
//--
// Functional Description:
//
//	Set the terminal mode to reflect the new size.
//--
//	Rewritten by JC to make it work
//
	struct VECTOR * devdep[2];
	struct TVT$BLOCK * TVT;
    {
      extern SET_DEVDEP();
      long pty_chan	= TVT->TVT$PTY_CHN;
      long tty_chan	= TVT->TVT$TTY_CHN;
      struct _qcbdef PTY_Char;
      union _ttdef * Charistics = &PTY_Char.QCB$L_CHARISTICS;
      union _tt2def * Extend_Char = &PTY_Char.QCB$L_EXTEND_CHAR;   // JC
	 LCLOPTS		= TVT->TVT$LCLOPTS : OPT$LIST,
	State_Echo	= LCLOPTS[Telnet$K_Echo].Opt$State,
	State_Binary	= LCLOPTS[Telnet$K_Binary].Opt$State,
	State_Eightbit	= LCLOPTS[Telnet$K_Extended_ASCII].Opt$State;
    struct _iosb io_stats;
    signed long
	Status	;

    Status = exe$qiow (
0,
	PTY_CHAN,
	IO$_SENSEMODE,
	io_stats,
  0,0,
	PTY_Char,
	QCB$K_SIZE,
0,0,0,0);
    if (! Status)
	{
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Term_Type: SENSEMODE status=!UL!/",0,status);
	}
    else if (! io_stats.iosb$w_status)
	{
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Term_Type: SENSEMODE status=!UL!/"
		,0,io_stats.iosb$w_status);

	}

    else
    {
	PTY_Char->QCB$B_TYPE = type;
//
//	Set the new terminal characteristics JC
//
	PTY_Char->QCB$L_CHARISTICS = (PTY_Char->QCB$L_CHARISTICS AND
		(NOT	(   TT$M_LOWER
			 || TT$M_EIGHTBIT
			 || TT$M_MECHFORM
			 || TT$M_MECHTAB
			 || TT$M_WRAP
			 || TT$M_SCOPE)))
		OR		devdep[0];
	PTY_Char[QCB$L_EXTEND_CHAR]= (PTY_Char[QCB$L_EXTEND_CHAR] AND
		(NOT	(   TT2$M_PRINTER
			 || TT2$M_REGIS
			 || TT2$M_SIXEL
//!!JC			 || TT2$M_PASTHRU
			 || TT2$M_ANSICRT
			 || TT2$M_BLOCK
			 || TT2$M_AVO
			 || TT2$M_EDIT
			 || TT2$M_DECCRT
			 || TT2$M_DECCRT2
			 || TT2$M_DECCRT3)))
	OR		devdep[1];

	Charistics [TT$V_EIGHTBIT] = State_Binary || State_Eightbit;
	Status = exe$qiow (
0,
		PTY_Chan,
		IO$_SETMODE,
		io_stats,
  0,0,
		PTY_Char,
		QCB$K_SIZE,
0,0,0,0);
	if (! Status)
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Term_Type: SETMODE status=!UL!/",
		0,status);
	}
	else if (! io_stats.iosb$w_status)
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Term_Type: SETMODE status=!UL!/",
		0,io_stats.iosb$w_status);
	};
	TVT->TVT$TTSET = 1;		// Set TTSET flag
	SET_DEVDEP(TVT);
    }
    SS$_NORMAL
    }

Void Terminal_Type_On (TVT) (void)
//++
// Functional Description:
//
//	We've agreed to talk about terminal types.
//--
    {
    EXTERNAL ROUTINE
	TVT_SEND_SUBOP;
    MAP
	struct TVT$BLOCK * TVT;

    // Ask for the terminal type.
    TVT_SEND_SUBOP(TVT,Telnet$K_Terminal_Type,
        %REF(Option$K_Terminal_Type_Send), 1);

    }

Void Window_Size_On (TVT) (void)
//++
// Functional Description:
//
//	We've agreed to talk about terminal types.
//--
//	Added by JC
    {
    EXTERNAL ROUTINE
	TVT_SEND_SUBOP;
    MAP
	struct TVT$BLOCK * TVT;

    // Ask for the terminal type.
    TVT_SEND_SUBOP(TVT,Telnet$K_Window_Size,
        %REF(Option$K_Terminal_Type_Send), 1);

    }

Lookup_terminal(Term_Desc, Devdep)
//++
// Functional Description:
//
//	Search the terminal name list to find a string that matches
//	term_desc.  If none found, return -1,
//	Otherwise return(SS$_NORMAL) and set Type_id = term type number.
//--
{
    MAP
        struct VECTOR * Devdep[2],
	Term_Desc	: REF Block[8,BYTE];
    EXTERNAL ROUTINE
	STR$UPCASE		: BLISS ADDRESSING_MODE(GENERAL),
	STR$TRIM		: BLISS ADDRESSING_MODE(GENERAL),
	STR$MATCH_WILD		: BLISS ADDRESSING_MODE(GENERAL),
	STR$CASE_BLIND_COMPARE	: BLISS ADDRESSING_MODE(GENERAL);
    signed long
	Buffer : VECTOR[32, BYTE],
	TYPDSC : BLOCK[8,BYTE],
	i,
	Status;

//
//	Setup buffer to hold input string
//
	Buffer[1]="*";
	TYPDSC->dsc$w_length = sizeof(Buffer)-2;
	TYPDSC->dsc$b_dtype = DSC$K_DTYPE_T;
	TYPDSC->dsc$b_class = DSC$K_CLASS_S;
	TYPDSC->dsc$a_pointer = Buffer+2;
//
//	Get the string and trim off blanks
//	Then Make it upper case
//
	Status = STR$TRIM(TYPDSC,Term_Desc,TYPDSC->dsc$w_length);
	Status = STR$UPCASE(TYPDSC,TYPDSC);
	TYPDSC->dsc$a_pointer = Buffer+1;
//
//	Now add the * to the string for a wild match
//
	TYPDSC->dsc$w_length = TYPDSC->dsc$w_length +1;
	Buffer[0] = TERM_DESC->dsc$w_length+1;
    INCR i FROM 1 TO Term_Table[0] DO
	{

//!!	Status =  STR$CASE_BLIND_COMPARE(
//
//	Make a wild match
//
	Status =  STR$MATCH_WILD(
			.Term_Table[Term_Table[1]*.i+0],
			TYPDSC);
	    if ((Status && 1) == 1)
	    {
		devdep[0] = Term_Table[Term_Table[1]*.i+2];
		devdep[1] = Term_Table[Term_Table[1]*.i+3];
		return(Term_Table[Term_Table[1]*.i+1]);
	    };
	};

    -1
    }

void Terminal_Type_Sub (TVT) 
	struct TVT$BLOCK * TVT;
    {
    extern
	LIB$_UNRKEY,
	LIB$_AMBKEY;
    extern
      STR$COPY_DX(),
      STR$CASE_BLIND_COMPARE(),
	TVT_SEND_SUBOP();
    BIND
	LCLOPTS = TVT->TVT$LCLOPTS : OPT$LIST,
	REMOPTS = TVT->TVT$REMOPTS : OPT$LIST,
	OPTBLK = (
		REMOPTS[Telnet$K_Terminal_type].OPT$BASE) : OPT$BLOCK;
    signed long
	Type_id  = %X"FFFF",
	devdep  : VECTOR[2],
	Status,
	OLDTYPDSC : BLOCK[DSC$K_Z_BLN,BYTE],
	TYPDSC : BLOCK[DSC$K_Z_BLN,BYTE];

    CH$RCHAR_A(TVT->TVT$SUB_PTR);
    TYPDSC->dsc$w_length = TVT->TVT$SUB_CNT - 1;	// current string
    TYPDSC->dsc$a_pointer = TVT->TVT$SUB_PTR;
    TYPDSC->dsc$b_dtype = DSC$K_DTYPE_T;
    TYPDSC->dsc$b_class = DSC$K_CLASS_S;
//!//    OLDTYPDSC->dsc$w_length = sizeof( TVT->TVT$TERMINAL_TYPE);
    OLDTYPDSC->dsc$w_length = 16;			// Saved terminal type
    OLDTYPDSC->dsc$a_pointer = TVT->TVT$TERMINAL_TYPE;
    OLDTYPDSC->dsc$b_dtype = DSC$K_DTYPE_T;
    OLDTYPDSC->dsc$b_class = DSC$K_CLASS_S;
    TVT->TVT$SUB_PTR = TVT->TVT$SUB_BUF;
    TVT->TVT$SUB_CNT = 0;

    Type_ID = Lookup_terminal(TYPDSC,devdep);	// Get terminal type

    if (Type_ID > 0)			// Is it OK ?
	{
//
//		Setup the device dependent bits
//
	    Set_PTY_Term_Type(TVT,type_id,devdep);
	}
    else IF	STR$CASE_BLIND_COMPARE(OLDTYPDSC,TYPDSC)	// Not same ?
		AND (TVT->TVT$KILL_TERMINAL_TYPE LEQU 6)	// Not 6 tries ?
		AND (Type_ID != 0)
	THEN {
	    TVT_SEND_SUBOP(TVT,Telnet$K_Terminal_Type,
		%REF(Option$K_Terminal_Type_Send), 1);
//!!	    OPTBLK->OPT$PREFER = OPT$DONT_CARE;
	};
	TVT->TVT$KILL_TERMINAL_TYPE = TVT->TVT$KILL_TERMINAL_TYPE+1;
	Status = STR$COPY_DX(OLDTYPDSC,TYPDSC);
    SS$_NORMAL
    }

Set_DEVDEP_DONE(TVT) : NOVALUE (void)
	struct TVT$BLOCK * TVT;
    {
	short int tty_chan	= &TVT->TVT$TTY_CHN;
    signed long
	Status;

	XLOG$FAO(LOG$TELNEG,"!%T Set_DEVDEP_Done:!/",0);
	if tty_chan == 0 then return(SS$_Normal);
	Status = $DASSGN (CHAN = tty_chan);
	if (! Status)
		    XLOG$FAO(LOG$TELERR
		    ,"!%T Set_DEVDEP_Done: DASSGN status=!UL!/",0,status);
	tty_chan = 0;
    ss$_normal
};

void Set_DEVDEP(TVT)
//--
// Functional Description:
//
//	Set the terminal device dependent characteristics
//	Added by JC
//--
	struct TVT$BLOCK * TVT;
    {
      struct _iosb io_stats;
    signed long
	Telnet_Passall,
	Changed,
	Unit_Number,
	devstr : VECTOR[20,BYTE],
	devnam : $BBLOCK->DSC$K_S_BLN PRESET (
		[dsc$w_length] = sizeof(devstr),
		[dsc$b_dtype] = DSC$K_DTYPE_Z,
		[dsc$b_class] = DSC$K_CLASS_Z,
		[dsc$a_pointer] = devstr),
	ptystr : VECTOR[20,BYTE],
	ptynam : $BBLOCK->DSC$K_S_BLN PRESET (
		[dsc$w_length] = sizeof(ptystr),
		[dsc$b_dtype] = DSC$K_DTYPE_Z,
		[dsc$b_class] = DSC$K_CLASS_Z,
		[dsc$a_pointer] = ptystr),
	Item_List		: $ITMLST_DECL (Items = 3),
	Status		: LONG;
	short * pty_chan	= &TVT->TVT$PTY_CHN,
	short * tty_chan	= &TVT->TVT$TTY_CHN,
	  struct _qcbdef * PTY_Char= &TVT->TVT$TTY_CHAR;
	union _ttdef * Charistics = PTY_Char->QCB$L_CHARISTICS;
	union _tt2def * Extend_Char = PTY_Char->QCB$L_EXTEND_CHAR;   // JC

	LCLOPTS		= TVT->TVT$LCLOPTS 		: OPT$LIST,
	Echo_opt	= LCLOPTS[Telnet$K_Echo].OPT$BASE : OPT$BLOCK,
	State_Echo	= LCLOPTS[Telnet$K_Echo].Opt$State,
	State_Binary	= LCLOPTS[Telnet$K_Binary].Opt$State,
	State_Eightbit	= LCLOPTS[Telnet$K_Extended_ASCII].Opt$State;

//
//	If user is not yet setup we must exit to prevent conflicts.
//
    if (TVT->TVT$DO_PID) return(0);

    Status = exe$qiow (
0,
		pty_chan,
		IO$_SENSEMODE,
		io_stats,
  0,0,
		PTY_char,
		QCB$K_SIZE,
0,0,0,0);

    IF ! Status
	THEN {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_DEVDEP: SENSEMODE PTY status=!UL!/",0,status);
	}

    else IF ! io_stats.iosb$w_status
	THEN {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_DEVDEP: SENSEMODE PTY status=!UL!/"
		,0,io_stats.iosb$w_status);
	}
    else {
	XLOG$FAO(LOG$TELNEG
		,"!%T Set_DEVDEP: Before Class=!UB Type=!UB Char:x!XL x!XL Size=!UW !UB!/",0
		,PTY_Char->QCB$B_Class
		,PTY_Char->QCB$B_Type
		,(PTY_Char->QCB$L_CHARISTICS && %X"FFFFFF")
		,PTY_Char[QCB$L_EXTEND_CHAR]
		,PTY_Char->QCB$W_PAGE_WIDTH
		,PTY_Char->QCB$B_PAGE_LENGTH);

	Telnet_passall = $TRNLOG(
		LOGNAM=%ASCID"TELNET_PASSALL",		// JC
		RSLLEN=ptynam->dsc$w_length,		// JC
		RSLBUF=ptynam);				// JC

	Changed = 0;
	Changed = TVT->TVT$TTSET;
	if Telnet_Passall == 1 THEN
	    Changed = Changed OR
		(Charistics [TT$V_PASSALL] XOR State_Binary);

	IF ! Echo_OPT->OPT$CURRENT		// Echo being negotiated ??
	THEN {
	    Changed = Changed OR
		(Charistics [TT$V_NOECHO] XOR (! State_Echo));
	    Charistics [TT$V_NOECHO] = 1 && (! State_Echo);
	    };


	XLOG$FAO(LOG$TELNEG
		,"!%T Set_DEVDEP: Changed=x!XL!/",0
		,Changed);

	If	! (Changed OR
		(Charistics->TT$V_EIGHTBIT XOR
			(State_Binary || State_Eightbit)))
		THEN return(SS$_NORMAL);	// No changes to be made

	Charistics [TT$V_EIGHTBIT] = State_Binary || State_Eightbit;
	if Telnet_Passall == 1 THEN
		Charistics [TT$V_PASSALL] = State_Binary;

        TVT[TVT$TTYDEPEND] =  PTY_Char->QCB$L_CHARISTICS;
        TVT[TVT$TTYDEPEND2] = PTY_Char[QCB$L_EXTEND_CHAR];
	XLOG$FAO(LOG$TELNEG
		,"!%T Set_DEVDEP: After  Class=!UB Type=!UB Char:x!XL x!XL Size=!UW !UB!/",0
		,PTY_Char->QCB$B_Class
		,PTY_Char->QCB$B_Type
		,(PTY_Char->QCB$L_CHARISTICS && %X"FFFFFF")
		,PTY_Char[QCB$L_EXTEND_CHAR]
		,PTY_Char->QCB$W_PAGE_WIDTH
		,PTY_Char->QCB$B_PAGE_LENGTH);

	Status = exe$qiow (
0,
		pty_chan,
		IO$_SETMODE,
		io_stats,
  0, 0,
		PTY_char,
		QCB$K_SIZE,
0,0,0,0);

	IF ! Status
	THEN {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_DEVDEP: SETMODE PTY status=!UL!/",0,status);
	}

	else IF ! io_stats.iosb$w_status
	THEN {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_DEVDEP: SETMODE PTY status=!UL!/",
		0,io_stats.iosb$w_status);
	}
    }

    TVT->TVT$TTSET = Changed;
    if (! Changed) return(SS$_Normal);

//
//	If we changed some vital stuff, change it through front end.
//	JC
//

    if (tty_chan != 0) return(SS$_Normal);		// Front end in use ?
//
//	We find the correct terminal by:
//	1.	Get unit number
//	2.	Get Front end device name
//	3.	Concatonate name and number
//	4.	Then attach to it.

	devnam->dsc$a_pointer = TVT->TVT$TTY_DEVSTR;

	$ITMLST_INIT (ITMLST = Item_List,
		(ITMCOD = DVI$_UNIT, BUFADR = Unit_Number));
	Status = $GETDVIW (ITMLST = Item_List
			,CHAN=.PTY_chan);		// Get unit

//!!JC	XLOG$FAO(LOG$TELNEG
//!!JC		,"!%T Set_DEVDEP: Unit_Number=x!XL!/",0
//!!JC		,Unit_Number);

	ptynam->dsc$w_length = sizeof(ptystr);
	Status = $TRNLOG(
		LOGNAM=%ASCID"INET$PTY_TERM",		// JC
		RSLLEN=ptynam->dsc$w_length,		// JC
		RSLBUF=ptynam);				// JC

	Status = Status == SS$_NORMAL;
	if (Status)
	    Status = $FAO(%ASCID"_!ASA!UL:",devnam,devnam,ptynam,Unit_Number);
	if (Status)
		Status = $GETDVIW (ITMLST = Item_List, DEVNAM = devnam);

//!!JC	xlog$fao(LOG$TELNEG
//!!JC			,"!%T Set_DEVDEP: TTY_TERM == "!AS""
//!!JC			,0,devnam);			// Concatonate

	Status = $ASSIGN( DEVNAM=devnam
			,CHAN=tty_chan);		// Open new channel


	IF ! Status					// No channel ?
	    THEN {
		tty_chan = 0;
		XLOG$FAO(LOG$TELERR
		,"!%T Set_DEVDEP: ASSIGN status=!UL!/",0,status);
	    }
	    else {
		Status = $QIO (
0,
			tty_chan,
			IO$_SETMODE,
  0,
			Set_DEVDEP_done,
		        TVT,
			PTY_char,
			QCB$K_SIZE,
0,0,0,0);		// Set front end
		IF ! Status
		THEN {
		    XLOG$FAO(LOG$TELERR
			,"!%T Set_DEVDEP: SETMODE TTY status=!UL!/",0,status);
		    Status = $DASSGN (CHAN = tty_chan);
		    tty_chan = 0;
		};
	    };

	TVT->TVT$TTSET = 0;			// True if characteristics changed
//!!JC	    XLOG$FAO(LOG$TELNEG
//!!JC		,"!%T Set_DEVDEP: tty_chan=!UL!/",0,tty_chan);
//!!JC	    XLOG$FAO(LOG$TELNEG
//!!JC		,"!%T Set_DEVDEP: pty_chan=!UL!/",0,PTY_chan);

    SS$_NORMAL
    }


Void LineMode_Sub (TVT) (void)
    {
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	b1,b2;

    // Do we have enough data
    if (TVT->TVT$SUB_CNT LSS 2) return;

    // What kind of sub-option is it?
    b1 = CH$RCHAR_A(TVT->TVT$SUB_PTR);

    !++
    // What kind of suboption is it?
    !--
/*    if (b1 == Option$K_LineMode_Mode)
        Linemode_SubOp_Mode (CH$RCHAR_A(TVT->TVT$SUB_PTR))
    else if (b1 == Option$K_LineMode_SLC)
        Linemode_SubOp_SLC (TVT->TVT$SUB_PTR, TVT->TVT$SUB_CNT-1)
    else
	{
	b2 = CH$RCHAR_A(TVT->TVT$SUB_PTR);
	if (b2 == Option$K_LineMode_Forwardmask)
	    Linemode_SubOp_Forwardmask(b1,
		TVT->TVT$SUB_PTR,TVT->TVT$SUB_CNT-2)
	}
*/
    SS$_NORMAL
    }


/*
Linemode_SubOp_Mode ( Mask )
    {
	extern Send_TCP_SubOption();
    signed long
	Mode_EDIT,Mode_TRAPSIG;
	signed char New_Mask;
	signed char SubOption_Data[2],
	struct dsc$descriptor SubOption_Desc	= {
				dsc$w_length	: 2,
				dsc$b_dtype	: DSC$K_DTYPE_T,
				dsc$b_class	: DSC$K_CLASS_D,
				dsc$a_pointer	: SubOption_Data};

    // If the ACK bit is set, don't do anything.
    if (Mask<Option$K_LineMode_MODE_ACK,1>) return SS$_NORMAL;

    Mask = Mask && 3;	// mask the mask.

    // Is the new mask different?
    if (Mask != Linemode_Modeflags)
	{
	Linemode_Modeflags = Mask;
	Mask<Option$K_Linemode_Mode_EDIT_ACK,1> = 1;
	SubOption_Data[0] = Option$K_Linemode_Mode;
	SubOption_Data[1] = Mask;
	!!!HACK!!// ???
	Send_TCP_SubOption ( Option$K_Linemode , SubOption_Desc )
	};

    return SS$_NORMAL;
    }


Linemode_SubOp_Forwardmask ( Action , Data_A , Length )
    {
	char * Data = Data_A;
    signed long
	SrcI  = 0,
	DstI  = 0;
    BIND
	Curr_FMask = Forward_Mask : VECTOR[32,BYTE];

    switch (Action)
	{
	case Telnet$K_DO :
	    {
//!!HACK!!// Should check for double IACs? Should check for Binary mode?
	    while (SrcI < Length)
		{
		Curr_FMask[DstI] = Data[SrcI];
		DstI = DstI + 1;
		SrcI = SrcI + 1
	 	}
	    Linemode_FM_State = Option$K_State_On;
	    };
	case Telnet$K_DONT :
	    Linemode_FM_State = Option$K_State_Off;
//	[Telnet$K_WILL].Telnet$K_WONT :
	};

    return SS$_NORMAL;
    }



Process_SLC(Code,Modifier,Value,Reply_Desc)
	struct dsc$descriptor * Reply_Desc	;
    {
    signed long
	Level,
	struct $BBlock * Char_Def->SLC$K_Size;

    if ((Code LSS 0) || (Code > Option$K_Limemode_SLC_MaxOpt))
	return SS$_NORMAL;

    Level = Modifier<SLC$F_Level>;
    Char_Def = SLC_Table[Code,SLC$B_{ING] ;

    // 1) Check if the new and old values are the same.
    if ((Value == Char_Def->SLC$B_CURRENT)) return(SS$_NORMAL);

    // 2) Same level / different value
    IF ((Level == (Char_Def->SLC$B_STATUS)<SLC$F_Level>) AND
	(Modifier<SLC$F_Ack)) THEN
	{
	Char_Def->SLC$B_CURRENT = Value;
	return SS$_NORMAL
	};

    // 3) "Do we agree?"
    // !!!HACK!!//  what do I do here?
    // Hardwired/specific/arbitrary code?

    // 4) Yes, we agree.
    Modifier<SLC$F_Ack> = 1;
    Append_Char(Reply_Desc,Code);
    Append_Char(Reply_Desc,Modifier);
    Append_Char(Reply_Desc,Value);

    return SS$_NORMAL;
    }

Linemode_SubOp_SLC(Data_A, Length)
    {
    char * data = Data_A;

	struct dsc$descriptor Reply_Desc=	 {
				dsc$w_length	: 0,
				dsc$b_dtype	: DSC$K_DTYPE_T,
				dsc$b_class	: DSC$K_CLASS_D,
				dsc$a_pointer	: 0};

    while (LENGTH >= 3)
	{
	Process_SLC(Data[0], Data[1], Data[2], &Reply_Desc);
	Length = Length - 3;
	Data = Data[3]
	};

    if (Reply_Desc->dsc$w_length > 0)
	Send_TCP_SubOpt ( Option$K_Linemode , &Reply_Desc );

    return SS$_NORMAL
    }
*/


