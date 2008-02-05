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

#include <misc.h>

int is_user_mode() {
  __asm__ __volatile__ (
			"movw %cs, %ax\n\t"
			"andw $0x3, %ax\n\t"
			"cmpw $0x3, %ax\n\t"
			"je 1f\n\t"
			"movl $0, %eax\n\t"
			"jmp 2f\n\t"
			"1:\n\t"
			"movl $1, %eax\n\t"
			"2:\n\t"
			);
}

my_cdu_free = 0;
struct _cdu my_cdu_root[100];

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

#if 0
cdu_strncmp(int cdu, char * n, int size) {
  extern struct _cdu * cdu_root; // not used. check
  struct _cdu * np = &cdu_root[cdu];
  return strncmp(np->cdu$t_name, n, size);
}
#endif

int cdu_search_next(int i, int type, char * s, int size, int * retval) {
  if (size==0)
    return 0;
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

  my_cdu_free = 0;
  if (mm_done==0) {
    mm_done=1;
    mmap(0x3f000000, 4,PROT_READ|PROT_WRITE,MAP_FIXED|MAP_ANONYMOUS|MAP_SHARED,0,0);
  }

  struct dsc$descriptor * com = command_string;
  *comdsc = com;
  struct _cdu * cdu_root = table;
  *root_cdu=table;

  char * line = com->dsc$a_pointer;
  int len = com->dsc$w_length;

  char * end = line + len;

#if 1
  cli_scan_bytes(line, len);
#endif
#if 1
  initparser();
#endif

  int i;
#if 0
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
#endif

  int toktype;
  char token[256];
  int toklen;
  toktype = cli_token(token, &toklen);

  i = 0;
  int found = 0;

  found = cdu_search_next(0, CDU$C_VERB, token, toklen, &i);

  int endverb = 0;

  *cur_cdu=&cdu_root[i];
  if (!found)
    return 0;

  my_cdu_free=0;
  struct _cdu * my = &my_cdu_root[my_alloc_cdu(CDU$C_VERB)];
  *my_cdu=my;

#if 0
  line=endverb;
#endif

  int pn = 1;

#if 0
  char * endunit = 0;
#endif

  extern int checkq;

  checkq = 1;
  toktype = cli_token(token, &toklen);
  checkq = 0;
  while (toktype) {

#if 0
    while (*line==' ')
      line++;
#endif

#if 0
    endunit = line;
#endif

    if (toktype == '/') { // T_QUALIFIER
      toktype = cli_token(token, &toklen);
#if 0
      line++;
      endunit++;
      while (*endunit!=' ' && *endunit!='/' && *endunit!='=' && endunit < end)
	endunit++;
#endif
      int q = my_alloc_cdu(CDU$C_QUALIFIER);
      int n = my_alloc_cdu(CDU$C_NAME);
      my_cdu_root[q].cdu$l_name = n;
      struct _cdu * np = &my_cdu_root[n];
      memcpy(np->cdu$t_name,token,toklen);
      my_cdu_root[q].cdu$l_next=my->cdu$l_qualifiers;
      my->cdu$l_qualifiers=q;

      struct _cdu * cdu = (*cur_cdu);
      int s = 0;
      int q2 = cdu->cdu$l_qualifiers;
      found = cdu_search_next(q2, CDU$C_QUALIFIER, token, toklen, &q2);
      if (q2)
	s = cdu_root[q2].cdu$l_syntax;
      if (s) {
	int i;
	found = cdu_search_next(0, CDU$C_SYNTAX, cdu_root[s].cdu$t_name, strlen(cdu_root[s].cdu$t_name), &i);
	if (found)
	  *cur_cdu=&cdu_root[i];
      }

#if 0
      line = endunit;
      while (*line==' ')
	line++;
      endunit = line;
#endif

      checkq = 3;
      toktype = cli_token(token, &toklen);
      checkq = 0;

      if (toktype == '=') {
#if 0
	line++;
	endunit++;
	while (*endunit!=' ' && *endunit!='/' && endunit < end)
	  endunit++;
#endif
	toktype = cli_token(token, &toklen);

	if (toktype == '(') {

	  toktype = cli_token(token, &toklen);

	  int k = my_alloc_cdu(CDU$C_KEYWORD);
	  int v = my_alloc_cdu(CDU$C_NAME);
	  my_cdu_root[q].cdu$l_value=k;
	  my_cdu_root[k].cdu$l_name=v;
	  my_cdu_root[q].cdu$l_flags=cdu_root[cdu_root[q2].cdu$l_value].cdu$l_flags;
	  struct _cdu * np = &my_cdu_root[v];
	  memcpy(np->cdu$t_name,token,toklen);

	  toktype = cli_token(token, &toklen);
	  if (toktype != ',' && toktype != '=')
	    return 0;

	  if (toktype == '=') {
	    toktype = cli_token(token, &toklen);
	    int v3 = my_alloc_cdu(CDU$C_NAME);
	    my_cdu_root[k].cdu$l_value=v3;
	    struct _cdu * np = &my_cdu_root[v3];
	    memcpy(np->cdu$t_name,token,toklen);
	    toktype = cli_token(token, &toklen);
	  }

	  toktype = cli_token(token, &toklen);

	  while (toktype && toktype != ')') {
	    int k2 = my_alloc_cdu(CDU$C_KEYWORD);
	    int v2 = my_alloc_cdu(CDU$C_NAME);
	    my_cdu_root[k].cdu$l_next=k2;
#if 0
	    my_cdu_root[q].cdu$l_value=k2;
#endif
	    my_cdu_root[k2].cdu$l_name=v2;
	    my_cdu_root[q].cdu$l_flags=cdu_root[cdu_root[q2].cdu$l_value].cdu$l_flags; // check
	    struct _cdu * np = &my_cdu_root[v2];
	    memcpy(np->cdu$t_name,token,toklen);

	    k = k2;

	    toktype = cli_token(token, &toklen);
	    if (toktype == ')')
	      continue;
	    if (toktype != ',' && toktype != '=')
	      return 0;
	    
	    if (toktype == '=') {
	      toktype = cli_token(token, &toklen);
	      int v3 = my_alloc_cdu(CDU$C_NAME);
	      my_cdu_root[k2].cdu$l_value=v3;
	      my_cdu_root[k2].cdu$l_flags = CDU$M_LIST; // check
	      struct _cdu * np = &my_cdu_root[v3];
	      memcpy(np->cdu$t_name,token,toklen);
	      //toktype = cli_token(token, &toklen);
	      if (toktype == ')')
		continue;
	      k2 = v3;
	      while (toktype && toktype != ')') {
		int k3 = my_alloc_cdu(CDU$C_KEYWORD);
		int v4 = my_alloc_cdu(CDU$C_NAME);
		my_cdu_root[k2].cdu$l_next=k3;
#if 0
		my_cdu_root[q].cdu$l_value=k3;
#endif
		my_cdu_root[k3].cdu$l_name=v4;
		my_cdu_root[q].cdu$l_flags=cdu_root[cdu_root[q2].cdu$l_value].cdu$l_flags; // check
		struct _cdu * np = &my_cdu_root[v4];
		memcpy(np->cdu$t_name,token,toklen);

		k2 = k3;

		toktype = cli_token(token, &toklen);
		if (toktype == ')')
		  goto out_of_inner;
#if 0
		if (toktype == '+')
		  continue;
#endif
		if (toktype != '+' && toktype != '=')
		  return 0;
	    
		toktype = cli_token(token, &toklen);
	      }
	    }
	    toktype = cli_token(token, &toklen);

	  out_of_inner:
	  }

	} else {

	  int k = my_alloc_cdu(CDU$C_KEYWORD);
	  int v = my_alloc_cdu(CDU$C_NAME);
	  my_cdu_root[q].cdu$l_value=k;
	  my_cdu_root[k].cdu$l_name=v;
	  my_cdu_root[q].cdu$l_flags=cdu_root[cdu_root[q2].cdu$l_value].cdu$l_flags;
	  struct _cdu * np = &my_cdu_root[v];
	  memcpy(np->cdu$t_name,token,toklen);
	}
      } else
	continue; // check

    } else {
      // param handling
      // just swallow until space or /, mostly
      // later do a refined thingie with $infile, $rest etc
#if 0
      while (*endunit!=' ' && *endunit!='/' && endunit < end)
	endunit++;
#endif

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
	found = cdu_search_next(keyw, CDU$C_KEYWORD, token, toklen, &keyw);

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
	  for(count=total-pn;count>0;count--)
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
	memcpy(vp->cdu$t_name,token,toklen);
	my_cdu_root[p].cdu$l_next=my->cdu$l_parameters;
	my->cdu$l_parameters=p;
	pn++;
      }
    }
#if 0
    line = endunit;
#endif
    checkq = 1;
    toktype = cli_token(token, &toklen);
    checkq = 0;
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

  if (!vms_mm) {
    path="/vms$common/sysexe/";
    pathlen=strlen(path);
  } else {
    path="SYS$SYSTEM:";
    pathlen=strlen(path);
  }

  memcpy(image,path,pathlen);
  memcpy(image+pathlen,imagebase,len);

  if (is_ele && vms_mm) image[pathlen+len-4]=0;
  if (is_ele && vms_mm) len-=4;
  if (is_ele)
    goto ele;

  memcpy(image+pathlen+len,".exe",4);
  image[pathlen+len+4]=0;

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
  dflnam.dsc$w_length=pathlen+len+4;
  dflnam.dsc$a_pointer=image;

  hdrbuf=malloc(512); // little leak
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
    image[pathlen+len]=0;
    load_elf(image);
    func = elf_get_symbol(image, routine);
    goto skip;
  }
 skip_if_mm:

  memcpy(image+pathlen+len,".ele",4);
  image[pathlen+len+4]=0;
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
#if 0
  if ((*my_cdu)->cdu$l_parameters) {
    int i=(*my_cdu)->cdu$l_parameters;
    while (i) {
      struct _cdu * p = &my_cdu_root[i];
      struct _cdu * n = &my_cdu_root[p->cdu$l_value];
      myargv[my_cdu_root[my_cdu_root[i].cdu$l_name].cdu$t_name[1]-'0'] = n->cdu$t_name;
      i=my_cdu_root[i].cdu$l_next;
    }
  }
#endif
  if (func == 0) {
    printf("func is 0, error\n");
    goto no_func;
  }
  if (is_user_mode())
    func(userarg,myargv,0,0);
  else
    // check. related to CLI supervisor
    mymymyuserfunc(func,userarg,myargv,0,0);
 no_func:
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

// check. related to CLI supervisor
static void do_userfunc(long (*func)(), long argc, long argv) {
  func(argc, argv);
  sys$exit(0);
}

// check. related to CLI supervisor
static int userfunc(long (*func)(), long argc, long argv) {
#ifdef __i386__
  __asm__ __volatile__ (
			"movl $0x7ffdfff0, %edx\n\t"
			"movl $0, 0(%edx)\n\t"
			"movl 8(%esp), %eax\n\t"
			"movl %eax, 4(%edx)\n\t"
			"movl 12(%esp), %eax\n\t"
			"movl %eax, 8(%edx)\n\t"
			"movl 16(%esp), %eax\n\t"
			"movl %eax, 12(%edx)\n\t"
			"int $0xb4\n\t"
			"movl 0x7ffff0a8, %eax\n\t"
			"movl $0xf, 2032(%eax)\n\t" /* psl */
			"movl 2124(%eax), %eax\n\t" /* ipr_sp[3] */
			"movl %esp, %edx\n\t"
			"addl $-0x14, %edx\n\t"
			"movl $do_userfunc, 0x0(%edx)\n\t"
			"movl $0x23, 0x4(%edx)\n\t"
			"movl $0x202, 0x8(%edx)\n\t" // check
			"movl $0x7ffdfff0, 0xc(%edx)\n\t"
			"movl $0x2b, 0x10(%edx)\n\t"
			"addl $-0x14, %esp\n\t"
			"movl $0x2b, %eax\n\t"
			"movl %eax, %ds\n\t"
			"movl %eax, %es\n\t"
			"int $0xb5\n\t"
			"iret\n\t"
			);
#endif
}

// check. related to CLI supervisor
static int mymyuserfunc(int dummy,int (*func)(),void * start, long count) {
#ifdef __i386__
  long * ret = &func;
  ret=&dummy;
#else
  long * ret;
  __asm__ __volatile__ ("movq %%rbp,%0; ":"=r" (ret) );
  ret+=2;
#endif
  struct _exh exh;
  memset(&exh, 0, sizeof(exh));
  exh.exh$l_handler=ret[-1];
  exh.exh$l_first_arg=&ret[-1];
  int sts = sys$dclexh(&exh);
  return userfunc(*func,start,count);
}

// check. related to CLI supervisor
int mymymyuserfunc(int (*func)(),void * start, long count) {
  register int __res;
#ifdef __i386__
  __asm__ ( "movl %%ebp,%%eax\n\t" :"=a" (__res) );
  mymyuserfunc(__res,*func,start,count);
  __asm__ ( "movl (%esp),%ebp\n\t" );
#endif
#ifdef __x86_64__
  __asm__ __volatile__ (
			"movq %rbp,%rax\n\t"
			"pushq %rax\n\t"
			);
  mymyuserfunc(__res,*func,start,count);
  __asm__ ( "movq (%rsp),%rbp\n\t" );
#endif
}

