// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/init.h>
#include <linux/sched.h>
#include <system_data_cells.h>
#include <ddtdef.h>
#include <dptdef.h>
#include <fdtdef.h>
#include <ucbdef.h>
#include <iodef.h>
#include <irpdef.h>
#include <ccbdef.h>

struct _ucb ucb0;

struct _dpt mb_dpt = { };

struct _fdt mb_fdt = {
};

struct __mmb {
  union {
    struct __mmb * mmb$l_mfl;
    void * mmb$l_startdata;
  };
  union {
    struct __mmb * mmb$l_mbl;
    void * mmb$l_userbuffer;
  };
  unsigned short int mmb$w_size;
  unsigned char mmb$b_type;
  unsigned char mmb$b_func;
  unsigned short int mmb$w_msgsize;
  struct _irp * mmb$l_irp;
  unsigned long mmb$l_pid;
  char mmb$t_data[65536];
};

void mb_setmode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { 

}

void mb_sensemode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { 
  int func=i->irp$l_func;

}

void mb_write (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int func=i->irp$l_func;
  if (func==IO$_WRITEOF) {
    i->irp$l_boff=0;
    i->irp$l_bcnt=0;
  } else {
    i->irp$l_bcnt=i->irp$l_qio_p2;
    i->irp$l_media=i->irp$l_qio_p1;
  }
}

void mb_read (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int func=i->irp$l_func;

}

void mb_cancel (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int func=i->irp$l_func;

}

void mb_aux_routine (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int func=i->irp$l_func;

}

struct _ddt mb_ddt = {
  //  devnam: "MB",
  ddt$l_functb: &mb_fdt,
  ddt$l_cancel: mb_cancel,
  ddt$l_aux_routine: mb_aux_routine
};

int __init driverinit() {

  mb$ar_ucb0=&ucb0;

  ini_dpt_name(&mb_dpt, "MBDRIVER");
  ini_dpt_end(&mb_dpt);

  ini_fdt_act(&mb_fdt, IO$_READLBLK, mb_read, 1);
  ini_fdt_act(&mb_fdt, IO$_READPBLK, mb_read, 1);
  ini_fdt_act(&mb_fdt, IO$_READVBLK, mb_read, 1);
  ini_fdt_act(&mb_fdt, IO$_READVBLK, mb_read, 1);
  ini_fdt_act(&mb_fdt, IO$_WRITELBLK, mb_write, 1);
  ini_fdt_act(&mb_fdt, IO$_WRITEPBLK, mb_write, 1);
  ini_fdt_act(&mb_fdt, IO$_WRITEVBLK, mb_write, 1);
  ini_fdt_act(&mb_fdt, IO$_WRITEOF, mb_write, 1);
  ini_fdt_act(&mb_fdt, IO$_SETMODE, mb_setmode, 1);
  ini_fdt_act(&mb_fdt, IO$_SENSEMODE, mb_sensemode, 1);
}
