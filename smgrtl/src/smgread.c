// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iodef.h>
#include <ssdef.h>

extern int smgunix;

#undef smg$read_composed_line
int
smg$read_composed_line(long * keyboard_id, long * key_table_id ,void * resultant_string ,void * prompt_string ,unsigned short * resultant_length , long * display_id , unsigned int * flags , void * initial_string ,signed int * timeout , unsigned int * rendition_set , unsigned int * rendition_complement , signed short int * word_terminator_code) {
  struct dsc$descriptor * in , * out;
  in = resultant_string;
  out = prompt_string;
  if (display_id)
    smg$$cursor(*display_id);
  if (smgunix)
    printf("%s",out->dsc$a_pointer);
  else {
#if 0
    $DESCRIPTOR(as,"!AS");
    print_routine(&as,out);
#else
    // replace read write with qio later
    write (1, out, strlen(out));
#endif
  }
  in->dsc$a_pointer=malloc(50); // leak;
  in->dsc$w_length=50;
  // check for smgunix here soon
  short int len = read(0,in->dsc$a_pointer,in->dsc$w_length);
  if (resultant_length)
    *resultant_length=len;
  return 1;
}

#undef smg$read_string
int
smg$read_string(long * keyboard_id , void * resultant_string , void * prompt_string , signed int * maximum_length , unsigned int * modifiers , signed int timeout , void * terminator_set , unsigned short int * resultant_length , unsigned short int * word_terminator_code , long * display_id , void * initial_string ,unsigned int * rendition_set ,unsigned int * rendition_complement , void * terminator_string) {
  struct dsc$descriptor * in , * out;
  in = resultant_string;
  out = prompt_string;
  smg$$cursor(*display_id);
  if (smgunix) {
    printf("%s",out->dsc$a_pointer);
    short int len = read(0,in->dsc$a_pointer,in->dsc$w_length);
    if (resultant_length)
      *resultant_length=len;
  } else {
    short int chan;
    smg$$virtual_kbd_chan(*keyboard_id, &chan);
    sys$qio(0, chan, IO$_WRITEVBLK, 0, 0, 0, out->dsc$a_pointer, out->dsc$w_length, 0, 0, 0, 0);
    sys$qiow(0, chan, IO$_READVBLK, 0, 0, 0, in->dsc$a_pointer, in->dsc$w_length, 0, 0, 0, 0);
  }
  return 1;
}

