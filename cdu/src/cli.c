#define GTY(x)
#define HOST_BITS_PER_WIDE_INT 64
#define POINTER_SIZE 32
#define true 1
#define false 0

#include<stdio.h>
#include<dlfcn.h>
#include<sys/mman.h>

#include<descrip.h>
#include<ssdef.h>

#include "tree.h"
#include "cld.h"

#include "cli.h"

insert_cdu(struct _cdu * c) {
  c->cdu$l_next=cdu_root;
  cdu_root=c;
}

alloc_cdu() {
  int a = malloc(sizeof(struct _cdu));
  memset(a,0,sizeof(struct _cdu));
  return a;
}

void * cli$cli(char * name) {
  extern FILE *yyin;
  init_stringpool();
  yyin=fopen(name, "r");
  yyparse();
  extern tree root_tree;
  gencode(root_tree);
  mmap(0x3f000000, 4,PROT_READ|PROT_WRITE,MAP_FIXED|MAP_ANONYMOUS|MAP_SHARED,0,0); // gross hack
  return cdu_root;
}

unsigned int cli$dcl_parse(void * command_string ,void * table ,void * param_routine, void * prompt_routine, void * prompt_string) {
  struct dsc$descriptor * com = command_string;
  struct _cdu * cdu_root = table;

  char * line = com->dsc$a_pointer;
  int len = com->dsc$w_length;

  char * end = line + len;

  int i;
  for (i=len;i;i--)
    if (line[i]>='A' && line[i]<='Z') line[i]+=32;

  while (*line==' ')
    line++;

  int charno=(long)line-(long)com->dsc$a_pointer;
  char * endverb = line;

  while (endverb<(line+len)) {
    if (*endverb<'a' || *endverb>'z')
      break;
    endverb++;
  }
  //printf("line %x %s\n",line,line);
  //printf("endverb %x %s\n",endverb,endverb);

  struct _cdu * cdu = cdu_root;

  while (cdu) {
    if (strncmp(cdu->cdu$l_verb,line,endverb-line)==0)
      break;
    cdu=cdu->cdu$l_next;
  }
  *cur_cdu=cdu;
  if (cdu==0)
    return 0;

  struct _cdu * my = malloc(sizeof(struct _cdu));
  memset(my,0,sizeof(struct _cdu));
  *my_cdu=my;

  line=endverb;

  int pn = 1;

  char * endunit = 0;

  while (line<end) {

    while (*line==' ')
      line++;

    endunit = line;

    if (*line=='/') {
      line++;
      endunit++;
      while (*endunit!=' ' && *endunit!='/')
	endunit++;
      struct _qual * q = malloc (sizeof(struct _qual));
      q->name = malloc(endunit-line+1);
      memcpy(q->name,line,endunit-line);
      q->name[endunit-line]=0;
      q->next=my->cdu$l_qualifiers;
      my->cdu$l_qualifiers=q;
    } else {
      while (*endunit!=' ' && *endunit!='/')
	endunit++;
      struct _para * p = malloc (sizeof(struct _para));
      p->name[0]='p';
      p->name[1]='0'+pn;
      p->value = malloc(endunit-line+1);
      memcpy(p->value,line,endunit-line);
      p->value[endunit-line]=0;
      p->next=my->cdu$l_parameters;
      my->cdu$l_parameters=p;
      pn++;
    }
    line = endunit;
  }

  return SS$_NORMAL;
}

unsigned int cli$dispatch(int userarg){
  char * mainp="main";
  char * routine=mainp;
  char * path="/vms$common/sysexe/";
  int pathlen=strlen(path);
  char image[256];
  memcpy(image,path,pathlen);
  memcpy(image+pathlen,(*cur_cdu)->cdu$l_image,strlen((*cur_cdu)->cdu$l_image));
  memcpy(image+pathlen+strlen((*cur_cdu)->cdu$l_image),".exe2",5);
  image[pathlen+strlen((*cur_cdu)->cdu$l_image)+5]=0;
  printf("Opening %s\n",image);
  void * handle = dlopen(image,RTLD_NOW);
  if (handle==0) {
    printf("dlopen: %s\n",dlerror());
    return 0;
  }
  dlerror(); // clear error
  if ((*cur_cdu)->cdu$l_routine) routine=(*cur_cdu)->cdu$l_routine;
  printf("Find routine %s\n",routine);
  int(*fn)() = dlsym(handle,routine);
  printf("Got function address %x\n",fn);
  int error=dlerror();
  if (error) {
    printf("dlsym: %s\n",error);
    dlclose(handle);
    return 0;
  }
  // not yet  fn(userarg);
  char * myp1 = "";
  if ((*my_cdu)->cdu$l_parameters) {
    struct _para * p = (*my_cdu)->cdu$l_parameters;
    myp1 = p->value;
  }
  char * myargv[2]={"",myp1};
  fn(userarg,myargv,0,0);
  //fn(argc,argv++);
  
  dlclose(handle);

  return SS$_NORMAL;
}

int gencode(tree t) {
  while (t) {
    switch (TREE_CODE(t)) {
    case DEFINE_VERB_STMT:
      {
	struct _cdu * cdu = alloc_cdu();
	cdu->cdu$l_verb = IDENTIFIER_POINTER(TREE_OPERAND(t, 0));
	insert_cdu(cdu);
	tree verb = TREE_OPERAND(t, 1);
	gencode_verb(verb, cdu);
      }
      break;
    default:
    }
    t=TREE_CHAIN(t);
  }
}

int gencode_verb(tree t,struct _cdu * cdu) {
  while (t) {
    switch (TREE_CODE(t)) {
    case ROUTINE_CLAUSE:
      cdu->cdu$l_routine=IDENTIFIER_POINTER(TREE_OPERAND(t, 0));
      break;
    case IMAGE_CLAUSE:
      cdu->cdu$l_image=IDENTIFIER_POINTER(TREE_OPERAND(t, 0));
      break;
    case QUALIFIER_CLAUSE:
      {
	struct _qual * q = malloc(sizeof(struct _qual));
	char * name = IDENTIFIER_POINTER(TREE_OPERAND(t, 0));
	q->name = malloc(strlen(name));
	memcpy(q->name, name, strlen(name));
	q->next=cdu->cdu$l_qualifiers;
	cdu->cdu$l_qualifiers=q;
      }
      break;
    case PARAMETER_CLAUSE:
      {
	struct _para * p = malloc(sizeof(struct _para));
	memcpy(p->name,IDENTIFIER_POINTER(TREE_OPERAND(t, 0)),2);
	p->next=cdu->cdu$l_parameters;
	cdu->cdu$l_parameters=p;
      }
      break;
    default:
      break;
    }
    t=TREE_CHAIN(t);
  }
}
  
