#include<linux/linkage.h>
#include<linux/vmalloc.h>
#include<asm/current.h>
#include"../../freevms/sys/src/system_data_cells.h"
#include"../../freevms/starlet/src/lckdef.h"
#include"../../freevms/starlet/src/misc.h"
#include"../../freevms/starlet/src/ssdef.h"
#include"../../freevms/starlet/src/starlet.h"
#include"../../freevms/lib/src/rsbdef.h"
#include"../../freevms/lib/src/lkbdef.h"
#include"../../freevms/librtl/src/descrip.h"

asmlinkage int exe$enq(struct struct_enq * s) {
  int convert;

  convert=s->flags&LCK$M_CONVERT;
  if (s->lkmode>LCK$K_EXMODE) return SS$_BADPARAM;
  if (!convert) {
    /* new lock */
    struct _rsb * r;
    struct _lkb * l;
    struct dsc$descriptor * d;
    int sserror;

    r=vmalloc(sizeof(struct _rsb));
    bzero(r,sizeof(struct _rsb));
    l=vmalloc(sizeof(struct _lkb));
    bzero(l,sizeof(struct _lkb));

    d=s->resnam;
    if (d->dsc$w_length==0 || d->dsc$w_length>RSB$K_MAXLEN) { 
      sserror=SS$_IVBUFLEN;
      goto error;
    }
    if (s->flags&LCK$M_EXPEDITE)
      if (s->lkmode!=LCK$K_NLMODE) {
	sserror=SS$_UNSUPPORTED;
	goto error;
      }
    l->lkb$b_efn=s->efn;
    l->lkb$b_rqmode=s->lkmode;
    l->lkb$l_cplastadr=s->astadr;
    l->lkb$l_pid=current->pid;
    strncpy(r->rsb$t_resnam,d->dsc$a_pointer,d->dsc$w_length);

    if (s->parid) {
      
    }

    /* raise ipl */
    return SS$_NORMAL;
  error:
    /* ipl back */
    vfree(r);
    vfree(l);
    return sserror;
    
  } else {

  }
}

asmlinkage int exe$deq(unsigned int lkid, void *valblk, unsigned int acmode, unsigned int flags) {

}
