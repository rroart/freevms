// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>

#include <ccbdef.h>
#include <ddtdef.h>
#include <dptdef.h>
#include <dyndef.h>
#include <fdtdef.h>
#include <internals.h>
#include <iodef.h>
#include <ipldef.h>
#include <irpdef.h>
#include <ssdef.h>
#include <system_data_cells.h>
#include <ucbdef.h>

struct _mb_ucb ucb0;

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
  void * mmb$l_wqfl;
  void * mmb$l_wqbl;
  void * mmb$l_startdata;
  unsigned short mmb$w_datasize;
  unsigned short mmb$w_bufquo;
  char mmb$t_data[65536];
};

struct __rdb {
  union {
    struct __rdb * rdb$l_mfl;
    void * rdb$l_startdata;
  };
  union {
    struct __rdb * rdb$l_mbl;
    void * rdb$l_userbuffer;
  };
  unsigned short int rdb$w_size;
  unsigned char rdb$b_type;
  unsigned char rdb$b_func;
  unsigned short int rdb$w_msgsize;
  //  struct _irp * rdb$l_irp;
  //  unsigned long rdb$l_pid;
  void * rdb$l_startdata;
  unsigned short rdb$w_datasize;
  unsigned short rdb$w_reqsiz;
  unsigned short rdb$w_bufquo;
  char rdb$t_data[65536];
};

void mb_setmode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { 

}

void mb_sensemode(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) { 
  int func=i->irp$l_func;

}

void mb_write (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int savipl;
  struct __mmb * m;
  int func=i->irp$l_func;
  if (func==IO$_WRITEOF) {
    i->irp$l_boff=0;
    i->irp$l_bcnt=0;
  } else {
    i->irp$l_bcnt=i->irp$l_qio_p2;
    i->irp$l_media=i->irp$l_qio_p1;
  }
  m=vmalloc(sizeof(struct __mmb));
  bzero(m,sizeof(struct __mmb));
  m->mmb$w_size=sizeof(struct __mmb);
  m->mmb$b_type=DYN$C_BUFIO;
  m->mmb$b_func=func;
  m->mmb$w_msgsize=i->irp$l_qio_p2;
  m->mmb$l_irp=i;
  m->mmb$l_pid=smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb->pcb$l_pid;
  if (m->mmb$w_msgsize)
    memcpy(m->mmb$t_data,i->irp$l_qio_p2,m->mmb$w_size);
  savipl=vmslock(&SPIN_MAILBOX,IPL$_MAILBOX);
  u->ucb$w_msgcnt++;
  insque(m,&u->ucb$l_mb_msgqfl);
  vmsunlock(&SPIN_MAILBOX,savipl);
  exe$finishioc(i->irp$l_iosb,i,p,u);
  return SS$_NORMAL;
}

void mb_read (struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int func=i->irp$l_func;
  struct __mmb * m;
  int savipl;
  i->irp$l_bcnt=i->irp$l_qio_p2;
  i->irp$l_media=i->irp$l_qio_p1;
  m=vmalloc(sizeof(struct __mmb));
  bzero(m,sizeof(struct __mmb));
  m->mmb$w_size=sizeof(struct __mmb);
  m->mmb$b_type=DYN$C_BUFIO;
  m->mmb$b_func=func;
  m->mmb$w_msgsize=i->irp$l_qio_p2;
  m->mmb$l_irp=i;
  m->mmb$l_pid=smp$gl_cpu_data[smp_processor_id()]->cpu$l_curpcb->pcb$l_pid;
  //  if (m->mmb$w_msgsize)
  //    memcpy(m->mmb$t_data,i->irp$l_qio_p2,m->mmb$w_size);
  savipl=vmslock(&SPIN_MAILBOX,IPL$_MAILBOX);
  u->ucb$w_msgcnt++;
  insque(m,&((struct _mb_ucb *)u)->ucb$l_mb_readqfl);
  vmsunlock(&SPIN_MAILBOX,savipl);
  exe$finishioc(i->irp$l_iosb,i,p,u);
  return SS$_NORMAL;
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
