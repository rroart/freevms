// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/unistd.h>
#include<linux/linkage.h>

#include "../../freevms/lib/src/mytypes.h"
#include "../../freevms/starlet/src/misc.h"
#include "../../freevms/starlet/src/fiddef.h"
#include "../../freevms/starlet/src/uicdef.h"
#include "../../freevms/lib/src/hm2def.h"
#include "../../freevms/rms/src/cache.h"
#include "../../freevms/rms/src/access.h"

asmlinkage int exe$mount(void *itmlst) {
  int i;
  struct item_list_3 *it=itmlst;
  struct VCB * vcb;
  char *devs[2],*labs[2];
  int status;

  devs[0]=it[0].bufaddr;

  status=mount(0,1,devs,labs,&vcb);

  if (status&1==0) return;

  if (vcb==0) return;

  for (i = 0; i < vcb->devices; i++)
    if (vcb->vcbdev[i].dev != 0)
      printk("%%MOUNT-I-MOUNTED, Volume %12.12s mounted on %s\n",
	     vcb->vcbdev[i].home.hm2$t_volname,vcb->vcbdev[i].dev->devnam);
}
