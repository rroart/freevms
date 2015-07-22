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
* TITLE:    CMUDType.H
*
* VERSION:  V1.0
*
* FACILITY:
*
*   VAX C
*
* ABSTRACT:
*
*   C header file defining some of the CMU-OpenVMS/IP data types needed for
*   using CMU-OpenVMS/IP from VAX C
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

#ifndef CMUDTYPE_H
#define CMUDTYPE_H

#include <descrip.h>
#define NUL       '\0'

struct cmu$text
{
    struct cmu$text *txt$a_flink, *txt$a_blink;
    struct dsc$descriptor *txt$q_desc;
};

#endif /* CMUDTYPE_H */
