// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<starlet.h>
#include<iodef.h>
#include<ssdef.h>
#include<descrip.h>
#include<misc.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ddbdef.h>
#include<ccbdef.h>
#include<system_data_cells.h>
#include<sysgen.h>
#include<linux/vmalloc.h>
#include<linux/linkage.h>

int ioc$ffchan(unsigned short int *chan) {
  unsigned short int i;
  for (i=0; i<CHANNELCNT,  ctl$gl_ccbbase[i].ccb$b_amod; i++) ;
  if (ctl$gl_ccbbase[i].ccb$b_amod)
    return SS$_NOIOCHAN;
  else {
    *chan=i;
    return SS$_NORMAL;
  }
}

int ioc$searchdev() {
  /* implement later */
}

// remember to make this interface like ioc_std$search in ioc_routines.h

int ioc$search(struct return_values *r, void * devnam) {
  // no use of ioc$trandevnam yet 
  /* ddb d not needed? */
  /* real device, no logical. do not have logicals yet */
  /* return ucb or 0 */
  struct dsc$descriptor *s=devnam;
  struct _ddb * d=ioc$gl_devlist;
  do {
    //    printk("bcmp %s %s\n",d->ddb$t_name,s->dsc$a_pointer);
    if (!bcmp(d->ddb$t_name,s->dsc$a_pointer,3/* was s->dsc$w_length*/)) {
      struct _ucb * tmp = d->ddb$l_ucb;
      char * c=s->dsc$a_pointer;
      char unit=c[3]-48;
      do {
	//printk("unitcmp %x %x\n",unit,tmp->ucb$w_unit);
	if (unit==tmp->ucb$w_unit)
	  goto out;
	tmp=tmp->ucb$l_link;
      } while (tmp && tmp!=d->ddb$l_ucb);
      return SS$_NOSUCHDEV;
    out:
      r->val1=tmp; // was d->ddb$l_ucb;
      return SS$_NORMAL;
    }
    d=d->ddb$ps_link;
  } while (d && d!=ioc$gl_devlist);
  return SS$_NOSUCHDEV;
}
