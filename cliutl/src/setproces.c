// $Id$
// $Locker$

// Author. Roar Thronæs.

#include"../../starlet/src/starlet.h"
#include"../../starlet/src/vmsopt.h" 
#include"../../librtl/src/descrip.h"
#include<stdio.h>

/* Author: Roar Thronæs */

/* Don't know anywhere else to put this */

main(int argc, char**argv){
  int pid=1;
  char * newprcnam=0;
  //sys$setpri(&pid,30,30,30,30,30);
  signed int n;
  while ((n=vmsopt_parse( argc, argv ) ) != -1) {
    switch(n) {
    case VMSOPT_NOARG:
      printf("noarg\n");
      break;
    case VMSOPT_ARG_CO:
    case VMSOPT_ARG_EQ:
      printf("arg\n");
      break;
    case VMSOPT_ARG_PL:
      newprcnam=vms_optstr;
    default:
      break;
    }
  }
  if (newprcnam) {
    $DESCRIPTOR(newstr,newprcnam);
    sys$setprn(&newstr);
	       }
} 

