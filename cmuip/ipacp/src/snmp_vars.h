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
/*
 * Definitions for SNMP (RFC 1067) agent variable finder.
 *
 *
 */
/***********************************************************
    Copyright 1988, 1989 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/

#define     MAX_INTERFACES  2
extern  struct mib_ifEntry  mib_ifEntry[MAX_INTERFACES];
extern  struct mib_ip       mib_ip;
#define     ROUTE_ENTRIES   2
extern  struct mib_udp      mib_udp;
extern  long    mib_icmpInMsgs;
extern  long    mib_icmpOutMsgs;
extern  long    mib_icmpInErrors;   /* not checked in KIP */
extern  long    mib_icmpOutErrors;  /* not checked in KIP */
extern  long    mib_icmpInCount[];
extern  long    mib_icmpOutCount[];

u_char  *var_system();
u_char  *var_ifEntry();
u_char  *var_atEntry();
u_char  *var_ip();
u_char  *var_ipAddrEntry();
u_char  *var_ipRouteEntry();
u_char  *var_icmp();
u_char  *var_udp();
u_char  *getStatPtr();


