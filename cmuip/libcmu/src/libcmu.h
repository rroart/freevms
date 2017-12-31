/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * LIBCMU.h, Copyright (C) 1993, Mike O'Malley, Digital Equipment Corporation
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
 * Facility:	LIBCMU
 *
 * Abstract:	Socket interface routines for CMU-OpenVMS/IP v6.6
 *
 * Description:
 *		Definitions and structures for LIBCMU.C
 * Author:
 *	Mike O'Malley
 *
 * Modifications:
 *	 7-OCT-1993 mlo
 *		Renamed iosb to read_iosb in FD_ENTRY structure.
 *		Added write_iosb to FD_ENTRY structure.
 */
#ifndef __LIBCMU__
#define __LIBCMU__
#define CMUTCP


/*
 * CMU-OpenVMS/IP QIO Function Codes
 */
#define   TCP$SEND        (IO$_WRITEVBLK)
#define   TCP$RECEIVE     (IO$_READVBLK)
#define   TCP$OPEN        (IO$_CREATE)
#define   TCP$CLOSE       (IO$_DELETE)
#define   TCP$ABORT       (IO$_DEACCESS)
#define   TCP$STATUS      (IO$_ACPCONTROL)
#define   TCP$INFO        (IO$_MODIFY)
#define   TCP$DUMP        (IO$_UNLOAD)
#define   TCP$GTHST       (IO$_SKIPFILE)

/*
 * CMU-OpenVMS/IP TCP$OPEN mode flags
 */
/* Bit 1 TCP */
#define MODE_TCP_WaitSYN	0x00	/* Passive connection */
#define MODE_TCP_SendSYN	0x01	/* Active connection  */
/* Bit 1 UDP */
#define MODE_UDP_A_data		0x00	/* pass address in data buffer*/
#define MODE_UDP_A_struct	0x01	/* or in an address structure*/
/* Bit 2 */
#define MODE_OpenNoWait		0x00	/* Don't wait for connection */
#define MODE_OpenWait		0x02	/* Wait for connection       */
/* Bit 3 */
#define MODE_AddrDesc		0x00
#define MODE_Addr32bit		0x04


/*
 * min and max buffer sizes for send and receive.  Note that the min buffer
 * size will be set at run time to smaller of `DEF' below or the SYSGEN
 * parameter MAXBUF.
 *
#define TCP_SO_RCVBUF_DEF 8192
#define UDP_SO_RCVBUF_DEF 9216
 *
 * actually, look like the maximum buffer is hardcoded somewhere down in the
 * INETACP.  Through trial and error these are the maximum defaults allowed
 * on my system.
 */
#define TCP_SO_RCVBUF_DEF 3072
#define UDP_SO_RCVBUF_DEF 3072
#define TCP_SO_RCVBUF_MAX 58254
#define UDP_SO_RCVBUF_MAX 58254

/*
 * Structures
 */

/*
 * Attached to a socket that is listening for connections.
 */
struct backlogEntry
{
    struct	backlogEntry *flink;
    int	sock;
};

/*
 * Internal structure definition of individual socket file descriptors.
 */
struct FD_ENTRY
{
    int			domain;		/* domain of socket AF_INET	      */
    int			type;		/* type of socket stream or datagram  */
    int			protocol;	/* protocol of socket		      */
    int			sock_opts;	/* socket options		      */
    int			ioctl_opts;	/* ioctl/fcntl/file options	      */
    int			flags;		/* socket state flags		      */
#define SD_BIND		1		/* is bound			      */
#define SD_CONNECTED	2		/* is connected			      */
#define SD_LISTENING	4		/* active listen		      */

    int			mylen;		/* local socket address name	      */
    struct sockaddr	my;
    int	 		fromlen;	/* from socket address name	      */
    struct sockaddr	from;
    int			tolen;		/* to socket address name	      */
    struct sockaddr	to;

    unsigned long		ef;		/* socket priviate event flag	      */
    unsigned short int	chan;		/* channel assigned to this socket    */
    NetIO_Status_Block	read_iosb;	/* qio completion status block	      */
    NetIO_Status_Block	write_iosb;	/* qio completion status block	      */

    int			listen_socket;	/* socket that listen was from	      */
    int			backlog;	/* max number of connection to accept */
    int			backlogSize;	/* number of connections active	      */
    struct backlogEntry	*backlogQueue;	/* the backlog queue		      */

    int			rcvbufsize;	/* receive buffer size		      */
    int			rcvbufoffset;	/* offset for partial receive	      */
    char			*rcvbuf;	/* receive buffer		      */
    IPADR$ADDRESS_BLOCK	rcvfrom;	/* address structure		      */
};

struct ITEM_LIST
{
    unsigned short  itm$w_length;
    unsigned short  itm$w_itmcode;
    long   itm$a_bufaddr;
    long   itm$a_retlen;
};

#endif /* LIBCMU */
