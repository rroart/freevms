#ifndef cli_h
#define cli_h

#define CDU$C_VERB 1
#define CDU$C_TYPE 2
#define CDU$C_SYNTAX 3
#define CDU$C_PARAMETER 4
#define CDU$C_QUALIFIER 5
#define CDU$C_NAME 6
#define CDU$C_VALUE 7
#define CDU$C_KEYWORD 8

#define CDU$M_NODISALLOWS 0x1
#define CDU$M_NOPARAMETERS 0x2
#define CDU$M_NOQUALIFIERS 0x4
#define CDU$M_BATCH 0x8
#define CDU$M_NEGATABLE 0x10
#define CDU$M_NONNEGATABLE 0x20
#define CDU$M_DEFAULT 0x40
#define CDU$M_ABBREVIATE 0x80
#define CDU$M_FOREIGN 0x100
#define CDU$M_IMMEDIATE 0x200
#define CDU$M_MCRIGNORE 0x400
#define CDU$M_MCROPTDELIM 0x800
#define CDU$M_MCRPARSE 0x1000
#define CDU$M_NOSTATUS 0x2000
#define CDU$M_CONCATENATE 0x4000
#define CDU$M_NOCONCATENATE 0x8000
#define CDU$M_IMPCAT 0x10000
#define CDU$M_LIST 0x20000
#define CDU$M_REQUIRED 0x40000
#define CDU$M_INTERNAL 0x80000 // cliroutine is such

struct _cdu {
  int cdu$l_cfl;
  int cdu$l_cbl; 
  unsigned short int cdu$w_size;
  unsigned char cdu$b_type;
  unsigned char cdu$b_rmod;
  int cdu$l_next;
  char cdu$t_name[0];
  int cdu$l_parent;
  int cdu$l_child;
  int cdu$l_verb;
  int cdu$l_image;
  int cdu$l_routine;
  int cdu$l_qualifiers;
  int cdu$l_parameters;
  int cdu$l_name;
  int cdu$l_value;
  int cdu$l_flags;
  int cdu$l_label;
  int cdu$l_type;
  int cdu$l_syntax;
  int cdu$l_clauses;
  int cdu$l_keywords;
};

#if 0
#define cdu$l_next u1.cdu$l_next
#define cdu$l_parent u3.u2.cdu$l_parent
#define cdu$l_child u3.u2.cdu$l_child
#define cdu$l_verb u3.u2.cdu$l_verb
#define cdu$l_image u3.u2.cdu$l_image
#define cdu$t_name u3.cdu$t_name
#endif

#ifdef __i386__
static struct _cdu ** cur_cdu=0x3f000000;
static struct _cdu ** my_cdu=0x3f000004;
static struct _cdu ** root_cdu=0x3f000008;
static void ** comdsc = 0x3f00000c; 
#else
static struct _cdu ** cur_cdu=0x3f000000;
static struct _cdu ** my_cdu=0x3f000008;
static struct _cdu ** root_cdu=0x3f000010;
static void ** comdsc = 0x3f000018; 
#endif

#endif
