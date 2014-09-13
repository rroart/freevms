// $Id$
// $Locker$

// Author. Roar Thronæs.

// Borrowed lots of stuff from gcc-4.1.1 c-parser.c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../cdu/src/cli.h"
#include "../../cdu/src/cliparse.h"
#include <descrip.h>
#include <ssdef.h>

static struct c_expr c_parser_unary_expression (c_parser *parser);
struct c_expr parser_build_binary_op(long a, struct c_expr b, struct c_expr c);
struct c_expr parser_build_unary_op(long a, struct c_expr b);
static struct c_expr c_parser_postfix_expression (c_parser *parser);
static struct c_expr c_parser_expression (c_parser *);
static struct c_expr c_parser_postfix_expression_after_primary (c_parser *parser, struct c_expr expr);

tree c_objc_common_truthvalue_conversion() {}

tree default_conversion() {}

int gcc_assert() {}

int in_system_header = 0;

int objc_need_raw_identifier = 0;

int objc_pq_context = 0;

int skip_evaluation = 0;

int truthvalue_false_node = 0;

int truthvalue_true_node = 0;

/* Read in and lex a single token, storing it in *TOKEN.  */

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
  token->in_system_header = in_system_header;
  switch (token->type)
    {
    case T_NAME:
    case CPP_NAME:
      token->id_kind = C_ID_NONE;
      token->keyword = RID_MAX;
      {
	tree decl;

	int objc_force_identifier = objc_need_raw_identifier;
#if 0
	OBJC_NEED_RAW_IDENTIFIER (0);
#endif

	if (0/*C_IS_RESERVED_WORD (token->value)*/)
	  {
	    enum rid rid_code = 0/*C_RID_CODE (token->value)*/;

#if 0
	    if (c_dialect_objc ())
	      {
		if (!OBJC_IS_AT_KEYWORD (rid_code)
		    && (!OBJC_IS_PQ_KEYWORD (rid_code) || objc_pq_context))
		  {
		    /* Return the canonical spelling for this keyword.  */
		    token->value = ridpointers[(int) rid_code];
		    token->type = CPP_KEYWORD;
		    token->keyword = rid_code;
		    break;
		  }
	      }
	    else
#endif
	      {
		/* Return the canonical spelling for this keyword.  */
#if 0
		token->value = ridpointers[(int) rid_code];
#endif
		token->type = CPP_KEYWORD;
		token->keyword = rid_code;
		break;
	      }
	  }

#if 0
	decl = lookup_name (token->value);
	if (decl)
	  {
	    if (TREE_CODE (decl) == TYPE_DECL)
	      {
		token->id_kind = C_ID_TYPENAME;
		break;
	      }
	  }
#if 0
	else if (c_dialect_objc ())
	  {
	    tree objc_interface_decl = objc_is_class_name (token->value);
	    /* Objective-C class names are in the same namespace as
	       variables and typedefs, and hence are shadowed by local
	       declarations.  */
	    if (objc_interface_decl
		&& (global_bindings_p ()
		    || (!objc_force_identifier && !decl)))
	      {
		token->value = objc_interface_decl;
		token->id_kind = C_ID_CLASSNAME;
		break;
	      }
	  }
#endif
#endif
      }
      token->id_kind = C_ID_ID;
      break;
#if 0
    case CPP_AT_NAME:
      /* This only happens in Objective-C; it must be a keyword.  */
      token->type = CPP_KEYWORD;
      token->id_kind = C_ID_NONE;
      token->keyword = C_RID_CODE (token->value);
      break;
#endif
    case ':':
    case ',':
    case ')':
    case ';':
    case CPP_COLON:
    case CPP_COMMA:
    case CPP_CLOSE_PAREN:
    case CPP_SEMICOLON:
      /* These tokens may affect the interpretation of any identifiers
	 following, if doing Objective-C.  */
#if 0
      OBJC_NEED_RAW_IDENTIFIER (0);
#endif
      token->id_kind = C_ID_NONE;
      token->keyword = RID_MAX;
      break;
    default:
      token->id_kind = C_ID_NONE;
      token->keyword = RID_MAX;
      break;
    }
#if 0
  timevar_pop (TV_LEX);
#endif
}

/* Return a pointer to the next token from PARSER, reading it in if
   necessary.  */

static inline c_token *
c_parser_peek_token (c_parser *parser)
{
  if (parser->tokens_avail == 0)
    {
      c_lex_one_token (&parser->tokens[0]);
      parser->tokens_avail = 1;
    }
  return &parser->tokens[0];
}

/* Consume the next token from PARSER.  */

static void
c_parser_consume_token (c_parser *parser)
{
  if (parser->tokens_avail == 2)
    parser->tokens[0] = parser->tokens[1];
  else
    {
      gcc_assert (parser->tokens_avail == 1);
      gcc_assert (parser->tokens[0].type != CPP_EOF);
    }
  parser->tokens_avail--;
}

/* Return true if the next token from PARSER has the indicated
   TYPE.  */

static inline bool
c_parser_next_token_is (c_parser *parser, enum cpp_ttype type)
{
  return c_parser_peek_token (parser)->type == type;
}

/* Return true if the next token from PARSER does not have the
   indicated TYPE.  */

static inline bool
c_parser_next_token_is_not (c_parser *parser, enum cpp_ttype type)
{
  return !c_parser_next_token_is (parser, type);
}

/* Like c_parser_require, except that tokens will be skipped until the
   desired token is found.  An error message is still produced if the
   next token is not as expected.  If MSGID is NULL then a message has
   already been produced and no message will be produced this
   time.  */

/* If the next token is of the indicated TYPE, consume it.  Otherwise,
   issue the error MSGID.  If MSGID is NULL then a message has already
   been produced and no message will be produced this time.  Returns
   true if found, false otherwise.  */

static bool
c_parser_require (c_parser *parser,
		  enum cpp_ttype type,
		  const char *msgid)
{
  if (c_parser_next_token_is (parser, type))
    {
      c_parser_consume_token (parser);
      return true;
    }
  else
    {
#if 0
      c_parser_error (parser, msgid);
#endif
      return false;
    }
}

static void
c_parser_skip_until_found (c_parser *parser,
			   enum cpp_ttype type,
			   const char *msgid)
{
  unsigned nesting_depth = 0;

  if (type == CPP_CLOSE_PAREN)
    type = ')';

  if (c_parser_require (parser, type, msgid))
    return;

  /* Skip tokens until the desired token is found.  */
  while (true)
    {
      /* Peek at the next token.  */
      c_token *token = c_parser_peek_token (parser);
      /* If we've reached the token we want, consume it and stop.  */
      if (token->type == type && !nesting_depth)
	{
	  c_parser_consume_token (parser);
	  break;
	}
      /* If we've run out of tokens, stop.  */
      if (token->type == CPP_EOF)
	return;
      if (token->type == 0)
	return;
      if (token->type == CPP_OPEN_BRACE
	  || token->type == CPP_OPEN_PAREN
	  || token->type == CPP_OPEN_SQUARE)
	++nesting_depth;
      else if (token->type == CPP_CLOSE_BRACE
	       || token->type == CPP_CLOSE_PAREN
	       || token->type == ')'
	       || token->type == CPP_CLOSE_SQUARE)
	{
	  if (nesting_depth-- == 0)
	    break;
	}
      /* Consume this token.  */
      c_parser_consume_token (parser);
    }
  parser->error = false;
}

struct c_expr c_parser_cast_expression(void * l, void * m) {
  return c_parser_unary_expression(l);
}

struct c_expr default_function_array_conversion(struct c_expr l) {
  return l;
}

static struct c_expr
c_parser_unary_expression (c_parser *parser)
{
  int ext;
  struct c_expr ret, op;
  switch (c_parser_peek_token (parser)->type)
    {
    case '+':
    case CPP_PLUS:
      c_parser_consume_token (parser);
      op = c_parser_cast_expression (parser, NULL);
      op = default_function_array_conversion (op);
      return parser_build_unary_op (CONVERT_EXPR, op);
    case '-':
    case CPP_MINUS:
      c_parser_consume_token (parser);
      op = c_parser_cast_expression (parser, NULL);
      op = default_function_array_conversion (op);
      return parser_build_unary_op (NEGATE_EXPR, op);
    case K_NOT: // check. violates priority?
    case CPP_COMPL:
      c_parser_consume_token (parser);
      op = c_parser_cast_expression (parser, NULL);
      op = default_function_array_conversion (op);
      return parser_build_unary_op (BIT_NOT_EXPR, op);
    case CPP_NOT:
      c_parser_consume_token (parser);
      op = c_parser_cast_expression (parser, NULL);
      op = default_function_array_conversion (op);
      return parser_build_unary_op (TRUTH_NOT_EXPR, op);
    default:
      return c_parser_postfix_expression (parser);
    }
}

static struct c_expr
c_parser_postfix_expression (c_parser *parser)
{
  struct c_expr expr, e1, e2, e3;
  struct c_type_name *t1, *t2;
  switch (c_parser_peek_token (parser)->type)
    {
    case T_DIGITS:
    case CPP_NUMBER:
    case CPP_CHAR:
    case CPP_WCHAR:
      expr.value = c_parser_peek_token (parser)->value;
      expr.original_code = ERROR_MARK;
      c_parser_consume_token (parser);
      break;
    case T_STRING:
    case CPP_STRING:
    case CPP_WSTRING:
      expr.value = c_parser_peek_token (parser)->value;
      expr.original_code = STRING_CST;
      c_parser_consume_token (parser);
      break;
    case T_NAME:
    case CPP_NAME:
      {
	tree id = c_parser_peek_token (parser)->value;
	location_t loc = c_parser_peek_token (parser)->location;
	c_parser_consume_token (parser);
	expr.value = id;
	expr.original_code = ERROR_MARK;
      }
      break;
    case '(':
    case CPP_OPEN_PAREN:
      /* A parenthesized expression, statement expression or compound
	 literal.  */
	{
	  /* A parenthesized expression.  */
	  c_parser_consume_token (parser);
	  expr = c_parser_expression (parser);
#if 0
	  if (TREE_CODE (expr.value) == MODIFY_EXPR)
	    TREE_NO_WARNING (expr.value) = 1;
	  expr.original_code = ERROR_MARK;
#endif
	  c_parser_skip_until_found (parser, CPP_CLOSE_PAREN,
				     "expected %<)%>");
	}
      break;
    case CPP_KEYWORD:
      switch (c_parser_peek_token (parser)->keyword)
	{
	default:
#if 0
	  c_parser_error (parser, "expected expression");
	  expr.value = error_mark_node;
#endif
	  expr.original_code = ERROR_MARK;
	  break;
	}
      break;
      /* Else fall through to report error.  */
    default:
#if 0
      c_parser_error (parser, "expected expression");
      expr.value = error_mark_node;
#endif
      expr.original_code = ERROR_MARK;
      break;
    }
  return c_parser_postfix_expression_after_primary (parser, expr);
}

static struct c_expr
c_parser_postfix_expression_after_primary (c_parser *parser,
					   struct c_expr expr)
{
  tree ident, idx, exprlist;
  while (true)
    {
      switch (c_parser_peek_token (parser)->type)
	{
	case CPP_OPEN_PAREN:
	  /* Function call.  */
	  c_parser_consume_token (parser);
#if 0
	  if (c_parser_next_token_is (parser, CPP_CLOSE_PAREN))
	    exprlist = NULL_TREE;
	  else
#endif
#if 0
	    exprlist = c_parser_expr_list (parser, true);
#endif
	  c_parser_skip_until_found (parser, CPP_CLOSE_PAREN,
				     "expected %<)%>");
#if 0
	  expr.value = build_function_call (expr.value, exprlist);
#endif
	  expr.original_code = ERROR_MARK;
	  break;
	default:
	  return expr;
	}
    }
}

static int get_integer (struct c_expr op) {
  if (op.value.is_integer)
    return op.value.p1;
  return 0;
}

static int get_integer_all (struct c_expr op) {
  if (op.value.is_integer)
    return op.value.p1;
  char s[16];
  memcpy(s, op.value.string, op.value.p1);
  s[((long)op.value.p1) - 1] = 0;
  return atoi(s + 1);
}

static get_string_all (struct c_expr op, char * c) {
  if (!op.value.is_integer)
    memcpy (c, op.value.string, op.value.p1);
  else
    sprintf(c,"%c%d%c\n",'"',op.value.p1,'"');
}

struct c_expr parser_build_binary_op(long code, struct c_expr op1, struct c_expr op2) {
  int is_integer = op1.value.is_integer | op2.value.is_integer;
  struct c_expr c;
#if 0
  c.value = malloc (sizeof (struct struct_tree));
#endif
  switch (code) {
  case PLUS_EXPR:
  case '+':
    {
      c.value.is_integer = is_integer;
      if (is_integer) {
	c.value.is_integer = 1;
	c.value.p1 = get_integer (op1) + get_integer (op2);
      } else {
	int len = ((long)op1.value.p1) + ((long)op2.value.p1) - 2;
	char s[len + 1];
	s[0] = '"';
	s[len - 1] = '"';
	s[len] = 0;
	memcpy (s + 1, ((long)op1.value.string) + 1, ((long)op1.value.p1) - 2);
	memcpy (s + ((long)op1.value.p1) - 1, ((long)op2.value.string) + 1, ((long)op2.value.p1) - 2);
	c.value.p1 = len;
	memcpy (c.value.string, s, len);
      }
    }
    break;
  case MINUS_EXPR:
  case '-':
    {
      c.value.is_integer = is_integer;
      if (is_integer) {
	c.value.is_integer = 1;
	c.value.p1 = get_integer (op1) - get_integer (op2);
      } else {
	c = op1;
	char needle[op2.value.p1 + 1];
	memcpy(needle, op2.value.string, op2.value.p1);
	needle[((long)op2.value.p1) - 1] = 0;
	char * found = strstr (c.value.string, needle + 1);
	if (found) {
	  char * s = c.value.string;
	  int len1 = op1.value.p1;
	  int len2 = op2.value.p1;
	  memcpy (found, found + len2 - 2, len1 - (found - s) - (len2 - 2));
	  c.value.p1 = c.value.p1 - len2 - 2; // - (found - s);
	  //memcpy (c.value.string, op1.value.string, c.value.p1);
	}
      }
    }
    break;
  case MULT_EXPR:
  case '*':
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	c.value.is_integer = 1;
	c.value.p1 = get_integer_all (op1) * get_integer_all (op2);
      }
    }
    break;
  case TRUNC_DIV_EXPR:
  case '/':
      {
      }
      break;
  case BIT_IOR_EXPR:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	c.value.is_integer = 1;
	c.value.p1 = get_integer_all (op1) | get_integer_all (op2);
      }
    }
    break;
  case BIT_AND_EXPR:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	c.value.is_integer = 1;
	c.value.p1 = get_integer_all (op1) & get_integer_all (op2);
      }
    }
    break;
  case EQ_EXPR:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	if (get_integer_all (op1) == get_integer_all (op2))
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case NE_EXPR:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	if (get_integer_all (op1) != get_integer_all (op2))
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case GE_EXPR:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	if (get_integer_all (op1) >= get_integer_all (op2))
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case GT_EXPR:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	if (get_integer_all (op1) > get_integer_all (op2))
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case LE_EXPR:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	if (get_integer_all (op1) <= get_integer_all (op2))
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case LT_EXPR:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	if (get_integer_all (op1) < get_integer_all (op2))
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case K_EQS:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	char s1[128];
	char s2[128];
	get_string_all (op1, s1);
	get_string_all (op2, s2);
	if (strcmp (s1, s2) == 0)
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case K_NES:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	char s1[128];
	char s2[128];
	get_string_all (op1, s1);
	get_string_all (op2, s2);
	if (strcmp (s1, s2))
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case K_LES:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	char s1[128];
	char s2[128];
	get_string_all (op1, s1);
	get_string_all (op2, s2);
	if (strcmp (s1, s2) <= 0)
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case K_LTS:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	char s1[128];
	char s2[128];
	get_string_all (op1, s1);
	get_string_all (op2, s2);
	if (strcmp (s1, s2) < 0)
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case K_GES:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	char s1[128];
	char s2[128];
	get_string_all (op1, s1);
	get_string_all (op2, s2);
	if (strcmp (s1, s2) >= 0)
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  case K_GTS:
    {
      c.value.is_integer = 1;
      if (c.value.is_integer) {
	int retval = 0; 
	char s1[128];
	char s2[128];
	get_string_all (op1, s1);
	get_string_all (op2, s2);
	if (strcmp (s1, s2) > 0)
	  retval = 1;
	c.value.p1 = retval;
      }
    }
    break;
  default:
    printf("not recognized %d\n",code);
  }
  return c;
#if 0
  tree t = malloc (sizeof(struct struct_tree));
  t.is_integer = b.value.is_integer | c.value.is_integer;
  t.code = a;
  t.p1 = b.value;
  t.string = c.value;
  struct c_expr * e = malloc(sizeof(struct c_expr));
  e->value = t;
  return *e;
#endif
}

struct c_expr parser_build_unary_op(long code, struct c_expr b) {
  struct c_expr c;
#if 0
  c.value.is_integer = 1;
  c.value = malloc (sizeof (struct struct_tree));
#endif
  switch (code) {
  case CONVERT_EXPR:
    {
      c = b;
    }
    break;
  case NEGATE_EXPR:
    {
      c = b;
      c.value.p1 = - ((long)c.value.p1);
    }
    break;
  case BIT_NOT_EXPR:
    {
      c = b;
      c.value.p1 = ~((long)c.value.p1);
    }
    break;
  default:
    printf("not recognized %d\n",code);
  }
  return c;
#if 0
  tree t = malloc (sizeof(struct struct_tree));
  t.is_integer = b.value.is_integer;
  t.code = a;
  t.p1 = b.value;
  t.string = 0;
  struct c_expr * e = malloc(sizeof(struct c_expr));
  e->value = t;
  return *e;
#endif
}

struct c_expr
c_parser_binary_expression (c_parser *parser, struct c_expr *after)
{
  /* A binary expression is parsed using operator-precedence parsing,
     with the operands being cast expressions.  All the binary
     operators are left-associative.  Thus a binary expression is of
     form:

     E0 op1 E1 op2 E2 ...

     which we represent on a stack.  On the stack, the precedence
     levels are strictly increasing.  When a new operator is
     encountered of higher precedence than that at the top of the
     stack, it is pushed; its LHS is the top expression, and its RHS
     is everything parsed until it is popped.  When a new operator is
     encountered with precedence less than or equal to that at the top
     of the stack, triples E[i-1] op[i] E[i] are popped and replaced
     by the result of the operation until the operator at the top of
     the stack has lower precedence than the new operator or there is
     only one element on the stack; then the top expression is the LHS
     of the new operator.  In the case of logical AND and OR
     expressions, we also need to adjust skip_evaluation as
     appropriate when the operators are pushed and popped.  */

  /* The precedence levels, where 0 is a dummy lowest level used for
     the bottom of the stack.  */
  enum prec {
    PREC_NONE,
    PREC_LOGOR,
    PREC_LOGAND,
    PREC_BITOR,
    PREC_BITXOR,
    PREC_BITAND,
    PREC_LOGNOT,
    PREC_EQ,
    PREC_REL,
    PREC_SHIFT,
    PREC_ADD,
    PREC_MULT,
    NUM_PRECS
  };
  struct {
    /* The expression at this stack level.  */
    struct c_expr expr;
    /* The precedence of the operator on its left, PREC_NONE at the
       bottom of the stack.  */
    enum prec prec;
    /* The operation on its left.  */
#if 0
    enum tree_code op;
#else
    int op;
#endif
  } stack[NUM_PRECS];
  int sp;
#define POP								      \
  do {									      \
    /*switch (stack[sp].op)						      \
      {									      \
      case TRUTH_ANDIF_EXPR:						      \
	skip_evaluation -= stack[sp - 1].expr.value == truthvalue_false_node; \
	break;								      \
      case TRUTH_ORIF_EXPR:						      \
	skip_evaluation -= stack[sp - 1].expr.value == truthvalue_true_node;  \
	break;								      \
      default:								      \
	break;								      \
	}*/									      \
    stack[sp - 1].expr							      \
      = default_function_array_conversion (stack[sp - 1].expr);		      \
    stack[sp].expr							      \
      = default_function_array_conversion (stack[sp].expr);		      \
    stack[sp - 1].expr = parser_build_binary_op (stack[sp].op,		      \
						 stack[sp - 1].expr,	      \
						 stack[sp].expr);	      \
    sp--;								      \
  } while (0)
#if 0
  gcc_assert (!after || c_dialect_objc ());
#endif
  ;
  stack[0].expr = c_parser_cast_expression (parser, after);
  stack[0].prec = PREC_NONE;
  sp = 0;
  while (true)
    {
      enum prec oprec;
#if 0
      enum tree_code ocode;
#else
      int ocode;
#endif
      if (parser->error)
	goto out;
      switch (c_parser_peek_token (parser)->type)
	{
	case '*':
	case CPP_MULT:
	  oprec = PREC_MULT;
	  ocode = MULT_EXPR;
	  break;
#if 0
	case '/':
	case CPP_DIV:
	  oprec = PREC_MULT;
	  ocode = TRUNC_DIV_EXPR;
	  break;
	case CPP_MOD:
	  oprec = PREC_MULT;
	  ocode = TRUNC_MOD_EXPR;
	  break;
#endif
	case '+':
	case CPP_PLUS:
	  oprec = PREC_ADD;
	  ocode = PLUS_EXPR;
	  break;
	case '-':
	case CPP_MINUS:
	  oprec = PREC_ADD;
	  ocode = MINUS_EXPR;
	  break;
#if 0
	case CPP_LSHIFT:
	  oprec = PREC_SHIFT;
	  ocode = LSHIFT_EXPR;
	  break;
	case CPP_RSHIFT:
	  oprec = PREC_SHIFT;
	  ocode = RSHIFT_EXPR;
	  break;
#endif
	case K_LT:
	case CPP_LESS:
	  oprec = PREC_REL;
	  ocode = LT_EXPR;
	  break;
	case K_GT:
	case CPP_GREATER:
	  oprec = PREC_REL;
	  ocode = GT_EXPR;
	  break;
	case K_LE:
	case CPP_LESS_EQ:
	  oprec = PREC_REL;
	  ocode = LE_EXPR;
	  break;
	case K_GE:
	case CPP_GREATER_EQ:
	  oprec = PREC_REL;
	  ocode = GE_EXPR;
	  break;
	case K_EQ:
	case CPP_EQ_EQ:
	  oprec = PREC_EQ;
	  ocode = EQ_EXPR;
	  break;
	case K_NE:
	case CPP_NOT_EQ:
	  oprec = PREC_EQ;
	  ocode = NE_EXPR;
	  break;
	case K_EQS:
	case K_NES:
	case K_LES:
	case K_LTS:
	case K_GES:
	case K_GTS:
	  oprec = PREC_EQ;
	  ocode = c_parser_peek_token (parser)->type;
	  break;
	case K_AND:
	case CPP_AND:
	  oprec = PREC_BITAND;
	  ocode = BIT_AND_EXPR;
	  break;
	case CPP_XOR:
	  oprec = PREC_BITXOR;
	  ocode = BIT_XOR_EXPR;
	  break;
	case K_OR:
	case CPP_OR:
	  oprec = PREC_BITOR;
	  ocode = BIT_IOR_EXPR;
	  break;
	case CPP_AND_AND:
	  oprec = PREC_LOGAND;
	  ocode = TRUTH_ANDIF_EXPR;
	  break;
	case CPP_OR_OR:
	  oprec = PREC_LOGOR;
	  ocode = TRUTH_ORIF_EXPR;
	  break;
	default:
	  /* Not a binary operator, so end of the binary
	     expression.  */
	  goto out;
	}
      c_parser_consume_token (parser);
      while (oprec <= stack[sp].prec)
	POP;
      switch (ocode)
	{
#if 0
	case TRUTH_ANDIF_EXPR:
	  stack[sp].expr
	    = default_function_array_conversion (stack[sp].expr);
	  stack[sp].expr.value = c_objc_common_truthvalue_conversion
	    (default_conversion (stack[sp].expr.value));
	  skip_evaluation += stack[sp].expr.value == truthvalue_false_node;
	  break;
	case TRUTH_ORIF_EXPR:
	  stack[sp].expr
	    = default_function_array_conversion (stack[sp].expr);
	  stack[sp].expr.value = c_objc_common_truthvalue_conversion
	    (default_conversion (stack[sp].expr.value));
	  skip_evaluation += stack[sp].expr.value == truthvalue_true_node;
	  break;
#endif
	default:
	  break;
	}
      sp++;
      stack[sp].expr = c_parser_cast_expression (parser, NULL);
      stack[sp].prec = oprec;
      stack[sp].op = ocode;
    }
 out:
  while (sp > 0)
    POP;
  return stack[0].expr;
#undef POP
}

static struct c_expr c_parser_expression (c_parser * p) {
  extern int get_symbol_value;
  get_symbol_value = 1;
  struct c_expr c = c_parser_binary_expression(p, 0);
  extern int get_symbol_value;
  get_symbol_value = 0;
  return c;
}

void * c_parser_binary_expression2 (int a, int b) {
  extern int get_symbol_value;
  get_symbol_value = 1;
  struct c_expr c = c_parser_binary_expression (getparser(), 0);
  extern int get_symbol_value;
  get_symbol_value = 0;
  if (c.value.is_integer)
    printf("INT %d\n",c.value.p1);
  else
    printf("STR %s\n",c.value.string);
  return 0;
}

void * c_parser_binary_expression3 (long a, int b) {
  extern int get_symbol_value;
  get_symbol_value = 1;
  struct c_expr c = c_parser_binary_expression (a, 0);
  extern int get_symbol_value;
  get_symbol_value = 0;
  if (c.value.is_integer)
    return c.value.p1 & 1;
  return 0;
}

void * c_parser_binary_expression4 (int a, int b) {
  extern int get_symbol_value;
  get_symbol_value = 1;
  struct c_expr c = c_parser_binary_expression (getparser(), 0);
  extern int get_symbol_value;
  get_symbol_value = 0;
  if (c.value.is_integer)
    return c.value.p1 & 1;
  return 0;
}

// check boundary
void * c_parser_binary_expression2_as_string (int a, int b, char * s) {
  extern int get_symbol_value;
  get_symbol_value = 1;
  struct c_expr c = c_parser_binary_expression (getparser(), 0);
  extern int get_symbol_value;
  get_symbol_value = 0;
  if (c.value.is_integer)
    sprintf(s, "%d\n", c.value.p1);
  else
    sprintf(s, "%s\n", c.value.string);
  return 0;
}

