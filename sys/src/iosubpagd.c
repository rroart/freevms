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

int ioc$search(struct return_values *r, void * devnam) {
  /* ddb d not needed? */
  /* real device, no logical. do not have logicals yet */
  /* return ucb or 0 */
  struct dsc$descriptor *s=devnam;
  struct _ddb * d=ioc$gl_devlist;
  do {
    if (!bcmp(d->ddb$t_name,s->dsc$a_pointer,s->dsc$w_length)) {
      r->val1=d->ddb$l_ucb;
      return SS$_NORMAL;
    }
    d=d->ddb$ps_link;
  } while (d && d!=ioc$gl_devlist);
  return SS$_NOSUCHDEV;
}
