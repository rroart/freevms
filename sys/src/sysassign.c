#include"../../freevms/starlet/src/starlet.h"
#include"../../freevms/starlet/src/iodef.h"
#include"../../freevms/starlet/src/ssdef.h"
#include"../../freevms/lib/src/irpdef.h"
#include"../../freevms/lib/src/ucbdef.h"
#include"../../freevms/lib/src/ccbdef.h"
#include"../../freevms/sys/src/system_data_cells.h"
#include<linux/vmalloc.h>
#include<linux/linkage.h>

asmlinkage int exe$assign(void *devnam, unsigned short int *chan,unsigned int acmode, void *mbxnam) {
  /* probe chan */
  /* probe mbxnam */
  /* 2 more */
  *chan = ioc$ffchan();
  struct _ccb * c;
  struct _ucb * u;
  /* lock i/o db */
  if (*chan) c=&ctl$gl_ccbbase[*chan];
  if (mbxnam) ioc$searchdev(void);
  u=ioc$search(devnam);
}


