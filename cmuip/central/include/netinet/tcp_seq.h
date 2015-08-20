/*
#ifndef lint
static  char    *sccsid = "@(#)tcp_seq.h    4.1.1.1 (ULTRIX)    1/20/88";
#endif lint
*/
/************************************************************************
 *                                  *
 *          Copyright (c) 1984 by               *
 *      Digital Equipment Corporation, Maynard, MA      *
 *          All rights reserved.                *
 *                                  *
 *   This software is furnished under a license and may be used and *
 *   copied  only  in accordance with the terms of such license and *
 *   with the  inclusion  of  the  above  copyright  notice.   This *
 *   software  or  any  other copies thereof may not be provided or *
 *   otherwise made available to any other person.  No title to and *
 *   ownership of the software is hereby transferred.           *
 *                                  *
 *   This software is  derived  from  software  received  from  the *
 *   University    of   California,   Berkeley,   and   from   Bell *
 *   Laboratories.  Use, duplication, or disclosure is  subject  to *
 *   restrictions  under  license  agreements  with  University  of *
 *   California and with AT&T.                      *
 *                                  *
 *   The information in this software is subject to change  without *
 *   notice  and should not be construed as a commitment by Digital *
 *   Equipment Corporation.                     *
 *                                  *
 *   Digital assumes no responsibility for the use  or  reliability *
 *   of its software on equipment which is not supplied by Digital. *
 *                                  *
 ************************************************************************/
/*
 *
 *  15-Jan-88   lp
 *      Merge of final 43BSD changes.
 *
 */
/*  tcp_seq.h   6.1 83/07/29    */

#ifndef TCP_SEQ_H
#define TCP_SEQ_H

/*
 * TCP sequence numbers are 32 bit integers operated
 * on with modular arithmetic.  These macros can be
 * used to compare such integers.
 */
#define SEQ_LT(a,b) ((int)((a)-(b)) < 0)
#define SEQ_LEQ(a,b)    ((int)((a)-(b)) <= 0)
#define SEQ_GT(a,b) ((int)((a)-(b)) > 0)
#define SEQ_GEQ(a,b)    ((int)((a)-(b)) >= 0)

/*
 * Macros to initialize tcp sequence numbers for
 * send and receive from initial send and receive
 * sequence numbers.
 */
#define tcp_rcvseqinit(tp) \
    (tp)->rcv_adv = (tp)->rcv_nxt = (tp)->irs + 1

#define tcp_sendseqinit(tp) \
    (tp)->snd_una = (tp)->snd_nxt = (tp)->snd_max = (tp)->snd_up = \
        (tp)->iss

#define TCP_ISSINCR (125*1024)  /* increment for tcp_iss each second */

#ifdef KERNEL
tcp_seq tcp_iss;        /* tcp initial send seq # */
#endif

#endif /* TCP_SEQ_H */
