typedef int bool;

typedef struct struct_tree tree;

struct struct_tree
{
    int is_integer;
    long p1;
    char string[128];
};

typedef struct yystype
{
    tree type_node_p;
#if 0
    long long value;
    int stringlen;
    char string[128];
#endif
} YYSTYPE;

extern YYSTYPE yylval;

enum
{
    UNONE = 0,
    UPLUS = 128,
    UMINUS,
    T_DIGITS,
    T_NAME,
    K_IF,
    K_THEN,
    K_ELSE,
    K_ENDIF,
    T_STRING,
    T_QUALIFIER,
    END_EXPR,
    END_BUILTIN,
    K_WRITE,
    U_ABSOLUTE,
    REALLY_MAX,
    MULT_EXPR,
    PLUS_EXPR,
    MINUS_EXPR,
    LT_EXPR,
    GT_EXPR,
    LE_EXPR,
    GE_EXPR,
    EQ_EXPR,
    NE_EXPR,
    BIT_AND_EXPR,
    BIT_XOR_EXPR,
    BIT_IOR_EXPR,
    TRUTH_ANDIF_EXPR,
    TRUTH_ORIF_EXPR,
    CONVERT_EXPR,
    NEGATE_EXPR,
    BIT_NOT_EXPR,
    TRUTH_NOT_EXPR,
    CPP_NUMBER,
    CPP_CHAR,
    ERROR_MARK,
    CPP_STRING,
    CPP_WCHAR,
    CPP_WSTRING,
    STRING_CST,
    MODIFY_EXPR,
    K_AND,
    K_OR,
    K_NOT,
    K_EQ,
    K_NE,
    K_GE,
    K_GT,
    K_LE,
    K_LT,
    K_EQS,
    K_NES,
    K_GES,
    K_GTS,
    K_LES,
    K_LTS,
    TRUNC_DIV_EXPR
};

#define NULL_TREE 0

#define GTY(x)
#if 0
#define ENUM_BITFIELD(TYPE) __extension__ enum TYPE
#else
#define ENUM_BITFIELD(TYPE) unsigned int
#endif
#define BOOL_BITFIELD unsigned int

typedef int location_t;

struct c_expr
{
    /* The value of the expression.  */
    tree value;
    /* Record the original binary operator of an expression, which may
     have been changed by fold, STRING_CST for unparenthesized string
     constants, or ERROR_MARK for other expressions (including
     parenthesized expressions).  */
#if 0
    enum tree_code original_code;
#else
    int original_code;
#endif
};

#define TV_LEX 0
#define NULL 0
#define CPP_NAME 0

enum rid
{
    RID_NULL = 0, RID_MAX
};

#define TTYPE_TABLE                                                     \
  OP(EQ,                "=")                                            \
  OP(NOT,               "!")                                            \
  OP(GREATER,           ">")    /* compare */                           \
  OP(LESS,              "<")                                            \
  OP(PLUS,              "+")    /* math */                              \
  OP(MINUS,             "-")                                            \
  OP(MULT,              "*")                                            \
  OP(DIV,               "/")                                            \
  OP(MOD,               "%")                                            \
  OP(AND,               "&")    /* bit ops */                           \
  OP(OR,                "|")                                            \
  OP(XOR,               "^")                                            \
  OP(RSHIFT,            ">>")                                           \
  OP(LSHIFT,            "<<")                                           \
  OP(MIN,               "<?")   /* extension */                         \
  OP(MAX,               ">?")                                           \
                                                                        \
  OP(COMPL,             "~")                                            \
  OP(AND_AND,           "&&")   /* logical */                           \
  OP(OR_OR,             "||")                                           \
  OP(QUERY,             "?")                                            \
  OP(COLON,             ":")                                            \
  OP(COMMA,             ",")    /* grouping */                          \
  OP(OPEN_PAREN,        "(")                                            \
  OP(CLOSE_PAREN,       ")")                                            \
  TK(EOF,               NONE)                                           \
  OP(EQ_EQ,             "==")   /* compare */                           \
  OP(NOT_EQ,            "!=")                                           \
  OP(GREATER_EQ,        ">=")                                           \
  OP(LESS_EQ,           "<=")                                           \
                                                                        \
  /* These two are unary + / - in preprocessor expressions.  */         \
  OP(PLUS_EQ,           "+=")   /* math */                              \
  OP(MINUS_EQ,          "-=")                                           \
                                                                        \
  OP(MULT_EQ,           "*=")                                           \
  OP(DIV_EQ,            "/=")                                           \
  OP(MOD_EQ,            "%=")                                           \
  OP(AND_EQ,            "&=")   /* bit ops */                           \
  OP(OR_EQ,             "|=")                                           \
  OP(XOR_EQ,            "^=")                                           \
  OP(RSHIFT_EQ,         ">>=")                                          \
  OP(LSHIFT_EQ,         "<<=")                                          \
  OP(MIN_EQ,            "<?=")  /* extension */                         \
  OP(MAX_EQ,            ">?=")                                          \
  /* Digraphs together, beginning with CPP_FIRST_DIGRAPH.  */           \
  OP(HASH,              "#")    /* digraphs */                          \
  OP(PASTE,             "##")                                           \
  OP(OPEN_SQUARE,       "[")                                            \
  OP(CLOSE_SQUARE,      "]")                                            \
  OP(OPEN_BRACE,        "{")                                            \
  OP(CLOSE_BRACE,       "}")                                            \
  /* The remainder of the punctuation.  Order is not significant.  */   \
  OP(SEMICOLON,         ";")    /* structure */                         \
  OP(ELLIPSIS,          "...")                                          \
  OP(PLUS_PLUS,         "++")   /* increment */                         \
  OP(MINUS_MINUS,       "--")                                           \
  OP(DEREF,             "->")   /* accessors */                         \
  OP(DOT,               ".")                                            \
  OP(SCOPE,             "::")                                           \
  OP(DEREF_STAR,        "->*")                                          \
  OP(DOT_STAR,          ".*")                                           \
  OP(ATSIGN,            "@")  /* used in Objective-C */                 \
 
#define OP(e, s) CPP_ ## e,
#define TK(e, s) CPP_ ## e,
enum cpp_ttype
{
    TTYPE_TABLE N_TTYPES,

    /* Positions in the table.  */
    CPP_LAST_EQ = CPP_MAX,
    CPP_FIRST_DIGRAPH = CPP_HASH,
    CPP_LAST_PUNCTUATOR = CPP_DOT_STAR,
    CPP_LAST_CPP_OP = CPP_LESS_EQ
};
#undef OP
#undef TK

#define CPP_KEYWORD ((enum cpp_ttype) (N_TTYPES + 1))

#define false 0
#define true 1

/* More information about the type of a CPP_NAME token.  */
typedef enum c_id_kind
{
    /* An ordinary identifier.  */
    C_ID_ID,
    /* An identifier declared as a typedef name.  */
    C_ID_TYPENAME,
    /* An identifier declared as an Objective-C class name.  */
    C_ID_CLASSNAME,
    /* Not an identifier.  */
    C_ID_NONE
} c_id_kind;

/* A single C token after string literal concatenation and conversion
 of preprocessing tokens to tokens.  */
typedef struct c_token
GTY (())
{
    /* The kind of token.  */ENUM_BITFIELD (cpp_ttype) type :8;
    /* If this token is a CPP_NAME, this value indicates whether also
     declared as some kind of type.  Otherwise, it is C_ID_NONE.  */
    ENUM_BITFIELD (c_id_kind) id_kind :8;
    /* If this token is a keyword, this value indicates which keyword.
     Otherwise, this value is RID_MAX.  */
    ENUM_BITFIELD (rid) keyword :8;
    /* True if this token is from a system header.  */
    BOOL_BITFIELD in_system_header :1;
    /* The value associated with this token, if any.  */
    tree value;
    /* The location at which this token was found.  */
    location_t location;
} c_token;

/* A parser structure recording information about the state and
 context of parsing.  Includes lexer information with up to two
 tokens of look-ahead; more are not needed for C.  */
typedef struct c_parser
GTY(())
{
    /* The look-ahead tokens.  */
    c_token tokens[2];
    /* How many look-ahead tokens are available (0, 1 or 2).  */
    short tokens_avail;
    /* True if a syntax error is being recovered from; false otherwise.
     c_parser_error sets this flag.  It should clear this flag when
     enough tokens have been consumed to recover from the error.  */
    BOOL_BITFIELD error :1;
} c_parser;

#if 0
static inline tree build_not (int is_integer, long p1, long p2)
{
    tree t = malloc (sizeof (struct struct_tree));
    t->is_integer = is_integer;
    t->p1 = p1;
    t->p2 = p2;
    return t;
}
#endif
