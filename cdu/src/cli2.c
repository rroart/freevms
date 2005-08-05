// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <climsgdef.h>
#include <ssdef.h>
#include "cli.h"

int my_cdu_search_next(int i, int type, char * s, int size, int * retval) {
  struct _cdu * my_cdu_root = *my_cdu;
  do {
    struct _cdu * name;
    switch (type) {
    case CDU$C_VERB:
      name=&my_cdu_root[my_cdu_root[i].cdu$l_verb];
      break;
    case CDU$C_TYPE:
      name=&my_cdu_root[my_cdu_root[i].cdu$l_type];
      break;
    case CDU$C_SYNTAX:
      name=&my_cdu_root[my_cdu_root[i].cdu$l_syntax];
      break;
    case CDU$C_KEYWORD:
      name=&my_cdu_root[my_cdu_root[i].cdu$l_name];
      break;
    case CDU$C_PARAMETER:
    case CDU$C_QUALIFIER:
      name=&my_cdu_root[my_cdu_root[i].cdu$l_name];
      break;
    }
    int dlen=size; 
    int len=strlen(name->cdu$t_name);
    int min=len<dlen?len:dlen;
    if (my_cdu_root[i].cdu$b_type==type && strncasecmp(name->cdu$t_name,s,min)==0) {
      if (retval)
	*retval=i;
      return 1;
    }
    i=my_cdu_root[i].cdu$l_next;
  } while (i);
  return 0;
}

unsigned int cli$get_value(void *entity_desc, void *retdesc,short * retlen) {
  struct _cdu * my_cdu_root = *my_cdu;
  
  struct dsc$descriptor * ret = retdesc;

  struct dsc$descriptor * desc = entity_desc;
  char * name = desc->dsc$a_pointer;
  int dlen = desc->dsc$w_length;
  int min; 
  struct _cdu * cdu = *my_cdu;
  int sts=0;
  
  int e; 
  int elem;
  e = cdu->cdu$l_parameters; 
  sts = my_cdu_search_next(e, CDU$C_PARAMETER, name, dlen, &elem);
  e = cdu->cdu$l_qualifiers;
  if ((sts&1)==0)
    sts = my_cdu_search_next(e, CDU$C_QUALIFIER, name, dlen, &elem);
  if (sts&1) {
    e=elem;
    int valuecdu=my_cdu_root[e].cdu$l_value;
    char * vname = my_cdu_root[valuecdu].cdu$t_name;
    {
      if (ret->dsc$b_class==DSC$K_CLASS_D && ret->dsc$b_dtype==DSC$K_DTYPE_T && ret->dsc$a_pointer==0 && ret->dsc$w_length==0) {
	ret->dsc$a_pointer=malloc(strlen(vname));
	ret->dsc$w_length=strlen(vname);
      }
    }
    memcpy(ret->dsc$a_pointer,vname,strlen(vname));
    if (retlen)
      *retlen=strlen(vname);
  }
  if (sts&1)
    return SS$_NORMAL;
  else
    return CLI$_ABSENT;
}

unsigned int cli$present(void *entity_desc) {
  struct _cdu * my_cdu_root = *my_cdu;

  struct dsc$descriptor * desc = entity_desc;
  char * name = desc->dsc$a_pointer;
  int dlen = desc->dsc$w_length;
  int min; 
  struct _cdu * cdu = *my_cdu;
  int e = cdu->cdu$l_parameters;
  int sts = my_cdu_search_next(e, CDU$C_PARAMETER, name, dlen, 0);
  e = cdu->cdu$l_qualifiers;
  if ((sts&1)==0)
    sts = my_cdu_search_next(e, CDU$C_QUALIFIER, name, dlen, 0);
  if (sts&1)
    return CLI$_PRESENT;
  else
    return CLI$_ABSENT;
}

