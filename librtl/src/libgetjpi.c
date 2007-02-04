#include <descrip.h>
#include <misc.h>
#include <ssdef.h>

int lib$getjpi(signed int * item_code, unsigned int * process_id, void * process_name, signed int * longword_integer_value, void * resultant_string, unsigned short * resultant_length) {
  struct _iosb iosb;
  struct item_list_3 itmlst[2];
  int retlen;
  int * retlenaddr;
  int buflen;
  void * bufaddr;
  struct dsc$descriptor * dsc;
  struct dsc$descriptor * res = resultant_string;
  int sts;
  int efn;

  sts = lib$get_ef(&efn);
  if ((sts&1)==0)
    return sts;

  // doing some approximations since I can not now decide the return type
  
  if (process_id) {
    buflen = 4;
    bufaddr = process_id;
    retlenaddr = &retlen;
  } else {
    if (res) {
      buflen = res->dsc$w_length;
      bufaddr = res->dsc$a_pointer;
    } else {
      buflen = 4;
      bufaddr = longword_integer_value;
    }
    if (resultant_length)
      retlenaddr = resultant_length;
    else
      retlenaddr = &retlen;
  }
  
  itmlst[0].item_code=*item_code;
  itmlst[0].buflen=buflen;
  itmlst[0].retlenaddr=retlenaddr;
  itmlst[0].bufaddr=bufaddr;
  itmlst[1].item_code=0;

  sts=sys$getjpiw(efn,process_id,process_name,itmlst,&iosb,0,0,0);

  if ((sts&1)==0)
    return sts;

  sts = lib$free_ef(&efn);
  if ((sts&1)==0)
    return sts;

  return sts;
}
