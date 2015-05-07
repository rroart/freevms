// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <linux/mm.h>
#include <sysgen.h>
#include <lnmdef.h>
#include <lnmsub.h>
#include <system_data_cells.h>
#include <queue.h>
#include <lnmdef.h>
#include <lnmstrdef.h>
#include <ssdef.h>
#include <descrip.h>
#include <misc.h>
#include <exe_routines.h>
#include <sch_routines.h>
#include <misc_routines.h>
#include <linux/slab.h>
#include <internals.h>
#include <mtxdef.h>

/* Author: Roar Thron�s */

#define LNM_DEBUG
#undef LNM_DEBUG

#define lnmprintf printk

/* << does not rotate ... ? */

int lnm$hash(const int length, const unsigned char * log, const unsigned long mask, unsigned long * myhash)
{
    unsigned char *tmp = log;
    unsigned char count = length;
    unsigned long hash = count;
#ifdef LNM_DEBUG
    lnmprintf("count %x %x %s\n",count,tmp,tmp);
#endif
    while (count > 3)
    {
        //  lnmprintf("here %x %x\n",tmp[0],count);
        //tmp[0]=tmp[0]&223;
        //tmp[0]&=223;
        //  lnmprintf("here %x %x\n",tmp[0],count);
        //tmp[1]&=223;
        //  lnmprintf("here %x %x\n",tmp[0],count);
        //tmp[2]&=223;
        //  lnmprintf("here %x %x\n",tmp[0],count);
        //tmp[3]&=223;
        //  lnmprintf("here %x %x\n",tmp[0],count);
        hash = hash ^ (tmp[0] & 223) ^ (tmp[1] & 223) ^ (tmp[2] & 223) ^ (tmp[3] & 223);
        //  lnmprintf("here %x %x\n",tmp[0],count);
        count -= 4;
        tmp += 4;
        hash = hash << 9;
    };
    //lnmprintf("here4 %x\n",hash);
    while (count)
    {
        //*tmp&=223;
        hash = hash ^ ((*tmp) & 223);
        count--;
        tmp++;
        //hash=hash<<13;
        //lnmprintf("here5 %x\n",hash);
    };
    hash *= 0x71279461;
    hash /= 3;
    //lnmprintf("here6 %x\n",hash);
    /*
     hash&=something.LNMHSH$L_MASK;
     Not implemented yet
     */
    hash = hash % LNMSHASHTBL;
    //lnmprintf("herei7 %x\n",hash);
    hash = hash & mask;
    //lnmprintf("here3 %x\n",hash);
    *myhash = hash;
    //  printk("hash %lx %ls %s %lx\n",length , log, log, hash);
    //lnmprintf("here2 %x %x\n",myhash,hash);
    return SS$_NORMAL;
}

int lnm$searchlog(struct struct_lnm_ret * r, int loglen, char * lognam, int tabnamlen, char * tablename)
{
    int status;
    void * hash;
    long ahash;
    struct _pcb * pcb = ctl$gl_pcb;
    struct struct_rt * rt = lnmmalloc(sizeof(struct struct_rt));
    struct struct_nt * nt = lnmmalloc(sizeof(struct struct_nt));
    memset(rt, 0, sizeof(struct struct_rt));
    memset(nt, 0, sizeof(struct struct_nt));

#ifdef LNM_DEBUG
    lnmprintf("searchlog\n");
#endif

    status = lnm$hash(loglen, lognam, 0xffff, &ahash);

    nt->loglen = loglen;
    nt->lognam = lognam;
    nt->hash = ahash;

    hash = ctl$gl_lnmhash;
    status = lnm$presearch(r, hash, nt);

    if ((status & 1) == 0)
    {
        hash = &lnmhshs;
        status = lnm$presearch(r, hash, nt);
    }

    if ((status & 1) == 0)
    {
        kfree(nt);
        kfree(rt);
        return status;
    }

    lnm$setup(r, rt, loglen, lognam, nt);
    lnm$table(r, rt, nt);
    lnm$contsearch(r, ahash, hash, nt);
#ifdef LNM_DEBUG
    lnmprintf("searchlogexit\n");
#endif
    kfree(nt);
    kfree(rt);
    return status;
}

int lnm$search_one(struct struct_lnm_ret * r, int loglen, char * logical, int tabnamlen, char * tablename, char * result)
{
    /* lock mutex */
    // lnm$searchlog();
#ifdef LNM_DEBUG
    lnmprintf("searchoneexit\n");
#endif
    /* unlock */
}

int lnm$presearch(struct struct_lnm_ret * r, struct lnmhshs * hashtable, struct struct_nt * nt)
{
    int status;
    unsigned long * myhash;
    // struct _lnmb * mylnmb; not needed?
    myhash = lnmmalloc(sizeof(unsigned long))
             ;
#ifdef LNM_DEBUG
    lnmprintf("presearch %x %s\n",nt->loglen,nt->lognam);
#endif
    status = lnm$hash(nt->loglen, nt->lognam, 0xffff, myhash);
    status = lnm$contsearch(r, *myhash, hashtable, nt);
    //  r->mylnmb=mylnmb; erroneous?
    lnmfree(myhash);
    return status;
}

int lnm$contsearch(struct struct_lnm_ret * r, int hash, struct lnmhshs * hashtable, struct struct_nt * nt)
{
    int status;
    int lenstatus;
    struct _lnmb *head, *tmp, *oldtmp;
    head = hashtable->entry[hash * 2];
#ifdef LNM_DEBUG
    lnmprintf("contsearch %x %x\n",head,hash);
#endif
    if (head)
    {
        tmp = nt->lnmb_cur;
        if (tmp == 0)
        {
            tmp = head;
            nt->lnmb_cur = tmp;
        }
        do
        {
            /*if (tmp->lnmb$b_count>nt->loglen) return SS$_NOLOGNAM; not yet*/
            if (tmp->lnmb$b_count == nt->loglen)
            {
#if 0
                lenstatus=strncmp(nt->lognam,tmp->lnmb$t_name,nt->loglen);
#else
                // do caseless anyway
                int i;
                for (i = 0, lenstatus = 0; i < nt->loglen && lenstatus == 0; i++)
                {
                    char c1, c2;
                    c1 = nt->lognam[i];
                    c2 = tmp->lnmb$t_name[i];
                    if (c1 >= 'A' && c1 <= 'Z')
                        c1 |= 0x20;
                    if (c2 >= 'A' && c2 <= 'Z')
                        c2 |= 0x20;
                    lenstatus = c1 - c2;
                }
#endif
                if (lenstatus == 0)
                {
                    if (nt->lnmth && nt->lnmth != tmp->lnmb$l_table)
                    {
                    }
                    else
                    {
                        r->mylnmb = tmp;
#ifdef LNM_DEBUG
                        lnmprintf("found %x\n",tmp);
#endif
                        return SS$_NORMAL;
                    }
                }
            }
            /* no case-blind search yet */
            nt->lnmb_cur = tmp;
            oldtmp = tmp;
            tmp = tmp->lnmb$l_flink;
        }
        while (tmp != head && tmp != oldtmp);
        nt->lnmb_cur = 0;
    }
    return SS$_NOLOGNAM;
}

int lnm$firsttab(struct struct_lnm_ret * r, int tabnamlen, char * tablename)
{
    struct struct_rt * MYRT;
    struct struct_nt * MYNT;
    MYRT = (struct struct_rt *) lnmmalloc(sizeof(struct struct_rt))
           ;
    memset(MYRT, 0, sizeof(struct struct_rt));
    MYNT = (struct struct_nt *) lnmmalloc(sizeof(struct struct_nt))
           ;
    memset(MYNT, 0, sizeof(struct struct_nt));
#if 0
    MYNT->loglen=tabnamlen;
    MYNT->lognam=tablename;
#endif
#ifdef LNM_DEBUG
    lnmprintf("firstab %s\n",tablename);
#endif
    lnm$setup(r, MYRT, tabnamlen, tablename, MYNT);
    lnmfree(MYRT);
    lnmfree(MYNT);
}

int lnm$setup(struct struct_lnm_ret * r, struct struct_rt * RT, int tabnamlen, char * tablename, struct struct_nt * nt)
{
    int status;
    RT->depth = 0;
    RT->tries = 255;
#ifdef LNM_DEBUG
    lnmprintf("lnm$setup %x %s\n",tablename, tablename);
#endif
    status = lnm$lookup(r, RT, tabnamlen, tablename, nt);
    if (status == SS$_NORMAL)
        RT->context[RT->depth] = r->mylnmb;
    else
        return status;
    /* cache not implemented */
    status = lnm$table_srch(r, RT, nt); // not necessary?
    status = lnm$table(r, RT, nt);
    return status;
}

int lnm$table(struct struct_lnm_ret * r, struct struct_rt * rt, struct struct_nt * nt)
{
    /* cache not implemented */
    int status;
    status = lnm$table_srch(r, rt, nt);
    return status;
}

int lnm$lookup(struct struct_lnm_ret * r, struct struct_rt * rt, int loglen, char * lognam, struct struct_nt * nt)
{
    int status;
    struct _pcb * pcb = ctl$gl_pcb;
    void * hash;
    nt->loglen = loglen;
    nt->lognam = lognam;
#ifdef LNM_DEBUG
    lnmprintf("lookup %s %x\n",nt->lognam,nt->loglen);
#endif
    nt->lnmb = *lnm$al_dirtbl[1];
    hash = ctl$gl_lnmhash;
    status = lnm$presearch(r, hash, nt);
    if ((status & 1) == 0)
    {
        nt->lnmb = *lnm$al_dirtbl[0];
        hash = &lnmhshs;
        status = lnm$presearch(r, hash, nt);
    }
    if (status != SS$_NOLOGNAM)
        return status;
    return status;
}

int lnm$table_srch(struct struct_lnm_ret * r, struct struct_rt *RT, struct struct_nt * nt)
{
    struct _lnmx * lnmx = (r->mylnmb)->lnmb$l_lnmx;
    int len, status;
    do
    {
        RT->tries--;
        if (!RT->tries)
            return SS$_TOOMANYLNAM;
        if ((lnmx->lnmx$l_flags) & LNMX$M_TERMINAL)
            RT->flags |= LNMX$M_TERMINAL;
        else
            RT->flags &= ~LNMX$M_TERMINAL;

        if (RT->depth > 10)
            return SS$_TOOMANYLNAM;
        RT->context[RT->depth] = (r->mylnmb);
        if (lnmx->lnmx$l_index == LNMX$C_TABLE)
        {
            return SS$_NORMAL;
        }
        RT->depth++;
        len = lnmx->lnmx$l_xlen;
#ifdef LNM_DEBUG
        lnmprintf("tsr %x %s \n",lnmx->lnmx$t_xlation,lnmx->lnmx$t_xlation);
#endif
        status = lnm$lookup(r, RT, len, lnmx->lnmx$t_xlation, nt);
        lnmx = lnmx->lnmx$l_next;
    }
    while (lnmx);
    if (lnmx && lnmx->lnmx$l_index == LNMX$C_TABLE)
    {
        RT->depth--;
    }
}

int lnm$inslogtab(struct struct_lnm_ret * r, struct _lnmb * mylnmb)
{
    int status;
    unsigned long * myhash;
    myhash = lnmmalloc(sizeof(unsigned long))
             ;
#ifdef LNM_DEBUG
    lnmprintf("inslog\n");
    lnmprintf("%x %s\n",mylnmb->lnmb$b_count,&(mylnmb->lnmb$t_name[0]));
#endif
    status = lnm$hash(mylnmb->lnmb$b_count, &(mylnmb->lnmb$t_name[0]), 0xffff, myhash);

    // maybe presearch and contsearch is need here also?

    struct lnmhshs * hashtable = &lnmhshs;
    if (mylnmb->lnmb$l_table && mylnmb->lnmb$l_table->lnmth$l_hash)
        hashtable = mylnmb->lnmb$l_table->lnmth$l_hash;
    else
        printk("zero hashtable\n");

    if (hashtable->entry[2 * (*myhash)])
        insque(mylnmb, hashtable->entry[2 * (*myhash)]);
    else
    {
        hashtable->entry[2 * (*myhash)] = mylnmb;
        hashtable->entry[2 * (*myhash) + 1] = mylnmb;
        mylnmb->lnmb$l_flink = mylnmb;
        mylnmb->lnmb$l_blink = mylnmb;
    }
}

int lnm$check_prot()
{
    ;
}

#define lnm$aq_mutex lnm_spin
static struct _mtx lnm_spin =
    { 65535 /* mtx$w_sts : 1, mtx$w_owncnt : -1 */};

void lnm$lock(void)
{
    sch$lockw(&lnm$aq_mutex);
}

void lnm$unlock(void)
{
    sch$unlockw(&lnm$aq_mutex);
}

void lnm$lockr(void)
{
    sch$lockw(&lnm$aq_mutex);
}

void lnm$unlockr(void)
{
    sch$unlockw(&lnm$aq_mutex);
}

void lnm$lockw(void)
{
    sch$lockw(&lnm$aq_mutex);
}

void lnm$unlockw(void)
{
    sch$unlockw(&lnm$aq_mutex);
}

int search_log_prc(char * name, int namelen, char ** retname, int * retsize)
{
#if 0
    $DESCRIPTOR(prc,"LNM$PROCESS_TABLE");
    int sts;
    int retlen;
    struct item_list_3 itm[2];
    struct dsc$descriptor mytabnam, mynam;
    char resstring[LNM$C_NAMLENGTH]="";
    mynam.dsc$w_length=namelen;
    mynam.dsc$a_pointer=name;
    itm[0].item_code=LNM$_STRING;
    itm[0].buflen=LNM$C_NAMLENGTH;
    itm[0].bufaddr=resstring;
    itm[0].retlenaddr=&retlen;
    memset(&itm[1],0,sizeof(struct item_list_3));
    sts = exe$trnlnm(0, &prc, &mynam, 0, itm);

    if (sts&1)
    {
        char * c = kmalloc(retlen,GFP_KERNEL);
        memcpy(c,resstring,retlen);
        *retname = c;
        *retsize = retlen;
    }
    return sts;
#else
    $DESCRIPTOR(prc, "LNM$PROCESS_TABLE");
    int sts;
    int retlen;
    struct item_list_3 itm[2];
    struct dsc$descriptor * mytabnam = &prc;
    struct struct_lnm_ret ret =
        { 0, 0 };
    sts = lnm$searchlog(&ret, namelen, name, mytabnam->dsc$w_length, mytabnam->dsc$a_pointer);
    if (sts & 1)
    {
        retlen = (ret.mylnmb)->lnmb$l_lnmx->lnmx$l_xlen;
        char * c = kmalloc(retlen, GFP_KERNEL);
        memcpy(c, (ret.mylnmb)->lnmb$l_lnmx->lnmx$t_xlation, retlen);
        *retname = c;
        *retsize = retlen;
    }
    return sts;
#endif
}

int search_log_sys(char * name, int namelen, char ** retname, int * retsize)
{
#if 0
    $DESCRIPTOR(sys,"LNM$SYSTEM_TABLE");
    int sts;
    int retlen;
    struct item_list_3 itm[2];
    struct dsc$descriptor mytabnam, mynam;
    char resstring[LNM$C_NAMLENGTH]="";
    mynam.dsc$w_length=namelen;
    mynam.dsc$a_pointer=name;
    itm[0].item_code=LNM$_STRING;
    itm[0].buflen=LNM$C_NAMLENGTH;
    itm[0].bufaddr=resstring;
    itm[0].retlenaddr=&retlen;
    memset(&itm[1],0,sizeof(struct item_list_3));
    sts = exe$trnlnm(0, &sys, &mynam, 0, itm);

    if (sts&1)
    {
        char * c = kmalloc(retlen,GFP_KERNEL);
        memcpy(c,resstring,retlen);
        *retname = c;
        *retsize = retlen;
    }
    return sts;
#else
    $DESCRIPTOR(sys, "LNM$SYSTEM_TABLE");
    int sts;
    int retlen;
    struct item_list_3 itm[2];
    struct dsc$descriptor * mytabnam = &sys;
    struct struct_lnm_ret ret =
        { 0, 0 };
    sts = lnm$searchlog(&ret, namelen, name, mytabnam->dsc$w_length, mytabnam->dsc$a_pointer);
    if (sts & 1)
    {
        retlen = (ret.mylnmb)->lnmb$l_lnmx->lnmx$l_xlen;
        char * c = kmalloc(retlen, GFP_KERNEL);
        memcpy(c, (ret.mylnmb)->lnmb$l_lnmx->lnmx$t_xlation, retlen);
        *retname = c;
        *retsize = retlen;
    }
    return sts;
#endif
}

#if 0
int search_log_repl(char * name, char ** retname, int * retsize)
{
    int sts;
    int namelen=strlen(name);
#ifndef __i386__
    char * myname=kmalloc(namelen,GFP_KERNEL);
#else
    char myname[namelen];
#endif
    memcpy(myname,name,namelen);
    sts=search_log_prc(myname,namelen,retname,retsize);
    if (sts&1) goto ret;
    sts=search_log_sys(myname,namelen,retname,retsize);
    if (sts&1) goto ret;
    char * semi = strchr(myname,':');
    if (semi==0) goto ret;
    * semi = 0;
    sts=search_log_prc(myname,semi-myname,retname,retsize);
    if (sts&1) goto found;
    sts=search_log_sys(myname,semi-myname,retname,retsize);
    if ((sts&1)==0) goto ret;

found:
    {}
    // check leak newret
#ifndef __i386__
    char * newret = kmalloc((*retsize)+namelen-strlen(myname),GFP_KERNEL);
#else
    char * newret;
    int alosize;
    sts = exe_std$alononpaged((*retsize)+namelen-strlen(myname), &alosize, &newret);
#endif
    memcpy(newret,*retname,(*retsize));
    newret[*retsize]=0;
    if (strchr(newret,':'))
    {
        memcpy(newret+(*retsize),semi+1,namelen-strlen(myname)-1);
        (*retsize)--;
    }
    else
    {
        printk("for RMS sys$input, check\n");
        newret[*retsize]=':';
        memcpy(newret+(*retsize),semi,namelen-strlen(myname));
    }
    *retname=newret;
    *retsize=(*retsize)+namelen-strlen(myname);
    //printk("newret %x %s\n",*retsize,newret);

ret:
#ifndef __i386__
    kfree(myname);
#endif
    //if (sts&1) printk("ret %s\n",*retname);
    return sts;
}
#else
int search_log_repl(char * name, char ** retname, int * retsize)
{
    int sts;
    char * semi = 0;
    int namelen = strlen(name);
    int acmode = 0;
    char * newret;
    int alosize;
    int rms_std$alononpaged(int reqsize, int32 *alosize_p, void **pool_p);
    sts = rms_std$alononpaged(128, &alosize, &newret); // check. leak

    struct dsc$descriptor log;
    log.dsc$a_pointer = name;
    log.dsc$w_length = strlen(name);
    $DESCRIPTOR(prc, "LNM$PROCESS_TABLE");
    $DESCRIPTOR(sys, "LNM$SYSTEM_TABLE");
    struct _iosb iosb;
    struct item_list_3 itmlst[2];
    int retlen;
    int * retlenaddr;
    int buflen;
    void * bufaddr;
    char s[255];
    int slen = 255;

    // doing some approximations since I can not now decide the return type

    retlenaddr = retsize;

    buflen = 128;
    bufaddr = newret;

    itmlst[0].item_code = LNM$_STRING;
    itmlst[0].buflen = buflen;
    itmlst[0].retlenaddr = retlenaddr;
    itmlst[0].bufaddr = bufaddr;
    itmlst[1].item_code = 0;

    unsigned int sys$trnlnm();
    sts = sys$trnlnm(0, &prc, &log, acmode, itmlst);

    if (sts & 1)
        goto found;

    sts = sys$trnlnm(0, &sys, &log, acmode, itmlst);

    if (sts & 1)
        goto found;

    semi = strchr(name, ':');
    if (semi == 0)
        return sts;

    log.dsc$w_length = semi - name;

    sts = sys$trnlnm(0, &prc, &log, acmode, itmlst);

    if (sts & 1)
        goto found;

    sts = sys$trnlnm(0, &sys, &log, acmode, itmlst);

    if ((sts & 1) == 0)
        return sts;

found:

    if (semi == 0)
        goto ret;

    newret[*retsize] = 0;
    if (strchr(newret, ':'))
    {
        memcpy(newret + (*retsize), semi + 1, namelen - (semi - name) - 1);
        (*retsize)--;
    }
    else
    {
        printk("for RMS sys$input, check\n");
        newret[*retsize] = ':';
        memcpy(newret + (*retsize), semi, namelen - (semi - name));
    }
    *retsize = (*retsize) + namelen - (semi - name);
ret:
    *retname = newret;

    return sts;
}
#endif
