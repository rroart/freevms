// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<descrip.h>

#include "cli.h"

unsigned int cli$get_value(void *entity_desc, void *retdesc,short * retlen) {
  struct _cdu * cdu_root = *root_cdu;
  
  struct dsc$descriptor * ret = retdesc;

  struct dsc$descriptor * desc = entity_desc;
  char * name = desc->dsc$a_pointer;
  int dlen = desc->dsc$w_length;
  int min; 
  struct _cdu * cdu = *my_cdu;

  if (dlen == 2 && name[0]=='p' && name[1]>='0' && name[1]<='9') {
    int p = cdu->cdu$l_parameters;
    while (p) {
      int namecdu=cdu_root[p].cdu$l_name;
      char * pname = cdu_root[namecdu].cdu$t_name;
      if (0==strncmp(pname,name,2)) {
	int valuecdu=cdu_root[p].cdu$l_value;
	char * vname = cdu_root[valuecdu].cdu$t_name;
	memcpy(ret->dsc$a_pointer,vname,strlen(vname));
	if (retlen)
	  *retlen=strlen(vname);
	return 1;
      }
      p=cdu_root[p].cdu$l_next;
    }
  } else {
    int q = cdu->cdu$l_qualifiers;
    while (q) {
      int len=strlen(cdu_root[q].cdu$t_name);
      min=len<dlen?len:dlen;
      if (0==strncmp(cdu_root[q].cdu$t_name,name,min)) {
	// not yet
	return 1;
      }
      q=cdu_root[q].cdu$l_next;
    }
  }
  return 0;
}

unsigned int cli$present(void *entity_desc) {
  struct _cdu * my_cdu_root = *my_cdu;
  struct _cdu * cdu_root = *root_cdu;

  struct dsc$descriptor * desc = entity_desc;
  char * name = desc->dsc$a_pointer;
  int dlen = desc->dsc$w_length;
  int min; 
  struct _cdu * cdu = *my_cdu;
  int q = cdu->cdu$l_qualifiers;
  while (q) {
    int n = my_cdu_root[q].cdu$l_name;
    int len=strlen(my_cdu_root[n].cdu$t_name);
    //printf("q n %x %x %x\n",q,n,len);
    min=len<dlen?len:dlen;
    //printf("cmp %s, %s, %x\n",my_cdu_root[n].cdu$t_name,name,min);
    if (0==strncmp(my_cdu_root[n].cdu$t_name,name,min))
      return 1;
    q=my_cdu_root[q].cdu$l_next;
  }
  return 0;
}

