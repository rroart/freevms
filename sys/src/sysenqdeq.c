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

extern struct _rsb * reshashtbl[];
extern unsigned long lockidtbl[];

int reshash(struct dsc$descriptor * d) {
  return d->dsc$w_length;
}

struct _rsb * find_reshashtbl(struct dsc$descriptor * d) {
  struct _rsb * tmp, *head=reshashtbl[reshash(d)*2];
  tmp=head->rsb$l_hshchn;
  while (tmp!=head && strncmp(tmp->rsb$t_resnam,d->dsc$a_pointer,d->dsc$w_length)) {
    tmp=tmp->rsb$l_hshchn;
  }
  if (strncmp(tmp->rsb$t_resnam,d->dsc$a_pointer,d->dsc$w_length)) 
    return 0;
  else
    return tmp;
}

void insert_reshashtbl(struct _rsb *r) {
  struct dsc$descriptor d;
  d.dsc$w_length=r->rsb$b_rsnlen;
  d.dsc$a_pointer=r->rsb$t_resnam;
  insque(r,&reshashtbl[2*reshash(&d)]);
}

int insert_lck(struct _lkb * l) {
  int i;
  for(i=0;i<LOCKIDTBL && (lockidtbl[i]&0xffff0000);i++) ;
  lockidtbl[i]=l;
  /* index pointer not implemented yet */
  return i;
}

asmlinkage int exe$enqw(struct struct_enq * s) {}

asmlinkage int exe$enq(struct struct_enq * s) {
  int convert;

  convert=s->flags&LCK$M_CONVERT;
  if (s->lkmode>LCK$K_EXMODE) return SS$_BADPARAM;
  if (!convert) {
    /* new lock */
    struct _rsb * r;
    struct _rsb * old;
    struct _lkb * l;
    struct dsc$descriptor * d;
    int sserror;

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

    if (s->lkmode!=LCK$K_NLMODE) {
      sserror=SS$_UNSUPPORTED;
      goto error;
    }

    r=vmalloc(sizeof(struct _rsb));
    bzero(r,sizeof(struct _rsb));
    l=vmalloc(sizeof(struct _lkb));

    bzero(l,sizeof(struct _lkb));
    l->lkb$b_efn=s->efn;
    l->lkb$b_rqmode=s->lkmode;
    l->lkb$l_cplastadr=s->astadr;
    l->lkb$l_pid=current->pid;
    strncpy(r->rsb$t_resnam,d->dsc$a_pointer,d->dsc$w_length);

    if (s->flags&LCK$M_SYSTEM) {
      /* priv checks */
    } else {
	
    }
    if (s->parid) {
      
    }

    old=find_reshashtbl(d);
    if (!old) {
      qhead_init(&r->rsb$l_grqfl);
      qhead_init(&r->rsb$l_cvtqfl);
      qhead_init(&r->rsb$l_wtqfl);
      insque(&l->lkb$q_sqfl,r->rsb$l_grqfl);
      l->lkb$l_rsb=r;
      insert_reshashtbl(r);
      if (!s->parid) {
	insque(&r->rsb$l_rrsfl,lck$gl_rrsfl);
	r->rsb$b_depth=0;
	r->rsb$l_rtrsb=r;
      }
    } else {
      /* something else? */
      vfree(r);
      r=old;
      if (aqempty(r->rsb$l_cvtqfl) && aqempty(r->rsb$l_wtqfl)) {
	insque(l->lkb$q_sqfl,r->rsb$l_grqfl);
	l->lkb$b_grmode=s->lkmode;
      } else {
	insque(l->lkb$q_sqfl,r->rsb$l_wtqfl);
      }
    }
    s->lksb->lksb$l_lkid=insert_lck(l);
    s->lksb->lksb$w_status=SS$_NORMAL;

    /* raise ipl */
    return SS$_NORMAL;
  error:
    /* ipl back */
    vfree(r);
    vfree(l);
    return sserror;
    
  } else {
    /* convert */
    struct _lkb * l;
    struct _rsb * r;
    void * dummy;
    l=lockidtbl[s->lksb->lksb$l_lkid];
    r=l->lkb$l_rsb;
    remque(r->rsb$l_grqfl,dummy);
    if (aqempty(r->rsb$l_cvtqfl) && aqempty(r->rsb$l_wtqfl)) {
      r->rsb$b_cgmode=s->lkmode;
      l->lkb$b_grmode=s->lkmode;
      insque(l,r->rsb$l_grqfl);
    }
  }
}

asmlinkage int exe$deq(unsigned int lkid, void *valblk, unsigned int acmode, unsigned int flags) {
  
}
