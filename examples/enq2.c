#include <stdio.h>
#include <descrip.h>
#include <lckdef.h>
#include <starlet.h>
#include <lksbdef.h>

$DESCRIPTOR(resource,"STRUCTURE_1");

signed long long time1=-10000000;
signed long long time2=-20000000;
signed long long time3=-30000000;
signed long long time4=-40000000;
signed long long time10=-100000000;
signed long long time15=-150000000;
signed long long time20=-200000000;
signed long long time30=-300000000;

void blkast(int par) {
  printf("blkast %x %x\n",getpid(),par);
}

void ast(int par) {
  printf("ast %x %x\n",getpid(),par);
}

main(){
  struct _lksb lksb;
  unsigned int status, lkmode, flags;

  lkmode = LCK$K_NLMODE;

  flags = 0;//LCK$M_SYSTEM;

  status = sys$enqw(0,
		    lkmode,
		    &lksb,
		    flags,
		    &resource,
		    0, ast, 42, blkast, 0, 0);

  printf("main %x %x %x %x\n",getpid(),status,lksb.lksb$w_status,lksb.lksb$l_lkid);

  sleep (10);

  lkmode = LCK$K_PWMODE;
  flags = LCK$M_CONVERT;

  printf("a %x\n",getpid());

  status = sys$enqw(0,
		    lkmode,
		    &lksb,
		    flags,
		    0, 0, ast, 43, blkast, 0, 0);

  printf("a %x %x %x %x\n",getpid(),status,lksb.lksb$w_status,lksb.lksb$l_lkid)\
    ;

  sleep (10);

  lkmode = LCK$K_PRMODE;

  lkmode = LCK$K_PRMODE;
  flags = LCK$M_CONVERT;

  printf("b %x\n",getpid());

  status = sys$enqw(0,
		    lkmode,
		    &lksb,
		    flags,
		    0, 0, ast, 44, blkast, 0, 0);

  printf("b %x %x %x %x\n",getpid(),status,lksb.lksb$w_status,lksb.lksb$l_lkid)\
    ;

  sleep (10);

  status = sys$deq(lksb.lksb$l_lkid,0,0,0);

  printf("c %x %x %x %x\n",getpid(),status,lksb.lksb$w_status,lksb.lksb$l_lkid)\
    ;

  sleep(20);

}
