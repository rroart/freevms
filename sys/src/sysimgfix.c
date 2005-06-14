// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>

#include<linux/mm.h>
#include <asm/uaccess.h>

#include<starlet.h>

#include<descrip.h>
#include<ihadef.h>
#include<ihddef.h>
#include<ihidef.h>
#include<ihpdef.h>
#include<ihsdef.h>
#include<ihvndef.h>
#include<isddef.h>

#include<imcbdef.h>

#include<va_rangedef.h>

#include<system_data_cells.h>

#include <iafdef.h>
#include <ihddef.h>
#include <shldef.h>

#include<ssdef.h>

typedef struct {
  unsigned char r_offset[4];    /* Location at which to apply the action */
  unsigned char r_info[4];      /* index and type of relocation */
  unsigned char r_addend[4];    /* Constant addend used to compute value */
} Elf32_External_Rela;

typedef struct {
  unsigned char st_name[4];             /* Symbol name, index in string tbl */
  unsigned char st_value[4];            /* Value of the symbol */
  unsigned char st_size[4];             /* Associated symbol size */
  unsigned char st_info[1];             /* Type and binding attributes */
  unsigned char st_other[1];            /* No defined meaning, 0 */
  unsigned char st_shndx[2];            /* Associated section index */
} Elf32_External_Sym;

typedef struct bfd_symbol
{
  const char *name;
  int value;
  int flags;
  struct bfd_section *section;
}
asymbol;

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

signed short
bfd_getl16 (void * dummy, const void *p)
{
  const unsigned char *addr = p;
  return (addr[1] << 8) | addr[0];
}


#define bfd_h_get_16 bfd_getl16
#define bfd_h_get_32 bfd_getl32
#define bfd_h_get_signed_32 bfd_getl_signed_32
#define H_GET_16 bfd_h_get_16
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
  const Elf32_External_Sym *src = psrc;
  //  int signed_vma = get_nisse_backend_data (abfd)->sign_extend_vma;
  int signed_vma = 0; // check. added this to be sure.
  dst->name = H_GET_32 (abfd, src->st_name);
  if (signed_vma)
    dst->value = H_GET_SIGNED_WORD (abfd, src->st_value);
  else
    dst->value = H_GET_WORD (abfd, src->st_value);
  dst->flags = H_GET_WORD (abfd, src->st_size);
  dst->section = H_GET_16 (abfd, src->st_shndx);
}

int
find_symtab_offset(void * symtab, long offset, void **v) {
  while (1) {
    Elf32_External_Sym * src = symtab;
    if (src->st_name[0]==0 && src->st_name[1]==0) break;
    asymbol dst;
    nisse_swap_symbol_in(0, src, 0, &dst);
    symtab += sizeof(Elf32_External_Sym);
    if (offset==dst.name)
      *v=src;
    if (offset==dst.name)
      return dst.value;
    src++;
  }
  return -1;
}

int
find_section_offset (struct _ihd * ihd, int secno)
{
  struct _isd * isd = (long)ihd+ihd->ihd$w_size;
  while (secno && isd<((long)ihd+512*ihd->ihd$b_hdrblkcnt)) {
    if (isd->isd$w_size==0)
      break;
    if (isd->isd$w_size==0xffffffff) {
      int no=((unsigned long)isd-(unsigned long)ihd)>>9;
      isd=(long)+512*(no+1);
      continue;
    }
    secno--;
    isd=(unsigned long)isd+isd->isd$w_size;
  }
  return isd->isd$v_vpn<<12;
}

int find_symtab(void * abfd, void * symtab, long symstr, char * s) {
  while (1) {
    Elf32_External_Sym * src = symtab;
    if (src->st_name[0]==0 && src->st_name[1]==0) break;
    asymbol dst;
    nisse_swap_symbol_in(abfd, src, 0, &dst);
    symtab += sizeof(Elf32_External_Sym);
    if (0==strcmp(symstr+dst.name,s/*,strlen(routine)*/)) {
      return src;
    }
    src++;
  }
  return 0;
}

void *
find_it(void * sh, char * s, void ** retiaf ) {
  struct _shl * shlst=sh;
  int i;
  for(i=0;shlst[i].shl$t_imgnam[0];i++) {
    struct _iaf * iaf = ctl$gl_fixuplnk;
    char * imgnam=&shlst[i].shl$t_imgnam[0];
    for(;iaf;iaf=iaf->iaf$l_fixuplnk) {
      struct _ihd * ihd=iaf->iaf$l_iaflink;
      struct _ihi * ihi=(unsigned long)ihd+ihd->ihd$w_imgidoff;
      struct _ihs * ihs=(unsigned long)ihd+ihd->ihd$w_symdbgoff;

      int symtab = (long)iaf->iaf$l_permctx+0xf000+(ihs->ihs$l_dstvbn<<9); // check
      char * symtabstr = (long)iaf->iaf$l_permctx+0xf000+(ihs->ihs$l_dmtvbn<<9);
      void *src;
      src=find_symtab(0,symtab,symtabstr,s);
      if (src==0)
	continue;
      asymbol dst;
      nisse_swap_symbol_in(0, src, 0, &dst);
      if (dst.section==255)
	continue;
      *retiaf=iaf;
      return src;
    }
  }
  return 0;
}

asmlinkage int exe$imgfix(void) {
#ifdef CONFIG_VMS

  char buf[512];

  struct _ihd * ehdr32 = buf;

  struct _iaf * iaf = ctl$gl_fixuplnk;

  for(;iaf;iaf=iaf->iaf$l_fixuplnk) {
    int fixup_vector = iaf;

    long * g_fix = fixup_vector + iaf->iaf$l_g_fixoff;
    long * dotad = fixup_vector + iaf->iaf$l_dotadroff;
    long * shlp = fixup_vector + iaf->iaf$l_shlstoff;
    int shrimgcnt = iaf->iaf$l_shrimgcnt;
    
    struct _shl * shlst = shlp;
    int * addr;
    
    int shri;

#if 0
    for (shri=0;shri<1/*shrimgcnt*/;shri++) { /* not yet?  */ }
#endif

    int mainbase=shlst[shri].shl$l_baseva;

    int * addr_g;
    addr_g = fixup_vector+iaf->iaf$l_g_fixoff;
    int * addr_d;
    addr_d = fixup_vector+iaf->iaf$l_dotadroff;

    addr=addr_g;
    int again=2;
    while (again) {
      int refc=*addr++;
      int shli=*addr++;
#if 0
      // not  yet
      long base=shlst[shli].shl$l_baseva;
#endif
#if 0
      // not  yet
      while (refc--) {}
	*addr+=mainbase;
	addr++;
#else
      Elf32_External_Rela * adr = addr;
      if (refc==0)
	goto norefc;
      while (refc--) {
	int * tmp = &adr->r_info;
	char type=(*tmp)&0xff;
	int symo=(*tmp)>>8;
	tmp=&adr->r_offset;
	Elf32_External_Sym * p;
	struct _ihd * ihd=iaf->iaf$l_iaflink;
	struct _ihs * ihs=(unsigned long)ihd+ihd->ihd$w_symdbgoff;
	int symtab = (long)iaf->iaf$l_permctx+0xf000+(ihs->ihs$l_dstvbn<<9); // check
	char * symtabstr = (long)iaf->iaf$l_permctx+0xf000+(ihs->ihs$l_dmtvbn<<9);
	void *src;
	if (symo)
	  p=find_symtab_offset(symtab, symo, &src);
	if (p==-1)
	  printk("problem finding %x %x\n",symtab, symo);
	if (p==-1)
	  goto skip_me;

	asymbol dst;
	nisse_swap_symbol_in(0, src, 0, &dst);
	
	struct _iaf * iaf2=iaf;
	int base2=0;
	int base3=0;
	int sec3=dst.section;
	if (dst.section==255) {
	  src=find_it((long)iaf+iaf->iaf$l_shlstoff,symtabstr+symo, &iaf2);
	  if (src==0)
	    printk("problem2 finding %x %s\n",(long)iaf->iaf$l_shlstoff,symtabstr+symo);
	  if (src==0)
	    goto skip_me;
	  nisse_swap_symbol_in(0, src, 0, &dst);
	  sec3=dst.section;
	}

	base2=find_section_offset(iaf->iaf$l_iaflink,shli);
	base3=find_section_offset(iaf2->iaf$l_iaflink,sec3);

	int base4=iaf->iaf$l_permctx;
	int base5=iaf2->iaf$l_permctx;

	int offset = *tmp;
	long * myaddr = base4 + base2 + /* not yet base*/ + offset;

#if 0
	printk("rel %x %x %x %x %x %x %x %x %s\n",type,shli/* not yet base */,base2,base3,base4,base5,p,offset,symtabstr+symo);
#endif
	// check. plus section base or something else?
	if (type==1)
	  *myaddr=base5+base3+(int)p; //check
	if (type==2)
	  *myaddr=base5+base3+(int)p-(int)myaddr-4; //check
#if 0
	printk("rel2 %x %x\n",myaddr,*myaddr);
#endif
      skip_me:
	adr++;
      }
      addr=adr;
      if (*addr==0)
	addr+=2;
      norefc:
      if (*addr==0) {
	addr+=2;
	again--;
      }
    }
#endif

#if 0
    // not necessary yet
    if (dotad) {
      addr=0x3e000000 + dotad;
      while (*addr) {
	int refc=*addr++;
	int shli=*addr++;
	long base=shlst[shli].shl$l_baseva;
#if 0
	//not yet 
	while (refc--) {
	  int * dest = mainbase + *addr++;
	  *dest+=base;
	}
#else
	Elf32_External_Rela * adr = addr;
	while (refc--) {
	}
#endif
      }
    }
#endif
  }

  ctl$gl_iaflnkptr=0;
  ctl$gl_fixuplnk=0;
#endif
  return SS$_NORMAL;   
}

/*  LocalWords:  isd iaf
 */
