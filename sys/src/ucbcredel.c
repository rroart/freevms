// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<ssdef.h>
#include<misc.h>
#include<crbdef.h>

#include<linux/vmalloc.h>
#include<ucbdef.h>
#include<ddbdef.h>
#include<ssdef.h>

int ioc_std$link_ucb(struct _ucb * u) {
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

int ioc_std$clone_ucb(struct return_values * r, struct _ucb * u) {
  int status;
  struct _ucb * c=vmalloc(sizeof(struct _ucb));
  memcpy(c,u,sizeof(struct _ucb));

  qhead_init(&c->ucb$l_fqfl);
  c->ucb$l_fr3=0;
  c->ucb$l_fr4=0;
  c->ucb$l_fpc=0;
  c->ucb$w_bufquo=0;
  // c->ucb$l_link=something
  qhead_init(&c->ucb$l_ioqfl);
  c->ucb$l_devsts=0;
  c->ucb$l_opcnt=0;
  c->ucb$l_svapte=0;
  c->ucb$l_boff=0;
  c->ucb$l_bcnt=0;

  c->ucb$l_refc=1;
  c->ucb$l_sts|=UCB$M_ONLINE;
  c->ucb$l_sts&=~UCB$M_TEMPLATE;
  // c->ucb$l_charge

 again:
  if (++u->ucb$w_unit_seed>9999) u->ucb$w_unit_seed=1;
  c->ucb$w_unit=u->ucb$w_unit_seed;
  status=ioc_std$link_ucb(c);
  if (status!=SS$_NORMAL) goto again; // original weakness?

  r->val1=c;
  return SS$_NORMAL;
};
