// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <linux/bitops.h>

#include<ipldef.h>
#include<irpdef.h>
#include<mpwdef.h>
#include<schdef.h>
#include<system_data_cells.h>

#if 0
struct _irp * mpw$gl_irpfl = &mpw$gl_irpfl;
struct _irp * mpw$gl_irpbl = &mpw$gl_irpfl;
#endif

unsigned char mpw$gb_state;

void mmg$purgempl(unsigned long command);

void mmg$wrtmfypag(void) {
  if (mmg$gl_maxpfidx==0)
    return;
  if (sch$gl_mfylim>sch$gl_mfycnt)
    return;
  if (test_and_set_bit(SCH$V_MPW,sch$gl_sip))
    return;
#if 0
  setipl(IPL$_MMG);
  spin_lock(&SPIN_MMG);
#endif
  mmg$purgempl(MPW$C_MAINTAIN);

  
}

void mmg$purgempl(unsigned long command) {
  if (mpw$ar_perfstats==MPW$C_SVAPTE)
    return;
  mpw$gb_state=command;
}
