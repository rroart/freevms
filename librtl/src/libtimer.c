// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <descrip.h>
#include <jpidef.h>
#include <libdef.h>
#include <ssdef.h>
#include <starlet.h>

// note will use -1 for void really

static long timer_context[5];

static int set_context(long * context) {
  int status;
  signed long long timevalue; 
  status = sys$gettim(&timevalue); 
  context[0] = timevalue; // fix later
  int item_code;
  int len = 0;
  item_code = JPI$_CPUTIM;
  status = lib$getjpi (&item_code, 0, 0, &context[1], 0, &len);
  item_code = JPI$_BIOCNT;
  status = lib$getjpi (&item_code, 0, 0, &context[2], 0, &len);
  item_code = JPI$_DIOCNT;
  status = lib$getjpi (&item_code, 0, 0, &context[3], 0, &len);
  item_code = JPI$_PAGEFLTS;
  status = lib$getjpi (&item_code, 0, 0, &context[4], 0, &len);
}

int lib$init_timer (long * context) {
  long * mycontext = 0;
  if (*context == -1)
    mycontext = &timer_context[0];
  if (*context == 0)
    mycontext = *context = malloc (5 * sizeof(long));
  memset (mycontext, 0, 5 * sizeof(long));
  set_context (mycontext);
  return SS$_NORMAL;
}

int lib$free_timer (long * context) {
  long * mycontext = 0;
  if (*context == -1 || *context == 0)
    return SS$_NORMAL;
  free (*context);
  *context = 0;
  return SS$_NORMAL;
}

int lib$show_timer (void * handle_address, int * code, int (*user_action_procedure)(), unsigned long user_argument_value) {
  long * context = handle_address;
  int status;
  long * mycontext = 0;
  if (*context == -1)
    mycontext = &timer_context[0];
  if (*context == 0)
    mycontext = *context = malloc (5 * sizeof(long));

  long new_context[5];
  memset (&new_context[0], 0, 5 * sizeof(long));

  set_context(&new_context[0]);

  char c[256];
  sprintf(c, "ELAPSED: %d CPU: %d BUFIO: %d  DIRIO: %d  FAULTS: %d", new_context[0]-mycontext[0], new_context[0]-mycontext[0], new_context[1]-mycontext[1], new_context[2]-mycontext[2], new_context[3]-mycontext[3], new_context[4]-mycontext[4]); // pretty-print later
  struct dsc$descriptor d;

  struct dsc$descriptor * out_str = &d;

  out_str->dsc$a_pointer = c;
  out_str->dsc$w_length = strlen(c);

  if (user_action_procedure)
    user_action_procedure (out_str, user_argument_value);
  else
    printf("%s\n", out_str->dsc$a_pointer);
  return SS$_NORMAL;
}

int lib$stat_timer (int * code, long * value_argument, void * handle_address) {
  long * context = handle_address;
  int status;
  long * mycontext = 0;
  if (*context == -1)
    mycontext = &timer_context[0];
  if (*context == 0)
    mycontext = *context = malloc (5 * sizeof(long));

  long new_context[5];
  memset (&new_context[0], 0, 5 * sizeof(long));

  set_context(&new_context[0]);

  *value_argument = new_context[*code - 1];

  return SS$_NORMAL;
}

