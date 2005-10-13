// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <ssdef.h>
#include <misc.h>
#include <descrip.h>
#include <dvidef.h>

#ifdef __KERNEL__
#define sys$assign exe$assign
#define sys$getdviw exe$getdviw
#define sys$crembx exe$crembx
#endif

int lib$asn_wth_mbx (void * device_name , long * maximum_message_size, long * buffer_quota , short * device_channel , short * mailbox_channel) {
  struct _iosb iosb;
  struct item_list_3 itmlst[2];
  int retlen;
  char retbuf[16];
  struct dsc$descriptor d;
  int efn;

  int sts=sys$crembx(0,mailbox_channel,*maximum_message_size,*buffer_quota,0,0,0,0);
  if ((sts&1)==0)
    return sts;

  itmlst[0].item_code=DVI$_DEVNAM;
  itmlst[0].buflen=16;
  itmlst[0].retlenaddr=&retlen;
  itmlst[0].bufaddr=retbuf;
  itmlst[1].item_code=0;

#ifndef __KERNEL__
  sts = lib$get_ef(&efn);
  if ((sts&1)==0)
    return sts;
#else
  efn = 0;
#endif

  sts=sys$getdviw(efn,*mailbox_channel,0,itmlst,&iosb,0,0,0);

  if ((sts&1)==0)
    return sts;
  
#ifndef __KERNEL__
  sts = lib$free_ef(&efn);
  if ((sts&1)==0)
    return sts;
#endif

  d.dsc$a_pointer=retbuf;
  d.dsc$w_length=retlen;

  int devnam=device_name;
  sts=sys$assign(devnam,device_channel,0,&d,0);

  if ((sts&1)==0)
    return sts;
  
  return sts;
}

