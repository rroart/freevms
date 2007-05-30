// $Id$
// $Locker$

// Author. Roar Thronæs.

// This is supposed to be a test driver

#include<cdtdef.h>
#include<ddtdef.h>
#include<dptdef.h>
#include<fdtdef.h>
#include<pdtdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ipldef.h>
#include<dyndef.h>
#include<iodef.h>
#include<devdef.h>
#include <ioc_routines.h>
#include <misc_routines.h>
#include <ssdef.h>
#include <linux/string.h>

struct _fdt fdt_rnd = {
  fdt$q_valid:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_WRITECHECK|IO$_READPBLK|IO$_WRITELBLK|IO$_DSE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_READRCT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_SETPRFPATH|IO$_FORMAT,
  fdt$q_buffered:IO$_NOP|IO$_UNLOAD|IO$_AVAILABLE|IO$_PACKACK|IO$_DSE|IO$_SENSECHAR|IO$_SETCHAR|IO$_SENSEMODE|IO$_SETMODE|IO$_ACCESS|IO$_ACPCONTROL|IO$_CREATE|IO$_DEACCESS|IO$_DELETE|IO$_MODIFY|IO$_MOUNT|IO$_CRESHAD|IO$_ADDSHAD|IO$_COPYSHAD|IO$_REMSHAD|IO$_SHADMV|IO$_DISPLAY|IO$_FORMAT
};

void  rnd_startio (struct _irp * i, struct _ucb * u) { 
};

/* more yet undefined dummies */
void  rnd_unsolint (void) { };
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
  ddt$l_fdt: &fdt_rnd,
  ddt$l_cancel: rnd_cancel,
  ddt$l_regdump: rnd_regdump,
  ddt$l_diagbuf: rnd_diagbuf,
  ddt$l_errorbuf: rnd_errorbuf,
  ddt$l_unitinit: rnd_unitinit,
  ddt$l_altstart: rnd_altstart,
  ddt$l_mntver: rnd_mntver,
  ddt$l_cloneducb: rnd_cloneducb,
  ddt$w_fdtsize: 0,
  ddt$ps_mntv_sssc: rnd_mntv_sssc,
  ddt$ps_mntv_for: rnd_mntv_for,
  ddt$ps_mntv_sqd: rnd_mntv_sqd,
  ddt$ps_aux_storage: rnd_aux_storage,
  ddt$ps_aux_routine: rnd_aux_routine
};

/* include a buffered 4th param? */
//extern inline void ini_fdt_act(struct _fdt * f, unsigned long long mask, void * fn);

static struct _fdt rnd_fdt;

void acp_std$readblk();
void acp_std$writeblk();

struct _dpt rnd_dpt;

struct _ucb randomucb;

void rnd_init(void) {
  struct _ucb * u;
  //  u=vmalloc(sizeof(struct _ucb));
  u=&randomucb;
  memset(u,0,sizeof(struct _ucb));
  u->ucb$b_type=DYN$C_UCB;
  u->ucb$b_flck=IPL$_IOLOCK8;
  /* devchars? */
  u->ucb$b_devclass=DEV$M_RND; /* just maybe? */
  u->ucb$b_dipl=IPL$_IOLOCK8;

  //  rnd_dpt.dpt$t_name="RNDRIVER";
  
  /* a lot of these? */
  ini_fdt_act(&rnd_fdt,IO$_READLBLK, 0/*acp_std$readblk*/,1);
  ini_fdt_act(&rnd_fdt,IO$_READPBLK, 0/*acp_std$readblk*/,1);
  ini_fdt_act(&rnd_fdt,IO$_READVBLK, 0/*acp_std$readblk*/,1);
}

char rnddriverstring[]="RNDDRIVER";

struct _dpt rnd_dpt = {
  //  dpt$t_name:"RNDDRIVER"
  //  dpt$b_type:&testme,
  // dpt$t_name:rnddriverstring  // have no idea why this won't compile
};

void rnd$unit_init() {}
void rnd$struc_init() {}
void rnd$struc_reinit() {}

extern struct _ddt driver$ddt;
extern struct _dpt driver$dpt;
extern struct _fdt driver$fdt;

int driver$init_tables() {
  int printk();
  printk("in driver$init_tables\n");
  ini_dpt_name(&driver$dpt, "NLDRIVER");
  ini_dpt_adapt(&driver$dpt, 0);
  ini_dpt_defunits(&driver$dpt, 1);
  ini_dpt_ucbsize(&driver$dpt,sizeof(struct _ucb));
  ini_dpt_struc_init(&driver$dpt, rnd$struc_init);
  ini_dpt_struc_reinit(&driver$dpt, rnd$struc_reinit);
  ini_dpt_ucb_crams(&driver$dpt, 1/*NUMBER_CRAMS*/);
  ini_dpt_end(&driver$dpt);

  ini_ddt_unitinit(&driver$ddt, rnd$unit_init);
  ini_ddt_start(&driver$ddt, rnd_startio);
  ini_ddt_cancel(&driver$ddt, 0 /*ioc_std$cancelio*/);
  ini_ddt_end(&driver$ddt);

  /* for the fdt init part */
  /* a lot of these? */
  ini_fdt_act(&driver$fdt,IO$_READLBLK, 0/*acp_std$readblk*/,1);
  ini_fdt_act(&driver$fdt,IO$_READPBLK, 0/*acp_std$readblk*/,1);
  ini_fdt_act(&driver$fdt,IO$_READVBLK, 0/*acp_std$readblk*/,1);
  ini_fdt_end(&driver$fdt);

  return SS$_NORMAL;
}
