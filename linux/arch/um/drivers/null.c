/* 
 * Copyright (C) 2002 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#include <errno.h>
#include <fcntl.h>
#include "chan_user.h"

static int null_chan;

void *null_init(char *str, int device, struct chan_opts *opts)
{
	return(&null_chan);
}

int null_open(int input, int output, int primary, void *d)
{
	return(open("/dev/null", O_RDWR));
}

int null_read(int fd, void *unused)
{
	return(-ENODEV);
}

void null_free(void *data)
{
}

struct chan_ops null_ops = {
	init:		null_init,
	open:		null_open,
	close:		generic_close,
	read:		null_read,
	write:		generic_write,
	console_write:	generic_console_write,
	window_size:	generic_window_size,
	free:		null_free,
	winch:		0,
};

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
