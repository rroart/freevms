#include <ttyucbdef.h>
#include <ttytadef.h>
#include <system_data_cells.h>

int tty$getnextchar(int * chr, int * CC, struct _ucb * u) {
  struct _tty_ucb * tty=u;
  struct _tt_type_ahd * ahd = tty->ucb$l_tt_typahd;
  *CC=0;
  *chr=0;
  tty->ucb$b_tt_outype=0;

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
