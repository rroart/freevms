// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <clidef.h>
#include <cliservdef.h>
#include <descrip.h>
#include <ssdef.h>

int lib$set_symbol(void * symbol , void * value_string, long * table_type_indicator) {
  struct _clidef1 cli_,*cli=&cli_;
  struct _clidef2 * cli2 = cli;
  cli->cli$b_rqtype=CLI$K_CLISERV;
  cli->cli$w_servcod=CLI$K_DEFLOCAL;
  memcpy(&cli2->cli$q_namdesc,symbol,8);
  memcpy(&cli2->cli$q_valdesc,value_string,8);
  sys$cli(cli);
  return SS$_NORMAL;
}

int lib$get_symbol(void * symbol , void * resultant_string, long * table_type_indicator) {
  struct _clidef1 cli_,*cli=&cli_;
  struct _clidef2 * cli2 = cli;
  cli->cli$b_rqtype=CLI$K_CLISERV;
  cli->cli$w_servcod=CLI$K_GETSYM;
  memcpy(&cli2->cli$q_namdesc,symbol,8);
  memcpy(&cli2->cli$q_valdesc,resultant_string,8);
  sys$cli(cli);
  return SS$_NORMAL;
}

int lib$delete_symbol(void * symbol, long * table_type_indicator) {
  struct _clidef1 cli_,*cli=&cli_;
  struct _clidef2 * cli2 = cli;
  cli->cli$b_rqtype=CLI$K_CLISERV;
  cli->cli$w_servcod=CLI$K_DELELCL;
  memcpy(&cli2->cli$q_namdesc,symbol,8);
  sys$cli(cli);
  return SS$_NORMAL;
}

int lib$set_logical(void * logical_name, void * value_string, void * table, long * attributes, void * item_list) {
  struct _clidef1 cli_,*cli=&cli_;
  struct _clidef2 * cli2 = cli;
  cli->cli$b_rqtype=CLI$K_CLISERV;
  cli->cli$w_servcod=CLI$K_CREALOG;
  memcpy(&cli2->cli$q_namdesc,logical_name,8);
  if (value_string)
    memcpy(&cli2->cli$q_valdesc,value_string,8);
  if (table)
    memcpy(&cli2->cli$q_tabdesc,table,8);
  cli2->cli$l_attr=*attributes;
  cli2->cli$l_itmlst=item_list;
  sys$cli(cli);
  return SS$_NORMAL;
}

int lib$delete_logical(void * logical_name, void * table_name) {
  struct _clidef1 cli_,*cli=&cli_;
  struct _clidef2 * cli2 = cli;
  cli->cli$b_rqtype=CLI$K_CLISERV;
  cli->cli$w_servcod=CLI$K_DELELOG;
  memcpy(&cli2->cli$q_namdesc,logical_name,8);
  if (table_name)
    memcpy(&cli2->cli$q_tabdesc,table_name,8);
  sys$cli(cli);
  return SS$_NORMAL;
}

