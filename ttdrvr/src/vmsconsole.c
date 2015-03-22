// $Id$
// $Locker$

// Author. Roar Thron�s.
// Modified Linux source file, 2001-2004. Based on console.c.
// Have also used some CMU stuff

//
//  ****************************************************************
//
//      Copyright (c) 1992, Carnegie Mellon University
//
//      All Rights Reserved
//
//  Permission  is  hereby  granted   to  use,  copy,  modify,  and
//  distribute  this software  provided  that the  above  copyright
//  notice appears in  all copies and that  any distribution be for
//  noncommercial purposes.
//
//  Carnegie Mellon University disclaims all warranties with regard
//  to this software.  In no event shall Carnegie Mellon University
//  be liable for  any special, indirect,  or consequential damages
//  or any damages whatsoever  resulting from loss of use, data, or
//  profits  arising  out of  or in  connection  with  the  use  or
//  performance of this software.
//
//  ****************************************************************
//

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
#include <linux/vmalloc.h>
#include <linux/pci.h>
#include <system_service_setup.h>
#include <descrip.h>
#include <ttyucbdef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <ttyvecdef.h>
#include <ttydef.h>
#include <ttyrbdef.h>
#include <ioc_routines.h>
#include <exe_routines.h>
#include <com_routines.h>
#include <misc_routines.h>

#include <linux/blkdev.h>
#include <linux/console.h>

extern struct tty_driver console_driver;

void con$startio(int R3, struct _ucb * u, signed int CC)                // START I/O ON UNIT
{
    struct _ucb * ucb = u;
    struct _tty_ucb * tty=ucb;
    if (CC==1)              // Single character
    {
        tty->tty$b_tank_char = R3;      // Save output character
        tty->ucb$w_tt_hold|=TTY$M_TANK_HOLD;    // Signal charater in tank
    }
    else
    {
        tty->ucb$w_tt_hold|=TTY$M_TANK_BURST;   // Signal burst active
    }

    //
    // Here we must do something to notify our mate device that
    // there is data to pick up
    //
    // not?  ucb = ((struct _tz_ucb *)ucb)->ucb$l_tz_xucb;      // Switch to PZ UCB
    if (ucb)            // PZ is disconnected: skip
    {
#if 0
        int savipl = forklock(ucb->ucb$b_flck, ucb->ucb$b_flck);    // Take out PZ device FORK LOCK
#endif
#if 0
        if (ucb->ucb$l_irp->irp$l_func==IO$_WRITEPBLK)
        {
#else
        if (tty->tty$v_st_write)
        {
#endif
#if 0
            kfree(ucb->ucb$l_irp->irp$l_svapte);
            ucb->ucb$l_irp->irp$l_svapte=0;
            ucb->ucb$l_svapte=0;
            ucb->ucb$l_sts&=~UCB$M_BSY; // reset this somewhere?
#endif
            tty->tty$v_st_write=0;
            return con$fdtwrite(ucb->ucb$l_irp,ctl$gl_pcb,ucb,0);
            // return ioc$reqcom(SS$_NORMAL,0,u); // not needed
        }
        else
        {
#if 0
            console_driver.put_char(ucb, R3);
            con_put_char_alt(ucb, R3);
            con$fdtwrite(ucb->ucb$l_irp,ctl$gl_pcb,ucb,0);
#endif
            struct _tt_readbuf * bd = u->ucb$l_svapte;
            int count = 1;
            char * buf = &R3;
            char stkbuf[64];

            if (count==1)
            {
                switch (R3)
                {
                case 4:
                    count=3;
                    buf="\33[D";
                    break;
                case 5:
                    buf="\33[11C";
                    sprintf(stkbuf,"\33[%dC",bd->tty$b_rb_rvffil);
                    buf=stkbuf;
                    count=strlen(buf);
                    if (bd->tty$b_rb_rvffil==0)
                        count=0;
                    break;
                case 6:
                    count=3;
                    buf="\33[C";
                    break;
                case 8:
#if 0
                    buf="\33[G";
#else
                    sprintf(stkbuf,"\33[%dD",bd->tty$b_rb_rvffil);
                    buf=stkbuf;
#endif
                    count=strlen(buf);
                    if (bd->tty$b_rb_rvffil==0)
                        count=0;
                    break;
                case 127:
                    count=4;
                    buf="\x8\33[P";
                    break;
                }
            }

#if 0
            for (; count; count--,buf++)
                myout(ucb,buf,1);
#else
            if (count)
                myout(ucb, buf, count);
#endif
            return SS$_NORMAL;
#if 0
            u->ucb$l_svapte=u->ucb$l_irp->irp$l_svapte; // workaround
            int cc;
            int ch;
again:
            tty$getnextchar(&ch,&cc,u); // do this as con_ucb->tt_class->getnxt
            if (cc==1)
                goto again;
            return ioc$reqcom(SS$_NORMAL|(1<<16),0,u);
#endif
        }
#if 0
        if  (UCB$M_BSY&     // If the device isn't busy,
                ucb->ucb$l_sts)     // then dont do i/o

            ioc$initiate(ucb->ucb$l_irp, ucb);      // IOC$INITIATE needs IRP addr
#endif
#if 0
        forkunlock(ucb->ucb$b_flck, savipl);            // Release PZ drvice FORK LOCK
#endif

        return;
    }
    else
    {
        //
        // Come here if we have no PZ control device to send stuff to.  Just
        // suck all the data we can out of the class driver and throw it away.
        //
        ucb=u;
        tty=ucb;
        tty->ucb$w_tt_hold&= ~( TTY$M_TANK_HOLD|    // Nothing in progress now
                                TTY$M_TANK_BURST|
                                TTY$M_TANK_PREMPT);
    }
    do
    {
        ucb->ucb$l_sts&=~UCB$M_INT;
        tty=ucb;
        long chr, cc;
        tty->ucb$l_tt_getnxt(&chr,&cc,ucb);
    }
    while (tty->ucb$b_tt_outype);

    return;

}

static struct _fdt op$fdt =
{
fdt$q_valid:
    IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
fdt$q_buffered:
    IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

void con$null(void) { }

struct _tt_port con_port_vector =
{
    //
    // Added port vector table using VEC macros
    //
    //      $VECINI TZ:con$null
port_startio:
    con$startio,
port_disconnect:
    con$null,
port_set_line:
    con$null,
port_ds_set:
    con$null,
port_xon:
    con$null,
port_xoff:
    con$null,
port_stop:
    con$null,
port_stop2:
    con$null,
port_abort:
    con$null,
port_resume:
    con$null,
port_set_modem:
    con$null,
port_glyphload:
    con$null,
port_maint:
    con$null,
port_forkret:
    con$null,
port_start_read:
    con$null,
port_middle_read:
    con$null,
port_end_read:
    con$null,
port_cancel:
    con$null,
};

/* more yet undefined dummies */
static void  unsolint (void) { };
static void  cancel (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
int  con$wrtstartio (int a,int b) { }
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

static struct _ddt op$ddt =
{
    ddt$l_start: 0,
    ddt$l_fdt: 0,
#if 0
ddt$l_start:
    con$startio,
ddt$l_unsolint:
    unsolint,
ddt$l_fdt:
    &op$fdt,
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
    con$wrtstartio,
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
#endif
};

#if 0
static char *vidmem = (char *)0xb8000;
static int vidport=0x3d4;
static int lines=25, mycols=80, orig_x=1, orig_y=24;

static void scroll(void)
{
    int i;

    memcpy ( vidmem, vidmem + mycols * 2, ( lines - 1 ) * mycols * 2 );
    for ( i = ( lines - 1 ) * mycols * 2; i < lines * mycols * 2; i += 2 )
        vidmem[i] = ' ';
}

static void puts(const char *s)
{
    int x,y,pos;
    char c;

    x=orig_x;
    y=orig_y;

    while ( ( c = *s++ ) != '\0' )
    {
        if ( c == '\n' )
        {
            x = 0;
            if ( ++y >= lines )
            {
                scroll();
                y--;
            }
        }
        else
        {
            vidmem [ ( x + mycols * y ) * 2 ] = c;
            if ( ++x >= mycols )
            {
                x = 0;
                if ( ++y >= lines )
                {
                    scroll();
                    y--;
                }
            }
        }
    }

    orig_x = x;
    orig_y = y;

    pos = (x + mycols * y) * 2;       /* Update cursor position */
    outb_p(14, vidport);
    outb_p(0xff & (pos >> 9), vidport+1);
    outb_p(15, vidport);
    outb_p(0xff & (pos >> 1), vidport+1);
}
#endif

long pidtab[1024];
long pididx=0;

int con$fdtread(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    //  return read(0,i->irp$l_qio_p1,i->irp$l_qio_p2);
    return kbd$fdtread(i,p,u,c);
}

int con$fdtwrite(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c)
{
    //  return write(1,i->irp$l_qio_p1,i->irp$l_qio_p2);
    //puts("test\n");
    //puts(i->irp$l_qio_p1);
    struct _tty_ucb * tty = u;

    //  init_dev2(chan2dev(i->irp$w_chan ,&tty));
    //printk("dev %x %x\n",chan2dev(i->irp$w_chan),i->irp$w_chan);
    //init_dev2(0x0401,&tty);
    console_driver.write(i,p,u,c);
#if 1
    // this must be put back sometime
#if 0
    u->ucb$l_irp=i;
    u->ucb$l_irp->irp$l_iost1 = SS$_NORMAL;
#else
    tty->ucb$l_tt_wrtbuf->tty$l_wb_irp->irp$l_iost1 = SS$_NORMAL;
    i=tty->ucb$l_tt_wrtbuf->tty$l_wb_irp;
    if (i->irp$l_func!=IO$_WRITEPBLK && i->irp$l_func!=IO$_WRITEVBLK)
        printk("XXX %x\n",i->irp$l_func);
#if 0
    i->irp$l_svapte=0;
#endif
#endif
#if 0
    com$post(u->ucb$l_irp,u);
#else
    com$post(tty->ucb$l_tt_wrtbuf->tty$l_wb_irp,u);
#endif
#endif
    return SS$_NORMAL;
}

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void op$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb)
{
    ucb->ucb$b_flck=IPL$_IOLOCK8;
    ucb->ucb$b_dipl=IPL$_IOLOCK8;

    ucb->ucb$l_devchar = DEV$M_REC | DEV$M_IDV | DEV$M_ODV | DEV$M_TRM | DEV$M_AVL | DEV$M_CCL /*| DEV$M_OOV*/;

    ucb->ucb$l_devchar2 = DEV$M_NNM | TT2$M_LOCALECHO;
    ucb->ucb$b_devclass = DC$_TERM;
    ucb->ucb$b_devtype = DT$_TTYUNKN;
    ucb->ucb$w_devbufsiz = 80;

    ucb->ucb$l_devdepend = 24; // just something to fill

    // dropped the mutex stuff

    return;
}

void op$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb)
{
    ddb->ddb$ps_ddt=&op$ddt;
    //dpt_store_isr(crb,nl_isr);
    return;
}

int op$unit_init (struct _idb * idb, struct _ucb * ucb)
{
    ucb->ucb$v_online = 0;
    //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

    // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
    // no adp or cram stuff

    // or ints etc

    ucb->ucb$v_online = 1;

    int R0=&con_port_vector; // check TZ$VEC?       // Set TZ port vector table
    CLASS_UNIT_INIT(ucb,R0);
    struct _tty_ucb * tty;
    struct _tt_class * R2;
    tty = ucb;
    R2   = tty->ucb$l_tt_class;     // Address class vector table
    R2->class_setup_ucb(tty);       // Init ucb fields

    return SS$_NORMAL;
}

struct _dpt op$dpt;
struct _ddb op$ddb;
struct _ucb op$ucb;
struct _crb op$crb;

int op$init_tables()
{
    ini_dpt_name(&op$dpt, "OPDRIVER");
    ini_dpt_adapt(&op$dpt, 0);
    ini_dpt_defunits(&op$dpt, 1);
    ini_dpt_ucbsize(&op$dpt,sizeof(struct _ucb));
    ini_dpt_struc_init(&op$dpt, op$struc_init);
    ini_dpt_struc_reinit(&op$dpt, op$struc_reinit);
    ini_dpt_ucb_crams(&op$dpt, 1/*NUMBER_CRAMS*/);
    ini_dpt_vector(&op$dpt, &con_port_vector);
    ini_dpt_end(&op$dpt);

    // check if ctrlinit needed
    ini_ddt_unitinit(&op$ddt, op$unit_init);
    ini_ddt_start(&op$ddt, con$startio); // check should be 0?
    ini_ddt_cancel(&op$ddt, ioc_std$cancelio);
    ini_ddt_end(&op$ddt);

    /* for the fdt init part */
    /* a lot of these? */
    ini_fdt_act(&op$fdt,IO$_READLBLK,con$fdtread,1);
    ini_fdt_act(&op$fdt,IO$_READPBLK,con$fdtread,1);
    ini_fdt_act(&op$fdt,IO$_READVBLK,con$fdtread,1);
    ini_fdt_act(&op$fdt,IO$_WRITELBLK,con$fdtwrite,1);
    ini_fdt_act(&op$fdt,IO$_WRITEPBLK,con$fdtwrite,1);
    ini_fdt_act(&op$fdt,IO$_WRITEVBLK,con$fdtwrite,1);
    ini_fdt_end(&op$fdt);

    return SS$_NORMAL;
}

long con_iodb_vmsinit(void)
{
#if 0
    struct _ucb * ucb=&op$ucb;
    struct _ddb * ddb=&op$ddb;
    struct _crb * crb=&op$crb;
#endif
    struct _ucb * ucb=kmalloc(sizeof(struct _tty_ucb),GFP_KERNEL);
    struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
    struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
    unsigned long idb=0,orb=0;

    memset(ucb,0,sizeof(struct _tty_ucb)); // check
    memset(ddb,0,sizeof(struct _ddb));
    memset(crb,0,sizeof(struct _crb));

    ucb -> ucb$w_size = sizeof(struct _tty_ucb); // temp placed // check

#if 0
    // not in this one?
    ucb -> ucb$w_unit_seed = 1; // was: 0 // check // temp placed
    ucb -> ucb$w_unit = 0; // temp placed
#endif

    ucb -> ucb$l_sts |= UCB$M_TEMPLATE; // temp placed

#if 0
    init_ddb(&op$ddb,&op$ddt,&op$ucb,"dqa");
    init_ucb(&op$ucb, &op$ddb, &op$ddt, &op$crb);
    init_crb(&op$crb);
#endif

    init_ddb(ddb,&op$ddt,ucb,"opa");
    init_ucb(ucb, ddb, &op$ddt, crb);
    init_crb(crb);

//  ioc_std$clone_ucb(&op$ucb,&ucb);
    op$init_tables();
    op$struc_init (crb, ddb, idb, orb, ucb);
    op$struc_reinit (crb, ddb, idb, orb, ucb);
    // not yet?  op$unit_init (idb, ucb);

    insertdevlist(ddb);

    //opa$ar_ucb0=ucb;

    return ddb;

}

int con_ucb = 0;

long con_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc)
{
    unsigned short int chan;
    struct _ucb * newucb;
    ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&op$ucb*/,&newucb); // check. skip?
    op$unit_init (0, newucb); // check. moved here
    exe$assign(dsc,&chan,0,0,0);
#if 0
    registerucbchan(newucb,chan);
#endif

    con_ucb = newucb;
    opa$ar_ucb0 = newucb;

    return newucb;
}

int con_vmsinit(void)
{
    //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

    unsigned short chan0, chan1, chan2;
    $DESCRIPTOR(dsc,"opa0");
    unsigned long idb=0,orb=0;
    struct _ccb * ccb;
    struct _ucb * newucb0,*newucb1,*newucb2;
    struct _ddb * ddb;

    printk(KERN_INFO "dev con here pre\n");

    ddb=con_iodb_vmsinit();

    /* for the fdt init part */
    /* a lot of these? */

    con_iodbunit_vmsinit(ddb,0,&dsc);

    printk(KERN_INFO "dev con here\n");

    // return chan0;

}

unsigned int video_font_height;
unsigned int default_font_height;
unsigned int video_scan_lines;
int sel_cons = 0;       /* must not be disallocated */

void clear_selection(void) { }

struct vt_struct *vt_cons[MAX_NR_CONSOLES];

int (*kbd_rate)(struct kbd_repeat *rep);
