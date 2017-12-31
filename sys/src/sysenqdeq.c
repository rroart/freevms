// $Id$
// $Locker$

// Author. Roar Thronæs.

// Some lines/parts may be taken from lustre and modified. (Required line)

#include<linux/linkage.h>
#include<asm/current.h>
#include<system_data_cells.h>
#include<ipl.h>
#include<lckdef.h>
#include<lckctxdef.h>
#include<lksbdef.h>
#include<misc.h>
#include<ssdef.h>
#include<starlet.h>
#include<acbdef.h>
#include<ipldef.h>
#include<rsbdef.h>
#include<lkbdef.h>
#include<pridef.h>
#include<descrip.h>
#include<cdrpdef.h>
#include <queue.h>
#include <exe_routines.h>
#include <misc_routines.h>
#include <scs_routines.h>
#include <sch_routines.h>
#include <linux/slab.h>
#include <internals.h>

// no vmslock etc here yet

unsigned char lck$ar_compat_tbl[6]= {0x3f,0x1f,0x7,0xb,3,1};
#if 0
unsigned char lck$ar_compat_tbl2[6,6]=
{
    {1,1,1,1,1,1},
    {0,1,1,1,1,1},
    {0,0,0,1,1,1},
    {0,0,1,0,1,1},
    {0,0,0,0,1,1},
    {0,0,0,0,0,1}
};
#endif

unsigned char lck$synccvt_tbl[6]= {1,3,7,0xb,0x1f,0x3f};

unsigned char lck$quecvt_tbl[6]= {0x3e,0x3c,0x38,0x34,0x20,0};

unsigned char lck$ggmax_tbl[6]= {0,0,8,4,0,0};

unsigned char lck$ggred_tbl[6]= {0,0,0,0,0xc,0};

unsigned char lck$valblk_tbl[6]= {0,0,0,0,0x1f,0x3f};

unsigned char lck$al_dwncvt_tbl[6]= {0,1,3,3,0xf,0x1f};

void dlmmyerr() {}

void dlmlisten() {}

int dlminit(void)
{
    char myname[]="dlm$dlm";
    char myinfo[]="dlm dlm";

    //  listen(msgbuf,err,cdt,pdt,cdt);
    scs_std$listen(dlmlisten,dlmmyerr,myname,myinfo,0);
}

extern struct _rsb * reshashtbl[];
extern unsigned long lockidtbl[];

static int dlmconnected=0;

dlm_msg() {}

dlm_dg() {}

dlm_err() {}

void lck$snd_granted(struct _lkb * lck)
{
    struct _cdrp * cdrp = kmalloc(sizeof(struct _cdrp), GFP_KERNEL);
    memset(cdrp,0,sizeof(struct _cdrp));
    cdrp->cdrp$l_val1=lck->lkb$l_remlkid;
    cdrp->cdrp$l_val2=lck-lockidtbl[0];
    if (!dlmconnected)
    {
        dlmconnected=1;
    }
    scs_std$senddg(0,500,cdrp);
}

// this was suddenly not needed, I think
static void * enq_find_oldest_parent(struct _rsb * res,struct _rsb * par)
{
    struct _rsb * rsb=res;
    while (res)
    {
        rsb->rsb$b_depth++;
        par=res->rsb$l_parent;
        if (par==0) return res;
        res=par;
    };
}

int reshash(struct dsc$descriptor * d)
{
    return d->dsc$w_length;
}

struct _rsb * find_reshashtbl(struct dsc$descriptor * d)
{
    struct _rsb * tmp, *head=&reshashtbl[reshash(d)*2];
    tmp=head->rsb$l_hshchn;
    while (tmp!=head && strncmp(tmp->rsb$t_resnam,d->dsc$a_pointer,d->dsc$w_length))
    {
        tmp=tmp->rsb$l_hshchn;
    }
    if (strncmp(tmp->rsb$t_resnam,d->dsc$a_pointer,d->dsc$w_length))
        return 0;
    else
        return tmp;
}

void insert_reshashtbl(struct _rsb *res)
{
    struct dsc$descriptor d;
    d.dsc$w_length=res->rsb$b_rsnlen;
    d.dsc$a_pointer=res->rsb$t_resnam;
    insque(res,&reshashtbl[2*reshash(&d)]);
}

int insert_lck(struct _lkb * lck)
{
    int i;
    for(i=1; i<LOCKIDTBL && (lockidtbl[i]&0xffff0000); i++) ;
    lockidtbl[i]=lck;
    /* index pointer not implemented yet */
    return i;
}

asmlinkage int exe$enq(unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam, unsigned int parid, void (*astadr)(), unsigned long astprm, void (*blkastadr)(), unsigned int acmode, unsigned int rsdm_id)
{
    int convert;
    int retval=SS$_NORMAL;
    int sts;

    // some tests. one only for now, should be more.
    if (lkmode>LCK$K_EXMODE) return SS$_BADPARAM;

    vmslock(&SPIN_SCS,IPL$_SCS); // check. probably too early
    convert=flags&LCK$M_CONVERT;
    if (!convert)
    {
        /* new lock */
        struct _rsb * res = 0;
        struct _rsb * old;
        struct _lkb * lck = 0, *par = 0;
        struct dsc$descriptor * resnamdsc;
        int sserror=0;

        resnamdsc=resnam;
        if (resnamdsc->dsc$w_length==0 || resnamdsc->dsc$w_length>RSB$K_MAXLEN)
        {
            sserror=SS$_IVBUFLEN;
            goto error;
        }
        if (flags&LCK$M_EXPEDITE)
            if (lkmode!=LCK$K_NLMODE)
            {
                sserror=SS$_UNSUPPORTED;
                goto error;
            }

        if (lkmode!=LCK$K_NLMODE)
        {
            sserror=SS$_UNSUPPORTED;
            goto error;
        }

        res=kmalloc(sizeof(struct _rsb),GFP_KERNEL);
        memset(res,0,sizeof(struct _rsb));
        lck=kmalloc(sizeof(struct _lkb),GFP_KERNEL);
        memset(lck,0,sizeof(struct _lkb));

        lck->lkb$b_efn=efn;
        lck->lkb$l_flags=flags;
        lck->lkb$b_rqmode=lkmode;
        lck->lkb$l_cplastadr=astadr;
        lck->lkb$l_blkastadr=blkastadr;
        lck->lkb$l_astprm=astprm;
        lck->lkb$l_pid=current->pcb$l_pid;
        lck->lkb$l_lksb=lksb;
        qhead_init(&lck->lkb$l_sqfl);
        qhead_init(&lck->lkb$l_ownqfl);

        strncpy(res->rsb$t_resnam,resnamdsc->dsc$a_pointer,resnamdsc->dsc$w_length);
        res->rsb$b_rsnlen=resnamdsc->dsc$w_length;

        setipl(IPL$_SCS);
        // do scs spinlock
        //setipl(IPL$_ASTDEL);

        if (flags&LCK$M_SYSTEM)
        {
            /* priv checks */
        }
        else
        {

        }

        if (parid==0)
        {
            //list_add(&res->lr_childof, &ns->ns_root_list);
            //this is added to lck$gl_rrsfl down below, I think
        }
        else
        {
            //check valid lock
            // check lock access mode
            par=lockidtbl[parid];
            if (current->pcb$l_pid != par->lkb$l_pid)
            {
                vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
                return SS$_IVLOCKID;
            }
            //check if parent granted, if not return SS$_PARNOTGRANT;
            if (par->lkb$b_state!=LKB$K_CONVERT  || par->lkb$b_state!=LKB$K_GRANTED)
                if ((par->lkb$l_flags & LCK$M_CONVERT) == 0)
                {
                    vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
                    return SS$_PARNOTGRANT;
                }
            par->lkb$w_refcnt++;
            res->rsb$l_parent = par->lkb$l_rsb; // should not be here?
            //check if uic-specific resource
            //check if system-wide
            //charge lock against quota
            //list_add(&res->lr_childof, &parent->lr_children);
            //res->rsb$l_rtrsb=enq_find_oldest_parent(r,p->lkb$l_rsb);
            lck->lkb$l_parent=par;
        }

        old=find_reshashtbl(resnamdsc);
        if (!old)
        {
            lck$gl_rsbcnt++;
            lck$gl_lckcnt++;
            if (flags & LCK$M_SYNCSTS) retval=SS$_SYNCH;
            qhead_init(&res->rsb$l_grqfl);
            qhead_init(&res->rsb$l_cvtqfl);
            qhead_init(&res->rsb$l_wtqfl);
            //insque(&lck->lkb$l_sqfl,res->rsb$l_grqfl);
            lck->lkb$l_rsb=res;
            insert_reshashtbl(res);
            if (parid==0)
            {
                insque(&res->rsb$l_rrsfl,lck$gl_rrsfl);
                qhead_init(&res->rsb$l_srsfl);
                res->rsb$b_depth=0;
                res->rsb$l_rtrsb=res;
                exe$clref(lck->lkb$b_efn);
                insque(&lck->lkb$l_ownqfl,&current->pcb$l_lockqfl);
                //?if (q->flags & LKB$M_DCPLAST)

                lksb->lksb$l_lkid=insert_lck(lck);
                lksb->lksb$w_status=SS$_NORMAL;

                sts = lck$grant_lock(lck ,res ,-1,lkmode,flags,efn,res->rsb$b_ggmode);

                goto end;
            }
            else
            {
                // it has a parid non-zero
                res->rsb$l_csid=par->lkb$l_rsb->rsb$l_csid;
                par->lkb$l_rsb->rsb$w_refcnt++;
                res->rsb$b_depth=par->lkb$l_rsb->rsb$b_depth+1;
                //check maxdepth
                if (res->rsb$b_depth>10)   // pick a number ?
                {
                    retval=SS$_EXDEPTH;
                    goto error;
                }
                res->rsb$l_rtrsb=par->lkb$l_rsb->rsb$l_rtrsb;
                insque(&res->rsb$l_srsfl,&par->lkb$l_rsb->rsb$l_srsfl);
                if (par->lkb$l_csid)   //remote
                {
                    lck$snd_granted(lck);
                }
                else
                {
                    sts = lck$grant_lock(lck,res,-1,lkmode,flags,efn,res->rsb$b_ggmode);
                }
            }
        }
        else
        {
            /* old, found in resource hash table */
            /* something else? */
            int granted = 0;
            if (flags & LCK$M_SYNCSTS) retval=SS$_SYNCH;
            kfree(res);
            res=old;
            lck->lkb$l_rsb=res;

            //after, also check whether something in cvtqfl or wtqfl -> insque wtqfl

            if (0!=test_bit(res->rsb$b_ggmode,&lck$ar_compat_tbl[lck->lkb$b_rqmode]))
            {
                if (aqempty(res->rsb$l_wtqfl))
                {
                    granted=1;
                    //sts = lck$grant_lock(lck ,res ,-1,lkmode,flags,efn);
                }
                else
                {
                    if (flags&LCK$M_NOQUEUE)
                    {
                        res->rsb$w_lckcnt--;
                        kfree(lck);
                        vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
                        return SS$_NOTQUEUED;
                    }
                    else
                    {
                        lck->lkb$b_state=LKB$K_WAITING;
                        insque(&lck->lkb$l_sqfl,res->rsb$l_wtqfl);
                        lksb->lksb$w_status=0;
                        lck->lkb$l_status|=LKB$M_ASYNC;
                        maybe_blkast(res,lck);
                    }
                }
            }
            else
            {
                // if not compatible
                if (flags&LCK$M_NOQUEUE)
                {
                    res->rsb$w_lckcnt--;
                    kfree(lck);
                    vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
                    return SS$_NOTQUEUED;
                }
                else
                {
                    lck->lkb$b_state=LKB$K_WAITING;
                    insque(&lck->lkb$l_sqfl,res->rsb$l_wtqfl);
                    lksb->lksb$w_status=0;
                    lck->lkb$l_status|=LKB$M_ASYNC;
                    maybe_blkast(res,lck);
                    // insque(&lck->lkb$l_ownqfl,&current->pcb$l_lockqfl);
                }
            }

            lksb->lksb$l_lkid=insert_lck(lck);
            lksb->lksb$w_status=SS$_NORMAL;

            if ((granted & 1)==1)
            {
                if (0/*par->lkb$l_csid*/)   //remote
                {
                    lck$snd_granted(lck);
                }
                else
                {
                    sts = lck$grant_lock(lck, res, -1,lkmode,flags,efn,res->rsb$b_ggmode);
                }
            }
        }
end:
        /* raise ipl */
        vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
        return retval;
error:
        /* ipl back */
        kfree(res);
        kfree(lck);
        vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
        return sserror;

    }
    else     // convert
    {
        /* convert */
        int granted = 0, newmodes = 0;
        struct _lkb * lck;
        struct _rsb * res;
        void * dummy;
        int newmode;
        lck=lockidtbl[lksb->lksb$l_lkid];
        res=lck->lkb$l_rsb;
        if (lck->lkb$b_state!=LKB$K_GRANTED)
        {
            vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
            return SS$_CVTUNGRANT;
        }
        lck->lkb$b_efn=efn;
        lck->lkb$l_flags=flags;
        lck->lkb$b_rqmode=lkmode;
        lck->lkb$l_cplastadr=astadr;
        lck->lkb$l_blkastadr=blkastadr;
        lck->lkb$l_astprm=astprm;
        lck->lkb$l_lksb=lksb;
        remque(&lck->lkb$l_sqfl,&lck->lkb$l_sqfl);// ?
        //remque(&res->rsb$l_grqfl,dummy); // superfluous
        if (aqempty(res->rsb$l_cvtqfl) && aqempty(res->rsb$l_grqfl))
        {
            sts = lck$grant_lock(lck ,res,lck->lkb$b_grmode,lkmode,flags,efn,-1);
            vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
            return SS$_NORMAL;
        }
        else     // convert, something in cvtqfl or grqfl
        {
            if (res->rsb$b_cgmode!=lck->lkb$b_grmode)
            {
                newmode=res->rsb$b_ggmode;
            }
            else
            {
                newmode=find_highest(lck,res);
                newmodes= 0;
            }
            if (test_bit(lkmode,&lck$ar_compat_tbl[newmode]))
            {
                //sts = lck$grant_lock(lck,res,lck->lkb$b_grmode,lkmode,flags,efn);
                granted = 1;
            }
        }

        if (granted)
        {
            if (newmodes)
            {
                res->rsb$b_fgmode=newmode;
                res->rsb$b_ggmode=newmode;
                res->rsb$b_cgmode=newmode;
            }
            sts = lck$grant_lock(lck,res,lck->lkb$b_grmode,lkmode /*newmode*/,flags,efn,res->rsb$b_ggmode);
            grant_queued(res,newmode,1,1);
        }
        else
        {
            int wasempty=aqempty(&res->rsb$l_cvtqfl);
            lck->lkb$b_rqmode=lkmode;
            insque(&lck->lkb$l_sqfl,res->rsb$l_cvtqfl);
            lck->lkb$b_state=LKB$K_CONVERT;
            lksb->lksb$w_status=0;
            lck->lkb$l_status|=LKB$M_ASYNC;
            maybe_blkast(res,lck);
            if (wasempty)
                res->rsb$b_cgmode=newmode;
            sts=SS$_NORMAL;
        }
        vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
        return sts;
    }
    vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
}

int lck$deqlock(struct _lkb *lck, int flags, unsigned int lkid)
{
    vmslock(&SPIN_SCS,IPL$_SCS);
    struct _rsb * res = lck->lkb$l_rsb;
    int newmode;

    remque(&lck->lkb$l_ownqfl,0);
    remque(&lck->lkb$l_sqfl,0);

    // check if no locks on resource, remove the resource then

    newmode=find_highest(lck,res);

    res->rsb$b_fgmode=newmode;
    res->rsb$b_ggmode=newmode;
    res->rsb$b_cgmode=newmode;

    grant_queued(res,0,1,1);

    if (lck->lkb$b_state)
    {
    }
    kfree(lck);
    lockidtbl[lkid] = lkid + 1;
    if (aqempty(&res->rsb$l_grqfl) && aqempty(&res->rsb$l_cvtqfl) && aqempty(&res->rsb$l_wtqfl) && aqempty(&res->rsb$l_rrsfl) && aqempty(&res->rsb$l_srsfl))
    {
        remque(res, 0);
        kfree(res);
    }
    vmsunlock(&SPIN_SCS,IPL$_ASTDEL);
}

asmlinkage int exe$deq(unsigned int lkid, void *valblk, unsigned int acmode, unsigned int flags)
{
    int sts = SS$_NORMAL;
    struct _lkb * lck;
    if (lkid)
    {
        lck = lockidtbl[lkid];
        sts = lck$deqlock(lck, flags, lkid);
        if (lck->lkb$l_parent!=lck && ((flags&LCK$M_DEQALL)==0))
        {
            // sts = SS$_SUBLOCKS;
            goto end;
        }
    }
    else
    {
        if (flags & LCK$M_DEQALL)
        {
            // no lockid, dequeue all
        }
        else
        {
            return SS$_SUBLOCKS;
        }
    }
end:
    return sts;
}

void lock_iosb_kast(long par)
{
    struct _lkb * lck = par;
    struct _lksb * lksb = lck->lkb$l_lksb;
    if (lck->lkb$l_status&LKB$M_ASYNC)
    {
#if 0
        if (lksb) // check. temp workaround
            lksb->lksb$w_status=SS$_NORMAL;
#endif
        lck->lkb$l_status&=~LKB$M_ASYNC;
    }
    if (lck->lkb$l_status&LKB$M_DCPLAST)
    {
        lck->lkb$b_rmod=LKB$M_NODELETE;
        lck->lkb$l_ast=lck->lkb$l_cplastadr;
        lck->lkb$l_astprm=lck->lkb$l_oldastprm;
        sch$qast(lck->lkb$l_pid,PRI$_RESAVL,lck);
    }
}

int lck$grant_lock(struct _lkb * lck,struct _rsb * res, signed int curmode, signed int nextmode, int flags, int efn, signed int ggmode)
{
    int retsts=SS$_NORMAL;
    int blocking = 0;

    if (ggmode==-1 || nextmode>curmode)
    {
        res->rsb$b_ggmode=nextmode;
        res->rsb$b_fgmode=nextmode;
        res->rsb$b_cgmode=nextmode;
    }

    lck->lkb$b_grmode=nextmode;
    lck->lkb$l_lkst1=SS$_NORMAL;

    // check timeout queue

    if (lck->lkb$l_blkastadr)
    {
        struct _lkb * head = &res->rsb$l_cvtqfl;
        int diff=((char*)&head->lkb$l_sqfl)-((char*)head);
        struct _lkb * next = res->rsb$l_cvtqfl - diff;
        head=((unsigned long)head)-diff;

        res->rsb$w_blkastcnt++;

        while (head!=next)
        {
            // two like whiles. remember to copy
            if (0==test_bit(nextmode,&lck$ar_compat_tbl[next->lkb$b_rqmode]))
            {
                blocking = 1;
                goto endblockchk;
            }
            next=next->lkb$l_sqfl-diff;
        }

        head = &res->rsb$l_wtqfl;
        next = res->rsb$l_wtqfl - diff;
        head=((unsigned long)head)-diff;

        while (head!=next)
        {
            // two like whiles. remember to copy
            if (0==test_bit(nextmode,&lck$ar_compat_tbl[next->lkb$b_rqmode]))
            {
                blocking = 1;
                goto endblockchk;
            }
            next=next->lkb$l_sqfl-diff;
        }
    }
endblockchk:

    insque(&lck->lkb$l_sqfl,res->rsb$l_grqfl);
    lck->lkb$b_state=LKB$K_GRANTED;

    retsts=SS$_SYNCH;

    lck->lkb$b_rmod|=ACB$M_NODELETE;
    lck->lkb$b_rmod&=~(LKB$M_KAST|LKB$M_PKAST);

    if (blocking || lck->lkb$l_cplastadr || (lck->lkb$l_status&LKB$M_ASYNC))
    {

        if (blocking && lck->lkb$l_cplastadr)
        {
            lck->lkb$l_status|=LKB$M_DCPLAST;
            lck->lkb$l_oldastprm=lck->lkb$l_astprm;
            lck->lkb$b_rmod|=LKB$M_PKAST;
        }

        if (blocking)
        {
            lck->lkb$l_ast=lck->lkb$l_blkastadr;
        }
        else
        {
            lck->lkb$l_ast=lck->lkb$l_cplastadr;
        }

        if (lck->lkb$l_status&LKB$M_ASYNC || (lck->lkb$b_rmod&LKB$M_PKAST))
        {
            if (0==(lck->lkb$b_rmod&LKB$M_PKAST))
            {
                lck->lkb$b_rmod|=LKB$M_KAST;
            }
            lck->lkb$l_kast=lock_iosb_kast;
            lck->lkb$l_astprm=lck;
        }

#if 0
        if (lck->lkb$l_cplastadr && (lck->lkb$l_flags&LCK$M_SYNCSTS)==0)
        {
            lck->lkb$l_ast=lck->lkb$l_cplastadr;
        }
#endif

        sch$qast(lck->lkb$l_pid,PRI$_RESAVL,lck);
    }

    //if (current->pcb$w_state!=SCH$C_CUR)

    sch$postef(lck->lkb$l_pid,PRI$_RESAVL,lck->lkb$b_efn);

    return retsts;
}

int maybe_blkast(struct _rsb * res, struct _lkb * lck)
{
    if (lck->lkb$l_blkastadr)
    {
        struct _lkb * head = &res->rsb$l_grqfl;
        int diff=((char*)&head->lkb$l_sqfl)-((char*)head);
        struct _lkb * next = res->rsb$l_grqfl - diff;
        head=((unsigned long)head)-diff;

        while (head!=next)
        {
            // three like whiles. remember to copy
            if (0==test_bit(next->lkb$b_grmode,&lck$ar_compat_tbl[lck->lkb$b_rqmode]))
            {
                next->lkb$b_rmod|=ACB$M_NODELETE;
                next->lkb$b_rmod&=~(LKB$M_KAST|LKB$M_PKAST);
                next->lkb$l_ast=next->lkb$l_blkastadr;
                sch$qast(next->lkb$l_pid,PRI$_RESAVL,next);
            }
            next=next->lkb$l_sqfl-diff;
        }

    }
    return SS$_NORMAL;
}

int find_highest(struct _lkb * lck, struct _rsb * res)
{
    int high=0;
    struct _lkb * skipme=&lck->lkb$l_sqfl;

    struct _lkb * head=&res->rsb$l_grqfl;
    struct _lkb * tmp=res->rsb$l_grqfl;

    int diff=((char*)&tmp->lkb$l_sqfl)-((char*)tmp);

    while (tmp!=head)
    {
        if (tmp!=skipme)
        {
            struct _lkb * lock=((char*)tmp)-((char*)diff);
            if (lock->lkb$b_grmode>high)
                high=lock->lkb$b_grmode;
        }
        tmp=tmp->lkb$l_astqfl; //really lkb$l_sqfl;
    }

    while (tmp!=head)
    {
        if (tmp!=skipme)
        {
            struct _lkb * lock=((char*)tmp)-((char*)diff);
            if (lock->lkb$b_grmode>high)
                high=lock->lkb$b_grmode;
        }
        tmp=tmp->lkb$l_astqfl; //really lkb$l_sqfl;
    }

    head=&res->rsb$l_cvtqfl;
    tmp=res->rsb$l_cvtqfl;


    return high;
}

int grant_queued(struct _rsb * res, int ggmode_not, int docvt, int dowt)
{
    struct _lkb * head, * tmp;
    int diff;
    int newmode;
    if (docvt)
    {
        head=&res->rsb$l_cvtqfl;
        tmp=res->rsb$l_cvtqfl;
        diff=((char*)&tmp->lkb$l_sqfl)-((char*)tmp);
        while (head!=tmp)
        {
            tmp=((char *)tmp)-((char*)diff);
            newmode=res->rsb$b_ggmode;
            if (test_bit(tmp->lkb$b_rqmode,&lck$ar_compat_tbl[res->rsb$b_ggmode]))
            {
                struct _lkb * lck=tmp;
                struct _lkb * next=tmp->lkb$l_sqfl;
                remque(((long)tmp)+diff,0);
                lck$grant_lock(lck ,res,lck->lkb$b_grmode,lck->lkb$b_rqmode,0,0,res->rsb$b_ggmode);
                tmp=next;
                continue;
            }
            newmode=find_highest(tmp,res);
            if (res->rsb$b_ggmode==tmp->lkb$b_grmode)
            {
                if (test_bit(tmp->lkb$b_rqmode,&lck$ar_compat_tbl[newmode]))
                {
                    struct _lkb * lck=tmp;
                    struct _lkb * next=tmp->lkb$l_sqfl;
                    remque(((long)tmp)+diff,0);
                    lck$grant_lock(lck ,res,lck->lkb$b_grmode,lck->lkb$b_rqmode,0,0,res->rsb$b_ggmode);
                    tmp=next;
                    continue;
                }
                else
                {
                    res->rsb$b_cgmode=newmode;
                }
            }
            tmp=tmp->lkb$l_sqfl;
        }
    }
    if (dowt)
    {
    }

}

asmlinkage int exe$enqw(unsigned int efn, unsigned int lkmode, struct _lksb *lksb, unsigned int flags, void *resnam, unsigned int parid, void (*astadr)(), unsigned long astprm, void (*blkastadr)(), unsigned int acmode, unsigned int rsdm_id)
{
    int status=exe$enq(efn,lkmode,lksb,flags,resnam,parid,astadr,astprm,blkastadr,acmode,rsdm_id);
    printk("\n\nremember to do like qiow\n\n");
    if ((status&1)==0) return status;
    return exe$synch(efn,lksb);
}

asmlinkage int exe$enqw_wrap(struct struct_args * s)
{
    return exe$enqw(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7,s->s8,s->s9,s->s10,s->s11);
}


asmlinkage int exe$enq_wrap(struct struct_args * s)
{
    return exe$enq(s->s1,s->s2,s->s3,s->s4,s->s5,s->s6,s->s7,s->s8,s->s9,s->s10,s->s11);
}

