#ifndef __UM_NET_KERN_H
#define __UM_NET_KERN_H

#include "linux/netdevice.h"
#include "linux/skbuff.h"
#include "linux/socket.h"
#include "linux/list.h"

#define MAX_UML_NETDEV (16)

struct uml_net {
	struct net_device *dev;
	struct net_user_info *user;
	struct net_kern_info *kern;
	int private_size;
	int transport_index;
};

struct uml_net_private {
	spinlock_t lock;
	
	struct timer_list tl;
	struct list_head list;
	struct net_device_stats stats;
	int fd;
	unsigned short (*protocol)(struct sk_buff *);
	int (*set_mac)(struct sockaddr *hwaddr, void *);
	int (*open)(void *);
	void (*close)(int, void *);
	int (*read)(int, char * buf, int len, struct uml_net_private *);
	int (*write)(int, char * buf, int len, struct uml_net_private *);
	
	void (*add_address)(unsigned char *, unsigned char *, void *);
	void (*delete_address)(unsigned char *, unsigned char *, void *);
	int (*set_mtu)(int mtu, void *);
	int user[1];
};

struct net_kern_info {
	struct net_device *(*init)(int, int);
	unsigned short (*protocol)(struct sk_buff *);
	int (*set_mac)(struct sockaddr *hwaddr, void *);
	int (*read)(int, struct sk_buff **skb, struct uml_net_private *);
	int (*write)(int, struct sk_buff **skb, struct uml_net_private *);
};

extern struct net_device *ether_init(int);
extern unsigned short ether_protocol(struct sk_buff *);
extern int ether_set_mac(struct sockaddr *hwaddr, void *);
extern int setup_etheraddr(char *str, unsigned char *addr);
extern struct sk_buff *ether_adjust_skb(struct sk_buff *skb, int extra);
extern void tap_setup_common(char *str, char *type, char **dev_name, 
			     char *hw_addr, int *hw_setup, char **gate_addr);

#endif

/*
 * Overrides for Emacs so that we follow Linus's tabbing style.
 * Emacs will notice this stuff at the end of the file and automatically
 * adjust the settings for this buffer only.  This must remain at the end
 * of the file.
 * ---------------------------------------------------------------------------
 * Local variables:
 * c-file-style: "linux"
 * End:
 */
