%{

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
#include "cld.h"

extern int tnamemode;

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
%token K_DEFINE K_SYNTAX K_VERB K_IDENT K_MODULE
%token K_NOSTATUS K_PROMPT K_DEFAULT K_IMAGE K_CLIFLAGS
%token K_QUALIFIER K_LABEL K_DISALLOW K_ABBREVIATE K_ROUTINE
%token K_FOREIGN K_NODISALLOWS K_IMMEDIATE K_BATCH K_VALUE
%token K_NOPARAMETERS K_SYNONYM K_TYPE K_NEGATABLE K_NONNEGATABLE
%token K_MCROPTDELIM K_PARAMETER K_PREFIX K_CLIROUTINE K_OUTPUTS
%token K_MCRPARSE K_KEYWORD K_MCRIGNORE K_NOQUALIFIERS
%token K_CONCATENATE K_NOCONCATENATE K_IMPCAT K_LIST K_REQUIRED
%token K_AND K_OR K_NOT K_ANY2
%token REALLY_MAX

%type <type_node_p> K_DEFINE K_SYNTAX K_VERB K_IDENT K_MODULE
%type <type_node_p> K_NOSTATUS K_PROMPT K_DEFAULT K_IMAGE K_CLIFLAGS
%type <type_node_p> K_QUALIFIER K_LABEL K_DISALLOW K_ABBREVIATE K_ROUTINE
%type <type_node_p> K_FOREIGN K_NODISALLOWS K_IMMEDIATE K_BATCH K_VALUE
%type <type_node_p> K_NOPARAMETERS K_SYNONYM K_TYPE K_NEGATABLE K_NONNEGATABLE
%type <type_node_p> K_MCROPTDELIM K_PARAMETER K_PREFIX K_CLIROUTINE K_OUTPUTS
%type <type_node_p> K_MCRPARSE K_KEYWORD K_MCRIGNORE K_NOQUALIFIERS
%type <type_node_p> K_CONCATENATE K_NOCONCATENATE K_IMPCAT K_LIST K_REQUIRED
%type <type_node_p> keyword_clause name_list_or_zero cli_flag_list
%type <type_node_p> value_clauses qualifier_clause clauses
%type <type_node_p> name_list cli_flag
%type <type_node_p> value_clause statements statement
%type <type_node_p> define_syntax define_type define_verb mystart
%type <type_node_p> verb_clause_list type_clause_list keyword_clause_list
%type <type_node_p> bool_expr bool_expr_list bool_expr_list2 parameter_clause
%type <type_node_p> verb_clause any_list type_clause parameter_clause_list
%type <type_node_p> qualifier_clause_list_start qualifier_clause_list
%type <type_node_p> parameter_clause_list_start

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
statements statement { $$ = chainon ($1, $2); }
;

statement: 
define_syntax
|
define_type
|
define_verb
|
K_IDENT T_NAME
|
K_MODULE T_NAME
;

define_syntax:
K_DEFINE K_SYNTAX T_NAME verb_clause_list
;

define_type:
K_DEFINE K_TYPE T_NAME type_clause_list
;

define_verb:
K_DEFINE K_VERB T_NAME verb_clause_list
{ $$ = build_nt(DEFINE_VERB_STMT,$3,$4); }
;

verb_clause_list:
verb_clause
|
verb_clause_list verb_clause { $$ = chainon ($1, $2); }
;

type_clause_list:
type_clause
|
type_clause_list type_clause { $$ = chainon ($1, $2); }
;

verb_clause:
K_CLIFLAGS cli_flag_list
|
K_CLIROUTINE T_NAME
|
K_DISALLOW bool_expr_list
|
K_IMAGE T_NAME
{ $$ = build_nt(IMAGE_CLAUSE, $2); }
|
K_NODISALLOWS
|
K_NOPARAMETERS
|
K_NOQUALIFIERS
|
K_OUTPUTS name_list_or_zero
|
K_PARAMETER T_NAME parameter_clause_list_start
{
  $$ = build_nt(PARAMETER_CLAUSE, $2);
}
|
K_PREFIX T_NAME
|
K_QUALIFIER
{
  tnamemode=1; 
}
T_NAME
{
  tnamemode=0; 
}
qualifier_clause_list_start
{
  $$ = build_nt(QUALIFIER_CLAUSE, $3);
}
|
K_ROUTINE T_NAME
{ $$ = build_nt(ROUTINE_CLAUSE, $2); }
|
K_SYNONYM T_NAME
;

qualifier_clause_list_start:
|
',' qualifier_clause_list { $$ = $2; }
;

qualifier_clause_list:
qualifier_clause_list ',' qualifier_clause { $$ = chainon ($1, $3); }
|
qualifier_clause
;

type_clause:
K_KEYWORD
{
  tnamemode=1; 
}
T_NAME
{
  tnamemode=0; 
}
',' keyword_clause_list
;

keyword_clause_list:
keyword_clause_list ',' keyword_clause { $$ = chainon ($1, $3); }
|
keyword_clause
;

parameter_clause_list_start:
{
  $$ = 0;
}
|
',' parameter_clause_list
{
  $$ = $2;
}
;

parameter_clause_list:
parameter_clause_list ',' parameter_clause { $$ = chainon ($1, $3); }
|
parameter_clause
;

parameter_clause:
K_PROMPT '=' T_STRING
|
clauses
;

qualifier_clause:
K_BATCH
|
K_NEGATABLE
|
K_NONNEGATABLE
|
clauses
;

clauses:
K_CLIFLAGS cli_flag_list
|
K_DEFAULT
|
K_LABEL '='
{
  tnamemode=1; 
}
T_NAME
{
  tnamemode=0; 
}
|
K_SYNTAX '=' T_NAME
|
K_VALUE '(' value_clauses ')'
;

cli_flag:
K_ABBREVIATE
|
K_FOREIGN
|
K_IMMEDIATE
|
K_MCRIGNORE
|
K_MCROPTDELIM
|
K_MCRPARSE
|
K_NOSTATUS
;

keyword_clause:
K_NEGATABLE
|
K_NONNEGATABLE
|
clauses
;

name_list_or_zero:
name_list
|
{}
;

name_list:
name_list T_NAME { $$ = chainon ($1, $2); }
|
T_NAME
;

cli_flag_list:
cli_flag_list ',' cli_flag { $$ = chainon ($1, $3); }
|
cli_flag
;

value_clauses:
value_clauses ',' value_clause { $$ = chainon ($1, $3); }
|
value_clause
;

value_clause:
K_CONCATENATE
|
K_NOCONCATENATE
|
K_DEFAULT '='
{
  tnamemode=1; 
}
name_or_string
{
  tnamemode=0; 
}
|
K_IMPCAT
|
K_LIST
|
K_REQUIRED
|
K_TYPE '=' T_NAME
;

bool_expr_list:
bool_expr_list
K_OR
{
  tnamemode=1; 
}
bool_expr_list2
|
bool_expr_list2
;

bool_expr_list2:
bool_expr_list2
K_AND
{
  tnamemode=1; 
}
bool_expr
|
bool_expr
;

bool_expr:
'('
{
  tnamemode=0; 
}
bool_expr_list
')'
{ $$ = $3; }
|
K_NOT
{
  tnamemode=1; 
}
T_NAME
{
  tnamemode=0; 
}
|
T_NAME
{
  tnamemode=0; 
}
|
K_ANY2 '(' any_list ')'
{
  tnamemode=0; 
}
;

any_list:
any_list ',' T_NAME { $$ = chainon ($1, $3); }
|
T_NAME
;

name_or_string:
T_NAME
|
T_STRING
;

%%

yyerror (char *s)
{
  if (s) error("\n\n%s\n",s); 
  error("Nu b;lev det fel %d\n",input_location.line);
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
