// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <prcdef.h>
#include <prvdef.h>
#include <sysgen.h>

// This shall later be implemented in a process

int jobctl_unsolicit(struct dsc$descriptor * term) {
  //  printk("<0>" "soon to creprc, term = %s\n",term->dsc$a_pointer);
  // in addition to term some more data is ordinarily sent to this routine.
  // normally received by a permanent mailbox
  // not yet $DESCRIPTOR(log, "[vms$common.sysexe]loginout");
   $DESCRIPTOR(log, "[vms$common.sysexe]dcl");
  int defpri = DEFPRI;
  int uic=0x00010004;
  long long priv = PRV$M_TMPMBX | PRV$M_NETMBX | PRV$M_SETPRV;
  int sts = PRC$M_INTER;
  struct dsc$descriptor prcnam;
  char nam[16]="_";

  prcnam.dsc$w_length=2+term->dsc$w_length;
  prcnam.dsc$a_pointer=nam;

  memcpy(&nam[1],term->dsc$a_pointer, term->dsc$w_length);
  nam[term->dsc$w_length+1]=':';

  sts = exe$creprc(0, &log, term, term, term, &priv, 0, &prcnam, defpri, uic, 0, sts);
  return sts;
}
