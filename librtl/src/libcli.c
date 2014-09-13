// $Id$
// $Locker$

// Author. Roar Thronæs.

#include <clidef.h>
#include <cliservdef.h>
#include <descrip.h>
#include <ssdef.h>
#include <starlet.h>

#include <string.h>

int lib$set_symbol(void * symbol , void * value_string, long * table_type_indicator) {
  struct _clidef1 cli_,*cli=&cli_;
  struct _clidef2 * cli2 = cli;
  cli->cli$b_rqtype=CLI$K_CLISERV;
  cli->cli$w_servcod=CLI$K_DEFLOCAL;
  memcpy(&cli2->cli$q_namdesc,symbol, sizeof(struct dsc$descriptor));
  memcpy(&cli2->cli$q_valdesc,value_string, sizeof(struct dsc$descriptor));
  sys$cli(cli, 0, 0);
  return SS$_NORMAL;
}

int lib$get_symbol(void * symbol , void * resultant_string, short * resultant_length, long * table_type_indicator) {
  struct _clidef1 cli_,*cli=&cli_;
  struct _clidef2 * cli2 = cli;
  cli->cli$b_rqtype=CLI$K_CLISERV;
  cli->cli$w_servcod=CLI$K_GETSYM;
  memcpy(&cli2->cli$q_namdesc,symbol, sizeof(struct dsc$descriptor));
  memcpy(&cli2->cli$q_valdesc,resultant_string, sizeof(struct dsc$descriptor));
  return sys$cli(cli, 0, 0);
}

int lib$delete_symbol(void * symbol, long * table_type_indicator) {
  struct _clidef1 cli_,*cli=&cli_;
  struct _clidef2 * cli2 = cli;
  cli->cli$b_rqtype=CLI$K_CLISERV;
  cli->cli$w_servcod=CLI$K_DELELCL;
  memcpy(&cli2->cli$q_namdesc,symbol, sizeof(struct dsc$descriptor));
  sys$cli(cli, 0, 0);
  return SS$_NORMAL;
}

int lib$set_logical(void * logical_name, void * value_string, void * table, long * attributes, void * item_list) {
  struct _clidef1 cli_,*cli=&cli_;
  struct _clidef2 * cli2 = cli;
  cli->cli$b_rqtype=CLI$K_CLISERV;
  cli->cli$w_servcod=CLI$K_CREALOG;
  memcpy(&cli2->cli$q_namdesc,logical_name, sizeof(struct dsc$descriptor));
  if (value_string)
    memcpy(&cli2->cli$q_valdesc,value_string, sizeof(struct dsc$descriptor));
  if (table)
    memcpy(&cli2->cli$q_tabdesc,table, sizeof(struct dsc$descriptor));
  cli2->cli$l_attr=*attributes;
  cli2->cli$l_itmlst=item_list;
  sys$cli(cli, 0, 0);
  return SS$_NORMAL;
}

int lib$delete_logical(void * logical_name, void * table_name) {
  struct _clidef1 cli_,*cli=&cli_;
  struct _clidef2 * cli2 = cli;
  cli->cli$b_rqtype=CLI$K_CLISERV;
  cli->cli$w_servcod=CLI$K_DELELOG;
  memcpy(&cli2->cli$q_namdesc,logical_name, sizeof(struct dsc$descriptor));
  if (table_name)
    memcpy(&cli2->cli$q_tabdesc,table_name, sizeof(struct dsc$descriptor));
  sys$cli(cli, 0, 0);
  return SS$_NORMAL;
}

