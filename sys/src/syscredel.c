// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/unistd.h>
#include<linux/linkage.h>
#include <system_data_cells.h>
#include <linux/mm.h>
#include <mmgdef.h>
#include <pfndef.h>
#include <rdedef.h>
#include <ssdef.h>
#include <va_rangedef.h>

// exe$cntreg
// exe$cretva
// exe$deltva
// exe$expreg
// mmg$credel
// mmg$crepag
// mmg$cretva
// mmg$delpag
// mmg$fast_create
// mmg$try_all
// create_bufobj

struct _mmg mymmg;

int mmg$credel(int acmode, void * first, void * last, void (* pagesubr)(), struct _va_range *inadr, struct _va_range *retadr, unsigned int acmodeagain, struct _pcb * p, int numpages) {
  unsigned long tmp=first;
  if (((long)first & 0x80000000) || ((long)last & 0x80000000))
    return SS$_NOPRIV;
  mymmg.mmg$l_pagesubr=pagesubr;
  while (tmp<last) {
    pagesubr(tmp);
    tmp=tmp+PAGE_SIZE;
  }
  return SS$_NORMAL;
}

asmlinkage int exe$cretva (struct _va_range *inadr, struct _va_range *retadr, unsigned int acmode) {

}

int mmg$crepag (int acmode, void * va, struct _pcb * p, signed int pagedirection, struct _rde * rde) {

  if (va>=rde->rde$pq_first_free_va) {

  }

  

}
