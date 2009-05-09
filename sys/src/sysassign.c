// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file sysassign.c
   \brief QIO assign channel - TODO still more doc
   \author Roar Thronæs
*/

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

/**
   \brief assign i/o channel system service - see 5.2 21.5.2
   \param devnam device name
   \param chan return channel value
   \param acmode access mode
   \param mbxnam mailbox name
   \param flags misc flags - TODO not yet used
*/

asmlinkage int exe$assign(void *devnam, unsigned short int *chan,unsigned int acmode, void *mbxnam, int flags) {
  /** 21.5.2.1 */
  int status;
  /** probe chan writeable - MISSING */
  /** probe mbxnam readable - MISSING */
  /** probe devnam readable - MISSING */
  /** handle acmode - MISSING */
  struct _ccb * c;
  struct _ucb * u;
  struct _ddb * d;
  struct return_values r,r2={0,0};
  //  printk("here assign\n");
  /** lock i/o db */
  sch$iolockw(); // moved up because of global db
  /** find free channel */
  status=ioc$ffchan(chan);
  /** if error return */
  /** TODO maybe also unlock i/o db */
  if (status!=SS$_NORMAL) return status;
  /* lock i/o db */
  // sch$iolockw();
  // printk("here assign %x\n", chan);
  c=&ctl$gl_ccbbase[*chan];
  c->ccb$b_amod=1; /* wherever this gets set */
  //  printk("here assign\n");
  /** search for an eventual mbxnam */
  if (mbxnam) ioc$searchdev(&r2,mbxnam);
  /** search for devnam */
  status=ioc$search(&r,devnam);
  //printk("here assign\n");
  /** if error unlock i/o db and return */
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

  /** 21.5.2.2 */

  if (u->ucb$l_sts&UCB$M_TEMPLATE) {
    /** if network dev, test for NETMBX priv - MISSING */
    /** ucb quota check - MISSING */
    struct _ucb * new;
    struct _ddt * ddt=u->ucb$l_ddt;
    /** invoke ucb cloning */
    int sts=ioc_std$clone_ucb(u, &new);
    /** store uic in its orb - MISSING */
    /** set deleteucb bit - MISSING */
    /** if template is a mailbox, set delmbx - MISSING */
    /** clear refc - MISSING */
    /** invoke ioc$debit_ucb - MISSING */
    u=new;
    printk("ucb cloned in assign %x\n",ddt->ddt$l_cloneducb);
    if (ddt->ddt$l_cloneducb) { 
     int (*fn)() = ddt->ddt$l_cloneducb;
     /** invoke ddt cloneucb routine */
     fn(u);
     /** handle error return - MISSING */
     /** if nonshared device set ucb pid - MISSING */
     /** 21.5.2.2.3 associated mbx processing - MISSING */
    }
  }

  /** 21.5.2.2.4 local device final processing */

  /** if appropiate, invoke ioc$lock_dev - MISSING */

  /** if nonshared and nonowned, set pid - MISSING */

  /** set ccb ucb */
  c->ccb$l_ucb=u;

  /** increase ucb ref count */
  c->ccb$l_ucb->ucb$l_refc++;

  /** set ccb amod - MISSING */

  /** set ccb sts - MISSING */

  /** chan was previously written */

  /** unlock i/o db and return normal? */
  sch$iounlock();
  setipl(0); // simulate rei
  return status;

  /** 21.5.2.3 assign non-local - MISSING */
  /** it might have worked once, but clustering etc is non-working now */
}


