// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/unistd.h>
#include<linux/linkage.h>
#include<linux/mm.h>

#include <mytypes.h>
#include <misc.h>
#include <fiddef.h>
#include <uicdef.h>
#include <vcbdef.h>
#include <hm2def.h>
#include "../../rms/src/cache.h"
#include "../../rms/src/access.h"
#include <linux/slab.h>
#include <misc_routines.h>

static struct {
  char * from, * to;
} translate[] = {
  {"ubd","daa"},
  {"hda","dqa"},
  {"hdb","dqb"},
  {"hdc","dqc"},
  {"hdd","dqd"},
  {0,0}
}
;
extern char root_device_name[];

char * do_translate(char * from) {
  int i;
  if (0==strncmp(from,"DKA200",6)) // see rms.c
    from=root_device_name;
  for (i=0;i<5;i++) {
    if (0==strncmp(from,translate[i].from,3)) {
      char * c=kmalloc(15,GFP_KERNEL);
      memcpy(c,translate[i].to,3);
      memcpy(c+3,from+3,10);
      return c;
    }
  }
  return from;
}

asmlinkage int exe$mount(void *itmlst) {
  int i;
  struct item_list_3 *it=itmlst;
  struct _vcb * vcb;
  char *devs[2],*labs[2];
  int status;

  devs[0]=do_translate(it[0].bufaddr);

  if (it[1].item_code) {
#ifdef CONFIG_VMS
    status=mounte2(1,1,devs,labs,&vcb);
#endif
  } else {
    status=mount(1,1,devs,labs,&vcb);
  }

  if ((status&1)==0) return;

  if (vcb==0) return;

  printk("%%MOUNT-I-MOUNTED, Volume %12.12s mounted on %s\n",
	 vcb->vcb$t_volname,devs[0]);
}
