#include <descrip.h>
#include <lnmdef.h>
#include <misc.h>
#include <ssdef.h>

int lib$get_logical(void * logical_name , void * resultant_string, unsigned short * resultant_length , void * table_name, int * max_index , unsigned int * index , unsigned char * acmode , unsigned int * flags) {
  struct _iosb iosb;
  struct item_list_3 itmlst[2];
  int retlen;
  int * retlenaddr;
  int buflen;
  void * bufaddr;
  struct dsc$descriptor * res = resultant_string;
  int sts;
  char s[255];
  int slen=255;

  // doing some approximations since I can not now decide the return type

  if (resultant_length)
    retlenaddr=resultant_length;
  else
    retlenaddr=&retlen;
  
  if (resultant_string) {
    buflen = res->dsc$w_length;
    bufaddr = res->dsc$a_pointer;
  } else {
    buflen = slen;
    bufaddr = s;
  }
  
  itmlst[0].item_code=LNM$_STRING;
  itmlst[0].buflen=buflen;
  itmlst[0].retlenaddr=retlenaddr;
  itmlst[0].bufaddr=bufaddr;
  itmlst[1].item_code=0;

  sts=sys$trnlnm(0,table_name,logical_name,acmode,itmlst);

  if ((sts&1)==0)
    return sts;

  return sts;
}
