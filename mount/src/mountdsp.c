// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/unistd.h>
#include<linux/linkage.h>

#include <mytypes.h>
#include <misc.h>
#include <fiddef.h>
#include <uicdef.h>
#include <vcbdef.h>
#include <hm2def.h>
#include "../../freevms/rms/src/cache.h"
#include "../../freevms/rms/src/access.h"

asmlinkage int exe$mount(void *itmlst) {
  int i;
  struct item_list_3 *it=itmlst;
  struct _vcb * vcb;
  char *devs[2],*labs[2];
  int status;

  devs[0]=it[0].bufaddr;

  if (it[1].item_code) {
#ifdef CONFIG_VMS
    status=mounte2(1,1,devs,labs,&vcb);
#endif
  } else {
    status=mount(1,1,devs,labs,&vcb);
  }

  if (status&1==0) return;

  if (vcb==0) return;

  printk("%%MOUNT-I-MOUNTED, Volume %12.12s mounted on %s\n",
	 vcb->vcb$t_volname,devs[0]);
}
