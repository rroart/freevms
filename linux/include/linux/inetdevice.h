#ifndef _LINUX_INETDEVICE_H
#define _LINUX_INETDEVICE_H

#ifdef __KERNEL__

struct ipv4_devconf
{
    int accept_redirects;
    int send_redirects;
    int secure_redirects;
    int shared_media;
    int accept_source_route;
    int rp_filter;
    int proxy_arp;
    int bootp_relay;
    int log_martians;
    int forwarding;
    int mc_forwarding;
    int tag;
    int     arp_filter;
    void    *sysctl;
};

struct in_device
{
    struct net_device       *dev;
    atomic_t        refcnt;
    rwlock_t        lock;
    int         dead;
    struct in_ifaddr    *ifa_list;  /* IP ifaddr chain      */
    struct ip_mc_list   *mc_list;   /* IP multicast filter chain    */
    unsigned long       mr_v1_seen;
    struct neigh_parms  *arp_parms;
    struct ipv4_devconf cnf;
};

struct in_ifaddr
{
    struct in_ifaddr    *ifa_next;
    struct in_device    *ifa_dev;
    u32         ifa_local;
    u32         ifa_address;
    u32         ifa_mask;
    u32         ifa_broadcast;
    u32         ifa_anycast;
    unsigned char       ifa_scope;
    unsigned char       ifa_flags;
    unsigned char       ifa_prefixlen;
    char            ifa_label[IFNAMSIZ];
};

#endif /* __KERNEL__ */

#endif /* _LINUX_INETDEVICE_H */
