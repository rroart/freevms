// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<starlet.h>
#include<iodef.h>
#include<ssdef.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ccbdef.h>
#include<system_data_cells.h>
#include<linux/vmalloc.h>
#include<linux/linkage.h>

asmlinkage int exe$dassgn(unsigned short int chan) {
  struct _ccb * ccb;
  int sts = ioc$verify_chan(chan, &ccb);
  if ((sts & 1) == 0) 
    return sts;
  // call exe$canceln, which is not yet implemented
  // call verify_chan again, in case of asts
  // check ccb$l_wind and close the file
  // check ccb$w_ioc to see if outstanding io
  // lock io db
  ccb->ccb$b_amod=0;
  // disassociate mailbox
  // dec ucb ref count
  ccb->ccb$l_ucb->ucb$l_refc--;
  // checks and stuff to do if refc == 0
  // do ioc$last_chan
  // ioc$unlock
  return SS$_NORMAL;
}


