// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include <system_data_cells.h>
#include<ipldef.h>
#include<va_rangedef.h>
#include<wsldef.h>

asmlinkage int exe$crmpsc(struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode, unsigned int flags, void *gsdnam, unsigned int relpag, unsigned short int chan, unsigned int pagcnt, unsigned int vbn, unsigned int prot,unsigned int pfc) {
  // we will just have to pretend this fd is chan and offset i vbn (mmap)?

  mmg$vfysecflg();
  // check channel
  // skip wcb for some time?
  setipl(IPL$_ASTDEL);
  mmg$dalcstxscn();
  

}
