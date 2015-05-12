/* atmapi.h - ATM API user space/kernel compatibility */

/* Written 1999,2000 by Werner Almesberger, EPFL ICA */


#ifndef _LINUX_ATMAPI_H
#define _LINUX_ATMAPI_H

/*
 * Opaque type for kernel pointers. Note that _ is never accessed. We need
 * the struct in order hide the array, so that we can make simple assignments
 * instead of being forced to use memcpy. It also improves error reporting for
 * code that still assumes that we're passing unsigned longs.
 *
 * Convention: NULL pointers are passed as a field of all zeroes.
 */

typedef struct
{
    unsigned char _[8];
} atm_kptr_t;

#endif
