// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/config.h>

#include<starlet.h>
#include<iodef.h>
#include<ssdef.h>
#include<misc.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ccbdef.h>
#include <ddtdef.h>
#include<system_data_cells.h>
#include <ioc_routines.h>
#include<linux/linkage.h>
#include <sch_routines.h>
#include <ioc_routines.h>

#include <descrip.h>

#include <ipl.h>

asmlinkage int exe$assign(void *devnam, unsigned short int *chan,unsigned int acmode, void *mbxnam, int flags) {
  int status;
  /* probe chan */
  /* probe mbxnam */
  /* 2 more */
  struct _ccb * c;
  struct _ucb * u;
  struct _ddb * d;
  struct return_values r,r2={0,0};
  //  printk("here assign\n");
  /* lock i/o db */
  sch$iolockw(); // moved up because of global db
  status=ioc$ffchan(chan);
  if (status!=SS$_NORMAL) return status;
  /* lock i/o db */
  // sch$iolockw();
  // printk("here assign %x\n", chan);
  c=&ctl$gl_ccbbase[*chan];
  c->ccb$b_amod=1; /* wherever this gets set */
  //  printk("here assign\n");
  if (mbxnam) ioc$searchdev(&r2,mbxnam);
  status=ioc$search(&r,devnam);
  //printk("here assign\n");
  if (status!=SS$_NORMAL) {
    sch$iounlock();
    return status;
  }
  u=r.val1;

  u->ucb$l_amb=r2.val1; // maybe set associated mb someplace?

  /* not yet?
  c=vmalloc(sizeof(struct _ccb));
  bzero(c,sizeof(struct _ccb));
  */

  if (u->ucb$l_sts&UCB$M_TEMPLATE) {
    struct _ucb * new;
    struct _ddt * ddt=u->ucb$l_ddt;
    int sts=ioc_std$clone_ucb(u, &new);
    u=new;
    printk("ucb cloned in assign %x\n",ddt->ddt$l_cloneducb);
    if (ddt->ddt$l_cloneducb) { 
     int (*fn)() = ddt->ddt$l_cloneducb;
     fn(u);
    }
  }

  c->ccb$l_ucb=u;
  c->ccb$l_ucb->ucb$l_refc++;

  sch$iounlock();
  setipl(0); // simulate rei
  return status;
}


