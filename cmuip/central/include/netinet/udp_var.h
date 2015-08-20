/*
#ifndef lint
static  char    *sccsid = "@(#)udp_var.h    4.1.1.1 (ULTRIX)    1/20/88";
#endif lint
*/
/************************************************************************
 *                                  *
 *          Copyright (c) 1984 by               *
 *      Digital Equipment Corporation, Maynard, MA      *
 *          All rights reserved.                *
 *                                  *
 *   This software is furnished under a license and may be used and *
 *   copied  only  in accordance with the terms of such license and *
 *   with the  inclusion  of  the  above  copyright  notice.   This *
 *   software  or  any  other copies thereof may not be provided or *
 *   otherwise made available to any other person.  No title to and *
 *   ownership of the software is hereby transferred.           *
 *                                  *
 *   This software is  derived  from  software  received  from  the *
 *   University    of   California,   Berkeley,   and   from   Bell *
 *   Laboratories.  Use, duplication, or disclosure is  subject  to *
 *   restrictions  under  license  agreements  with  University  of *
 *   California and with AT&T.                      *
 *                                  *
 *   The information in this software is subject to change  without *
 *   notice  and should not be construed as a commitment by Digital *
 *   Equipment Corporation.                     *
 *                                  *
 *   Digital assumes no responsibility for the use  or  reliability *
 *   of its software on equipment which is not supplied by Digital. *
 *                                  *
 ************************************************************************/
/*  udp_var.h   6.1 83/07/29    */

#ifndef UDP_VAR_H
#define UDP_VAR_H

/*
 * UDP kernel structures and variables.
 */
struct  udpiphdr
{
    struct  ipovly ui_i;        /* overlaid ip structure */
    struct  udphdr ui_u;        /* udp header */
};
#define ui_next     ui_i.ih_next
#define ui_prev     ui_i.ih_prev
#define ui_x1       ui_i.ih_x1
#define ui_pr       ui_i.ih_pr
#define ui_len      ui_i.ih_len
#define ui_src      ui_i.ih_src
#define ui_dst      ui_i.ih_dst
#define ui_sport    ui_u.uh_sport
#define ui_dport    ui_u.uh_dport
#define ui_ulen     ui_u.uh_ulen
#define ui_sum      ui_u.uh_sum

struct  udpstat
{
    int udps_hdrops;
    int udps_badsum;
    int udps_badlen;
    int udps_total;
    int udps_nospace;
};

#define UDP_TTL     30      /* time to live for UDP packets */

#ifdef KERNEL
struct  inpcb udb;
struct  udpstat udpstat;
#endif

#endif /* UDP_VAR_H */
