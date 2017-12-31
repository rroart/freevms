// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/config.h>
#include <linux/mm.h>

#include <ssdef.h>
#include <descrip.h>
#include <dyndef.h>
#include <irpdef.h>
#include <misc.h>
#include <ucbdef.h>
#include <ccbdef.h>
#include <ddbdef.h>
#include <ftucbdef.h>
#include <ftrddef.h>
#include <pcbdef.h>
#include <iodef.h>

#include <system_data_cells.h>
#include <misc_routines.h>
#include <ioc_routines.h>
#include <sch_routines.h>
#include <queue.h>
#include <linux/slab.h>

ptd$cancel( unsigned short chan)
{

}

int
ptd$create (unsigned short *chan ,unsigned int acmode ,void *charbuff ,unsigned short int buflen ,void  (*astadr)() ,long astprm ,unsigned int ast_acmode, struct _va_range *inadr)
{
    $DESCRIPTOR(devnam_,"fta0");
    struct dsc$descriptor * devnam=&devnam_;
    struct return_values r;
    struct _ucb * u, *newucb;
    struct _ccb * c;
    struct _pcb * p=ctl$gl_pcb;
    int status;
    status=ioc$ffchan(chan);
    sch$iolockw();
    // printk("here assign %x\n", chan);
    c=&ctl$gl_ccbbase[*chan];
    c->ccb$b_amod=1; /* wherever this gets set */

    ioc$search(&r, devnam) ;
    u=r.val1;

    ioc_std$clone_ucb(u->ucb$l_ddb->ddb$ps_ucb /*&er$ucb*/,&newucb);
    //    exe$assign(dsc,&chan,0,0,0);

    status=ioc$ffchan(chan);
    c=&ctl$gl_ccbbase[*chan];
    c->ccb$b_amod=1; /* wherever this gets set */
    c->ccb$l_ucb=newucb;
    c->ccb$l_ucb->ucb$l_refc++;

    // the rest in specific port driver?

    struct _ft_ucb * ft = newucb;

    ft->ucb$l_ft_ipid=p->pcb$l_pid;
    ft->ucb$w_ft_chan=chan;

    struct _ftrd * ftrd = kmalloc(sizeof(struct _ftrd),GFP_KERNEL);
    memset(ftrd,0,sizeof(struct _ftrd));

    qhead_init(&ft->ucb$l_ft_readqfl);
    insque(ftrd,&ft->ucb$l_ft_readqfl);



}

int
ptd$delete ( unsigned short chan)
{

}

int
ptd$read (unsigned int efn, unsigned short chan ,void (*astadr)() ,long astprm, void * readbuf, unsigned int readbuf_len)
{
    int func = IO$_READPBLK;
    struct _ccb * c=&ctl$gl_ccbbase[chan];
    struct _pcb * p=ctl$gl_pcb;
    struct _ucb * u=c->ccb$l_ucb;
    // what about avoiding irping?
    struct _irp * i=kmalloc(sizeof(struct _irp),GFP_KERNEL);
    memset(i,0,sizeof(struct _irp));
    i->irp$b_type=DYN$C_IRP;
#if 0
    i->irp$b_efn=efn;
#endif
    i->irp$l_ast=astadr;
    i->irp$l_astprm=astprm;
#if 0
    i->irp$l_iosb=iosb;
#endif
    i->irp$w_chan=chan;
    i->irp$l_func=func;
    i->irp$b_pri=p->pcb$b_pri;
    i->irp$l_qio_p1=readbuf;
    i->irp$l_qio_p2=readbuf_len;
    i->irp$l_ucb=u;
    i->irp$l_pid=p->pcb$l_pid;

    ft$fdtread(i,p,u,c);
    return SS$_NORMAL;
}

int
ptd$readw (unsigned int efn, unsigned short chan ,void (*astadr)() ,long astprm, void * readbuf, unsigned int readbuf_len)
{

}

ptd$set_event_notification (unsigned short chan, void (astadr)() ,long astprm ,int acmode, int type)
{

}

int
ptd$write (unsigned short int chan ,void (*astadr)() ,long astprm, void * wrtbuf, unsigned int wrtbuf_len ,void * echobuf ,unsigned int echobuf_len)
{
    int func = IO$_WRITEPBLK;
    struct _ccb * c=&ctl$gl_ccbbase[chan];
    struct _pcb * p=ctl$gl_pcb;
    struct _ucb * u=c->ccb$l_ucb;
    // what about avoiding irping?
    struct _irp * i=kmalloc(sizeof(struct _irp),GFP_KERNEL);
    memset(i,0,sizeof(struct _irp));
    i->irp$b_type=DYN$C_IRP;
#if 0
    i->irp$b_efn=efn;
#endif
    i->irp$l_ast=astadr;
    i->irp$l_astprm=astprm;
#if 0
    i->irp$l_iosb=iosb;
#endif
    i->irp$w_chan=chan;
    i->irp$l_func=func;
    i->irp$b_pri=p->pcb$b_pri;
    i->irp$l_qio_p1=wrtbuf;
    i->irp$l_qio_p2=wrtbuf_len;
    i->irp$l_ucb=u;
    i->irp$l_pid=p->pcb$l_pid;

    ft$fdtwrite(i,p,u,c);
    return SS$_NORMAL;

    //  ucb$l_tt_port
    // port_ft_write

}

