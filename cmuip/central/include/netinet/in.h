/* static	char	*sccsid = "@(#)in.h	4.1.1.1	(ULTRIX)	1/20/88"; */

/************************************************************************
 *									*
 *			Copyright (c) 1985, 1986  by			*
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
 *	
 *	15-Jan-88	lp
 *		Merge of final 43BSD changes.
 *
 *	Larry Cohen  -	09/16/85					*
 * 		Add 43bsd alpha tape changes for subnet routing		*
 *									*
 *	Larry Cohen  -	01/28/87
 *		Add IP options flag for control routines. 		
 *									*
 ************************************************************************/

/*
 * Copyright (c) 1982 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	in.h	6.5 (Berkeley) 6/8/85
 */

/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981.
 */

/*
 * Protocols
 */
#define	IPPROTO_IP		0		/* dummy for IP */
#define	IPPROTO_ICMP		1		/* control message protocol */
#define	IPPROTO_GGP		2		/* gateway^2 (deprecated) */
#define	IPPROTO_TCP		6		/* tcp */
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#define	IPPROTO_PUP		12		/* pup */
#define	IPPROTO_UDP		17		/* user datagram protocol */

#define	IPPROTO_RAW		255		/* raw IP packet */
#define	IPPROTO_MAX		256


/*
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).
 */
#define	IPPORT_RESERVED		1024
#define IPPORT_USERRESERVED	5000

/*
 * Link numbers
 */
#define	IMPLINK_IP		155
#define	IMPLINK_LOWEXPER	156
#define	IMPLINK_HIGHEXPER	158

/*
 * Internet address (a structure for historical reasons)
 */
struct in_addr {
	union {
		struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
		struct { u_short s_w1,s_w2; } S_un_w;
		u_long S_addr;
	} S_un;
#define	s_addr	S_un.S_addr	/* can be used for most tcp & ip code */
#define	s_host	S_un.S_un_b.s_b2	/* host on imp */
#define	s_net	S_un.S_un_b.s_b1	/* network */
#define	s_imp	S_un.S_un_w.s_w2	/* imp */
#define	s_impno	S_un.S_un_b.s_b4	/* imp # */
#define	s_lh	S_un.S_un_b.s_b3	/* logical host */
};

/*
 * Definitions of bits in internet address integers.
 */
#define	IN_CLASSA(i)		((((long)(i))&0x80000000)==0)
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSA_NSHIFT	24
#define	IN_CLASSA_HOST		0x00ffffff
#define	IN_CLASSA_MAX		128

#define	IN_CLASSB(i)		((((long)(i))&0xc0000000)==0x80000000)
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSB_NSHIFT	16
#define	IN_CLASSB_HOST		0x0000ffff
#define	IN_CLASSB_MAX		65536

#define	IN_CLASSC(i)		((((long)(i))&0xc0000000)==0xc0000000)
#define	IN_CLASSC_NET		0xffffff00
#define	IN_CLASSC_NSHIFT	8
#define	IN_CLASSC_HOST		0x000000ff

#define IN_CLASSD(i)            (((long)(i) & 0xf0000000) == 0xe0000000)
#define IN_MULTICAST(i)         IN_CLASSD(i)

#define IN_EXPERIMENTAL(i)      (((long)(i) & 0xe0000000) == 0xe0000000)
#define IN_BADCLASS(i)          (((long)(i) & 0xf0000000) == 0xf0000000)

#define	INADDR_ANY		0x00000000
#define	INADDR_BROADCAST	0xffffffff
#ifndef KERNEL
#define INADDR_NONE             0xffffffff              /* -1 return */
#endif

#define IN_LOOPBACKNET          127                     /* official! */


/*
 * Macros for subnetworks.  A subnet is distinguished by
 * 	(1) the network number is a `local' network number, and
 *	(2) the most significant bit of the host part is set.
 * Such addresses include one additional byte in the network number,
 * and use one less byte in the host part (i.e., a subnet of a Class A
 * network uses the rules for Class B net/host number extraction,
 * a Class B subnet is dealt with as if it were a Class C net).
 * Subnets of Class C nets are not supported.
 */
#define	SUBNETSHIFT		8  /* used to get main net number from subnet */

#define	IN_SUBNETA(i)		((((long)(i))&0x80800000)==0x00800000)
#define	IN_CLASSA_SUBNET	0xffff0000
#define	IN_CLASSA_SUBNSHIFT	(IN_CLASSA_NSHIFT - SUBNETSHIFT)
#define	IN_CLASSA_SUBHOST	0x0000ffff

#define	IN_SUBNETB(i)		((((long)(i))&0xc0008000)==0x80008000)
#define	IN_CLASSB_SUBNET	0xffffff00
#define	IN_CLASSB_SUBNSHIFT	(IN_CLASSB_NSHIFT - SUBNETSHIFT)
#define	IN_CLASSA_SUBHOST	0x0000ffff
#define	IN_CLASSB_SUBHOST	0x000000ff

/*
 * Socket address, internet style.
 */
struct sockaddr_in {
	short	sin_family;
	u_short	sin_port;
	struct	in_addr sin_addr;
	char	sin_zero[8];
};

/*
 * Options for use with [gs]etsockopt at the IP level.
 */
#define IP_OPTIONS      1               /* set/get IP per-packet options */

#if !defined(vax)
/*
 * Macros for number representation conversion.
 */
#define	ntohl(x)	(x)
#define	ntohs(x)	(x)
#define	htonl(x)	(x)
#define	htons(x)	(x)
#endif

#ifdef KERNEL
extern	struct domain inetdomain;
extern	struct protosw inetsw[];
struct	in_addr in_makeaddr();
extern 	int nINET;
u_long        in_netof(), in_lnaof();
#endif
