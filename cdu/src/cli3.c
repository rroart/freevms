#include "cliparse.h"

#include <string.h>

int clilex ( void );

static void
c_lex_one_token (c_token *token)
{
#if 0
  timevar_push (TV_LEX);
  token->type = c_lex_with_flags (&token->value, &token->location, NULL);
#else
  token->type = clilex();
  token->value = yylval.type_node_p;
#endif
}

cli_token(char * str, int * retlen){
  c_token token;
  c_lex_one_token(&token);
  if (token.value.is_integer) {
#ifdef __i386__
    memcpy(str, token.value.string, strlen(token.value.string));
    str[strlen(token.value.string)] = 0;
#if 0
    // check. kernel link problem
    sprintf(str, "%d", token.value.p1);
#endif
#else
    memcpy(str, token.value.string, strlen(token.value.string));
    str[strlen(token.value.string)] = 0;
#endif
    *retlen = strlen(str);
  } else {
    *retlen = token.value.p1;
    memcpy (str, token.value.string, *retlen);
  }
  return token.type;
}

c_parser dcl_parser;

void initparser() {
  memset(&dcl_parser, 0, sizeof(c_parser));
  extern signed int include_stack_ptr;
  include_stack_ptr = 0;
  extern int get_symbol_value;
  get_symbol_value = 0;
}

void * getparser() {
  return &dcl_parser;
}

