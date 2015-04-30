// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <cdtdef.h>
#include <pdtdef.h>

#include <acbdef.h>
#include <aqbdef.h>
#include <cdrpdef.h>
#include <ddbdef.h>
#include <dyndef.h>
#include <descrip.h>
#include <iodef.h>
#include <iosbdef.h>
#include <hrbdef.h>
#include <irpdef.h>
#include <misc.h>
#include <mscpdef.h>
#include <sbdef.h>
#include <scsdef.h>
#include <ssdef.h>
#include <ucbdef.h>
#include <vcbdef.h>
#include <system_data_cells.h>
#include <queue.h>
#include <exe_routines.h>
#include <ioc_routines.h>
#include <misc_routines.h>
#include <scs_routines.h>

#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <starlet.h>

void mscpmyerr(void)
{
    /* do nothing yet */
}

int get_mscp_chan(char * s)
{
    int sts;
    unsigned short int chan;
    struct _ucb * ucb;
    struct dsc$descriptor dsc;
    struct return_values r;
    struct _vcb * vcb;
    struct _aqb * aqb;
    dsc.dsc$a_pointer=do_file_translate(s);
    dsc.dsc$w_length=strlen(dsc.dsc$a_pointer);
    if (ioc$search(&r,&dsc)==SS$_NORMAL)
        return ((struct _ucb *)r.val1)->ucb$ps_adp;
    ucb = fl_init(s);
    vcb = (struct _vcb *) kmalloc(sizeof(struct _vcb),GFP_KERNEL);
    memset(vcb,0,sizeof(struct _vcb));
    vcb->vcb$b_type=DYN$C_VCB;
    aqb = (struct _aqb *) kmalloc(sizeof(struct _aqb),GFP_KERNEL);
    memset(aqb,0,sizeof(struct _aqb));
    aqb->aqb$b_type=DYN$C_AQB;
    qhead_init(&aqb->aqb$l_acpqfl);
    ucb->ucb$l_vcb=vcb;
    vcb->vcb$l_aqb=aqb;
    qhead_init(&vcb->vcb$l_fcbfl);
    vcb->vcb$l_cache = NULL; // ?
    sts = phyio_init(strlen(s),s,&ucb->ucb$l_vcb->vcb$l_aqb->aqb$l_mount_count,0,0); // check
    dsc.dsc$a_pointer=do_file_translate(s);
    dsc.dsc$w_length=strlen(dsc.dsc$a_pointer);
    sts=exe$assign(&dsc,&chan,0,0,0);
    ucb->ucb$ps_adp=chan; //wrong field and use, but....
    return chan;
}

extern struct _cdt cdtl[1024];

static in_workaround=0;

void returnsome(struct _irp *i);

unsigned long long hrbq = 0;

void * find_hrb(int rspid)
{
    struct _hrb * head=&hrbq;
    struct _hrb * tmp=head->hrb$l_flink;
    while (tmp!=head)
    {
        if (rspid==tmp->hrb$l_cmd_time) return tmp;
        tmp=tmp->hrb$l_flink;
    }
    return 0;
}

extern struct _ddt du$ddt;

int mscplisten(void * packet, struct _cdt * c, struct _pdt * p)
{
    int sts;
    struct _iosb * iosb=kmalloc(sizeof(struct _iosb),GFP_KERNEL);
    struct _cdrp * cdrp;
    struct _scs * scs = packet;
    struct _scs1 * scs1 = scs;
    struct _ppd * ppd = scs;
    struct _cdt * cdt = &cdtl[scs->scs$l_dst_conid];
    struct _mscp_basic_pkt * basic = ((unsigned long)packet) + sizeof(*scs);
    void * next = basic;
    struct _transfer_commands * trans = basic;
    unsigned short int chan;//wasint chan =get_mscp_chan(cdt->cdt$l_condat);
    char * nam;
    $DESCRIPTOR(devnam,"d0a0");
    nam=devnam.dsc$a_pointer;
    nam[1]=basic->mscp$b_caa;
    nam[3]=48+basic->mscp$w_unit;

    exe$assign(&devnam,&chan,0,0,0);

    if (hrbq==0) qhead_init(&hrbq);

    //printk("packet %x %x %x %x\n",packet,ppd,ppd->ppd$b_opc,chan);

    switch (ppd->ppd$b_opc)
    {
    case PPD$C_REQDAT:
        goto read;
        break;
    case PPD$C_SNDDAT:
        goto write;
        break;
    case PPD$C_SNDDG:
        goto mscp;
        break;
    default:
        panic("ppdopc\n");

    }

mscp:
    {
        unsigned long lbn=trans->mscp$l_lbn;
        struct _hrb * hrb=kmalloc(sizeof(struct _hrb),GFP_KERNEL);
        cdrp = vmalloc(sizeof(struct _cdrp));
        memset(cdrp,0,sizeof(struct _cdrp));
        memset(hrb,0,sizeof(struct _hrb));
        hrb->hrb$l_lbn=lbn;
        hrb->hrb$l_cmd_time=scs1->scs$l_rspid; // wrong, but have to have some id...
        insque(hrb,&hrbq);
        if (basic->mscp$b_opcode == MSCP$K_OP_WRITE)
        {
            basic->mscp$b_opcode = MSCP$K_OP_END;
            cdrp->cdrp$w_cdrpsize=600;
            cdrp->cdrp$l_cdt=c;
            cdrp->cdrp$l_msg_buf=basic;
            cdrp->cdrp$l_xct_len=512;
            //scs_std$senddg(0,600,cdrp);
        }
        if (basic->mscp$b_opcode == MSCP$K_OP_READ)
        {
        }
        if (basic->mscp$b_opcode == MSCP$K_OP_GTUNT)
        {
            $DESCRIPTOR(disk,"dqa0");
            char * str = disk.dsc$a_pointer;
            str[3]='0'+basic->mscp$w_unit;
            long long dummy;
            int sts=ioc$search(&dummy, &disk);
            struct _gtunt * gtunt = kmalloc(sizeof(struct _gtunt),GFP_KERNEL);
            memset(gtunt, 0, sizeof(struct _gtunt));
            struct _mscp_basic_pkt * gtbas = gtunt;
            gtbas->mscp$b_opcode=MSCP$K_OP_GTUNT|MSCP$M_OP_END;
            gtbas->mscp$w_unit=basic->mscp$w_unit;
            if (sts==SS$_NORMAL)
                gtbas->mscp$w_status=MSCP$K_ST_SUCC;
            else
                gtbas->mscp$w_status=MSCP$K_ST_OFFLN;

            struct _cdrp * cdrp = vmalloc(sizeof(struct _cdrp));
            memset(cdrp,0,sizeof(struct _cdrp));

            cdrp->cdrp$w_cdrpsize=400;
            cdrp->cdrp$l_rspid=scs_std$alloc_rspid(0,0,cdrp,0);
            cdrp->cdrp$l_cdt=find_mscp_cdt();
            cdrp->cdrp$l_msg_buf=gtunt;
            cdrp->cdrp$l_xct_len=sizeof(struct _gtunt);
            if (cdrp->cdrp$l_cdt==0)
            {
                printk("gtunt srv no cdt\n");
                return;
            }
            scs_std$senddg(0,400,cdrp);

        }
        if (basic->mscp$b_opcode == (MSCP$K_OP_GTUNT|MSCP$M_OP_END) )
        {
            if (basic->mscp$w_status==MSCP$K_ST_SUCC)
            {
                struct _ddb * ddb;
                struct _ucb * ucb;
                char dev[16];
                extern struct _sb othersb;
                char len=othersb.sb$t_nodename[0];
                memcpy(dev,&othersb.sb$t_nodename[1],len);
                dev[len]='$';
                dev[len+1]='d';
                dev[len+2]='q';
                dev[len+3]='a';
                dev[len+4]='0';//+basic->mscp$w_unit;
                struct dsc$descriptor disk;
                disk.dsc$a_pointer=dev;
                disk.dsc$w_length=len+5;
                long long dummy;
                int sts=ioc$search(&dummy, &disk);
                if (sts!=SS$_NORMAL)
                {
                    ddb=ide_iodb_vmsinit(1);
                    du_iodb_clu_vmsinit(ddb->ddb$ps_ucb);
                    struct _ucb * ucb = ddb->ddb$ps_ucb;
                    ucb->ucb$l_ddt=&du$ddt;
                    if (ddb)
                        ddb->ddb$ps_sb=&othersb;
                }
                else
                {
                    long *l=&dummy;
                    struct _ucb * u = *l;
                    ddb = u->ucb$l_ddb;
                }
                int i=basic->mscp$w_unit;
                $DESCRIPTOR(d, "dqa0");
                char * c= d.dsc$a_pointer;
                c[3]='0'+i;
                ucb = ide_iodbunit_vmsinit(ddb,i,&d);
                ucb->ucb$l_ddt=&du$ddt;
                ((struct _mscp_ucb *)ucb)->ucb$w_mscpunit=ucb->ucb$w_unit;
                printk("UCB MSCPUNIT %x\n",((struct _mscp_ucb *)ucb)->ucb$w_mscpunit);
            }
        }
    }
    return;

read:
    {
        char * buf = vmalloc(1024);
        struct _irp * i=kmalloc(sizeof(struct _irp),GFP_KERNEL);
        struct _ucb * u;
        struct _hrb * hrb = find_hrb(scs1->scs$l_rspid);
        //printk("mscpread\n");
        remque(hrb,hrb);
        memset(i,0,sizeof(struct _irp));
        iosb->iosb$w_status=0;
        i->irp$b_type=DYN$C_IRP;
        i->irp$w_chan=chan;
        i->irp$l_func=IO$_READLBLK;
        i->irp$l_iosb=iosb;
        i->irp$l_qio_p1=buf;
        i->irp$l_qio_p2=512;
        i->irp$l_qio_p3=hrb->hrb$l_lbn;
        ((struct _acb *)i)->acb$b_rmod|=ACB$M_NODELETE; // bad idea to free this
        //printk("chan %x\n",chan);
        u=ctl$gl_ccbbase[chan].ccb$l_ucb;
        i->irp$l_ucb=u;
        i->irp$l_pid=current->pcb$l_pid;
        i->irp$l_sts|=IRP$M_BUFIO;
        i->irp$l_astprm=i;
        i->irp$l_ast=returnsome;
        i->irp$l_cdt=c;
        i->irp$l_rspid=scs1->scs$l_rspid;
        exe$insioq(i,u);
    }
    exe$dassgn(chan);
    return;
write:
    {
        struct _irp * i=kmalloc(sizeof(struct _irp),GFP_KERNEL);
        struct _ucb * u;
        struct _hrb * hrb = find_hrb(scs1->scs$l_rspid);
        //printk("mscpwrite\n");
        remque(hrb,hrb);
        memset(i,0,sizeof(struct _irp));
        iosb->iosb$w_status=0;
        i->irp$b_type=DYN$C_IRP;
        i->irp$w_chan=chan;
        i->irp$l_func=IO$_WRITELBLK;
        i->irp$l_iosb=iosb;
        i->irp$l_qio_p1=next;
        i->irp$l_qio_p2=512;
        i->irp$l_qio_p3=hrb->hrb$l_lbn;
        //printk("chan %x\n",chan);
        u=ctl$gl_ccbbase[chan].ccb$l_ucb;
        i->irp$l_ucb=u;
        i->irp$l_pid=current->pcb$l_pid;
        i->irp$l_sts|=IRP$M_BUFIO;
        i->irp$l_astprm=i;
        i->irp$l_ast=returnsome;
        i->irp$l_cdt=c;
        i->irp$l_rspid=scs1->scs$l_rspid;
        exe$insioq(i,u);
    }
    exe$dassgn(chan);
    return;
}

extern struct _pdt dupdt;

void returnsome(struct _irp * i)
{
    struct _cdrp * cdrp = vmalloc(sizeof(struct _cdrp));
    struct _mscp_basic_pkt * basic = vmalloc(sizeof(struct _mscp_basic_pkt));
    memset(cdrp,0,sizeof(struct _cdrp));
    memset(basic,0,sizeof(struct _mscp_basic_pkt));
    //basic->mscp$b_caa=MSCP$K_OP_READ;
    basic->mscp$b_opcode = MSCP$K_OP_END;
    basic->mscp$l_cmd_ref=0;
    //bcopy(i->irp$l_qio_p1,(unsigned long)basic + sizeof(struct _transfer_commands),512);
    cdrp->cdrp$w_cdrpsize=600;
    cdrp->cdrp$l_rspid=i->irp$l_rspid;
    cdrp->cdrp$l_cdt=i->irp$l_cdt;
    cdrp->cdrp$l_msg_buf=i->irp$l_qio_p1;
    cdrp->cdrp$l_xct_len=512;
    scs_std$senddata(&dupdt,cdrp,0);
    return;
    cdrp->cdrp$l_msg_buf=basic;
    cdrp->cdrp$l_xct_len=sizeof(struct _mscp_basic_pkt);
    scs_std$senddg(0,600,cdrp);
}

void mscpdaemonize() { }

/* mscp.exe mscp$disk let it be a kernel_thread? maybe not... */
int mscp(void)
{
    char myname[]="mscp$disk";
    char myinfo[]="mscp disk";
    mscpdaemonize(); /* find out what this does */

    //  listen(msgbuf,err,cdt,pdt,cdt);
    scs_std$listen(mscplisten,mscpmyerr,myname,myinfo,0);
}

void mscp_talk_with(char * node, char * sysap)
{
    // both parameters are unused right now
    // we know they are the other node, and the second is mscp$disk

    __du_init(); //temp placement?

    signed long long time=-100000000;
    int unit;
    struct _cdrp * cdrp = vmalloc(sizeof(struct _cdrp));
    struct _mscp_basic_pkt * basic = vmalloc(sizeof(struct _mscp_basic_pkt));
    memset(cdrp,0,sizeof(struct _cdrp));
    memset(basic,0,sizeof(struct _mscp_basic_pkt));
    //basic->mscp$b_caa=MSCP$K_OP_READ;
    for (unit=1; unit < 5; unit ++)
    {
        exe$schdwk(0,0,&time,0);
        sys$hiber();
        printk("probing other node dqa%x\n",unit);
        basic->mscp$b_opcode = MSCP$K_OP_GTUNT;
        basic->mscp$w_unit=unit;
        basic->mscp$l_cmd_ref=0;
        cdrp->cdrp$w_cdrpsize=600;
        cdrp->cdrp$l_rspid=scs_std$alloc_rspid(0,0,cdrp,0);
        cdrp->cdrp$l_cdt=find_mscp_cdt();
        cdrp->cdrp$l_msg_buf=basic;
        cdrp->cdrp$l_xct_len=sizeof(struct _mscp_basic_pkt);
        if (cdrp->cdrp$l_cdt==0)
        {
            printk("gtunt cli no cdt\n");
            return;
        }
        scs_std$senddg(0,600,cdrp);
    }
}
