/* sccsid  =  @(#)in_var.h  4.1 ULTRIX  11/23/87 */

/************************************************************************
 *                                  *
 *          Copyright (c) 1985 by               *
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

/************************************************************************
 *          Modification History                *
 *                                  *
 *  Larry Cohen  -  09/16/85                    *
 *      Add 43bsd alpha tape changes for subnet routing     *
 *                                  *
 *  Marc Teitelbaum and Fred Templin - 08/21/86         *
 *      Added 4.3bsd beta tape enhancemets. Added       *
 *      "ia_netbroadcast" field to the "in_ifaddr" struct.  *
 *                                  *
 ************************************************************************/


/*
 * Copyright (c) 1982 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *  @(#)in_var.h    6.2 (Berkeley) 6/8/85
 */

#ifndef IN_VAR_H
#define IN_VAR_H

/*
 * Interface address, internet version.  One of these structures
 * is allocated for each interface with an internet address.
 * The ifaddr structure contains the protocol-independent part
 * of the structure and is assumed to be first.
 */
struct in_ifaddr
{
    struct  ifaddr ia_ifa;      /* protocol-independent info */
#define ia_addr ia_ifa.ifa_addr
#define ia_broadaddr    ia_ifa.ifa_broadaddr
#define ia_dstaddr  ia_ifa.ifa_dstaddr
#define ia_ifp      ia_ifa.ifa_ifp
    u_long  ia_net;         /* network number of interface */
    u_long  ia_netmask;     /* mask of net part */
    u_long  ia_subnet;      /* subnet number, including net */
    u_long  ia_subnetmask;      /* mask of net + subnet */
    struct  in_addr ia_netbroadcast; /* broadcast addr for (logical) net */
    int ia_flags;
    struct  in_ifaddr *ia_next; /* next in list of internet addresses */
};
/*
 * Given a pointer to an in_ifaddr (ifaddr),
 * return a pointer to the addr as a sockadd_in.
 */
#define IA_SIN(ia) ((struct sockaddr_in *)(&((struct in_ifaddr *)ia)->ia_addr))
/*
 * ia_flags
 */
#define IFA_ROUTE   0x01        /* routing entry installed */

#ifdef  KERNEL
extern  struct in_ifaddr *in_ifaddr;
extern  struct in_ifaddr *in_iaonnetof();
#endif

#endif /* IN_VAR_H */
