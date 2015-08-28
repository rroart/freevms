#ifndef CMU_TYPES_H
#define CMU_TYPES_H

typedef unsigned long int time_t;

#define makedev yes /* this is not a proper definition, however it is used
* to test if types is loaded.
*/

typedef unsigned int uid_t;
typedef unsigned int gid_t;

#ifndef __SOCKET_TYPEDEFS
#ifndef CADDR_T
#define CADDR_T
typedef char * caddr_t;
#endif
typedef unsigned short  u_short;
typedef unsigned int    u_int;
typedef unsigned long   u_long;
typedef unsigned char   u_char;
#define __SOCKET_TYPEDEFS
#endif


/*
 * Select uses bit masks of file descriptors in longs.
 * These macros manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here
 * should be >= NOFILE (param.h).
 */

#define MAX_NOFILE  32
#define NBBY         8      /* number of bits in a byte */

#ifndef FD_SETSIZE
#define FD_SETSIZE  MAX_NOFILE
#endif  /* FD_SETSIZE */

/* How many things we'll allow select to use. 0 if unlimited */
#define MAXSELFD    MAX_NOFILE
typedef long    fd_mask;
#define NFDBITS (sizeof(fd_mask) * NBBY)    /* bits per mask (power of 2!)*/
#define NFDSHIFT 5              /* Shift based on above */

#ifndef howmany
#define howmany(x, y)   (((x)+((y)-1))/(y))
#endif /* howmany */

/*
 * fd setup for:
 *  - VAXCRTL and DEC TCP/IP Services for OpenVMS
 *  - LIBCMU and CMU-OpenVMS/IP v6.6
 * Both use one longword bitmask for 32 file descriptors.
 * Note: Wollongong WIN/TCP is different...  DEFINE WINTCP for WIN/TCP support
 */
#ifndef WINTCP
#define _FD_SETUP_
typedef fd_mask fd_set;

#define FD_SET(n, p)    (*(p) |= (1 << ((n) % NFDBITS)))
#define FD_CLR(n, p)    (*(p) &= ~(1 << ((n) % NFDBITS)))
#define FD_ISSET(n, p)  (*(p) & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)  memset((char *)(p), 0, sizeof(*(p)))
#endif

#ifdef WINTCP
#define _FD_SETUP_
/*
 * fd setup for Wollongong's WIN/TCP
 */
typedef unsigned short fd_channel;

typedef struct fd_set
{
fd_mask     fds_bits[howmany(FD_SETSIZE, NFDBITS)];
    fd_channel  fds_chan[howmany(FD_SETSIZE, NFDBITS)][NFDBITS];
} fd_set;

#define FD_SET(n, p)    _$fdset(n, p)
#define FD_CLR(n, p)    _$fdclr(n, p)
#define FD_ISSET(n, p)  (n == 0 ? n : _$fdisset(n, p))
#define FD_ZERO(p)  _$fdzero(p)

#endif /* WINTCP */

#ifndef _FD_SETUP_
#define fd_set "fd_set not defined in header file sys/types.h"
#endif
#endif /* CMU_TYPES_H */
