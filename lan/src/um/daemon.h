/* 
 * Copyright (C) 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "net_user.h"

struct daemon_data {
	char *sock_type;
	char *ctl_sock;
	void *ctl_addr;
	void *data_addr;
	void *local_addr;
	unsigned char hwaddr[ETH_ADDR_LEN];
	int hw_setup;
	int control;
	void *dev;
};

extern struct net_user_info daemon_user_info;

extern int daemon_user_set_mac(struct daemon_data *pri, unsigned char *hwaddr,
			       int len);
extern int daemon_user_write(int fd, void *buf, int len, 
			     struct daemon_data *pri);

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
