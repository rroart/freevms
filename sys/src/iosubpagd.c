#include"../../freevms/starlet/src/starlet.h"
#include"../../freevms/starlet/src/iodef.h"
#include"../../freevms/starlet/src/ssdef.h"
#include"../../freevms/librtl/src/descrip.h"
#include"../../freevms/starlet/src/misc.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/lib/src/ddbdef.h"
#include"../../freevms/lib/src/ccbdef.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include"../../freevms/sys/src/sysgen.h"
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
  } while (d!=ioc$gl_devlist);
  return SS$_NOSUCHDEV;
}
