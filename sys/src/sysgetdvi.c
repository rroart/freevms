// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>

#include<starlet.h>
#include<ssdef.h>
#include<dvidef.h>
#include<system_data_cells.h>
#include<misc.h>
#include<ddbdef.h>
#include<ucbdef.h>

struct _generic_64 {
  long long l;
};

asmlinkage int exe$getdvi(unsigned int efn, unsigned short int chan, void *devnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), int astprm, struct _generic_64 *nullarg);

asmlinkage int exe$getdvi_wrap(struct struct_getdvi *s) {
  return exe$getdvi(s->efn,s->chan,s->devnam,s->itmlst,s->iosb,s->astadr,s->astprm,s->nullarg);
}

struct _ddb * contxt=0;

asmlinkage int exe$getdvi(unsigned int efn, unsigned short int chan, void *devnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), int astprm, struct _generic_64 *nullarg) {

  struct _ddb * d;
  struct item_list_3 * it=itmlst;

  exe$clref(efn);
  // check and do with iosb
  // check quota 
  // check chan no?
  // then check devnam


  // check itemcode with dvsdef defs
  if (contxt)
    d=contxt;
  else
    d=ioc$gl_devlist;
  // sch$iolockr
  while (it->item_code) {
    switch (it->item_code) {
    case DVI$_DEVNAM:
      if (chan) {
	struct _ccb * c = &ctl$gl_ccbbase[chan];
	struct _ucb * u = c->ccb$l_ucb;
	struct _ddb * d = u->ucb$l_ddb;
	memcpy(it->bufaddr, &d->ddb$t_name[1], 3);
	snprintf(&it->bufaddr[3],3,"%d",u->ucb$w_unit);
      } else {
	memcpy(it->bufaddr,&d->ddb$t_name[1],15);
      }
      break;
    case DVI$_UNIT:
      if (chan) {
	struct _ccb * c = &ctl$gl_ccbbase[chan];
	struct _ucb * u = c->ccb$l_ucb;
	struct _ddb * d = u->ucb$l_ddb;
	memcpy(it->bufaddr, &u->ucb$w_unit, 2);
      }
      break;
    case DVI$_PID:
      if (chan) {
	struct _ccb * c = &ctl$gl_ccbbase[chan];
	struct _ucb * u = c->ccb$l_ucb;
	struct _ddb * d = u->ucb$l_ddb;
	memcpy(it->bufaddr, &u->ucb$l_pid, 4);
      }
      break;
    case DVI$_OWNUIC:
      if (chan) {
	struct _ccb * c = &ctl$gl_ccbbase[chan];
	struct _ucb * u = c->ccb$l_ucb;
	struct _ddb * d = u->ucb$l_ddb;
	int i=0;
	memcpy(it->bufaddr, &i, 4); // implement later
      }
      break;
    default:
      printk("getdvi item_code %x not implemented\n",it->item_code);
      break;
    }
    it++;
  }

  contxt=d->ddb$l_link;
  if (contxt==0) 
    return SS$_NOMOREDEV;

  return SS$_NORMAL;

}

asmlinkage int exe$getdviw(unsigned int efn, unsigned short int chan, void *devnam, void *itmlst, struct _iosb *iosb, void (*astadr)(), int astprm, struct _generic_64 *nullarg) {

  /* I think this is about it */

  int status=exe$getdvi(efn,chan,devnam,itmlst,iosb,astadr,astprm,nullarg);
  if ((status&1)==0) return status;
  return exe$synch(efn,iosb);

}

asmlinkage int exe$getdviw_wrap(struct struct_getdvi *s) {
  return exe$getdviw(s->efn,s->chan,s->devnam,s->itmlst,s->iosb,s->astadr,s->astprm,s->nullarg);
}

asmlinkage int exe$device_scan(void *return_devnam, unsigned short int *retlen, void *search_devnam, void *itmlst, unsigned long long *contxt) {
  struct _ddb * d;
  struct item_list_3 * it=itmlst;
  // check itemcode with dvsdef defs
  if (*contxt)
    d=*contxt;
  else
    d=ioc$gl_devlist;
  // sch$iolockr
 again:
  ioc$scan_iodb_usrctx(&d);
  if (d==0)
    return SS$_NOMOREDEV;

  while (it->item_code) {
    switch (it->item_code) {
    case DVI$_DEVNAM:
      memcpy(it->bufaddr,&d->ddb$t_name[1],15);
      break;

    }
    it++;
  }
  return SS$_NORMAL;
  
}

