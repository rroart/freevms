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
      bcopy(&d->ddb$t_name,it->bufaddr,15);
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
      bcopy(&d->ddb$t_name,it->bufaddr,15);
      break;

    }
    it++;
  }
  return SS$_NORMAL;
  
}

