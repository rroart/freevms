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
#include"../../freevms/starlet/src/iodef.h"
#include"../../freevms/starlet/src/devdef.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include<linux/vmalloc.h>

struct _fdt fdt_null = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

void  null_startio (struct _irp * i, struct _ucb * u) { 
  //wfikpch

  //iofork

  //reqcom
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

void nl_read(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int funcno, void * fdt, void * p1, long p2, long p3, long p4, long p5, long p6) {
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
extern inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn);

//static struct _fdt null_fdt;

void acp$readblk();
void acp$writeblk();

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
  c=&nullccb;
  d=&nullddb;
  u=&nullucb;
  bzero(u,sizeof(struct _ucb));
  bzero(u,sizeof(struct _ddb));
  bzero(u,sizeof(struct _crb));

  ioc$gl_devlist=d; /* one-shot */

  /* this is a all-in-one, should be split later */

  /* for the dpt part */
  bcopy("NLDRIVER",null_dpt.dpt$t_name,8);
  null_dpt.dpt$ps_ddt=&ddt_null;

  /* for the ddb init part */
  d->ddb$ps_link=d;
  d->ddb$ps_blink=d;
  d->ddb$b_type=DYN$C_DDB;
  d->ddb$l_ddt=&ddt_null;
  d->ddb$ps_ucb=u;
  bcopy("NLA0",d->ddb$t_name,4);

  /* for the ucb init part */
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
  ini_fdt_act(&fdt_null,IO$_READLBLK,nl_read);
  ini_fdt_act(&fdt_null,IO$_READPBLK,nl_read);
  ini_fdt_act(&fdt_null,IO$_READVBLK,nl_read);
}

char nulldriverstring[]="NLDRIVER";

struct _dpt null_dpt = {
  //  dpt$t_name:"NLDRIVER"
  //  dpt$b_type:&testme,
  // dpt$t_name:nulldriverstring  // have no idea why this won't compile
  // dpt$ps_ddt:&ddt_null;
};

