// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<starlet.h>
#include<iodef.h>
#include<ssdef.h>
#include<misc.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ccbdef.h>
#include<system_data_cells.h>
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
  sch$iolockw();
  // printk("here assign %x\n", chan);
  c=&ctl$gl_ccbbase[*chan];
  c->ccb$b_amod=1; /* wherever this gets set */
  //  printk("here assign\n");
  if (mbxnam) ioc$searchdev();
  status=ioc$search(&r,devnam);
  //printk("here assign\n");
  if (status!=SS$_NORMAL) {
    sch$iounlock();
    return status;
  }
  u=r.val1;

  /* not yet?
  c=vmalloc(sizeof(struct _ccb));
  bzero(c,sizeof(struct _ccb));
  */

  c->ccb$l_ucb=u;
  sch$iounlock();
  return status;
}


