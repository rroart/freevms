// This is supposed to be a test driver

#include"../../freevms/lib/src/crbdef.h"
#include"../../freevms/lib/src/cdtdef.h"
#include"../../freevms/lib/src/ddtdef.h"
#include"../../freevms/lib/src/dptdef.h"
#include"../../freevms/lib/src/fdtdef.h"
#include"../../freevms/lib/src/pdtdef.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/lib/src/ddbdef.h"
#include"../../freevms/lib/src/ipldef.h"
#include"../../freevms/lib/src/dyndef.h"
#include"../../freevms/starlet/src/ssdef.h"
#include"../../freevms/starlet/src/iodef.h"
#include"../../freevms/starlet/src/devdef.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include"../../freevms/pal/src/ipl.h"
#include<linux/vmalloc.h>

struct _fdt fdt_null = {
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
void  null_functb (void) { };
void  null_cancel (void) { };
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

struct _ddt ddt_null = {
  ddt$l_start: null_startio,
  ddt$l_unsolint: null_unsolint,
  ddt$l_functb: null_functb,
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

/* include a buffered 4th param? */
extern inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long);

//static struct _fdt null_fdt;

struct _dpt null_dpt;

struct _ddb nullddb;
struct _ucb nullucb;
struct _crb nullcrb;
struct _ccb nullccb;

void nl_init(void) {
  struct _ucb * u;
  struct _ddb * d;
  struct _crb * c;

#if 0
  vmalloc does not work this early
  u=vmalloc(sizeof(struct _ucb));
  bzero(u,sizeof(struct _ucb));
  d=vmalloc(sizeof(struct _ddb));
  bzero(d,sizeof(struct _ddb));
  c=vmalloc(sizeof(struct _crb));
  bzero(c,sizeof(struct _crb));
#endif
  c=&nullcrb;
  d=&nullddb;
  u=&nullucb;
  bzero(u,sizeof(struct _ucb));
  bzero(d,sizeof(struct _ddb));
  bzero(c,sizeof(struct _crb));

  insertdevlist(d);

  /* this is a all-in-one, should be split later */

  /* for the dpt part */
  bcopy("NLDRIVER",null_dpt.dpt$t_name,8);
  null_dpt.dpt$ps_ddt=&ddt_null;

  /* for the ddb init part */
  //  d->ddb$ps_link=d;
  //  d->ddb$ps_blink=d;
  d->ddb$b_type=DYN$C_DDB;
  d->ddb$l_ddt=&ddt_null;
  d->ddb$ps_ucb=u;
  bcopy("NLA0",d->ddb$t_name,4);

  /* for the ucb init part */
  qhead_init(&u->ucb$l_ioqfl);
  u->ucb$b_type=DYN$C_UCB;
  u->ucb$b_flck=IPL$_IOLOCK8;
  /* devchars? */
  u->ucb$b_devclass=DEV$M_RND; /* just maybe? */
  u->ucb$b_dipl=IPL$_IOLOCK8;
  //  bcopy("nla0",u->ucb$t_name,4);
  u->ucb$l_ddb=d;
  u->ucb$l_crb=c;
  u->ucb$l_ddt=&ddt_null;

  /* for the crb init part */
  c->crb$b_type=DYN$C_CRB;

  /* and for the ddt init part */
  ddt_null.ddt$l_fdt=&fdt_null;
  ddt_null.ddt$l_functb=&fdt_null;

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&fdt_null,IO$_READLBLK,nl_read,1);
  ini_fdt_act(&fdt_null,IO$_READPBLK,nl_read,1);
  ini_fdt_act(&fdt_null,IO$_READVBLK,nl_read,1);
}

char nulldriverstring[]="NLDRIVER";

struct _dpt null_dpt = {
  //  dpt$t_name:"NLDRIVER"
  //  dpt$b_type:&testme,
  // dpt$t_name:nulldriverstring  // have no idea why this won't compile
  // dpt$ps_ddt:&ddt_null;
};

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



