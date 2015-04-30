// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified Linux source file, 2001-2009
// From 2.4.37.6
// support the same as virtualbox, may skip the rest
// only Intel Pro/1000 MT Desktop (82540EM), Intel Pro/1000 MT Server (82545EM), and Intel Pro/1000 T Server (82543GC)?

#include <crbdef.h>
#include <cxbdef.h>
#include <cdtdef.h>
#include <dcdef.h>
#include <ddtdef.h>
#include <dptdef.h>
#include <fdtdef.h>
#include <pdtdef.h>
#include <idbdef.h>
#include <irpdef.h>
#include <ucbdef.h>
#include <ucbnidef.h>
#include <ddbdef.h>
#include <ipldef.h>
#include <dyndef.h>
#include <ssdef.h>
#include <iodef.h>
#include <devdef.h>
#include <system_data_cells.h>
#include <ipl.h>
#include <linux/pci.h>
#include <system_service_setup.h>
#include <descrip.h>
#include <ioc_routines.h>
#include <misc_routines.h>
#include <exe_routines.h>

#include <linux/netdevice.h>

/*static*/extern int e1000_xmit_frame(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

struct _ucbnidef * ipu;

static unsigned long startio (struct _irp * i, struct _ucb * u)
{
    return (SS$_BADPARAM);
}

static struct _irp * globali;
static struct _ucb * globalu;

static void  startio3 (struct _irp * i, struct _ucb * u)
{
    ioc$reqcom(SS$_NORMAL,0,u);
    return;
};

static void  startio2 (struct _irp * i, struct _ucb * u)
{
    u->ucb$l_fpc=startio3;
    exe$iofork(i,u);
    return;
}

static void ubd_intr2(int irq, void *dev, struct pt_regs *unused)
{
    struct _irp * i;
    struct _ucb * u;
    void (*func)();

    if (intr_blocked(20))
        return;
    regtrap(REG_INTR,20);
    setipl(20);
    /* have to do this until we get things more in order */
    i=globali;
    u=globalu;

    func=u->ucb$l_fpc;
    func(i,u);
#if 0
    myrei();
#endif
}

static struct _fdt ip$fdt =
{
fdt$q_valid:
    IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
fdt$q_buffered:
    IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
//static void  startio ();
static void  unsolint (void) { };
static void  cancel (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
static void  altstart (void) { };
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

static struct _ddt ip$ddt =
{
ddt$l_start:
    startio,
ddt$l_unsolint:
    unsolint,
ddt$l_fdt:
    &ip$fdt,
ddt$l_cancel:
    cancel,
ddt$l_regdump:
    regdump,
ddt$l_diagbuf:
    diagbuf,
ddt$l_errorbuf:
    errorbuf,
ddt$l_unitinit:
    unitinit,
ddt$l_altstart:
    altstart,
ddt$l_mntver:
    mntver,
ddt$l_cloneducb:
    cloneducb,
    ddt$w_fdtsize: 0,
ddt$ps_mntv_sssc:
    mntv_sssc,
ddt$ps_mntv_for:
    mntv_for,
ddt$ps_mntv_sqd:
    mntv_sqd,
ddt$ps_aux_storage:
    aux_storage,
ddt$ps_aux_routine:
    aux_routine
};

int lan$setmode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int lan$sensemode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int lan$setchar(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int lan$sensechar(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int ip$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int ip$writeblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void ip$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb)
{
    ucb->ucb$b_flck=IPL$_IOLOCK8;
    ucb->ucb$b_dipl=IPL$_IOLOCK8;

    ucb->ucb$l_devchar = DEV$M_REC | DEV$M_AVL | DEV$M_CCL | DEV$M_FOD/*| DEV$M_OOV*/;

    ucb->ucb$l_devchar2 = DEV$M_NNM;
    ucb->ucb$b_devclass = DC$_MISC;
    ucb->ucb$b_devtype = DT$_TTYUNKN;
    ucb->ucb$w_devbufsiz = 132;

    ucb->ucb$l_devdepend = 99; // just something to fill

    // dropped the mutex stuff

    return;
}

void ip$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb)
{
    ddb->ddb$ps_ddt=&ip$ddt;
    //dpt_store_isr(crb,nl_isr);
    return;
}

int ip$unit_init (struct _idb * idb, struct _ucb * ucb)
{
    ucb->ucb$v_online = 0;
    //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

    // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
    // no adp or cram stuff

    // or ints etc

    ucb->ucb$v_online = 1;

    return SS$_NORMAL;
}

struct _dpt ip$dpt;
struct _ddb ip$ddb;
struct _ucbnidef ip$ucb ;
struct _crb ip$crb;

int ip$init_tables()
{
    ini_dpt_name(&ip$dpt, "IPDRIVER");
    ini_dpt_adapt(&ip$dpt, 0);
    ini_dpt_defunits(&ip$dpt, 1);
    ini_dpt_ucbsize(&ip$dpt,sizeof(struct _ucbnidef));
    ini_dpt_struc_init(&ip$dpt, ip$struc_init);
    ini_dpt_struc_reinit(&ip$dpt, ip$struc_reinit);
    ini_dpt_ucb_crams(&ip$dpt, 1/*NUMBER_CRAMS*/);
    ini_dpt_end(&ip$dpt);

    ini_ddt_unitinit(&ip$ddt, ip$unit_init);
    ini_ddt_start(&ip$ddt, startio);
    ini_ddt_cancel(&ip$ddt, ioc_std$cancelio);
    ini_ddt_end(&ip$ddt);

    /* for the fdt init part */
    /* a lot of these? */

    ini_fdt_act(&ip$fdt,IO$_READLBLK,ip$readblk,1);
    ini_fdt_act(&ip$fdt,IO$_READPBLK,ip$readblk,1);
    ini_fdt_act(&ip$fdt,IO$_READVBLK,ip$readblk,1);
    ini_fdt_act(&ip$fdt,IO$_WRITELBLK,ip$writeblk,1);
    ini_fdt_act(&ip$fdt,IO$_WRITEPBLK,ip$writeblk,1);
    ini_fdt_act(&ip$fdt,IO$_WRITEVBLK,ip$writeblk,1);
    ini_fdt_act(&ip$fdt,IO$_SETMODE,lan$setmode,1);
    ini_fdt_act(&ip$fdt,IO$_SETCHAR,lan$setchar,1);
    ini_fdt_act(&ip$fdt,IO$_SENSEMODE,lan$sensemode,1);
    ini_fdt_act(&ip$fdt,IO$_SENSECHAR,lan$sensechar,1);
    ini_fdt_end(&ip$fdt);

    return SS$_NORMAL;
}

long ip_iodb_vmsinit(long dev)
{
#if 0
    struct _ucb * ucb=&ip$ucb;
    struct _ddb * ddb=&ip$ddb;
    struct _crb * crb=&ip$crb;
#endif
    struct _ucb * ucb=kmalloc(sizeof(struct _ucbnidef),GFP_KERNEL);
    struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
    struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
    unsigned long idb=0,orb=0;

    memset(ucb,0,sizeof(struct _ucbnidef));
    memset(ddb,0,sizeof(struct _ddb));
    memset(crb,0,sizeof(struct _crb));

#if 0
    init_ddb(&ip$ddb,&ip$ddt,&ip$ucb,"dqa");
    init_ucb(&ip$ucb, &ip$ddb, &ip$ddt, &ip$crb);
    init_crb(&ip$crb);
#endif

    ucb -> ucb$w_size = sizeof(struct _ucbnidef); // temp placed
    ((struct _ucbnidef *)ucb)->ucb$l_extra_l_1=dev;

    init_ddb(ddb,&ip$ddt,ucb,"ipa");
    init_ucb(ucb, ddb, &ip$ddt, crb);
    init_crb(crb);

//  ioc_std$clone_ucb(&er$ucb,&ucb);
    ip$init_tables();
    ip$struc_init (crb, ddb, idb, orb, ucb);
    ip$struc_reinit (crb, ddb, idb, orb, ucb);
    ip$unit_init (idb, ucb);

    insertdevlist(ddb);

    return ddb;

}

long ip_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc)
{
    unsigned short int chan;
    struct _ucbnidef * newucb;
    ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&ip$ucb*/,&newucb);
    exe$assign(dsc,&chan,0,0,0);
    //  registerdevchan(MKDEV(IDE0_MAJOR,unitno),chan);

    ipu = newucb;

    return newucb;
}

extern char * mydevice;

int ip_vmsinit(long dev)
{
    //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

    unsigned short chan0, chan1, chan2;
    $DESCRIPTOR(u0,"ipa0");
    mydevice="ipa0";
    unsigned long idb=0,orb=0;
    struct _ccb * ccb;
    struct _ucb * newucb0,*newucb1,*newucb2;
    struct _ddb * ddb;

    printk(KERN_INFO "dev ipa0 here pre\n");

    ddb=ip_iodb_vmsinit(dev);

    /* for the fdt init part */
    /* a lot of these? */

    ip_iodbunit_vmsinit(ddb,0,&u0);

    printk(KERN_INFO "dev ipa0 here\n");

    // return chan0;

}

int ip$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    return lan$readblk(i,p,u,c);
}

static int ip_timeout(struct _irp * i, struct _ucb * u)
{
    //  startio(i,u);
    ((struct _ucb *)ipu)->ucb$l_duetim = 0; // check. temp fix
    struct _ucb * ucb = ipu;
    void * dev = ((struct _ucbnidef *)ucb)->ucb$l_extra_l_1;
    /*static*/
    extern void e1000_tx_timeout(struct net_device *dev);
    e1000_tx_timeout(dev);
    (dev);
#if 0
    int er$writeblk();
    er$writeblk(i, 0, u, 0); // check. temp workaround. makes dups?
#else
    forklock(((struct _ucb *)ipu)->ucb$b_flck, -1);
    ioc$reqcom(SS$_NORMAL, 0x0800, ipu);
    forkunlock(((struct _ucb *)ipu)->ucb$b_flck, -1);
#endif
    return 1;
}

int ip$writeblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    int savipl=forklock(u->ucb$b_flck,u->ucb$b_flck);
    // hardcode this until we get sizeof struct _iosb down from 12 to 8
#if 0
    if (i->irp$l_iosb) *(long long *)i->irp$l_iosb=SS$_NORMAL|0x080000000000;
#else
    ((struct _ucb *)ipu)->ucb$l_irp = i;
#endif
    struct net_device * dev = ((struct _ucbnidef *)u)->ucb$l_extra_l_1;
    int (*func)() = dev->hard_start_xmit;
    int ip_timeout();
#if 1
#if 1
    if (func == e1000_xmit_frame)
        ioc$wfikpch(ip_timeout, ip_timeout, i, ipu, ipu, 1, 0);
#else
    ioc$wfikpch(ip_timeout, ip_timeout, i, u, u, 1, 0);
#endif
#endif
    int sts = func  (i, p, u, c);
    forkunlock(u->ucb$b_flck,savipl);
#if 1
#if 1
    if (func == e1000_xmit_frame)
#endif
        if (i->irp$l_iosb) *(long long *)i->irp$l_iosb=SS$_NORMAL|0x080000000000;
#endif
    return sts = SS$_NORMAL;
}

#ifndef __OPTIMIZE__
#error You must compile this file with the correct options!
#error See the last lines of the source file.
#error You must compile this driver with "-O".
#endif


/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  tab-width: 4
 * End:
 */
