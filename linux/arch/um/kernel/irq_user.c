/* 
 * Copyright (C) 2000 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/time.h>
#include "user_util.h"
#include "kern_util.h"
#include "user.h"
#include "process.h"
#include "signal_user.h"

struct irq_fd {
	struct irq_fd *next;
	void *id;
	int fd;
	int irq;
	int pid;
	int events;
	int current_events;
};

static struct irq_fd *active_fds = NULL;
static struct irq_fd **last_irq_ptr = &active_fds;

static struct pollfd *pollfds = NULL;
static int pollfds_num = 0;
static int pollfds_size = 0;

extern int io_count, intr_count;

void sigio_handler(int sig, struct uml_pt_regs *regs)
{
	struct irq_fd *irq_fd, *next;
	int i, n;

	if(smp_sigio_handler()) return;
	pushpsli();
	while(1){
		if((n = poll(pollfds, pollfds_num, 0)) < 0){
			if(errno == EINTR) continue;
			printk("sigio_handler : poll returned %d, "
			       "errno = %d\n", n, errno);
			break;
		}
		if(n == 0) break;

		irq_fd = active_fds;
		for(i = 0; i < pollfds_num; i++){
			if(pollfds[i].revents != 0){
				irq_fd->current_events = pollfds[i].revents;
				pollfds[i].events = 0;
			}
			irq_fd = irq_fd->next;
		}

		for(irq_fd = active_fds; irq_fd != NULL; irq_fd = next){
			/* This mysterious assignment protects us against
			 * the irq handler freeing the irq from under us.
			 */
			next = irq_fd->next;
			if(irq_fd->current_events != 0){
				irq_fd->current_events = 0;
				do_IRQ(irq_fd->irq, regs);
			}
		}
	}
	myrei();
}

static int prepare_fd_async(int fd, int pid)
{
	int retval;

	if((retval = fcntl(fd, F_SETFL, O_ASYNC | O_NONBLOCK)) < 0){
		printk("Failed to set O_ASYNC and O_NONBLOCK on fd # %d, "
		       "errno = %d\n", fd, errno);
		return(-retval);
	}

	if(((retval = fcntl(fd, F_SETSIG, SIGIO)) < 0) ||
	   ((retval = fcntl(fd, F_SETOWN, pid)) < 0)){
		printk("Failed to fcntl F_SETOWN (or F_SETSIG) "
		       "fd %d to pid %d, errno = %d\n", fd, pid, errno);
		return(-retval);
	}

	return(0);
}

int activate_ipi(int fd, int pid)
{
	return prepare_fd_async(fd, pid);
}

int activate_fd(int irq, int fd, void *dev_id)
{
	struct irq_fd *new_fd;
	int pid, retval, events = POLLIN | POLLPRI;

	for(new_fd = active_fds;new_fd;new_fd = new_fd->next){
		if(new_fd->fd == fd){
			printk("Registering fd %d twice\n", fd);
			printk("Irqs : %d, %d\n", new_fd->irq, irq);
			printk("Ids : 0x%x, 0x%x\n", new_fd->id, dev_id);
			return(-EIO);
		}
	}
	pid = cpu_tasks[0].pid;
	if ((retval = prepare_fd_async(fd, pid)) != 0)
		return(retval);
	new_fd = um_kmalloc(sizeof(*new_fd));
	if(new_fd == NULL) return(-ENOMEM);
	pollfds_num++;
	if(pollfds_num > pollfds_size){
		struct pollfd *tmp_pfd;

		tmp_pfd = um_kmalloc(pollfds_num * sizeof(pollfds[0]));
		if(tmp_pfd == NULL){
			pollfds_num--;
			return(-ENOMEM);
		}
		if(pollfds != NULL){
			memcpy(tmp_pfd, pollfds,
			       sizeof(pollfds[0]) * pollfds_size);
			kfree(pollfds);
		}
		pollfds = tmp_pfd;
		pollfds_size = pollfds_num;
	}
	*new_fd = ((struct irq_fd) { next : 		NULL,
				     id :		dev_id,
				     fd :		fd,
				     irq :		irq,
				     pid : 		pid,
				     events :		events,
				     current_events: 	0 } );

	*last_irq_ptr = new_fd;
	last_irq_ptr = &new_fd->next;

	pollfds[pollfds_num - 1].fd = fd;
	pollfds[pollfds_num - 1].events  = events;
	pollfds[pollfds_num - 1].revents = 0;
	return(0);
}

static void free_irq_by_cb(int (*test)(struct irq_fd *, void *), void *arg)
{
	struct irq_fd **prev;
	int i = 0;

	prev = &active_fds;
	while(*prev != NULL){
		if((*test)(*prev, arg)){
			struct irq_fd *old_fd = *prev;
			if(pollfds[i].fd != (*prev)->fd){
				printk("free_irq_fd - mismatch between "
				       "active_fds and pollfds, fd %d vs %d\n",
				       (*prev)->fd, pollfds[i].fd);
				return;
			}
			memcpy(&pollfds[i], &pollfds[i + 1],
			       (pollfds_num - i - 1) * sizeof(pollfds[0]));
			pollfds_num--;
			if(last_irq_ptr == &old_fd->next) 
				last_irq_ptr = prev;
			*prev = (*prev)->next;
			kfree(old_fd);
			continue;
		}
		prev = &(*prev)->next;
		i++;
	}
}

static int same_dev(struct irq_fd *irq, void *dev)
{
	return(irq->id == dev);
}

void free_irq_by_dev(void *dev)
{
	free_irq_by_cb(same_dev, dev);
}

static int same_fd(struct irq_fd *irq, void *fd)
{
	return(irq->fd == *((int *) fd));
}

void free_irq_by_fd(int fd)
{
	free_irq_by_cb(same_fd, &fd);
}

static struct irq_fd *find_irq_by_fd(int fd, int *index_out)
{
	struct irq_fd *irq;
	int i = 0;
	
	for(irq=active_fds; irq != NULL; irq = irq->next){
		if(irq->fd == fd) break;
		i++;
	}
	if(irq == NULL){
		printk("find_irq_by_fd doesn't have descriptor %d\n", fd);
		return(NULL);
	}
	if(pollfds[i].fd != fd){
		printk("find_irq_by_fd - mismatch between active_fds and "
		       "pollfds, fd %d vs %d, need %d\n", irq->fd, 
		       pollfds[i].fd, fd);
		return(NULL);
	}
	*index_out = i;
	return(irq);
}

void reactivate_fd(int fd)
{
	struct irq_fd *irq;
	int i;

	irq = find_irq_by_fd(fd, &i);
	if(irq == NULL) return;
	pollfds[i].events = irq->events;
}

void forward_ipi(int fd, int pid)
{
	if(fcntl(fd, F_SETOWN, pid) < 0){
		int save_errno = errno;
		if(fcntl(fd, F_GETOWN, 0) != pid){
			printk("forward_ipi: F_SETOWN failed, fd = %d, "
			       "me = %d, target = %d, errno = %d\n", fd, 
			       getpid(), pid, save_errno);
		}
	}
}

void forward_interrupts(int pid)
{
	struct irq_fd *irq;

	for(irq=active_fds;irq != NULL;irq = irq->next){
		if(fcntl(irq->fd, F_SETOWN, pid) < 0){
			int save_errno = errno;
			if(fcntl(irq->fd, F_GETOWN, 0) != pid){
				/* XXX Just remove the irq rather than
				 * print out an infinite stream of these
				 */
				printk("Failed to forward %d to pid %d, "
				       "errno = %d\n", irq->fd, pid, 
				       save_errno);
			}
		}
		irq->pid = pid;
	}
}

void init_irq_signals(int on_sigstack)
{
	int flags;

	flags = on_sigstack ? SA_ONSTACK : 0;
	set_handler(SIGVTALRM, (__sighandler_t) alarm_handler, 
		    flags | SA_NODEFER | SA_RESTART, SIGUSR1, SIGIO, 
		    SIGWINCH, -1);
	set_handler(SIGIO, (__sighandler_t) irq_handler, flags | SA_RESTART,
		    SIGUSR1, SIGIO, SIGWINCH, -1);
	signal(SIGWINCH, SIG_IGN);
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
