// $Id$
// $Locker$

// Author. Roar Thronæs.
/**
   \file iosubpagd.c
   \brief QIO - TODO still more doc
   \author Roar Thronæs
*/

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
#include <ioc_routines.h>
#include<linux/vmalloc.h>
#include<linux/linkage.h>
#include <exe_routines.h>
#include <lnmsub.h>
#include <lnmstrdef.h>

/**
   \brief find first free i/o channel - see 5.2 21.5.2.1
   \param chan return value
*/

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

long int dsc$strtol(const char *nptr, int len, char **endptr, int base) {
  char str[len+1];
  memcpy(str, nptr, len);
  str[len]=0;
  //printk("str %d %x %s\n",len, str, str);
  return simple_strtol(str, endptr, base); 
}

// remember to make this interface like ioc_std$search in ioc_routines.h

/**
   \brief search for device - see 5.2 21.4.1
   \param r struct for return ddb etc
   \param devnam device name
*/

int ioc$search(struct return_values *r, void * devnam) {
  char out[255];
  int outlen;
  char * outagain;
  memset(out,0,255); // suspect outlen is not quite operative
  /** translate devnam argument to device */
  int sts = ioc_std$trandevnam(devnam, 0, out, &outlen, &outagain);
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
  /** search i/o db for name */
  do {
    //    printk("bcmp %s %s\n",d->ddb$t_name,s->dsc$a_pointer);
    struct _sb * sb=d->ddb$ps_sb;
    if ((node==0) && sb)
      goto next;
    if (node && (sb==0))
      goto next;
    if (sb && 0!=strncmp(&sb->sb$t_nodename[1],node,nodelen))
      goto next;
    if (!memcmp(&d->ddb$t_name[1],device,3/* was s->dsc$w_length*/)) {
      struct _ucb * tmp = d->ddb$ps_ucb;
      char * c=device;
#if 1
      char unit=dsc$strtol(&c[3], devstrlen-3-(device-devstr), 0, 10); // was: c[3]-48;
      // printk ("unit %s %d\n",devstr, unit);
#else
      char unit=c[3]-48;
#endif
      do {
	//printk("unitcmp %x %x\n",unit,tmp->ucb$w_unit);
	if (unit==tmp->ucb$w_unit)
	  goto out;
	tmp=tmp->ucb$l_link;
      } while (tmp && tmp!=d->ddb$ps_ucb);
      return SS$_NOSUCHDEV;
    out:
      r->val1=tmp; // was d->ddb$ps_ucb;
      return SS$_NORMAL;
      /** access check - MISSING */
      /** if cluster device, do ioc$lock_dev - MISSING */
    }
  next:
    d=d->ddb$ps_link;
  } while (d && d!=ioc$gl_devlist);
  return SS$_NOSUCHDEV;
}

/**
   \brief verify that the chann is legal - see 5.2 21.5.3
   \param chan channel
   \param ccbp return value
*/

int ioc$verify_chan(unsigned short int chan, struct _ccb ** ccbp) {
  struct _ccb * ccb=&ctl$gl_ccbbase[chan];
  // also check channel num and access mode
  /** TODO: need to be properly implemented */
  if (ccb->ccb$b_amod==1) {
    *ccbp=ccb;
    return SS$_NORMAL;
  } else 
    return 0;
}

/**
   \brief translate eventual device name logical
*/

int ioc_std$trandevnam (void * descr_p, int flags, char *buf, int *outlen, void **out_p) {
#if 0
  int sts;
  $DESCRIPTOR(mytabnam_, "LNM$SYSTEM_TABLE");
  struct dsc$descriptor * mytabnam = &mytabnam_;
  $DESCRIPTOR(mytabnam2_, "LNM$PROCESS_TABLE");
  struct dsc$descriptor * mytabnam2 = &mytabnam2_;
  struct item_list_3 itm[2];
  itm[0].item_code=LNM$_STRING;
  itm[0].buflen=4;
  itm[0].retlenaddr=outlen;
  itm[0].bufaddr=buf;
  memset(&itm[1],0,sizeof(struct item_list_3));
  *out_p=buf;
  struct dsc$descriptor * d = descr_p, descr;
  descr.dsc$a_pointer=d->dsc$a_pointer;
  char * c = strchr(d->dsc$a_pointer,':'); // get rid of colon here
  if (c)
    descr.dsc$w_length=(long)c-(long)d->dsc$a_pointer;
  else
    descr.dsc$w_length=d->dsc$w_length;
  descr_p=&descr;
  sts=exe$trnlnm(0,mytabnam,descr_p,0,itm);
  if (sts&1)
    return sts;
  sts=exe$trnlnm(0,mytabnam2,descr_p,0,itm);
  return sts;
#else
  int sts;
  $DESCRIPTOR(mytabnam_, "LNM$SYSTEM_TABLE");
  struct dsc$descriptor * mytabnam = &mytabnam_;
  $DESCRIPTOR(mytabnam2_, "LNM$PROCESS_TABLE");
  struct dsc$descriptor * mytabnam2 = &mytabnam2_;
  *out_p=buf;
  struct dsc$descriptor * d = descr_p, descr;
  descr.dsc$a_pointer=d->dsc$a_pointer;
  char * c = strchr(d->dsc$a_pointer,':'); // get rid of colon here
  if (c)
    descr.dsc$w_length=(long)c-(long)d->dsc$a_pointer;
  else
    descr.dsc$w_length=d->dsc$w_length;
  descr_p=&descr;
  struct dsc$descriptor * descr_p2 = descr_p;
  struct struct_lnm_ret ret={0,0};
  sts=lnm$searchlog(&ret,descr_p2->dsc$w_length,descr_p2->dsc$a_pointer,mytabnam->dsc$w_length,mytabnam->dsc$a_pointer);
  if (sts&1)
    goto found;
  sts=lnm$searchlog(&ret,descr_p2->dsc$w_length,descr_p2->dsc$a_pointer,mytabnam2->dsc$w_length,mytabnam2->dsc$a_pointer);
  if ((sts&1)==0)
    return sts;
 found:
  *outlen=(ret.mylnmb)->lnmb$l_lnmx->lnmx$l_xlen;
  memcpy(buf,(ret.mylnmb)->lnmb$l_lnmx->lnmx$t_xlation,*outlen);
  return sts;
#endif
  /** check for :: - MISSING */
}

int ioc_std$search (void * descr_p, int flags, void *lock_val_p, struct _ucb **ucb_p, struct _ddb **ddb_p, struct _sb **sb_p) {

}

int ioc_std$searchdev (void * descr_p, struct _ucb **ucb_p, struct _ddb **ddb_p, struct _sb **sb_p) {

}

