#include <ttyucbdef.h>
#include <ttytadef.h>
#include <system_data_cells.h>

int tty$putnextchar(int * chr, struct _ucb * u) {
  struct _tty_ucb * tty=u;
  struct _tt_type_ahd * ahd = tty->ucb$l_tt_typahd;

  tty->ucb$b_tt_outype=1;
  char * c=ahd->tty$l_ta_put;
  *c=(char)*chr;
  ahd->tty$l_ta_put=(long)ahd->tty$l_ta_put+1;
  ahd->tty$w_ta_inahd++;

  if (ahd->tty$l_ta_put>=ahd->tty$l_ta_end)
    ahd->tty$l_ta_put=ahd->tty$l_ta_data;

  return 1;
}
