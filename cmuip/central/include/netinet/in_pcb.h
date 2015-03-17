/* static	char	*sccsid = "@(#)in_pcb.h	4.1	(ULTRIX)	11/23/87"; */

/************************************************************************
 *									*
 *			Copyright (c) 1985 by				*
 *		Digital Equipment Corporation, Maynard, MA		*
 *			All rights reserved.				*
 *									*
 *   This software is furnished under a license and may be used and	*
 *   copied  only  in accordance with the terms of such license and	*
 *   with the  inclusion  of  the  above  copyright  notice.   This	*
 *   software  or  any  other copies thereof may not be provided or	*
 *   otherwise made available to any other person.  No title to and	*
 *   ownership of the software is hereby transferred.			*
 *									*
 *   This software is  derived  from  software  received  from  the	*
 *   University    of   California,   Berkeley,   and   from   Bell	*
 *   Laboratories.  Use, duplication, or disclosure is  subject  to	*
 *   restrictions  under  license  agreements  with  University  of	*
 *   California and with AT&T.						*
 *									*
 *   The information in this software is subject to change  without	*
 *   notice  and should not be construed as a commitment by Digital	*
 *   Equipment Corporation.						*
 *									*
 *   Digital assumes no responsibility for the use  or  reliability	*
 *   of its software on equipment which is not supplied by Digital.	*
 *									*
 ************************************************************************/

/************************************************************************
 *			Modification History				*
 *									*
 *	Larry Cohen  -	09/16/85					*
 * 		Add 43bsd alpha tape changes for subnet routing		*
 *									*
 *	Larry Cohen  -  01/28/87
 *		Add options field to inpcb
 ************************************************************************/

/*
 * Copyright (c) 1982 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)in_pcb.h	6.2 (Berkeley) 6/8/85
 */

/*
 * Common structure pcb for internet protocol implementation.
 * Here are stored pointers to local and foreign host table
 * entries, local and foreign socket numbers, and pointers
 * up (to a socket structure) and down (to a protocol-specific)
 * control block.
 */
struct inpcb
{
    struct	inpcb *inp_next,*inp_prev;
    /* pointers to other pcb's */
    struct	inpcb *inp_head;	/* pointer back to chain of inpcb's
					   for this protocol */
    struct	in_addr inp_faddr;	/* foreign host table entry */
    u_short	inp_fport;		/* foreign port */
    struct	in_addr inp_laddr;	/* local host table entry */
    u_short	inp_lport;		/* local port */
    struct	socket *inp_socket;	/* back pointer to socket */
    caddr_t	inp_ppcb;		/* pointer to per-protocol pcb */
    struct	route inp_route;	/* placeholder for routing entry */
    struct  mbuf *inp_options;      /* IP options */
};

#define	INPLOOKUP_WILDCARD	1
#define	INPLOOKUP_SETLOCAL	2

#define	sotoinpcb(so)	((struct inpcb *)(so)->so_pcb)

#ifdef KERNEL
struct	inpcb *in_pcblookup();
#endif
