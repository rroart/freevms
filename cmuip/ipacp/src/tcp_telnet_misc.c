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

MODULE TELNET_MISC(IDENT="1.0",LANGUAGE(BLISS32),
	      ADDRESSING_MODE(EXTERNAL=LONG_RELATIVE,
			      NONEXTERNAL=LONG_RELATIVE),
	      LIST(REQUIRE,ASSEMBLY,OBJECT,BINARY),
	      OPTIMIZE,OPTLEVEL=3,ZIP)=
{

#include "SYS$LIBRARY:LIB";		// JC
!LIBRARY "SYS$LIBRARY:STARLET";
#include "CMUIP_SRC:[CENTRAL]NETXPORT";	// BLISS common definitions
#include "TELNET";		// TELNET protocol definitions
#include "TCPMacros";		// Local (tcp) Macro defintions.



!++
// This macro describes the terminal translation table entries.
!--
//  Modified by J Clement		JC
!	Changed table structure to include the Device dependent terminal
!	This has been modified to include all known DEC terminal types.
!	Characteristics.
$FIELD TTAB$FIELDS (void)
    SET
    TTab$L_NAME		= [$ADDRESS],
    TTab$W_TYPE		= [$BYTES(4)],
    TTab$L_DEVDEP	= [$BYTES(4)],
    TTab$L_DEVDEP2	= [$BYTES(4)]
    TES;

LITERAL
    TTab$K_Size	= $Field_Set_Size,
    TTab$K_Table_Size	= 39;
MACRO
    TTab$Entry = BLOCK->TTab$K_Size FIELD(TTAB$FIELDS)%;

static signed long
    Term_Table :	VECTOR[(TTab$K_Size+1)*TTab$K_Table_Size, LONG]
	PRESET (
	[0]  = TTab$K_Table_Size,
	[1]  = TTab$K_Size,
	[1*TTAB$K_Size+0] = %ASCID"UNKNOWN",
		[1*TTab$K_Size+1] = TT$_UNKNOWN,
		[1*TTab$K_Size+2] = 0,
		[1*TTab$K_Size+3] = 0,
	[2*TTAB$K_Size+0] = %ASCID"DEC-FT1",
		[2*TTab$K_Size+1] = TT$_FT1,
		[2*TTab$K_Size+2] = 0,
		[2*TTab$K_Size+3] = 0,
	[3*TTAB$K_Size+0] = %ASCID"DEC-FT2",
		[3*TTab$K_Size+1] = TT$_FT2,
		[3*TTab$K_Size+2] = 0,
		[3*TTab$K_Size+3] = 0,
	[4*TTAB$K_Size+0] = %ASCID"DEC-FT3",
		[4*TTab$K_Size+1] = TT$_FT3,
		[4*TTab$K_Size+2] = 0,
		[4*TTab$K_Size+3] = 0,
	[5*TTAB$K_Size+0] = %ASCID"DEC-FT4",
		[5*TTab$K_Size+1] = TT$_FT4,
		[5*TTab$K_Size+2] = 0,
		[5*TTab$K_Size+3] = 0,
	[6*TTAB$K_Size+0] = %ASCID"DEC-FT5",
		[6*TTab$K_Size+1] = TT$_FT5,
		[6*TTab$K_Size+2] = 0,
		[6*TTab$K_Size+3] = 0,
	[7*TTAB$K_Size+0] = %ASCID"DEC-FT6",
		[7*TTab$K_Size+1] = TT$_FT6,
		[7*TTab$K_Size+2] = 0,
		[7*TTab$K_Size+3] = 0,
	[8*TTAB$K_Size+0] = %ASCID"DEC-FT7",
		[8*TTab$K_Size+1] = TT$_FT7,
		[8*TTab$K_Size+2] = 0,
		[8*TTab$K_Size+3] = 0,
	[9*TTAB$K_Size+0] = %ASCID"DEC-FT8",
		[9*TTab$K_Size+1] = TT$_FT8,
		[9*TTab$K_Size+2] = 0,
		[9*TTab$K_Size+3] = 0,
	[10*TTAB$K_Size+0] = %ASCID"DEC-TQ_BTS",	// ????
		[10*TTab$K_Size+1] = TT$_TQ_BTS,
		[10*TTab$K_Size+2] = 0,
		[10*TTab$K_Size+3] = 0,
	[11*TTAB$K_Size+0] = %ASCID"DEC-VT05",
		[11*TTab$K_Size+1] = TT$_VT05,
		[11*TTab$K_Size+2] = 0
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_LFFILL
			+TT$M_SCOPE,
		[11*TTab$K_Size+3] = 0,
	[12*TTAB$K_Size+0] = %ASCID"DEC-VK100",
		[12*TTab$K_Size+1] = TT$_VK100,
		[12*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[12*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_DECCRT,
	[13*TTAB$K_Size+0] = %ASCID"TEK",
		[13*TTab$K_Size+1] = TT$_TEK401X,
		[13*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_MECHFORM,
		[13*TTab$K_Size+3] = 0,
	[14*TTAB$K_Size+0] = %ASCID"DEC-LAX",
		[14*TTab$K_Size+1] = TT$_LAX,
		[14*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_MECHFORM,
		[14*TTab$K_Size+3] = 0,
	[15*TTAB$K_Size+0] = %ASCID"DEC-LA34",
		[15*TTab$K_Size+1] = TT$_LA34,
		[15*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_WRAP
			+TT$M_MECHFORM,
		[15*TTab$K_Size+3] = 0,
	[16*TTAB$K_Size+0] = %ASCID"DEC-LA36",
		[16*TTab$K_Size+1] = TT$_LA36,
		[16*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_WRAP,
		[16*TTab$K_Size+3] = 0,
	[17*TTAB$K_Size+0] = %ASCID"DEC-LA38",
		[17*TTab$K_Size+1] = TT$_LA38,
		[17*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_WRAP
			+TT$M_MECHFORM,
		[17*TTab$K_Size+3] = 0,
	[18*TTAB$K_Size+0] = %ASCID"DEC-VT5X",
		[18*TTab$K_Size+1] = TT$_VT5X,
		[18*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[18*TTab$K_Size+3] = 0,
	[19*TTAB$K_Size+0] = %ASCID"DEC-VT50",
		[19*TTab$K_Size+1] = TT$_VT5X,
		[19*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[19*TTab$K_Size+3] = 0,
	[20*TTAB$K_Size+0] = %ASCID"DEC-VT52",
		[20*TTab$K_Size+1] = TT$_VT52,
		[20*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[20*TTab$K_Size+3] = 0,
	[21*TTAB$K_Size+0] = %ASCID"DEC-VT55",
		[21*TTab$K_Size+1] = TT$_VT55,
		[21*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[21*TTab$K_Size+3] = 0,
	[22*TTAB$K_Size+0] = %ASCID"DEC-VT80",
		[22*TTab$K_Size+1] = TT$_VT80,
		[22*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[22*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
!!!Optional		+TT2$M_AVO
			+TT2$M_DECCRT,
	[23*TTAB$K_Size+0] = %ASCID"DEC-VT100",
		[23*TTab$K_Size+1] = TT$_VT100,
		[23*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[23*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
!!!Optional		+TT2$M_AVO
			+TT2$M_DECCRT,
	[24*TTAB$K_Size+0] = %ASCID"DEC-VT101",
		[24*TTab$K_Size+1] = TT$_VT101,
		[24*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[24*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
			+TT2$M_DECCRT,
	[25*TTAB$K_Size+0] = %ASCID"DEC-VT102",
		[25*TTab$K_Size+1] = TT$_VT102,
		[25*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[25*TTab$K_Size+3] = 0
			+TT2$M_PRINTER
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
	[26*TTAB$K_Size+0] = %ASCID"DEC-VT105",
		[26*TTab$K_Size+1] = TT$_VT105,
		[26*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[26*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
!!!Optional		+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
	[27*TTAB$K_Size+0] = %ASCID"DEC-VT125",
		[27*TTab$K_Size+1] = TT$_VT125,
		[27*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[27*TTab$K_Size+3] = 0
			+TT2$M_SIXEL
			+TT2$M_PRINTER
			+TT2$M_APP_KEYPAD
			+TT2$M_ANSICRT
			+TT2$M_REGIS
			+TT2$M_AVO
			+TT2$M_DECCRT,
	[28*TTAB$K_Size+0] = %ASCID"DEC-VT131",
		[28*TTab$K_Size+1] = TT$_VT131,
		[28*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[28*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
			+TT2$M_BLOCK
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
	[29*TTAB$K_Size+0] = %ASCID"DEC-VT132",
		[29*TTab$K_Size+1] = TT$_VT132,
		[29*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[29*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
			+TT2$M_BLOCK
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
	[30*TTAB$K_Size+0] = %ASCID"DEC-VT132",
		[30*TTab$K_Size+1] = TT$_VT132,
		[30*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[30*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
			+TT2$M_BLOCK
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
	[31*TTAB$K_Size+0] = %ASCID"DEC-VT173",
		[31*TTab$K_Size+1] = TT$_VT173,
		[31*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[31*TTab$K_Size+3] = 0
			+TT2$M_BLOCK
			+TT2$M_AVO
			+TT2$M_EDIT,
	[32*TTAB$K_Size+0] = %ASCID"DECPRO",
		[32*TTab$K_Size+1] = TT$_Pro_Series,
		[32*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[32*TTab$K_Size+3] = 0
			+TT2$M_PRINTER
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT,
	[33*TTAB$K_Size+0] = %ASCID"DEC-VT200",
		[33*TTab$K_Size+1] = TT$_VT200_Series,
		[33*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[33*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2,
	[34*TTAB$K_Size+0] = %ASCID"DEC-VT220",
		[34*TTab$K_Size+1] = TT$_VT200_Series,
		[34*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[34*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2,
	[35*TTAB$K_Size+0] = %ASCID"DEC-VT240",
		[35*TTab$K_Size+1] = TT$_VT200_Series,
		[35*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[35*TTab$K_Size+3] = 0
			+TT2$M_REGIS
			+TT2$M_SIXEL
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2,
	[36*TTAB$K_Size+0] = %ASCID"DEC-VT300",
		[36*TTab$K_Size+1] = TT$_VT300_Series,
		[36*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[36*TTab$K_Size+3] = 0
			+TT2$M_PRINTER
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2
			+TT2$M_DECCRT3,
	[37*TTAB$K_Size+0] = %ASCID"DEC-VT320",
		[37*TTab$K_Size+1] = TT$_VT300_Series,
		[37*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[37*TTab$K_Size+3] = 0
			+TT2$M_PRINTER
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2
			+TT2$M_DECCRT3,
	[38*TTAB$K_Size+0] = %ASCID"DEC-VT340",
		[38*TTab$K_Size+1] = TT$_VT300_Series,
		[38*TTab$K_Size+2] = 0
			+TT2$M_REGIS
			+TT2$M_SIXEL
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[38*TTab$K_Size+3] = 0
			+TT2$M_PRINTER
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2
			+TT2$M_DECCRT3,
	[39*TTAB$K_Size+0] = %ASCID"DEC-VT400",
		[39*TTab$K_Size+1] = TT$_VT400_Series,
		[39*TTab$K_Size+2] = 0
			+TT$M_LOWER
			+TT$M_MECHTAB
			+TT$M_WRAP
			+TT$M_SCOPE,
		[39*TTab$K_Size+3] = 0
			+TT2$M_ANSICRT
			+TT2$M_AVO
			+TT2$M_EDIT
			+TT2$M_DECCRT
			+TT2$M_DECCRT2
			+TT2$M_DECCRT3
			+TT2$M_DECCRT4
	);



Print (Control_string, P1)=
    {
    EXTERNAL ROUTINE
	Send_2_Operator;
    signed long
	Out_Buffer	: VECTOR[256, BYTE],
	Out_Desc	: BLOCK[8, BYTE],
	Status;

    Out_Desc->DSC$B_CLASS = DSC$K_CLASS_Z;
    Out_Desc->DSC$B_DTYPE = DSC$K_DTYPE_Z;
    Out_Desc->DSC$W_LENGTH = %ALLOCATION (Out_Buffer);
    Out_Desc->DSC$A_POINTER = Out_Buffer;
    Status = $FAOL (
	CTRSTR = Control_String,
	OUTLEN = Out_Desc->DSC$W_LENGTH,
	OUTBUF = Out_Desc,
	PRMLST = P1);
    if (NOT Status) return Status;
    Send_2_Operator(Out_Desc)
    };



void Line_Changed_AST (Parm) (void)
!--
// Functional Description:
!
// 	Called whenever the TW's characteristics are changed by the child proc
!--
    {
    EXTERNAL ROUTINE
	TVT_S}_SUBOP;
    signed long
	PTY_Char	: $BBLOCK [QCB$K_Size],
	io_stats : BLOCK[8,BYTE],
	data : initial(1),
	RC		: LONG;
    BIND
	TVT = struct TVT$BLOCK * Parm,
	Old_Chars = TVT[TVT$TTYDEP}] : $BBLOCK,
	Charistics	= PTY_Char [QCB$L_Charistics]	: $BBLOCK,
	Extend_Char	= PTY_Char [QCB$L_EXT}_CHAR]	: $BBLOCK;	// JC

    // Make sure the TVT is still valid
    if (TVT->TVT$CANCEL)
	RETURN;

    RC = $QIOW (
	CHAN	= TVT->TVT$PTY_CHN,
	FUNC	= IO$_SENSEMODE,
	IOSB	= io_stats,
	P1	= PTY_Char,
	P2	= QCB$K_Size);
    if (NOT RC)
        {
            XLOG$FAO(LOG$TELERR
		,"!%T Set_LINE_Change_AST: SENSEMODE status=!UL!/",0,RC);
	}
    else if (NOT io_stats[0,0,16,0])
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_LINE_Change_AST: SENSEMODE status=!UL!/"
		,0,io_stats[0,0,16,0]);
	}
    else
    {
	if (Charistics->TT$V_TTSYNC != Old_Chars->TT$V_TTSYNC)
	    TVT_S}_SUBOP(TVT,Telnet$K_Toggle_Flow_Control,
		%REF(IF (Charistics->TT$V_TTSYNC)
			THEN Option$K_Tog_Flow_Cntl_ON
			else Option$K_Tog_Flow_Cntl_OFF),
		1);
	Old_Chars = Charistics;
	TVT[TVT$TTYDEP}2] = Extend_Char;
	};

	RC = $QIOW (
		CHAN	= TVT->TVT$PTY_CHN,
		FUNC	= IO$_SETMODE,
		IOSB	= io_stats,
		P1	= Line_Changed_AST,
		P2	= TVT,
		P3	= 0,
		P4	= 3);
	if (NOT RC)
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_LINE_Change_AST: SETMODE status=!UL!/",0,RC);
	}
	else if (NOT io_stats[0,0,16,0])
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_LINE_Change_AST: SETMODE status=!UL!/"
		,0,io_stats[0,0,16,0]);
	};
    };


IS_CNTRLT_GOOD (TVT)
!--
// Functional Description:
!
!	Returns 1 if process will respond to ^T with status information.
!
!	Right now it checks to see if the term is in Pasthru/ALL or TTSYNC
!	mode which usually means I'm in EMACS, the only thing I know of
!	Also TELNET !!!!!!!
!	that eats ^T's.
!--
    {
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	RC,
	io_stats : BLOCK[8,BYTE],
	PTY_Char	: $BBLOCK [QCB$K_Size],
	Status		: LONG;
    BIND
	Charistics	= PTY_Char [QCB$L_Charistics]	: $BBLOCK,
	Extend_Char	= PTY_Char [QCB$L_EXT}_CHAR]	: $BBLOCK;

    RC = $QIOW (
	CHAN	= TVT->TVT$PTY_CHN,
	FUNC	= IO$_SENSEMODE,
	IOSB	= io_stats,
	P1	= PTY_Char,
	P2	= QCB$K_Size);
    NOT (
	(Charistics->TT$V_TTSYNC == 0) OR
	(Charistics->TT$V_PASSALL == 1) OR
	(Extend_Char->TT2$V_PASTHRU == 1)
	)
    };



Timing_Mark_On(TVT) : NOVALUE (void)
!--
// Functional Description:
!
!	The TM option has been turned on.  Turn it right back off.
!	In the future we shouldn't respond until we think the process
!	has, um, well, processed all of the data before the TM.
!++
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	LCLOPTS = TVT->TVT$LCLOPTS : OPT$LIST;

    LCLOPTS[Telnet$K_Timing_Mark,OPT$STATE] = OPT$STATE_OFF;

    };



void Set_PTY_Window_Size (TVT, pag, width) (void)
!--
// Functional Description:
!
!	Set the terminal mode to reflect the new size.
!--
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	pty_chan	= TVT->TVT$PTY_CHN,
	PTY_Char	= TVT->TVT$TTY_CHAR : $BBLOCK,
	Charistics	= PTY_Char [QCB$L_Charistics]	: $BBLOCK,
	Extend_Char	= PTY_Char [QCB$L_EXT}_CHAR]	: $BBLOCK;
    signed long
	io_stats	: $BBLOCK [8],
	Status		: LONG;

    Status = $QIOW (
	CHAN	= PTY_CHAN,
	FUNC	= IO$_SENSEMODE,
	IOSB	= io_stats,
	P1	= PTY_Char,
	P2	= QCB$K_Size);
    if (NOT Status)
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Window_size: SENSEMODE status=!UL!/",0,status);
	}
    else if (NOT io_stats[0,0,16,0])
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Window_size: SENSEMODE status=!UL!/"
		,0,io_stats[0,0,16,0]);
	}
    else
    {
	if (Width > 0)
		PTY_Char->QCB$W_PAGE_WIDTH = width;
	if (Pag > -1)
		PTY_Char->QCB$B_PAGE_LENGTH = pag;
	Status = $QIOW (
		CHAN	= PTY_Chan,
		FUNC	= IO$_SETMODE,
		IOSB	= io_stats,
		P1	= PTY_Char,
		P2	= QCB$K_Size);
	if (NOT Status)
	{
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Window_size: SETMODE status=!UL!/",0,status);
	}
	else if (NOT io_stats[0,0,16,0])
	{
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Window_size: SETMODE status=!UL!/",
		0,io_stats[0,0,16,0]);
	}
    };
    SS$_NORMAL
    };

Void Window_Size_Sub (TVT) (void)
!++
// Functional Description:
!
!	We've received new window size information.
!--
    {
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	Width,
	Length;

    if (TVT->TVT$SUB_CNT LSS 4) RETURN;

    Width = 255 * CH$RCHAR_A(TVT->TVT$SUB_PTR);
    Width = Width + CH$RCHAR_A(TVT->TVT$SUB_PTR);

    Length = 255 * CH$RCHAR_A(TVT->TVT$SUB_PTR);
    Length = Length + CH$RCHAR_A(TVT->TVT$SUB_PTR);

    if ((Width == 0) || (Length == 0)) RETURN(SS$_NORMAL);

    Set_PTY_Window_Size(TVT, Length, Width);

    SS$_NORMAL
    };


void Set_PTY_Term_Type (TVT, type, devdep) (void)
!--
// Functional Description:
!
!	Set the terminal mode to reflect the new size.
!--
!	Rewritten by JC to make it work
!
    {
    EXTERNAL ROUTINE
	SET_DEVDEP;
    MAP
	struct VECTOR * devdep->2;
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	pty_chan	= TVT->TVT$PTY_CHN,
	tty_chan	= TVT->TVT$TTY_CHN,
	PTY_Char	= TVT->TVT$TTY_CHAR : $BBLOCK,
	Charistics	= PTY_Char [QCB$L_Charistics]	: $BBLOCK,
	Extend_Char	= PTY_Char [QCB$L_EXT}_CHAR]	: $BBLOCK,
	LCLOPTS		= TVT->TVT$LCLOPTS : OPT$LIST,
	State_Echo	= LCLOPTS[Telnet$K_Echo,Opt$State],
	State_Binary	= LCLOPTS[Telnet$K_Binary,Opt$State],
	State_Eightbit	= LCLOPTS[Telnet$K_Extended_ASCII,Opt$State];
    signed long
	io_stats	: $BBLOCK [8],
	Status		: LONG;

    Status = $QIOW (
	CHAN	= PTY_CHAN,
	FUNC	= IO$_SENSEMODE,
	IOSB	= io_stats,
	P1	= PTY_Char,
	P2	= QCB$K_Size);
    if (NOT Status)
	{
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Term_Type: SENSEMODE status=!UL!/",0,status);
	}
    else if (NOT io_stats[0,0,16,0])
	{
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Term_Type: SENSEMODE status=!UL!/"
		,0,io_stats[0,0,16,0]);

	}

    else
    {
	PTY_Char->QCB$B_TYPE = type;
!
!	Set the new terminal characteristics JC
!
	PTY_Char->QCB$L_CHARISTICS = (PTY_Char->QCB$L_CHARISTICS AND
		(NOT	(   TT$M_LOWER
			 || TT$M_EIGHTBIT
			 || TT$M_MECHFORM
			 || TT$M_MECHTAB
			 || TT$M_WRAP
			 || TT$M_SCOPE)))
		OR		devdep->0;
	PTY_Char[QCB$L_EXT}_CHAR]= (PTY_Char[QCB$L_EXT}_CHAR] AND
		(NOT	(   TT2$M_PRINTER
			 || TT2$M_REGIS
			 || TT2$M_SIXEL
!!!JC			 || TT2$M_PASTHRU
			 || TT2$M_ANSICRT
			 || TT2$M_BLOCK
			 || TT2$M_AVO
			 || TT2$M_EDIT
			 || TT2$M_DECCRT
			 || TT2$M_DECCRT2
			 || TT2$M_DECCRT3)))
	OR		devdep->1;

	Charistics [TT$V_EIGHTBIT] = State_Binary || State_Eightbit;
	Status = $QIOW (
		CHAN	= PTY_Chan,
		FUNC	= IO$_SETMODE,
		IOSB	= io_stats,
		P1	= PTY_Char,
		P2	= QCB$K_Size);
	if (NOT Status)
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Term_Type: SETMODE status=!UL!/",
		0,status);
	}
	else if (NOT io_stats[0,0,16,0])
        {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_PTY_Term_Type: SETMODE status=!UL!/",
		0,io_stats[0,0,16,0]);
	};
	TVT->TVT$TTSET = 1;		// Set TTSET flag
	SET_DEVDEP(TVT);
    };
    SS$_NORMAL
    };

Void Terminal_Type_On (TVT) (void)
!++
// Functional Description:
!
!	We've agreed to talk about terminal types.
!--
    {
    EXTERNAL ROUTINE
	TVT_S}_SUBOP;
    MAP
	struct TVT$BLOCK * TVT;

    // Ask for the terminal type.
    TVT_S}_SUBOP(TVT,Telnet$K_Terminal_Type,
        %REF(Option$K_Terminal_Type_Send), 1);

    };

Void Window_Size_On (TVT) (void)
!++
// Functional Description:
!
!	We've agreed to talk about terminal types.
!--
!	Added by JC
    {
    EXTERNAL ROUTINE
	TVT_S}_SUBOP;
    MAP
	struct TVT$BLOCK * TVT;

    // Ask for the terminal type.
    TVT_S}_SUBOP(TVT,Telnet$K_Window_Size,
        %REF(Option$K_Terminal_Type_Send), 1);

    };

Lookup_terminal(Term_Desc, Devdep)
!++
// Functional Description:
!
!	Search the terminal name list to find a string that matches
!	term_desc.  If none found, return -1,
!	Otherwise return(SS$_NORMAL) and set Type_id = term type number.
!--
{
    MAP
        struct VECTOR * Devdep->2,
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

!
!	Setup buffer to hold input string
!
	Buffer->1="*";
	TYPDSC->DSC$W_LENGTH = %ALLOCATION(Buffer)-2;
	TYPDSC->DSC$B_DTYPE = DSC$K_DTYPE_T;
	TYPDSC->DSC$B_CLASS = DSC$K_CLASS_S;
	TYPDSC->DSC$A_POINTER = Buffer+2;
!
!	Get the string and trim off blanks
!	Then Make it upper case
!
	Status = STR$TRIM(TYPDSC,Term_Desc,TYPDSC->DSC$W_LENGTH);
	Status = STR$UPCASE(TYPDSC,TYPDSC);
	TYPDSC->DSC$A_POINTER = Buffer+1;
!
!	Now add the * to the string for a wild match
!
	TYPDSC->DSC$W_LENGTH = TYPDSC->DSC$W_LENGTH +1;
	Buffer->0 = TERM_DESC->DSC$W_LENGTH+1;
    INCR i FROM 1 TO Term_Table->0 DO
	{

!!!	Status =  STR$CASE_BLIND_COMPARE(
!
!	Make a wild match
!
	Status =  STR$MATCH_WILD(
			.Term_Table[Term_Table->1*.i+0],
			TYPDSC);
	    if ((Status && 1) == 1)
	    {
		devdep->0 = Term_Table[Term_Table->1*.i+2];
		devdep->1 = Term_Table[Term_Table->1*.i+3];
		RETURN(Term_Table[Term_Table->1*.i+1]);
	    };
	};

    -1
    };

Void Terminal_Type_Sub (TVT) (void)
    {
    EXTERNAL
	LIB$_UNRKEY,
	LIB$_AMBKEY;
    EXTERNAL ROUTINE
	STR$COPY_DX		: BLISS ADDRESSING_MODE(GENERAL),
	STR$CASE_BLIND_COMPARE	: BLISS ADDRESSING_MODE(GENERAL),
	TVT_S}_SUBOP;
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	LCLOPTS = TVT->TVT$LCLOPTS : OPT$LIST,
	REMOPTS = TVT->TVT$REMOPTS : OPT$LIST,
	OPTBLK = (
		REMOPTS[Telnet$K_Terminal_type,OPT$BASE]) : OPT$BLOCK;
    signed long
	Type_id  = %X"FFFF",
	devdep  : VECTOR->2,
	Status,
	OLDTYPDSC : BLOCK[DSC$K_Z_BLN,BYTE],
	TYPDSC : BLOCK[DSC$K_Z_BLN,BYTE];

    CH$RCHAR_A(TVT->TVT$SUB_PTR);
    TYPDSC->DSC$W_LENGTH = TVT->TVT$SUB_CNT - 1;	// current string
    TYPDSC->DSC$A_POINTER = TVT->TVT$SUB_PTR;
    TYPDSC->DSC$B_DTYPE = DSC$K_DTYPE_T;
    TYPDSC->DSC$B_CLASS = DSC$K_CLASS_S;
!!//    OLDTYPDSC->DSC$W_LENGTH = %ALLOCATION( TVT->TVT$TERMINAL_TYPE);
    OLDTYPDSC->DSC$W_LENGTH = 16;			// Saved terminal type
    OLDTYPDSC->DSC$A_POINTER = TVT->TVT$TERMINAL_TYPE;
    OLDTYPDSC->DSC$B_DTYPE = DSC$K_DTYPE_T;
    OLDTYPDSC->DSC$B_CLASS = DSC$K_CLASS_S;
    TVT->TVT$SUB_PTR = TVT->TVT$SUB_BUF;
    TVT->TVT$SUB_CNT = 0;

    Type_ID = Lookup_terminal(TYPDSC,devdep);	// Get terminal type

    if (Type_ID > 0)			// Is it OK ?
	{
!
!		Setup the device dependent bits
!
	    Set_PTY_Term_Type(TVT,type_id,devdep);
	}
    else IF	STR$CASE_BLIND_COMPARE(OLDTYPDSC,TYPDSC)	// Not same ?
		AND (TVT->TVT$KILL_TERMINAL_TYPE LEQU 6)	// Not 6 tries ?
		AND (Type_ID != 0)
	THEN {
	    TVT_S}_SUBOP(TVT,Telnet$K_Terminal_Type,
		%REF(Option$K_Terminal_Type_Send), 1);
!!!	    OPTBLK->OPT$PREFER = OPT$DONT_CARE;
	};
	TVT->TVT$KILL_TERMINAL_TYPE = TVT->TVT$KILL_TERMINAL_TYPE+1;
	Status = STR$COPY_DX(OLDTYPDSC,TYPDSC);
    SS$_NORMAL
    };

Set_DEVDEP_DONE(TVT) : NOVALUE (void)
    {
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	tty_chan	= TVT->TVT$TTY_CHN;
    signed long
	Status;

	XLOG$FAO(LOG$TELNEG,"!%T Set_DEVDEP_Done:!/",0);
	if tty_chan == 0 then RETURN(SS$_Normal);
	Status = $DASSGN (CHAN = tty_chan);
	if (NOT Status)
		    XLOG$FAO(LOG$TELERR
		    ,"!%T Set_DEVDEP_Done: DASSGN status=!UL!/",0,status);
	tty_chan = 0;
    ss$_normal
};

Set_DEVDEP(TVT) : NOVALUE (void)
!--
// Functional Description:
!
!	Set the terminal device dependent characteristics
!	Added by JC
!--
    {
    signed long
	Telnet_Passall,
	Changed,
	IO_Stats : BLOCK[8,BYTE],
	Unit_Number,
	devstr : VECTOR[20,BYTE],
	devnam : $BBLOCK->DSC$K_S_BLN PRESET (
		[DSC$W_LENGTH] = %ALLOCATION(devstr),
		[DSC$B_DTYPE] = DSC$K_DTYPE_Z,
		[DSC$B_CLASS] = DSC$K_CLASS_Z,
		[DSC$A_POINTER] = devstr),
	ptystr : VECTOR[20,BYTE],
	ptynam : $BBLOCK->DSC$K_S_BLN PRESET (
		[DSC$W_LENGTH] = %ALLOCATION(ptystr),
		[DSC$B_DTYPE] = DSC$K_DTYPE_Z,
		[DSC$B_CLASS] = DSC$K_CLASS_Z,
		[DSC$A_POINTER] = ptystr),
	Item_List		: $ITMLST_DECL (Items = 3),
	Status		: LONG;
    MAP
	struct TVT$BLOCK * TVT;
    BIND
	pty_chan	= TVT->TVT$PTY_CHN,
	tty_chan	= TVT->TVT$TTY_CHN,
	PTY_Char	= TVT->TVT$TTY_CHAR 		: $BBLOCK,
	Charistics	= PTY_Char [QCB$L_Charistics]	: $BBLOCK,
	Extend_Char	= PTY_Char [QCB$L_EXT}_CHAR]	: $BBLOCK,
	LCLOPTS		= TVT->TVT$LCLOPTS 		: OPT$LIST,
	Echo_opt	= LCLOPTS[Telnet$K_Echo,OPT$BASE] : OPT$BLOCK,
	State_Echo	= LCLOPTS[Telnet$K_Echo,Opt$State],
	State_Binary	= LCLOPTS[Telnet$K_Binary,Opt$State],
	State_Eightbit	= LCLOPTS[Telnet$K_Extended_ASCII,Opt$State];

!
!	If user is not yet setup we must exit to prevent conflicts.
!
    if (TVT->TVT$DO_PID) RETURN(0);

    Status = $QIOW (
		CHAN	= pty_chan,
		FUNC	= IO$_SENSEMODE,
		IOSB	= io_stats,
		P1	= PTY_char,
		P2	= QCB$K_Size);

    IF NOT Status
	THEN {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_DEVDEP: SENSEMODE PTY status=!UL!/",0,status);
	}

    else IF NOT io_stats[0,0,16,0]
	THEN {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_DEVDEP: SENSEMODE PTY status=!UL!/"
		,0,io_stats[0,0,16,0]);
	}
    else {
	XLOG$FAO(LOG$TELNEG
		,"!%T Set_DEVDEP: Before Class=!UB Type=!UB Char:x!XL x!XL Size=!UW !UB!/",0
		,PTY_Char->QCB$B_Class
		,PTY_Char->QCB$B_Type
		,(PTY_Char->QCB$L_CHARISTICS && %X"FFFFFF")
		,PTY_Char[QCB$L_EXT}_CHAR]
		,PTY_Char->QCB$W_PAGE_WIDTH
		,PTY_Char->QCB$B_PAGE_LENGTH);

	Telnet_passall = $TRNLOG(
		LOGNAM=%ASCID"TELNET_PASSALL",		// JC
		RSLLEN=ptynam->DSC$W_LENGTH,		// JC
		RSLBUF=ptynam);				// JC

	Changed = 0;
	Changed = TVT->TVT$TTSET;
	if Telnet_Passall == 1 THEN
	    Changed = Changed OR
		(Charistics [TT$V_PASSALL] XOR State_Binary);

	IF NOT Echo_OPT->OPT$CURRENT		// Echo being negotiated ??
	THEN {
	    Changed = Changed OR
		(Charistics [TT$V_NOECHO] XOR (NOT State_Echo));
	    Charistics [TT$V_NOECHO] = 1 && (NOT State_Echo);
	    };


	XLOG$FAO(LOG$TELNEG
		,"!%T Set_DEVDEP: Changed=x!XL!/",0
		,Changed);

	If	NOT (Changed OR
		(Charistics->TT$V_EIGHTBIT XOR
			(State_Binary || State_Eightbit)))
		THEN RETURN(SS$_NORMAL);	// No changes to be made

	Charistics [TT$V_EIGHTBIT] = State_Binary || State_Eightbit;
	if Telnet_Passall == 1 THEN
		Charistics [TT$V_PASSALL] = State_Binary;

        TVT[TVT$TTYDEP}] =  PTY_Char->QCB$L_CHARISTICS;
        TVT[TVT$TTYDEP}2] = PTY_Char[QCB$L_EXT}_CHAR];
	XLOG$FAO(LOG$TELNEG
		,"!%T Set_DEVDEP: After  Class=!UB Type=!UB Char:x!XL x!XL Size=!UW !UB!/",0
		,PTY_Char->QCB$B_Class
		,PTY_Char->QCB$B_Type
		,(PTY_Char->QCB$L_CHARISTICS && %X"FFFFFF")
		,PTY_Char[QCB$L_EXT}_CHAR]
		,PTY_Char->QCB$W_PAGE_WIDTH
		,PTY_Char->QCB$B_PAGE_LENGTH);

	Status = $QIOW (
		CHAN	= pty_chan,
		FUNC	= IO$_SETMODE,
		IOSB	= io_stats,
		P1	= PTY_char,
		P2	= QCB$K_Size);

	IF NOT Status
	THEN {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_DEVDEP: SETMODE PTY status=!UL!/",0,status);
	}

	else IF NOT io_stats[0,0,16,0]
	THEN {
	    XLOG$FAO(LOG$TELERR
		,"!%T Set_DEVDEP: SETMODE PTY status=!UL!/",
		0,io_stats[0,0,16,0]);
	}
    };

    TVT->TVT$TTSET = Changed;
    if (NOT Changed) RETURN(SS$_Normal);

!
!	If we changed some vital stuff, change it through front end.
!	JC
!

    if (tty_chan != 0) RETURN(SS$_Normal);		// Front end in use ?
!
!	We find the correct terminal by:
!	1.	Get unit number
!	2.	Get Front end device name
!	3.	Concatonate name and number
!	4.	Then attach to it.

	devnam->DSC$A_POINTER = TVT->TVT$TTY_DEVSTR;

	$ITMLST_INIT (ITMLST = Item_List,
		(ITMCOD = DVI$_UNIT, BUFADR = Unit_Number));
	Status = $GETDVIW (ITMLST = Item_List
			,CHAN=.PTY_chan);		// Get unit

!!!JC	XLOG$FAO(LOG$TELNEG
!!!JC		,"!%T Set_DEVDEP: Unit_Number=x!XL!/",0
!!!JC		,Unit_Number);

	ptynam->DSC$W_LENGTH = %ALLOCATION(ptystr);
	Status = $TRNLOG(
		LOGNAM=%ASCID"INET$PTY_TERM",		// JC
		RSLLEN=ptynam->DSC$W_LENGTH,		// JC
		RSLBUF=ptynam);				// JC

	Status = Status == SS$_NORMAL;
	if (Status)
	    Status = $FAO(%ASCID"_!ASA!UL:",devnam,devnam,ptynam,Unit_Number);
	if (Status)
		Status = $GETDVIW (ITMLST = Item_List, DEVNAM = devnam);

!!!JC	xlog$fao(LOG$TELNEG
!!!JC			,"!%T Set_DEVDEP: TTY_TERM == "!AS""
!!!JC			,0,devnam);			// Concatonate

	Status = $ASSIGN( DEVNAM=devnam
			,CHAN=tty_chan);		// Open new channel


	IF NOT Status					// No channel ?
	    THEN {
		tty_chan = 0;
		XLOG$FAO(LOG$TELERR
		,"!%T Set_DEVDEP: ASSIGN status=!UL!/",0,status);
	    }
	    else {
		Status = $QIO (
			CHAN	= tty_chan,
			FUNC	= IO$_SETMODE,
			ASTADR	= Set_DEVDEP_done,
		        ASTPRM	= TVT,
			P1	= PTY_char,
			P2	= QCB$K_Size);		// Set front end
		IF NOT Status
		THEN {
		    XLOG$FAO(LOG$TELERR
			,"!%T Set_DEVDEP: SETMODE TTY status=!UL!/",0,status);
		    Status = $DASSGN (CHAN = tty_chan);
		    tty_chan = 0;
		};
	    };

	TVT->TVT$TTSET = 0;			// True if characteristics changed
!!!JC	    XLOG$FAO(LOG$TELNEG
!!!JC		,"!%T Set_DEVDEP: tty_chan=!UL!/",0,tty_chan);
!!!JC	    XLOG$FAO(LOG$TELNEG
!!!JC		,"!%T Set_DEVDEP: pty_chan=!UL!/",0,PTY_chan);

    SS$_NORMAL
    };


Void LineMode_Sub (TVT) (void)
    {
    MAP
	struct TVT$BLOCK * TVT;
    signed long
	b1,b2;

    // Do we have enough data
    if (TVT->TVT$SUB_CNT LSS 2) RETURN;

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
    };


/*
Linemode_SubOp_Mode ( Mask )
    {
    EXTERNAL ROUTINE
	Send_TCP_SubOption;
    signed long
	New_Mask : BYTE,
	Mode_EDIT,Mode_TRAPSIG,
	SubOption_Data	: $BBLOCK->2,
	SubOption_Desc	: $BBLOCK [DSC$K_S_BLN] PRESET (
				[DSC$W_LENGTH]	= %ALLOCATION(SubOption_Data),
				[DSC$B_DTYPE]	= DSC$K_DTYPE_T,
				[DSC$B_CLASS]	= DSC$K_CLASS_D,
				[DSC$A_POINTER]	= SubOption_Data);

    // If the ACK bit is set, don't do anything.
    if (Mask<Option$K_LineMode_MODE_ACK,1>) return SS$_NORMAL;

    Mask = Mask && 3;	// mask the mask.

    // Is the new mask different?
    if (Mask != Linemode_Modeflags)
	{
	Linemode_Modeflags = Mask;
	Mask<Option$K_Linemode_Mode_EDIT_ACK,1> = 1;
	SubOption_Data->0 = Option$K_Linemode_Mode;
	SubOption_Data->1 = Mask;
	!!!HACK!!// ???
	Send_TCP_SubOption ( Option$K_Linemode , SubOption_Desc )
	};

    SS$_NORMAL
    };


Linemode_SubOp_Forwardmask ( Action , Data_A , Length )
    {
    BIND
	Data = Data_A : VECTOR[,BYTE];
    signed long
	SrcI  = 0,
	DstI  = 0;
    BIND
	Curr_FMask = Forward_Mask : VECTOR[32,BYTE];

    SELECT Action OF
	SET
	[Telnet$K_DO] :
	    {
!!!HACK!!// Should check for double IACs? Should check for Binary mode?
	    WHILE SrcI LSS Length DO
		{
		Curr_FMask[DstI] = Data[SrcI];
		DstI = DstI + 1;
		SrcI = SrcI + 1
	 	}
	    Linemode_FM_State = Option$K_State_On;
	    };
	[Telnet$K_DONT] :
	    Linemode_FM_State = Option$K_State_Off;
!	[Telnet$K_WILL,Telnet$K_WONT] :
	TES;

    SS$_NORMAL
    };



Process_SLC(Code,Modifier,Value,Reply_Desc)
    {
    MAP
	Reply_Desc	: $BBLOCK [DSC$K_S_BLN];
    signed long
	Level,
	struct $BBlock * Char_Def->SLC$K_Size;

    if ((Code LSS 0) || (Code > Option$K_Limemode_SLC_MaxOpt))
	return SS$_NORMAL;

    Level = Modifier<SLC$F_Level>;
    Char_Def = SLC_Table[Code,SLC$B_{ING] ;

    // 1) Check if the new and old values are the same.
    if ((Value == Char_Def->SLC$B_CURRENT)) RETURN(SS$_NORMAL);

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

    SS$_NORMAL
    };

Linemode_SubOp_SLC(Data_A, Length)
    {
    BIND
	Data = Data_A : VECTOR [, BYTE];
    signed long
	Reply_Desc	: $BBLOCK [DSC$K_S_BLN] PRESET (
				[DSC$W_LENGTH]	= 0,
				[DSC$B_DTYPE]	= DSC$K_DTYPE_T,
				[DSC$B_CLASS]	= DSC$K_CLASS_D,
				[DSC$A_POINTER]	= 0);

    WHILE LENGTH GEQ 3 DO
	{
	Process_SLC(Data->0, Data->1, Data->2, Reply_Desc);
	Length = Length - 3;
	Data = Data->3
	};

    if (Reply_Desc->DSC$W_LENGTH > 0)
	Send_TCP_SubOpt ( Option$K_Linemode , Reply_Desc );

    SS$_NORMAL
    };
*/

} ELUDOM

