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
#include<climsgdef.h>

#include "tree.h"
#include "cld.h"

#include "cli.h"

#define CDU_ROOT_SIZE 1000
static struct _cdu parse_cdu_root[CDU_ROOT_SIZE];
int cdu_free = 0;

insert_cdu(int c) {
#if 0
  c->cdu$l_next=parse_cdu_root;
  parse_cdu_root=c;
#endif
  parse_cdu_root[c].cdu$l_next=parse_cdu_root[0].cdu$l_next;
  parse_cdu_root[0].cdu$l_next=c;
}

#if 0
get_cdu_root() {
  extern struct _cdu cdu_root[];
  return &cdu_root[0];
}
#endif

my_cdu_free = 0;
struct _cdu my_cdu_root[50];

my_alloc_cdu(int t) {
  int b = my_cdu_free++;
  int a = &my_cdu_root[b];
  memset(a,0,sizeof(struct _cdu));
  my_cdu_root[b].cdu$b_type=t;
  return b;
}

my_alloc_name(char * n) {
  int name = my_alloc_cdu(CDU$C_NAME);
  struct _cdu * np = &my_cdu_root[name];
  memcpy(np->cdu$t_name,n,strlen(n));
  return name;
}

alloc_cdu(int t) {
  int b = cdu_free++;
  if (b==CDU_ROOT_SIZE) {
    printf("cdu_root overflow\n");
    exit(0);
  }
  int a = &parse_cdu_root[b];
  memset(a,0,sizeof(struct _cdu));
  parse_cdu_root[b].cdu$b_type=t;
  return b;
}

alloc_name(char * n) {
  int name = alloc_cdu(CDU$C_NAME);
  struct _cdu * np = &parse_cdu_root[name];
  memcpy(np->cdu$t_name,n,strlen(n));
  return name;
}

#if 0
cdu_strncmp(int cdu, char * n, int size) {
  extern struct _cdu * cdu_root; // not used. check
  struct _cdu * np = &cdu_root[cdu];
  return strncmp(np->cdu$t_name, n, size);
}
#endif

int cdu_search_next(int i, int type, char * s, int size, int * retval) {
  struct _cdu * cdu_root = *root_cdu;
  do {
    struct _cdu * name;
    switch (type) {
    case CDU$C_VERB:
      name=&cdu_root[cdu_root[i].cdu$l_verb];
      break;
    case CDU$C_TYPE:
      name=&cdu_root[cdu_root[i].cdu$l_type];
      break;
    case CDU$C_SYNTAX:
      name=&cdu_root[cdu_root[i].cdu$l_syntax];
      break;
    case CDU$C_KEYWORD:
      name=&cdu_root[cdu_root[i].cdu$l_name];
      break;
    case CDU$C_QUALIFIER:
      name=&cdu_root[cdu_root[i].cdu$l_name];
      break;
    default:
      printf("oops not found\n");
    }
    if (cdu_root[i].cdu$b_type==type && strncmp(name->cdu$t_name,s,size)==0) {
      if (retval)
	*retval=i;
      return 1;
    }
    i=cdu_root[i].cdu$l_next;
  } while (i);
  return 0;
}

int cdu_search_incr(int i, int type, char * s, int size, int * retval) {
  struct _cdu * cdu_root = *root_cdu;
  do {
    struct _cdu * name;
    switch (type) {
    case CDU$C_VERB:
      name=&cdu_root[cdu_root[i].cdu$l_verb];
      break;
    case CDU$C_TYPE:
      name=&cdu_root[cdu_root[i].cdu$l_type];
      break;
    case CDU$C_SYNTAX:
      name=&cdu_root[cdu_root[i].cdu$l_syntax];
      break;
    case CDU$C_QUALIFIER:
      name=&cdu_root[cdu_root[i].cdu$l_name];
      break;
    default:
      printf("oops not found\n");
    }
    if (cdu_root[i].cdu$b_type==type && strncmp(name->cdu$t_name,s,size)==0) {
      if (retval)
	*retval=i;
      return 1;
    }
    i++;
  } while (cdu_root[i].cdu$b_type);
  return 0;
}

static int mm_done = 0;

unsigned int cli$dcl_parse(void * command_string ,void * table ,void * param_routine, void * prompt_routine, void * prompt_string) {

  if (mm_done==0) {
    mm_done=1;
    mmap(0x3f000000, 4,PROT_READ|PROT_WRITE,MAP_FIXED|MAP_ANONYMOUS|MAP_SHARED,0,0);
  }

  struct dsc$descriptor * com = command_string;
  struct _cdu * cdu_root = table;
  *root_cdu=table;

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

  found = cdu_search_next(0, CDU$C_VERB, line, endverb-line, &i);

  *cur_cdu=&cdu_root[i];
  if (!found)
    return 0;

  my_cdu_free=0;
  struct _cdu * my = &my_cdu_root[my_alloc_cdu(CDU$C_VERB)];
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
      while (*endunit!=' ' && *endunit!='/' && *endunit!='=' && endunit < end)
	endunit++;
      int q = my_alloc_cdu(CDU$C_QUALIFIER);
      int n = my_alloc_cdu(CDU$C_NAME);
      my_cdu_root[q].cdu$l_name = n;
      struct _cdu * np = &my_cdu_root[n];
      memcpy(np->cdu$t_name,line,endunit-line);
      my_cdu_root[q].cdu$l_next=my->cdu$l_qualifiers;
      my->cdu$l_qualifiers=q;

      struct _cdu * cdu = (*cur_cdu);
      int s = 0;
      int q2 = cdu->cdu$l_qualifiers;
      found = cdu_search_next(q2, CDU$C_QUALIFIER, line, endunit-line, &q2);
      if (q2)
	s = cdu_root[q2].cdu$l_syntax;
      if (s) {
	int i;
	found = cdu_search_next(0, CDU$C_SYNTAX, cdu_root[s].cdu$t_name, strlen(cdu_root[s].cdu$t_name), &i);
	if (found)
	  *cur_cdu=&cdu_root[i];
      }

      line = endunit;
      while (*line==' ')
	line++;
      endunit = line;

      if (*endunit == '=') {
	line++;
	endunit++;
	while (*endunit!=' ' && *endunit!='/' && endunit < end)
	  endunit++;
	int v = my_alloc_cdu(CDU$C_NAME);
	my_cdu_root[q].cdu$l_value=v;
	struct _cdu * np = &my_cdu_root[v];
	memcpy(np->cdu$t_name,line,endunit-line);
      }

    } else {
      while (*endunit!=' ' && *endunit!='/' && endunit < end)
	endunit++;

      struct _cdu * cdu = (*cur_cdu);
      int p,n,t,v;
      p=0;
      n=0;
      t=0;
      v=0;
      p = cdu->cdu$l_parameters;
      if (p) v = cdu_root[p].cdu$l_value;
      if (v) t = cdu_root[v].cdu$l_type;
      if (t) {
	i = 0;

	found = cdu_search_next(0, CDU$C_TYPE, cdu_root[t].cdu$t_name, strlen(cdu_root[t].cdu$t_name), &i);

	int type = i;

	int keyw = cdu_root[type].cdu$l_keywords;
	found = cdu_search_next(keyw, CDU$C_KEYWORD, line, endunit-line, &keyw);

	if (cdu_root[keyw].cdu$l_syntax) {
	  
	  found = cdu_search_next(0, CDU$C_SYNTAX, cdu_root[cdu_root[keyw].cdu$l_syntax].cdu$t_name, strlen(cdu_root[cdu_root[keyw].cdu$l_syntax].cdu$t_name), &i);
	  if (found)
	    *cur_cdu=&cdu_root[i];
	  
	}
      } else { // ordinary param?
	p = my_alloc_cdu(CDU$C_PARAMETER);
#if 0
	n = my_alloc_cdu(CDU$C_NAME);
	my_cdu_root[p].cdu$l_name = n;
	struct _cdu * np = &my_cdu_root[n];
	np->cdu$t_name[0]='p';
	np->cdu$t_name[1]='0'+pn;
#else
	{
	  int i;
	  int count;
	  int total;
	  i = cdu->cdu$l_parameters;
	  for(total=0;i;total++)
	    i=cdu_root[i].cdu$l_next;
	  i = cdu->cdu$l_parameters;
	  for(count=total-pn;count;count--)
	    i=cdu_root[i].cdu$l_next;
	  n = my_alloc_cdu(CDU$C_NAME);
	  my_cdu_root[p].cdu$l_name = n;
	  struct _cdu * np = &my_cdu_root[n];
	  *np=cdu_root[cdu_root[i].cdu$l_name]; // memcpy included
	}
#endif
	v = my_alloc_cdu(CDU$C_NAME);
	struct _cdu * vp = &my_cdu_root[v];
	my_cdu_root[p].cdu$l_value = v;
	memcpy(vp->cdu$t_name,line,endunit-line);
	my_cdu_root[p].cdu$l_next=my->cdu$l_parameters;
	my->cdu$l_parameters=p;
	pn++;
      }
    }
    line = endunit;
  }

  return CLI$_NORMAL;
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
  char * mainp="main";
  char * routine=mainp;
  char * myp1 = "";
  char * myargv[4]={"",myp1,myp1,myp1};
  char image[256];
  char * path;
  int pathlen = 0;
  int n;
  char * imagebase;
  struct _cdu * cdu;
  struct _cdu * cdu_root = *root_cdu;
  void (*func)();
  void * handle = 0;
  int value = 0;
  int internal = 0;

  cdu= &cdu_root[(*cur_cdu)->cdu$l_routine];
  if (cdu->cdu$t_name[0])
    routine = cdu->cdu$t_name;

#if 0
  if ((*cur_cdu)->cdu$l_flags&CDU$M_INTERNAL) {
    value=get_cli_int(routine);
    func=value;
    internal=1;
    goto skip;
  }
#else
  value=get_cli_int(routine);
  if (value) {
    func=value;
    internal=1;
    goto skip;
  }
#endif

  n = (*cur_cdu)->cdu$l_image;
  imagebase = cdu_root[n].cdu$t_name;
  int len = strlen(imagebase);

  int is_ele=0;
  if (vms_mm==0 || 0==strncmp(".ele",imagebase+len-4,4)) goto skipthis;
  if (vms_mm==0 || 0==strncmp("_ele",imagebase+len-4,4)) {
  skipthis:
    if (vms_mm) image[pathlen+len-4]=0;
    is_ele=1;
  }

  if (is_ele) {
    path="/vms$common/sysexe/";
    pathlen=strlen(path);
  } else {
    path="SYS$SYSTEM:";
    pathlen=strlen(path);
  }

  memcpy(image,path,pathlen);
  memcpy(image+pathlen,imagebase,strlen(imagebase));

  if (is_ele && vms_mm) image[pathlen+len-4]=0;
  if (is_ele && vms_mm) imagebase[len-4]=0;
  if (is_ele)
    goto ele;

  memcpy(image+pathlen+strlen(imagebase),".exe",4);
  image[pathlen+strlen(imagebase)+4]=0;

  unsigned long sts;
  struct dsc$descriptor aname;
  struct dsc$descriptor dflnam;
  struct file * f;
  struct _ihd * hdrbuf;
  struct _iha * active;
  struct _va_range inadr;
  char * imgnam = imagebase;

  aname.dsc$w_length=len;
  aname.dsc$a_pointer=imgnam;
  dflnam.dsc$w_length=pathlen+strlen(imagebase)+4;
  dflnam.dsc$a_pointer=image;

  hdrbuf=malloc(512);
  memset(hdrbuf, 0, 512);

  sts=sys$imgact(&aname,&dflnam,hdrbuf,0,0,0,0,0);
  printf("imgact got sts %x\n",sts);
  sts=sys$imgfix();
  printf("imgfix got sts %x\n",sts);

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

  goto skip;

 ele:
  {}

  int fildes=open(image, 0);
  if (fildes>0) {
    close(fildes);
    image[pathlen+strlen(imagebase)]=0;
    load_elf(image);
    func = elf_get_symbol(image, routine);
    goto skip;
  }
 skip_if_mm:

  memcpy(image+pathlen+strlen(imagebase),".ele",4);
  image[pathlen+strlen(imagebase)+4]=0;
  printf("Opening %s\n",image);
  handle = dlopen(image,RTLD_NOW);
  if (handle==0) {
    printf("dlopen: %s\n",dlerror());
    fflush(stdout);
    return 0;
  }
  dlerror(); // clear error
  printf("Find routine %s\n",routine);
  func = dlsym(handle,routine);
  printf("Got function address %x\n",func);
  int error=dlerror();
  if (error) {
    printf("dlsym: %s\n",error);
    dlclose(handle);
    return 0;
  }

 skip:
#if 0
  if (handle==0)
    printf("entering image? %x\n",func);
#endif

  // not yet  func(userarg);
  if ((*my_cdu)->cdu$l_parameters) {
    int i=(*my_cdu)->cdu$l_parameters;
    while (i) {
      struct _cdu * p = &my_cdu_root[i];
      struct _cdu * n = &my_cdu_root[p->cdu$l_value];
      myargv[my_cdu_root[my_cdu_root[i].cdu$l_name].cdu$t_name[1]-'0'] = n->cdu$t_name;
      i=my_cdu_root[i].cdu$l_next;
    }
  }
  func(userarg,myargv,0,0);
  //func(argc,argv++);
  if (!internal) {
    if (handle==0) {
      //printf("after image\n");
      sys$rundwn();
    } else {
      dlclose(handle);
    }
  }

  return SS$_NORMAL;
}

char * module_name = 0;

int gencode(tree t) {
  while (t) {
    switch (TREE_CODE(t)) {
    case DEFINE_VERB_STMT:
      {
	int cdu = alloc_cdu(CDU$C_VERB);
	parse_cdu_root[cdu].cdu$l_verb = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
	insert_cdu(cdu);
	tree verb = TREE_OPERAND(t, 1);
	gencode_verb(verb, &parse_cdu_root[cdu]);
      }
      break;
    case DEFINE_SYNTAX_STMT:
      {
	int cdu = alloc_cdu(CDU$C_SYNTAX);
	parse_cdu_root[cdu].cdu$l_syntax = alloc_name (IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
	insert_cdu(cdu);
	tree verb = TREE_OPERAND(t, 1);
	gencode_verb(verb, &parse_cdu_root[cdu]);
      }
      break;
    case DEFINE_TYPE_STMT:
      {
	int cdu = alloc_cdu(CDU$C_TYPE);
	parse_cdu_root[cdu].cdu$l_type = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
	insert_cdu(cdu);
	tree type = TREE_OPERAND(t, 1);
	gencode_type(type, &parse_cdu_root[cdu]);
      }
      break;
    case MODULE_STMT:
      module_name = IDENTIFIER_POINTER(TREE_OPERAND(t, 0));
      break;
    default:
      { }
    }
    t=TREE_CHAIN(t);
  }
}

int gencode_verb(tree t,struct _cdu * cdu) {
  while (t) {
    switch (TREE_CODE(t)) {
    case CLIFLAGS_CLAUSE:
      gencode_cliflags(TREE_OPERAND(t, 0), cdu);
      break;
    case CLIROUTINE_CLAUSE:
      cdu->cdu$l_routine = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
      cdu->cdu$l_flags|=CDU$M_INTERNAL;
      break;
    case ROUTINE_CLAUSE:
      cdu->cdu$l_routine = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
      break;
    case IMAGE_CLAUSE:
      cdu->cdu$l_image = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
      break;
    case QUALIFIER_CLAUSE:
      {
	int q = alloc_cdu(CDU$C_QUALIFIER);
	parse_cdu_root[q].cdu$l_name = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
	gencode_qual_clauses(TREE_OPERAND(t, 1),&parse_cdu_root[q]);
	parse_cdu_root[q].cdu$l_next=cdu->cdu$l_qualifiers;
	cdu->cdu$l_qualifiers=q;
      }
      break;
    case PARAMETER_CLAUSE:
      {
	int p = alloc_cdu(CDU$C_PARAMETER);
	parse_cdu_root[p].cdu$l_name=alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));;
	gencode_para_clauses(TREE_OPERAND(t, 1),&parse_cdu_root[p]);
	parse_cdu_root[p].cdu$l_next=cdu->cdu$l_parameters;
	cdu->cdu$l_parameters=p;
      }
      break;
    default:
      break;
    }
    t=TREE_CHAIN(t);
  }
}

int gencode_type(tree t,struct _cdu * cdu) {
  while (t) {
    switch (TREE_CODE(t)) {
    case KEYWORD_CLAUSE:
      {
	int k = alloc_cdu(CDU$C_KEYWORD);
	parse_cdu_root[k].cdu$l_name = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
	gencode_keyw_clauses(TREE_OPERAND(t, 1),&parse_cdu_root[k]);
	parse_cdu_root[k].cdu$l_next=cdu->cdu$l_keywords;
	cdu->cdu$l_keywords=k;
      }
      break;
    default:
      break;
    }
    t=TREE_CHAIN(t);
  }
}

struct flag_table {
  int code;
  int flag;
};

struct flag_table codeflags[] = {
{ NODISALLOWS_CLAUSE, CDU$M_NODISALLOWS },
{ NOPARAMETERS_CLAUSE, CDU$M_NOPARAMETERS },
{ NOQUALIFIERS_CLAUSE, CDU$M_NOQUALIFIERS },
{ BATCH_CLAUSE, CDU$M_BATCH },
{ NEGATABLE_CLAUSE, CDU$M_NEGATABLE },
{ NONNEGATABLE_CLAUSE, CDU$M_NONNEGATABLE },
{ DEFAULT_CLAUSE, CDU$M_DEFAULT },
{ ABBREVIATE_CLAUSE, CDU$M_ABBREVIATE },
{ FOREIGN_CLAUSE, CDU$M_FOREIGN },
{ IMMEDIATE_CLAUSE, CDU$M_IMMEDIATE },
{ MCRIGNORE_CLAUSE, CDU$M_MCRIGNORE },
{ MCROPTDELIM_CLAUSE, CDU$M_MCROPTDELIM },
{ MCRPARSE_CLAUSE, CDU$M_MCRPARSE },
{ NOSTATUS_CLAUSE, CDU$M_NOSTATUS },
{ CONCATENATE_CLAUSE, CDU$M_CONCATENATE },
{ NOCONCATENATE_CLAUSE, CDU$M_NOCONCATENATE },
{ IMPCAT_CLAUSE, CDU$M_IMPCAT },
{ LIST_CLAUSE, CDU$M_LIST },
{ REQUIRED_CLAUSE, CDU$M_REQUIRED },
{ 0, 0 },
};

int gencode_set_flag(struct _cdu * cdu, int code) {
  int i;
  for (i=0;codeflags[i].code;i++)
    if (codeflags[i].code==code) {
      cdu->cdu$l_flags|=codeflags[i].flag;
      return 1;
    }
  return 0;
}
  
int gencode_para_clauses(tree t,struct _cdu * cdu) {
  while (t) {
    if (gencode_set_flag(cdu, TREE_CODE(t)))
      goto next;
    switch (TREE_CODE(t)) {
    default:
      break;
    }
    gencode_single_clause(t,cdu);
  next:
    t=TREE_CHAIN(t);
  }
}

int gencode_qual_clauses(tree t,struct _cdu * cdu) {
  while (t) {
    if (gencode_set_flag(cdu, TREE_CODE(t)))
      goto next;
    switch (TREE_CODE(t)) {
    default:
      break;
    }
    gencode_single_clause(t,cdu);
  next:
    t=TREE_CHAIN(t);
  }
}

int gencode_cliflags(tree t,struct _cdu * cdu) {
  while (t) {
    gencode_set_flag(cdu, TREE_CODE(t));
    t=TREE_CHAIN(t);
  }
}

int gencode_single_clause(tree t,struct _cdu * cdu) {
  if (gencode_set_flag(cdu, TREE_CODE(t)))
    return;
  switch (TREE_CODE(t)) {
  case CLIFLAGS_CLAUSE:
    gencode_cliflags(TREE_OPERAND(t, 0), cdu);
    break;
  case LABEL_CLAUSE:
    cdu->cdu$l_label=alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
    cdu->cdu$l_name=cdu->cdu$l_label; // check
    break;
  case SYNTAX_CLAUSE:
    cdu->cdu$l_syntax=alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
    break;
  case VALUE_CLAUSE:
    {
      int n = alloc_cdu(CDU$C_VALUE);
      cdu->cdu$l_value=n;
      gencode_value_clauses(TREE_OPERAND(t, 0), &parse_cdu_root[n]);
    }
    break;
  default:
    break;
  }
}

int gencode_value_clauses(tree t,struct _cdu * cdu) {
  while (t) {
    if (gencode_set_flag(cdu, TREE_CODE(t)))
      goto next;
    switch (TREE_CODE(t)) {
    case TYPE_CLAUSE:
      cdu->cdu$l_type = alloc_name(IDENTIFIER_POINTER(TREE_OPERAND(t, 0)));
    default:
      break;
    }
  next:
    t=TREE_CHAIN(t);
  }
}

int gencode_keyw_clauses(tree t,struct _cdu * cdu) {
  while (t) {
    if (gencode_set_flag(cdu, TREE_CODE(t)))
      goto next;
    switch (TREE_CODE(t)) {
    default:
      break;
    }
    gencode_single_clause(t,cdu);
  next:
    t=TREE_CHAIN(t);
  }
}

genwrite() {
  int out;
  if (module_name) {
    int len = strlen(module_name);
    char * dup = strndup(module_name,len+4);
    dup[len]='.';
    dup[len+1]='c';
    dup[len+2]=0;
    char * dupagain = strndup(dup,len+2);
    out = fopen(dupagain, "w");
  } else
    out = fopen("dcltables.c", "w");
  fprintf(out, "#include \"cli.h\"\n\n");
  if (module_name)
    fprintf(out, "struct _cdu %s[] = {\n",module_name);
  else
    fprintf(out, "struct _cdu cdu_root[] = {\n");
  int i = 0;
  struct _cdu * cdu = &parse_cdu_root[0];

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
    fprintf(out, "    cdu$l_flags: 0x%x,\n",cdu->cdu$l_flags);
    fprintf(out, "    cdu$l_label: 0x%x,\n",cdu->cdu$l_label);
    fprintf(out, "    cdu$l_type: 0x%x,\n",cdu->cdu$l_type);
    fprintf(out, "    cdu$l_syntax: 0x%x,\n",cdu->cdu$l_syntax);
    fprintf(out, "    cdu$l_clauses: 0x%x,\n",cdu->cdu$l_clauses);
    fprintf(out, "    cdu$l_keywords: 0x%x,\n",cdu->cdu$l_keywords);
    //    fprintf(out, "    cdu$l_: 0x%x,\n",cdu->cdu$l_);

    fprintf(out, "  },\n");

    i++;
    cdu++;
  }

  fprintf(out,"};\n");

  fclose(out);
}
