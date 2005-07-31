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
#include<linux/vmalloc.h>
#include<linux/pci.h>
#include<system_service_setup.h>
#include<descrip.h>
#include<ttyucbdef.h>
#include<ttyvecdef.h>

#include<linux/blkdev.h>

static struct _irp * globali;
static struct _ucb * globalu;

static void  startio3 (struct _irp * i, struct _ucb * u) { 
  ioc$reqcom(SS$_NORMAL,0,u);
  return;
};

static struct _fdt tt$fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

/* more yet undefined dummies */
int tty$startio (int,int);
static void  unsolint (void) { };
static void  cancel (void) { };
static void  ioc_std$cancelio (void) { };
static void  regdump (void) { };
static void  diagbuf (void) { };
static void  errorbuf (void) { };
static void  unitinit (void) { };
int  tty$wrtstartio (int,int);
static void  mntver (void) { };
static void  cloneducb (void) { };
static void  mntv_sssc (void) { };
static void  mntv_for (void) { };
static void  mntv_sqd (void) { };
static void  aux_storage (void) { };
static void  aux_routine (void) { };

static struct _ddt tt$ddt = {
  ddt$l_start: tty$startio,
  ddt$l_unsolint: unsolint,
  ddt$l_fdt: &tt$fdt,
  ddt$l_cancel: cancel,
  ddt$l_regdump: regdump,
  ddt$l_diagbuf: diagbuf,
  ddt$l_errorbuf: errorbuf,
  ddt$l_unitinit: unitinit,
  ddt$l_altstart: tty$wrtstartio,
  ddt$l_mntver: mntver,
  ddt$l_cloneducb: cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: mntv_sssc,
  ddt$l_mntv_for: mntv_for,
  ddt$l_mntv_sqd: mntv_sqd,
  ddt$l_aux_storage: aux_storage,
  ddt$l_aux_routine: aux_routine
};

  extern tty$getnextchar();
  extern tty$putnextchar();
  extern tty$setup_ucb();
  extern tty$ds_tran();
  extern tty$readerror(); 
  extern tty$class_disconnect();
  extern tty$class_fork();
  extern tty$powerfail();

int class_tables=0; // dummy yet
int tty$tables=0; // dummy yet

struct _tt_class tt$class = {
  class_getnxt: tty$getnextchar,
  class_putnxt: tty$putnextchar,
  class_setup_ucb: tty$setup_ucb,
  class_ds_tran: tty$ds_tran,
  class_ddt: &tt$ddt,
  class_readerror: tty$readerror, 
  class_disconnect: tty$class_disconnect,
  class_fork: tty$class_fork,
  class_powerfail: tty$powerfail,
  class_tables: &tty$tables
};

int tty$fdtread(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int tty$fdtwrite(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int tty$fdtset(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int tty$fdtsensec(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int tty$fdtsensem(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

extern void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type);

void tt$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
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

void tt$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ddb->ddb$ps_ddt=&tt$ddt;
  //dpt_store_isr(crb,nl_isr);
  return;
}

int tt$unit_init (struct _idb * idb, struct _ucb * ucb) {
  ucb->ucb$v_online = 0;
  //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

  // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
  // no adp or cram stuff

  // or ints etc
  
  ucb->ucb$v_online = 1;

  return SS$_NORMAL;
}

struct _dpt tt$dpt;
struct _ddb tt$ddb;
struct _ucb tt$ucb;
struct _crb tt$crb;

int tt$init_tables() {
  ini_dpt_name(&tt$dpt, "TTDRIVER");
  ini_dpt_adapt(&tt$dpt, 0);
  ini_dpt_defunits(&tt$dpt, 1);
  ini_dpt_ucbsize(&tt$dpt,sizeof(struct _ucb));
  ini_dpt_struc_init(&tt$dpt, tt$struc_init);
  ini_dpt_struc_reinit(&tt$dpt, tt$struc_reinit);
  ini_dpt_ucb_crams(&tt$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_vector(&tt$dpt, &tt$class);
  ini_dpt_end(&tt$dpt);

  tty$gl_dpt = &tt$dpt; // place here temporarily?

  ini_ddt_unitinit(&tt$ddt, tt$unit_init);
  ini_ddt_start(&tt$ddt, tty$startio);
  ini_ddt_cancel(&tt$ddt, ioc_std$cancelio);
  ini_ddt_end(&tt$ddt);

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&tt$fdt,IO$_READLBLK,tty$fdtread,1);
  ini_fdt_act(&tt$fdt,IO$_READPBLK,tty$fdtread,1);
  ini_fdt_act(&tt$fdt,IO$_READVBLK,tty$fdtread,1);
  ini_fdt_act(&tt$fdt,IO$_WRITELBLK,tty$fdtwrite,1);
  ini_fdt_act(&tt$fdt,IO$_WRITEPBLK,tty$fdtwrite,1);
  ini_fdt_act(&tt$fdt,IO$_WRITEVBLK,tty$fdtwrite,1);
  ini_fdt_act(&tt$fdt,IO$_SENSECHAR,tty$fdtsensec,1);
  ini_fdt_act(&tt$fdt,IO$_SENSEMODE,tty$fdtsensem,1);
  ini_fdt_act(&tt$fdt,IO$_SETCHAR,tty$fdtset,1);
  ini_fdt_act(&tt$fdt,IO$_SETMODE,tty$fdtset,1);
  ini_fdt_end(&tt$fdt);

  return SS$_NORMAL;
}

int tty_iodb_vmsinit(void) {
#if 0
  struct _ucb * ucb=&tt$ucb;
  struct _ddb * ddb=&tt$ddb;
  struct _crb * crb=&tt$crb;
#endif 
  struct _ucb * ucb=kmalloc(sizeof(struct _ucb),GFP_KERNEL);
  struct _ddb * ddb=kmalloc(sizeof(struct _ddb),GFP_KERNEL);
  struct _crb * crb=kmalloc(sizeof(struct _crb),GFP_KERNEL);
  unsigned long idb=0,orb=0;

  bzero(ucb,sizeof(struct _ucb));
  bzero(ddb,sizeof(struct _ddb));
  bzero(crb,sizeof(struct _crb));

#if 0
  init_ddb(&tt$ddb,&tt$ddt,&tt$ucb,"dqa");
  init_ucb(&tt$ucb, &tt$ddb, &tt$ddt, &tt$crb);
  init_crb(&tt$crb);
#endif

  init_ddb(ddb,&tt$ddt,ucb,"tta");
  init_ucb(ucb, ddb, &tt$ddt, crb);
  init_crb(crb);

//  ioc_std$clone_ucb(&tt$ucb,&ucb);
  tt$init_tables();
  tt$struc_init (crb, ddb, idb, orb, ucb);
  tt$struc_reinit (crb, ddb, idb, orb, ucb);
  tt$unit_init (idb, ucb);

  insertdevlist(ddb);

  return ddb;

}

int tty_iodbunit_vmsinit(struct _ddb * ddb,int unitno,void * dsc) {
  unsigned short int chan;
  struct _ucb * newucb;
  ioc_std$clone_ucb(ddb->ddb$ps_ucb/*&tt$ucb*/,&newucb);
  exe$assign(dsc,&chan,0,0,0);
  registerdevchan(MKDEV(TTYAUX_MAJOR,unitno),chan);


  return newucb;
}

int tty_vmsinit(void) {
  //struct _ucb * u=makeucbetc(&ddb,&ddt,&dpt,&fdt,"hda","hddriver");

  unsigned short chan0, chan1, chan2;
  $DESCRIPTOR(dsc,"opa0");
  unsigned long idb=0,orb=0;
  struct _ccb * ccb;
  struct _ucb * newucb0,*newucb1,*newucb2;
  struct _ddb * ddb;

  printk(KERN_INFO "dev con here pre\n");

  ddb=tty_iodb_vmsinit();

  /* for the fdt init part */
  /* a lot of these? */

  tty_iodbunit_vmsinit(ddb,1,&dsc);

  printk(KERN_INFO "dev con here\n");

  // return chan0;

}
