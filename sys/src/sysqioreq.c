#include"../../freevms/starlet/src/starlet.h"
#include"../../freevms/starlet/src/iodef.h"
#include"../../freevms/starlet/src/ssdef.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include<linux/vmalloc.h>

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
  if (u->ucb$l_sts & UCB$V_BSY)
    exe$insertirp(&u->ucb$l_ioqfl,i);
  else {
    u->ucb$l_sts|=UCB$V_BSY;
    /* ioc$initiate */
  }
  /* restore ipl */
  /* release fork lock */
  return;
}

/* put this into a struct */
int exe$qio (struct struct_qio * q) {
  int func;
  struct _irp * i;
  exe$clref(q->efn);
  if (q->chan<0 || q->chan>ctl$gw_chindx) return SS$_IVCHAN;
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
  i->irp$b_func=q->func;
  /* do preprocessing */

  
}

void exe$qioacppkt (void) {
}

void exe$qiodrvpkt (struct _pcb * p, struct _ucb * u, struct _irp * i) {

  exe$insioq(u,i);
  /* restore ipl to 0 */

}

void exe$qioqxqppkt (void) {
}

