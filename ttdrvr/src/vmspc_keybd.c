// $Id$
// $Locker$

// Author. Roar Thronæs.
// Modified Linux source file, 2001-2004. Based on pc_keybd.c.

#include<crbdef.h>
#include<cdtdef.h>
#include<dcdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<fdtdef.h>
#include<pdtdef.h>
#include<idbdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ddbdef.h>
#include<ipldef.h>
#include<dyndef.h>
#include<ssdef.h>
#include<iodef.h>
#include<devdef.h>
#include<system_data_cells.h>
#include<ipl.h>
#include<linux/vmalloc.h>
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>

#include<linux/blkdev.h>

static struct _irp * globali;
static struct _ucb * globalu;

static void  startio3 (struct _irp * i, struct _ucb * u) { 
  ioc$reqcom(SS$_NORMAL,0,u);
  return;
};

static void  startio2 (struct _irp * i, struct _ucb * u) { 
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
  myrei();
}

static struct _fdt kb$fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
int kbd$startio (int a,int b ) { }
static void  unsolint (void) { };
static void  cancel (void) { };
static void  ioc_std$cancelio (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
int  kbd$wrtstartio (int a,int b) { }
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

static struct _ddt kb$ddt = {
  ddt$l_start: kbd$startio,
  ddt$l_unsolint: unsolint,
  ddt$l_fdt: &kb$fdt,
  ddt$l_cancel: cancel,
  ddt$l_regdump: regdump,
  ddt$l_diagbuf: diagbuf,
  ddt$l_errorbuf: errorbuf,
  ddt$l_unitinit: unitinit,
  ddt$l_altstart: kbd$wrtstartio,
  ddt$l_mntver: mntver,
  ddt$l_cloneducb: cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: mntv_sssc,
  ddt$l_mntv_for: mntv_for,
  ddt$l_mntv_sqd: mntv_sqd,
  ddt$l_aux_storage: aux_storage,
  ddt$l_aux_routine: aux_routine
};

int kbd$fdtread(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  //  return read(0,i->irp$l_qio_p1,i->irp$l_qio_p2);
}

int kbd$fdtwrite(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  //  return write(1,i->irp$l_qio_p1,i->irp$l_qio_p2);
}

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void kb$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ucb->ucb$b_flck=IPL$_IOLOCK8;
  ucb->ucb$b_dipl=IPL$_IOLOCK8;

  ucb->ucb$l_devchar = DEV$M_REC | DEV$M_AVL | DEV$M_CCL /*| DEV$M_OOV*/;

  ucb->ucb$l_devchar2 = DEV$M_NNM;
  ucb->ucb$b_devclass = DC$_MISC;
  ucb->ucb$b_devtype = DT$_TTYUNKN;
  ucb->ucb$w_devbufsiz = 132;

  ucb->ucb$l_devdepend = 99; // just something to fill

  // dropped the mutex stuff

  return;
}

void kb$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ddb->ddb$ps_ddt=&kb$ddt;
  //dpt_store_isr(crb,nl_isr);
  return;
}

int kb$unit_init (struct _idb * idb, struct _ucb * ucb) {
  ucb->ucb$v_online = 0;
  //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

  // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
  // no adp or cram stuff

  // or ints etc
  
  ucb->ucb$v_online = 1;

  return SS$_NORMAL;
}

struct _dpt kb$dpt;
struct _ddb kb$ddb;
struct _ucb kb$ucb;
struct _crb kb$crb;

int kb$init_tables() {
  ini_dpt_name(&kb$dpt, "OPDRIVER");
  ini_dpt_adapt(&kb$dpt, 0);
  ini_dpt_defunits(&kb$dpt, 1);
  ini_dpt_ucbsize(&kb$dpt,sizeof(struct _ucb));
  ini_dpt_struc_init(&kb$dpt, kb$struc_init);
  ini_dpt_struc_reinit(&kb$dpt, kb$struc_reinit);
  ini_dpt_ucb_crams(&kb$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_end(&kb$dpt);

  ini_ddt_unitinit(&kb$ddt, kb$unit_init);
  ini_ddt_start(&kb$ddt, kbd$startio);
  ini_ddt_cancel(&kb$ddt, ioc_std$cancelio);
  ini_ddt_end(&kb$ddt);

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&kb$fdt,IO$_READLBLK,kbd$fdtread,1);
  ini_fdt_act(&kb$fdt,IO$_READPBLK,kbd$fdtread,1);
  ini_fdt_act(&kb$fdt,IO$_READVBLK,kbd$fdtread,1);
  ini_fdt_act(&kb$fdt,IO$_WRITELBLK,kbd$fdtwrite,1);
  ini_fdt_act(&kb$fdt,IO$_WRITEPBLK,kbd$fdtwrite,1);
  ini_fdt_act(&kb$fdt,IO$_WRITEVBLK,kbd$fdtwrite,1);
  ini_fdt_end(&kb$fdt);

  return SS$_NORMAL;
}

int kbd_iodb_vmsinit(void) {
#if 0
  struct _ucb * ucb=&kb$ucb;
  struct _ddb * ddb=&kb$ddb;
  struct _crb * crb=&kb$crb;
#endif 
  struct _ucb * ucb=kmalloc(sizeof(struct _ucb),GFP_KERNEL);
  struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
  struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
  unsigned long idb=0,orb=0;

  bzero(ucb,sizeof(struct _ucb));
  bzero(ddb,sizeof(struct _ddb));
  bzero(crb,sizeof(struct _crb));

#if 0
  init_ddb(&kb$ddb,&kb$ddt,&kb$ucb,"dqa");
  init_ucb(&kb$ucb, &kb$ddb, &kb$ddt, &kb$crb);
  init_crb(&kb$crb);
#endif

  init_ddb(ddb,&kb$ddt,ucb,"opa");
  init_ucb(ucb, ddb, &kb$ddt, crb);
  init_crb(crb);

//  ioc_std$clone_ucb(&kb$ucb,&ucb);
  kb$init_tables();
  kb$struc_init (crb, ddb, idb, orb, ucb);
  kb$struc_reinit (crb, ddb, idb, orb, ucb);
  kb$unit_init (idb, ucb);

  insertdevlist(ddb);

  return ddb;

}

int kbd_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc) {
  unsigned short int chan;
  struct _ucb * newucb;
  ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&kb$ucb*/,&newucb);
  exe$assign(dsc,&chan,0,0,0);
  registerdevchan(MKDEV(TTYAUX_MAJOR,unitno),chan);


  return newucb;
}

int kbd_vmsinit(void) {
  //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

  unsigned short chan0, chan1, chan2;
  $DESCRIPTOR(dsc,"opa0");
  unsigned long idb=0,orb=0;
  struct _ccb * ccb;
  struct _ucb * newucb0,*newucb1,*newucb2;
  struct _ddb * ddb;

  printk(KERN_INFO "dev con here pre\n");

  ddb=kbd_iodb_vmsinit();

  /* for the fdt init part */
  /* a lot of these? */

  kbd_iodbunit_vmsinit(ddb,1,&dsc);

  printk(KERN_INFO "dev con here\n");

  // return chan0;

}

