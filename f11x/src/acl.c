// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <linux/kernel.h>
#include <linux/slab.h>
#include <acedef.h>
#include <acldef.h>
#include <atrdef.h>
#include <descrip.h>
#include <dyndef.h>
#include <fabdef.h>
#include <fcbdef.h>
#define _is_itmlst64 _is_itmlst64_not
#include <iledef.h>
#include <iodef.h>
#include <iosbdef.h>
#include <fatdef.h>
#include <fibdef.h>
#include <fiddef.h>
#include <uicdef.h>
#include <fh2def.h>
#include <misc.h>
#include <namdef.h>
#include <ssdef.h>
#include <starlet.h>
#include <xabdatdef.h>
#include <xabfhcdef.h>

int create_fcb_acl(struct _fcb * fcb, struct _fh2 * fh2)
{
    struct _iosb iosb;
    unsigned int retsts;
    unsigned curvbn=1;
    unsigned extents = 0;
    struct _fh2 *head = 0;

    unsigned short *mp;
    long acep;
    struct _acedef * ace;

#if 0
    struct _vcb * vcb = xqp->current_vcb;
    struct _ucb * ucb;
    if (vcb->vcb$l_rvn)
    {
        struct _rvt * rvt = vcb->vcb$l_rvt;
        struct _ucb ** ucblst = rvt->rvt$l_ucblst;
        ucb = ucblst[fcb->fcb$b_fid_rvn - 1];
    }
    else
        ucb = vcb->vcb$l_rvt;
#endif

    void qhead_init();
    qhead_init(&fcb->fcb$l_aclfl);

    if (fh2)
        head=fh2;
    mp = (unsigned short *) head + head->fh2$b_acoffset;
    acep = mp;
    ace = acep;

    while (ace->ace$b_size)
    {
        switch (ace->ace$b_type)
        {
        case ACE$C_AUDIT:
            break;
        case ACE$C_ALARM:
            break;
        case ACE$C_DIRDEF:
            break;
        case ACE$C_INFO:
            break;
        case ACE$C_KEYID:
            break;
        case ACE$C_RMSJNL_AI:
        case ACE$C_RMSJNL_AT:
        case ACE$C_RMSJNL_BI:
        case ACE$C_RMSJNL_RU:
        case ACE$C_RMSJNL_RU_DEFAULT:
            break;
        default:
            panic("ace does not exist\n");
            break;
        }
        struct _acldef * acl = kmalloc(sizeof(struct _acldef), GFP_KERNEL);
        struct _acedef * newace = kmalloc(ace->ace$b_size, GFP_KERNEL); // check. needed?
        memcpy(newace, ace, ace->ace$b_size);
        acl->acl$w_size = sizeof(struct _acldef);
        acl->acl$b_type = DYN$C_ACL;
        acl->acl$l_list = newace;
        void insque();
        insque(acl, &fcb->fcb$l_aclfl);

        acep += ace->ace$b_size;
        ace = acep;
    }
    retsts = SS$_NORMAL;
    return retsts;
}

int head_write_acl(struct _fcb * fcb, struct _fh2 * fh2)
{
    struct _iosb iosb;
    unsigned int retsts;
    unsigned curvbn=1;
    unsigned extents = 0;
    struct _fh2 *head = 0;

    unsigned short *mp;
    long acep;
    struct _acedef * ace;

#if 0
    struct _vcb * vcb = xqp->current_vcb;
    struct _ucb * ucb;
    if (vcb->vcb$l_rvn)
    {
        struct _rvt * rvt = vcb->vcb$l_rvt;
        struct _ucb ** ucblst = rvt->rvt$l_ucblst;
        ucb = ucblst[fcb->fcb$b_fid_rvn - 1];
    }
    else
        ucb = vcb->vcb$l_rvt;
#endif

    if (fh2)
        head=fh2;
    head->fh2$b_acoffset = head->fh2$b_mpoffset + head->fh2$b_map_inuse;
    mp = (unsigned short *) head + head->fh2$b_acoffset;
    acep = mp;

    struct _acldef * acl = fcb->fcb$l_aclfl;
    while (acl != &fcb->fcb$l_aclfl)
    {
        ace = acl->acl$l_list;
        switch (ace->ace$b_type)
        {
        case ACE$C_AUDIT:
            break;
        case ACE$C_ALARM:
            break;
        case ACE$C_DIRDEF:
            break;
        case ACE$C_INFO:
            break;
        case ACE$C_KEYID:
            break;
        case ACE$C_RMSJNL_AI:
        case ACE$C_RMSJNL_AT:
        case ACE$C_RMSJNL_BI:
        case ACE$C_RMSJNL_RU:
        case ACE$C_RMSJNL_RU_DEFAULT:
            break;
        default:
            panic("ace does not exist\n");
            break;
        }
        //struct _acldef * acl = kmalloc(sizeof(struct _acldef), GFP_KERNEL);
        //struct _acedef * newace = kmalloc(ace->ace$b_size, GFP_KERNEL); // check. needed?
        memcpy(acep, ace, ace->ace$b_size);
        acep += ace->ace$b_size;
        acl = acl->acl$l_flink;
    }
    retsts = SS$_NORMAL;
    return retsts;
}

asmlinkage int exe$change_acl(unsigned short int chan, unsigned int *objtyp, void *objnam, void *itmlst, unsigned int *acmode, struct _iosb *iosb, unsigned int *contxt, int (*routin)(void), void *orbadr)
{
}

asmlinkage int exe$set_security(void *clsnam, void *objnam, unsigned int *objhan, unsigned int flags, void *itmlst, unsigned int *contxt, unsigned int *acmode)
{
    struct dsc$descriptor * cls = clsnam;
    struct _ile3 * itmlst3 = itmlst;
    if (strncmp(cls->dsc$a_pointer, "FILE", 4)==0)
    {
        struct dsc$descriptor * fildsc = objnam;
        char * filename = fildsc->dsc$a_pointer;

        struct _xabfhcdef cc$rms_xabfhc = {XAB$C_FHC,0,0,0,0,0,0,0,0,0,0,0};
        struct _xabdatdef cc$rms_xabdat= {XAB$C_DAT,XAB$C_DATLEN,0,0,0,0,0,0,0,0,0,0};
        struct _fabdef * fab = kmalloc(sizeof(struct _fabdef), GFP_KERNEL);
#if 0
        struct _rabdef * rab = kmalloc(sizeof(struct _rabdef), GFP_KERNEL);
#endif
        // remember too free next two
        struct _xabdatdef * dat = kmalloc(sizeof(struct _xabdatdef), GFP_KERNEL);
        struct _xabfhcdef * fhc = kmalloc(sizeof(struct _xabfhcdef), GFP_KERNEL);
        *fab = cc$rms_fab;
#if 0
        *rab = cc$rms_rab;
#endif
        *dat = cc$rms_xabdat;
        *fhc = cc$rms_xabfhc;
        fab->fab$l_xab = dat;
        dat->xab$l_nxt = fhc;
        fab->fab$l_fna = filename;
        fab->fab$b_fns = strlen(fab->fab$l_fna);
#if 1
        struct _namdef cc$rms_nam = {0,0,0,0,0,0,0,0,0,0,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,0,0};
        char res[NAM$C_MAXRSS + 1], rsa[NAM$C_MAXRSS + 1];
        struct _namdef nam = cc$rms_nam;
        nam.nam$l_esa = res;
        nam.nam$b_ess = NAM$C_MAXRSS;
        nam.nam$l_rsa = rsa;
        nam.nam$b_rss = NAM$C_MAXRSS;
        fab->fab$l_nam = &nam;
#endif
        int exe$open();
        int sts = exe$open(fab, 0, 0);
        if ((sts & 1) == 0)
        {
            printk("Open error: %d\n",sts);
        }
        else
        {
            struct _atrdef atr[2];
            char * buf = itmlst3[0].ile3$ps_bufaddr;
            atr[0].atr$l_addr=buf;
            atr[0].atr$w_type=ATR$C_ADDACLENT;
            atr[0].atr$w_size=ATR$S_HEADER; // check
            atr[1].atr$w_type=0;
            struct dsc$descriptor fibdsc;
            fibdsc.dsc$w_length=sizeof(struct _fibdef);
            void * get_wccfile_fib(struct _fabdef * fab);
            fibdsc.dsc$a_pointer=get_wccfile_fib(fab);
            int ifi_no = fab->fab$w_ifi;
            int get_ifb_table_chan(int);
#define RMS_EF 29
            struct _iosb iosb;
            sts = sys$qiow(RMS_EF,get_ifb_table_chan(ifi_no),IO$_MODIFY|IO$M_ACCESS,&iosb,0,0, // check MODIFY
                           &fibdsc,0,0,0,atr,0);
            sts = iosb.iosb$w_status;
            int exe$close();
            sts = exe$close(fab, 0, 0);
        }
    }
}

asmlinkage int exe$get_security(void *clsnam, void *objnam, unsigned int *objhan, unsigned int flags, void *itmlst, unsigned int *contxt, unsigned int *acmode)
{
    struct dsc$descriptor * cls = clsnam;
    struct _ile3 * itmlst3 = itmlst;
    if (strncmp(cls->dsc$a_pointer, "FILE", 4)==0)
    {
        struct dsc$descriptor * fildsc = objnam;
        char * filename = fildsc->dsc$a_pointer;

        struct _xabfhcdef cc$rms_xabfhc = {XAB$C_FHC,0,0,0,0,0,0,0,0,0,0,0};
        struct _xabdatdef cc$rms_xabdat= {XAB$C_DAT,XAB$C_DATLEN,0,0,0,0,0,0,0,0,0,0};
        struct _fabdef * fab = kmalloc(sizeof(struct _fabdef), GFP_KERNEL);
#if 0
        struct _rabdef * rab = kmalloc(sizeof(struct _rabdef), GFP_KERNEL);
#endif
        // remember too free next two
        struct _xabdatdef * dat = kmalloc(sizeof(struct _xabdatdef), GFP_KERNEL);
        struct _xabfhcdef * fhc = kmalloc(sizeof(struct _xabfhcdef), GFP_KERNEL);
        *fab = cc$rms_fab;
#if 0
        *rab = cc$rms_rab;
#endif
        *dat = cc$rms_xabdat;
        *fhc = cc$rms_xabfhc;
        fab->fab$l_xab = dat;
        dat->xab$l_nxt = fhc;
        fab->fab$l_fna = filename;
        fab->fab$b_fns = strlen(fab->fab$l_fna);
#if 1
        struct _namdef cc$rms_nam = {0,0,0,0,0,0,0,0,0,0,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,NULL,0,0,0};
        char res[NAM$C_MAXRSS + 1], rsa[NAM$C_MAXRSS + 1];
        struct _namdef nam = cc$rms_nam;
        nam.nam$l_esa = res;
        nam.nam$b_ess = NAM$C_MAXRSS;
        nam.nam$l_rsa = rsa;
        nam.nam$b_rss = NAM$C_MAXRSS;
        fab->fab$l_nam = &nam;
#endif
        int exe$open();
        int sts = exe$open(fab, 0, 0);
        if ((sts & 1) == 0)
        {
            printk("Open error: %d\n",sts);
        }
        else
        {
            struct _atrdef atr[2];
            char * retbuf = itmlst3[0].ile3$ps_bufaddr;
            atr[0].atr$l_addr=retbuf;
            atr[0].atr$w_type=ATR$C_HEADER;
            atr[0].atr$w_size=ATR$S_HEADER;
            atr[1].atr$w_type=0;
            struct dsc$descriptor fibdsc;
            fibdsc.dsc$w_length=sizeof(struct _fibdef);
            void * get_wccfile_fib(struct _fabdef * fab);
            fibdsc.dsc$a_pointer=get_wccfile_fib(fab);
            int ifi_no = fab->fab$w_ifi;
            int get_ifb_table_chan(int);
#define RMS_EF 29
            struct _iosb iosb;
            sts = sys$qiow(RMS_EF,get_ifb_table_chan(ifi_no),IO$_ACCESS|IO$M_ACCESS,&iosb,0,0,
                           &fibdsc,0,0,0,atr,0);
            sts = iosb.iosb$w_status;
            int exe$close();
            sts = exe$close(fab, 0, 0);
        }
    }
}

asmlinkage int exe$set_security_wrap(struct struct_args * s)
{
    return exe$set_security(s->s1, s->s2, s->s3, s->s4, s->s5, s->s6, s->s7);
}

asmlinkage int exe$get_security_wrap(struct struct_args * s)
{
    return exe$get_security(s->s1, s->s2, s->s3, s->s4, s->s5, s->s6, s->s7);
}

int check_protect(struct _acldef * acl)
{
    return SS$_NORMAL;
}
