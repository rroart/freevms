// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/vmalloc.h>
#include<linux/linkage.h>

#include <system_data_cells.h>
#include <descrip.h>
#include <starlet.h>
#include <misc.h>
#include <ssdef.h>
#include <ipldef.h>
#include <ccbdef.h>

asmlinkage int exe$crembx  (char prmflg, unsigned short int *chan, unsigned int maxmsg, unsigned int bufquo, unsigned int promsk, unsigned int acmode, void *lognam,...) {
  int status;
  struct _ccb * c;
  $DESCRIPTOR(mytabnam,"LNM$SYSTEM_TABLE");
  $DESCRIPTOR(mypartab,"LNM$SYSTEM_DIRECTORY");
  struct item_list_3 i[2];
  /* verify writable chan arg */
  /* ma780 stuff will not be relevant for some time */
  setipl(IPL$_ASTDEL);
  status=ioc$ffchan(chan);
  /* no priv check yet */
  /* lock i/o database mutex */
  if (lognam==0) {
    /* assume nonexist and must create */
    
  } else {
    i[0].item_code=1;
    
    i[1].item_code=0;
    //    status=exe$trnlnm(0,mytabnam,lognam,0,i);
    
  }
  if (status&0==0) { //does not exist?
    //    status=exe$crelnm(0,mytabnam,lognam,0,i);
  }
  /* incr ref count */
  // ucb ccb stuff 
  c=&ctl$gl_ccbbase[*chan];
  ioc_std$clone_ucb(0 /*mb$ar_ucb0*/,&c->ccb$l_ucb);
  // unlock mutex
  setipl(0);
  return SS$_NORMAL;
}

int exe$delmbx  (unsigned short int chan)
{
}

asmlinkage int exe$crembx_wrap(struct struct_crembx * s) {
  return exe$crembx(s->prmflg, s->chan, s->maxmsg, s->bufquo, s->promsk, s->acmode, s->lognam);
}
