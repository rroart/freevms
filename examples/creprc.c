#include <stdio.h>
#include <ssdef.h>
#include <descrip.h>

main(){
  $DESCRIPTOR(imgname,"/bin/ls");
  $DESCRIPTOR(prcname,"ls");
  int pid;

  int RC = sys$creprc(&pid, &imgname,0,0,0,
		      0, 0, &prcname, 24, 0,
		      0, 0, 0, 0, 0);

  printf("creprc pid %x returned %x\n",pid,RC);

  return SS$_NORMAL;
}
