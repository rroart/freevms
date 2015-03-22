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
/*++
*******************************************************************************
*
* TITLE:    LPRDef.H
*
* VERSION:  V1.0
*
* FACILITY:
*
*   CMU-OpenVMS/IP
*
* ABSTRACT:
*
*   C header file defining LPR protocol constants
*
* ENVIRONMENT:
*
*   VAX/VMS
*
* AUTHOR:   Art Stine, Clarkson U/ERC
*           Copyright (c) 1990, Clarkson University
*
* CREATION DATE:    January, 1990
*
* MODIFIED BY:
*
*******************************************************************************
*--*/

/* LPR main commands and constants */

#define LPR$K_Timeout       120
#define LPR$K_Longest_Line  32766

#define LPR$K_Success       '\000'
#define LPR$K_Error         '\001'
#define LPR$K_No_Room       '\002'
#define LPR$K_Null          '\000'
#define LPR$K_EOL           '\010'

#define LPR$K_Check_Queue   '\001'
#define LPR$K_Add_Entry     '\002'
#define LPR$K_Show_Brief    '\003'
#define LPR$K_Show_Long     '\004'
#define LPR$K_Remove_Entry  '\005'

/* LPR Send File sub-commands */

#define LPR$K_Send_Noop     '\001'
#define LPR$K_Send_Control  '\002'
#define LPR$K_Send_Data     '\003'


#define LPR$K_Init          '\001'
#define LPR$K_Send_Control  '\002'
#define LPR$K_Show_Brief    '\003'
#define LPR$K_Show_Long     '\004'
#define LPR$K_Remove_Entry  '\005'

/* LPR Port definitions */

#define LPR$K_LPDPort         515
#define LPR$K_MaxLocalPort    1023
