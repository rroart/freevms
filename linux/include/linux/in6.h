/*
 *  Types and definitions for AF_INET6
 *  Linux INET6 implementation
 *
 *  Authors:
 *  Pedro Roque     <roque@di.fc.ul.pt>
 *
 *  Sources:
 *  IPv6 Program Interfaces for BSD Systems
 *      <draft-ietf-ipngwg-bsd-api-05.txt>
 *
 *  Advanced Sockets API for IPv6
 *  <draft-stevens-advanced-api-00.txt>
 *
 *  This program is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation; either version
 *      2 of the License, or (at your option) any later version.
 */

#ifndef _LINUX_IN6_H
#define _LINUX_IN6_H

#include <linux/types.h>

/*
 *  IPv6 address structure
 */

struct in6_addr
{
    union
    {
        __u8        u6_addr8[16];
        __u16       u6_addr16[8];
        __u32       u6_addr32[4];
    } in6_u;
#define s6_addr         in6_u.u6_addr8
#define s6_addr16       in6_u.u6_addr16
#define s6_addr32       in6_u.u6_addr32
};


/*
 *  IPV6 extension headers
 */
#define IPPROTO_HOPOPTS     0   /* IPv6 hop-by-hop options  */
#define IPPROTO_ROUTING     43  /* IPv6 routing header      */
#define IPPROTO_FRAGMENT    44  /* IPv6 fragmentation header    */
#define IPPROTO_ICMPV6      58  /* ICMPv6           */
#define IPPROTO_NONE        59  /* IPv6 no next header      */
#define IPPROTO_DSTOPTS     60  /* IPv6 destination options */


#endif
