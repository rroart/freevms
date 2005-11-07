#include <sys/types.h>
#include <unistd.h>
#include <libelf.h>
#include <ssdef.h>
#include <descrip.h>
#include <misc.h>

static int myfunc(int (*func)(),void * start, int count) {
  __asm__ __volatile__(
		       "pushl %ebx\n\t"
		       "pushl %ecx\n\t"
		       "pushl %edx\n\t"
		       "pushl %edi\n\t"
		       "pushl %esi\n\t"
		       "movl 0x8(%ebp),%eax\n\t"
		       "movl 0xc(%ebp),%esi\n\t"
		       "movl 0x10(%ebp),%ecx\n\t"
		       "movl $0x400,%ecx\n\t"
		       "movl %ecx,%edx\n\t" 
		       "subl $0x1000,%esp\n\t"
		       "movl %esp,%edi\n\t"
		       "rep ; movsl\n\t"
		       "jmp *%eax\n\t"
		       );
  // return eax default?
}

static int mymyfunc(int dummy,int (*func)(),void * start, int count) {
  long * ret = &func;
  ret=&dummy;
  struct _exh exh;
  memset(&exh, 0, sizeof(exh));
  exh.exh$l_handler=ret[-1];
  exh.exh$l_first_arg=&ret[-1];
  int sts = sys$dclexh(&exh);
  return myfunc(*func,start,count);
}

static int mymymyfunc(int (*func)(),void * start, int count) {
  register int __res;
  __asm__ ( "movl %%ebp,%%eax\n\t" :"=a" (__res) );
  mymyfunc(__res,*func,start,count);
  __asm__ ( "movl (%esp),%ebp\n\t" );
}

static int dummy_routine(void) {
  exit(0);
  return 0;
}

int load_elf(char * filename) {
  int sts;
  void (*func)();
  struct dsc$descriptor dflnam;
  dflnam.dsc$w_length=strlen(filename);
  dflnam.dsc$a_pointer=filename;
  char * hdrbuf=malloc(512 * 8);
  memset(hdrbuf, 0, 512 * 8);

  sts=sys$imgact(&dflnam,&dflnam,hdrbuf,0,0,0,0,0);
  printf("imgact got sts %x\n",sts);
  sts=sys$imgfix();
  printf("imgfix got sts %x\n",sts);

  if (sts!=SS$_NORMAL) return sts;

  Elf32_Ehdr * elf = hdrbuf;
  func = elf->e_entry;
  printf("entering image? %x\n",func);
  long entry=func;
  long arg=0;
  long *addr=&elf->e_version;
  if (*addr!=func) {
    arg=func;
    func=*addr;
  }
  addr=&elf->e_ident;
  int i;
  long * prep=*addr;
  for(i=0;i<100;i++) {
    if (prep[i]==9 && prep[i+1]==entry) {
      prep[i+1]=dummy_routine;
      if (func==entry)
	func=dummy_routine;
      break;
    }
  }
  int offset = ((long)(*addr)) - ((long)elf);
  sts = mymymyfunc(func,*addr,(4096-offset)>>2);
  printf("after image\n");
  return SS$_NORMAL;
}

long elf_get_symbol(char * filename, char * name){
  int i;
  int fildes;
  Elf32_Ehdr * ehdr;
  Elf * arf, * elf;
  Elf32_Shdr * section_headers;
  Elf32_Shdr * section;
  Elf32_Shdr * strtabsh = 0, * symtabsh = 0;
  Elf32_Sym * symtab;
  char * strtab, * mystrtab;
  int offs;
  int val=0;
   
  fildes=open(filename, 0);
  if (elf_version(EV_CURRENT) == EV_NONE)
    {
      printf("dated\n");
      return 0;
    }
  arf = elf_begin(fildes, ELF_C_READ, (Elf *)0);
  elf = elf_begin(fildes, ELF_C_READ, arf);
  ehdr = elf32_getehdr(elf);

  section_headers = (void *) malloc(sizeof(Elf32_Shdr)*ehdr->e_shnum);
  lseek(fildes,ehdr->e_shoff,SEEK_SET);
  read(fildes, section_headers, sizeof(Elf32_Shdr)*ehdr->e_shnum);
  for (i = 0, section = section_headers;
       i < ehdr->e_shnum;
       i++, section++)
    {
      if (  (section->sh_flags&SHF_ALLOC) || i==ehdr->e_shstrndx || ( section->sh_type != SHT_SYMTAB && section->sh_type != SHT_STRTAB))
	continue;
      if (section->sh_type==SHT_SYMTAB)
	symtabsh=section;
      if (section->sh_type==SHT_STRTAB)
	strtabsh=section;
    }
  if (strtabsh==0 || symtabsh==0)
    return 0;
  strtab = (void *) malloc(strtabsh->sh_size);
  lseek(fildes, strtabsh->sh_offset, SEEK_SET);
  read(fildes, strtab, strtabsh->sh_size);
  symtab = (void *) malloc(symtabsh->sh_size);
  lseek(fildes, symtabsh->sh_offset, SEEK_SET);
  read(fildes, symtab, symtabsh->sh_size);
  mystrtab=strtab;
  mystrtab++;
  for(;*mystrtab;mystrtab+=strlen(mystrtab)+1) {
    if (0==strcmp(name,mystrtab))
      break;
  }
  offs=mystrtab-strtab;
  if (offs) {
    int max=symtabsh->sh_size/sizeof(Elf32_Sym);
    for(i=0;i<max;i++) {
      if(symtab[i].st_name==offs) {
	val=symtab[i].st_value;
	break;
      }
    }
  }
  elf_end(elf);
  elf_end(arf);
  close(fildes);
  free(section_headers);
  free(symtab);
  free(strtab);
  return val;
}
