// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<starlet.h>
#include<iodef.h>
#include<ssdef.h>
#include<descrip.h>
#include<misc.h>
#include<irpdef.h>
#include<ucbdef.h>
#include<ddbdef.h>
#include<ccbdef.h>
#include<lnmdef.h>
#include<sbdef.h>
#include<system_data_cells.h>
#include<sysgen.h>
#include<linux/vmalloc.h>
#include<linux/linkage.h>

int ioc$ffchan(unsigned short int *chan) {
  unsigned short int i;
  for (i=0; i<CHANNELCNT && ctl$gl_ccbbase[i].ccb$b_amod; i++) ;
  if (ctl$gl_ccbbase[i].ccb$b_amod)
    return SS$_NOIOCHAN;
  else {
    *chan=i;
    return SS$_NORMAL;
  }
}

int ioc$searchdev(struct return_values *r, void * devnam) {
  /* implement exact later */
  return ioc$search(r,devnam);
}

extern struct _sb othersb;

// remember to make this interface like ioc_std$search in ioc_routines.h

int ioc$search(struct return_values *r, void * devnam) {
  // no use of ioc$trandevnam yet 
  char out[255];
  int outlen;
  char * outagain;
  memset(out,0,255); // suspect outlen is not quite operative
  int sts = ioc_std$trandevnam(devnam, 0, out, outlen, &outagain);
  /* ddb d not needed? */
  /* real device, no logical. do not have logicals yet */
  /* return ucb or 0 */
  struct dsc$descriptor *s=devnam;
  struct _ddb * d=ioc$gl_devlist;
  char * node = 0;
  int nodelen = 0;
  char * devstr;
  int devstrlen;
  if ((sts&1)==1) {
    devstr=out;
    devstrlen=outlen; // not quite there yet?
    devstrlen=strlen(out);
  } else {
    devstr=s->dsc$a_pointer;
    devstrlen=s->dsc$w_length;
  }
  char * device = strchr(devstr,'$');
  if (device>=(devstr+devstrlen))
    device=0;
  if (device) {
    node=devstr;
    nodelen=device-node;
    device++;
  } else {
    device=devstr;
  }
  {
    // temp downcase?
    int i;
    for(i=0;i<3;i++) {
      if (device[i]>='A' && device[i]<='Z') device[i]|=0x20;
    }
  }
  do {
    //    printk("bcmp %s %s\n",d->ddb$t_name,s->dsc$a_pointer);
    struct _sb * sb=d->ddb$ps_sb;
    if ((node==0) && sb)
      goto next;
    if (sb && 0!=strncmp(&sb->sb$t_nodename[1],node,nodelen))
      goto next;
    if (!bcmp(&d->ddb$t_name[1],device,3/* was s->dsc$w_length*/)) {
      struct _ucb * tmp = d->ddb$l_ucb;
      char * c=device;
      char unit=c[3]-48;
      do {
	//printk("unitcmp %x %x\n",unit,tmp->ucb$w_unit);
	if (unit==tmp->ucb$w_unit)
	  goto out;
	tmp=tmp->ucb$l_link;
      } while (tmp && tmp!=d->ddb$l_ucb);
      return SS$_NOSUCHDEV;
    out:
      r->val1=tmp; // was d->ddb$l_ucb;
      return SS$_NORMAL;
    }
  next:
    d=d->ddb$ps_link;
  } while (d && d!=ioc$gl_devlist);
  return SS$_NOSUCHDEV;
}

int ioc$verify_chan(unsigned short int chan, struct _ccb ** ccbp) {
  struct _ccb * ccb=&ctl$gl_ccbbase[chan];
  // also check channel num and access mode
  if (ccb->ccb$b_amod==1) {
    *ccbp=ccb;
    return SS$_NORMAL;
  } else 
    return 0;
}

int ioc_std$trandevnam (void * descr_p, int flags, char *buf, int *outlen, void **out_p) {
  int sts;
  $DESCRIPTOR(mytabnam_, "LNM$SYSTEM_TABLE");
  struct dsc$descriptor * mytabnam = &mytabnam_;
  struct item_list_3 itm[2];
  itm[0].item_code=LNM$_STRING;
  itm[0].buflen=4;
  itm[0].retlenaddr=outlen;
  itm[0].bufaddr=buf;
  bzero(&itm[1],sizeof(struct item_list_3));
  *out_p=buf;
  sts=exe$trnlnm(0,mytabnam,descr_p,0,itm);
  return sts;
}

int ioc_std$search (void * descr_p, int flags, void *lock_val_p, struct _ucb **ucb_p, struct _ddb **ddb_p, struct _sb **sb_p) {

}

int ioc_std$searchdev (void * descr_p, struct _ucb **ucb_p, struct _ddb **ddb_p, struct _sb **sb_p) {

}

