// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <clidef.h>
#include <cliservdef.h>
#include <descrip.h>
#include <ssdef.h>

// use clicall for this some time in the future

static void ** comdsc = 0x3f00000c;

int lib$get_foreign(void * resultant_string, void * prompt_string, short int * resultant_length, int flags) {
  // fix str$ use later
  struct dsc$descriptor * dsc = *comdsc;
  struct dsc$descriptor * resdsc = resultant_string; 
  memcpy(resdsc->dsc$a_pointer, dsc->dsc$a_pointer, dsc->dsc$w_length);
  if (resultant_length)
    (*resultant_length) = dsc->dsc$w_length;
  return SS$_NORMAL;
}

