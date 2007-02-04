%x lexparam

%{

  /* File lexlex.l
   * The Unknown Scandinavian was here
   * 9 Nov 1990
   *
   * Taken over by
   * Roar Thronæs <roart@nvg.ntnu.no>
   * 7-MAY-2000
   *
   *
   * Lexically analyzing BLISS using lex
   * lex.yy.c is #included in parse.y
   */

#if 0
#include "config.h"
#endif
#include <stdio.h>

#undef IN_GCC

#if 0
#include "system.h"
#include "coretypes.h"

#include "tree.h"

#include "bli-tree.h"

#include "parse.h"

#include "bliss.h"
#endif

#include "cliparse.h"
#include <descrip.h>
#include <ssdef.h>

#define my_isblank(c) ((c) == ' ' || (c) == '\t')
#define my_isblank2(c) ((c) == ' ' || (c) == '\t' || (c) == 0)
#define my_isletter(c) ( ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') || (c)=='$' || (c)=='_')
#define my_isend(c) ((c) == '\n' || (c) == 0)

#define inform(...) printf(__VA_ARGS__)

#if 0
#define get_identifier(x) strdup(x)
#define build_string(x) strdup(x)
#endif

//  void error(),warning(),inform();
  char * my_strcasestr(const char *, const char *);
  char * my_strstrend(const char *, const char *);
  char * my_strstrendbal(const char *, const char *, char , int);

  extern int yydebug;
  /*extern*/ int yy2debug;
  extern int quiet_flag;
  int opexpcnt=0;

#if 0
  char * s;
#endif

  YYSTYPE yylval;
  unsigned int intern_hash_size=0;
  unsigned int estimated_token_count=0;

  int cond_iter_macro_count = 0;
  int cur_act_par = 0;
  int pname_string=0;
  char pname_out[256];
  int pname_len;
  char * use_yytext;
  int * use_yyleng;
  void pname_macro(char *, int);

  char *p_title;
  int undefmode=0;
  int anymode=0;
  int tnamemode=0;
  int macromode=0;
  int if_mode=0;
  int then_mode=0;
  int skip_mode=0;
  int base_skip_mode=0;
  int do_skip=0;
  int last_p=0;
  int one_lexeme=0;
  int balanced_lexemes=0;
  int do_quote=0;

  int blipre_lineno=1;
  int blipre_charno=1;
  int blipre_debug_line=0; 

  int last_eq=0;

  int input_line = 0;

  int get_symbol_value = 0;

  int checkq = 0;

#define LEXINIT_INVALID 3
  int which_lex=LEXINIT_INVALID;

  struct bli_token_struct * first_token=NULL;

  static int keyword (char *);
  static int ukeyword (char *);
  static void downcase (char *);
  void push_req_stack(char *);
  void pop_req_stack(void);
  void push_cond_stack(int);
  void pop_cond_stack(void);
  void yy2error (const char *);
  int isupper(int);
  int islower(int);
  int tolower(int);
  int bli_init_reswords (void);
  int yylex ( void );
  int something_that_uses_yyunput(void);
  void strip_par(char *);
  tree * tree_last_p (tree *);

#define MAX_INCLUDE_DEPTH 40
  YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
  long include_stack_state[MAX_INCLUDE_DEPTH];
  long include_stack_start[MAX_INCLUDE_DEPTH];
  long include_stack_cond[MAX_INCLUDE_DEPTH];
  long include_stack_act_par[MAX_INCLUDE_DEPTH];
  long include_stack_rem[MAX_INCLUDE_DEPTH]; 
  long include_stack_free[MAX_INCLUDE_DEPTH];
 signed int include_stack_ptr = 0;
#define MAX_COND_DEPTH 40
  int cond_stack[MAX_COND_DEPTH];
  int cond_stack_ptr = 0;
#define MAX_MACRO_DEPTH 40
  tree macro_stack[MAX_MACRO_DEPTH];
  char macro_stack_char[MAX_MACRO_DEPTH];
  int macro_stack_ptr=0;
#define MAX_NESTED_MACRO_DEPTH 10
  void* nested_macro_stack_mac[MAX_MACRO_DEPTH];
  tree nested_macro_stack_act[MAX_MACRO_DEPTH];
  int nested_macro_stack_ptr=0;

 enum{SIMP_MACRO,COND_MACRO,ITER_MACRO,KEYW_MACRO};

  int expr_parse = 0;

  int do_builtin = 0;

  char macrostart=0;
  struct mymacro * macr=0;
#if 0
  tree macroact=0;
#endif
  char macrocomma=1;
#if 0
  tree remaining=0;
  tree old_remain=0;
#endif

  int longest_string=0;
  int add_memcpy=0;

  char * opened_req[100];
  char opened_req_ptr=0;

  int quiet_flag = 0;

  int 
	 is_opened(char * file)
	 {
		downcase(file);
		char i;
		for(i=0;i<opened_req_ptr;i++) {
		  if (0==strcmp(file,opened_req[i]))
			 return 1;
		}
		return 0;
	 }

  void
    push_macro(struct dsc$descriptor *dsc)
    {
      char * empty="";
      char * new=dsc->dsc$a_pointer;
      if (new==0)
        new=empty;
      include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
      include_stack_state[include_stack_ptr] = 0; 
      include_stack_start[include_stack_ptr] = YY_START; 
      include_stack_cond[include_stack_ptr] = cond_stack_ptr; 
      include_stack_act_par[include_stack_ptr] = cur_act_par; 
#if 0
      include_stack_rem[include_stack_ptr] = old_remain; 
#endif
      include_stack_free[include_stack_ptr] = dsc->dsc$a_pointer; 
      include_stack_ptr++;

      yy_switch_to_buffer(yy_scan_bytes(new, dsc->dsc$w_length));
    }

  void
    push_req_stack(char *new)
    {
      include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
      include_stack_state[include_stack_ptr] = 0;
      include_stack_start[include_stack_ptr] = YY_START;
      include_stack_cond[include_stack_ptr] = cond_stack_ptr; 
      include_stack_act_par[include_stack_ptr] = 0; 
#if 0
      include_stack_rem[include_stack_ptr] = remaining; 
#endif
      include_stack_ptr++;
      downcase(new);
      opened_req[opened_req_ptr++]=strdup(new);
      yyin = fopen( new, "r" );
     
      yy_switch_to_buffer(yy_create_buffer( yyin, YY_BUF_SIZE ) );
    }

  void
    pop_req_stack(void)
    {
      if ( --include_stack_ptr < 0 )
	{
	  if (yy2debug) inform("\nempty include stack\n");
	}
      else
	{
	  yy_delete_buffer( YY_CURRENT_BUFFER );
	  yy_switch_to_buffer(include_stack[include_stack_ptr] );
	  BEGIN(include_stack_start[include_stack_ptr]);
	}
    }

  void
    push_cond_stack(int new)
    {
      cond_stack[cond_stack_ptr++] = /*base_*/skip_mode;
    }

  void
    pop_cond_stack(void)
    {
      if ( --cond_stack_ptr < 0 )
		  {
			 if (yy2debug) inform("\nempty cond stack\n");
		  }
      else
		  {
			 /*base_*/skip_mode=cond_stack[cond_stack_ptr];
		  }
	 }

  int
    get_cond_stack(void)
    {
      if (cond_stack_ptr==0) fprintf(stderr,"cond_stack_ptr 0\n");
      return cond_stack[cond_stack_ptr-1];
    }

  int
    get_prev_cond_stack(void)
    {
      if (cond_stack_ptr==0) fprintf(stderr,"cond_stack_ptr 0\n");
      return cond_stack[cond_stack_ptr-2];
    }

  void
    push_macro_stack(tree t, char *c)
    {
      macro_stack[macro_stack_ptr] = t;
      macro_stack_char[macro_stack_ptr++] = c;
    }

  tree
    pop_macro_stack(void)
    {
      if ( --macro_stack_ptr < 0 )
		  {
			 if (yy2debug) inform("\nempty cond stack\n");
		  }
      else
		  {
			 return macro_stack[cond_stack_ptr];
		  }
	 }

  char
    get_macro_stack_char(void)
    {
      if (cond_stack_ptr==0) fprintf(stderr,"cond_stack_ptr 0\n");
      return macro_stack_char[macro_stack_ptr-1];
    }

%}

%option never-interactive

idigit		[0-9a-f]
digit		[0-9]
letter		[A-Za-z_$]
string		\"([^\"\n\f]|\"\")*\"
illstring	'([^']|'')*'
name		{letter}({letter}|{digit})*
delim   "="|":"|","|"/"|"."|"<"|"["|"("|"-"|"*"|"+"|"^"|">"|"]"|")"|";"
delim2   "="|":"|","|"."|"<"|"["|"("|"-"|"*"|"+"|"^"|">"|"]"|")"|";"
others	  \"|"#"|"&"|"?"|"@"|\\|"`"|"{"|"|"|"}"|"~"
any        {digit}|{letter}|{delim}|"%"
any2        {digit}|{letter}|{delim2}|"%"
space      [ \t]
/*optarg "+"|"-"
hexdigit [0-9a-f]
*/

%%

  // executed last?
  if (last_eq) {
	 if (*yytext!='=' && !my_isblank2(*yytext))
      last_eq=0;
  }

\n            input_line++;
 { }
\r { }
 { }

{space}+          { }

{delim}				{
  if (*yytext != '/' && checkq) {
    if ( (checkq & 2 ) == 0 || *yytext != '=' ) {
      BEGIN(lexparam);
      yyless(0);
      continue;
    }
  }
#if 0
  if (skip_mode && if_mode == 0) {
	 yyless(yyleng);
    break;
  }
#endif  

  if (last_p && *yytext==')') {
    int last=last_p;
    yyless(yyleng);
    last_p=0;
  }

  if (*yytext=='=')
	 last_eq=1;

  if (*yytext=='+' || *yytext=='-') {
	 // workaround because shift/reduce etc with plain chars
	 if (opexpcnt==0) {
		if (*yytext=='+')
		  return UPLUS;
		else
		  return UMINUS;
	 }
  }

  if (*yytext=='=' || *yytext=='(' || *yytext=='[' || *yytext==',')
    opexpcnt=0; // for unary op workaround
  else {
#if 0
    opexpcnt++;
#endif
  }

  return *yytext;
}

{digit}+			{
#if 0
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
#endif
  long long l = atoll(yytext);
#ifdef __x86_64__
  memcpy(yylval.type_node_p.string, yytext, yyleng);
  yylval.type_node_p.string[yyleng] = 0;
#endif
  yylval.type_node_p.is_integer = 1;
  yylval.type_node_p.p1 = l;

  opexpcnt++; // for unary op workaround
  return T_DIGITS;
}

{name}|"."{name}"."				{
  if (*yytext != '/' && checkq) {
    BEGIN(lexparam);
    yyless(0);
    continue;
  }
 do_name:
#if 0
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
#else
  {}
#endif

  int key=0;

  downcase (yytext);
  if (do_quote) {
    do_quote=0;
    if (macromode)
      goto out_do_quote;
#if 1
    yylval.type_node_p.is_integer = 0;
    yylval.type_node_p.p1 = yyleng;
    memcpy (yylval.type_node_p.string, yytext, yyleng);
#else
    yylval.type_node_p = maybe_get_identifier(yytext);
    if (yylval.type_node_p==0)
      yylval.type_node_p=copy_node(build_string(strlen(yytext),yytext));
#endif
    return T_NAME;
  out_do_quote:
    {}
  }
  if (tnamemode) goto onlytname;
  if (undefmode) { key=ukeyword(yytext); /*undefmode=0;*/ }
  if (key<=0) key = keyword (yytext);
  if (key == -1) 
    {
    onlytname:
#if 1
      yylval.type_node_p.is_integer = 0;
      yylval.type_node_p.p1 = yyleng;
      memcpy (yylval.type_node_p.string, yytext, yyleng);
      //yylval.type_node_p=build_string(strlen(yytext),yytext);
#else
      yylval.type_node_p = maybe_get_identifier(yytext);
      if (yylval.type_node_p==0)
	yylval.type_node_p=copy_node(build_string(strlen(yytext),yytext));
#endif
      opexpcnt++; // for unary op workaround
      if ( ! get_symbol_value )
        return T_NAME;
        extern int vms_mm;
        if (vms_mm) {
	  struct dsc$descriptor sym, val;
	  sym.dsc$a_pointer = yylval.type_node_p.string;
	  sym.dsc$w_length = yylval.type_node_p.p1;
	  char retval[256];
	  memset(retval, 0, 256);
	  val.dsc$w_length = 256;
	  val.dsc$a_pointer = retval;
	  int sts = lib$get_symbol(&sym, &val, 0);
	  if ((sts & 1) == 0)
	    return T_NAME;
	  yyless(sym.dsc$w_length);
	  val.dsc$a_pointer = strdup (retval); // check leak
	  val.dsc$w_length = strlen (retval);
	  push_macro(&val);
	  continue;
	} else {
	  struct dsc$descriptor sym, val;
	  sym.dsc$a_pointer = yylval.type_node_p.string;
	  sym.dsc$w_length = yylval.type_node_p.p1;
	  char retval[256];
	  memset(retval, 0, 256);
	  retval[0]=48+(random()&9);
	  retval[1]=48+(random()&9);
	  val.dsc$w_length = 256;
	  val.dsc$a_pointer = retval;
	  yyless(sym.dsc$w_length);
	  val.dsc$a_pointer = strdup (retval); // check leak
	  val.dsc$w_length = strlen (retval);
	  push_macro(&val);
	  continue;
	}
      return T_NAME;
    }
  yylval.type_node_p.is_integer = 0;
  yylval.type_node_p.p1 = yyleng;
  memcpy (yylval.type_node_p.string, yytext, yyleng);
  opexpcnt=0; // for unary op workaround
  return key;
}

{name}				{
  if (*yytext != '/' && checkq) {
    BEGIN(lexparam);
    yyless(0);
    continue;
  }
 do_name2:
#if 0
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
#else
  {}
#endif

#if 0
  char * line = yytext + 1;
  int linlen = yyleng - 1;
  while (*line==' ')
    line++, yyleng--;
#endif

  int key=0;

  downcase (yytext);
  if (do_quote) {
    do_quote=0;
    if (macromode)
      goto out_do_quote2;
#if 1
#if 1
     yylval.type_node_p.is_integer = 0;
     yylval.type_node_p.p1 = yyleng;
     memcpy (yylval.type_node_p.string, yytext, yyleng);
#else
     yylval.type_node_p.p1 = linlen;
     memcpy (yylval.type_node_p.string, line, linlen);
#endif
#else
    yylval.type_node_p = maybe_get_identifier(yytext);
    if (yylval.type_node_p==0)
      yylval.type_node_p=copy_node(build_string(strlen(yytext),yytext));
#endif
    return T_NAME;
  out_do_quote2:
    {}
  }
  if (tnamemode) goto onlytname2;
  if (undefmode) { key=ukeyword(yytext); /*undefmode=0;*/ }
  if (key<=0) key = keyword (yytext);
  if (key == -1) 
    {
    onlytname2:
#if 1
#if 1
      yylval.type_node_p.is_integer = 0;
      yylval.type_node_p.p1 = yyleng;
      memcpy (yylval.type_node_p.string, yytext, yyleng);
      //yylval.type_node_p=build_string(strlen(yytext),yytext);
#else
     yylval.type_node_p.p1 = linlen;
     memcpy (yylval.type_node_p.string, line, linlen);
#endif
#else
      yylval.type_node_p = maybe_get_identifier(yytext);
      if (yylval.type_node_p==0)
	yylval.type_node_p=copy_node(build_string(strlen(yytext),yytext));
#endif
      opexpcnt++; // for unary op workaround
      return T_QUALIFIER;
    }
  /*yylval.type_str=strdup(yytext);*/
  yylval.type_node_p.is_integer = 0;
  yylval.type_node_p.p1 = yyleng;
  memcpy (yylval.type_node_p.string, yytext, yyleng);
  //yylval.type_node_p=build_string(strlen(yytext),yytext);
  opexpcnt=0; // for unary op workaround
  return T_QUALIFIER;
}

"%"{name}			{
 p_name:
  {}
  int my_yyleng=0;
  signed int key = -3;
  downcase (yytext);
  key = keyword (yytext);
  int key_len = strlen(yytext);

  do_skip=0;
  if (skip_mode && if_mode == 0 && key!=K_IF && key!=K_THEN && key != K_ELSE && key != K_ENDIF) {
#if 0
    yyless(yyleng);
    break;
#else
	 do_skip=1;
#endif
  }

  if (key == -1) {
    goto do_name;
  }

  int retval = 0;
  char * my_yytext=yytext+strlen(yytext);
  char * my_end;
  yyless(strlen(yytext));

  switch (key) {
  case K_IF:
	 {
		my_end=my_strcasestr(my_yytext, "%then");
		int my_yyleng=5+my_end-my_yytext;
		char myline[255];
		memset(myline, 0, 255);
		if (!quiet_flag) printf("my %x %x %x\n",my_end,my_yytext,my_yyleng);
		memcpy(myline,my_yytext,my_end-my_yytext);
		if (!quiet_flag) printf("my2 %s\n",myline);
		//fprintf(stderr,"myline%smyline\n",myline); fflush(stderr);
		//yytext=my_yytext+=my_yyleng;
		int len=my_end-yytext+5;
		yyless(len);
		//yyless(my_yyleng);
		if (!quiet_flag) printf("skipp %x %x %x\n",base_skip_mode,skip_mode,cond_stack_ptr);
		if (skip_mode==0) {
#if 0
		  tree t = 0;
parse_this(myline);
		  t = TREE_VALUE(t);
#else
#if 0 
		  int t = parse_this_expr (myline);
#else
                  int t = 0;
                  exit(42);
#endif
#endif
		  push_cond_stack(0);
#if 1
		  skip_mode=get_cond_stack()|(((long)t) & 1);
#endif
		  push_cond_stack(0);
		} else {
		  push_cond_stack(0);
		  skip_mode=1;
		  push_cond_stack(0);
		}
		goto myout;
	 }
	 break;

  case K_ELSE: {
	 then_mode = 0;
	 skip_mode=get_prev_cond_stack()|!get_cond_stack();
	 //fprintf(stderr,"pel %s\n",yytext); fflush(stderr); 
	 yyless(5/*yyleng*/);
	 //fprintf(stderr,"pel2 %s\n",yytext); fflush(stderr);
	 key = -1;
  }
	 break;

  case K_ENDIF: { /*skip_mode = 0;*/ pop_cond_stack(); pop_cond_stack(); /*yyless(yyleng);*/ yyless(3); key=-1; }
	 break;
}

  if (key>=0)
    return key;

 myout:
  {}
  if (pname_string)
    pname_string=0;
}

{string}			{
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
  struct mymacro * m;
  /* store that string w/o '' */

  yylval.type_node_p.is_integer = 0;
  yylval.type_node_p.p1 = yyleng;
  memcpy(yylval.type_node_p.string, yytext, yyleng);
  return T_STRING;
}

<lexparam>{any2}+			{
  BEGIN(0);
  if (skip_mode && if_mode == 0) {
    yyless(yyleng);
    break;
  }
  struct mymacro * m;
  /* store that string w/o '' */

  yylval.type_node_p.is_integer = 0;
  yylval.type_node_p.p1 = yyleng;
  memcpy(yylval.type_node_p.string, yytext, yyleng);
  return T_NAME;
}

![^\n\f]*[\n\f]			/* ignore trailing comment */ input_line++ ; 

"%("([^\)]|")"[^\%])*")%"	/* ignore embedded comment */  ;

"@" { if (yy2debug) inform("%%lexical gone?\n"); }

{others}			{
#define yy2error printf
  yy2error ("not recognized");
}

<<EOF>> { 
 eof:
  if ( --include_stack_ptr < 0 )
    {
      if (yy2debug) inform("\nempty include stack\n");
      return YY_NULL;
    }
  else
    {
      yy_delete_buffer( YY_CURRENT_BUFFER );
      if (include_stack_free[include_stack_ptr])
        free (include_stack_free[include_stack_ptr]);
      yy_switch_to_buffer(include_stack[include_stack_ptr] );
      if (include_stack_cond[include_stack_ptr] != cond_stack_ptr) {
	if (!quiet_flag)printf("resetting cond_stack_ptr %x %x\n", include_stack_cond[include_stack_ptr], cond_stack_ptr); 
#if 0
	sleep (1);
#endif
	cond_stack_ptr = include_stack_cond[include_stack_ptr]; 
      }
      if (include_stack_act_par[include_stack_ptr] != cur_act_par) {
	if (!quiet_flag)printf("setting cur_act_par %x %x\n", include_stack_act_par[include_stack_ptr], cur_act_par); 
	cur_act_par = include_stack_act_par[include_stack_ptr]; 
      }
#if 0
      remaining = include_stack_rem[include_stack_ptr]; 
#endif
      if (include_stack_state[include_stack_ptr])
	BEGIN(include_stack_start[include_stack_ptr]);
    }
#if 0
  if (expr_parse) {
	 expr_parse--;
	 return END_EXPR;
  }
#else
  if (include_stack_state[include_stack_ptr])
    return END_EXPR;
#endif
  if (do_builtin) {
	 do_builtin--;
	 return END_BUILTIN;
  }
} 

%%


struct dict {
  const char *d_text;
  int	d_num;
};

static void downcase (cp)
     char *cp;
{
  while (cp && *cp) {
    if (isupper (*cp))
      *cp = tolower (*cp);
    cp++;
  }
}

struct dict keywords[] = 
  {
	 { ".and.", K_AND }, /* Normal names */
	 { ".eq.", K_EQ },
	 { ".eqs.", K_EQS },
	 { ".ge.", K_GE },
	 { ".ges.", K_GES },
	 { ".gt.", K_GT },
	 { ".gts.", K_GTS },
	 { ".leq.", K_LE },
	 { ".les.", K_LES },
	 { ".lt.", K_LT },
	 { ".lts.", K_LTS },
	 { ".ne.", K_NE },
	 { ".nes.", K_NES },
	 { ".not.", K_NOT },
	 { ".or.", K_OR },
	 { "else", K_ELSE },
	 { "endif", K_ENDIF },
	 { "if", K_IF },
	 { "then", K_THEN },
	 { NULL, -1 }
  };

int keyw_len(int key) {
  if (!quiet_flag) printf(" KEY %x %s\n",strlen(keywords[key-K_WRITE].d_text),(keywords[key-K_WRITE].d_text));
  return strlen(keywords[key-K_WRITE].d_text);
}

static struct dict ukeywords[] = {
  { "absolute", U_ABSOLUTE },
  { NULL, -1 }
};

#define NKEYW 19
#define NUKEYW 1

static int keyword (cp)
     char *cp;				/* It better be lower case */
{
  int first, last, half, c;

  if (yy2debug) inform("key %x %s\n",(unsigned int)cp,cp);

  first = 0;				/* inclusive */
  last = NKEYW;				/* exclusive */
  last = sizeof keywords / sizeof (struct dict) - 1;
  while (first < last)
    {
      half = (first + last) / 2;
      c = strcmp (cp, keywords[half].d_text);
      if (c == 0)
		  return keywords[half].d_num;
      else if (c < 0)
		  last = half;
      else
		  first = half + 1;
    }
  if (yy2debug) inform("keynot\n");
  return -1;				/* Not a keyword */
}

static int ukeyword (cp)
     char *cp;				/* It better be lower case */
{
  int first, last, half, c;

  if (yy2debug) inform("ukey %x %s\n",(unsigned int)cp,cp);
  first = 0;				/* inclusive */
  last = NUKEYW;				/* exclusive */
  while (first < last)
    {
      half = (first + last) / 2;
      c = strcmp (cp, ukeywords[half].d_text);
      if (c == 0)
		  return ukeywords[half].d_num;
      else if (c < 0)
		  last = half;
      else
		  first = half + 1;
    }
  if (yy2debug) inform("ukeynot\n");
  return -1;				/* Not a keyword */
}

int
yywrap(void)
{
  return 1;
}

tree *bli_ridpointers;
#define N_reswords (sizeof keywords / sizeof (struct dict))
#if 0
int
bli_init_reswords (void)
{
  unsigned int i;
  tree id;
  /* It is not necessary to register ridpointers as a GC root, because
     all the trees it points to are permanently interned in the
     get_identifier hash anyway.  */
  bli_ridpointers = (tree *) xcalloc ((int) REALLY_MAX, sizeof (tree));
  for (i = 0; i < N_reswords && keywords[i].d_num>-1; i++)
    {
      /* If a keyword is disabled, do not enter it into the table
         and so create a canonical spelling that isn't a keyword.  */

      id = get_identifier (keywords[i].d_text);
      bli_ridpointers [(int) keywords[i].d_num] = id;
    }
  return 0;
}
#endif

int something_that_uses_yyunput(void) {
  void (*f)(int c, char *);
  f=yyunput;
  return 0;
}

#if 0
int parse_this_expr (char * this) {
  if (!quiet_flag) printf("parse this %s\n",this);
  expr_parse++;
  include_stack[include_stack_ptr] = YY_CURRENT_BUFFER;
  include_stack_state[include_stack_ptr] = 1;
  include_stack_start[include_stack_ptr] = YY_START;
  include_stack_cond[include_stack_ptr] = cond_stack_ptr; 
  include_stack_act_par[include_stack_ptr] = cur_act_par; 
#if 0
  include_stack_rem[include_stack_ptr] = remaining; 
#endif
  include_stack_ptr++;
  yy_switch_to_buffer(yy_scan_string(this));
  BEGIN(0);
  //tree t = save_last_tree();
  c_parser dcl_parser;
  memset(&dcl_parser, 0, sizeof(c_parser));
#if 0 
  yyparse();
  //restore_last_tree(t);
  return mylast();
#else
  return c_parser_binary_expression3 (&dcl_parser, 0);
#endif
}
#endif
typedef char chartype;

char *
my_strcasestr (phaystack, pneedle)
     const char *phaystack;
     const char *pneedle;
{
  register const unsigned char *haystack, *needle;
  register chartype b, c;

  haystack = (const unsigned char *) phaystack;
  needle = (const unsigned char *) pneedle;

  b = tolower (*needle);
  if (b != '\0')
    {
      haystack--;                               /* possible ANSI violation */
      do
        {
          c = *++haystack;
          if (c == '\0')
            goto ret0;
        }
      while (tolower (c) != (int) b);

      c = tolower (*++needle);
      if (c == '\0')
        goto foundneedle;
      ++needle;
      goto jin;

      for (;;)
        {
          register chartype a;
          register const unsigned char *rhaystack, *rneedle;

          do
            {
              a = *++haystack;
              if (a == '\0')
                goto ret0;
              if (tolower (a) == (int) b)
                break;
              a = *++haystack;
              if (a == '\0')
                goto ret0;
				shloop:
              ;
            }
          while (tolower (a) != (int) b);

		  jin:      a = *++haystack;
          if (a == '\0')
            goto ret0;

          if (tolower (a) != (int) c)
            goto shloop;

          rhaystack = haystack-- + 1;
          rneedle = needle;
          a = tolower (*rneedle);

          if (tolower (*rhaystack) == (int) a)
            do
              {
                if (a == '\0')
                  goto foundneedle;
                ++rhaystack;
                a = tolower (*++needle);
                if (tolower (*rhaystack) != (int) a)
                  break;
                if (a == '\0')
                  goto foundneedle;
                ++rhaystack;
                a = tolower (*++needle);
              }
				while (tolower (*rhaystack) == (int) a);

          needle = rneedle;             /* took the register-poor approach */

          if (a == '\0')
            break;
        }
    }
 foundneedle:
  return (char*) haystack;
 ret0:
  return 0;
}

char * my_strstrend(const char * s, const char * t) {
  char * e = strchr(s, '\n');
  if (e)
	 return e;
  return s + strlen(s);
}

char * my_strstrendbal(const char * s, const char * end, char t, int bal) {
  char * e = end; //strchr(s, '\n');
    if (e==0)
	 e=s+strlen(s);
  if (bal) {
	 signed int mybalcnt = 0;
	 char * c = s;
	 for(;c!=e;c++) {
		if (*c=='(' || *c=='[' || *c=='<')
		  mybalcnt++;
		if (*c==')' || *c==']' || *c=='>') {
		  mybalcnt--;
		  if (mybalcnt<0)
			 return c; // check
		  if (mybalcnt==0 && t==0) // bad shortcut
			 return c+1;
		}
		if (t && t==*c && mybalcnt==0) // bad shortcut
		  return c;
	 }
  }
 get_out:
  return e;
}

change_macromode() {
  if (one_lexeme) {
	 macromode=0;
	 one_lexeme=0;
  }
}

void
strip_par (s)
	  char * s;
{
#if 0
  return;
#endif
  int end = strlen(s)-1;
  int start = 0;
  int alt_end = start+1;
  while (my_isblank(s[end]))
    end--;
  for (;alt_end<end;alt_end++)
    if (s[alt_end]==')')
      return;
  for (;start<end;start++,end--) {
    while (my_isblank(s[start]))
      start++;
    while (my_isblank(s[end]))
      end--;
    if (s[start]=='(' && s[end]==')')
      s[start]=s[end]=' ';
    else
      return;
  }
  return;
#if 0
  if (my_isblank(s[start]))
	 start++;
  if (my_isblank(s[end]))
	 end--;
  int alt_end = start+1;
  for (;alt_end<end;alt_end++)
    if (s[alt_end]==')')
      return;
  if (s[start]=='(' && s[end]==')')
	 s[start]=s[end]=' ';
#endif
}

void
pname_macro(src,len)
     char * src;
     int len;
{
  if (pname_string==0)
    return;
  if (len>250) {
    printf("LEN %x %s\n",len,src);
    exit(84);
  }
  memcpy(pname_out,src,len);
  pname_out[len]=0;
  pname_len=len;
}

check_lib(s) 
     char * s;
{
  // lib.req is really compiled lib and seems to imply starlet, too
  downcase(s);
  if (0==strcmp("lib",s)) {
    char * f="starlet.req";
    if (!is_opened(f))
      push_req_stack(f);
  }
}

do_eof()
{ 
 eof:
  if ( --include_stack_ptr < 0 )
    {
      if (yy2debug) inform("\nempty include stack\n");
      return YY_NULL;
    }
  else
    {
      yy_delete_buffer( YY_CURRENT_BUFFER );
      yy_switch_to_buffer(include_stack[include_stack_ptr] );
      if (include_stack_cond[include_stack_ptr] != cond_stack_ptr) {
	if (!quiet_flag)printf("resetting cond_stack_ptr %x %x\n", include_stack_cond[include_stack_ptr], cond_stack_ptr); 
#if 0
	sleep (1);
#endif
	cond_stack_ptr = include_stack_cond[include_stack_ptr]; 
      }
      if (include_stack_act_par[include_stack_ptr] != cur_act_par) {
	if (!quiet_flag)printf("setting cur_act_par %x %x\n", include_stack_act_par[include_stack_ptr], cur_act_par); 
	cur_act_par = include_stack_act_par[include_stack_ptr]; 
      }
#if 0
      remaining = include_stack_rem[include_stack_ptr]; 
#endif
      if (include_stack_state[include_stack_ptr])
	BEGIN(include_stack_start[include_stack_ptr]);
    }
#if 0
  if (expr_parse) {
	 expr_parse--;
	 return END_EXPR;
  }
#else
  if (include_stack_state[include_stack_ptr])
    return END_EXPR;
#endif
  if (do_builtin) {
	 do_builtin--;
	 return END_BUILTIN;
  }
} 

int htab_create_alloc () {
return 0;
}

#if 0
int getyylval() {
return yylval.type_node_p;
}
#endif

int cliconditional(int key) {
  return key==K_IF || key==K_THEN || key == K_ELSE || key == K_ENDIF;
}

#if 0
void set_start(int value) {
  yy_start = value;
}

char get_yytext() {
  return *yytext;
}
#endif