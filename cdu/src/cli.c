#define GTY(x)
#define HOST_BITS_PER_WIDE_INT 64
#define POINTER_SIZE 32
#define true 1
#define false 0

#include<stdio.h>
#include<dlfcn.h>

#include<descrip.h>
#include<ssdef.h>

#include "tree.h"
#include "cld.h"

struct _cdu {
  void * cdu$l_next;
  void * cdu$l_child;
  char * cdu$l_verb;
  char * cdu$l_image;
  char * cdu$l_routine;
};

struct _cdu * cdu_root=0;
struct _cdu * cur_cdu=0;

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
}

unsigned int cli$dcl_parse(void * command_string ,void * table ,void * param_routine, void * prompt_routine, void * prompt_string) {
  struct dsc$descriptor * com = command_string;
  struct _cdu * cdu_root = table;

  char * line = com->dsc$a_pointer;
  int len = com->dsc$w_length;

  int i;
  for (i=len;i;i--)
    if (line[i]>='A' && line[i]<='Z') line[i]+=32;

  while (*line==' ')
    line++;

  int charno=(long)line-(long)com->dsc$a_pointer;
  char * endverb = line;

  while (endverb<(line+len)) {
    if (*endverb<'a' && *endverb>'z')
      break;
    endverb++;
  }

  struct _cdu * cdu = cdu_root;

  while (cdu) {
    if (strncmp(cdu->cdu$l_verb,line,endverb-line))
      break;
    cdu=cdu->cdu$l_next;
  }
  cur_cdu=cdu;
}

unsigned int cli$dispatch(int userarg){
  char * mainp="main";
  char * routine=mainp;
  char * path="/vms$common/sysexe/";
  int pathlen=strlen(path);
  char image[256];
  memcpy(image,path,pathlen);
  memcpy(image,cur_cdu->cdu$l_image,strlen(cur_cdu->cdu$l_image));
  image[pathlen+strlen(cur_cdu->cdu$l_image)]=0;
  void * handle = dlopen(image,RTLD_NOW);
  if (handle==0) {
    printf("dlopen: %s\n",dlerror());
    return 0;
  }
  dlerror(); // clear error
  if (cur_cdu->cdu$l_routine) routine=cur_cdu->cdu$l_routine;
  int(*fn)() = dlsym(handle,routine);
  int error=dlerror();
  if (error) {
    printf("dlsym: %s\n",error);
    dlclose(handle);
    return 0;
  }
  fn(userarg);
  //fn(argc,argv++);
  
  dlclose(handle);

  return SS$_NORMAL;
}

unsigned int cli$get_value(void *entity_desc, void *retdesc,short * retlen) {

}

unsigned int cli$present(void *entity_desc) {

}

int gencode(tree t) {
  while (t) {
    switch (TREE_CODE(t)) {
    case DEFINE_VERB_STMT:
      {
	struct _cdu * cdu = alloc_cdu();
	cdu->cdu$l_verb = TREE_OPERAND(t, 1);
	insert_cdu(cdu);
	tree verb = TREE_OPERAND(t, 2);
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
      cdu->cdu$l_routine=TREE_OPERAND(t, 1);
      break;
    case IMAGE_CLAUSE:
      cdu->cdu$l_image=TREE_OPERAND(t, 1);
      break;
    default:
      break;
    }
    t=TREE_CHAIN(t);
  }
}
  
