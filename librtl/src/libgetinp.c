#include <descrip.h>
#include <ssdef.h>
#include <fabdef.h>
#include <rabdef.h>
#include <starlet.h>

static struct _fabdef * input_fab = 0;
static struct _rabdef * input_rab = 0;

static const char * input = "SYS$INPUT";

int lib$get_input(void * v) {
  struct dsc$descriptor * dsc = v;
  struct _fabdef * fab;
  struct _rabdef * rab;

  if (input_rab ==0) {
    fab=malloc(sizeof(struct _fabdef));
    rab=malloc(sizeof(struct _rabdef));
    bzero(fab,sizeof(struct _fabdef));
    bzero(rab,sizeof(struct _rabdef));
    *fab = cc$rms_fab;
    *rab = cc$rms_rab;
    fab->fab$b_fac |= FAB$M_GET;
    fab->fab$l_fna = input;
    fab->fab$b_fns = 9;
    rab->rab$l_fab = fab;
    sys$create(fab,0,0);
    sys$connect(rab,0,0);
  } else {
    fab = input_fab;
    rab = input_rab;
  }

  rab->rab$l_ubf = dsc->dsc$a_pointer ;
  rab->rab$w_usz = dsc->dsc$w_length ;

  sys$get(rab,0,0);

  return SS$_NORMAL;
}
