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
  struct dsc$descriptor d;
  if (0==strncmp(argv[0],"/quota",strlen(argv[0]))) {
    sys$adjwsl(atoi(argv[1]),0);
    return SS$_NORMAL;
  }
  if (0==strncmp(argv[0],"/extent",strlen(argv[0]))) {
    sys$adjwsl(atoi(argv[1]),0);
    return SS$_NORMAL;
  }
} 

