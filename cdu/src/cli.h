struct _qual {
  void * next;
  char * name;
};

struct _para {
  void * next;
  char name[2];
  char * value;
};

struct _cdu {
  void * cdu$l_next;
  void * cdu$l_child;
  char * cdu$l_verb;
  char * cdu$l_image;
  char * cdu$l_routine;
  void * cdu$l_qualifiers;
  void * cdu$l_parameters;
};

static struct _cdu * cdu_root=0;
static struct _cdu ** cur_cdu=0x3f000000;
static struct _cdu ** my_cdu=0x3f000004;

