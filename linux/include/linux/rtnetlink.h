#ifndef __LINUX_RTNETLINK_H
#define __LINUX_RTNETLINK_H

#include <linux/netlink.h>


/****
 *      Routing/neighbour discovery messages.
 ****/

/* Types of messages */

#define RTM_BASE    0x10

#define RTM_NEWLINK (RTM_BASE+0)
#define RTM_DELLINK (RTM_BASE+1)
#define RTM_GETLINK (RTM_BASE+2)

#define RTM_NEWADDR (RTM_BASE+4)
#define RTM_DELADDR (RTM_BASE+5)
#define RTM_GETADDR (RTM_BASE+6)

#define RTM_NEWROUTE    (RTM_BASE+8)
#define RTM_DELROUTE    (RTM_BASE+9)
#define RTM_GETROUTE    (RTM_BASE+10)

#define RTM_NEWNEIGH    (RTM_BASE+12)
#define RTM_DELNEIGH    (RTM_BASE+13)
#define RTM_GETNEIGH    (RTM_BASE+14)

#define RTM_NEWRULE (RTM_BASE+16)
#define RTM_DELRULE (RTM_BASE+17)
#define RTM_GETRULE (RTM_BASE+18)

#define RTM_NEWQDISC    (RTM_BASE+20)
#define RTM_DELQDISC    (RTM_BASE+21)
#define RTM_GETQDISC    (RTM_BASE+22)

#define RTM_NEWTCLASS   (RTM_BASE+24)
#define RTM_DELTCLASS   (RTM_BASE+25)
#define RTM_GETTCLASS   (RTM_BASE+26)

#define RTM_NEWTFILTER  (RTM_BASE+28)
#define RTM_DELTFILTER  (RTM_BASE+29)
#define RTM_GETTFILTER  (RTM_BASE+30)

#define RTM_MAX     (RTM_BASE+31)

/*
   Generic structure for encapsulation optional route information.
   It is reminiscent of sockaddr, but with sa_family replaced
   with attribute type.
 */

struct rtattr
{
    unsigned short  rta_len;
    unsigned short  rta_type;
};

#endif  /* __LINUX_RTNETLINK_H */
