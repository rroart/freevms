#include"../../freevms/starlet/src/starlet.h"
#include"../../freevms/starlet/src/iodef.h"
#include"../../freevms/starlet/src/ssdef.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/lib/src/ccbdef.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include"../../freevms/sys/src/sysgen.h"
#include<linux/vmalloc.h>
#include<linux/linkage.h>

int ioc$ffchan(void) {
  int i;
  for (i=0; i<CHANNELCNT,  ctl$gl_ccbbase[i].ccb$b_amod; i++) ;
  if (ctl$gl_ccbbase[i].ccb$b_amod)
    return 0;
  else
    return i;
}

int ioc$searchdev(void) {
  /* implement later */
}

void * ioc$search(void * devnam) {
  /* real device, no logical. do not have logicals yet */
  /* return ucb or 0 */
}
