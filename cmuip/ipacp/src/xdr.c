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
/****************************************************************************

        XDR - eXternal Data Representation
Facility:

    XDR.C - NFS.BLI - Provide XDR convertion routines (RFC 1014)

Abstract:

    Supports the external data representation protocol (XDR) for
    the IPACP.  Used by RPC services.

Notes:

    These routines are very simple and are not built for speed.

    NTOV == Network --> VAX
    VTON == VAX --> Network

    The following types have been implemented:

        int - INTEGER
        uint    - UNSIGNED INTEGER
        arb - arbitrary data block (n bytes with longword padding)

Author:

    Bruce R. Miller, CMU NetDev, 12-Nov-1990
    Copyright (c) 1990, Bruce R. Miller and Carnegie-Mellon University

Module Modification History:

 ***************************************************************************/

typedef unsigned int u_int;

#include <stdio.h>
#include <ssdef.h>
#include <ctype.h>

typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned long u_long;
typedef char * caddr_t;

#include <netinet/in.h>         /* network defs (defines u_long!) */

#include "rpc_types.h"      /* usefull and relavant definitions */
#include "xdr.h"

typedef unsigned char BYTE;



/* XDR type: INTEGER

   XDR integers are stored highest byte first.

   NB: this could be done more elegantly with bitfields and rotation.

   NB: Is this broken?  what happens to the sign bit?
*/

XDR$ntov_int (in,out)
BYTE *in,*out;
{
    BYTE tmp[4];

    /* Move to the tmp array */
    tmp[0] = in[3];
    tmp[1] = in[2];
    tmp[2] = in[1];
    tmp[3] = in[0];

    /* move back, flipped */
    out[0] = tmp[0];
    out[1] = tmp[1];
    out[2] = tmp[2];
    out[3] = tmp[3];
}

XDR$vton_int (in,out)
BYTE *in,*out;
{
    XDR$ntov_int(in,out);
}



/* XDR type: UNSIGNED INTEGER

   XDR integers are stored highest byte first.

   NB: this could be done more elegantly with bitfields and rotation.

*/

XDR$ntov_uint (in,out)
BYTE *in,*out;
{
    BYTE tmp[4];

    /* Move to the tmp array */
    tmp[0] = in[3];
    tmp[1] = in[2];
    tmp[2] = in[1];
    tmp[3] = in[0];

    /* move back, flipped */
    out[0] = tmp[0];
    out[1] = tmp[1];
    out[2] = tmp[2];
    out[3] = tmp[3];
}

XDR$vton_uint (in,out)
BYTE *in,*out;
{
    XDR$ntov_int(in,out);
}

XDR$ntov_arb (char **in, char *out, int numbytes)
{
    memcpy(out, *in, numbytes);
    *in += ((numbytes + 3) / 4) * 4;    /* round up to next 4-byte increment */
}
