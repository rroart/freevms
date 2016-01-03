#ifndef __LINUX_PKT_SCHED_H
#define __LINUX_PKT_SCHED_H

/* Logical priority bands not depending on specific packet scheduler.
   Every scheduler will map them to real traffic classes, if it has
   no more precise mechanism to classify packets.

   These numbers have no special meaning, though their coincidence
   with obsolete IPv6 values is not occasional :-). New IPv6 drafts
   preferred full anarchy inspired by diffserv group.

   Note: TC_PRIO_BESTEFFORT does not mean that it is the most unhappy
   class, actually, as rule it will be handled with more care than
   filler or even bulk.
 */

#define TC_PRIO_BESTEFFORT      0
#define TC_PRIO_FILLER          1
#define TC_PRIO_BULK            2
#define TC_PRIO_INTERACTIVE_BULK    4
#define TC_PRIO_INTERACTIVE     6
#define TC_PRIO_CONTROL         7

#define TC_PRIO_MAX         15

/* Generic queue statistics, available for all the elements.
   Particular schedulers may have also their private records.
 */

struct tc_stats
{
    __u64   bytes;          /* NUmber of enqueues bytes */
    __u32   packets;        /* Number of enqueued packets   */
    __u32   drops;          /* Packets dropped because of lack of resources */
    __u32   overlimits;     /* Number of throttle events when this
                     * flow goes out of allocated bandwidth */
    __u32   bps;            /* Current flow byte rate */
    __u32   pps;            /* Current flow packet rate */
    __u32   qlen;
    __u32   backlog;
#ifdef __KERNEL__
    spinlock_t *lock;
#endif
};

#endif
