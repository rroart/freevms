// $Id$
// $Locker$

// Author. Roar Thronæs.

// This is supposed to be a test driver

#include"../../freevms/lib/src/cdtdef.h"
#include"../../freevms/lib/src/ddtdef.h"
#include"../../freevms/lib/src/dptdef.h"
#include"../../freevms/lib/src/fdtdef.h"
#include"../../freevms/lib/src/pdtdef.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/lib/src/ipldef.h"
#include"../../freevms/lib/src/dyndef.h"
#include"../../freevms/starlet/src/iodef.h"
#include"../../freevms/starlet/src/devdef.h"
#include<linux/vmalloc.h>

struct _fdt fdt_rnd = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

void  rnd_startio (struct _irp * i, struct _ucb * u) { 
  if (u->ucb$l_devchar2&DEV$M_CDP) {
    /* not local? */
    struct _cdrp * c;
    c=(struct _cdrp *) i->irp$l_fqfl;
    c->cdrp$l_cdt=((struct _mscp_ucb *)u)->ucb$l_cdt;
    
  } else {
    /* local */
    /* error if it gets here, got no real mscp */
    panic("no real mscp\n");
  }
};

/* more yet undefined dummies */
void  rnd_unsolint (void) { };
void  rnd_functb (void) { };
void  rnd_cancel (void) { };
void  rnd_regdump (void) { };
void  rnd_diagbuf (void) { };
void  rnd_errorbuf (void) { };
void  rnd_unitinit (void) { };
void  rnd_altstart (void) { };
void  rnd_mntver (void) { };
void  rnd_cloneducb (void) { };
void  rnd_mntv_sssc (void) { };
void  rnd_mntv_for (void) { };
void  rnd_mntv_sqd (void) { };
void  rnd_aux_storage (void) { };
void  rnd_aux_routine (void) { };


struct _ddt ddt_rnd = {
  ddt$l_start: rnd_startio,
  ddt$l_unsolint: rnd_unsolint,
  ddt$l_functb: rnd_functb,
  ddt$l_cancel: rnd_cancel,
  ddt$l_regdump: rnd_regdump,
  ddt$l_diagbuf: rnd_diagbuf,
  ddt$l_errorbuf: rnd_errorbuf,
  ddt$l_unitinit: rnd_unitinit,
  ddt$l_altstart: rnd_altstart,
  ddt$l_mntver: rnd_mntver,
  ddt$l_cloneducb: rnd_cloneducb,
  ddt$w_fdtsize: 0,
  ddt$l_mntv_sssc: rnd_mntv_sssc,
  ddt$l_mntv_for: rnd_mntv_for,
  ddt$l_mntv_sqd: rnd_mntv_sqd,
  ddt$l_aux_storage: rnd_aux_storage,
  ddt$l_aux_routine: rnd_aux_routine
};

/* include a buffered 4th param? */
extern inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn);

static struct _fdt rnd_fdt;

void acp_std$readblk();
void acp_std$writeblk();

struct _dpt rnd_dpt;

struct _ucb randomucb;

void rnd_init(void) {
  struct _ucb * u;
  //  u=vmalloc(sizeof(struct _ucb));
  u=&randomucb;
  bzero(u,sizeof(struct _ucb));
  u->ucb$b_type=DYN$C_UCB;
  u->ucb$b_flck=IPL$_IOLOCK8;
  /* devchars? */
  u->ucb$b_devclass=DEV$M_RND; /* just maybe? */
  u->ucb$b_dipl=IPL$_IOLOCK8;

  //  rnd_dpt.dpt$t_name="RNDRIVER";
  
  /* a lot of these? */
  ini_fdt_act(&rnd_fdt,IO$_READLBLK,acp_std$readblk);
  ini_fdt_act(&rnd_fdt,IO$_READPBLK,acp_std$readblk);
  ini_fdt_act(&rnd_fdt,IO$_READVBLK,acp_std$readblk);
}

char rnddriverstring[]="RNDDRIVER";

struct _dpt rnd_dpt = {
  //  dpt$t_name:"RNDDRIVER"
  //  dpt$b_type:&testme,
  // dpt$t_name:rnddriverstring  // have no idea why this won't compile
};

