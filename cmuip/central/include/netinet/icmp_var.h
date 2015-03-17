/* static	char	*sccsid = "@(#)icmp_var.h	4.1.1.1	(ULTRIX)	1/20/88"; */

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
 *
 *	15-Jan-88	lp
 *		Merge of final 43BSD changes.
 *
 *	Rebecca Harris - May 1986					*
 * 		Added sccs id and copyright statements.			*
 *									*
 ************************************************************************/

/*
 * Copyright (c) 1982 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	icmp_var.h	6.3 (Berkeley) 6/8/85
 */

/*
 * Variables related to this implementation
 * of the internet control message protocol.
 */
struct	icmpstat
{
    /* statistics related to icmp packets generated */
    int	icps_error;		/* # of calls to icmp_error */
    int	icps_oldshort;		/* no error 'cuz old ip too short */
    int	icps_oldicmp;		/* no error 'cuz old was icmp */
    int	icps_outhist[ICMP_MAXTYPE + 1];
    /* statistics related to input messages processed */
    int	icps_badcode;		/* icmp_code out of range */
    int	icps_tooshort;		/* packet < ICMP_MINLEN */
    int	icps_checksum;		/* bad checksum */
    int	icps_badlen;		/* calculated bound mismatch */
    int	icps_reflect;		/* number of responses */
    int	icps_inhist[ICMP_MAXTYPE + 1];
};

#ifdef KERNEL
struct	icmpstat icmpstat;
#endif
