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
#include <smgdef.h>

int smgunix = 1;

struct pasteboard {
  long smg$l_virtual_display;
  short int smg$w_chan;
  int smg$l_rows;
  int smg$l_columns;
  int smg$l_flags;
  int smg$l_terminal_type;
};

struct paste_virtual_display {
  long smg$l_link;
  long smg$l_display_id;
  //  int smg$l_pasteboard_id;
  int smg$l_pasteboard_row;
  int smg$l_pasteboard_column;
  long smg$l_top_display_id;
};

#undef smg$create_pasteboard
int
smg$create_pasteboard(long * pasteboard_id , void * output_device , signed int * number_of_pasteboard_rows , signed int * number_of_pasteboard_columns ,unsigned int * flags , unsigned int * type_of_terminal , void * device_name) {
  int status;
  *pasteboard_id=0;
#if 0
  if (number_of_pasteboard_rows)
    *number_of_pasteboard_rows=80;
  if (number_of_pasteboard_columns)
    *number_of_pasteboard_columns=20;
#endif
  $DESCRIPTOR(dsc, "sys$output");
  struct pasteboard * smg = malloc (sizeof (struct pasteboard));
  *pasteboard_id = smg;
  if (output_device == 0)
    output_device = &dsc;
  if (!smgunix)
    status = sys$assign (output_device, &smg->smg$w_chan, 0, 0, 0);
  else {
    smg->smg$w_chan = 1;
    status = 1;
  }
  char tt_char_save[12];
  if (!smgunix) {
    status = sys$qiow (0, smg->smg$w_chan, IO$_SENSEMODE, 0, 0, 0, tt_char_save, 12, 0, 0, 0, 0);
    smg->smg$l_rows = tt_char_save[7];
    smg->smg$l_columns = * (short *) &tt_char_save[2];
  } else {
    smg->smg$l_rows = 24;
    smg->smg$l_columns = 80;
  }
  if (number_of_pasteboard_rows)
    *number_of_pasteboard_rows = smg->smg$l_rows;
  if (number_of_pasteboard_columns)
    *number_of_pasteboard_columns = smg->smg$l_columns;
  if (flags)
    smg->smg$l_flags = *flags;
  if (type_of_terminal)
    *type_of_terminal = smg->smg$l_terminal_type = SMG$K_VTTERMTABLE;
  // check chan fd later
  char buf[16];
  sprintf(buf, "\33[H");
  write (1, buf, strlen(buf));
  return SS$_NORMAL;
}

#undef smg$delete_pasteboard
int
smg$delete_pasteboard(long * pasteboard_id, int * flags) {
  free(*pasteboard_id);
  return 1;
}

#undef smg$erase_pasteboard
int
smg$erase_pasteboard (long * pasteboard_id , void * output_device , signed int * number_of_pasteboard_rows , signed int * number_of_pasteboard_columns ,unsigned int * flags , unsigned int * type_of_terminal , void * device_name) {
  // replace later
  char buf[16];
  sprintf (buf, "\33[2J");
  write (1, buf, strlen(buf));
  return 1;
}

int smg$$get_pasteboard_chan (void * pasteboard_id, short * chan) {
  struct pasteboard * p = pasteboard_id;
  *chan = p->smg$w_chan;
  return SS$_NORMAL;
}

#undef smg$paste_virtual_display
int smg$paste_virtual_display (long * display_id, long * pasteboard_id, int * pasteboard_row, int * pasteboard_column, long * top_display_id) {
  struct paste_virtual_display * pvd = malloc (sizeof (struct paste_virtual_display));
  pvd->smg$l_display_id = *display_id;
  pvd->smg$l_pasteboard_row = *pasteboard_row;
  pvd->smg$l_pasteboard_column = *pasteboard_column;
  //later top_display
  pvd->smg$l_link = **(long **)pasteboard_id;
  **(long **)pasteboard_id = pvd;
  smg$$display(*display_id, *pasteboard_row, *pasteboard_column);
  return SS$_NORMAL;
}

int smg$begin_pasteboard_update (long * pasteboard_id) {
  return SS$_NORMAL;
}

int smg$end_pasteboard_update (long * pasteboard_id) {
  struct pasteboard * smg = *pasteboard_id;
  struct paste_virtual_display * pvd = smg->smg$l_virtual_display;
  for (; pvd ; pvd = pvd->smg$l_link) {
    smg$$display(pvd->smg$l_display_id, pvd->smg$l_pasteboard_row, pvd->smg$l_pasteboard_column);
  }
  return SS$_NORMAL;
}

int smg$set_cursor_mode (long * pasteboard_id, int * flags) {
  return SS$_NORMAL;
}

#undef smg$get_broadcast_message
int smg$get_broadcast_message (long * pasteboard_id, void * message, short * message_length, short * message_type) {
  return SS$_NORMAL;
}

#undef smg$set_broadcast_trapping
int smg$set_broadcast_trapping (long * pasteboard_id, long AST_routine, long AST_argument) {
  return SS$_NORMAL;
}

int smg$disable_broadcast_trapping (long * pasteboard_id) {
  return SS$_NORMAL;
}

#undef smg$put_pasteboard
int smg$put_pasteboard (long * pasteboard_id, long action_routine, long user_argument, int * flags) {
  return SS$_NORMAL;
}

#undef smg$change_pbd_characteristics
int smg$change_pbd_characteristics (long * pasteboard_id, int * desired_width, int * width, int * desired_height, int * height, int * desired_background_color, int * background_color) {
  // bitmask problems?
  int old2;
  int on2 = 0;
  if (desired_width)
    on2 = *desired_width << 16;
  int sts = smg$set_term_characteristics (pasteboard_id, 0, &on2, 0, 0, 0, &old2, 0, 0, 0);
  if (width)
    *width = (old2>>16)&0xff;
  return SS$_NORMAL;
}

int smg$flush_buffer (long * pasteboard_id) {
  return smg$end_pasteboard_update (pasteboard_id);
}
