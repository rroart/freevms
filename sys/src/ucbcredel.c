// $Id$
// $Locker$

// Author. Roar Thron�s.

#include<ssdef.h>
#include<misc.h>
#include<crbdef.h>
#include<dyndef.h>

#include<linux/vmalloc.h>
#include<ucbdef.h>
#include<ddbdef.h>
#include<ssdef.h>

int ioc_std$link_ucb (struct _ucb *ucb) {
  struct _ucb * u = ucb;
  struct _ucb * next=u->ucb$l_ddb->ddb$l_ucb;
  struct _ucb * prev;
 again:
  prev=next;
  next=prev->ucb$l_link;
  if (next==0) goto notagain;
  if (next->ucb$w_unit<u->ucb$w_unit) goto again;
  if (next->ucb$w_unit==u->ucb$w_unit) return SS$_OPINCOMPL;
 notagain:

  prev->ucb$l_link=u;
  u->ucb$l_link=next;

  u->ucb$l_crb->crb$l_refc++;

  return SS$_NORMAL;
}

int ioc_std$copy_ucb (struct _ucb *src_ucb, struct _ucb **new_ucb) {
  int status;
  struct _ucb * u;
  int size;

  if (src_ucb->ucb$w_size) {
    size=src_ucb->ucb$w_size;
  } else {
    size=sizeof(struct _ucb);
    printk("ucb %x size zero\n");
  }
  u=kmalloc(size,GFP_KERNEL);
  memcpy(u,src_ucb,size);

  qhead_init(&u->ucb$l_fqfl);
  u->ucb$l_fr3=0;
  u->ucb$l_fr4=0;
  u->ucb$l_fpc=0;
  u->ucb$w_bufquo=0;
  // u->ucb$l_link=something
  qhead_init(&u->ucb$l_ioqfl);
  u->ucb$l_devsts=0;
  u->ucb$l_opcnt=0;
  u->ucb$l_svapte=0;
  u->ucb$l_boff=0;
  u->ucb$l_bcnt=0;

  u->ucb$l_refc=1;
  u->ucb$l_sts|=UCB$M_ONLINE;
  u->ucb$l_sts&=~UCB$M_TEMPLATE;
  // u->ucb$l_charge
  qhead_init(&u->ucb$l_mb_msgqfl); // probably this too?

  *new_ucb = u;
  return SS$_NORMAL;
};

int ioc_std$create_ucb (struct _pcb *pcb, struct _ucb *ucb, struct _ucb **new_ucb_p) {
  int status;
  //struct _ucb * u=kmalloc(sizeof(struct _ucb),GFP_KERNEL);
  //bzero(u,sizeof(struct _ucb));
  status=ioc_std$clone_ucb (ucb, new_ucb_p);
  return SS$_NORMAL;
}

int ioc_std$clone_ucb (struct _ucb *tmpl_ucb, struct _ucb **new_ucb) {
  int status;
  struct _ucb * u;
  status=ioc_std$copy_ucb(tmpl_ucb,new_ucb);
  u=*new_ucb;

  u->ucb$w_unit=tmpl_ucb->ucb$w_unit_seed;

 again:
  if (++tmpl_ucb->ucb$w_unit_seed>9999) tmpl_ucb->ucb$w_unit_seed=0;
  status=ioc_std$link_ucb(u);
  if (status!=SS$_NORMAL) goto again; // original weakness?


  return SS$_NORMAL;
}

int init_ddb(struct _ddb * ddb, struct _ddt * ddt, struct _ucb * ucb, char * sddb) {
  bzero(ddb,sizeof(struct _ddb));
  ddb->ddb$b_type=DYN$C_DDB;
  ddb->ddb$l_ddt=ddt;
  ddb->ddb$ps_ucb=ucb;
  bcopy(sddb,ddb->ddb$t_name,strlen(sddb));
  return SS$_NORMAL;
}

int init_ucb(struct _ucb * ucb, struct _ddb * ddb,struct _ddt * ddt, struct _crb *crb) {
  ucb->ucb$b_type=DYN$C_UCB;
  ucb->ucb$l_ddb=ddb;
  ucb->ucb$l_crb=crb;
  ucb->ucb$l_ddt=ddt;

  ucb->ucb$l_sts|=UCB$M_TEMPLATE;

  ucb->ucb$w_unit=9999;
  ucb->ucb$w_unit_seed=0;

  return SS$_NORMAL;
}

int init_crb(struct _crb * crb) {
  crb->crb$b_type=DYN$C_CRB;
  return SS$_NORMAL;
}

// temporary stuff for dua

int ioc_std$copy_mscp_ucb (struct _ucb *src_ucb, struct _ucb **new_ucb) {
  int status;
  struct _ucb * u=kmalloc(sizeof(struct _mscp_ucb),GFP_KERNEL);
  memcpy(u,src_ucb,sizeof(struct _mscp_ucb));

  qhead_init(&u->ucb$l_fqfl);
  u->ucb$l_fr3=0;
  u->ucb$l_fr4=0;
  u->ucb$l_fpc=0;
  u->ucb$w_bufquo=0;
  // u->ucb$l_link=something
  qhead_init(&u->ucb$l_ioqfl);
  u->ucb$l_devsts=0;
  u->ucb$l_opcnt=0;
  u->ucb$l_svapte=0;
  u->ucb$l_boff=0;
  u->ucb$l_bcnt=0;

  u->ucb$l_refc=1;
  u->ucb$l_sts|=UCB$M_ONLINE;
  u->ucb$l_sts&=~UCB$M_TEMPLATE;
  // u->ucb$l_charge
  qhead_init(&u->ucb$l_mb_msgqfl); // probably this too?

  *new_ucb = u;
  return SS$_NORMAL;
};

int ioc_std$clone_mscp_ucb (struct _ucb *tmpl_ucb, struct _ucb **new_ucb) {
  int status;
  struct _ucb * u;
  status=ioc_std$copy_mscp_ucb(tmpl_ucb,new_ucb);
  u=*new_ucb;

  u->ucb$w_unit=tmpl_ucb->ucb$w_unit_seed;

 again:
  if (++tmpl_ucb->ucb$w_unit_seed>9999) tmpl_ucb->ucb$w_unit_seed=0;
  status=ioc_std$link_ucb(u);
  if (status!=SS$_NORMAL) goto again; // original weakness?

  ((struct _mscp_ucb *)u)->ucb$w_mscpunit=u->ucb$w_unit;

  return SS$_NORMAL;
}

