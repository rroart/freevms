// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/vmalloc.h>
#include<linux/linkage.h>

#include <system_data_cells.h>
#include <descrip.h>
#include <starlet.h>
#include <misc.h>
#include <ssdef.h>
#include <ipldef.h>
#include <ccbdef.h>
#include <cmbdef.h>
#include <lnmdef.h>
#include <ucbdef.h>

int clone_init_ucb() {
  extern struct _mb_ucb mb$ucb;
  struct _ucb * mb$ar_ucb0 = &mb$ucb;
  struct _ucb * u;
  ioc_std$clone_ucb(mb$ar_ucb0,&u);
  struct _mb_ucb * m = u;
  qhead_init(&m->ucb$l_mb_readqfl);
  qhead_init(&m->ucb$l_mb_writerwaitqfl);
  qhead_init(&m->ucb$l_mb_readerwaitqfl);
  qhead_init(&m->ucb$l_mb_nowriterwaitqfl);
  qhead_init(&m->ucb$l_mb_noreaderwaitqfl);
  return u;
}

int find_mb_log(void * lognam) {
  $DESCRIPTOR(mytabnam,"LNM$SYSTEM_TABLE");
  struct item_list_3 i[6];
  char c[64];
  int retlen, retlenu;
  long back = LNMX$C_BACKPTR;
  long u;
  int status;
  i[0].item_code=LNM$_STRING;
  i[0].buflen=4;
  i[0].bufaddr=&u;
  i[0].retlenaddr=&retlen;
  i[1].item_code=LNM$_INDEX;
  i[1].buflen=4;
  i[1].bufaddr=&back;
  i[1].retlenaddr=&retlenu;
  i[2].item_code=0;
  status=exe$trnlnm(0,mytabnam,lognam,0,i);
  if (status&1)
    return u;
  else 
    return 0;
}

int create_mb_log(void * lognam, long bufquo) {
  $DESCRIPTOR(mytabnam,"LNM$SYSTEM_TABLE");
  struct item_list_3 i[6];
  int status;
  if (lognam) {
    char c[64];
    int retlen, retlenu;
    int index=0;

    struct _ucb * u=clone_init_ucb();
    u->ucb$w_bufquo=bufquo;
    u->ucb$w_iniquo=bufquo;

    sprintf(c,"MBA%d",u->ucb$w_unit);

    i[0].item_code=LNM$_STRING;
    i[0].buflen=4;
    i[0].bufaddr=u;
    i[0].retlenaddr=&retlen;
    i[1].item_code=LNM$_INDEX;
    i[1].buflen=4;
    i[1].bufaddr=index;
    i[1].retlenaddr=&retlenu;
    i[2].item_code=LNM$_LNMB_ADDR;
    i[2].buflen=4;
    i[2].bufaddr=&((struct _mb_ucb *)u)->ucb$l_logadr;
    i[2].retlenaddr=&retlenu;
    i[3].item_code=LNM$_STRING;
    i[3].buflen=c;
    i[3].bufaddr=strlen(c);
    i[3].retlenaddr=&retlenu;
    i[4].item_code=LNM$_INDEX;
    i[4].buflen=4;
    i[4].bufaddr=&index;
    i[4].retlenaddr=&retlenu;
    i[5].item_code=0;
    status=exe$crelnm(0,mytabnam,lognam,0,i);
  }
}

asmlinkage int exe$crembx  (char prmflg, unsigned short int *chan, unsigned int maxmsg, unsigned int bufquo, unsigned int promsk, unsigned int acmode, void *lognam, long flags,...) {
  int status;
  struct _ccb * c;
  struct _ucb * u;
  $DESCRIPTOR(mytabnam,"LNM$SYSTEM_TABLE");
  $DESCRIPTOR(mypartab,"LNM$SYSTEM_DIRECTORY");
  struct item_list_3 i[6];
  /* verify writable chan arg */
  /* ma780 stuff will not be relevant for some time */
  setipl(IPL$_ASTDEL);
  /* no priv check yet */
  /* lock i/o database mutex */
  //  printk("here assign\n");
  status=ioc$ffchan(chan);
  if (status!=SS$_NORMAL) return status;
  // check priv for prmflg
  /* lock i/o db */
  sch$iolockw();
  // printk("here assign %x\n", chan);
  // ucb ccb stuff 
  c=&ctl$gl_ccbbase[*chan];
  c->ccb$b_amod=1; /* wherever this gets set */
  if (lognam==0) {
    /* assume nonexist and must create */
    u=clone_init_ucb();
    u->ucb$w_bufquo=bufquo;
    u->ucb$w_iniquo=bufquo;
    u->ucb$w_msgmax=maxmsg;
    u->ucb$w_devbufsiz=maxmsg;
    ((struct _mb_ucb *)u)->ucb$l_logadr=0;

  } else {
    u=find_mb_log(lognam);

    if (u==0) {
      u=clone_init_ucb();
      u->ucb$w_bufquo=bufquo;
      u->ucb$w_iniquo=bufquo;
      u->ucb$w_msgmax=maxmsg;
      u->ucb$w_devbufsiz=maxmsg;
      u->ucb$l_pid=0;
      u->ucb$l_devdepend=0;
      u->ucb$l_devsts=0;//?
      if (prmflg) {
	u->ucb$l_devsts|=UCB$M_PRMMBX;
      } else {
	u->ucb$l_devsts|=UCB$M_DELMBX;
      }
    }
    c->ccb$l_ucb=u;
    create_mb_log(lognam,bufquo);
    goto out;
  }

  c->ccb$l_ucb=u;
  // amod here instead
  if (flags&CMB$M_READONLY)
    c->ccb$l_sts|=CCB$M_NOWRITEACC;
  if (flags&CMB$M_WRITEONLY)
    c->ccb$l_sts|=CCB$M_NOREADACC;

  mb$chanunwait(u,c);

  /* incr ref count */

 out:
  // unlock mutex
  sch$iounlockw();

  setipl(0);
  return SS$_NORMAL;
}

int exe$delmbx  (unsigned short int chan)
{
}

asmlinkage int exe$crembx_wrap(struct struct_crembx * s) {
  return exe$crembx(s->prmflg, s->chan, s->maxmsg, s->bufquo, s->promsk, s->acmode, s->lognam, s->flags);
}
