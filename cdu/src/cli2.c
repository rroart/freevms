#include<descrip.h>

#include "cli.h"

unsigned int cli$get_value(void *entity_desc, void *retdesc,short * retlen) {

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

