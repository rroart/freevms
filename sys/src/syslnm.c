// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <linux/linkage.h>
#include <linux/mm.h>
#include <starlet.h>
#include <lnmdef.h>
#include <ssdef.h>
#include <misc.h>
#include <descrip.h>
#include <lnmstrdef.h>
#include <orbdef.h>
#include <ipldef.h>
#include <dyndef.h>
#include <system_service_setup.h>
#include <lnmsub.h>
#include <sysgen.h>
#include <system_data_cells.h>
#include <ipl.h>
#include <internals.h>
#include <exe_routines.h>
#include <misc_routines.h>
#include <linux/slab.h>

/* Author: Roar Thron�s */

int isp1(void * a)
{
    return 0;
}

int isshar(void * a)
{
    return 1;
}

asmlinkage sys_$CRELNM();
asmlinkage exe$crelnm(unsigned int *attr, void *tabnam, void *lognam,
                      unsigned char *acmode, void *itmlst)
{
    int status;
    struct struct_lnm_ret ret =
        { 0, 0 };
    struct _lnmb * mylnmb;
    struct _lnmth * mylnmth;
    struct _lnmx * mylnmx = 0;
    struct dsc$descriptor * mytabnam = tabnam;
    struct dsc$descriptor * mylognam = lognam;
    struct item_list_3 * i;
    struct struct_rt * RT;

    if (mytabnam && mytabnam->dsc$a_pointer)
        exe$prober_simple(mytabnam->dsc$a_pointer); // just eventual page in
    if (mylognam && mylognam->dsc$a_pointer)
        exe$prober_simple(mylognam->dsc$a_pointer); // just eventual page in

    setipl(IPL$_ASTDEL);

    mylnmb = lnmmalloc(sizeof(struct _lnmb));
    memset(mylnmb, 0, sizeof(struct _lnmb));
    mylnmth = lnmmalloc(sizeof(struct _lnmth));
    memset(mylnmth, 0, sizeof(struct _lnmth));

    RT = (struct struct_rt *) lnmmalloc(sizeof(struct struct_rt));
    memset(RT, 0, sizeof(struct struct_rt));

    lnm$lockw();

    status = lnm$firsttab(&ret, mytabnam->dsc$w_length,
                          mytabnam->dsc$a_pointer);

    mylnmb = lnmmalloc(sizeof(struct _lnmb));
    memset(mylnmb, 0, sizeof(struct _lnmb));
    mylnmb->lnmb$l_flink = 0;
    mylnmb->lnmb$l_blink = 0;
    mylnmb->lnmb$w_size = sizeof(struct _lnmb);
    mylnmb->lnmb$b_type = DYN$C_LNM;
    mylnmb->lnmb$b_acmode = 0;
    mylnmb->lnmb$l_table = ret.mylnmb->lnmb$l_table;
    mylnmb->lnmb$b_flags = 0;
    mylnmb->lnmb$b_count = mylognam->dsc$w_length;
    mylnmb->lnmb$l_lnmx = 0;
    struct _lnmx ** next_lnmx = &mylnmb->lnmb$l_lnmx;
    strncpy(&(mylnmb->lnmb$t_name[0]), mylognam->dsc$a_pointer,
            mylognam->dsc$w_length);

    for (i = itmlst; i->item_code != 0; i++)
    {
        switch (i->item_code)
        {
        case LNM$_INDEX:
            memcpy(&mylnmx->lnmx$l_index, i->bufaddr, 4);
            break;
        case LNM$_STRING:
            mylnmx = lnmmalloc(sizeof(struct _lnmx));
            memset(mylnmx, 0, sizeof(struct _lnmx));

            *next_lnmx = mylnmx;
            next_lnmx = &mylnmx->lnmx$l_next;

            mylnmx->lnmx$l_xlen = i->buflen;
            strncpy(&(mylnmx->lnmx$t_xlation[0]), i->bufaddr, i->buflen);
            mylnmx->lnmx$l_flags = LNMX$M_TERMINAL;
            mylnmx->lnmx$l_flags |= LNMX$M_XEND;

            break;
        case LNM$_LNMB_ADDR:
            memcpy(i->bufaddr, &mylnmb, i->buflen);
            break;
        default:
            printk("yet unknown LNM param %x\n", i->item_code);
        }
    }

    status = lnm$inslogtab(&ret, mylnmb);

    lnm$unlock();
    setipl(0); // simulate return int
    return status;
}

asmlinkage sys_$CRELNT();
/*asmlinkage sys$crelnt  (unsigned int *attr, void *resnam, unsigned
 short int *reslen, unsigned int *quota,
 unsigned short int *promsk, void *tabnam, void
 *partab, unsigned char *acmode);*/

//asmlinkage int exe$crelnt  (struct struct_crelnt *s) {
asmlinkage int exe$crelnt(unsigned int *attr, void *resnam,
                          unsigned int *reslen, unsigned int *quota, unsigned short *promsk,
                          void *tabnam, void *partab, unsigned char *acmode)
{
    int status;
    /*  char * mytabnam;
     int tabnamlen;*/
    struct struct_lnm_ret ret =
        { 0, 0 };
    struct _lnmb * mylnmb;
    struct _lnmx * mylnmx;
    struct _lnmth * mylnmth;
    struct _orb * myorb;
    long * trailer;
    struct struct_rt * RT;
    struct dsc$descriptor_s * mytabnam = 0, *mypartab;
    if (!(partab))
        return 0;
    mypartab = partab;
#ifdef LNM_DEBUG
    lnmprintf("partab %s\n",mypartab->dsc$a_pointer);
#endif
    if (tabnam)
        mytabnam = tabnam;
    else
    {
        /*    mytabnam="LNM$something";*/
    }

    if (mytabnam && mytabnam->dsc$a_pointer)
        exe$prober_simple(mytabnam->dsc$a_pointer); // just eventual page in
    if (mypartab && mypartab->dsc$a_pointer)
        exe$prober_simple(mypartab->dsc$a_pointer); // just eventual page in

    setipl(IPL$_ASTDEL);
    mylnmb = lnmmalloc(sizeof(struct _lnmb))
             ;
    memset(mylnmb, 0, sizeof(struct _lnmb));
    mylnmx = lnmmalloc(sizeof(struct _lnmx))
             ;
    memset(mylnmx, 0, sizeof(struct _lnmx));
    mylnmth = lnmmalloc(sizeof(struct _lnmth))
              ;
    memset(mylnmth, 0, sizeof(struct _lnmth));
    myorb = lnmmalloc(sizeof(struct _orb))
            ;
    memset(myorb, 0, sizeof(struct _orb));
    trailer = lnmmalloc(sizeof(long))
              ;

    /* mutex lock */
    lnm$lockw();
    RT = (struct struct_rt *) lnmmalloc(sizeof(struct struct_rt))
         ;
    memset(RT, 0, sizeof(struct struct_rt));
#ifdef LNM_DEBUG
    //      lnmprintf("");/* this makes a difference somehow */
    //  lnmprintf("this %x\n",mytabnam->dsc$w_length);
    //  lnmprintf("this %x %s\n",mytabnam->dsc$w_length,mytabnam->dsc$a_pointer);
    lnmprintf("fir %x\n",ret.mylnmth);
#endif
    status = lnm$firsttab(&ret, mypartab->dsc$w_length,
                          mypartab->dsc$a_pointer);
#ifdef LNM_DEBUG
    lnmprintf("fir %x\n",ret.mylnmth);
#endif
    if (status == SS$_NOLOGTAB)
    {
        lnmfree(mylnmb);
        lnmfree(mylnmx);
        lnmfree(mylnmth);
        lnmfree(myorb);
        lnmfree(trailer);
        /* unlock mutex */
        lnm$unlock();
        setipl(0); // simulate return int
        return status;
    }

    if (isp1(mylnmth))
    {
        /*
         lnmfree(mylnmb);
         lnmfree(mylnmx);
         lnmfree(mylnmth);
         lnmfree(myorb);
         lnmfree(trailer);
         deallocate these
         and allocate from process-private region insted
         */
    }
    if (isshar(mylnmth))
    {
        status = lnm$check_prot();
        if (status != SS$_NORMAL)
        {
            lnmfree(mylnmb);
            lnmfree(mylnmx);
            lnmfree(mylnmth);
            lnmfree(myorb);
            lnmfree(trailer);
            /* unlock mutex */
            lnm$unlock();
            setipl(0); // simulate return int
            return SS$_NOPRIV;
        }
    }
    /* check quota

     lnmfree(mylnmb);
     lnmfree(mylnmx);
     lnmfree(mylnmth);
     lnmfree(myorb);
     lnmfree(trailer);
     unlock mutex
     lnm$unlock();
     setipl(0); // simulate return int
     return SS$_EXLNMQUOTA;

     */

    mylnmb->lnmb$l_flink = 0;
    mylnmb->lnmb$l_blink = 0;
    mylnmb->lnmb$w_size = sizeof(struct _lnmb);
    mylnmb->lnmb$b_type = DYN$C_LNM;
    mylnmb->lnmb$b_acmode = 0;
    mylnmb->lnmb$l_lnmx = mylnmx;
    mylnmth = &mylnmx->lnmx$l_xlen;
    mylnmb->lnmb$l_table = mylnmth; /* should be parent */
    mylnmb->lnmb$b_flags = LNM$M_TABLE;
    mylnmb->lnmb$b_count = mytabnam->dsc$w_length;
    strncpy(&(mylnmb->lnmb$t_name[0]), mytabnam->dsc$a_pointer,
            mytabnam->dsc$w_length);

    mylnmx->lnmx$l_flags = LNMX$M_TERMINAL;
    mylnmx->lnmx$l_index = LNMX$C_TABLE;

    mylnmth->lnmth$l_flags = LNMTH$M_SHAREABLE | LNMTH$M_DIRECTORY;
    mylnmth->lnmth$l_name = mylnmb;
    mylnmth->lnmth$l_parent = ret.mylnmb;
    mylnmth->lnmth$l_hash = ret.mylnmb->lnmb$l_table->lnmth$l_hash;
    mylnmth->lnmth$l_sibling = 0;
    mylnmth->lnmth$l_child = 0;
    mylnmth->lnmth$l_qtable = 0;
    mylnmth->lnmth$l_hash = 0;
    mylnmth->lnmth$l_orb = 0;
    mylnmth->lnmth$l_byteslm = 0;
    mylnmth->lnmth$l_bytes = 0;

#ifdef LNM_DEBUG
    lnmprintf("bef inslogtab %x %s\n",mylnmb->lnmb$b_count,mytabnam->dsc$a_pointer);
#endif
    status = lnm$inslogtab(&ret, mylnmb);
    //  lnmprintf("exit here\n");
    //  exit(1);
#ifdef LNM_DEBUG
    lnmprintf("so far %x\n",status);
#endif

    /* unlock mutex */
    lnm$unlock();

    setipl(0); // simulate return int
    return status;
}

asmlinkage sys_$DELLNM()
{
    ;
}
asmlinkage exe$dellnm(void *tabnam, void *lognam, unsigned char *acmode)
{

#if 0
    needs these

    setipl(IPL$_ASTDEL);
    lnm$lockw();

#endif

}

asmlinkage sys_$TRNLNM()
{
    ;
}
asmlinkage exe$trnlnm(unsigned int *attr, void *tabnam, void *lognam,
                      unsigned char *acmode, void *itmlst)
{
    int status;
    struct dsc$descriptor *mytabnam, *mylognam;
    struct struct_lnm_ret ret =
        { 0, 0 };
    struct item_list_3 * i = itmlst;

    mylognam = lognam;
    mytabnam = tabnam;
    if (!(tabnam && itmlst))
        return 0;

    if (mytabnam && mytabnam->dsc$a_pointer)
        exe$prober_simple(mytabnam->dsc$a_pointer); // just eventual page in
    if (mylognam && mylognam->dsc$a_pointer)
        exe$prober_simple(mylognam->dsc$a_pointer); // just eventual page in

    /* lock mutex */
    setipl(IPL$_ASTDEL);
    lnm$lockr();

    status = lnm$searchlog(&ret, mylognam->dsc$w_length,
                           mylognam->dsc$a_pointer, mytabnam->dsc$w_length,
                           mytabnam->dsc$a_pointer);
    if (status == SS$_NOLOGNAM || status != SS$_NORMAL)
    {
        /* unlock mutex */
        lnm$unlock();
        setipl(0);
        return status;
    }
    for (i = itmlst; i->item_code != 0; i++)
    {
        switch (i->item_code)
        {
        case LNM$_INDEX:
            //i->retlenaddr
            //memcpy(i->bufaddr,(ret.mylnmb)->lnmb$l_lnmx->lnmx$t_xlation,4);
            memcpy(i->bufaddr, &(ret.mylnmb)->lnmb$l_lnmx->lnmx$l_index, 4);
#if 0
            if (*(long*)i->bufaddr!=LNMX$C_BACKPTR)
                return SS$_BADPARAM;
#endif
            break;
        case LNM$_STRING:
            i->buflen = (ret.mylnmb)->lnmb$l_lnmx->lnmx$l_xlen;
            if (i->retlenaddr)   // this is right?
            {
                long * l = i->retlenaddr;
                *l = i->buflen; // check
            }
            memcpy(i->bufaddr, (ret.mylnmb)->lnmb$l_lnmx->lnmx$t_xlation,
                   i->buflen);
#ifdef LNM_DEBUG
            lnmprintf("found lnm %x %s\n",i->bufaddr,i->bufaddr);
#endif
            break;
        default:
            printk("another unsupport LNM %x\n", i->item_code);
        }
    }
    lnm$unlock();
    setipl(0); // simulate return int
    return status;
}

asmlinkage int exe$crelnt_wrap(struct struct_crelnt *s)
{
    return exe$crelnt(s->attr, s->resnam, s->reslen, s->quota, s->promsk,
                      s->tabnam, s->partab, s->acmode);
}

void cre_syscommon(char * name)
{
    int sts;
    char * myname = "DKA200:[vms$common.]";
    char * myname2 = "DKA200:[vms$common.sysexe]";
    char * myname3 = "DKA200:[vms$common.syslib]";
    char * myname4 = "DKA200:[vms$common.sys$ldr]";
    $DESCRIPTOR(mytabnam_, "LNM$SYSTEM_TABLE");
    struct dsc$descriptor * mytabnam = &mytabnam_;
    $DESCRIPTOR(dev_, "SYS$COMMON");
    struct dsc$descriptor * dev = &dev_;
    $DESCRIPTOR(dev2_, "SYS$SYSTEM");
    struct dsc$descriptor * dev2 = &dev2_;
    $DESCRIPTOR(dev3_, "SYS$LIBRARY");
    struct dsc$descriptor * dev3 = &dev3_;
    $DESCRIPTOR(dev4_, "SYS$LOADABLE_IMAGES");
    struct dsc$descriptor * dev4 = &dev4_;
    struct item_list_3 itm[2];

    memcpy(myname, name, strlen(name));
    memcpy(myname2, name, strlen(name));
    memcpy(myname3, name, strlen(name));

    itm[0].item_code = LNM$_STRING;
    itm[0].buflen = strlen(myname);
    itm[0].bufaddr = myname;
    memset(&itm[1], 0, sizeof(struct item_list_3));
    sts = exe$crelnm(0, mytabnam, dev, 0, itm);

    itm[0].item_code = LNM$_STRING;
    itm[0].buflen = strlen(myname2);
    itm[0].bufaddr = myname2;
    memset(&itm[1], 0, sizeof(struct item_list_3));
    sts = exe$crelnm(0, mytabnam, dev2, 0, itm);

    itm[0].item_code = LNM$_STRING;
    itm[0].buflen = strlen(myname3);
    itm[0].bufaddr = myname3;
    memset(&itm[1], 0, sizeof(struct item_list_3));
    sts = exe$crelnm(0, mytabnam, dev3, 0, itm);

    itm[0].item_code = LNM$_STRING;
    itm[0].buflen = strlen(myname4);
    itm[0].bufaddr = myname4;
    memset(&itm[1], 0, sizeof(struct item_list_3));
    sts = exe$crelnm(0, mytabnam, dev4, 0, itm);
}
