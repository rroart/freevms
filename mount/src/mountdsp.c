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
#if 0
#include "../../rms/src/cache.h"
#include "../../rms/src/access.h"
#endif
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
  int fromlen = strlen(from);
  if (0==strncmp(from,"DKA200",6)) // see rms.c
    from=root_device_name;
  for (i=0;i<5;i++) {
    if (0==strncmp(from,translate[i].from,2)) {
      char * c=kmalloc(15,GFP_KERNEL); // check. leak.
      memcpy(c,translate[i].to,3);
      c[3] = from[2] - 'a' + '0';
      if (c[0] != 'd')
	memcpy(c+3,from+3,10);
      else {
	int unit = 0;
	if (fromlen > 3)
	  unit = from[3] - '0';
	sprintf(c+4, "%02d", unit);
      }
#if 0
      printk("do_tran %s %s\n",from,c);
#endif
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
    status=mounte2(1,1,devs,labs,&vcb);
  } else {
    status=mount(1,1,devs,labs,&vcb);
  }

  if ((status&1)==0) return;

  if (vcb==0) return;

  printk("%%MOUNT-I-MOUNTED, Volume %12.12s mounted on %s\n",
	 vcb->vcb$t_volname,devs[0]);
}
