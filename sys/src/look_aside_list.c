// $Id$
// $Locker$

// Author. Roar Thron�s.

struct _myhead
{
    struct _myhead * hd$l_flink;
    long hd$l_seq;
};

#include <linux/kernel.h>
#include <spldef.h>
#include <internals.h>
#include <linux/cache.h>

static struct _spl SPIN_LAL __cacheline_aligned = { spl$l_own_cpu : 0, spl$l_own_cnt :
    -1 /*, spl$l_spinlock : 0*/
};

void *exe$lal_remove_first(void *listhead)
{
    int savipl = vmslock(&SPIN_LAL, 8);
    struct _myhead * h=listhead;
    struct _myhead * ret = h->hd$l_flink;
    h->hd$l_seq--;
    if (ret) h->hd$l_flink=ret->hd$l_flink;
    vmsunlock(&SPIN_LAL, savipl);
    return ret;
}

void exe$lal_insert_first(void *packet,void *listhead)
{
    int savipl = vmslock(&SPIN_LAL, 8);
    struct _myhead * h=listhead;
    struct _myhead * p=packet;
    h->hd$l_seq++;
    p->hd$l_flink=h->hd$l_flink;
    h->hd$l_flink=p;
    vmsunlock(&SPIN_LAL, savipl);
}

