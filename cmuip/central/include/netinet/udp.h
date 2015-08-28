/*  udp.h   6.1 83/07/29    */

#ifndef NETINET_UDP_H
#define NETINET_UDP_H

/*
 * Udp protocol header.
 * Per RFC 768, September, 1981.
 */
struct udphdr
{
    u_short uh_sport;       /* source port */
    u_short uh_dport;       /* destination port */
    short   uh_ulen;        /* udp length */
    u_short uh_sum;         /* udp checksum */
};

#endif /* NETINET_UDP_H */
