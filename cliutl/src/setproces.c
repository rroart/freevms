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

set_process(int argc, char**argv){
  int pid=1;
  struct dsc$descriptor d;
  if (0==strncmp(argv[0],"/name",strlen(argv[0]))) {
    d.dsc$w_length=strlen(argv[1]);
    d.dsc$a_pointer=argv[1];
    sys$setprn(&d);
    return SS$_NORMAL;
  }
  if (0==strncmp(argv[0],"/priority",strlen(argv[0]))) {
    sys$setpri(0,0,atoi(argv[1]),0,0,0);
    return SS$_NORMAL;
  }
} 

