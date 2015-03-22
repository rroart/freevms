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
* TITLE:    VMSDType.H
*
* VERSION:  V1.0
*
* FACILITY:
*
*   VAX C
*
* ABSTRACT:
*
*   C header file defining some of the missing C data types and
*   structures needed for using VMS system services from VAX C
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
*  1-001         A. Stine       Clarkson U/ERC
*  Added NUL definition because DEC (ANSI?) decided that NULL
*  as defined in stdio.h should now be (void *) 0 instead of 0.
*
*******************************************************************************
*--*/

#define NUL       '\0'

struct item_list_3
{
    short buffer_length, item_code;
    long buffer_address, return_length_address;
};

struct io_status_block
{
    unsigned short status;
    unsigned short bytes_transferred;
    unsigned int ipacp_status;
};

