#include"../../freevms/starlet/src/starlet.h"
#include"../../freevms/starlet/src/iodef.h"
#include"../../freevms/starlet/src/ssdef.h"
#include"../../freevms/starlet/src/misc.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/lib/src/ccbdef.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include<linux/vmalloc.h>
#include<linux/linkage.h>

asmlinkage int exe$assign(void *devnam, unsigned short int *chan,unsigned int acmode, void *mbxnam, int flags) {
  int status;
  /* probe chan */
  /* probe mbxnam */
  /* 2 more */
  struct _ccb * c;
  struct _ucb * u;
  struct _ddb * d;
  struct return_values r;
  //  printk("here assign\n");
  status=ioc$ffchan(chan);
  if (status!=SS$_NORMAL) return status;
  /* lock i/o db */
  // printk("here assign %x\n", chan);
  if (*chan) c=&ctl$gl_ccbbase[*chan];
  //  printk("here assign\n");
  if (mbxnam) ioc$searchdev();
  status=ioc$search(&r,devnam);
  //printk("here assign\n");
  if (status!=SS$_NORMAL) return status;
  u=r.val1;

  /* not yet?
  c=vmalloc(sizeof(struct _ccb));
  bzero(c,sizeof(struct _ccb));
  */

  c->ccb$l_ucb=u;
  return status;
}


