#include"../../freevms/starlet/src/starlet.h"
#include"../../freevms/starlet/src/iodef.h"
#include"../../freevms/starlet/src/ssdef.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include<linux/vmalloc.h>
#include<linux/linkage.h>

void exe$insertirp(void * u, struct _irp * i) {
  struct _irp *tmp=((struct _irp *)u)->irp$l_ioqfl;
  while (tmp!=u && i->irp$b_pri>tmp->irp$b_pri)
    tmp=tmp->irp$l_ioqfl;
  insque(i,tmp);
}

void exe$abortio(void) {
}

void exe$altqueuepkt (void) {
}

void exe$finishio (void) {
}

void exe$finishioc (void) {
}

int exe$insioq (struct _ucb * u, struct _irp * i) {
  /* raise ipl */
  if (u->ucb$l_sts & UCB$M_BSY)
    exe$insertirp(&u->ucb$l_ioqfl,i);
  else {
    u->ucb$l_sts|=UCB$M_BSY;
    ioc$initiate(u,i);
  }
  /* restore ipl */
  /* release fork lock */
  return;
}

/* put this into a struct */
asmlinkage int exe$qio (struct struct_qio * q) {
  int func;
  unsigned int c, d;
  struct _irp * i;
  exe$clref(q->efn);
  if (q->chan<0 || q->chan>ctl$gl_chindx) return SS$_IVCHAN;
  /*ccb$l_wind stuff*/
  func=q->func;
  /*spooled?*/
  /*no access check yet*/
  /*check fdt mask*/
  /*check func code*/
  /*check iosb*/
  /*set ipl 2*/
  /*check proc quota*/
  i=vmalloc(sizeof(struct _irp));
  bzero(i,sizeof(struct _irp));
  i->irp$b_efn=q->efn;
  i->irp$l_ast=q->astadr;
  i->irp$l_astprm=q->astprm;
  i->irp$l_iosb=q->iosb;
  i->irp$w_chan=q->chan;
  i->irp$w_func=q->func;
  /* do preprocessing */
  /* does it do one or more functions */
  for(c=0,d=1;c<64;c++,d=d<1) /* right order? */
    if (d&func) { }
      //      ctl$ga_ccb_table[q->chan].ccb$l_ucb->ucb$l_ddt->ddt$l_fdt->fdt$ps_func_rtn[c](q->p1,q->p2,q->p3,q->p4,q->p5,q->p6);
  
}

void exe$qioacppkt (void) {
}

void exe$qiodrvpkt (struct _pcb * p, struct _ucb * u, struct _irp * i) {

  exe$insioq(u,i);
  /* restore ipl to 0 */

}

void exe$qioqxqppkt (void) {
}

