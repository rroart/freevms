// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/unistd.h>
#include<linux/linkage.h>
#include<linux/sched.h>
#include <linux/vmalloc.h>
#include<ssdef.h>
#include<starlet.h>
#include<cebdef.h>
#include<pridef.h>
#include<statedef.h>
#include<evtdef.h>
#include<ipldef.h>
#include<descrip.h>
#include <system_data_cells.h>
#include <internals.h>

// Author. Roar Thronæs.

asmlinkage int exe$ascefc(unsigned int efn, void *name, char prot, char perm) {
  struct _ceb * first=&sch$gq_cebhd;
  struct _ceb * tmp=first->ceb$l_cebfl;
  struct _ceb * c;
  struct _pcb * p;
  unsigned long * efcp;
  int found=1;
  if (efn<64 || efn >127) 
    return SS$_ILLEFC;
  if (((struct dsc$descriptor *)name)->dsc$w_length==0)
    return SS$_IVLOGNAM;
  if (((struct dsc$descriptor *)name)->dsc$w_length>15)
    return SS$_IVLOGNAM;
  /* lock ceb mutex */
  while (tmp!=first) {
    //    int res;
    //printk("%x %x %x %x %x %x \n",((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1], strncmp(((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1],tmp->ceb$t_efcnam[0]),((struct dsc$descriptor *)name)->dsc$w_length,tmp->ceb$t_efcnam[0],((struct dsc$descriptor *)name)->dsc$w_length==tmp->ceb$t_efcnam[0]);
    // res=strncmp(((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1],tmp->ceb$t_efcnam[0]);
    if (((struct dsc$descriptor *)name)->dsc$w_length==tmp->ceb$t_efcnam[0] && strncmp(((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1],tmp->ceb$t_efcnam[0])==0) goto out;
    tmp=tmp->ceb$l_cebfl;
    //res=strncmp(((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1],tmp->ceb$t_efcnam[0]);
  }
  found=0;
 out:
  if (!found) {
    c=vmalloc(sizeof(struct _ceb));
    bzero(c,sizeof(struct _ceb));
    qhead_init(&c->ceb$l_wqfl);
    c->ceb$l_state=SCH$C_CEF;
    c->ceb$t_efcnam[0]=((struct dsc$descriptor *)name)->dsc$w_length;
    bcopy(((struct dsc$descriptor *)name)->dsc$a_pointer,&c->ceb$t_efcnam[1],c->ceb$t_efcnam[0]);
    insque(c,first);
    tmp=c;
  }
  tmp->ceb$l_refc++; // do a adawi?
  p=current;
  efcp=getefcp(p,efn);
  *efcp=tmp;
  /* unlock mutex */
}

asmlinkage int exe$dacefc(unsigned int efn) {
  unsigned long * efcp;
  struct _pcb * p;
  int retval;
  struct _ceb * c;
  if (efn<64 || efn >127) 
    return SS$_ILLEFC;
  /* lock ceb mutex */
  p=current;
  efcp=getefcp(p,efn);
  if (!(*efcp)) {
    retval=SS$_ILLEFC;
    goto end;
  }
  c=*efcp;
  *efcp=0;
  c->ceb$l_refc--;
  /* if temporary etc and ... */
  if (!c->ceb$l_refc && aqempty(c->ceb$l_wqfl)) {
    remque(c,c);
    vfree(c);
  }
 end:
  /* unlock ceb mutex */
  return retval;
}

asmlinkage int exe$dlcefc(void *name) {
  struct _ceb * first=((struct _ceb *)sch$gq_cebhd)->ceb$l_cebfl;
  struct _ceb * tmp=sch$gq_cebhd;
  struct _ceb * c;
  int retval;
  int found=1;
  /* lock ceb mutex */
  while (tmp!=first) {
    if (((struct dsc$descriptor *)name)->dsc$w_length==tmp->ceb$t_efcnam[0] && !strncmp(((struct dsc$descriptor *)name)->dsc$a_pointer,&tmp->ceb$t_efcnam[1],tmp->ceb$t_efcnam[0])) goto out;
    tmp=tmp->ceb$l_cebfl;
  }
  found=0;
 out:

  if (!found) 
    goto end;
  c=tmp;

  /* some unsupported stuff */

  /* overlaps with dacefc. copy. */

  /* if temporary etc and ... */
  if (!c->ceb$l_refc && aqempty(c->ceb$l_wqfl)) {
    remque(c,c);
    vfree(c);
  }

  retval=SS$_NORMAL;

 end:
  {
  }
  /* unlock ceb mutex */
}

