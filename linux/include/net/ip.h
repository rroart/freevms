/*
 * INET     An implementation of the TCP/IP protocol suite for the LINUX
 *      operating system.  INET is implemented using the  BSD Socket
 *      interface as the means of communication with the user level.
 *
 *      Definitions for the IP module.
 *
 * Version: @(#)ip.h    1.0.2   05/07/93
 *
 * Authors: Ross Biro, <bir7@leland.Stanford.Edu>
 *      Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *      Alan Cox, <gw4pts@gw4pts.ampr.org>
 *
 * Changes:
 *      Mike McLagan    :       Routing by source
 *
 *      This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */
#ifndef _IP_H
#define _IP_H

#include <linux/ip.h>
#include <linux/threads.h>

#ifndef _SNMP_H
#include <net/snmp.h>
#endif

struct inet_skb_parm
{
    struct ip_options   opt;        /* Compiled IP options      */
    unsigned char       flags;

#define IPSKB_MASQUERADED   1
#define IPSKB_TRANSLATED    2
#define IPSKB_FORWARDED     4
};

/* IP flags. */
#define IP_CE       0x8000      /* Flag: "Congestion"       */
#define IP_DF       0x4000      /* Flag: "Don't Fragment"   */
#define IP_MF       0x2000      /* Flag: "More Fragments"   */
#define IP_OFFSET   0x1FFF      /* "Fragment Offset" part   */

/*
 *  Functions provided by ip.c
 */

extern struct linux_mib net_statistics[NR_CPUS*2];
#define NET_INC_STATS(field)        SNMP_INC_STATS(net_statistics, field)
#define NET_INC_STATS_BH(field)     SNMP_INC_STATS_BH(net_statistics, field)
#define NET_INC_STATS_USER(field)   SNMP_INC_STATS_USER(net_statistics, field)

#endif  /* _IP_H */
