// $Id$
// $Locker$

// Author. Roar Thronæs.

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
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>
#include<ftucbdef.h>
#include<ftrddef.h>

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

static struct _fdt ft$fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
int ft$startio (int a,int b) { };
static void  unsolint (void) { };
static void  cancel (void) { };
static void  ioc_std$cancelio (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
int  ft$wrtstartio (int a,int b) { };
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

static struct _ddt ft$ddt = {
  ddt$l_start: ft$startio,
  ddt$l_unsolint: unsolint,
  ddt$l_fdt: &ft$fdt,
  ddt$l_cancel: cancel,
  ddt$l_regdump: regdump,
  ddt$l_diagbuf: diagbuf,
  ddt$l_errorbuf: errorbuf,
  ddt$l_unitinit: unitinit,
  ddt$l_altstart: ft$wrtstartio,
  ddt$l_mntver: mntver,
  ddt$l_cloneducb: cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: mntv_sssc,
  ddt$l_mntv_for: mntv_for,
  ddt$l_mntv_sqd: mntv_sqd,
  ddt$l_aux_storage: aux_storage,
  ddt$l_aux_routine: aux_routine
};

int ft$fdtread(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int ft$fdtwrite(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void ft$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
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

void ft$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ddb->ddb$ps_ddt=&ft$ddt;
  //dpt_store_isr(crb,nl_isr);
  return;
}

int ft$unit_init (struct _idb * idb, struct _ucb * ucb) {
  ucb->ucb$v_online = 0;
  //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

  // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
  // no adp or cram stuff

  // or ints etc
  
  ucb->ucb$v_online = 1;

  return SS$_NORMAL;
}

struct _dpt ft$dpt;
struct _ddb ft$ddb;
struct _ucb ft$ucb;
struct _crb ft$crb;

int ft$init_tables() {
  ini_dpt_name(&ft$dpt, "FTDRIVER");
  ini_dpt_adapt(&ft$dpt, 0);
  ini_dpt_defunits(&ft$dpt, 1);
  ini_dpt_ucbsize(&ft$dpt,sizeof(struct _ft_ucb));
  ini_dpt_struc_init(&ft$dpt, ft$struc_init);
  ini_dpt_struc_reinit(&ft$dpt, ft$struc_reinit);
  ini_dpt_ucb_crams(&ft$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_end(&ft$dpt);

  ini_ddt_unitinit(&ft$ddt, ft$unit_init);
  ini_ddt_start(&ft$ddt, ft$startio);
  ini_ddt_cancel(&ft$ddt, ioc_std$cancelio);
  ini_ddt_end(&ft$ddt);

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&ft$fdt,IO$_READLBLK,ft$fdtread,1);
  ini_fdt_act(&ft$fdt,IO$_READPBLK,ft$fdtread,1);
  ini_fdt_act(&ft$fdt,IO$_READVBLK,ft$fdtread,1);
  ini_fdt_act(&ft$fdt,IO$_WRITELBLK,ft$fdtwrite,1);
  ini_fdt_act(&ft$fdt,IO$_WRITEPBLK,ft$fdtwrite,1);
  ini_fdt_act(&ft$fdt,IO$_WRITEVBLK,ft$fdtwrite,1);
  ini_fdt_end(&ft$fdt);

  return SS$_NORMAL;
}

int ft_iodb_vmsinit(void) {
#if 0
  struct _ucb * ucb=&ft$ucb;
  struct _ddb * ddb=&ft$ddb;
  struct _crb * crb=&ft$crb;
#endif 
  struct _ucb * ucb=kmalloc(sizeof(struct _ft_ucb),GFP_KERNEL);
  struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
  struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
  unsigned long idb=0,orb=0;

  bzero(ucb,sizeof(struct _ft_ucb));
  bzero(ddb,sizeof(struct _ddb));
  bzero(crb,sizeof(struct _crb));

#if 0
  init_ddb(&ft$ddb,&ft$ddt,&ft$ucb,"dqa");
  init_ucb(&ft$ucb, &ft$ddb, &ft$ddt, &ft$crb);
  init_crb(&ft$crb);
#endif

  ucb -> ucb$w_size = sizeof(struct _ft_ucb); // temp placed

  init_ddb(ddb,&ft$ddt,ucb,"fta");
  init_ucb(ucb, ddb, &ft$ddt, crb);
  init_crb(crb);

//  ioc_std$clone_ucb(&ft$ucb,&ucb);
  ft$init_tables();
  ft$struc_init (crb, ddb, idb, orb, ucb);
  ft$struc_reinit (crb, ddb, idb, orb, ucb);
  ft$unit_init (idb, ucb);

  insertdevlist(ddb);

  return ddb;

}

int ft_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc) {
  unsigned short int chan;
  struct _ucb * newucb;
  ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&ft$ucb*/,&newucb);
  exe$assign(dsc,&chan,0,0,0);
  registerdevchan(MKDEV(TTYAUX_MAJOR,unitno),chan);


  return newucb;
}

int ft_vmsinit(void) {
  //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

  unsigned short chan0, chan1, chan2;
  $DESCRIPTOR(dsc,"opa0");
  unsigned long idb=0,orb=0;
  struct _ccb * ccb;
  struct _ucb * newucb0,*newucb1,*newucb2;
  struct _ddb * ddb;

  printk(KERN_INFO "dev con here pre\n");

  ddb=ft_iodb_vmsinit();

  /* for the fdt init part */
  /* a lot of these? */

  ft_iodbunit_vmsinit(ddb,1,&dsc);

  printk(KERN_INFO "dev con here\n");

  // return chan0;

}

int ft$fdtread(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  printk("should not be in ft$read\n");
}

int ft$fdtwrite(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  printk("should not be in ft$write\n");
}
