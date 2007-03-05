/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * LIBCMU, Copyright (C) 1993,1994 by Mike O'Malley
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Facility:	LIBCMU
 *
 * Abstract:	Socket interface routines for CMU-OpenVMS/IP v6.6
 *
 * Module Description:
 *	This collection of routines provides the standard `C' programming
 * interface CMU-OpenVMS/IP network transport.
 *
 * Routine provided:
 *	int socket(int domain, int type, int protocol)
 *	int bind(int s, struct sockaddr *name, int namelen)
 *	int connect(int s, struct sockaddr *name, int namelen)
 *	int listen (int s, int backlog)
 *	int accept(int s, struct sockaddr *addr, int *addrlen)
 *	int recv(int s, char *buf, int len, int flags)
 *	int recvfrom(int s, char *buf, int len, int flags,
 *			struct sockaddr *from, int *fromlen)
 *	int send(int s, char *msg, int len, int flags)
 *	int sendto(int s, char *msg, int len, int flags, 
 *			struct sockaddr *to, int tolen)
 *	int shutdown(int s, int how)
 *
 *	int select(int nfds, int *readfds, int *writefds, int *execptfds,
 *			struct timeval *timeout)
 *
 *	int getsockname(int s, struct sockaddr *name, int *namelen)
 *	int getpeername(int s, struct sockaddr *name, int *namelen)
 *	int getsockopt(int s, int level, int optname,
 *			char *optval, int *optlen)
 *	int setsockopt(int s, int level, int optname,
 *			char *optval, int *optlen)
 *
 *	int gethostname(char *name, int namelen)
 *	struct servent *getservbyname(char *name, char *proto)
 *	struct servent *getservbyport(int port, char *proto)
 *	struct hostent *gethostbyname(char *name)
 *	struct hostent *gethostbyaddr(char *addr, int len, int type)
 *
 *	int ioctl(int s, int request, char *argp)
 *	int fcntl(int s, int request, int arg)
 *
 * The following routines were named to prevent conflict with routine names
 * in the VAXCRTL:
 *	int cmu_read(int s, char *buf, int len)
 *	int cmu_write(int s, char *msg, int len, int flags)
 *	int cmu_close(int s)
 *
 * The following routines may be called from outside this library:
 *	int *cmu_stdin_open(char *name)
 *	int cmu_stdin_read(flags,buf,len,prompt,mask)
 *	int cmu_get_sdc(int s)
 *	int cmu_trnlnm(char *table, char *name, char *mode, char *buff)
 *	int cmu_get_errno(NetIO_Status_Block *iosb)
 *
 * The following routines should not be accessed outside this library:
 *	int cmu_listen_accept(int s)
 *	int cmu_queue_listen(int s)
 *	int cmu_read_ast(int s)
 *	int cmu_write_ast(int s)
 *	int cmu_queue_net_read(int s)
 *
 * Acknowledgements:
 *	Guidence, concepts, examples derived from the works of:
 *		UNIXSHR		- unknown
 *		NETLIB		- Matt Madison, RPI
 * Author:
 *	Mike O'Malley (mlo)				      September 1993
 *	Digital Equipment Corp.			Sandia National Laboratories
 *	Digital Consulting	   Scientific Computing Center, User Support
 *	Albuquerque, NM
 *      Mike.OMalley@aqo.mts.dec.com			  mlomall@sandia.GOV
 *
 * With modifications by:
 *	 Chen He (che) (617)566-0001 Ext.2919
 *	 IDX Systems Corp.
 *	 he@chen.idx.com
 *
 * THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT NOTICE.  THIS SOFTWARE SHOULD
 * NOT BE CONSTRUED AS SUPPORTED, OR A COMMITMENT OF SUPPORT, BY
 * DIGITAL EQUIPMENT CORPORATION.
 *
 * Modifications:
 *	14-MAR-1994 mlo 1.2
 *		Added getpeername routine.
 *		Modified all qio calls to use vaxc$errno for status; modified
 *		get_cmu_errno to return EVMSERR when the IOSB is zero; perror()
 *		will now print the vms specific error.
 *	14-MAR-1994 mlo 1.1.3
 *		Modified all qio calls that use cmu_read_ast to NOT use the
 *		event flag. Set the event flag in the cmu_read_ast routine.
 *		In cmu_read_ast make the socket invalid if a socket shutdown
 *		was detected.
 *		In select insure that all sockets are still valid.
 *		Modified exception conditions in cmu_read_ast routine.
 *	 8-FEB-1994 che 1.1.2
 *		Fixed accept function to return the remote host information.
 *	 4-FEB-1994 che 1.1.1
 *		Fixed problem in gethostbyaddr; it worked after called 
 *		gethostbyname but would get access violation when it was 
 *		called first time.
 *	18-JAN-1994 mlo 1.1.0
 *		Removed references to `sys/', `vnet/', `netinet/' from
 *		include files.
 *	18-JAN-1994 mlo 1.0.8
 *		Fixed serious problems in gethostbyaddr; never worked!
 *	10-DEC-1993 mlo 1.0.7
 *		In cmu_read_ast detect `socket close by peer' event.
 *	10-DEC-1993 mlo 1.0.6
 *		detect ENOCONN in connect.  Seems that the TCP$OPEN qio does
 *		not fail when the remote system is not reachable.
 *	28-NOV-1993 mlo 1.0.5
 *		in recvfrom don't check error status if socket is not ready to
 *		read.  Caused bad return status.
 *	 8-NOV-1993 mlo 1.0.4
 *		detect numeric address passed to gethostbyname
 *	18-OCT-1993 mlo 1.0.3
 *		corrected return from cmu_get_errno
 *	 7-OCT-1993 mlo 1.0.2
 *		add write and exception events to select.
 *	 6-OCT-1993 mlo 1.0.1
 *		add better error reporting via errno
 *	16-SEP-1993 mlo 1.0.0
 *		original
 */
#ifdef VAXC
#module LIBCMU "v1.2"
#endif

/*
 * Include files
 */

/*
 * If not defined types.h will define it for us.  Don't want to handle more
 * than 32 file descriptors at this time.
 */
#define FD_SETSIZE 32

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <netdb.h>

#include <types.h>
#include <ctype.h>
#include <time.h>
#include <file.h>
#include <ioctl.h>
#include <socket.h>

#include <if.h>
#include <in.h>

#include <ssdef.h>
#include <descrip.h>
#include <lnmdef.h>
#include <msgdef.h>
#include <iodef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <syidef.h>

/*
 * CMU-OpenVMS/IP specific definitions
 */
#include "netconfig.h"
#include "netcommon.h"
#include "network.h"
#include "neterror.h"

/*
 * And a LIBCMU specific header
 */
#include "libcmu.h"

/*
 * forward declarations
 */
int cmu_get_errno(NetIO_Status_Block *iosb);
int cmu_trnlnm(char *table, char *name, char *mode, char *buff, int len);
int cmu_listen_accept(int s);
int cmu_queue_listen(int s);
int cmu_read_ast(int s);
int cmu_write_ast(int s);
int cmu_queue_net_read(int s);

/*
 * Global static variables
 */

/*
 * Our private file descriptor table
 */
static struct FD_ENTRY *sd[FD_SETSIZE];

/*
 * The default interface name.
 */
#define INET_DEVICE_NAME "INET$DEVICE"
static readonly struct dsc$descriptor inet_device = 
	{11, DSC$K_DTYPE_T, DSC$K_CLASS_S, INET_DEVICE_NAME };

/*
 * for system information call to get MAXBUF size.
 */
static int MAXBUF;
static struct ITEM_LIST syinfo[2] = {
	4, SYI$_MAXBUF, &MAXBUF, 0,
	0,           0,       0, 0  };

/*
 * File descriptor mask to keep track of i/o events.
 */
static fd_set sys_validfds;	/* is sd valid		*/
static fd_set sys_readfds;	/* is sd ready to read	*/
static fd_set sys_writefds;	/* is sd ready to write */
static fd_set sys_exceptfds;	/* does sd have an error*/

/*
 * Accept event flag so we know when a request comes in
 */
static int accept_net_event;


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int socket(int domain, int type, int protocol)
 *
 * Description:
 *	Creates an endpoint of a connection.
 *
 * domain   - AF_INET is the only address domain supported at this time.
 * type     - SOCK_STREAM or SOCK_DGRAM
 * protocol - IPPROTO_TCP, IPPROTO_UDP.  If IPPROTO_IP (default) is specified
 *	      protocol will be chosen that matches `type'.
 *
 * Returns:
 *  If successful a socket number is returned, otherwise, a -1 is returned and
 *  errno is set.
 */
int socket(domain,type,protocol)
int domain,type,protocol;
{

int	s;

    /*
     * Verify the address family (domain)
     */
    if ( domain != AF_INET ) {
	/*
	 * we don't handle any other address formats.
	 */
	errno = EAFNOSUPPORT;
	return(-1);
    }	

    /*
     * Use the dup() routine to aquire a unique discriptor.
     * Make sure it's in our range.
     */
    s = dup(0);
    if (s<0)
	return(-1);
    else if ((s < 0) || (s > FD_SETSIZE)) {
	errno = ENFILE;
	return (-1);
    }
    FD_SET(s,&sys_validfds);

    /*
     * Allocate a file descriptor data structure and initialize it.
     */
    sd[s] = calloc( 1, sizeof(struct FD_ENTRY));
    sd[s]->domain	= domain;
    sd[s]->type		= type;

    /*
     * If not specified select a default protocol
     */
    if (protocol == IPPROTO_IP) {
	switch(type) {
	    case SOCK_STREAM :
		sd[s]->protocol	= IPPROTO_TCP;
		break;
	    case SOCK_DGRAM :
	    default:
		sd[s]->protocol	= IPPROTO_UDP;
	}
    }
    else
	switch(protocol) {
	    case IPPROTO_TCP:
	    case IPPROTO_UDP:
		sd[s]->protocol = protocol;
		break;
	    default:
		errno = EPROTONOSUPPORT;
		return(-1);
	}


    /*
     * grab the OpenVMS SYSGEN parameter MAXBUF and set the default buffer size
     * to the minimum of *_SO_RCVBUF_DEF or MAXBUF
     */
    if (MAXBUF == 0) {
	vaxc$errno = sys$getsyi( 0, 0, 0, &syinfo, 0, 0, 0);
	if (vaxc$errno != SS$_NORMAL) {
		errno = EVMSERR;
		return(-1);
	}
    }

    if (sd[s]->protocol == IPPROTO_TCP)
	sd[s]->rcvbufsize = 
			MAXBUF < TCP_SO_RCVBUF_DEF ? MAXBUF : TCP_SO_RCVBUF_DEF;
    else
	sd[s]->rcvbufsize =
			MAXBUF < UDP_SO_RCVBUF_DEF ? MAXBUF : UDP_SO_RCVBUF_DEF;

    /*
     * assign a channel to the network device
     */
    vaxc$errno = sys$assign(&inet_device,&sd[s]->chan,0,0);
    if (vaxc$errno != SS$_NORMAL) {
	errno = EVMSERR;
	return(-1);
    }
    return(s);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int bind(int s, struct sockaddr *name, int namelen)
 *
 * Description:
 *	binds a name (address/port) to a socket.  Socket (s) must have been
 * created with call to socket.
 *
 * s       - valid socket descriptor
 * name	   - address of sockaddr structure for local host port information
 * namelen - length of the name structure in bytes
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int bind(s,name,namelen)
int			s;	/* socket to bind to	*/
struct sockaddr		*name;	/* name stuff		*/
int			namelen;/* length of name stuff	*/
{
int	status;
int	protocol;
struct	sockaddr_in *my = name;

    /*
     * Verify the address family (domain)
     */
    if ( name->sa_family != AF_INET ) {
	/*
	 * we don't handle any other address formats.
	 */
	errno = EAFNOSUPPORT;
	return(-1);
    }	

    /*
     * check for valid socket.
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * see if it's already named
     */
    if (sd[s]->flags & SD_BIND) {
	errno = EINVAL;
	return(-1);
    }

    /*
     * copy/save sockaddr info.
     */
    sd[s]->mylen = namelen;
    memcpy(&sd[s]->my, name, namelen);

    /*
     * If the bind is to a connectionless mode socket (IPPORTO_UDP) then
     * TCP$OPEN the channel.
     */
    if (sd[s]->protocol == IPPROTO_UDP) {
	/*
	 * Open the communication channel
	 */
	vaxc$errno = sys$qiow( 0, sd[s]->chan, TCP$OPEN, &sd[s]->read_iosb, 0, 0,
		0, 0,
		ntohs(my->sin_port),
		(MODE_UDP_A_struct | MODE_OpenNoWait),
		U$UDP_Protocol, 0);

	if (vaxc$errno != SS$_NORMAL) {
	    errno = cmu_get_errno(&sd[s]->read_iosb);
	    sys$qio(0,sd[s]->chan,TCP$ABORT,0,0,0,0,0,0,0,0,0);
	    FD_SET(s,&sys_exceptfds);
	    sys$setef(sd[s]->ef);
	    return(-1);
	}

	FD_SET(s,&sys_writefds);
	FD_CLR(s,&sys_exceptfds);

	sd[s]->flags |= SD_CONNECTED;

	/*
	 * queue a read to the socket
	 */
	status = cmu_queue_net_read(s);
	if (status != 0)
	    return(-1);
    }
    sd[s]->flags |= SD_BIND;
    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int connect(int s, struct sockaddr *name, int namelen)
 *
 * Description:
 *	initiates a connection on a socket.
 *
 * s       - valid socket descriptor
 * name	   - pointer to an address structure for the remote port
 * namelen - length of the name structure in bytes
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int connect(s,name,namelen)
int s;
struct sockaddr *name;
int namelen;
{
int	status;
int	to_addr, to_port, my_port;
struct	sockaddr_in *p;    
    /*
     * Verify the address family (domain) we are to connect to.
     */
    if ( name->sa_family != AF_INET ) {
	/*
	 * we don't handle any other address formats.
	 */
	errno = EAFNOSUPPORT;
	return(-1);
    }	

    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * see if it's already connected
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0) {
	errno = EISCONN;
	return(-1);
    }

    /*
     * copy/save remote sockaddr info.
     */
    sd[s]->tolen = namelen;
    memcpy(&sd[s]->to, name, namelen);

    p = &sd[s]->to;
    to_addr = p->sin_addr.s_addr;
    to_port = ntohs(p->sin_port);
    p = &sd[s]->my;
    my_port = ntohs(p->sin_port);
    /*
     * Open and connect to the remote system
     */
    if (sd[s]->protocol == IPPROTO_TCP)
	vaxc$errno = sys$qiow( 0, sd[s]->chan,
			TCP$OPEN, &sd[s]->read_iosb, 0, 0,
			&to_addr,		/* host to connect to */
			 to_port,		/* remote port	      */
			 my_port,		/* local port         */
			(MODE_TCP_SendSYN | MODE_OpenWait | MODE_Addr32bit),
			U$TCP_Protocol,
			0);
    else
	vaxc$errno = sys$qiow( 0, sd[s]->chan,
			TCP$OPEN, &sd[s]->read_iosb, 0, 0,
			&to_addr,
			to_port,
			my_port,
			(MODE_UDP_A_struct | MODE_OpenWait | MODE_Addr32bit),
			U$UDP_Protocol,
			0);

    /*
     * Now queue a read on the socket.  If the connect failed then the read
     * will also fail right away with NET$_CDE (%IPACP-E-CDE, Connection does
     * not exist).
     */
    if (vaxc$errno == SS$_NORMAL)
	if ((cmu_queue_net_read(s)) == -1)
	    return(-1);

    /*
     * could there be a timing problem here?  Will the read AST complete (on
     * error) before the next statement?  Lets wait a sec anyway just in case.
     */
    sleep(1);

    vaxc$errno = sd[s]->read_iosb.NSB$STATUS;

    /*
     * 0 = read in progress, 1 = read complete data waiting.
     */
    if ((vaxc$errno != 0) && (vaxc$errno != 1)) {
	errno = cmu_get_errno(&sd[s]->read_iosb);
	FD_SET(s,&sys_exceptfds);
	sys$setef(sd[s]->ef);
	return(-1);
    }

    FD_SET(s,&sys_writefds);
    FD_CLR(s,&sys_exceptfds);

    sd[s]->flags |= SD_CONNECTED;
    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * 
 * int listen (int s, int backlog)
 *
 * Description:
 *	Sets the maximum limit of outstanding connection requests for a socket
 * that is connection-oriented.
 *
 * s	   - a valid socket descriptor of type SOCK_STREAM
 * backlog - maximum number of pending connections that may be queued
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int listen(s,backlog)
int s;
int backlog;
{

    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    if ((sd[s]->protocol != IPPROTO_TCP) || (sd[s]->type != SOCK_STREAM)) {
	errno = EOPNOTSUPP;
	return(-1);
    }

    sd[s]->backlog = backlog;
    sd[s]->sock_opts |= SO_ACCEPTCONN;	/* socket has had listen() */

    /*
     * Allocate an event flag used to signal a blocked accept call
     */
    lib$get_ef(&accept_net_event);
    sys$clref(accept_net_event);

    /*
     * Clear the listen file descriptor 
     */
    FD_CLR(s,&sys_readfds);

    /*
     * Queue the socket listen
     */
    if (cmu_queue_listen(s) == 0)
	return(0);
    else
	return(-1);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int accept(int s, struct sockaddr *addr, int *addrlen)
 *
 * Description:
 *	Accepts a connection on a socket.
 *
 * s	   - valid socket that is listening
 * addr	   - sockaddr address structure filled with information from the accepted
 *           connection
 * addrlen - length of returned address structure
 *
 * Returns:
 *	Nonnegative integer that is the descriptor for the accepted socket or
 * -1 on error.  Addition error information is specified in the global
 * variable errno.
 */
int accept( s, addr, addrlen)
int	s;
struct	sockaddr *addr;
int	*addrlen;
{
int	ns, status;
struct	backlogEntry *entry;

    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * look see if socket is listening
     */
    if ((sd[s]->sock_opts & SO_ACCEPTCONN) == 0) {
	errno = EBADF;
	return(-1);
    }

    /*
     * see if anything is there
     */
CHECK_QUEUE:
    /*
     * check the socket for incomming connections.
     */
    if (!(FD_ISSET(s,&sys_readfds))) {
	/*
	 * block if necessary
	 */ 
	if ((sd[s]->ioctl_opts & O_NDELAY) == 0) {
	    sys$waitfr(accept_net_event);
	    /*
	     * see if the socket was shutdown
	     */
	    if ((sd[s]->ioctl_opts & FREAD) != 0) {
		return(0);
	    }
	}
	else {
	    errno = EWOULDBLOCK;
	    return(-1);
	}
    }

    /*
     * Clear the event flag
     */
    sys$clref(accept_net_event);

    /*
     * remove the first entry from the backlog queue
     */
    entry = sd[s]->backlogQueue;
    sd[s]->backlogQueue = entry->flink;

    /*
     * save the socket number and free the backlogEntry
     */
    ns = entry->sock;
    cfree(entry);

    /*
     * queue the first read to the accepted connection
     */
    status = cmu_queue_net_read(ns);
    if (status != 0)
	return(-1);

    /*
     * fill in addr with the connecting entity information
     */
    if (addr != NULL)
    	memcpy (addr,&(sd[ns]->to), (*addrlen < sizeof(struct sockaddr)
				   ? *addrlen : sizeof(struct sockaddr)));

    /*
     * give the user a new socket descriptor
     */
    return(ns);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int recv(int s, char *buf, int len, int flags)
 *
 * Description:
 *	Receive bytes from a connected socket.
 * 
 * s	 - a valid socket descriptor
 * buf	 - address of buffer to where input data is placed
 * len	 - max size of buf
 * flags - 0 or MSG_PEEK may be specified.
 *
 * Returns:
 *	Number of bytes read from the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int recv(s,buf,len,flags)
int	s;
char	*buf;
int	len, flags;
{
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * Must be connected
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0)
	return(recvfrom(s,buf,len,flags,0,0));
    else {
	errno = EBADF;
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int recvfrom(int s, char *buf, int len, int flags,
 *			struct sockaddr *from, int *fromlen)
 *
 * Description:
 *	receives bytes from a socket from any source.
 *
 * s	 - a valid socket descriptor
 * buf	 - address of buffer to where input data is placed
 * len	 - max size of buf
 * flags - 0 or MSG_PEEK may be specified.
 * from	 - address of sockaddr structure address of the socket which the data
 *         is received from.  May be specified as 0; no information will be
 *         returned.
 * fromlen - length of from structure returned.
 *
 * Returns:
 *	Number of bytes read from the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int recvfrom(s,buf,len,flags,from,fromlen)
int	s;
char	*buf;
int	len, flags;
struct	sockaddr *from;
int	*fromlen;
{
int	size, offset;
struct	sockaddr_in *frm;

    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * make sure the socket is not shutdown
     */
    if ((sd[s]->ioctl_opts & FREAD) != 0) {
	errno = EPIPE;
	return(-1);
    }

CHECKforDATA:
    /*
     * check for a socket error (if the socket read has completed!)
     */
    if (FD_ISSET(s,&sys_readfds))
	if (sd[s]->read_iosb.NSB$STATUS != SS$_NORMAL) {
	    errno = cmu_get_errno(&sd[s]->read_iosb);
	    return(-1);
	}

    /*
     * Check the file descriptor for data ready on the socket. Block or return
     * based on data availability.
     */
    if (!(FD_ISSET(s,&sys_readfds))) {
	if (sd[s]->ioctl_opts & O_NDELAY) {
	    errno = EWOULDBLOCK;
	    return(-1);
	}
	else {
	    sys$waitfr(sd[s]->ef);
	    goto CHECKforDATA;
	}
    }

    /*
     * move the from data if the user asked for it.
     */
    if (from != NULL) {
	frm = from;
	frm->sin_port = htons(sd[s]->rcvfrom.IPADR$DST_PORT);
	frm->sin_addr.s_addr = sd[s]->rcvfrom.IPADR$DST_HOST;
    }

    /*
     * data is ready on the socket.  copy it to the users buffer.
     * there could be data remaining in the receive buffer from a previous
     * read or more data in the receive buffer than what would fit in the users
     * buffer.
     */
    size = len < sd[s]->read_iosb.NSB$Byte_Count ? 
					len : sd[s]->read_iosb.NSB$Byte_Count;
    offset = sd[s]->rcvbufoffset;

    memcpy(buf, &sd[s]->rcvbuf[offset], size);

    /*
     * if this is just a peek then return without fixing up the byte count and
     * offset things.
     */
    if ((flags & MSG_PEEK) != 0)
	return (size);

    /*
     * now fix up the byte count in the iosb.  If there is and data left then
     * set the offset.
     */
    sd[s]->read_iosb.NSB$Byte_Count -= size;
    if (sd[s]->read_iosb.NSB$Byte_Count == 0) {
	/*
	 * The receive buffer has been drained; reset the offset and
	 * queue another read.
	 */
	sd[s]->rcvbufoffset = 0;
	cmu_queue_net_read(s);
    }
    else
	sd[s]->rcvbufoffset += size;

    /*
     * return the number of bytes that were copied to the users buffer.
     */
    return (size);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int send(int s, char *msg, int len, int flags)
 *
 * Description:
 *	Send bytes through a socket to its connected peer.
 *
 * s	 - a valid socket descriptor
 * buf	 - address of buffer of data to be sent
 * len	 - size of buf
 * flags - 0 or MSG_PEEK may be specified.
 *
 * Returns:
 *	Number of bytes written to the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int send(s,msg,len,flags)
int	s;
char	*msg;
int	len, flags;
{
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * Must be connected
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0)
	return(sendto(s,msg,len,flags,0,0));
    else {
	errno = EBADF;
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int sendto(int s, char *msg, int len, int flags, 
 *			struct sockaddr *to, int tolen)
 *
 * Description:
 *	Send bytes through a socket to any other socket.
 *
 * s	 - a valid socket descriptor
 * buf	 - address of buffer of data to be sent
 * len	 - size of buf
 * flags - none supported.
 * to	 - address of sockaddr structure which contains the address of the
	   socket which the data is to be written.
 * tolen - length of from structure returned.
 *
 * Returns:
 *	Number of bytes written to the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int sendto(s,msg,len,flags,to,tolen)
int	s;
char	*msg;
int	len, flags;
struct	sockaddr *to;
int	tolen;
{
struct	sockaddr_in *too;
IPADR$ADDRESS_BLOCK UDP_ADDR;
static int write_ef = 0;

    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * make sure the socket is not shutdown
     */
    if ((sd[s]->ioctl_opts & FWRITE) != 0) {
	errno = EPIPE;
	return(-1);
    }

    /*
     * in a UDP environment we could get to this point without actually opening
     * the communication channel via bind.  It might be cause the user only
     * wants a send only channel.
     */
    if ((sd[s]->flags & SD_CONNECTED) == 0) {
	vaxc$errno = sys$qiow( 0, sd[s]->chan, TCP$OPEN, &sd[s]->read_iosb, 0, 0,
		0, 0,
		0,	/* my port -- not specified */
		(MODE_UDP_A_struct | MODE_OpenNoWait),
		U$UDP_Protocol, 0);

	if (sd[s]->read_iosb.NSB$STATUS != SS$_NORMAL) {
	    errno = cmu_get_errno(&sd[s]->read_iosb);
	    return(-1);
	}
	sd[s]->flags |= SD_CONNECTED;

	FD_SET(s,&sys_writefds);
	FD_CLR(s,&sys_exceptfds);
    }

    /*
     * check the target domain (address family)
     */
    if (to != NULL)
	if (to->sa_family != AF_INET) {
	    errno = EAFNOSUPPORT;
	    return(-1);
	}

    /*
     * record who we sent it to...
     */
    if (to != NULL) {
	sd[s]->tolen = tolen;
	memcpy(&sd[s]->to, to, tolen);
    }

    too = &sd[s]->to;

    /*
     * setup the UDP target address buffer
     */
    UDP_ADDR.IPADR$SRC_PORT = 0;
    UDP_ADDR.IPADR$SRC_HOST = 0;
    UDP_ADDR.IPADR$DST_PORT = ntohs(too->sin_port);
    UDP_ADDR.IPADR$DST_HOST = too->sin_addr.s_addr;

    /*
     * get an event flag specific for writing
     */
    if (write_ef == 0)
	lib$get_ef(&write_ef);

    /*
     * clear the system write ready fds and queue the write
     */
    FD_CLR(s,&sys_writefds);
    if (sd[s]->protocol == IPPROTO_UDP)
	vaxc$errno = sys$qio( write_ef, sd[s]->chan, TCP$SEND, &sd[s]->write_iosb,
		cmu_write_ast, s,
		msg,	/* message buffer address */
		len,	/* message length */
		0,	/* unused */
		1,	/* set EXACT */
		4,	/* size of IP header buffer */
		&UDP_ADDR);	/* IP header address */
    else
	vaxc$errno = sys$qio( write_ef, sd[s]->chan, TCP$SEND, &sd[s]->write_iosb,
		cmu_write_ast, s,
		msg,	/* message buffer address */
		len,	/* message length*/
		0,	/* unused */
		0,	/* send EOL */
		0,	/* urgent flag (not yet implemented) */
		0);	/* unused in TCP */

    if (vaxc$errno == SS$_NORMAL)
	return (len);
    else {
	errno = cmu_get_errno(&sd[s]->write_iosb);
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <socket.h>
 *
 * int shutdown(int s, int how)
 *
 * Description:
 *	Shuts down all or part of a connection on a socket.
 *
 * s - a valid socket
 * how - one of:
 *	0 - no more receives
 *	1 - no more sends
 *	2 - no more receives or sends
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int shutdown(s,how)
int s, how;
{

    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * if not connected the no need to shutdown
     */
    if ((sd[s]->flags & SD_CONNECTED) == 0) {
	errno = ENOTCONN;
	return(-1);
    }

    switch (how) {
	case 2 :
	case 1 :
		sd[s]->ioctl_opts |= FWRITE;
		if (how != 2) break;
	case 0 :
		sd[s]->ioctl_opts |= FREAD;
		if ((sd[s]->sock_opts & SO_ACCEPTCONN) != 0)
		    sys$setef(accept_net_event);
		sys$cancel(sd[s]->chan);
		break;
	default :
	    errno = EINVAL;
	    return(-1);
    }
    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int select(int nfds, int *readfds, int *writefds, int *execptfds,
 *		struct timeval *timeout)
 *
 * Description:
 *	Allows the user to poll or check a group of sockets for I/O activity.
 * It can check what sockets are ready to be read or written, or what sockets
 * have a pending exception.
 *
 * Notes:
 *	This routine (and library) only handles 32 file descriptors max (0-31).
 * The `fd_set' type is long int.  To select on file descriptor 0 (stdin) the
 * routine cmu_stdin_open must be used to setup the file descriptor.  See
 * routines cmu_stdin_open and cmu_stdin_read for further information.
 *
 * nfds		- maximum file descriptor +1 to scan
 * readfds	- address of descriptor bit mask to scan for read events
 * writefds	- address of descriptor bit mask to scan for write events
 * exceptfds	- address of descriptor bit mask to scan for exception events
 * timeout	- specifies how long to wait for a read, write, or exception
 *		  event.  If timeout argument is NULL select will block until
 *		  one of the specified descriptors is ready.
 * 
 * Returns:
 *	Number of socket descriptors ready for I/O or that have exceptions, 
 * 0 if the operation timed out, -1 on error.  Addition error information is
 * specified in the global variable errno.
 */
int select(nfds,readfds,writefds,exceptfds,timeout)
int	nfds;
int	*readfds, *writefds, *exceptfds;
struct	timeval *timeout;
{
int	i;
int	maxfds, maxfds_mask, ready_fds, all_fds;
int	readyfds;

int	block, timer_ef, t[2];
char	at[20];
struct	tm *lt;
struct dsc$descriptor ascii_time = 
	{20, DSC$K_DTYPE_T, DSC$K_CLASS_S, at };

int	ef_mask;
#define EF_BASE 32

    readyfds = 0;
    FD_ZERO(&ready_fds);
    FD_ZERO(&all_fds);
    FD_ZERO(&ef_mask);

    block = timer_ef = 0;

    maxfds = nfds < FD_SETSIZE ? nfds+1 : FD_SETSIZE;

    /*
     * Don't allow more than 32 file descriptors
     */
    if (maxfds > 32) {
	errno = EBADF;
        readyfds = -1;
	goto EXIT;
    }

    if (maxfds == 32)
	maxfds_mask = 0xffffffff;
    else {
	maxfds_mask = 0;
	for (i=0; i < maxfds; i++)
	    maxfds_mask |= (1<<i);
    }

    /*
     * Clear extranious bits and check for bad file descriptors.  Gather all
     * file descriptor bits into `all_fds' for use later.
     */
    if (exceptfds != NULL) {
	*exceptfds &= maxfds_mask;
	if ((*exceptfds & sys_validfds) != *exceptfds) {
	    /*
	     * Set the exceptfds mask to indicate which fd was bad.
	     */
	    *exceptfds ^= sys_validfds;
	    errno = EBADF;
	    return(-1);
	}
	all_fds = *exceptfds;
    }

    if (writefds != NULL) {
	*writefds &= maxfds_mask;
	if ((*writefds & sys_validfds) != *writefds) {
	    /*
	     * Set the exceptfds mask to indicate which fd was bad.
	     */
	    *writefds ^= sys_validfds;
	    errno = EBADF;
	    return(-1);
	}
	all_fds |= *writefds;
    }

    if (readfds != NULL) {
	*readfds &= maxfds_mask;
	if ((*readfds & sys_validfds) != *readfds) {
	    /*
	     * Set the exceptfds mask to indicate which fd was bad.
	     */
	    *readfds ^= sys_validfds;
	    errno = EBADF;
	    return(-1);
	}
	all_fds |= *readfds;
    }

    /*
     * if this is a timed event then setup the timer.
     */
    if (timeout == NULL)
	block++;
    else
	if (timeout->tv_sec != 0) {
	    block++;
	    /*
	     * setup a timer AST to check later
	     */
	    if (timeout->tv_sec > 86399) { /* not grater than 24 hours */
		errno = EINVAL;
		readyfds = -1;
		goto EXIT;
	    }
	    lt = localtime(&timeout->tv_sec);
	    sprintf(at,"0 %02.2d:%02.2d:%02.2d.%02.2d",
		lt->tm_hour, lt->tm_min, lt->tm_sec, timeout->tv_usec);
	    ascii_time.dsc$w_length = strlen(at);
	    sys$bintim(&ascii_time,&t);
	    lib$get_ef(&timer_ef);
	    sys$setimr(timer_ef,&t,0,timer_ef,0);
	    FD_SET((timer_ef - EF_BASE),&ef_mask);
	}

CHECK_DESCRIPTORS:
    /*
     * exception file descriptors
     */
    if (exceptfds != NULL)
	if((ready_fds = sys_exceptfds & *exceptfds) != 0)
	    for (i=0; i < maxfds; i++)
		if (FD_ISSET(i,&ready_fds))
		    readyfds++;

    /*
     * write file descriptors
     */
    if (writefds != NULL)
	if((ready_fds = sys_writefds & *writefds) != 0)
	    for (i=0; i < maxfds; i++)
		if (FD_ISSET(i,&ready_fds))
		    readyfds++;

    /*
     * read file descriptors
     */
    if (readfds != NULL)
	if((ready_fds = sys_readfds & *readfds) != 0)
	    for (i=0; i < maxfds; i++)
		if (FD_ISSET(i,&ready_fds))
		    readyfds++;

    /*
     * See if we'er ready to exit
     */
    if (readyfds == 0)
	if (block) {
	    if (timer_ef != 0)
		if (sys$clref(timer_ef) == SS$_WASSET)
		    goto EXIT;
	    for (i=0; i < maxfds; i++)
		if (FD_ISSET(i,&all_fds))
		    FD_SET((sd[i]->ef - EF_BASE),&ef_mask);
	    sys$wflor(1,ef_mask);
	    goto CHECK_DESCRIPTORS;
	}
	
EXIT:
    if (timer_ef != 0) {
	sys$cantim(timer_ef,0);
	lib$free_ef(&timer_ef);
    }
    if (exceptfds != NULL)
	*exceptfds = (sys_exceptfds & maxfds_mask);
    if (writefds != NULL)
	*writefds = (sys_writefds & maxfds_mask);
    if (readfds != NULL)
	*readfds = (sys_readfds & maxfds_mask);
    return(readyfds);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int getsockname(int s, struct sockaddr *name, int *namelen)
 *
 * Description:
 *	Returns the name associated with a socket
 *
 * s	   - a valid socket descriptor
 * name	   - sockaddr address structure where data is to be written
 * namelen - number of bytes written to name
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int getsockname(s,name,namelen)
int	s;
struct	sockaddr *name;
int	*namelen;
{
int	size;

    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * return values held in data structure
     */
    size = (sd[s]->mylen < *namelen) ? sd[s]->mylen : *namelen;
    memcpy(name,&sd[s]->my,size);
    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int getpeername(int s, struct sockaddr *name, int *namelen)
 *
 * Description:
 *	Returns the name associated with a socket
 *
 * s	   - a valid socket descriptor
 * name	   - sockaddr address structure where data is to be written
 * namelen - number of bytes written to name
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int getpeername(s,name,namelen)
int	s;
struct	sockaddr *name;
int	*namelen;
{
int	conInfoSize;
struct sockaddr_in *from;
NetIO_Status_Block iosb;
Connection_Info_Return_Block conInfo;

    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * Must be connected
     */
    if ((sd[s]->flags & SD_CONNECTED) == 0) {
	errno = ENOTCONN;
	return(-1);
    }

    /*
     * Get connection from information
     */
    conInfoSize = sizeof(Connection_Info_Return_Block);
    vaxc$errno = sys$qiow( 0, sd[s]->chan, TCP$INFO, &iosb, 0, 0,
		&conInfo,		/* buffer		*/
		conInfoSize,		/* Size of said buffer  */
		0,0,0,0);

    /*
     * check the qio status
     */
    if (vaxc$errno != SS$_NORMAL) {
	errno = cmu_get_errno(&iosb);
	return (-1);
    }

    /*
     * record the from port info in our priviate data structure
     */
    from		= &sd[s]->from;
    from->sin_family	= AF_INET;
    from->sin_port	= htons(conInfo.CI$Foreign_Port);
    from->sin_addr.s_addr = conInfo.CI$remote_internet_adrs;

    /*
     * determine amount of data to return to caller
     */
    conInfoSize = (sizeof(struct sockaddr) < *namelen)
				? sizeof(struct sockaddr) : *namelen;
    /*
     * make the copy
     */
    memcpy(name, &sd[s]->from, conInfoSize);
    *namelen = conInfoSize;

    return(0);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int getsockopt(int s, int level, int optname,
 *			char *optval, int *optlen)
 *
 * Description:
 *	Returns the options set on a socket.
 *
 * Note:
 *	This routine does nothing.  Always returns -1 with errno set to
 * ENOPROTOOPT.
 *
 * Returns: (if it did do something)
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int getsockopt(s,level,optname,optval,optlen)
int	s, level, optname;
char	*optval;
int	*optlen;
{
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * there are no socket level options at this time.
     */
    errno = ENOPROTOOPT;
    return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int setsockopt(int s, int level, int optname,
 *			char *optval, int *optlen)
 *
 * Description: (if it did do something)
 *	Set the options on a socket.
 *
 * Note:
 *	This routine does nothing.  Always returns -1 with errno set to
 * ENOPROTOOPT.
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int setsockopt(s,level,optname,optval,optlen)
int	s, level, optname;
char	*optval;
int	optlen;
{
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * there are no socket level options at this time.
     */
    errno = ENOPROTOOPT;
    return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int gethostname(char *name, int namelen)
 *
 * Description:
 *	Returns the name currently associated to the host.
 *
 * name	   - address of buffer to write name of host
 * namelen - length of name buffer
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int gethostname(name, namelen)
char	*name;
int	namelen;
{
    return(cmu_trnlnm("LNM$SYSTEM","INTERNET_HOST_NAME","Super",name,namelen));
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * struct servent *getservbyname(char *name, char *proto);
 *
 * Description:
 *	returns a servent structure filled in with information about the
 * requested service.
 *
 * name  - pointer to character string of service to search for
 * proto - pointer to character string of protocol type desired (tcp, udp)
 *
 * Notes:
 *	The usual services text file is not supported.  Instead, services are
 * are logical names defined in the system logical name table.  Services
 * logical names are defined as follows:
 *
 *	$ DEFINE /SYSTEM INET$SERVICE_service_name_protocol value
 *
 * Example:
 *   To define the service telnet, protocol tcp, port 23 use the following
 * statement:
 *	$ DEFINE/SYSTEM INET$SERVICE_TELNET_TCP 23
 *
 * Returns:
 *	Returns the address of a servent structure on success, the NULL pointer
 * on error (see cmu_trnlnm).
 */

static struct servent serv;

struct servent *getservbyname(name, proto)
char	*name;
char	*proto;
{
char	logical[256];
char	port[16];


    sprintf(logical,"INET$SERVICE_%s_%s",name,proto);

    vaxc$errno = cmu_trnlnm( "LNM$SYSTEM", &logical, "Super", &port, 16);
    if (vaxc$errno == 0) {
	serv.s_name = name;
	serv.s_port = htons(atoi(&port));
	serv.s_proto = proto;
	return (&serv);
    }
    else
	return(NULL);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * struct servent *getservbyport(int port, char *proto);
 *
 * Description:
 *	returns a servent structure filled in with information about the
 * requested port.
 *
 * port  - int value of port number to search for
 * proto - pointer to character string of protocol type desired (tcp, udp)
 *
 * Notes:
 *	The usual services text file is not supported.  Instead, services are
 * are logical names defined in the system logical name table.  Services
 * logical names are defined as follows:
 *
 *	$ DEFINE /SYSTEM INET$SERVICE_port_number_protocol service
 *
 * Example:
 *   To define the service telnet, protocol tcp, port 23 use the following
 * statement:
 *	$ DEFINE/SYSTEM INET$SERVICE_23_TCP TELNET
 *
 * Returns:
 *	Returns the address of a servent structure on success, the NULL pointer
 * on error (see cmu_trnlnm).
 */
struct servent *getservbyport(port, proto)
int	port;
char	*proto;
{
char	logical[256];
char	name[32];

    sprintf(logical,"INET$SERVICE_%d_%s",port,proto);

    vaxc$errno = cmu_trnlnm( "LNM$SYSTEM", &logical, "Super", &name, 32);
    if (vaxc$errno == 0) {
	serv.s_name = &name;
	serv.s_port = htons(port);
	serv.s_proto = proto;
	return (&serv);
    }
    else
	return(NULL);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * struct hostent *gethostbyname(char *name)
 *
 * Description:
 *	returns the host address associated with the given name.
 *
 * name - pointer to a character string that contains the host/domain name to
 *        search for.
 *
 * Return:
 *	Returns the address of a hostent structure on success, the NULL pointer
 * on error.  Additional error information is returned in the global variable
 * errno.
 */
static	struct hostent inet_host;
static	GTHST_NMLOOK_BLOCK inet_list;
static	int	*addrlist = inet_list.GHA$NL_ADRLST;

struct hostent *gethostbyname(name)
char	*name;
{
int	chan;
NetIO_Status_Block iosb;

    /*
     * If the first character of the hostname is numeric the quit (must be dot
     * notation address).
     */

    if (isdigit(name[0]))
	return(NULL);

    sys$assign(&inet_device,&chan,0,0);

    inet_list.GHA$NL_ADRCNT = 0;
    vaxc$errno = sys$qiow( 0, chan, TCP$GTHST, &iosb, 0, 0,
		&inet_list,
		sizeof(GTHST_NMLOOK_BLOCK),
		GTH_NAMADR,
		name,
		0, 0);

    sys$dassgn(chan);

    if ((iosb.NSB$STATUS != SS$_NORMAL) || (inet_list.GHA$NL_ADRCNT < 1)) {
	errno = cmu_get_errno(&iosb);
	return(NULL);
    }

    inet_list.GHA$NL_NAMSTR[inet_list.GHA$NL_NAMLEN] = 0;
    inet_host.h_name     = inet_list.GHA$NL_NAMSTR;
    inet_host.h_aliases  = NULL;
    inet_host.h_addrtype = AF_INET;
    inet_host.h_length   = inet_list.GHA$NL_ADRCNT * 4;
    inet_host.h_addr_list= &addrlist;

    return(&inet_host);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * struct hostent *gethostbyaddr(char *addr, int len, int type)
 *
 * Description:
 *	Return official name of host given the host address.
 *
 * addr - a pointer to a series of bytes in network order specifying the
 *        address of the host to search for.
 * len  - number of bytes in the addr
 * type - address format. Only AF_INET is supported.
 *
 * Return:
 *	Returns the address of a hostent structure on success, the NULL pointer
 * on error.  Additional error information is returned in the global variable
 * errno.
 */
static GTHST_ADLOOK_BLOCK inet_name;

struct hostent *gethostbyaddr(addr,len,type)
char	*addr;
int	len, type;
{
int	chan;
int	laddr;
NetIO_Status_Block iosb;

    if (type != AF_INET) {
	errno = EAFNOSUPPORT;
	return(NULL);
    }

    sys$assign(&inet_device,&chan,0,0);

    memcpy(&laddr, addr, 4);

    vaxc$errno = sys$qiow( 0, chan, TCP$GTHST, &iosb, 0, 0,
		&inet_name,
		sizeof(inet_name),
		GTH_ADRNAM,
		laddr,
		0,0);

    sys$dassgn(chan);

    if (iosb.NSB$STATUS != SS$_NORMAL) {
	errno = cmu_get_errno(&iosb);
	return(NULL);
    }

    inet_list.GHA$NL_ADRCNT = 1;
    inet_list.GHA$NL_ADRLST[0] = laddr;

    inet_name.GHN$NAMSTR[inet_name.GHN$NAMLEN] = 0;
    inet_host.h_name     = inet_name.GHN$NAMSTR;
    inet_host.h_aliases  = NULL;
    inet_host.h_addrtype = AF_INET;
    inet_host.h_length   = inet_list.GHA$NL_ADRCNT * 4;
    inet_host.h_addr_list= &addrlist;

    return(&inet_host);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <ioctl.h>
 *
 * int ioctl(int s, int request, char *argp)
 *
 * Description:
 *	control device.
 *
 * s       - a valid socket descriptor
 * request - one of:
 *		FIOCLEX		- set exclusive use on socket
 *		FIONCLEX	- remove exclusive use
 *		FIONBIO		- set/clear non-blocking i/o
 *		FIONREAD	- get # bytes ready to be read
 *		SIOCGIFADDR	- get ifnet addres
 *		SIOCGIFFLAGS	- get ifnet flags
 *		SIOCGIFCONF	- get ifnet list
 * argp    - address of buffer for return information		
 *		
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int ioctl(s,request,argp)
int	s;
int	request;
char	*argp;
{
int	i;
#define BUFR_SIZE 512
char bufr[BUFR_SIZE];

unsigned long *n;

struct DU$IF_List {
    int	cnt;		/* number of if returned  */
    int	index[10];	/* limit of 10 interfaces */
} du_iflist;

struct ifconf *ifc;
struct ifreq  *ifr;
struct D$Dev_Dump_Return_Blk *dd;
NetIO_Status_Block iosb;
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    errno = 0;

    switch ((request & 0x0000ff00) >> 8) {
	case 't' :
	case 's' :
	case 'r' :
	    errno = EIO;
	    break;
	case 'f' :
	    switch (request & 0x000000ff) {
		case 1 :	/* FIOCLEX - set exclusive use on socket */
		case 2 :	/* FIONCLEX - remove exclusive use	 */
		    sd[s]->ioctl_opts ^= FEXLOCK;
		    break;
		case 126 :	/* FIONBIO - set/clear non-blocking i/o  */
		    sd[s]->ioctl_opts ^= O_NDELAY;
		    break;
		case 127:	/* FIONREAD - get # bytes ready to be read */
		    n  = argp;
		    /*
		     * if the socket read blocked return an error
		     */
		    if ((sd[s]->ioctl_opts & FREAD) != 0)
			errno = EPIPE;
		    if (FD_ISSET(s,&sys_readfds))
			*n = sd[s]->read_iosb.NSB$Byte_Count;
		    else
			*n = 0;
		    break;
		default:
		    errno = EIO;
		    break;
	    }
	    break;
	case 'i' :
	    switch (request & 0x000000ff) {
		case 13 : /* SIOCGIFADDR   - get ifnet address */
		    ifr = argp;

		    vaxc$errno = sys$qiow( 0, sd[s]->chan, TCP$DUMP, &iosb,
				0, 0,
				&du_iflist, sizeof(struct DU$IF_List),
				DU$Device_List, 0, 0, 0);
	
		    if (vaxc$errno != SS$_NORMAL) {
			errno = cmu_get_errno(&iosb);
			break;
		    }

		    for (i=0; i < du_iflist.cnt; i++) {
			vaxc$errno = sys$qiow(0,sd[s]->chan,TCP$DUMP, &iosb,
				0, 0,
				bufr,BUFR_SIZE,DU$Device_Stat,
				du_iflist.index[i],0,0);
			if ( iosb.NSB$STATUS != SS$_NORMAL ) {
			    errno = cmu_get_errno(&iosb);
			    break;
			}
			dd = bufr;
			if (strcmp(ifr->ifr_name, dd->DU$DevNam_Str) == 0) {
			    memcpy(&ifr->ifr_addr.sa_data[2],
				&dd->DU$Dev_Address, 4);
			    ifr->ifr_data  = 0;
			    ifr->ifr_flags = AF_INET;
			}
		    }
		    break;
		case 17 : /* SIOCGIFFLAGS  - get ifnet flags */
		    /*
		     * were going to fake it here and just say the interface
		     * is IFF_UP, RUNNING and BROADCAST enabled.  We should
		     * look at the interface and determine if it's a P-to-P
		     * connection.
		     */
		    ifr = argp;

		    ifr->ifr_flags = (IFF_UP | IFF_RUNNING | IFF_BROADCAST);
		    break;

		case 20 : /* SIOCGIFCONF   - get ifnet list */
		    ifc = argp;
		    vaxc$errno = sys$qiow(0, sd[s]->chan, TCP$DUMP, &iosb,
				0, 0,
				&du_iflist, sizeof(struct DU$IF_List),
				DU$Device_List, 0, 0, 0);
	
		    if (vaxc$errno != SS$_NORMAL) {
			errno = cmu_get_errno(&iosb);
			break;
		    }

		    ifc->ifc_len = 0;
		    for (i=0; i < du_iflist.cnt; i++) {
			ifr = &ifc->ifc_req[i];
			vaxc$errno = sys$qiow(0,sd[s]->chan,TCP$DUMP, &iosb,
				0, 0,
				bufr, BUFR_SIZE, DU$Device_Stat,
				du_iflist.index[i],0,0);
			if ( vaxc$errno != SS$_NORMAL ) {
			    errno = cmu_get_errno(&iosb);
			    break;
			}
			dd = bufr;
			memcpy(ifr->ifr_name, 
				dd->DU$DevNam_Str, dd->DU$DevNam_Len);
			ifr->ifr_name[dd->DU$DevNam_Len] = 0;
			memcpy(&ifr->ifr_addr.sa_data[2],
				&dd->DU$Dev_Address, 4);
			ifr->ifr_data  = 0;
			ifr->ifr_flags = AF_INET;
			ifc->ifc_len += sizeof(struct ifreq);
		    }
		    break;

		default :
		    errno = EIO;
		    break;		
	    }
    }

    return ( errno == 0 ? 0 : -1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <file.h>
 *
 * int fcntl(int s, int request, int arg)
 *
 * Description:
 *	file control.
 *
 * s       - a valid socket descriptor
 * request - One of:
 *		F_GETFL - get file flags
 *		F_SETFL - set file flags
 * arg     - flags to set
 *
 * Returns:
 *	Value of flags or -1 on error.  Addition error information is
 * specified in the global variable errno.
 */
int fcntl(s,request,arg)
int s, request, arg;
{
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    switch (request) {
	case F_GETFL :
	    return(sd[s]->ioctl_opts);
	case F_SETFL :
	    sd[s]->ioctl_opts = arg;
	    return(0);
	default :
	    errno = EINVAL;
	    return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int cmu_read(int s, char *buf, int len)
 *
 * Description:
 *	Receive bytes from a connected socket.
 *
 * To avoid conflict and link warnings regarding the `read' routine
 * in the standard VAXCRTL this routine name is `cmu_read'.
 * A direct call to cmu_read(s,b,l) is required to read a socket.
 *
 * s   - a valid socket descriptor
 * buf - address of buffer to where input data is placed
 * len - max size of buf
 *
 * Returns:
 *	Number of bytes read from the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int cmu_read(s,buf,len)
int	s;
char	*buf;
int	len;
{
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * Must be connected
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0)
	return(recvfrom(s,buf,len,0,0,0));
    else {
	errno = EBADF;
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include <types.h>
 * #include <socket.h>
 *
 * int cmu_write(int s, char *msg, int len, int flags)
 *
 * Description:
 *	Send bytes through a socket to its connected peer.
 *
 * s   - a valid socket descriptor
 * buf - address of buffer of data to be sent
 * len - size of buf
 *
 * Returns:
 *	Number of bytes written to the socket, -1 on error.  Addition error
 * information is specified in the global variable errno.
 */
int cmu_write(s,msg,len)
int	s;
char	*msg;
int	len;
{
    /*
     * check for valid socket
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(-1);
    }

    /*
     * Must be connected
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0)
	return(sendto(s,msg,len,0,0,0));
    else {
	errno = EBADF;
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int cmu_close(int s)
 *
 * Description:
 *	Closes a connection and deletes a socket descriptor.
 *
 * To avoid confusion and link warnings regarding the `close' routine
 * for other file descriptors this routine name is `cmu_close'.
 * A "#define close(s) cmu_close(s)" or direct call to cmu_close(s) is needed in routines
 * close sockets managed by this library.   If this routine is called with a socket
 * descriptor that is not managed by this library it will pass that descriptor
 * to the system close routine.
 *
 * s - an open socket descriptor
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int cmu_close(s)
int	s;
{
struct	backlogEntry *entry;

    /*
     * if this is not one of our channels then pass it on to the systems
     * close routine.
     */
    if (sd[s] == NULL)
	return(close(s));

    /*
     * Verify that the channel is indeed open -- in some cases it may not be.
     */
    if ((sd[s]->flags & SD_CONNECTED) != 0) {
	/*
	 * close/deallocate the I/O channel
	 */
	sys$qiow(0, sd[s]->chan, TCP$CLOSE, &sd[s]->write_iosb, 0, 0, 0, 0, 0, 0, 0, 0);
	sys$dalloc(sd[s]->chan);
    }

    /*
     * Clear all the associated fd bits
     */
    FD_CLR(s,&sys_validfds);
    FD_CLR(s,&sys_readfds);
    FD_CLR(s,&sys_writefds);
    FD_CLR(s,&sys_exceptfds);

    /*
     * free the various resources that we have accumulated
     */
    if (sd[s]->ef != 0)
	lib$free_ef(&sd[s]->ef);

    if (sd[s]->rcvbuf != NULL)
	free(sd[s]->rcvbuf);

    /*
     * was this an `accept'ed socket?
     */
    if (sd[s]->listen_socket != 0) {
	sd[sd[s]->listen_socket]->backlogSize--;
	/*
	 * requeue the listen if it was shutdown
	 */
	if ((sd[sd[s]->listen_socket]->flags & SD_LISTENING) == 0)
	    cmu_queue_listen(sd[s]->listen_socket);
    }

    /*
     * purge the listen queue if this was a listen socket
     */
    if ((sd[s]->sock_opts & SO_ACCEPTCONN) != 0) {
	while (sd[s]->backlogQueue != NULL) {
	    entry = sd[s]->backlogQueue;
	    sd[s]->backlogQueue = entry->flink;
	    cfree(entry);
	}
    }

    cfree(sd[s]);
    sd[s] = 0;
    close(s);	/* this should free up the duped file descriptor */
    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * #include "libcmu.h"
 *
 * int *cmu_stdin_open(char *name)
 *
 * Description:
 *	To have stdin a device that can be selected the descriptor needs to be
 * allocated within the domain of these routines.  This routine allocates the
 * file descriptor and returns the address of the FD_ENTRY so the user can
 * control reads/writes and still use select to see if the descriptor is ready.
 * Optionally the user can use the following routine to queue reads on stdin.
 *
 * name - character string device name of the device to assign a channel to.
 *
 * Returns:
 *	Address of an FD_ENTRY structure on success, -1 on error.  Addition
 * error information is specified in the global variable errno.
 */
int *cmu_stdin_open(name)
char	*name;
{
struct	dsc$descriptor dev_name;

    if (sd[0] != NULL) {
	errno = EBADF;
	return(-1);
    }
    sd[0] = calloc( 1, sizeof(struct FD_ENTRY));
    FD_CLR(0,&sys_readfds);
    FD_SET(0,&sys_validfds);

    /*
     * copy the `name' to the `my' sockaddr area (seems like a logical place)
     */
    if (strlen(name) <= 14) {
	sd[0]->mylen = strlen(name);
	sd[0]->my.sa_family = 0;
	memcpy(&sd[0]->my.sa_data, name, strlen(name));
    }
    else {
	errno = ENAMETOOLONG;
	goto ERROR_RETURN;
    }

    /*
     * setup a discriptor for the device and assign a channel to it
     */
    dev_name.dsc$w_length  = sd[0]->mylen;
    dev_name.dsc$b_dtype   = DSC$K_DTYPE_T;
    dev_name.dsc$b_class   = DSC$K_CLASS_S;
    dev_name.dsc$a_pointer = &sd[0]->my.sa_data;

    vaxc$errno = sys$assign(&dev_name, &sd[0]->chan, 0, 0);

    if (vaxc$errno != SS$_NORMAL) {
	errno = EVMSERR;
	goto ERROR_RETURN;
    }

    lib$get_ef(&sd[0]->ef);
    sys$clref(sd[0]->ef);
    /*
     * return the address of the socket descriptor so the user can play with
     * it.
     */
    return(sd[0]);

ERROR_RETURN:
    if (sd[0] != NULL)
	cfree(sd[0]);
    sd[0] = NULL;
    return(-1);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int cmu_stdin_read(int flags, char *buf, int len, char *prompt, int mask)
 *
 * Description:
 *	queue an i/o to the descriptor described in sd[0].  Since this read is
 * ment to be used with the select call a non-blocking qio is done.  The
 * completion routine will set the file descriptor and event flag on read
 * completion.
 *
 * flags  - qio read modifiers
 * buf    - to receive characters read
 * len    - max characters to read
 * prompt - prompt string for read
 * mask   - read terminator mask
 *
 * Returns:
 *	Status from the SYS$QIO system service call.
 */
int cmu_stdin_read(flags,buf,len,prompt,mask)
int	flags;
char	*buf;
int	len;
char	*prompt;
int	mask;
{

    /*
     * Clear these system things.
     */
    sys$clref(sd[0]->ef);
    FD_CLR(0, &sys_readfds);

    if (prompt != NULL)
	vaxc$errno = sys$qio( 0, sd[0]->chan, flags, &sd[0]->read_iosb,
			cmu_read_ast,0,
			buf, len,
			0, mask, prompt, strlen(prompt));
    else
	vaxc$errno = sys$qio( 0, sd[0]->chan, flags, &sd[0]->read_iosb,
			cmu_read_ast,0,
			buf, len,
			0, 0, 0, 0);
    return(vaxc$errno);
}				

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int cmu_get_sdc(int s)
 *
 * Description:
 *	Returns the assigned VMS channel for a particular file descriptor
 *
 * s - a valid socket descriptor
 *
 * Returns:
 *  If successful a VMS i/o channel number is returned otherwise a 0 is returned and
 *  errno is set.
 */
int cmu_get_sdc(s)
int	s;
{
    /*
     * check for valid socket.
     */
    if (sd[s] == NULL) {
	errno = EBADF;
	return(0);
    }
    else
	return(sd[s]->chan);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int cmu_trnlnm(char *table, char *name, char *mode, char *buff, int len)
 *
 * Description:
 *	Translate a logical name.  Terminate the output buffer with a null
 *	character.
 *
 * table - pointer to string that describes the logical name table to search
 * name  - pointer to character string of logical name to search for
 * mode  - mimimum access mode of logical name (kernel, exec, super, user)
 * buff  - pointer to buffer that is to receive the translation
 * len   - size of buff
 *
 * Returns:
 *	0 on success.  -1 on error.  vaxc$errno is set to status of
 * sys$trnlnm service call and errno is set to EVMSERR.
 */

int cmu_trnlnm( table, name, mode, buff, len )
char	*table;	/* table name */
char	*name;	/* logical name */
char	*mode;	/* access mode */
char	*buff;	/* return buffer */
int	len;	/* size of return buffer */
{
int	attr;
unsigned char *acmode;
struct	dsc$descriptor tabnam, lognam, eqvstr;
struct	ITEM_LIST itmlst[2];

/* definitions for different access modes
 * See STARLET.REQ $PSLDEF
 */
char	PSL$C_KERNEL = 0;
char	PSL$C_EXEC   = 1;
char	PSL$C_SUPER  = 2;
char	PSL$C_USER   = 3;

    tabnam.dsc$b_dtype	= DSC$K_DTYPE_T;
    tabnam.dsc$b_class	= DSC$K_CLASS_S;
    tabnam.dsc$a_pointer= table;
    tabnam.dsc$w_length	= strlen(table);

    lognam.dsc$b_dtype	= DSC$K_DTYPE_T;
    lognam.dsc$b_class	= DSC$K_CLASS_S;
    lognam.dsc$a_pointer= name;
    lognam.dsc$w_length	= strlen(name);

    eqvstr.dsc$b_dtype	= DSC$K_DTYPE_T;
    eqvstr.dsc$b_class	= DSC$K_CLASS_S;
    eqvstr.dsc$a_pointer= buff;
    eqvstr.dsc$w_length	= len;

    itmlst[0].itm$w_length = len;
    itmlst[0].itm$w_itmcode= LNM$_STRING;
    itmlst[0].itm$a_bufaddr= eqvstr.dsc$a_pointer;
    itmlst[0].itm$a_retlen = &eqvstr.dsc$w_length;

    itmlst[1].itm$w_length = 0;
    itmlst[1].itm$w_itmcode= 0;
    itmlst[1].itm$a_bufaddr= 0;
    itmlst[1].itm$a_retlen = 0;

    attr = LNM$M_CASE_BLIND;
  

    switch (toupper(mode[0])) {
	case 'K':
	    acmode = &PSL$C_KERNEL;
	    break;
	case 'E':
	    acmode = &PSL$C_EXEC;
	    break;
	case 'S':
	    acmode = &PSL$C_SUPER;
	    break;
	case 'U':
	    acmode = &PSL$C_USER;
	    break;
	default :
	    acmode = NULL;
    }
	
    vaxc$errno = sys$trnlnm( &attr, &tabnam, &lognam, acmode, itmlst );

    if(vaxc$errno == SS$_NORMAL) {
	buff[eqvstr.dsc$w_length] = 0;
	return(0);
    }
    else {
	errno = EVMSERR;
	return(-1);
    }
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int cmu_get_errno(NetIO_Status_Block *iosb)
 *
 * Description:
 *	returns best guess value for errno be examining the i/o status block.
 *
 * iosb - address of an i/o status block.
 *
 * Returns:
 *	Returns an errno value.
 */
int cmu_get_errno(iosb)
NetIO_Status_Block *iosb;
{
int qio_errno = 0;
int cmu_errno = 0;

    /*
     * Choose errno based on qio status return.
     */
    switch (iosb->NSB$STATUS) {
	case 0:
		qio_errno = EVMSERR;	/* use vaxc$errno */
		break;
	case SS$_MEDOFL:		/* INETACP has died */
		qio_errno = ENETDOWN;
		break;
	case SS$_BADPARAM:		/* parameter not valid for call */
		qio_errno = EINVAL;
		break;
	case SS$_EXQUOTA:		/* buffer allocation problems */
	case SS$_INSFMEM:
		qio_errno = EMSGSIZE;
		break;
	case SS$_ACCVIO:		/* buffer access problems */
		qio_errno = EFAULT;
		break;
	case SS$_ILLCNTRFUNC:		/* internal ACP error */
		qio_errno = EIO;
	default:
		qio_errno = 0;
	}

    /*
     * Choose errno based on extended error code (this is gross!)
     */
	/* no error */
    if      (iosb->NSB$XERROR == 0)
	    cmu_errno = EVMSERR;
	/* <Insufficient system resources> */
    else if (iosb->NSB$XERROR == NET$_IR)
	    cmu_errno = ENOMEM;
	/* <Invalid network function code> */
    else if (iosb->NSB$XERROR == NET$_IFC)
	    cmu_errno = EOPNOTSUPP;
	/* <Invalid network protocol code> */
    else if (iosb->NSB$XERROR == NET$_IPC)
	    cmu_errno = EPROTONOSUPPORT;
	/* <Non-unique connection specified> */
    else if (iosb->NSB$XERROR == NET$_NUC)
	    cmu_errno = EISCONN;
	/* <Connection RESET by remote host> */
    else if (iosb->NSB$XERROR == NET$_CR)
	    cmu_errno = ECONNRESET;
	/* <Foreign host unspecified> */
    else if (iosb->NSB$XERROR == NET$_FSU)
	    cmu_errno = EDESTADDRREQ;
	/* <Connection cancelled by process abort> */
    else if (iosb->NSB$XERROR == NET$_CCAN)
	    cmu_errno = ECONNABORTED;
	/* <IO Function in Progress> */
    else if (iosb->NSB$XERROR == NET$_FIP)
	    cmu_errno = EALREADY;
	/* <User specified buffer is Too small> */
    else if (iosb->NSB$XERROR == NET$_BTS)
	    cmu_errno = ERANGE;
	/* <Bad device index> */
    else if (iosb->NSB$XERROR == NET$_BDI)
	    cmu_errno = ENODEV;
	/* <Connection is closing - operation invalid> */
    else if (iosb->NSB$XERROR == NET$_CC)
	    cmu_errno = ESHUTDOWN;
	/* <Net ACP is Exiting> */
    else if (iosb->NSB$XERROR == NET$_TE)
	    cmu_errno = ENETDOWN;
	/* <Name lookup not yet complete - no host address> */
    else if (iosb->NSB$XERROR == NET$_NOADR)
	    cmu_errno = EINPROGRESS;
	/* <Connection killed> */
    else if (iosb->NSB$XERROR == NET$_KILL)
	    cmu_errno = EPIPE;
	/* <Connection refused by remote host> */
    else if (iosb->NSB$XERROR == NET$_CREF)
	    cmu_errno = ECONNREFUSED;
	/* <Name lookup failure: name resolver queue is full> */
    else if (iosb->NSB$XERROR == NET$_NSQFULL)
	    cmu_errno = ECONNREFUSED;
	/* <Connection does not exist> */
    else if (iosb->NSB$XERROR == NET$_CDE)
	    cmu_errno = ENOTCONN;
	/* <UDP wildcard connection not yet open> */
    else if (iosb->NSB$XERROR == NET$_NOPN)
	    cmu_errno = ENOTCONN;
	/* <Invalid GTHST function> */
    else if (iosb->NSB$XERROR == NET$_IGF)
	    cmu_errno = EINVAL;
	/* <NET$DUMP argument error> */
    else if (iosb->NSB$XERROR == NET$_DAE)
	    cmu_errno = EINVAL;
	/* <Invalid foreign port> */
    else if (iosb->NSB$XERROR == NET$_IFS)
	    cmu_errno = EADDRNOTAVAIL;
	/* <Invalid local port> */
    else if (iosb->NSB$XERROR == NET$_ILP)
	    cmu_errno = EADDRNOTAVAIL;
	/* <Connection table space exhausted> */
    else if (iosb->NSB$XERROR == NET$_CSE)
	    cmu_errno = EADDRNOTAVAIL;
	/* <Connection TimeOut> */
    else if (iosb->NSB$XERROR == NET$_CTO)
	    cmu_errno = ETIMEDOUT;
	/* <Time-Wait TimeOut> */
    else if (iosb->NSB$XERROR == NET$_TWT)
	    cmu_errno = ETIMEDOUT;
	/* <User function timeout. Network event didn't happen> */
    else if (iosb->NSB$XERROR == NET$_FTO)
	    cmu_errno = ETIMEDOUT;
	/* <Unable to create TCB> */
    else if (iosb->NSB$XERROR == NET$_UCT)
	    cmu_errno = ENOBUFS;
	/* <Valid-TCB table full (all connections in use)> */
    else if (iosb->NSB$XERROR == NET$_VTF)
	    cmu_errno = ENOBUFS;
	/* <GTHST queue full - can't queue name/address lookup> */
    else if (iosb->NSB$XERROR == NET$_GTHFUL)
	    cmu_errno = ENOBUFS;
	/* <No privilege for access to well-known port> */
    else if (iosb->NSB$XERROR == NET$_NOPRV)
	    cmu_errno = EACCES;
	/* <Connection illegal for this process> */
    else if (iosb->NSB$XERROR == NET$_CIP)
	    cmu_errno = EACCES;
	/* <Internet access not allowed> */
    else if (iosb->NSB$XERROR == NET$_NOINA)
	    cmu_errno = EACCES;
	/* <ARPANET access not allowed> */
    else if (iosb->NSB$XERROR == NET$_NOANA)
	    cmu_errno = EACCES;
	/* <Name lookup failure: name resolver is not running> */
    else if (iosb->NSB$XERROR == NET$_NONS)
	    cmu_errno = EHOSTDOWN;
	/* <Name not found in host table (domain service unavailable)> */
    else if (iosb->NSB$XERROR == NET$_DSDOWN)
	    cmu_errno = EHOSTDOWN;
	/* <Domain service: no domain servers could be contacted> */
    else if (iosb->NSB$XERROR == NET$_DSNODS)
	    cmu_errno = EHOSTDOWN;
	/* <Domain service: no usable servers returned by referral> */
    else if (iosb->NSB$XERROR == NET$_DSNONSRV)
	    cmu_errno = EHOSTDOWN;
	/* <Domain service: maximum referral limit exceeded> */
    else if (iosb->NSB$XERROR == NET$_DSREFEXC)
	    cmu_errno = EHOSTDOWN;
	/* <Unknown host number> */
    else if (iosb->NSB$XERROR == NET$_UNU)
	    cmu_errno = EHOSTUNREACH;
	/* <Unknown host name> */
    else if (iosb->NSB$XERROR == NET$_UNA)
	    cmu_errno = EHOSTUNREACH;
	/* <Destination Unreachable> */
    else if (iosb->NSB$XERROR == NET$_URC)
	    cmu_errno = EHOSTUNREACH;
	/* <Invalid known host index> */
    else if (iosb->NSB$XERROR == NET$_IHI)
	    cmu_errno = EHOSTUNREACH;
	/* <Unknown network node> */
    else if (iosb->NSB$XERROR == NET$_UNN)
	    cmu_errno = EHOSTUNREACH;
	/* <Domain service: name error (no such name)> */
    else if (iosb->NSB$XERROR == NET$_DSNAMERR)
	    cmu_errno = EHOSTUNREACH;
	/* <Domain service: no addresses found for host name> */
    else if (iosb->NSB$XERROR == NET$_DSNOADDR)
	    cmu_errno = EHOSTUNREACH;
	/* <Domain service: no name found for IP address> */
    else if (iosb->NSB$XERROR == NET$_DSNONAME)
	    cmu_errno = EHOSTUNREACH;
	/* <Name lookup: no name server could be found> */
    else if (iosb->NSB$XERROR == NET$_GP_NONMSR)
	    cmu_errno = EHOSTUNREACH;
	/* <Name lookup: request host info item does not exist> */
    else if (iosb->NSB$XERROR == NET$_GP_NOHINF)
	    cmu_errno = EHOSTUNREACH;
	/* <Name lookup: host name not found> */
    else if (iosb->NSB$XERROR == NET$_GP_NOTFND)
	    cmu_errno = EHOSTUNREACH;
	/* <Name lookup: all name servers declared down> */
    else if (iosb->NSB$XERROR == NET$_GP_NSDOWN)
	    cmu_errno = EHOSTUNREACH;
	/* <No route found for destination> */
    else if (iosb->NSB$XERROR == NET$_NRT)
	    cmu_errno = EHOSTUNREACH;
	/* <Error processing device dump> */
    else if (iosb->NSB$XERROR == NET$_EPD)
	    cmu_errno = EIO;
	/* <User function not yet implemented> */
    else if (iosb->NSB$XERROR == NET$_NYI)
	    cmu_errno = EIO;
	/* <Host name lookup time-out> */
    else if (iosb->NSB$XERROR == NET$_NMLTO)
	    cmu_errno = EIO;
	/* <Name lookup failure: name resolver is exiting> */
    else if (iosb->NSB$XERROR == NET$_NSEXIT)
	    cmu_errno = EIO;
	/* <Domain service: domain server error> */
    else if (iosb->NSB$XERROR == NET$_DSSRVERR)
	    cmu_errno = EIO;
	/* <Domain service: domain server returned format error> */
    else if (iosb->NSB$XERROR == NET$_DSFMTERR)
	    cmu_errno = EIO;
	/* <Domain service: received incomplete domain server reply> */
    else if (iosb->NSB$XERROR == NET$_DSINCOMP)
	    cmu_errno = EIO;
	/* <Domain service: domain server returned not implemented> */
    else if (iosb->NSB$XERROR == NET$_DSNOTIMP)
	    cmu_errno = EIO;
	/* <Domain service: domain server refused to resolve name> */
    else if (iosb->NSB$XERROR == NET$_DSREFUSD)
	    cmu_errno = EIO;
	/* <Domain service: unknown server error> */
    else if (iosb->NSB$XERROR == NET$_DSUNKERR)
	    cmu_errno = EIO;
	/* <Name lookup failure, unknown error code> */
    else if (iosb->NSB$XERROR == NET$_GREENERR)
	    cmu_errno = EIO;
	/* <Name lookup: invalid request (internal error)> */
    else if (iosb->NSB$XERROR == NET$_GP_INVREQ)
	    cmu_errno = EIO;
	/* <Name lookup: invalid host info item (internal error)> */
    else if (iosb->NSB$XERROR == NET$_GP_INVINF)
	    cmu_errno = EIO;
	/* <Name lookup: invalid domain name syntax (internal error)> */
    else if (iosb->NSB$XERROR == NET$_GP_INVNAM)
	    cmu_errno = EIO;
	/* <Name lookup: invalid address syntax (internal error)> */
    else if (iosb->NSB$XERROR == NET$_GP_INVADR)
	    cmu_errno = EIO;
	/* <Name lookup: invalid mailbox syntax (internal error)> */
    else if (iosb->NSB$XERROR == NET$_GP_INVMBX)
	    cmu_errno = EIO;
	/* <Name lookup: invalid domain class (internal error)> */
    else if (iosb->NSB$XERROR == NET$_GP_INVCLS)
	    cmu_errno = EIO;
	/* <Name lookup: resolver to busy to handle query> */
    else if (iosb->NSB$XERROR == NET$_GP_RSBUSY)
	    cmu_errno = EIO;
	/* <Name lookup: mailbox not found> */
    else if (iosb->NSB$XERROR == NET$_GP_UNKMBX)
	    cmu_errno = EIO;
	/* <Name lookup: request data too big for UDP> */
    else if (iosb->NSB$XERROR == NET$_GP_TOOBIG)
	    cmu_errno = EIO;
	/* <Name lookup: unimplemented request (internal error)> */
    else if (iosb->NSB$XERROR == NET$_GP_NOTIMP)
	    cmu_errno = EIO;

    if ((qio_errno != 0) || (cmu_errno != 0))
	return(cmu_errno != 0 ? cmu_errno : qio_errno);
    else
	return(EIO); /* punt! */
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int cmu_listen_accept(int s)
 *
 * Description:
 *	AST completion routine called when a connection completes on an open
 *	listening socket.
 *
 * s - valid socket descriptor
 *
 * Returns:
 *	None.
 */
int cmu_listen_accept(s)
int	s;	/* socket descriptor */
{
int	ns, tmp;
struct	backlogEntry *backlogQueue;
struct	sockaddr_in *to;
int	conInfoSize;
Connection_Info_Return_Block conInfo;

    /*
     * check the stats of the completed open
     */
    if (sd[s]->read_iosb.NSB$STATUS != SS$_NORMAL) {
	/* 
	 * failed; set exceptfds and event
	 */
	FD_SET(s,&sys_exceptfds);
	sys$setef(sd[s]->ef);
    }

    /*
     * get a new clean socket like the one we have now
     */
    ns = socket(sd[s]->domain, sd[s]->type, sd[s]->protocol);

    /*
     * Get connection from information
     */
    conInfoSize = sizeof(Connection_Info_Return_Block);
    vaxc$errno = sys$qiow( 0, sd[s]->chan, TCP$INFO, &sd[s]->read_iosb, 0, 0,
		&conInfo,		/* buffer		*/
		conInfoSize,		/* Size of said buffer  */
		0,0,0,0);
	
    /*
     * record from port info in new socket to port info
     */
    to			= &sd[ns]->to;
    to->sin_family	= AF_INET;
    to->sin_port	= htons(conInfo.CI$Foreign_Port);
    to->sin_addr.s_addr = conInfo.CI$remote_internet_adrs;

    /*
     * copy the local port information
     */
    memcpy(&sd[ns]->my, &sd[s]->my,   sizeof(struct sockaddr));

    /*
     * Set the bound and connected flags.
     */
    sd[ns]->flags = (SD_BIND | SD_CONNECTED);

    /*
     * indicate which socket this accept came from.
     */
    sd[ns]->listen_socket = s;

    /*
     * switch the assigned I/O channels
     */
    tmp		 = sd[s]->chan;
    sd[s]->chan  = sd[ns]->chan;
    sd[ns]->chan = tmp;    

    /*
     * insert this new socket into the backlog queue
     */
    if (sd[s]->backlogQueue == NULL) {
	sd[s]->backlogQueue = calloc(1,sizeof(struct backlogEntry));
	backlogQueue = sd[s]->backlogQueue;
    }
    else {
	backlogQueue = sd[s]->backlogQueue;
	while (backlogQueue->flink != NULL)
	    backlogQueue = backlogQueue->flink;
	backlogQueue->flink = calloc(1,sizeof(struct backlogEntry));
	backlogQueue = backlogQueue->flink;
    }
    backlogQueue->sock = ns;

    /*
     * Increment the count of accepted requests.
     */
    sd[s]->backlogSize++;

    /*
     * tell someone how cares that something came in...
     */
    FD_SET(s,&sys_readfds);
    sys$setef(accept_net_event);

    /*
     * queue another listen
     */
    cmu_queue_listen(s);

    return(SS$_NORMAL);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int cmu_queue_listen(int s)
 *
 * Description:
 *	Queue a read that will accept a TCP connection
 *
 * s - a valid socket descriptor
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int cmu_queue_listen(s)
int	s;	/* socket descriptor to listen on */
{
struct	sockaddr_in *my;


    /*
     * if reads are shutdown on this socket then don't queue the listen
     */
    if ((sd[s]->ioctl_opts & FREAD) != 0) {
	errno = ESHUTDOWN;
	return(-1);
    }

    /*
     * if there are already backlog number of connections then exit without
     * posting the listen.  The close routine will requeue the listen.
     */
    if (sd[s]->backlogSize < sd[s]->backlog) {
	sd[s]->flags |= SD_LISTENING;
	my = &sd[s]->my;
	vaxc$errno = sys$qio( sd[s]->ef, sd[s]->chan, TCP$OPEN, &sd[s]->read_iosb,
		cmu_listen_accept, s,	/* ast completion routine and param */
		0,			/* whomever			    */
		0,			/* remote port			    */
		ntohs(my->sin_port),	/* protocol filter		    */
		(MODE_TCP_WaitSYN | MODE_OpenNoWait),
		U$TCP_Protocol,		/* Protocol code		    */
		0);			/* timeout			    */

	if (vaxc$errno != SS$_NORMAL) {
	    errno = cmu_get_errno(&sd[s]->read_iosb);
	    return(-1);
	}
	sd[s]->flags |= SD_CONNECTED;
    }
    else {
	sd[s]->flags ^= SD_LISTENING;
    }

    return(0);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int cmu_read_ast(int s)
 *
 * Description:
 *	AST read completion routine.  Sets the global sys_readfds to
 * indicate the socket is ready to read or sys_exceptfds if there was an
 * error.
 *
 * s - valid socket descriptor
 *
 * Returns:
 *	Nothing.
 */
int cmu_read_ast(s)
int	s;	/* file descriptor that is ready. */
{

    switch (sd[s]->read_iosb.NSB$STATUS) {
	case SS$_NORMAL:
	    FD_SET(s,&sys_readfds);
	case SS$_ABORT:
	case SS$_CANCEL:
	    /*
	     * if the status code is SS$_CANCEL or SS$_ABORT then I/O on the
	     * channel was cancled.  Don't set the exception flag in this case.
	     */
	    break;
	default:
	    /*
	     * all other status returns indicate an error
	     */
	    FD_SET(s,&sys_exceptfds);
    }


    /*
     * check for socket closed by remote. If it was then make if no-read,
     * no-write, and invalid.
     */
    if ((sd[s]->read_iosb.X.STATUS & 0x0000ff00) == 0x0600) {
	sd[s]->ioctl_opts |= (FREAD & FWRITE);
	FD_CLR(s,&sys_validfds);
    }

    /*
     * now set the event flag to signal the read has completed.
     */
    sys$setef(sd[s]->ef);
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * int cmu_write_ast(int s)
 *
 * Description:
 *	AST write completion routine.  Sets the global sys_writefds to
 * indicate the socket is ready to be written or sys_exceptfds if there was an
 * error.
 *
 * s - valid socket descriptor
 *
 * Returns:
 *	Nothing.
 */
int cmu_write_ast(s)
int	s;	/* file descriptor that is ready. */
{
    if (sd[s]->write_iosb.NSB$STATUS == SS$_NORMAL)
	FD_SET(s,&sys_writefds);
    else
	FD_SET(s,&sys_exceptfds);

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *
 * int cmu_queue_net_read(int s)
 *
 * Description:
 *	Queue a read on socket `s' to be completed by net_read_ast.  This
 * routine will also allocate an event flag and receive buffer if necessary.
 *
 * s - valid socket descriptor
 *
 * Returns:
 *	0 on success, -1 on error.  Addition error information is specified in
 * the global variable errno.
 */
int cmu_queue_net_read(s)
int	s;
{
    /*
     * get and clear an event flag and file descriptor for this read request.
     */
    if (sd[s]->ef == 0)
	lib$get_ef(&sd[s]->ef);
    sys$clref(sd[s]->ef);
    FD_CLR(s, &sys_readfds);

    /*
     * Allocate a receive buffer for reads
     */
    if (sd[s]->rcvbuf == NULL)
	sd[s]->rcvbuf = malloc(sd[s]->rcvbufsize);
    if (sd[s]->rcvbuf == NULL) {
	sys$qio(0,sd[s]->chan,TCP$ABORT,0,0,0,0,0,0,0,0,0);
	errno = ENOBUFS;
	return(-1);
    }

    sys$clref(sd[s]->ef);

    if (sd[s]->protocol == IPPROTO_UDP)
	vaxc$errno = sys$qio( 0, sd[s]->chan, TCP$RECEIVE, &sd[s]->read_iosb,
		cmu_read_ast, s,	/* AST completion routine and arg */
		sd[s]->rcvbuf,		/* receive buffer*/
		sd[s]->rcvbufsize,	/* receive buffer size*/
		&sd[s]->rcvfrom,	/* receive from address buffer*/
		0,0,0);
    else
	vaxc$errno = sys$qio( 0, sd[s]->chan, TCP$RECEIVE, &sd[s]->read_iosb,
		cmu_read_ast, s,	/* AST completion routine and arg */
		sd[s]->rcvbuf,		/* receive buffer*/
		sd[s]->rcvbufsize,	/* receive buffer size*/
		0,0,0,0);

    if (vaxc$errno == SS$_NORMAL)
	return(0);
    else {
	errno = cmu_get_errno(&sd[s]->read_iosb);
	FD_SET(s,&sys_exceptfds);
	sys$setef(sd[s]->ef);
	sys$qio(0,sd[s]->chan,TCP$ABORT,0,0,0,0,0,0,0,0,0);
	return(-1);
    }
}
