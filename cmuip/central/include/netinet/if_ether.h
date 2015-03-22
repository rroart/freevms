/* static   char    *sccsid = "@(#)if_ether.h   4.1.1.2 (ULTRIX)    1/28/88"; */

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
 *
 *  15-Jan-88   lp
 *      Merge of final 43BSD changes.
 *
 *  Larry Cohen  -  09/16/85                    *
 *      Add 43bsd alpha tape changes for subnet routing     *
 *                                  *
 ************************************************************************/

/*
 * Copyright (c) 1982 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *  if_ether.h  6.5 (Berkeley) 6/8/85
 */

/*
 * Structure of a 10Mb/s Ethernet header.
 */
struct  ether_header
{
    u_char  ether_dhost[6];
    u_char  ether_shost[6];
    u_short ether_type;
};

#define ETHERTYPE_PUP   0x0200      /* PUP protocol */
#define ETHERTYPE_NS    0x0600      /* XNS protocol */
#define ETHERTYPE_IP    0x0800      /* IP protocol */
#define ETHERTYPE_ARP   0x0806      /* Addr. resolution protocol */
#define ETHERTYPE_DN    0x6003      /* DECnet protocol */
#define ETHERTYPE_LAT   0x6004      /* LAT protocol */
#define ETHERTYPE_ATALK 0x809B      /* Appletalk */
#define ETHERTYPE_AARP  0x80F3      /* Appletalk Arp */

/*
 * The ETHERTYPE_NTRAILER packet types starting at ETHERTYPE_TRAIL have
 * (type-ETHERPUP_TRAIL)*512 bytes of data followed
 * by a PUP type (as given above) and then the (variable-length) header.
 */
#define ETHERTYPE_TRAIL     0x1000      /* Trailer packet */
#define ETHERTYPE_NTRAILER  16

#define ETHERMTU    1500
#define ETHERMIN    (60-14)

/*
 * Ethernet Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  Structure below is adapted
 * to resolving internet addresses.  Field names used correspond to
 * RFC 826.
 */
struct  ether_arp
{
    struct arphdr ea_hdr;   /* fixed-size header */
#define ARPHRD_ETHER    1   /* ethernet hardware address */
#define ARPOP_REQUEST   1   /* request to resolve address */
#define ARPOP_REPLY 2   /* response to previous request */
    u_char  arp_sha[6]; /* sender hardware address */
    u_char  arp_spa[4]; /* sender protocol address */
    u_char  arp_tha[6]; /* target hardware address */
    u_char  arp_tpa[4]; /* target protocol address */
};
#define arp_hrd ea_hdr.ar_hrd
#define arp_pro ea_hdr.ar_pro
#define arp_hln ea_hdr.ar_hln
#define arp_pln ea_hdr.ar_pln
#define arp_op  ea_hdr.ar_op

/*
 * Structure shared between the ethernet driver modules and
 * the address resolution code.  For example, each ec_softc or il_softc
 * begins with this structure.
 */
struct  arpcom
{
    struct  ifnet ac_if;        /* network-visible interface */
    u_char  ac_enaddr[6];       /* ethernet hardware address */
    struct in_addr ac_ipaddr;   /* copy of ip address- XXX */
};

/*
 * Internet to ethernet address resolution table.
 */
struct  arptab
{
    struct  in_addr at_iaddr;   /* internet address */
    u_char  at_enaddr[6];       /* ethernet address */
    struct  mbuf *at_hold;      /* last packet until resolved/timeout */
    u_char  at_timer;       /* minutes since last reference */
    u_char  at_flags;       /* flags */
};

#ifdef  KERNEL
u_char etherbroadcastaddr[6];
struct  arptab *arptnew();
extern int nETHER; /* defined conf.c */
char *ether_sprintf();
#endif
