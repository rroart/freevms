// $Id$
// $Locker$

// Author. Roar Thronæs.

#define GTY(x)
#define HOST_BITS_PER_WIDE_INT 64
#define POINTER_SIZE 32
#define true 1
#define false 0

#include<stdio.h>
#include<dlfcn.h>
#include<sys/mman.h>

#include<descrip.h>
#include<ihadef.h>
#include<ihddef.h>
#include<ihsdef.h>
#include<isddef.h>
#include<ssdef.h>
#include<va_rangedef.h>

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
  if (yyin==0) {
    printf("could not fopen %s\n",name);
    return 0;
  }
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

typedef struct {
  unsigned char st_name[4];
  unsigned char st_value[4];
  unsigned char st_size[4]; 
  unsigned char st_info[1];
  unsigned char st_other[1];
  unsigned char st_shndx[2];
} Elf_External_Sym;

typedef struct bfd_symbol
{
  const char *name;
  unsigned long value;
  unsigned int flags;
} asymbol;

#define COERCE32(x) (((signed long) (x) ^ 0x80000000) - 0x80000000)

unsigned int
bfd_getl32 (void * dummy, const void *p)
{
  const unsigned char *addr = p;
  unsigned long v;

  v = (unsigned long) addr[0];
  v |= (unsigned long) addr[1] << 8;
  v |= (unsigned long) addr[2] << 16;
  v |= (unsigned long) addr[3] << 24;
  return v;
}

signed int
bfd_getl_signed_32 (void * dummy, const void *p)
{
  const unsigned char *addr = p;
  unsigned long v;

  v = (unsigned long) addr[0];
  v |= (unsigned long) addr[1] << 8;
  v |= (unsigned long) addr[2] << 16;
  v |= (unsigned long) addr[3] << 24;
  return COERCE32 (v);
}

#define bfd_h_get_32 bfd_getl32
#define bfd_h_get_signed_32 bfd_getl_signed_32
#define H_GET_32  bfd_h_get_32
#define H_GET_S32 bfd_h_get_signed_32
#define H_GET_WORD                H_GET_32
#define H_GET_SIGNED_WORD H_GET_S32
void
nisse_swap_symbol_in (void *abfd,
		      const void *psrc,
		      const void *pshn,
		      asymbol /*Elf_Internal_Sym*/ *dst)
{
  const Elf_External_Sym *src = psrc;
  //  int signed_vma = get_nisse_backend_data (abfd)->sign_extend_vma;
  int signed_vma = 0; // check. added this to be sure.
  dst->name = H_GET_32 (abfd, src->st_name);
  if (signed_vma)
    dst->value = H_GET_SIGNED_WORD (abfd, src->st_value);
  else
    dst->value = H_GET_WORD (abfd, src->st_value);
  dst->flags = H_GET_WORD (abfd, src->st_size);
}

extern int vms_mm;

unsigned int cli$dispatch(int userarg){
  char * routine;
  char * myp1 = "";
  char * myargv[2]={"",myp1};
  char image[256];
  char * path;
  int pathlen;

  if (vms_mm==0) goto exe2;

  path="SYS$SYSTEM:";
  pathlen=strlen(path);
  memcpy(image,path,pathlen);
  memcpy(image+pathlen,(*cur_cdu)->cdu$l_image,strlen((*cur_cdu)->cdu$l_image));
  memcpy(image+pathlen+strlen((*cur_cdu)->cdu$l_image),".exe",4);
  image[pathlen+strlen((*cur_cdu)->cdu$l_image)+4]=0;

  unsigned long sts;
  struct dsc$descriptor aname;
  struct dsc$descriptor dflnam;
  struct file * f;
  struct _ihd * hdrbuf;
  struct _iha * active;
  struct _va_range inadr;
  void (*func)();
  char * imgnam = (*cur_cdu)->cdu$l_image;
  int len = strlen(imgnam);

  aname.dsc$w_length=len;
  aname.dsc$a_pointer=imgnam;
  dflnam.dsc$w_length=pathlen+strlen((*cur_cdu)->cdu$l_image)+4;
  dflnam.dsc$a_pointer=image;

  hdrbuf=malloc(512);
  memset(hdrbuf, 0, 512);

  sts=sys$imgact(&aname,&dflnam,hdrbuf,0,0,0,0,0);
  printf("imgact got sts %x\n",sts);

  if (sts!=SS$_NORMAL) return sts;

  active=(unsigned long)hdrbuf+hdrbuf->ihd$w_activoff;

#if 0
  // can't do this, for some reason it causes pagefault
  char * str = argv[0];
  str[len-4]=0;
#endif

  func=active->iha$l_tfradr1;

  struct _ihd * ehdr32 = hdrbuf;

  struct _ihs * debug = (unsigned long)ehdr32+ehdr32->ihd$w_symdbgoff;

  char * buffer = ehdr32;

  struct _isd * section=(unsigned long)buffer+ehdr32->ihd$w_size;

  long symtab=0, symtabsize=0, symtabvbn=0, symstr=0, symstrsize=0, symstrvbn=0;

  while (section<(buffer+512*ehdr32->ihd$b_hdrblkcnt)) {
    if (section->isd$w_size==0)
      break;
    if (section->isd$w_size==0xffffffff) {
      int no=((unsigned long)section-(unsigned long)buffer)>>9;
      section=buffer+512*(no+1);
      continue;
    }
    if (debug->ihs$l_dstvbn==section->isd$l_vbn) {
      symtab=section->isd$v_vpn<<12;
      symtabvbn=debug->ihs$l_dstvbn;
      symtabsize=section->isd$w_pagcnt;
    }

    if (debug->ihs$l_dmtvbn==section->isd$l_vbn) {
      symstr=section->isd$v_vpn<<12;
      symstrvbn=debug->ihs$l_dmtvbn;
      symstrsize=section->isd$w_pagcnt;
    }

    section=(unsigned long)section+section->isd$w_size;
  }

  if (symtabsize == 0 || symstrsize == 0)
    goto skip;

#if 0
  symstr = malloc(symstrsize<<12);
  symtab = malloc(symtabsize<<12);
  memset(symstr,0,symstrsize<<12);
  memset(symtab,0,symtabsize<<12);
#endif

  routine = (*cur_cdu)->cdu$l_routine;
  int value = 0;

  while (1) {
    Elf_External_Sym * src = symtab;
    if (src->st_name[0]==0 && src->st_name[1]==0) break;
    asymbol dst;
    nisse_swap_symbol_in(0, src, 0, &dst);
    symtab += sizeof(Elf_External_Sym);
    src++;
    if (0==strncmp(symstr+dst.name,routine,strlen(routine))) {
      value = dst.value;
      break;
    }
  }

  if (value)
    func = value + 0x10000;

 skip:

  printf("entering image? %x\n",func);
  if ((*my_cdu)->cdu$l_parameters) {
    struct _para * p = (*my_cdu)->cdu$l_parameters;
    myargv[1] = p->value;
  }
  //  func(argc,argv++);
  func(userarg,myargv,0,0);
  printf("after image\n");

  sys$rundwn();

  return SS$_NORMAL;

 exe2:
  {}
  char * mainp="main";
  routine=mainp;
  path="/vms$common/sysexe/";
  pathlen=strlen(path);

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
  if ((*my_cdu)->cdu$l_parameters) {
    struct _para * p = (*my_cdu)->cdu$l_parameters;
   myargv[1] = p->value;
  }
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
  
