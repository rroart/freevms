// $Id$
// $Locker$

// Author. Roar Thronæs.

// This is supposed to be a file driver

#include<crbdef.h>
#include<cdtdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<fdtdef.h>
#include<pdtdef.h>
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

struct _fdt fdt_file = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

struct _irp * fglobali;
struct _ucb * fglobalu;

void fl_isr (void) {
  void (*func)(void *,void *);
  struct _irp * i;
  struct _ucb * u;

  if (intr_blocked(20))
    return;
  regtrap(REG_INTR,20);
  setipl(20);
  printk("isr\n");

  /* have to do this until we get things more in order */
  i=fglobali;
  u=fglobalu;

  func=u->ucb$l_fpc;
  func(i,u);
  myrei();
}

void  file_startio2 (struct _irp * i, struct _ucb * u);
void  file_startio3 (struct _irp * i, struct _ucb * u);

void  file_startio (struct _irp * i, struct _ucb * u) { 
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
  fglobali=i;
  fglobalu=u;

  u->ucb$b_second_time_in_startio=1;
  ioc$wfikpch(file_startio2,0,i,current,u,2,0);
  exe$setimr(0, &step1, fl_isr,0,0);
  return;
}

void  file_startio2 (struct _irp * i, struct _ucb * u) { 
  printk("secondtime\n");

  u->ucb$l_fpc=file_startio3;
  exe$iofork(i,u);
  return;
}

void  file_startio3 (struct _irp * i, struct _ucb * u) { 
  printk("thirdtime %x %x\n",i,u);
  ioc$reqcom(SS$_NORMAL,0,u);
  return;
};

/* more yet undefined dummies */
void  file_unsolint (void) { };
void  file_functb (void) { };
void  file_cancel (void) { };
void  file_regdump (void) { };
void  file_diagbuf (void) { };
void  file_errorbuf (void) { };
void  file_unitinit (void) { };
void  file_altstart (void) { };
void  file_mntver (void) { };
void  file_cloneducb (void) { };
void  file_mntv_sssc (void) { };
void  file_mntv_for (void) { };
void  file_mntv_sqd (void) { };
void  file_aux_storage (void) { };
void  file_aux_routine (void) { };

int acp_std$access(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$modify(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$mount(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$access(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

int acp_std$deaccess(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

void fl_read(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  exe$qiodrvpkt(i,p,u);
};

void file_write(void) {

};

struct _ddt ddt_file = {
  ddt$l_start: file_startio,
  ddt$l_unsolint: file_unsolint,
  ddt$l_functb: file_functb,
  ddt$l_cancel: file_cancel,
  ddt$l_regdump: file_regdump,
  ddt$l_diagbuf: file_diagbuf,
  ddt$l_errorbuf: file_errorbuf,
  ddt$l_unitinit: file_unitinit,
  ddt$l_altstart: file_altstart,
  ddt$l_mntver: file_mntver,
  ddt$l_cloneducb: file_cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: file_mntv_sssc,
  ddt$l_mntv_for: file_mntv_for,
  ddt$l_mntv_sqd: file_mntv_sqd,
  ddt$l_aux_storage: file_aux_storage,
  ddt$l_aux_routine: file_aux_routine
};

/* include a buffered 4th param? */
extern inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn, unsigned long);

//static struct _fdt file_fdt;

struct _dpt file_dpt;

#if 0
struct _ddb nullddb;
struct _ucb nullucb;
struct _crb nullcrb;
struct _ccb nullccb;
#endif

int acp_std$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);
int acp_std$writeblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c);

struct _ucb * fl_init(char * s) {
  struct _ucb * u;
  struct _ddb * d;
  struct _crb * c;

  u=vmalloc(sizeof(struct _ucb));
  bzero(u,sizeof(struct _ucb));
  d=vmalloc(sizeof(struct _ddb));
  bzero(d,sizeof(struct _ddb));
  c=vmalloc(sizeof(struct _crb));

  bzero(c,sizeof(struct _crb));
  bzero(u,sizeof(struct _ucb));
  bzero(d,sizeof(struct _ddb));
  bzero(c,sizeof(struct _crb));

  insertdevlist(d);
  insertfillist(u);

  /* this is a all-in-one, should be split later */

  /* for the dpt part */
  bcopy("FLDRIVER",file_dpt.dpt$t_name,8);
  file_dpt.dpt$ps_ddt=&ddt_file;

  /* for the ddb init part */
  //  d->ddb$ps_link=d;
  //  d->ddb$ps_blink=d;
  d->ddb$b_type=DYN$C_DDB;
  d->ddb$l_ddt=&ddt_file;
  d->ddb$ps_ucb=u;
  bcopy(s,d->ddb$t_name,strlen(s));

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
  u->ucb$l_ddt=&ddt_file;

  /* for the crb init part */
  c->crb$b_type=DYN$C_CRB;

  /* and for the ddt init part */
  ddt_file.ddt$l_fdt=&fdt_file;
  ddt_file.ddt$l_functb=&fdt_file;

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&fdt_file,IO$_READLBLK,acp_std$readblk,1);
  ini_fdt_act(&fdt_file,IO$_READPBLK,acp_std$readblk,1);
  ini_fdt_act(&fdt_file,IO$_READVBLK,acp_std$readblk,1);
  ini_fdt_act(&fdt_file,IO$_WRITELBLK,acp_std$writeblk,1);
  ini_fdt_act(&fdt_file,IO$_WRITEPBLK,acp_std$writeblk,1);
  ini_fdt_act(&fdt_file,IO$_WRITEVBLK,acp_std$writeblk,1);
  ini_fdt_act(&fdt_file,IO$_ACCESS,acp_std$access,1);
  ini_fdt_act(&fdt_file,IO$_CREATE,acp_std$access,1);
  ini_fdt_act(&fdt_file,IO$_DEACCESS,acp_std$deaccess,1);
  ini_fdt_act(&fdt_file,IO$_DELETE,acp_std$modify,1);
  ini_fdt_act(&fdt_file,IO$_MODIFY,acp_std$modify,1);
  ini_fdt_act(&fdt_file,IO$_ACPCONTROL,acp_std$modify,1);
  ini_fdt_act(&fdt_file,IO$_MOUNT,acp_std$mount,1);
  return u;
}

char filedriverstring[]="FLDRIVER";

struct _dpt file_dpt = {
  //  dpt$t_name:"FLDRIVER"
  //  dpt$b_type:&testme,
  // dpt$t_name:filedriverstring  // have no idea why this won't compile
  // dpt$ps_ddt:&ddt_file;
};

struct _ucb * myfilelist[50];
int myfilelistptr=0;

insertfillist(struct _ucb *u) {
  myfilelist[myfilelistptr++]=u;
}

#if 0
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



#endif
