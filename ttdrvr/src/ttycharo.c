#include <ttyucbdef.h>
#include <ttytadef.h>
#include <system_data_cells.h>

int tty$getnextchar(int * chr, int * CC, struct _ucb * u) {
  *CC=0;
  *chr=0;
  struct _tty_ucb * tty=u;
  tty->ucb$b_tt_outype=0;
  tty->tty$v_st_read==1; // mark reader?
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

}
