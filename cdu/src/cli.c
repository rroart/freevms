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

#define CDU_ROOT_SIZE 200
struct _cdu cdu_root[CDU_ROOT_SIZE];
int cdu_free = 0;

insert_cdu(int c) {
#if 0
  c->cdu$l_next=cdu_root;
  cdu_root=c;
#endif
  cdu_root[c].cdu$l_next=cdu_root[0].cdu$l_next;
  cdu_root[0].cdu$l_next=c;
}

get_cdu_root() {
  return &cdu_root[0];
}

my_cdu_free = 0;
struct _cdu my_cdu_root[50];

my_alloc_cdu() {
  int b = my_cdu_free++;
  int a = &my_cdu_root[b];
  memset(a,0,sizeof(struct _cdu));
  return b;
}

my_alloc_name(char * n) {
  int name = my_alloc_cdu();
  struct _cdu * np = &my_cdu_root[name];
  memcpy(np->cdu$t_name,n,strlen(n));
  return name;
}

alloc_cdu() {
  int b = cdu_free++;
  int a = &cdu_root[b];
  memset(a,0,sizeof(struct _cdu));
  return b;
}

alloc_name(char * n) {
  int name = alloc_cdu();
  struct _cdu * np = &cdu_root[name];
  memcpy(np->cdu$t_name,n,strlen(n));
  return name;
}

cdu_strncmp(int cdu, char * n, int size) {
  struct _cdu * np = &cdu_root[cdu];
  return strncmp(np->cdu$t_name, n, size);
}

static int mm_done = 0;

unsigned int cli$dcl_parse(void * command_string ,void * table ,void * param_routine, void * prompt_routine, void * prompt_string) {

  if (mm_done==0) {
    mm_done=1;
    mmap(0x3f000000, 4,PROT_READ|PROT_WRITE,MAP_FIXED|MAP_ANONYMOUS|MAP_SHARED,0,0);
    *root_cdu=&cdu_root[0];
  }

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

  i = 0;
  int found = 0;

  do {
    struct _cdu * name = &cdu_root[cdu_root[i].cdu$l_verb];
    if (strncmp(name->cdu$t_name,line,endverb-line)==0) {
      found = 1;
      break;
    }
    i=cdu_root[i].cdu$l_next;
  } while (i);

  *cur_cdu=&cdu_root[i];
  if (!found)
    return 0;

  my_cdu_free=0;
  struct _cdu * my = &my_cdu_root[my_alloc_cdu()];
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
      while (*endunit!=' ' && *endunit!='/' && endunit < end)
	endunit++;
      int q = my_alloc_cdu();
      int n = my_alloc_cdu();
      my_cdu_root[q].cdu$l_name = n;
      struct _cdu * np = &my_cdu_root[n];
      memcpy(np->cdu$t_name,line,endunit-line);
      my_cdu_root[q].cdu$l_next=my->cdu$l_qualifiers;
      my->cdu$l_qualifiers=q;
    } else {
      while (*endunit!=' ' && *endunit!='/' && endunit < end)
	endunit++;
      int p = my_alloc_cdu();
      int n = my_alloc_cdu();
      my_cdu_root[p].cdu$l_name = n;
      struct _cdu * np = &my_cdu_root[n];
      np->cdu$t_name[0]='p';
      np->cdu$t_name[1]='0'+pn;
      int v = my_alloc_cdu();
      struct _cdu * vp = &my_cdu_root[v];
      my_cdu_root[p].cdu$l_value = v;
      memcpy(vp->cdu$t_name,line,endunit-line);
      my_cdu_root[p].cdu$l_next=my->cdu$l_parameters;
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
  int n;
  char * imagebase;
  struct _cdu * cdu;

  if (vms_mm==0) goto exe2;

  path="SYS$SYSTEM:";
  pathlen=strlen(path);
  n = (*cur_cdu)->cdu$l_image;
  imagebase = cdu_root[n].cdu$t_name;
  memcpy(image,path,pathlen);
  memcpy(image+pathlen,imagebase,strlen(imagebase));
  memcpy(image+pathlen+strlen(imagebase),".exe",4);
  image[pathlen+strlen(imagebase)+4]=0;

  unsigned long sts;
  struct dsc$descriptor aname;
  struct dsc$descriptor dflnam;
  struct file * f;
  struct _ihd * hdrbuf;
  struct _iha * active;
  struct _va_range inadr;
  void (*func)();
  char * imgnam = imagebase;
  int len = strlen(imgnam);

  aname.dsc$w_length=len;
  aname.dsc$a_pointer=imgnam;
  dflnam.dsc$w_length=pathlen+strlen(imagebase)+4;
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

  cdu= &cdu_root[(*cur_cdu)->cdu$l_routine];
  routine = cdu->cdu$t_name;
  int value = 0;

  while (1) {
    Elf_External_Sym * src = symtab;
    if (src->st_name[0]==0 && src->st_name[1]==0) break;
    asymbol dst;
    nisse_swap_symbol_in(0, src, 0, &dst);
    symtab += sizeof(Elf_External_Sym);
    src++;
    if (0==strcmp(symstr+dst.name,routine/*,strlen(routine)*/)) {
      value = dst.value;
      break;
    }
  }

  if (value)
    func = value + 0x10000;
  else
    printf("routine %s not found, going for main in %x tfradr\n",routine,func);

 skip:

  printf("entering image? %x\n",func);
  if ((*my_cdu)->cdu$l_parameters) {
    struct _cdu * p = &my_cdu_root[(*my_cdu)->cdu$l_parameters];
    struct _cdu * n = &my_cdu_root[p->cdu$l_value];
    myargv[1] = n->cdu$t_name;
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
  n = (*cur_cdu)->cdu$l_image;
  imagebase = cdu_root[n].cdu$t_name;

  memcpy(image,path,pathlen);
  memcpy(image+pathlen,imagebase,strlen(imagebase));
  memcpy(image+pathlen+strlen(imagebase),".exe2",5);
  image[pathlen+strlen(imagebase)+5]=0;
  printf("Opening %s\n",image);
  void * handle = dlopen(image,RTLD_NOW);
  if (handle==0) {
    printf("dlopen: %s\n",dlerror());
    return 0;
  }
  dlerror(); // clear error
  cdu = &cdu_root[(*cur_cdu)->cdu$l_routine];
  routine = cdu->cdu$t_name;
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
    struct _cdu * p = &my_cdu_root[(*my_cdu)->cdu$l_parameters];
    struct _cdu * n = &my_cdu_root[p->cdu$l_value];
    myargv[1] = n->cdu$t_name;
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
	int cdu = alloc_cdu();
	int name = alloc_cdu();
	char * n =  IDENTIFIER_POINTER(TREE_OPERAND(t, 0));
	cdu_root[cdu].cdu$l_verb = name;
	insert_cdu(cdu);
	struct _cdu * np = &cdu_root[name];
	memcpy(np->cdu$t_name,n,strlen(n));
	tree verb = TREE_OPERAND(t, 1);
	gencode_verb(verb, &cdu_root[cdu]);
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
      cdu->cdu$l_routine = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
      break;
    case IMAGE_CLAUSE:
      cdu->cdu$l_image = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
      break;
    case QUALIFIER_CLAUSE:
      {
	int q = alloc_cdu();
	cdu_root[q].cdu$l_name = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
	cdu_root[q].cdu$l_next=cdu->cdu$l_qualifiers;
	cdu->cdu$l_qualifiers=q;
      }
      break;
    case PARAMETER_CLAUSE:
      {
	int p = alloc_cdu();
	cdu_root[p].cdu$l_name=alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));;
	cdu_root[p].cdu$l_next=cdu->cdu$l_parameters;
	cdu->cdu$l_parameters=p;
      }
      break;
    default:
      break;
    }
    t=TREE_CHAIN(t);
  }
}
  
genwrite() {
  int out = fopen("dcltables.c", "w");
  fprintf(out, "#include \"cli.h\"\n\n");
  fprintf(out, "struct _cdu cdu_root[] = {\n");

  int i = 0;
  struct _cdu * cdu = &cdu_root[0];

  while (i<cdu_free) {
    fprintf(out, "// element %x\n",i);
    fprintf(out, "  {\n");

    fprintf(out, "    cdu$l_next: 0x%x,\n",cdu->cdu$l_next);
    fprintf(out, "    cdu$l_cbl: 0x%x,\n",cdu->cdu$l_cbl);
    fprintf(out, "    cdu$w_size: 0x%x,\n",cdu->cdu$w_size);
    fprintf(out, "    cdu$b_type: 0x%x,\n",cdu->cdu$b_type);
    fprintf(out, "    cdu$b_rmod: 0x%x,\n",cdu->cdu$b_rmod);
    fprintf(out, "    cdu$l_parent: 0x%x,\n",cdu->cdu$l_parent);
    fprintf(out, "    cdu$l_child: 0x%x,\n",cdu->cdu$l_child);
    fprintf(out, "    cdu$l_verb: 0x%x,\n",cdu->cdu$l_verb);
    fprintf(out, "    cdu$l_image: 0x%x,\n",cdu->cdu$l_image);
    fprintf(out, "    cdu$l_routine: 0x%x,\n",cdu->cdu$l_routine);
    fprintf(out, "    cdu$l_qualifiers: 0x%x,\n",cdu->cdu$l_qualifiers);
    fprintf(out, "    cdu$l_parameters: 0x%x,\n",cdu->cdu$l_parameters);
    fprintf(out, "    cdu$l_name: 0x%x,\n",cdu->cdu$l_name);
    fprintf(out, "    cdu$l_value: 0x%x,\n",cdu->cdu$l_value);
    //    fprintf(out, "cdu$l_: 0x%x,\n",cdu->cdu$l_);

    fprintf(out, "  },\n");

    i++;
    cdu++;
  }

  fprintf(out,"};\n");

  fclose(out);
}
