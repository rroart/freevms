/*
 * Copyright (C) 2001 Lennert Buytenhek (buytenh@gnu.org)
 * Licensed under the GPL
 */

#ifndef __MCONSOLE_H__
#define __MCONSOLE_H__

#define MCONSOLE_MAGIC (0xcafebabe)
#define MCONSOLE_MAX_DATA (512)
#define MCONSOLE_VERSION 1

struct mconsole_request {
	unsigned long magic;
	int version;
	int len;
	char data[MCONSOLE_MAX_DATA];
};

struct mconsole_reply {
	int err;
	int more;
	int len;
	char data[MCONSOLE_MAX_DATA];
};

struct mc_request;

struct mconsole_command
{
	char *command;
	void (*handler)(struct mc_request *req);
	int as_interrupt;
};

struct mc_request
{
	int len;
	int as_interrupt;

	int originating_fd;
	int originlen;
	unsigned char origin[128];			/* sockaddr_un */

	struct mconsole_request request;
	struct mconsole_command *cmd;
};

extern char mconsole_socket_name[];

extern int mconsole_unlink_socket(void);
extern int mconsole_reply(struct mc_request *req, char *reply, int err,
			  int more);
extern void mconsole_version(struct mc_request *req);
extern void mconsole_help(struct mc_request *req);
extern void mconsole_halt(struct mc_request *req);
extern void mconsole_reboot(struct mc_request *req);
extern void mconsole_config(struct mc_request *req);
extern void mconsole_remove(struct mc_request *req);
extern void mconsole_sysrq(struct mc_request *req);
extern void mconsole_cad(struct mc_request *req);
extern int mconsole_create_listening_socket(void);
extern int mconsole_get_request(int fd, struct mc_request *req);
extern void mconsole_open_for_business(char *socket);

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
