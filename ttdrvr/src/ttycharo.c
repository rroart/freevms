// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <ttydef.h>
#include <ttyucbdef.h>
#include <ttytadef.h>
#include <system_data_cells.h>
#include <ttyrbdef.h>
#include <ssdef.h>
#include <irpdef.h>
#include <dyndef.h>

int tty$getnextchar(int * chr, int * CC, struct _ucb * u) {
 again:
  *CC=0;
  *chr=0;
  struct _tty_ucb * tty=u;
  struct _ltrm_ucb * lt=u;
  tty->ucb$b_tt_outype=0;
  if (tty->tty$v_st_write) {
    struct _twp * wb = tty->ucb$l_tt_wrtbuf;
    char * wrtc = wb->tty$l_wb_next;
    //    wb->tty$l_wb_next++;
    struct _irp * i = wb->tty$l_wb_irp;
    if (wb->tty$l_wb_next>=wb->tty$l_wb_end) { 
      // end of data
      i->irp$q_tt_state &= ~TTY$M_ST_WRITE;
      tty->ucb$q_tt_state &= ~TTY$M_ST_WRITE; // here?
      //not yet: fork 
      //wb->tty$l_wb_fpc=?;
      //exe_std$primitive_fork(wb,i,wb);
      struct _irp * temp_irp = lt->ucb$l_tl_phyucb->ucb$l_irp;
      // temp_irp is nonzero when telnetted. some other times too. fix later.
      if (temp_irp==0) 
	temp_irp=((struct _tty_ucb *)lt->ucb$l_tl_phyucb)->ucb$l_tt_wrtbuf->tty$l_wb_irp;
      // kfree(temp_irp->irp$l_svapte); // have to get rid of this first?
      // or maybe not? got double kfree?
      temp_irp->irp$l_svapte=0;
      temp_irp->irp$l_iost1=SS$_NORMAL;
      // int status = com$post(temp_irp,lt->ucb$l_tl_phyucb);
      // not working with duplex?      int status = ioc$reqcom(SS$_NORMAL,0,lt->ucb$l_tl_phyucb);
      goto again;
    } else {
      // more data
      char character = * wrtc;
      tty->ucb$b_tt_lastc = character;
      *CC=1;
      tty->ucb$b_tt_outype=1;
      wb->tty$l_wb_next++;
      //i->irp$q_tt_state &= ~TTY$M_ST_WRITE;
      *chr = character;
      return;
    }
    return; 
    // return  tty$putnextchar(chr,u); // this made a loop, implement some other way
  }
  tty->tty$v_st_read=1; // mark reader?
  if (tty->ucb$l_tt_typahd==0)
    return;
  struct _tt_type_ahd * ahd = tty->ucb$l_tt_typahd;

  if (ahd->tty$w_ta_inahd==0)
    return;

  *CC=1;
  tty->ucb$b_tt_outype=1;
  char * c=ahd->tty$l_ta_get;
  *chr=*c;
  ahd->tty$l_ta_get=(long)ahd->tty$l_ta_get+1;
  ahd->tty$w_ta_inahd--;

  if (ahd->tty$l_ta_get>=ahd->tty$l_ta_end)
    ahd->tty$l_ta_get=ahd->tty$l_ta_data;

  struct _tt_readbuf * bd = u->ucb$l_svapte;
  char * bd_txt = bd->tty$l_rb_txt;
  bd_txt[bd->tty$w_rb_txtoff]=*c;
  bd->tty$w_rb_txtoff++;

  con$startio((int)*chr, u, (int)*CC);

  // use tt_term etc instead?
  if (*c==13) {
    tty->tty$v_st_eol=1;
    ioc$reqcom(SS$_NORMAL,0,u);
  }

#if 0
  ioc$reqcom(SS$_NORMAL,0,u); // not needed here? bad place?
#endif

  return 1;

}
