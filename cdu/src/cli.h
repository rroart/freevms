#ifndef cli_h
#define cli_h

#define CDU$C_VERB 1
#define CDU$C_TYPE 2
#define CDU$C_SYNTAX 3
#define CDU$C_PARAMETER 4
#define CDU$C_QUALIFIER 5
#define CDU$C_NAME 6

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
};

#if 0
#define cdu$l_next u1.cdu$l_next
#define cdu$l_parent u3.u2.cdu$l_parent
#define cdu$l_child u3.u2.cdu$l_child
#define cdu$l_verb u3.u2.cdu$l_verb
#define cdu$l_image u3.u2.cdu$l_image
#define cdu$t_name u3.cdu$t_name
#endif

static struct _cdu ** cur_cdu=0x3f000000;
static struct _cdu ** my_cdu=0x3f000004;
static struct _cdu ** root_cdu=0x3f000008;

#endif
