// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <stdio.h> 
#include <ssdef.h> 
#include <descrip.h> 
#include <lkbdef.h>
#include <lkidef.h>
#include <rsbdef.h>
#include <sysgen.h>
#include <starlet.h>
#include <misc.h>

/*
Lock Database
-------------
   LKB Address     Lockid  ParentId   PID     BLKAST  SubLocks RQ GR  Queue     
RSB Address             Resource Name          Mode
----------------- -------- -------- -------- -------- -------- -- -- ------- ---
-------------- ------------------------------- ----
FFFFFFFF.79BD9E80 12000001 00000000 00000000 82B167A8        0    PR Granted FFF
FFFFF.7FCCBE80 F11B$aOBELIXSYS   ....          Kern
 */

// missing LKI codes, do this as privileged as show memory?

int sda$getmem(const void *src, void *dest, int n);

show_locks(){
  int i;
  char rsbname[15];
  char rsblen;
  printf("Lock Database\n");
  printf("-------------\n");
  printf("   LKB Address     Lockid  ParentId   PID     BLKAST  SubLocks RQ GR  Queue     RSB Address             Resource Name          Mode\n");
  printf("----------------- -------- -------- -------- -------- -------- -- -- ------- ----------------- ------------------------------- ----");

  extern unsigned long lockidtbl[LOCKIDTBL];
  unsigned long mylockidtbl[LOCKIDTBL];
  sda$getmem(&lockidtbl[0], &mylockidtbl[0], sizeof(lockidtbl));
  for(i = 0; i < LOCKIDTBL; i++) {
    if ((mylockidtbl[i] & 0xffff0000) == 0)
      continue;
    struct _lkb * l = mylockidtbl[i];
    struct _lkb lkb;
    sda$getmem(l, &lkb, sizeof(lkb));
    struct _rsb * r = lkb.lkb$l_rsb;
    struct _rsb rsb;
    sda$getmem(r, &rsb, sizeof(rsb));
    printf("%16lx ", l);
    printf("%8x ", i);
    printf("%8x ", lkb.lkb$l_parent); // add id
    printf("%8x ", lkb.lkb$l_pid);
    printf("%16lx ", lkb.lkb$l_blkastadr);
    printf("%8x ", lkb.lkb$w_refcnt); // check
    printf("%x ", lkb.lkb$b_rqmode);
    printf("%x ", lkb.lkb$b_grmode);
    printf("%x ", 0);
    printf("%16lx ", r);
    printf("%s", rsb.rsb$t_resnam);
    printf("%x ", 0);
    printf("\n");
  }
  return 1;
}
