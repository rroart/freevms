// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <linux/linkage.h>
#include <linux/vmalloc.h>
#include <asm/current.h>
#include <system_data_cells.h>
#include <lckdef.h>
#include <lckctxdef.h>
#include <misc.h>
#include <ssdef.h>
#include <starlet.h>
#include <ipldef.h>
#include <rsbdef.h>
#include <lkbdef.h>
#include <pridef.h>
#include <descrip.h>
#include <cdrpdef.h>
#include <exe_routines.h>
#include <lkidef.h>

extern unsigned long lockidtbl[];

unsigned long lkiscan_id = 0;

int exe_lkiscan_next_id(void ** p, int * id)
{
    int i;
    for(i = lkiscan_id; i < LOCKIDTBL && (lockidtbl[i] & 0xffff0000) == 0; i++) ;
    if (i == LOCKIDTBL)
    {
        lkiscan_id=0;
        return 0;
    }
    *p = lockidtbl[i];
    lkiscan_id++;
    return 1;
}

asmlinkage int exe$getlki(unsigned long efn, unsigned long *lkidadr,void *itmlst, struct _iosb *iosb, void (*astadr)(), long astprm,unsigned long reserved)
{
    struct item_list_3 * it=itmlst;
    struct _lkb * l;
    unsigned int id;
    int sts = exe_lkiscan_next_id ( &l, &id );
    if ( (sts & 1 ) == 0 )
        return sts;

    while (it->item_code)
    {
        switch (it->item_code)
        {
#if 0
        case LKI$_NAMSPACE: // check. borrowing this for address
            memcpy(it->bufaddr, &l, sizeof(long));
            break;
        case LKI$_RANGE: // check. borrowing this for address
            memcpy(it->bufaddr, &l->lkb$l_rsb, sizeof(long));
            break;
#endif
        case LKI$_RESNAM:
            memcpy(it->bufaddr, &l->lkb$l_rsb->rsb$t_resnam, 12);
            break;
        case LKI$_LKID:
            memcpy(it->bufaddr, &id, sizeof(int));
            break;
        case LKI$_PID:
            memcpy(it->bufaddr, &l->lkb$l_pid, 4);
            break;
        case LKI$_BLOCKING:
            memcpy(it->bufaddr, &l->lkb$l_blkastadr, sizeof(long));
            break;
        case LKI$_LOCKS:
            memcpy(it->bufaddr, &l->lkb$w_refcnt, 2);
            break;
#if 0
        case LKI$_BLOCKER_BR:
            memcpy(it->bufaddr, &l->lkb$b_rqmode, 1);
            break;
        case LKI$_LOCKS_BR:
            memcpy(it->bufaddr, &l->lkb$b_grmode, 1);
            break;
        case LKI$_MSTCSID:
            memcpy(it->bufaddr, &l->lkb$b_grmode, 1);
            break;
        case LKI$_SYSTEM:
            memcpy(it->bufaddr, &l->lkb$b_rmod, 1);
            break;
#endif
#if 0
        case LKI$_:
            memcpy(it->bufaddr, &l->lkb$l_, );
            break;
#endif
        default:
            printk("getlki item_code %x not implemented\n",it->item_code);
            break;
        }
        it++;
    }

    return sts;
}

asmlinkage int exe$getlkiw(unsigned long efn, unsigned long *lkidadr,void *itmlst, struct _iosb *iosb, void (*astadr)(), long astprm,unsigned long reserved)
{
    int status=exe$getlki(efn,lkidadr,itmlst,iosb,astadr,astprm,reserved);
    printk("\n\nremember to do like qiow\n\n");
    if ((status&1)==0) return status;
    return exe$synch(efn,iosb);
}

asmlinkage int exe$getlkiw_wrap(struct struct_args * s)
{
    return exe$getlkiw(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7);
}

asmlinkage int exe$getlki_wrap(struct struct_args * s)
{
    return exe$getlki(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7);
}

