// $Id$
// $Locker$

// Author. Roar Thron�s.

#include <crbdef.h>
#include <cdtdef.h>
#include <dcdef.h>
#include <ddtdef.h>
#include <dptdef.h>
#include <fdtdef.h>
#include <pdtdef.h>
#include <idbdef.h>
#include <irpdef.h>
#include <ucbdef.h>
#include <ddbdef.h>
#include <ipldef.h>
#include <dyndef.h>
#include <ssdef.h>
#include <iodef.h>
#include <devdef.h>
#include <system_data_cells.h>
#include <ipl.h>
#include <ttyucbdef.h>
#include <ttyvecdef.h>
#include <ioc_routines.h>
#include <internals.h>

#include <linux/slab.h>

/* move this later */
void insertdevlist(struct _ddb *d)
{
    d->ddb$ps_link=ioc$gl_devlist;
    ioc$gl_devlist=d;
}

void * struct_dup(void * v,int size)
{
    void * new = kmalloc(size,GFP_KERNEL);
    memcpy(new,v,size);
    return new;
}

int clone_ddb(struct _ddb * ddb)
{
    struct _ddb * newddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
    struct _ucb * newucb;
    ioc_std$copy_ucb(ddb->ddb$ps_ucb,&newucb);
    memcpy(newddb,ddb,sizeof(struct _ddb));
    newddb->ddb$ps_ucb=newucb;
    newucb->ucb$l_ddb=newddb;
    return newddb;
}

// check. overhaul CLASS_UNIT_INIT
void CLASS_UNIT_INIT(struct _ucb * ucb,struct _tt_port * port_vector)
{
    struct _tty_ucb * tty = ucb;
    struct _dpt * glob_dpt = tty$gl_dpt;
    if (tty->ucb$l_tt_class==0)   // done or not?
    {
        tty->ucb$l_tt_class=glob_dpt->dpt$ps_vector;
        tty->ucb$l_tt_port=port_vector;
        struct _ddb * ddb = ucb->ucb$l_ddb;
        struct _tt_class * ttc=tty->ucb$l_tt_class;
        ucb->ucb$l_ddt=ttc->class_ddt;
        ddb->ddb$l_ddt=ttc->class_ddt;
        tty->ucb$l_tt_getnxt=ttc->class_getnxt;
        tty->ucb$l_tt_putnxt=ttc->class_putnxt;
        // maybe initialize parity and speeds here?
    }
}

#if 0
struct _ucb * makeucbetc(struct _ddb * ddb, struct _ddt * ddt, struct _dpt * dpt, struct _fdt * fdt, char * sddb, char * sdpt)
{
    struct _ucb * u=vmalloc(sizeof(struct _ucb));
    struct _crb * c=vmalloc(sizeof(struct _crb));
    struct _idb * idb=vmalloc(sizeof(struct _idb));
    memset(c,0,sizeof(struct _crb));
    memset(ddb,0,sizeof(struct _ddb));
    memset(u,0,sizeof(struct _ucb));
    memset(idb,0,sizeof(struct _idb));

    dpt->dpt$t_name[0]=strlen(sdpt);
    memcpy(&dpt->dpt$t_name[1],sdpt,dpt->dpt$t_name[0]);

    /* for the ddb init part */
    //  d->ddb$ps_link=d;
    //  d->ddb$ps_blink=d;
    ddb->ddb$b_type=DYN$C_DDB;
    ddb->ddb$l_ddt=ddt;
    ddb->ddb$ps_ucb=u;
    ddb->ddb$t_name[0]=strlen(sddb);
    memcpy(&ddb->ddb$t_name[1],sddb,ddb->ddb$t_name[0]);


    /* for the ucb init part */
    qhead_init(&u->ucb$l_ioqfl);
    u->ucb$b_type=DYN$C_UCB;
    u->ucb$b_flck=IPL$_IOLOCK8;
    /* devchars? */
    u->ucb$b_devclass=DEV$M_RND; /* just maybe? */
    u->ucb$b_dipl=IPL$_IOLOCK8;
    //  bcopy("nla0",u->ucb$t_name,4);
    u->ucb$l_ddb=ddb;
    u->ucb$l_crb=c;
    u->ucb$l_ddt=ddt;

    /* for the crb init part */
    c->crb$b_type=DYN$C_CRB;

    /* and for the ddt init part */
    ddt->ddt$l_fdt=fdt;

    dpt->dpt$ps_ddt=ddt;

    insertdevlist(ddb);

    return u;
}
#endif

static unsigned long devchan[256];

void registerdevchan(unsigned long dev,unsigned short chan)
{
    //  devchan[chan]=MAJOR(dev);
    devchan[chan]=dev;
    printk("registerdevchan dev %x at chan %x\n",devchan[chan],chan);
    //{ int i; for(i=0;i<10000000;i++) ; }
}

unsigned short dev2chan(kdev_t dev)
{
    int i;
    //printk("dev2chan %x %x\n", dev, MAJOR(dev));
    for (i=0; i<256; i++)
        if (devchan[i]==dev) return i;
    //    if (devchan[i]==MAJOR(dev)) return i;
    printk("dev2chan failed\n");
    for (i=0; i<4; i++)
        printk("%x %x\n",devchan[i],dev);
    panic("dev2chan\n");
}

#if 0
kdev_t chan2dev(unsigned short chan)
{
    return devchan[chan];
}
#endif

#if 0
static unsigned long ucbchan[256];

registerucbchan(unsigned long dev,unsigned short chan)
{
    //  devchan[chan]=MAJOR(dev);
    ucbchan[chan]=dev;
    printk("registerucbchan dev %x at chan %x\n",devchan[chan],chan);
    //{ int i; for(i=0;i<10000000;i++) ; }
}

int ucb2chan(unsigned long dev,  unsigned short * chan)
{
    int i;
    //printk("dev2chan %x %x\n", dev, MAJOR(dev));
    for (i=0; i<256; i++)
        if (ucbchan[i]==dev)
        {
            *chan=i;
            return 1;
        }
    //    if (devchan[i]==MAJOR(dev)) return i;
    return 0;

    printk("ucb2chan failed\n");
    for (i=0; i<4; i++)
        printk("%x %x\n",devchan[i],dev);
    panic("ucb2chan\n");
}

unsigned long chan2ucb(unsigned short chan)
{
    return ucbchan[chan];
}
#endif

