/* 
 * Copyright (C) 2001 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include "linux/list.h"
#include "linux/slab.h"
#include "linux/irq.h"
#include "linux/spinlock.h"
#include "linux/errno.h"
#include "asm/semaphore.h"
#include "asm/errno.h"
#include "kern_util.h"
#include "kern.h"
#include "irq_user.h"
#include "port.h"

struct port_list {
	struct list_head list;
	struct semaphore sem;
	int port;
	int fd;
	spinlock_t lock;
	struct list_head connections;
};

struct port_dev {
	struct port_list *port;
	int fd;
};

struct connection {
	struct list_head list;
	int fd;
	int socket[2];
};

struct list_head ports = LIST_HEAD_INIT(ports);

static void port_interrupt(int irq, void *data, struct pt_regs *regs)
{
	struct port_list *port = data;
	struct connection *conn;
	int fd, socket[2];

	reactivate_fd(port->fd);
	fd = port_connection(port->fd, socket);
	if(fd < 0){
		printk("port_connection returned %d\n", -fd);
		return;
	}
	conn = kmalloc(sizeof(*conn), GFP_ATOMIC);
	if(conn == NULL){
		printk("port_interrupt : failed to allocate connection\n");
		close(fd);
		return;
	}
	*conn = ((struct connection) 
		{ list :	LIST_HEAD_INIT(conn->list),
		  fd :		fd,
		  socket : 	{ socket[0], socket[1] } });
	list_add(&conn->list, &port->connections);
	up(&port->sem);
} 

void *port_data(int port_num)
{
	struct list_head *ele;
	struct port_list *port;
	struct port_dev *dev;
	int fd;

	list_for_each(ele, &ports){
		port = list_entry(ele, struct port_list, list);
		if(port->port == port_num) goto found;
	}
	port = kmalloc(sizeof(struct port_list), GFP_KERNEL);
	if(port == NULL){
		printk(KERN_ERR "Allocation of port list failed\n");
		return(NULL);
	}

	fd = port_listen_fd(port_num);
	if(fd < 0){
		printk(KERN_ERR "binding to port %d failed, errno = %d\n",
		       port_num, -fd);
		return(NULL);
	}
	if(um_request_irq(ACCEPT_IRQ, fd, port_interrupt, 
			  SA_INTERRUPT | SA_SHIRQ, "port", port)){
		printk(KERN_ERR "Failed to get IRQ for port %d\n", port_num);
		return(NULL);
	}

	*port = ((struct port_list) 
		{ list : 	LIST_HEAD_INIT(port->list),
		  sem :		__SEMAPHORE_INITIALIZER(port->sem, 0),
		  lock :	SPIN_LOCK_UNLOCKED,
		  port : 	port_num,
		  fd : 		fd,
		  connections :	LIST_HEAD_INIT(port->connections) });
	list_add(&port->list, &ports);

 found:
	dev = kmalloc(sizeof(struct port_dev), GFP_KERNEL);
	if(dev == NULL){
		printk(KERN_ERR "Allocation of port device entry failed\n");
		return(NULL);
	}

	*dev = ((struct port_dev) 
		{ port : 	port,
		  fd :		-1 });
	return(dev);
}

int port_wait(void *data, int *socket_out)
{
	struct port_dev *dev = data;
	struct connection *conn;

	if(down_interruptible(&dev->port->sem)) return(-ERESTARTSYS);
	spin_lock(&dev->port->lock);
	conn = list_entry(dev->port->connections.next, struct connection, 
			  list);
	list_del(&conn->list);
	spin_unlock(&dev->port->lock);

	dev->fd = conn->fd;
	socket_out[0] = conn->socket[0];
	socket_out[1] = conn->socket[1];
	kfree(conn);

	return(dev->fd);
}

void port_kern_free(void *d)
{
	struct port_dev *dev = d;

	kfree(dev);
}

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
