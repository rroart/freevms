%x macro

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
#include <stdlib.h>

#undef IN_GCC

#if 0
#include "system.h"
#include "coretypes.h"
#endif
#include "tree.h"

#include "parse.h"

  extern int yydebug;
  extern int yy2debug;

  char * s;

  YYSTYPE yylval;
  unsigned int intern_hash_size=0;
  unsigned int estimated_token_count=0;

  char *p_title;
  int undefmode=0;
  int anymode=0;
  int tnamemode=0;

  int blipre_lineno=1;
  int blipre_charno=1;
  int blipre_debug_line=0; 

#define LEXINIT_INVALID 3
  int which_lex=LEXINIT_INVALID;

  struct bli_token_struct * first_token=NULL;

  static int keyword (char *);
  static int ukeyword (char *);
  static void downcase (char *);
  void yy2error (const char *);
  int isupper(int);
  int islower(int);
  int tolower(int);
  int bli_init_reswords (void);
  int yylex ( void );
  int something_that_uses_yyunput(void);

%}

%option never-interactive

idigit		[0-9a-f]
digit		[0-9]
letter		[A-Za-z_$]
string		<([^\n\f<>])*>
oldstring		<([^'\n\f]|'')*>
name		{letter}({letter}|{digit})*
delim   "="|":"|","|"/"|"."|"<"|"["|"("|"-"|"*"|"+"|"^"|">"|"]"|")"|";"
others	  \"|"#"|"&"|"?"|"@"|\\|"`"|"{"|"|"|"}"|"~"
any        {digit}|{letter}|{delim}|"%"
space      [ \t]
/*optarg "+"|"-"
hexdigit [0-9a-f]
*/

%%

\n            input_line++;

"-"               { }
{space}+          { }

{delim}				{

  return *yytext;
}

{digit}+			{
  s=strdup(yytext);
  yylval.type_node_p=copy_node(get_identifier(s));
  return T_NAME; /*T_DIGITS */
}


{name}|"."{name}|"/"{name}				{
  int key=0;

  downcase (yytext);
  if (*yytext=='.') tnamemode=0;
  if (tnamemode) goto onlytname;
  if (undefmode) { key=ukeyword(yytext); /*undefmode=0;*/ }
  if (key<=0) key = keyword (yytext);
  if (key == -1) 
    {
    onlytname:
      s=strdup(yytext);
      yylval.type_node_p=get_identifier(s);
     extern int yydebug;
     if (yydebug) printf("T %s T",s);
      return T_NAME;
    }
  /*yylval.type_str=strdup(yytext);*/
  yylval.type_node_p=get_identifier(yytext);
  return key;
}

"%"{name}			{
  int key;

  downcase (yytext);
  key = keyword (yytext);
  if (key == -1)
    yy2error ("no %");
  switch (key) {
  }
  return key;
}

"%"				{
  yylval.type_str = strdup(yytext);
  return '%';
}

{string}			{
  /* store that string w/o '' */
  s=strdup(yytext);
  yylval.type_node_p=copy_node(get_identifier(s));
  return T_STRING;
}

![^\n\f]*[\n\f]			/* ignore trailing comment */ input_line++ ; 

"%("([^\)]|")"[^\%])*")%"	/* ignore embedded comment */  ;

"@" { if (yy2debug) printf("%%lexical gone?\n"); }

{others}			{
  yy2error ("not recognized");
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
  { ".base", K_BASE },
  { ".end", K_END },
  { ".facility", K_FACILITY },
  { ".ident", K_IDENT },
  { ".listing", K_LISTING },
  { ".literal", K_LITERAL },
  { ".message", K_MESSAGE },
  { ".page", K_PAGE },
  { ".severity", K_SEVERITY },
  { ".title", K_TITLE },
  { NULL, -1 }
};

static struct dict ukeywords[] = {
  { "/fao_count", K_FAOCOUNT },
  { "/prefix", K_PREFIX },
  { "error", K_ERROR },
  { "fatal", K_FATAL },
  { "info", K_INFO },
  { "success", K_SUCCESS },
  { "warning", K_WARNING },
  { NULL, -1 }
};

#define NKEYW 10
#define NUKEYW 7

static int keyword (cp)
     char *cp;				/* It better be lower case */
{
  int first, last, half, c;

  if (yy2debug) printf("key %x %s\n",(unsigned int)cp,cp);

  first = 0;				/* inclusive */
  last = NKEYW;				/* exclusive */
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
  if (yy2debug) printf("keynot\n");
  return -1;				/* Not a keyword */
}

static int ukeyword (cp)
     char *cp;				/* It better be lower case */
{
  int first, last, half, c;

  if (yy2debug) printf("ukey %x %s\n",(unsigned int)cp,cp);
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
  if (yy2debug) printf("ukeynot\n");
  return -1;				/* Not a keyword */
}

int
yywrap(void)
{
  return 1;
}

tree *bli_ridpointers;
#define N_reswords (sizeof keywords / sizeof (struct dict))
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
#if 0
      C_RID_CODE (id) = keywords[i].rid;
      C_IS_RESERVED_WORD (id) = 1;
      TREE_LANG_FLAG_0 (id) = 1;
      id->common.lang_flag_0 = 1;
#endif
      bli_ridpointers [(int) keywords[i].d_num] = id;
    }
  return 0;
}

int something_that_uses_yyunput(void) {
  void (*f)(int c, char *);
  f=yyunput;
  return 0;
}
