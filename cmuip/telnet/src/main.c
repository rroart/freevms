#include <descrip.h>

int errno;

setunwind() {
  printf("setunwind not implemented\n");
  return 1;
}

sys$dclexh() {
  printf("dclexh not implemented\n");
  return 1;
}

sys$trnlog(void * lognam, short * rsllen, void * rslbuf) {
  $DESCRIPTOR(tab_dsc, "LNM$PROCESS_TABLE");
  return lib$get_logical(lognam, rslbuf, rsllen, &tab_dsc, 0, 0, 0, 0, 0);
}

sys$putmsg() {
  printf("putmsg not implemented\n");
  return 0;
}

lbr$output_help() {
  printf("output_help not implemented\n");
  return 0;
}

lib$spawn() {
  printf("spawn not implemented\n");
  return 0;
}

lib$attach() {
  printf("attach not implemented\n");
  return 1;
}

lib$tparse() {
  printf("lib$tparse not implemented\n");
  return 0;
}

int
smg$create_pasteboard(unsigned int * pasteboard_id , void * output_device , signed int * number_of_pasteboard_rows , signed int * number_of_pasteboard_columns ,unsigned int * flags , unsigned int * type_of_terminal , void * device_name) {
  *pasteboard_id=0;
  if (number_of_pasteboard_rows)
    *number_of_pasteboard_rows=80;
  if (number_of_pasteboard_columns)
    *number_of_pasteboard_columns=20;
  return 1;
  printf("smg$create_pasteboard not implemented\n");
}

int
smg$delete_virtual_keyboard(keyboard_id) {
  return 1;
  printf("smg$delete_virtual_keyboard not implemented\n");
}

int
smg$create_key_table(unsigned int * key_table_id) {
  *key_table_id=0;
  return 1;
  printf("smg$create_key_table not implemented\n");
}

int
smg$read_composed_line(unsigned int * keyboard_id ,unsigned int * key_table_id ,void * resultant_string ,void * prompt_string ,unsigned short * resultant_length , unsigned int * display_id , unsigned int * flags , void * initial_string ,signed int * timeout , unsigned int * rendition_set , unsigned int * rendition_complement , signed short int * word_terminator_code) {
  struct dsc$descriptor * in , * out;
  in = resultant_string;
  out = prompt_string;
  printf("%s",out->dsc$a_pointer);
  short int len = read(0,in->dsc$a_pointer,in->dsc$w_length);
  if (*resultant_length)
    *resultant_length=len;
  return 1;
  printf("smg$read_composed_line not implemented\n");
}

int
smg$read_string(unsigned int * keyboard_id , void * resultant_string , void * prompt_string , signed int * maximum_length , unsigned int * modifiers , signed int timeout , void * terminator_set , unsigned short int * resultant_length , unsigned short int * word_terminator_code , unsigned int * display_id , void * initial_string ,unsigned int * rendition_set ,unsigned int * rendition_complement , void * terminator_string) {
  struct dsc$descriptor * in , * out;
  in = resultant_string;
  out = prompt_string;
  printf("%s",out->dsc$a_pointer);
  short int len = read(0,in->dsc$a_pointer,in->dsc$w_length);
  return 1;
}

int
smg$create_virtual_keyboard(unsigned int * keyboard_id , void * input_device , void * default_filespec , void * resultant_filespec , unsigned char * recall_size) {
  *keyboard_id=0;
  return 1;
  printf("smg$create_virtual_keyboard not implemented\n");
}

int do_attach (int mask);
int close (int mask);
int do_connect (int mask);
int exit_telnet (int mask);
int do_negotiate_options (int mask);
int telnet_help (int mask);
int new_conn (int mask);
int do_send_ao (int mask);
int do_send_ayt (int mask);
int do_send_brk (int mask);
int do_send_ec (int mask);
int do_send_el (int mask);
int do_send_escape (int mask);
int do_send_file (int mask);
int do_send_ga (int mask);
int do_send_ip (int mask);
int do_send_nop (int mask);
int do_send_synch (int mask);
int do_send_timing_mark (int mask);
int do_set_autoflush_on (int mask);
int do_set_autoflush_off (int mask);
int do_set_binary_on (int mask);
int do_set_binary_off (int mask);
int do_set_blocking_on (int mask);
int do_set_blocking_off (int mask);
int do_set_key_ao (int mask);
int do_set_key_ayt (int mask);
int do_set_key_brk (int mask);
int do_set_key_ec (int mask);
int do_set_key_escape (int mask);
int do_set_key_el (int mask);
int do_set_key_ip (int mask);
int do_set_key_echo (int mask);
int do_set_key_synch (int mask);
int do_set_connect_on (int mask);
int do_set_connect_off (int mask);
int do_set_debug_on (int mask);
int do_set_debug_off (int mask);
int do_set_echo_on (int mask);
int do_set_echo_off (int mask);
int do_set_eightbit_on (int mask);
int do_set_eightbit_off (int mask);
int do_set_extended_on (int mask);
int do_set_extended_off (int mask);
int new_conn (int mask);
int do_set_localchars_on (int mask);
int do_set_logfile_name (int mask);
int do_set_line_mode_off (int mask);
int do_set_line_mode_on (int mask);
int do_set_line_mode_negotiated (int mask);
int do_set_eol_off (int mask);
int do_set_eol_on (int mask);
int do_set_synch_on (int mask);
int do_set_synch_off (int mask);
int do_set_quiet_on (int mask);
int do_set_quiet_off (int mask);
int do_set_urgent_on (int mask);
int do_set_urgent_off (int mask);
int do_show (int mask);
int do_spawn (int mask);
int do_status (int mask);
int do_version (int mask);

struct cli_struct {
  char * cliroutine;
  void (*fn)();
};

struct cli_struct cliroutines[]={
{  "do_attach",  do_attach,  },
{  "close",  close,  },
{  "do_connect",  do_connect,  },
{  "exit_telnet",  exit_telnet,  },
{  "do_negotiate_options",  do_negotiate_options,  },
{  "telnet_help",  telnet_help,  },
{  "new_conn",  new_conn,  },
{  "do_send_ao",  do_send_ao,  },
{  "do_send_ayt",  do_send_ayt,  },
{  "do_send_brk",  do_send_brk,  },
{  "do_send_ec",  do_send_ec,  },
{  "do_send_el",  do_send_el,  },
{  "do_send_escape",  do_send_escape,  },
{  "do_send_file",  do_send_file,  },
{  "do_send_ga",  do_send_ga,  },
{  "do_send_ip",  do_send_ip,  },
{  "do_send_nop",  do_send_nop,  },
{  "do_send_synch",  do_send_synch,  },
{  "do_send_timing_mark",  do_send_timing_mark,  },
{  "do_set_autoflush_on",  do_set_autoflush_on,  },
{  "do_set_autoflush_off",  do_set_autoflush_off,  },
{  "do_set_binary_on",  do_set_binary_on,  },
{  "do_set_binary_off",  do_set_binary_off,  },
{  "do_set_blocking_on",  do_set_blocking_on,  },
{  "do_set_blocking_off",  do_set_blocking_off,  },
{  "do_set_key_ao",  do_set_key_ao,  },
{  "do_set_key_ayt",  do_set_key_ayt,  },
{  "do_set_key_brk",  do_set_key_brk,  },
{  "do_set_key_ec",  do_set_key_ec,  },
{  "do_set_key_escape",  do_set_key_escape,  },
{  "do_set_key_el",  do_set_key_el,  },
{  "do_set_key_ip",  do_set_key_ip,  },
{  "do_set_key_echo",  do_set_key_echo,  },
{  "do_set_key_synch",  do_set_key_synch,  },
{  "do_set_connect_on",  do_set_connect_on,  },
{  "do_set_connect_off",  do_set_connect_off,  },
{  "do_set_debug_on",  do_set_debug_on,  },
{  "do_set_debug_off",  do_set_debug_off,  },
{  "do_set_echo_on",  do_set_echo_on,  },
{  "do_set_echo_off",  do_set_echo_off,  },
{  "do_set_eightbit_on",  do_set_eightbit_on,  },
{  "do_set_eightbit_off",  do_set_eightbit_off,  },
{  "do_set_extended_on",  do_set_extended_on,  },
{  "do_set_extended_off",  do_set_extended_off,  },
{  "new_conn",  new_conn,  },
{  "do_set_localchars_on",  do_set_localchars_on,  },
{  "do_set_logfile_name",  do_set_logfile_name,  },
{  "do_set_line_mode_off",  do_set_line_mode_off,  },
{  "do_set_line_mode_on",  do_set_line_mode_on,  },
{  "do_set_line_mode_negotiated",  do_set_line_mode_negotiated,  },
{  "do_set_eol_off",  do_set_eol_off,  },
{  "do_set_eol_on",  do_set_eol_on,  },
{  "do_set_synch_on",  do_set_synch_on,  },
{  "do_set_synch_off",  do_set_synch_off,  },
{  "do_set_quiet_on",  do_set_quiet_on,  },
{  "do_set_quiet_off",  do_set_quiet_off,  },
{  "do_set_urgent_on",  do_set_urgent_on,  },
{  "do_set_urgent_off",  do_set_urgent_off,  },
{  "do_show",  do_show,  },
{  "do_spawn",  do_spawn,  },
{  "do_status",  do_status,  },
{  "do_version",  do_version,  },
  { 0, 0 , },
};

void * get_cli_int(char * c) {
  int i;
  for (i=0;cliroutines[i].cliroutine;i++) {
    if (0==strncmp(c,cliroutines[i].cliroutine,strlen(cliroutines[i].cliroutine)))
      return cliroutines[i].fn;
  }
  return 0;
}

int vms_mm = 1;

#if 0
int
telnet$_get_inet=1,
telnet$_not_term=2,
telnet$_host_not_found=3,
telnet$_key_syntax=4,
telnet$_esc_syntax=5,
telnet$_port_syntax=6,
telnet$_tcp_recv=7,
telnet$_tcp_send=8,
telnet$_no_open=9,
telnet$_protocol=10,
telnet$_writefailed=11,
telnet$_no_switch=12,
telnet$_control_c=13,
telnet$_error=14,
telnet$_autoflush_on=15,
telnet$_autoflush_off=16,
telnet$_binary_on=17,
telnet$_binary_off=18,
telnet$_blocking_on=19,
telnet$_blocking_off=20,
telnet$_connect_on=21,
telnet$_connect_off=22,
telnet$_debug_on=23,
telnet$_debug_off=24,
telnet$_echo_on=25,
telnet$_echo_off=26,
telnet$_rem_echo_on=27,
telnet$_rem_echo_off=28,
telnet$_eightbit_on=29,
telnet$_eightbit_off=30,
telnet$_eol_type_crlf=31,
telnet$_eol_type_crnull=32,
telnet$_extended_on=33,
telnet$_extended_off=34,
telnet$_linemode_neg=35,
telnet$_linemode_on=36,
telnet$_linemode_off=37,
telnet$_localchars_on=38,
telnet$_localchars_off=39,
telnet$_local_synch=40,
telnet$_no_local_synch=41,
telnet$_log_on=42,
telnet$_log_off=43,
telnet$_negotiate=44,
telnet$_no_log=45,
telnet$_rem_synch_on=46,
telnet$_rem_synch_off=47,
telnet$_synch_on=48,
telnet$_synch_off=49,
telnet$_quiet_on=50,
telnet$_quiet_off=51,
telnet$_try_binary_on=52,
telnet$_try_binary_off=53,
telnet$_no_binary_on=54,
telnet$_urgent_on=55,
telnet$_urgent_off=56,
telnet$_x_on=57,
telnet$_x_off=58,
telnet$_duplicate_keys=59,
telnet$_trying=60,
telnet$_not_attached=61,
telnet$_attach_to=62,
telnet$_not_connected=63,
telnet$_option_oor=64,
telnet$_r_will=65,
telnet$_r_wont=66,
telnet$_r_do=67,
telnet$_r_dont=68,
telnet$_r_ttyloc=69,
telnet$_r_ttytype_send=70,
telnet$_r_window_size=71,
telnet$_r_ttyspeed_send=72,
telnet$_r_tog_flow_cntl_off=73,
telnet$_r_tog_flow_cntl_on=74,
telnet$_r_linemode_off=75,
telnet$_r_linemode_on=76,
telnet$_s_comm=77,
telnet$_s_comm_unknown=78,
telnet$_s_will=79,
telnet$_s_wont=80,
telnet$_s_do=81,
telnet$_s_dont=82,
telnet$_s_ttyloc=83,
telnet$_s_nop=84,
telnet$_s_ttytype=85,
telnet$_s_location=86,
telnet$_s_ttyspeed=87,
telnet$_s_window_size=88,
telnet$_s_line_width=89,
telnet$_s_page_size=90,
telnet$_s_user_id=91,
telnet$_closed=92,
telnet$_conn_closed=93,
telnet$_abort=94,
telnet$_connected =95,
telnet$_open=96,
telnet$_no_options=97,
telnet$_pause=98,
telnet$_returning=99,
telnet$_resume=100;
#endif

int smg$_eof=27;

int net$_cc = 101;

int cli$_absent = 229872; 
int cli$_negated = 229880;
int cli$_nocomd = 229552;
int cli$_present = 261401;
int cli$_defaulted = 261409;
