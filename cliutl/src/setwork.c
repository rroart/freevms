// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<starlet.h>
//#include"../../starlet/src/vmsopt.h" 
#include<descrip.h>
#include<stdio.h>
#include<ssdef.h>

/* Author: Roar Thronæs */

/* Don't know anywhere else to put this */

set_working_set(int argc, char**argv){
  int pid=1;

  unsigned long sts;

  $DESCRIPTOR(p, "p1");
  $DESCRIPTOR(d, "quota");
  $DESCRIPTOR(d2, "extent");

  char c[80];
  struct dsc$descriptor o;
  o.dsc$a_pointer=c;
  o.dsc$w_length=80;
  memset(c, 0, 80);

  int retlen;

  sts = cli$present(&d);

  if (sts&1) {
    sts = cli$get_value(&d, &o, &retlen);
    sys$adjwsl(atoi(c),0);
    return SS$_NORMAL;
  }

  sts = cli$present(&d2);

  if (sts&1) {
    sts = cli$get_value(&d2, &o, &retlen);
    sys$adjwsl(atoi(c),0);
    return SS$_NORMAL;
  }
} 

