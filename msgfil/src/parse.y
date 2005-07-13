%{

// $Id$
// $Locker$

// Author. Roar Thronæs.

int yy2debug=0;
int yydebug=0;

#define PARAMS(x) x
#define ATTRIBUTE_UNUSED

#define YYERROR_VERBOSE
#define YYDEBUG 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "msg.h"

extern int tnamemode;
extern int undefmode;

tree root_tree;

location_t input_location;

#define symbtabinfo
  typedef int nodeAttr_t;

  tree  creatnode (nodeAttr_t, tree, tree);
  tree  creatid (char *);
  tree  creatvalue (int);
  tree  ForExpand(char *, int, int, tree);
//  int      yyerror (char *);
  //  int input_location.line=1;

#define DSC$K_DTYPE_T 14
#define DSC$K_CLASS_S 1

 struct dsc$descriptor {
	unsigned short        dsc$w_length;   
	unsigned char dsc$b_dtype;    
	unsigned char dsc$b_class;    
	void          *dsc$a_pointer; 
 };
 // has not got descrip.h (yet)

 char * fac_name = 0;
 char * pref_name = 0;
 char * ident_name = 0;
 int severity = 0;
 int control = 0;
 int fac_num = 0;
 int msg_num = 0;

 const int MSG = 3;
 const int FAC = 16;
 const int CTL = 28;

%}

%start mystart

%union {
  int type_int;
  char     *type_str;
  tree  type_node_p;
  int type_node_code;
  location_t location;
  enum tree_code code;
}

/*%token_table yacc*/
%token <type_node_p> T_DIGITS
%token <type_node_p> T_NAME T_STRING T_IDIGITS LEXEME M_NAME

/*%light uplus uminus*/
%right <type_node_code> '='
%left <type_node_code> K_EQV K_XOR
%left <type_node_code> K_OR
%left <type_node_code> K_AND
%right <type_node_code> K_NOT
%left <type_node_code> K_EQL  K_EQLA  K_EQLU  K_NEQ  K_NEQA  K_NEQU K_LSS  K_LSSA  K_LSSU  K_LEQ  K_LEQA  K_LEQU K_GTR  K_GTRA  K_GTRU  K_GEQ  K_GEQA  K_GEQU
%left  <type_node_code> '+' '-'
%left <type_node_code> K_MOD '*' '/'
%left <type_node_code> '^'
%right <type_node_code> UMINUS UPLUS
%right <type_node_code> '.'
%right <type_node_code> UNARY
/*%token apo ''' yacc*/
/* You won't see these tokens
* %token EOF NIL ECOMM TCOMM
* %token ASSIGN COLON COMMA DIGITS DIV DOT LANG LBRACK LPAREN
* %token MINUS MUL NAME PERC PLUS POW RANG RBRACK RPAREN SEMI STRING
*/

/* These tokens will be recognized by the lexical analyzer */
/*%token T_DIGITS T_NAME T_STRING*/

/* These tokens are reserved keywords */
%token K_BASE K_FACILITY K_END K_IDENT K_LISTING K_LITERAL
%token K_MESSAGE K_SEVERITY K_TITLE K_PAGE
%token K_SUCCESS K_INFO K_WARNING K_ERROR K_FATAL
%token K_FAOCOUNT K_PREFIX
%token REALLY_MAX

%type <type_node_p> K_BASE K_FACILITY K_END K_IDENT K_LISTING K_LITERAL 
%type <type_node_p> K_MESSAGE K_SEVERITY K_TITLE K_PAGE
%type <type_node_p> K_SUCCESS K_INFO K_WARNING K_ERROR K_FATAL
%type <type_node_p> K_FAOCOUNT K_PREFIX 
%type <type_node_p> statements statement directive message
%type <type_node_p> maybe_fac_switch maybe_message_switch message_switch
%type <type_node_p> fac_switch name_list severity

%{
%}

%%

/**** 1.0 MODULES ***************************************************/

mystart: statements
{
  if (yydebug) printf ("\n%%CDU-I-PARSED-OK-That's a module alright\n");
  if (yydebug) printf ("\n%% %xH linex\n",input_location.line);
  root_tree = $1;
}
;

statements:
statement
|
statements statement
;

statement: 
directive
|
message
;

directive:
K_BASE T_NAME
{
  int msg_num = atoi(IDENTIFIER_POINTER(($2)));
}
|
K_FACILITY T_NAME ',' T_NAME 
{
  undefmode = 1;
}
maybe_fac_switch
{
  undefmode = 0;
  fac_name = strdup (IDENTIFIER_POINTER($2));
  fac_num = atoi(IDENTIFIER_POINTER($4));
  char name[255];
  if ($6) {
    pref_name = strdup (IDENTIFIER_POINTER(TREE_OPERAND($6,1)));
  } else {
    char name[255];
    sprintf(name,"%s$_",fac_name);
    pref_name = strdup(name);
  }
}
|
K_END
|
K_IDENT T_NAME
{
  ident_name = strdup (IDENTIFIER_POINTER($2));
}
|
K_LISTING
|
K_LITERAL
|
K_MESSAGE
|
K_SEVERITY
{
  undefmode = 1;
}
severity
{
  undefmode = 0;
  severity = $3;
}
|
K_TITLE { tnamemode = 1; } name_list
;

maybe_fac_switch:
{
  $$ = 0
}
|
fac_switch
;

maybe_message_switch:
{
  $$ = 0;
}
|
message_switch
;

message_switch:
K_FAOCOUNT '=' T_NAME
{
  $$ = 0;
}
;

fac_switch:
K_PREFIX '=' T_NAME
{
  $$ = build_nt(PREFIX, $1, $3);
}
;

severity:
K_FATAL
{
  $$ = 4;
}
|
K_ERROR
{
  $$ = 2;
}
|
K_SUCCESS
{ 
  $$ = 1;
}
|
K_INFO
{
  $$ = 3;
}
|
K_WARNING
{
  $$ = 0;
}
;

message:
T_NAME T_STRING
{
  undefmode = 1;
}
maybe_message_switch
{
  undefmode = 0;
  char * s = IDENTIFIER_POINTER($1);
  int num = (control<<CTL)|(fac_num<<FAC)|(msg_num<<MSG)|severity;
  extern int out;
  fprintf(out, "const int %s%s = 0x%x;\n",pref_name,s,num);
  msg_num++;
}
;

name_list:
name_list T_NAME { $$ = chainon ($1, copy_node($2)); }
|
T_NAME
;

%%

yyerror (char *s)
{
  if (s) fprintf(stderr, "\n\n%s\n",s); 
  fprintf(stderr, "Nu b;lev det fel %d\n",input_location.line);
}

void 
parse_init ()
{
  //z1zin=fopen(input_filename,"r");
  //current_token=NULL;
  //first_available_token=first_token;
#ifdef YYDEBUG
#else
  if (parser_trace_flag) 
    {
#if 0
            assert(0 && "lll; parser trace cannot be enabled - blicprs.y not compiled with YYDEBUG set");
#endif
      fprintf(stderr, "lll; parser trace cannot be enabled - blicprs.y not compiled with YYDEBUG set");
    }
#endif

}

void yy2error (const char *s)
{
  printf("yyerror\n");
  if (s) error("\n\n%s\n",s); 
  error("Nu b;lev det fel %d\n",input_location.line);
}
