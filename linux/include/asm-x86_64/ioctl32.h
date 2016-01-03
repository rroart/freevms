#ifndef IOCTL32_H
#define IOCTL32_H 1

int sys_ioctl(unsigned int, unsigned int, unsigned long);

/*
 * Register an 32bit ioctl translation handler for ioctl cmd.
 *
 * handler == NULL: use 64bit ioctl handler.
 * arguments to handler:  fd: file descriptor
 *                        cmd: ioctl command.
 *                        arg: ioctl argument
 *                        struct file *file: file descriptor pointer.
 */

#endif
