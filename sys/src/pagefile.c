// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <asm/bitops.h>
#include <linux/config.h>
#include <pfldef.h>
#include <ssdef.h>

extern int myswapfile;

int mmg$allocpagfil1(int size, int * page) {

  struct _pfl * pfl = myswapfile;
  if (pfl==0)
    return 0;
  if (pfl->pfl$l_frepagcnt==0)
    return 0;
  int bit=find_next_zero_bit(pfl->pfl$l_bitmap,pfl->pfl$l_bitmapsiz<<3,0);
#if 0
  if (bit==0)
    return 0;
  bit--;
#endif
  set_bit(bit,pfl->pfl$l_bitmap);
  *page=bit;
  pfl->pfl$l_frepagcnt--;
  return SS$_NORMAL;
}

int mmg$dallocpagfil1(int page) {

  struct _pfl * pfl = myswapfile;
  if (pfl==0)
    return 0;
  test_and_clear_bit(page,pfl->pfl$l_bitmap);
  pfl->pfl$l_frepagcnt++;
  return SS$_NORMAL;
}
