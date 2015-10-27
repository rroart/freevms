/*
 * phyio.h
 *
 * Definition of Physical I/O routines
 *
 *  Created on: Oct 17, 2015
 *      Author: guido
 */

#ifndef PHYIO_H
#define PHYIO_H

#include <ucbdef.h>

struct phyio_info
{
    unsigned status;
    unsigned sectors;
    unsigned sectorsize;
};

/**
 * \fn phyio_show
 * \brief Print some statistics about the other phyio calls.
 */
void phyio_show(void);

/**
 * \fn phyio_init
 * \brief Prepare a device for use by future read/write calls.
 * The device name should map to a local device. The call needs to return a handle (channel, file handle,
 * reference number...) for future reference, and optionally some device information.
 */
int phyio_init(int devlen, char *devnam, struct file **handle, struct phyio_info *info, struct _dt_ucb * ucb);
#if 0
int phyio_init(int devlen, char *devnam, unsigned *handle, struct phyio_info *info);
#endif

/**
 * \fn phyio_close
 * \brief Close a previously initialized device.
 */
int phyio_close(struct file * handle);
#if 0
int phyio_close(unsigned handle);
#endif

/**
 * \fn phyio_read
 * \brief Read data from a device.
 * Returns a specified number of bytes into a buffer from the start of a 512 byte block on the device referred to by the handle.
 */
int phyio_read(struct file * handle, unsigned block, unsigned length, char *buffer);
#if 0
int phyio_read(unsigned handle, unsigned block, unsigned length, char *buffer);
#endif

/**
 * \fn phyio_write
 * \brief Write data to a device.
 * Writes a number of bytes out to a 512 byte block address on a device.
 */
int phyio_write(struct file * handle, unsigned block, unsigned length, char *buffer);
#if 0
int phyio_write(unsigned handle, unsigned block, unsigned length, char *buffer);
#endif

#endif /* PHYIO_H */
