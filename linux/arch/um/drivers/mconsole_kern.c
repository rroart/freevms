/*
 * Copyright (C) 2001 Lennert Buytenhek (buytenh@gnu.org)
 * Licensed under the GPL
 */

#include "linux/kernel.h"
#include "linux/slab.h"
#include "linux/init.h"
#include "linux/notifier.h"
#include "linux/reboot.h"
#include "linux/utsname.h"
#include "linux/ctype.h"
#include "linux/interrupt.h"
#include "linux/sysrq.h"
#include "asm/irq.h"
#include "user_util.h"
#include "kern_util.h"
#include "kern.h"
#include "mconsole.h"
#include "mconsole_kern.h"
#include "irq_user.h"
#include "init.h"

static int do_unlink_socket(struct notifier_block *notifier, 
			    unsigned long what, void *data)
{
	return(mconsole_unlink_socket());
}


static struct notifier_block reboot_notifier = {
	notifier_call:		do_unlink_socket,
	priority:		0,
};

LIST_HEAD(mc_requests);

void mc_task_proc(void *unused)
{
	struct mconsole_entry *req;
	unsigned long flags;
	int done;

	do {
		save_flags(flags);
		req = list_entry(mc_requests.next, struct mconsole_entry, 
				 list);
		list_del(&req->list);
		done = list_empty(&mc_requests);
		restore_flags(flags);
		req->request.cmd->handler(&req->request);
		kfree(req);
	} while(!done);
}

struct tq_struct mconsole_task = {
	routine:	mc_task_proc,
	data: 		NULL
};

void mconsole_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	int fd;
	struct mconsole_entry *new;
	struct mc_request req;

	fd = (int) dev_id;
	while (mconsole_get_request(fd, &req)){
		if(req.cmd->as_interrupt) (*req.cmd->handler)(&req);
		else {
			new = kmalloc(sizeof(req), GFP_ATOMIC);
			if(new == NULL)
				mconsole_reply(&req, "Out of memory", 1, 0);
			else {
				new->request = req;
				list_add(&new->list, &mc_requests);
			}
		}
	}
#ifndef CONFIG_VMS
	if(!list_empty(&mc_requests)) schedule_task(&mconsole_task);
#else
//	mc_task_proc(0);
#endif
	reactivate_fd(fd);
}

void mconsole_version(struct mc_request *req)
{
	char version[256];

	sprintf(version, "%s %s %s %s %s", system_utsname.sysname, 
		system_utsname.nodename, system_utsname.release, 
		system_utsname.version, system_utsname.machine);
	mconsole_reply(req, version, 0, 0);
}

#define UML_MCONSOLE_HELPTEXT \
"Commands:\n    version - Get kernel version\n    help - Print this message\n    halt - Halt UML\n    reboot - Reboot UML\n    config <dev>=<config> - Add a new device to UML;\n 	same syntax as command line\n    remove <dev> - Remove a device from the client\n    sysrq <letter> - Performs the SysRq action controlled by the letter\n    cad - invoke the Ctl-Alt-Del handler\n"

void mconsole_help(struct mc_request *req)
{
	mconsole_reply(req, UML_MCONSOLE_HELPTEXT, 0, 0);
}

void mconsole_halt(struct mc_request *req)
{
	mconsole_reply(req, "", 0, 0);
	machine_halt();
}

void mconsole_reboot(struct mc_request *req)
{
	mconsole_reply(req, "", 0, 0);
	machine_restart(NULL);
}

extern void ctrl_alt_del(void);

void mconsole_cad(struct mc_request *req)
{
	mconsole_reply(req, "", 0, 0);
	ctrl_alt_del();
}

LIST_HEAD(mconsole_devices);

void mconsole_register_dev(struct mc_device *new)
{
	list_add(&new->list, &mconsole_devices);
}

static struct mc_device *mconsole_find_dev(char *name)
{
	struct list_head *ele;
	struct mc_device *dev;

	list_for_each(ele, &mconsole_devices){
		dev = list_entry(ele, struct mc_device, list);
		if(!strncmp(name, dev->name, strlen(dev->name)))
			return(dev);
	}
	return(NULL);
}

void mconsole_config(struct mc_request *req)
{
	struct mc_device *dev;
	char *ptr = req->request.data;
	int err;

	ptr += strlen("config");
	while(isspace(*ptr)) ptr++;
	dev = mconsole_find_dev(ptr);
	if(dev == NULL){
		mconsole_reply(req, "Bad configuration option", 1, 0);
		return;
	}
	err = (*dev->config)(&ptr[strlen(dev->name)]);
	mconsole_reply(req, "", err, 0);
}

void mconsole_remove(struct mc_request *req)
{
	struct mc_device *dev;	
	char *ptr = req->request.data;
	int err;

	ptr += strlen("remove");
	while(isspace(*ptr)) ptr++;
	dev = mconsole_find_dev(ptr);
	if(dev == NULL){
		mconsole_reply(req, "Bad remove option", 1, 0);
		return;
	}
	err = (*dev->remove)(&ptr[strlen(dev->name)]);
	mconsole_reply(req, "", err, 0);
}

#ifdef CONFIG_MAGIC_SYSRQ
void mconsole_sysrq(struct mc_request *req)
{
	char *ptr = req->request.data;

	ptr += strlen("sysrq");
	while(isspace(*ptr)) ptr++;

	handle_sysrq(*ptr, &current->thread.regs, NULL, NULL);
	mconsole_reply(req, "", 0, 0);
}
#else
void mconsole_sysrq(struct mc_request *req)
{
	mconsole_reply(req, "Sysrq not compiled in", 1, 0);
}
#endif

static char *notify_socket = NULL;

int mconsole_init(void)
{
	int err;
	int sock;

	sock = mconsole_create_listening_socket();
	if (sock < 0) {
		printk("Failed to initialize management console\n");
		return 1;
	}

	register_reboot_notifier(&reboot_notifier);

	err = um_request_irq(MCONSOLE_IRQ, sock, mconsole_interrupt,
			     SA_INTERRUPT | SA_SHIRQ, "mconsole",
			     (void *)sock);
	if (err) {
		printk("Failed to get IRQ for management console\n");
		return 1;
	}

	if(notify_socket != NULL){
		notify_socket = uml_strdup(notify_socket);
		if(notify_socket != NULL)
			mconsole_open_for_business(notify_socket);
		else printk(KERN_ERR "mconsole_setup failed to strdup "
			    "string\n");
	}

	printk("mconsole initialized on %s\n", mconsole_socket_name);
	return 0;
}

__initcall(mconsole_init);

#define NOTIFY "=notify:"

static int mconsole_setup(char *str)
{
	if(!strncmp(str, NOTIFY, strlen(NOTIFY))){
		str += strlen(NOTIFY);
		notify_socket = str;
	}
	else printk(KERN_ERR "mconsole_setup : Unknown option - '%s'\n", str);
	return(1);
}

__setup("mconsole", mconsole_setup);

__uml_help(mconsole_setup,
"mconsole=notify:<socket>\n"
"    Requests that the mconsole driver send a message to the named Unix\n"
"    socket containing the name of the mconsole socket.  This also serves\n"
"    to notify outside processes when UML has booted far enough to respond\n"
"    to mconsole requests.\n\n"
);

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
