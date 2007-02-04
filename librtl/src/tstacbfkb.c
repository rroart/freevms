#include <lib/src/acbdef.h>
#include <lib/src/cwpsdef.h>
#include <lib/src/cwpsdef.h>
#include <lib/src/ftrddef.h>
#include <lib/src/irpdef.h>
#include <lib/src/lkbdef.h>
#include <lib/src/tastdef.h>
#include <lib/src/tqedef.h>
#include <lib/src/ucbdef.h>
#include <lib/src/cdrpdef.h>
#include <lib/src/cdtdef.h>
#include <lib/src/fkbdef.h>

#define print1(str) printf("%x %x %x %x %x %x %x %x %x\n",&str->str##$l_astqfl,&str->str##$l_astqbl,&str->str##$w_size,&str->str##$b_type,&str->str##$b_rmod,&str->str##$l_pid,&str->str##$l_ast,&str->str##$l_astprm,&str->str##$l_kast);

main() {
  struct _acb * acb = 0;
  struct _ftrd * ftrd = 0;
  struct _irp * irp = 0;
  struct _lkb * lkb = 0;
  struct _tast * tast = 0;
  struct _tqe * tqe = 0;
  struct _ucb * ucb = 0;
  struct _cdrp * cdrp = 0;
  struct _cdt * cdt = 0;
  struct _fkb * fkb = 0;

 printf("%x %x %x %x %x %x %x %x %x\n",&acb->acb$l_astqfl,&acb->acb$l_astqbl,&acb->acb$w_size,&acb->acb$b_type,&acb->acb$b_rmod,&acb->acb$l_pid,&acb->acb$l_ast,&acb->acb$l_astprm,&acb->acb$l_kast);
 printf("%x %x %x %x %x %x %x %x %x\n",&lkb->lkb$l_astqfl,&lkb->lkb$l_astqbl,&lkb->lkb$w_size,&lkb->lkb$b_type,&lkb->lkb$b_rmod,&lkb->lkb$l_pid,&lkb->lkb$l_ast,&lkb->lkb$l_astprm,&lkb->lkb$l_kast);
 printf("%x %x %x %x %x %x %x %x %x\n",&irp->irp$l_ioqfl,&irp->irp$l_ioqbl,&irp->irp$w_size,&irp->irp$b_type,&irp->irp$b_rmod,&irp->irp$l_pid,&irp->irp$l_ast,&irp->irp$l_astprm,&irp->irp$l_wind);
 printf("%x %x %x %x %x %x %x %x %x\n",&ftrd->ftrd$l_astqfl,&ftrd->ftrd$l_astqbl,&ftrd->ftrd$w_size,&ftrd->ftrd$b_type,&ftrd->ftrd$b_rmod,&ftrd->ftrd$l_pid,&ftrd->ftrd$l_ast,&ftrd->ftrd$l_astprm,&ftrd->ftrd$l_ast);
 // printf("%x %x %x %x %x %x %x %x %x\n",&tast->tast$l_astqfl,&tast->tast$l_astqbl,&tast->tast$w_size,&tast->tast$b_type,&tast->tast$b_rmod,&tast->tast$l_pid,&tast->tast$l_ast,&tast->tast$l_astprm,&tast->tast$l_kast);
 printf("%x %x %x %x %x %x %x %x %x\n",&tqe->tqe$l_tqfl,&tqe->tqe$l_tqbl,&tqe->tqe$w_size,&tqe->tqe$b_type,&tqe->tqe$b_rmod,&tqe->tqe$l_pid,&tqe->tqe$l_ast,&tqe->tqe$l_astprm,&tqe->tqe$q_time);

 printf("%x %x %x %x %x %x %x %x %x\n",&fkb->fkb$l_fqfl,&fkb->fkb$l_fqbl,&fkb->fkb$w_size,&fkb->fkb$b_type,&fkb->fkb$b_flck,&fkb->fkb$l_fpc,&fkb->fkb$l_fr3,&fkb->fkb$l_fr4,&fkb->fkb$l_fr4);
 printf("%x %x %x %x %x %x %x %x %x\n",&ucb->ucb$l_fqfl,&ucb->ucb$l_fqbl,&ucb->ucb$w_size,&ucb->ucb$b_type,&ucb->ucb$b_flck,&ucb->ucb$l_fpc,&ucb->ucb$l_fr3,&ucb->ucb$l_fr4,&ucb->ucb$l_fr4);
 // printf("%x %x %x %x %x %x %x %x %x\n",&fkb->fkb$l_fqfl,&fkb->fkb$l_fqbl,&fkb->fkb$w_size,&fkb->fkb$b_type,&fkb->fkb$b_flck,&fkb->fkb$l_fpc,&fkb->fkb$l_fr3,&fkb->fkb$l_fr4,&fkb->fkb$l_fr4);
 printf("%x %x %x %x %x %x %x %x %x\n",&cdrp->cdrp$l_fqfl,&cdrp->cdrp$l_fqbl,&cdrp->cdrp$w_cdrpsize,&cdrp->cdrp$b_cd_type,&cdrp->cdrp$b_flck,&cdrp->cdrp$l_fpc,&cdrp->cdrp$l_fr3,&cdrp->cdrp$l_fr4,&cdrp->cdrp$l_fr4);


}

