// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<descrip.h>

#include "cli.h"

unsigned int cli$get_value(void *entity_desc, void *retdesc,short * retlen) {
  struct dsc$descriptor * ret = retdesc;

  struct dsc$descriptor * desc = entity_desc;
  char * name = desc->dsc$a_pointer;
  int dlen = desc->dsc$w_length;
  int min; 
  struct _cdu * cdu = *my_cdu;

  if (dlen == 2 && name[0]=='p' && name[1]>='0' && name[1]<='9') {
    struct _para * p = cdu->cdu$l_parameters;
    while (p) {
      if (0==strncmp(p->name,name,2)) {
	memcpy(ret->dsc$a_pointer,p->value,strlen(p->value));
	if (retlen)
	  *retlen=strlen(p->value);
	return 1;
      }
      p=p->next;
    }
  } else {
    struct _qual * q = cdu->cdu$l_qualifiers;
    while (q) {
      int len=strlen(q->name);
      min=len<dlen?len:dlen;
      if (0==strncmp(q->name,name,min)) {
	// not yet
	return 1;
      }
      q=q->next;
    }
  }
  return 0;
}

unsigned int cli$present(void *entity_desc) {
  struct dsc$descriptor * desc = entity_desc;
  char * name = desc->dsc$a_pointer;
  int dlen = desc->dsc$w_length;
  int min; 
  struct _cdu * cdu = *my_cdu;
  struct _qual * q = cdu->cdu$l_qualifiers;
  while (q) {
    int len=strlen(q->name);
    min=len<dlen?len:dlen;
    if (0==strncmp(q->name,name,min))
      return 1;
    q=q->next;
  }
  return 0;
}

