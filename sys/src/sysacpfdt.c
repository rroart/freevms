// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <iodef.h>
#include <irpdef.h>
#include <ucbdef.h>
#include <ccbdef.h>
#include <linux/sched.h>

int acp_std$access(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  return exe$qioacppkt(i,p,u);
}

int acp_std$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  // p1 is buffer address
  // p2 is byte count
  // p3 is disk address
  int blocks;
  i->irp$l_sts|=IRP$M_FUNC;
  blocks=(i->irp$l_qio_p2+511)/512;
  i->irp$l_abcnt=0;
  i->irp$l_obcnt=i->irp$l_qio_p2;
  if (i->irp$v_fcode<IO$_LOGICAL)
    return exe$qiodrvpkt(i,p,u);
  else
    return exe$qioacppkt(i,p,u);
  //  exe$finishioc(0,i,p,u); // only with errors?
}

void acp_std$writeblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  int blocks;
  i->irp$l_sts|=IRP$M_FUNC;
  blocks=(i->irp$l_qio_p2+511)/512;
  i->irp$l_abcnt=0;
  i->irp$l_obcnt=i->irp$l_qio_p2;
  if (i->irp$v_fcode<IO$_LOGICAL)
    return exe$qiodrvpkt(i,p,u);
  else
    return exe$qioacppkt(i,p,u);
  //  exe$finishioc(0,i,p,u); // only with errors?
}

void acp_std$accessnet(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
}

void acp_std$deaccess(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
}

void acp_std$modify(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
  return exe$qioacppkt(i,p,u);
}

void acp_std$mount(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c) {
}



