// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/vmalloc.h>
#include<asm/current.h>
#include<system_data_cells.h>
#include<lckdef.h>
#include<lckctxdef.h>
#include<misc.h>
#include<ssdef.h>
#include<starlet.h>
#include<ipldef.h>
#include<rsbdef.h>
#include<lkbdef.h>
#include<pridef.h>
#include<descrip.h>

// no vmslock etc here yet

unsigned char lck$ar_compat_tbl[6]={0x3f,0x1f,0x7,0xb,3,1};
unsigned char lck$ar_compat_tbl2[6,6]={
  {1,1,1,1,1,1},
  {0,1,1,1,1,1},
  {0,0,0,1,1,1},
  {0,0,1,0,1,1},
  {0,0,0,0,1,1},
  {0,0,0,0,0,1}
};

unsigned char lck$synccvt_tbl[6]={1,3,7,0xb,0x1f,0x3f};

unsigned char lck$quecvt_tbl[6]={0x3e,0x3c,0x38,0x34,0x20,0};

unsigned char lck$ggmax_tbl[6]={0,0,8,4,0,0};

unsigned char lck$ggred_tbl[6]={0,0,0,0,0xc,0};

unsigned char lck$valblk_tbl[6]={0,0,0,0,0x1f,0x3f};

unsigned char lck$al_dwncvt_tbl[6]={0,1,3,3,0xf,0x1f};



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

asmlinkage int exe$enqw(struct struct_enq * s) {
  int status=exe$enq(s);
  if ((status&1)==0) return status;
  return exe$synch(s->efn,s->lksb);
}

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

    setipl(IPL$_ASTDEL);

    r=vmalloc(sizeof(struct _rsb));
    bzero(r,sizeof(struct _rsb));
    l=vmalloc(sizeof(struct _lkb));

    bzero(l,sizeof(struct _lkb));
    l->lkb$b_efn=s->efn;
    l->lkb$b_rqmode=s->lkmode;
    l->lkb$l_cplastadr=s->astadr;
    l->lkb$l_blkastadr=s->blkastadr;
    l->lkb$l_astprm=s->astprm;
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
      insque(&l->lkb$l_sqfl,r->rsb$l_grqfl);
      l->lkb$l_rsb=r;
      insert_reshashtbl(r);
      if (!s->parid) {
	insque(&r->rsb$l_rrsfl,lck$gl_rrsfl);
	qhead_init(&r->rsb$l_srsfl);
	r->rsb$b_depth=0;
	r->rsb$l_rtrsb=r;
	insque(&l->lkb$l_ownqfl,&current->pcb$l_lockqfl);
	//?if (q->flags & LKB$M_DCPLAST) 
	
	s->lksb->lksb$l_lkid=insert_lck(l);
	s->lksb->lksb$w_status=SS$_NORMAL;
	
	lck$grant_lock(l,r,-1,s->lkmode);

	goto end;
      }
    } else {
      /* something else? */
      vfree(r);
      r=old;
      if (aqempty(r->rsb$l_cvtqfl) && aqempty(r->rsb$l_wtqfl)) {
	insque(l->lkb$l_sqfl,r->rsb$l_grqfl);
	l->lkb$b_grmode=s->lkmode;
      } else {
	insque(l->lkb$l_sqfl,r->rsb$l_wtqfl);
      }

    s->lksb->lksb$l_lkid=insert_lck(l);
    s->lksb->lksb$w_status=SS$_NORMAL;

    }
  end:
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
    remque(l->lkb$l_sqfl,l->lkb$l_sqfl);// ?
    remque(r->rsb$l_grqfl,dummy);
    if (aqempty(r->rsb$l_cvtqfl) && aqempty(r->rsb$l_grqfl)) {
      lck$grant_lock(l,r,l->lkb$b_grmode,s->lkmode);
      return SS$_NORMAL;
    } else {
      if (r->rsb$b_cgmode!=l->lkb$b_grmode) {
	if (test_bit(s->lkmode,lck$ar_compat_tbl[l->lkb$b_grmode])) {
	  lck$grant_lock(l,r,l->lkb$b_grmode,s->lkmode);
	}
      } else {
	int newmode=find_highest(l,r);
	if (test_bit(newmode,lck$ar_compat_tbl[l->lkb$b_grmode])) {
	  r->rsb$b_fgmode=newmode;
	  r->rsb$b_ggmode=newmode;
	  r->rsb$b_cgmode=newmode;
	  lck$grant_lock(l,r,l->lkb$b_grmode,newmode);
	}
      }
    }
  }
}

asmlinkage int exe$deq(unsigned int lkid, void *valblk, unsigned int acmode, unsigned int flags) {
  
}

int lck$grant_lock(struct _lkb * l,struct _rsb * r, signed int curmode, signed int nextmode) {
  if (nextmode>curmode) {
    r->rsb$b_ggmode=nextmode;
    r->rsb$b_fgmode=nextmode;
    r->rsb$b_cgmode=nextmode;
  }
    l->lkb$b_grmode=nextmode;
    l->lkb$l_lkst1=SS$_NORMAL;

    if (l->lkb$l_blkastadr) {
      /* not implemented */
    }

    insque(l->lkb$l_sqfl,r->rsb$l_grqfl);
    l->lkb$b_state=LKB$K_GRANTED;

    sch$postef(current->pid,PRI$_RESAVL,l->lkb$b_efn);

    if (l->lkb$l_cplastadr && l->lkb$l_flags&LCK$M_SYNCSTS==0) {
      l->lkb$l_ast=l->lkb$l_cplastadr;
      sch$qast(current->pid,PRI$_RESAVL,l);
    }
    if (l->lkb$l_ast) {
      /* not implemented */
    }

    sch$qast(current->pid,PRI$_RESAVL,l);
    //if (current->pcb$w_state!=SCH$C_CUR)
    sch$postef(current->pid,PRI$_RESAVL,l->lkb$b_efn);

}

int find_highest(struct _lkb * l, struct _rsb * skip) {
  struct _lkb * first=&l->lkb$l_sqfl;
  struct _rsb * skipme=&skip->rsb$l_grqfl;
  struct _lkb * tmp=first->lkb$l_astqfl;
  int high=0;
  while (tmp!=first) {
    if (tmp!=skipme)
      if (l->lkb$b_grmode>high)
	high=l->lkb$b_grmode;
  }
  return high;
}
