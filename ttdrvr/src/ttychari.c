#include <descrip.h>
#include <ddbdef.h>
#include <ttdef.h>
#include <tt2def.h>
#include <ttyucbdef.h>
#include <ttytadef.h>
#include <ucbdef.h>
#include <system_data_cells.h>

#include <linux/mm.h>

int tty$putnextchar(int * chr, struct _ucb * u) {
  struct _tty_ucb * tty=u;
  struct _tt_type_ahd * ahd = tty->ucb$l_tt_typahd;

  if (tty->tty$v_st_read==0) {
    if (u->ucb$l_devdepend&TT$M_NOTYPEAHD) {
      tty->ucb$b_tt_outype=0;
      return;
    }

    // if owned then insert
    
    // if owned and unsolicited then notify owner (jobctl_unsolicit)

    if (u->ucb$l_devdepnd2&TT2$M_AUTOBAUD) {
      // not ready for this yet
    }

    if (u->ucb$l_devdepnd2&TT2$M_SECURE) {
      goto out;
    }

    // don't know how standard terminator is defined, so skip it?
    // skip login allow check, don't know that one either

    struct dsc$descriptor termd;
    char term[8];
    termd.dsc$a_pointer=term;
    memcpy(term,&u->ucb$l_ddb->ddb$t_name[1],3);
    sprintf(&term[3],"%d",u->ucb$w_unit);
    termd.dsc$w_length=strlen(term);

#if 0
    if (u->ucb$l_ddb->ddb$t_name[1]=='t' && u->ucb$l_ddb->ddb$t_name[2]=='z') {
      // temp workaround ?
      term[0]='p';
      term[1]='n';
    }
#endif

    jobctl_unsolicit(&termd);

  }

 out:

  if (tty->ucb$l_tt_typahd==0) {
    tty->ucb$l_tt_typahd = kmalloc (sizeof (struct _tt_type_ahd),GFP_KERNEL);
    struct _tt_type_ahd * ahd = tty->ucb$l_tt_typahd;
    ahd->tty$l_ta_data=kmalloc(1024,GFP_KERNEL);
    ahd->tty$l_ta_get=ahd->tty$l_ta_put=ahd->tty$l_ta_data;
    ahd->tty$l_ta_end=(long)ahd->tty$l_ta_data+1024;
    // the book says this should fork and dismiss character
    tty->ucb$b_tt_outype=0;
    return;
  }

  tty->ucb$b_tt_outype=1;
  char * c=ahd->tty$l_ta_put;
  *c=(char)*chr;
  ahd->tty$l_ta_put=(long)ahd->tty$l_ta_put+1;
  ahd->tty$w_ta_inahd++;
  if (ahd->tty$l_ta_put>=ahd->tty$l_ta_end)
    ahd->tty$l_ta_put=ahd->tty$l_ta_data;

  int cc;
  tty$getnextchar(chr,&cc,u); // manual said echoing is needed, too

  return 1;
}
