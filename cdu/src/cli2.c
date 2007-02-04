// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <climsgdef.h>
#include <ssdef.h>
#include "cli.h"

int my_cdu_search_next(int i, int type, char * s, int size, int * retval) {
  if (size==0)
    return 0;
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
  static int list_prev=0;
  static int list_point=0;
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
    int is_list=my_cdu_root[e].cdu$l_flags & CDU$M_LIST;
    char * vname = my_cdu_root[valuecdu].cdu$t_name;
    {
      if (ret->dsc$b_class==DSC$K_CLASS_D && ret->dsc$b_dtype==DSC$K_DTYPE_T && ret->dsc$a_pointer==0 && ret->dsc$w_length==0) {
	ret->dsc$a_pointer=malloc(strlen(vname));
	ret->dsc$w_length=strlen(vname);
      }
    }
    char * src = vname;
    int srclen = strlen(vname);
    if (is_list) {
      if (list_point == 0 && list_prev == 0) {
	list_prev = 0;
	list_point = valuecdu;
      }
      if (list_prev != 0 && list_point == 0) {
	sts = 0;
	list_prev = 0;
	list_point = 0;
	goto end;
      }
      vname = my_cdu_root[list_point].cdu$t_name;
      src = vname;
      srclen = strlen(vname);
      list_prev = list_point;
      list_point = my_cdu_root[list_point].cdu$l_next;
    }
    memcpy(ret->dsc$a_pointer,src,srclen);
    if (retlen)
      *retlen=strlen(vname);
  }
 end:
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

