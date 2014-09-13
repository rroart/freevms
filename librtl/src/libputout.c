// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <ssdef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <starlet.h>

#include <stdlib.h>
#include <strings.h>

static struct _fabdef * output_fab = 0;
static struct _rabdef * output_rab = 0;

static const char * output = "SYS$OUTPUT";

int lib$put_output(void * v) {
  struct dsc$descriptor * dsc = v;
  struct _fabdef * fab;
  struct _rabdef * rab;

  if (output_rab == 0) {
    fab=malloc(sizeof(struct _fabdef));
    rab=malloc(sizeof(struct _rabdef));
    bzero(fab,sizeof(struct _fabdef));
    bzero(rab,sizeof(struct _rabdef));
    *fab = cc$rms_fab;
    *rab = cc$rms_rab;
    fab->fab$b_fac |= FAB$M_PUT;
    fab->fab$l_fna = output;
    fab->fab$b_fns = 10;
    rab->rab$l_fab = fab;
    sys$create(fab,0,0);
    sys$connect(rab,0,0);
    output_fab = fab;
    output_rab = rab;
  } else {
    fab = output_fab;
    rab = output_rab;
    sys$connect(rab,0,0); // check. fix later. should not really be here
  }

  rab->rab$l_rbf = dsc->dsc$a_pointer ;
  rab->rab$w_rsz = dsc->dsc$w_length ;

  sys$put(rab,0,0);

  return SS$_NORMAL;
}
