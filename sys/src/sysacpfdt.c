#include "../../freevms/lib/src/irpdef.h"
#include "../../freevms/lib/src/ucbdef.h"
#include "../../freevms/lib/src/ccbdef.h"
#include <linux/sched.h>

void acp$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int funcno, void * fdt, void * p1, long p2, long p3, long p4, long p5, long p6) {
  // p1 is buffer address
  // p2 is byte count
  // p3 is disk address
  int blocks;
  i->irp$w_sts|=IRP$M_FUNC;
  blocks=(p2+511)/512;
  i->irp$l_abcnt=0;
  i->irp$l_obcnt=p2;
  exe$qiodrvpkt(i,p,u);
  //  exe$finishioc(0,i,p,u); // only with errors?
}

void acp$writeblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int funcno, void * fdt, void * p1, long p2, long p3, long p4, long p5, long p6) {
}



