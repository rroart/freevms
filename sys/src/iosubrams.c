// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/kernel.h>
#include <asmlink.h>
#include <linux/smp.h>
#include <asm/hw_irq.h>
#include <asm/current.h>
#include <system_data_cells.h>
#include <internals.h>
#include<queue.h>
#include<ddtdef.h>
#include<ucbdef.h>
#include<irpdef.h>
#include<ipldef.h>
#include<wcbdef.h>
#include<ssdef.h>

int ioc_std$mapvblk (unsigned int vbn, unsigned int numbytes, struct _wcb *wcb, struct _irp *irp, struct _ucb *ucb, unsigned long *lbn_p, unsigned *notmapped_p, struct _ucb **new_ucb_p) {
  struct _wcb * head=wcb;
  struct _wcb * tmp=head->wcb$l_wlfl;
  while (tmp!=head) {
    if (vbn>=tmp->wcb$l_stvbn && vbn<(tmp->wcb$l_stvbn+tmp->wcb$l_p1_count)) {
      *lbn_p=tmp->wcb$l_p1_lbn+(vbn-tmp->wcb$l_stvbn);
      return SS$_NORMAL;
    }
    tmp=tmp->wcb$l_wlfl;
  }
  return 0;
}
