// $Id$
// $Locker$

// Author. Roar Thron�s.

// This is supposed to be a file driver

#include <aqbdef.h>
#include <cdtdef.h>
#include <crbdef.h>
#include <dcdef.h>
#include <ddbdef.h>
#include <ddtdef.h>
#include <devdef.h>
#include <dptdef.h>
#include <dyndef.h>
#include <fdtdef.h>
#include <iodef.h>
#include <ipl.h>
#include <ipldef.h>
#include <irpdef.h>
#include <pdtdef.h>
#include <ssdef.h>
#include <system_data_cells.h>
#include <ucbdef.h>
#include <vcbdef.h>
#include <exe_routines.h>
#include <ioc_routines.h>
#include <misc_routines.h>

#include <linux/mm.h>
#include <linux/slab.h>

struct _fdt file$fdt =
    { fdt$q_valid:
    IO$_NOP | IO$_UNLOAD | IO$_AVAILABLE | IO$_PACKACK | IO$_SENSECHAR | IO$_SETCHAR | IO$_SENSEMODE | IO$_SETMODE | IO$_WRITECHECK
            | IO$_READPBLK | IO$_WRITELBLK | IO$_DSE | IO$_ACCESS | IO$_ACPCONTROL | IO$_CREATE | IO$_DEACCESS | IO$_DELETE
            | IO$_MODIFY | IO$_MOUNT | IO$_READRCT | IO$_CRESHAD | IO$_ADDSHAD | IO$_COPYSHAD | IO$_REMSHAD | IO$_SHADMV
            | IO$_DISPLAY | IO$_SETPRFPATH | IO$_FORMAT, fdt$q_buffered:
    IO$_NOP | IO$_UNLOAD | IO$_AVAILABLE | IO$_PACKACK | IO$_DSE | IO$_SENSECHAR | IO$_SETCHAR | IO$_SENSEMODE | IO$_SETMODE
            | IO$_ACCESS | IO$_ACPCONTROL | IO$_CREATE | IO$_DEACCESS | IO$_DELETE | IO$_MODIFY | IO$_MOUNT | IO$_CRESHAD
            | IO$_ADDSHAD | IO$_COPYSHAD | IO$_REMSHAD | IO$_SHADMV | IO$_DISPLAY | IO$_FORMAT };

struct _irp * fglobali;
struct _ucb * fglobalu;

void fl_isr(struct _irp * i)
{
    int sts;
    void (*func)(void *, void *);
    unsigned handle = i->irp$l_ucb->ucb$l_vcb->vcb$l_aqb->aqb$l_mount_count;
    unsigned block = i->irp$l_qio_p3;
    unsigned length = i->irp$l_qio_p2;
    char *buffer = i->irp$l_qio_p1;

#if 0
    if (intr_blocked(20))
    return;
    regtrap(REG_INTR,20);
    setipl(20);
#endif
    //printk("isr\n");

    if (i == 0)
    {

    }

    if (i->irp$v_fcode == IO$_READLBLK)
    {

        sts = phyio_read(handle, block, length, buffer);

    }
    else
    {

        sts = phyio_write(handle, block, length, buffer);

    }

    /* have to do this until we get things more in order */

    func = i->irp$l_ucb->ucb$l_fpc;
    func(i, i->irp$l_ucb);
    //  myrei();
}

void file_startio2(struct _irp * i, struct _ucb * u);
void file_startio3(int fr3, int fr4, struct _ucb * u);

void file_startio(struct _irp * i, struct _ucb * u)
{
    int sts;
    static int first = 0;
    signed long long step1 = -1000000;
    //printk("times %x %x\n",u->ucb$b_second_time_in_startio,u->ucb$b_third_time_in_startio);
    //  { int j; for(j=100000000;j;j--);}

#if 0
    if (first)
    {
        int a=*(int *)0x88888888;
        int b=*(int *)0x82888888;
        int c=*(int *)0x98888888;
        int d=*(int *)0xb8888888;
        insque(0,0);
    }
    first++;
#endif

    //printk("firsttime %x %x\n",i,u);
    fglobali = i;
    fglobalu = u;

    u->ucb$b_second_time_in_startio = 1;

    ioc$wfikpch(file_startio2, 0, i, ctl$gl_pcb, u, 2, 0);
    exe$setimr(0, &step1, fl_isr, i, 0);
    return;
}

void file_startio2(struct _irp * i, struct _ucb * u)
{
    //printk("secondtime\n");

    u->ucb$l_fpc = file_startio3;
    exe_std$primitive_fork(0, 0, u);
    return;
}

void file_startio3(int fr3, int fr4, struct _ucb * u)
{
    //printk("thirdtime %x %x\n",i,u);
    ioc$reqcom(SS$_NORMAL, 0, u);
    return;
}
;

/* more yet undefined dummies */
void file_unsolint(void)
{
}
;
void file_cancel(void)
{
}
;
void file_regdump(void)
{
}
;
void file_diagbuf(void)
{
}
;
void file_errorbuf(void)
{
}
;
void file_unitinit(void)
{
}
;
void file_altstart(void)
{
}
;
void file_mntver(void)
{
}
;
void file_cloneducb(void)
{
}
;
void file_mntv_sssc(void)
{
}
;
void file_mntv_for(void)
{
}
;
void file_mntv_sqd(void)
{
}
;
void file_aux_storage(void)
{
}
;
void file_aux_routine(void)
{
}
;

int acp_std$access(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$modify(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$mount(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$access(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$deaccess(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$writeblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

void fl_read(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    exe$qiodrvpkt(i, p, u);
}
;

void file_write(void)
{

}
;

struct _ddt file$ddt =
    { ddt$l_start:
    file_startio, ddt$l_unsolint:
    file_unsolint, ddt$l_fdt:
    &file$fdt, ddt$l_cancel:
    file_cancel, ddt$l_regdump:
    file_regdump, ddt$l_diagbuf:
    file_diagbuf, ddt$l_errorbuf:
    file_errorbuf, ddt$l_unitinit:
    file_unitinit, ddt$l_altstart:
    file_altstart, ddt$l_mntver:
    file_mntver, ddt$l_cloneducb:
    file_cloneducb, ddt$w_fdtsize: 0, ddt$ps_mntv_sssc:
    file_mntv_sssc, ddt$ps_mntv_for:
    file_mntv_for, ddt$ps_mntv_sqd:
    file_mntv_sqd, ddt$ps_aux_storage:
    file_aux_storage, ddt$ps_aux_routine:
    file_aux_routine };

/* include a buffered 4th param? */
extern inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long);

void file$struc_init(struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb)
{
    ucb->ucb$b_flck = IPL$_IOLOCK8;
    ucb->ucb$b_dipl = IPL$_IOLOCK8;

    ucb->ucb$l_devchar = DEV$M_REC | DEV$M_AVL | DEV$M_CCL /*| DEV$M_OOV*/;

    ucb->ucb$l_devchar2 = DEV$M_NNM;
    ucb->ucb$b_devclass = DC$_DISK;
    ucb->ucb$b_devtype = DT$_TTYUNKN;
    ucb->ucb$w_devbufsiz = 132;

    ucb->ucb$l_devdepend = 99; // just something to fill

    // dropped the mutex stuff

    return;
}

void file$struc_reinit(struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb)
{
    ddb->ddb$ps_ddt = &file$ddt;
    //dpt_store_isr(crb,nl_isr);
    return;
}

int file$unit_init(struct _idb * idb, struct _ucb * ucb)
{
    ucb->ucb$v_online = 0;
    //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

    // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
    // no adp or cram stuff

    // or ints etc

    ucb->ucb$v_online = 1;

    return SS$_NORMAL;
}

struct _dpt file$dpt;
struct _ddb file$ddb;
struct _dt_ucb file$ucb;
struct _crb file$crb;

int file$init_tables()
{
    ini_dpt_name(&file$dpt, "DFDRIVER");
    ini_dpt_adapt(&file$dpt, 0);
    ini_dpt_defunits(&file$dpt, 1);
    ini_dpt_ucbsize(&file$dpt, sizeof(struct _dt_ucb));
    ini_dpt_struc_init(&file$dpt, file$struc_init);
    ini_dpt_struc_reinit(&file$dpt, file$struc_reinit);
    ini_dpt_ucb_crams(&file$dpt, 1/*NUMBER_CRAMS*/);
    ini_dpt_end(&file$dpt);

    ini_ddt_unitinit(&file$ddt, file$unit_init);
    ini_ddt_start(&file$ddt, file_startio);
    ini_ddt_cancel(&file$ddt, ioc_std$cancelio);
    ini_ddt_end(&file$ddt);

    /* for the fdt init part */
    /* a lot of these? */
    ini_fdt_act(&file$fdt, IO$_ACCESS, acp_std$access, 1);
    ini_fdt_act(&file$fdt, IO$_READLBLK, acp_std$readblk, 1);
    ini_fdt_act(&file$fdt, IO$_READPBLK, acp_std$readblk, 1);
    ini_fdt_act(&file$fdt, IO$_READVBLK, acp_std$readblk, 1);
    ini_fdt_act(&file$fdt, IO$_WRITELBLK, acp_std$writeblk, 1);
    ini_fdt_act(&file$fdt, IO$_WRITEPBLK, acp_std$writeblk, 1);
    ini_fdt_act(&file$fdt, IO$_WRITEVBLK, acp_std$writeblk, 1);
    ini_fdt_act(&file$fdt, IO$_CREATE, acp_std$access, 1);
    ini_fdt_act(&file$fdt, IO$_DEACCESS, acp_std$deaccess, 1);
    ini_fdt_act(&file$fdt, IO$_DELETE, acp_std$modify, 1);
    ini_fdt_act(&file$fdt, IO$_MODIFY, acp_std$modify, 1);
    ini_fdt_act(&file$fdt, IO$_ACPCONTROL, acp_std$modify, 1);
    ini_fdt_act(&file$fdt, IO$_MOUNT, acp_std$mount, 1);
    ini_fdt_end(&file$fdt);

    return SS$_NORMAL;
}

//static struct _fdt file_fdt;

struct _dpt file_dpt;

#if 0
struct _ddb nullddb;
struct _ucb nullucb;
struct _crb nullcrb;
struct _ccb nullccb;
#endif

long file_iodb_vmsinit(void)
{
#if 0
    struct _ucb * ucb=&file$ucb;
    struct _ddb * ddb=&file$ddb;
    struct _crb * crb=&file$crb;
#endif
    struct _ucb * ucb = kmalloc(sizeof(struct _dt_ucb), GFP_KERNEL);
    struct _ddb * ddb = kmalloc(sizeof(struct _ddb), GFP_KERNEL);
    struct _crb * crb = kmalloc(sizeof(struct _crb), GFP_KERNEL);
    unsigned long idb = 0, orb = 0;
    struct _ccb * ccb;
    struct _ucb * newucb, newucb1;
    struct _ddb * newddb;

//  ioc_std$clone_ucb(&file$ucb,&ucb);
    memset(ucb, 0, sizeof(struct _dt_ucb));
    memset(ddb, 0, sizeof(struct _ddb));
    memset(crb, 0, sizeof(struct _crb));

#if 0
    init_ddb(&file$ddb,&file$ddt,&file$ucb,"dfa");
    init_ucb(&file$ucb, &file$ddb, &file$ddt, &file$crb);
    init_crb(&file$crb);
#endif

    init_ddb(ddb, &file$ddt, ucb, "dfa");
    init_ucb(ucb, ddb, &file$ddt, crb);
    init_crb(crb);

    ucb->ucb$w_size = sizeof(struct _dt_ucb); // temp placed

    file$init_tables();
    file$struc_init(crb, ddb, idb, orb, ucb);
    file$struc_reinit(crb, ddb, idb, orb, ucb);
    file$unit_init(idb, ucb);

    insertdevlist(ddb);

    return ddb;
}

void __fl_init(void)
{
    file_iodb_vmsinit();
}

struct _ucb * myfilelist[50];
char * myfilelists[50];
int myfilelistptr = 0;

void insertfillist(struct _ucb *u, char *s)
{
    myfilelists[myfilelistptr] = kmalloc(strlen(s) + 1, 0);
    memcpy(myfilelists[myfilelistptr], s, strlen(s) + 1);
    myfilelist[myfilelistptr++] = u;
}

long file_iodbunit_vmsinit(struct _ddb * ddb, int unitno, void * d)
{
    struct _ucb * newucb;

    ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&file$ucb*/, &newucb);

    return newucb;
}

extern struct _sb mysb;

struct _ucb * fl_init(char * s)
{
    struct _ucb * newucb;
    struct _ddb * ddb = ioc$gl_devlist;

    while (ddb)
    {
        if (ddb->ddb$ps_sb == 0 || ddb->ddb$ps_sb == &mysb)
            if (!memcmp(&ddb->ddb$t_name[1], "dfa", 3))
                goto out;
        ddb = ddb->ddb$ps_link;
    }
    out:

    ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&file$ucb*/, &newucb);

    newucb->ucb$l_orb = myfilelistptr;

    insertfillist(newucb, s);

    return newucb;
}

char * do_file_translate(char * from)
{
    int i;
    int remote = (*from == '_');
    if (remote)
        from++;
    for (i = 0; i < myfilelistptr; i++)
    {
        if (0 == strncmp(from, myfilelists[i], strlen(myfilelists[i])))
        {
            char * c = kmalloc(strlen(myfilelists[i]) + 2, GFP_KERNEL);
            if (remote)
                memcpy(c, "dua0", 4);
            else
                memcpy(c, "dfa0", 4);
            c[3] += i;
            c[4] = 0;
            return c;
        }
    }
    return from;
}

#if 0
/* move this later */
insertdevlist(struct _ddb *d)
{
    d->ddb$ps_link=ioc$gl_devlist;
    ioc$gl_devlist=d;
}

/* just putting this here until I find out where it belong */

inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type)
{
    f->fdt$ps_func_rtn[mask]=fn;
    f->fdt$q_valid|=mask;
    if (type)
    f->fdt$q_buffered|=mask;
}

inline void ini_dpt_name(struct _dpt * d, char * n)
{
    bcopy(n,d->dpt$t_name,strlen(n));
}

inline void ini_dpt_adapt(struct _dpt * d, unsigned long type)
{
    d->dpt$b_adptype=type;
}

inline void ini_dpt_flags(struct _dpt * d, unsigned long type)
{
    d->dpt$l_flags=type;
}

inline void ini_dpt_maxunits(struct _dpt * d, unsigned long type)
{
    d->dpt$w_maxunits=type;
}

inline void ini_dpt_ucbsize(struct _dpt * d, unsigned long type)
{
    d->dpt$w_ucbsize=type;
}

inline void ini_dpt_struc_init(struct _dpt * d, unsigned long type)
{
    d->dpt$ps_init_pd=type;
}

inline void ini_dpt_struc_reinit(struct _dpt * d, unsigned long type)
{
    d->dpt$ps_reinit_pd=type;
}

inline void ini_dpt_ucb_crams(struct _dpt * d, unsigned long type)
{
    //  d->dpt$iw_ucb_crams=type; not now
}

inline void ini_dpt_end(struct _dpt * d)
{
    //  d->dpt$_=type; ??
}

inline void ini_ddt_ctrlinit(struct _ddt * d, unsigned long type)
{
    d->ddt$ps_ctrlinit_2=type;
}

inline void ini_ddt_unitinit(struct _ddt * d, unsigned long type)
{
    d->ddt$l_unitinit=type;
}

inline void ini_ddt_start(struct _ddt * d, unsigned long type)
{
    d->ddt$ps_start_2=type;
}

inline void ini_ddt_kp_startio(struct _ddt * d, unsigned long type)
{
    d->ddt$ps_kp_startio=type;
}

inline void ini_ddt_kp_stack_size(struct _ddt * d, unsigned long type)
{
    d->ddt$is_stack_bcnt=type;
}

inline void ini_ddt_kp_reg_mask(struct _ddt * d, unsigned long type)
{
    d->ddt$is_reg_mask=type;
}

inline void ini_ddt_cancel(struct _ddt * d, unsigned long type)
{
    d->ddt$ps_cancel_2=type;
}

inline void ini_ddt_regdmp(struct _ddt * d, unsigned long type)
{
    d->ddt$ps_regdump_2=type;
}

inline void ini_ddt_erlgbf(struct _ddt * d, unsigned long type)
{
    d->ddt$l_errorbuf=type;
}

inline void ini_ddt_qsrv_helper(struct _ddt * d, unsigned long type)
{
    d->ddt$ps_qsrv_helper=type;
}

inline void ini_ddt_end(struct _ddt * d)
{
    // d->ddt$=type; ??
}

#endif
