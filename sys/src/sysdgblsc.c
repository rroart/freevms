// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#include<linux/sched.h>
#include <linux/vmalloc.h>
#include <system_data_cells.h>
#include<descrip.h>
#include<gsddef.h>
#include<ipldef.h>
#include<phddef.h>
#include<rdedef.h>
#include<secdef.h>
#include<seciddef.h>
#include<va_rangedef.h>
#include<wsldef.h>

/**
   \brief test compatibility of the flags arguments to crmpsc - see 5.2 15.4.1.1
*/

int mmg$vfysecflg(void) {
  /** MISSING content */
}

int exe$dgblsc(void) {

}

mmg$gsdscn(void * input_gsnam, unsigned long long * input_ident, int section_flags, char *return_gsnam, unsigned long long  *return_ident, struct _gsd **gsd) {
  struct _pcb * p = ctl$gl_pcb;
  struct _gsd * head, * tmp;
  struct dsc$descriptor * dsc = input_gsnam;
  if (section_flags&SEC$M_SYSGBL) {
    head=&exe$gl_gsdsysfl; 
  } else {
    head=&exe$gl_gsdgrpfl;
  }
  for (tmp=head->gsd$l_gsdfl;tmp!=head;tmp=tmp->gsd$l_gsdfl) {
    if (section_flags&SEC$M_SYSGBL) {} else {
      if (p->pcb$l_uic!=tmp->gsd$l_pcbuic) {
	goto end;
      }
    }
    if (0!=strncmp(*(char**)&tmp->gsd$t_gsdnam,dsc->dsc$a_pointer,dsc->dsc$w_length)) {
      goto end;
    }

#if 0
    // not yet
    if (tmp->gsd$l_ident->secid$l_version!=((struct _secid *)input_ident)->secid$l_version) {
      goto end;
    }
#endif

  if (gsd) *gsd=tmp;
  return;

  end:
  {
  }
  }
  if (gsd) *gsd=0;
  return 0;
}
