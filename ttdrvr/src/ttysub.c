// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <devdef.h>
#include <ssdef.h>
#include <ucbdef.h>
#include <ttyvecdef.h>
#include <ttyucbdef.h>

#include <ttytadef.h> // not here according to manual
#include <linux/mm.h> // need to do above

int con_redirect(int x) {
  if (x==0x0501)
    return 0x400;
  else
    return x;
}

tty$setup_ucb(struct _ucb * u){
  struct _tty_ucb * tty=u;
  struct _ltrm_ucb * lt=u;
  if (tty->ucb$l_tt_logucb==0) {
    tty->ucb$l_tt_logucb=u;
    lt->ucb$l_tl_phyucb=u; // maybe this one, too?
    u->ucb$l_devchar=DEV$M_AVL;
    u->ucb$l_devchar2&=~DEV$M_RED;
    tty->ucb$w_tt_cursor=1;
    tty->ucb$w_tt_hold=0;
    tty->ucb$w_tt_speed=tty->ucb$w_tt_despee;
    tty->ucb$b_tt_parity=tty->ucb$b_tt_depari;
    u->ucb$b_devtype=tty->ucb$b_tt_detype;

    // fork stuff not implemented
    qhead_init(&tty->ucb$l_tt_wflink);
    // break stuff not implemented
    // passall stuff not implemented
    // dma dev char not implemented
    // not setting ucb$l_tt_rtimou either

  }
  return;
}

tty$ds_tran(){
printk("tty$ds_tran not impl\n");
}

tty$readerror(int R3, struct _ucb * u){
  struct _tty_ucb * tty=u;
  //  tty->ucb$b_tt_outype=0;
printk("tty$readerror not impl\n");
}

tty$class_disconnect(){
printk("tty$class_disconnect not impl\n");
}

tty$class_fork(){
printk("tty$class_fork not impl\n");
}

tty$powerfail(struct _ucb * u){
  u->ucb$l_sts&=~UCB$M_INT;
  u->ucb$l_sts|=UCB$M_TIM;
  u->ucb$l_duetim=0;
  printk("tty$powerfail\n");
}

