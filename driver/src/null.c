// $Id$
// $Locker$

// Author. Roar Thronæs.

// This is supposed to be a test driver

#include<crbdef.h>
#include<cdtdef.h>
#include<dcdef.h>
#include<ddtdef.h>
#include<devdef.h>
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

struct _fdt null$fdt = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

struct _irp * globali;
struct _ucb * globalu;

void nl_isr (void) {
  void (*func)(void *,void *);
  struct _irp * i;
  struct _ucb * u;

  if (intr_blocked(20))
    return;
  regtrap(REG_INTR,20);
  setipl(20);
  printk("isr\n");

  /* have to do this until we get things more in order */
  i=globali;
  u=globalu;

  func=u->ucb$l_fpc;
  func(i,u);
  myrei();
}

void  null_startio2 (struct _irp * i, struct _ucb * u);
void  null_startio3 (struct _irp * i, struct _ucb * u);

void  null_startio (struct _irp * i, struct _ucb * u) { 
  static int first=0;
  signed long long step1=-10000000;

  printk("times %x %x\n",u->ucb$b_second_time_in_startio,u->ucb$b_third_time_in_startio);
  //  { int j; for(j=100000000;j;j--);}

#if 0
  if (first) {
    int a=*(int *)0x88888888;
    int b=*(int *)0x82888888;
    int c=*(int *)0x98888888;
    int d=*(int *)0xb8888888;
    insque(0,0);
  }
  first++;
#endif

  printk("firsttime %x %x\n",i,u);
  globali=i;
  globalu=u;

  u->ucb$b_second_time_in_startio=1;
  ioc$wfikpch(null_startio2,0,i,current,u,2,0);
  exe$setimr(0, &step1, nl_isr,0,0);
  return;
}

void  null_startio2 (struct _irp * i, struct _ucb * u) { 
  printk("secondtime\n");

  u->ucb$l_fpc=null_startio3;
  exe$iofork(i,u);
  return;
}

void  null_startio3 (struct _irp * i, struct _ucb * u) { 
  printk("thirdtime %x %x\n",i,u);
  ioc$reqcom(SS$_NORMAL,0,u);
  return;
};

/* more yet undefined dummies */
void  null_unsolint (void) { };
void  null_cancel (void) { };
void  ioc_std$cancelio (void) { };
void  null_regdump (void) { };
void  null_diagbuf (void) { };
void  null_errorbuf (void) { };
void  null_unitinit (void) { };
void  null_altstart (void) { };
void  null_mntver (void) { };
void  null_cloneducb (void) { };
void  null_mntv_sssc (void) { };
void  null_mntv_for (void) { };
void  null_mntv_sqd (void) { };
void  null_aux_storage (void) { };
void  null_aux_routine (void) { };

void nl_read(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  exe$qiodrvpkt(i,p,u);
};

void null_write(void) {

};

struct _ddt null$ddt = {
  ddt$l_start: null_startio,
  ddt$l_unsolint: null_unsolint,
  ddt$l_fdt: &null$fdt,
  ddt$l_cancel: null_cancel,
  ddt$l_regdump: null_regdump,
  ddt$l_diagbuf: null_diagbuf,
  ddt$l_errorbuf: null_errorbuf,
  ddt$l_unitinit: null_unitinit,
  ddt$l_altstart: null_altstart,
  ddt$l_mntver: null_mntver,
  ddt$l_cloneducb: null_cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: null_mntv_sssc,
  ddt$l_mntv_for: null_mntv_for,
  ddt$l_mntv_sqd: null_mntv_sqd,
  ddt$l_aux_storage: null_aux_storage,
  ddt$l_aux_routine: null_aux_routine
};

struct _dpt null$dpt;
struct _ddb null$ddb;
struct _ucb null$ucb;
struct _crb null$crb;

void nl_init(void) {
  struct _ucb * ucb;
  struct _ddb * ddb;
  struct _crb * crb;
  unsigned long idb=0,orb=0;

  crb=&null$crb;
  ddb=&null$ddb;
  //ioc_std$clone_ucb(&null$ucb,&ucb);
  bzero(ddb,sizeof(struct _ddb));
  bzero(crb,sizeof(struct _crb));

  insertdevlist(ddb);

  /* this is a all-in-one, should be split later */

  init_ddb(&null$ddb,&null$ddt,&null$ucb,"nla");
  init_ucb(&null$ucb, &null$ddb, &null$ddt, &null$crb);
  init_crb(&null$crb);

  null$init_tables();
  null$struc_init (crb, ddb, idb, orb, ucb);
  null$struc_reinit (crb, ddb, idb, orb, ucb);
  null$unit_init (idb, ucb);

  ioc_std$clone_ucb(&null$ucb,&ucb);
}

/* include a buffered 4th param? */
extern inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long);

/* move this later */
insertdevlist(struct _ddb *d) {
    d->ddb$ps_link=ioc$gl_devlist;
    ioc$gl_devlist=d;
}

/* just putting this here until I find out where it belong */

inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long type) {
  f->fdt$ps_func_rtn[mask]=fn;
  f->fdt$q_valid|=mask;
  if (type)
    f->fdt$q_buffered|=mask;
}

inline void ini_fdt_end(struct _fdt * fdt) {
  // but what will it do?
}

inline void ini_dpt_name(struct _dpt * d, char * n) {
  bcopy(n,d->dpt$t_name,strlen(n));
}

inline void ini_dpt_adapt(struct _dpt * d, unsigned long type) {
  d->dpt$b_adptype=type;
}

inline void ini_dpt_flags(struct _dpt * d, unsigned long type) {
  d->dpt$l_flags=type;
}


inline void ini_dpt_maxunits(struct _dpt * d, unsigned long type) {
  d->dpt$w_maxunits=type;
}


inline void ini_dpt_ucbsize(struct _dpt * d, unsigned long type) {
  d->dpt$w_ucbsize=type;
}


inline void ini_dpt_struc_init(struct _dpt * d, unsigned long type) {
  d->dpt$ps_init_pd=type;
}


inline void ini_dpt_struc_reinit(struct _dpt * d, unsigned long type) {
  d->dpt$ps_reinit_pd=type;
}


inline void ini_dpt_ucb_crams(struct _dpt * d, unsigned long type) {
  //  d->dpt$iw_ucb_crams=type; not now
}


inline void ini_dpt_defunits(struct _dpt * d, unsigned long type) {
  d->dpt$w_defunits=type;
}


inline void ini_dpt_end(struct _dpt * d) {
  //  d->dpt$_=type; ??
}




inline void ini_ddt_ctrlinit(struct _ddt * d, unsigned long type) {
  d->ddt$ps_ctrlinit_2=type;
}

inline void ini_ddt_unitinit(struct _ddt * d, unsigned long type) {
  d->ddt$l_unitinit=type;
}


inline void ini_ddt_start(struct _ddt * d, unsigned long type) {
  d->ddt$ps_start_2=type;
}


inline void ini_ddt_kp_startio(struct _ddt * d, unsigned long type) {
  d->ddt$ps_kp_startio=type;
}


inline void ini_ddt_kp_stack_size(struct _ddt * d, unsigned long type) {
  d->ddt$is_stack_bcnt=type;
}


inline void ini_ddt_kp_reg_mask(struct _ddt * d, unsigned long type) {
  d->ddt$is_reg_mask=type;
}


inline void ini_ddt_cancel(struct _ddt * d, unsigned long type) {
  d->ddt$ps_cancel_2=type;
}


inline void ini_ddt_regdmp(struct _ddt * d, unsigned long type) {
  d->ddt$ps_regdump_2=type;
}


inline void ini_ddt_erlgbf(struct _ddt * d, unsigned long type) {
  d->ddt$l_errorbuf=type;
}


inline void ini_ddt_qsrv_helper(struct _ddt * d, unsigned long type) {
  d->ddt$ps_qsrv_helper=type;
}


inline void ini_ddt_end(struct _ddt * d) {
  // d->ddt$=type; ??
}

inline void dpt_store_isr(long a, long b) {
}

struct _ucb * makeucbetc(struct _ddb * ddb, struct _ddt * ddt, struct _dpt * dpt, struct _fdt * fdt, char * sddb, char * sdpt) {
  struct _ucb * u=vmalloc(sizeof(struct _ucb));
  struct _crb * c=vmalloc(sizeof(struct _crb));
  struct _idb * idb=vmalloc(sizeof(struct _idb));
  bzero(c,sizeof(struct _crb));
  bzero(ddb,sizeof(struct _ddb));
  bzero(u,sizeof(struct _ucb));
  bzero(idb,sizeof(struct _idb));

  bcopy(sdpt,dpt->dpt$t_name,strlen(sdpt));

  /* for the ddb init part */
  //  d->ddb$ps_link=d;
  //  d->ddb$ps_blink=d;
  ddb->ddb$b_type=DYN$C_DDB;
  ddb->ddb$l_ddt=ddt;
  ddb->ddb$ps_ucb=u;
  bcopy(sddb,ddb->ddb$t_name,strlen(sddb));

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

static unsigned long devchan[256];

registerdevchan(unsigned long dev,unsigned short chan) {
  //  devchan[chan]=MAJOR(dev);
  devchan[chan]=dev;
  printk("registerdevchan dev %x at chan %x\n",devchan[chan],chan);
  //{ int i; for(i=0;i<10000000;i++) ; }
}

unsigned short dev2chan(kdev_t dev) {
  int i;
  //printk("dev2chan %x %x\n", dev, MAJOR(dev));
  for (i=0;i<256;i++)
    if (devchan[i]==dev) return i;
  //    if (devchan[i]==MAJOR(dev)) return i;
 mypanic:
  printk("dev2chan failed\n");
  for (i=0;i<4;i++)
    printk("%x %x\n",devchan[i],dev);
  panic("dev2chan\n");
}





void null$struc_init (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
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

void null$struc_reinit (struct _crb * crb, struct _ddb * ddb, struct _idb * idb, struct _orb * orb, struct _ucb * ucb) {
  ddb->ddb$ps_ddt=&null$ddt;
  dpt_store_isr(crb,nl_isr);
  return;
}

int null$unit_init (struct _idb * idb, struct _ucb * ucb) {
  ucb->ucb$v_online = 0;
  //ucb->ucb$l_lr_msg_tmo = 0 ; // or offline? // where did this go?

  // idb->idb$ps_owner=&(ucb->ucb$r_ucb); // this is mailbox?
  // no adp or cram stuff

  // or ints etc
  
  ucb->ucb$v_online = 1;

  return SS$_NORMAL;
}

struct _dpt null$dpt;
struct _ddt null$ddt;
struct _fdt null$fdt;

int null$init_tables() {
  ini_dpt_name(&null$dpt, "NLDRIVER");
  ini_dpt_adapt(&null$dpt, 0);
  ini_dpt_defunits(&null$dpt, 1);
  ini_dpt_ucbsize(&null$dpt,sizeof(struct _ucb));
  ini_dpt_struc_init(&null$dpt, null$struc_init);
  ini_dpt_struc_reinit(&null$dpt, null$struc_reinit);
  ini_dpt_ucb_crams(&null$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_end(&null$dpt);

  ini_ddt_unitinit(&null$ddt, null$unit_init);
  ini_ddt_start(&null$ddt, null_startio);
  ini_ddt_cancel(&null$ddt, ioc_std$cancelio);
  ini_ddt_end(&null$ddt);

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&null$fdt,IO$_READLBLK,nl_read,1);
  ini_fdt_act(&null$fdt,IO$_READPBLK,nl_read,1);
  ini_fdt_act(&null$fdt,IO$_READVBLK,nl_read,1);
  ini_fdt_end(&null$fdt);

  return SS$_NORMAL;
}
