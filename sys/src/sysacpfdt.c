#include "../../freevms/lib/src/irpdef.h"
#include "../../freevms/lib/src/ucbdef.h"
#include "../../freevms/lib/src/ccbdef.h"
#include <linux/sched.h>

void acp$readblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int funcno, void * fdt, void * fpar) {
  exe$finishioc(0,i,p,u);
}

void acp$writeblk(struct _irp * i, struct _pcb * p, struct _ucb * u, struct _ccb * c, int funcno, void * fdt, void * fpar) {
  exe$finishioc(0,i,p,u);
}
