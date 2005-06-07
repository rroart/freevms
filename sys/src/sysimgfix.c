// $Id$
// $Locker$

// Author. Roar Thronæs.

#include<linux/linkage.h>
#if 0
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
#endif

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
  const Elf32_External_Sym *src = psrc;
  //  int signed_vma = get_nisse_backend_data (abfd)->sign_extend_vma;
  int signed_vma = 0; // check. added this to be sure.
  dst->name = H_GET_32 (abfd, src->st_name);
  if (signed_vma)
    dst->value = H_GET_SIGNED_WORD (abfd, src->st_value);
  else
    dst->value = H_GET_WORD (abfd, src->st_value);
  dst->flags = H_GET_WORD (abfd, src->st_size);
}

int
find_symtab_offset(void * symtab, long offset) {
  while (1) {
    Elf32_External_Sym * src = symtab;
    if (src->st_name[0]==0 && src->st_name[1]==0) break;
    asymbol dst;
    nisse_swap_symbol_in(0, src, 0, &dst);
    symtab += sizeof(Elf32_External_Sym);
    if (offset==dst.name)
      return dst.value;
    src++;
  }
  return -1;
}

asmlinkage void exe$imgfix(void) {
#ifdef CONFIG_VMS

  char buf[512];

  struct _ihd * ehdr32 = buf;

  struct _iaf * iaf = 0x3e000000;
  int fixup_vector = 0x3e000000;

  long * g_fix = 0x3e000000 + iaf->iaf$l_g_fixoff;
  long * dotad = 0x3e000000 + iaf->iaf$l_dotadroff;
  long * shlp = 0x3e000000 + iaf->iaf$l_shlstoff;
  int shrimgcnt = iaf->iaf$l_shrimgcnt;

  struct _shl * shlst = shlp;
  int * addr;

  int shri;

  for (shri=0;shri<1/*shrimgcnt*/;shri++) {

    int mainbase=shlst[shri].shl$l_baseva;

    int * addr_g;
    addr_g = fixup_vector+iaf->iaf$l_g_fixoff;

    int * addr;
    addr=addr_g;
    int again=2;
    while (again) {
      int refc=*addr++;
      int shli=*addr++;
      long base=shlst[shli].shl$l_baseva;
#if 0
      // not  yet
      while (refc--) {
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
	int symtab = 0; // check
	if (symo)
	  p=find_symtab_offset(symtab, symo);
	if (p==-1)
	  printk("problem finding %x %X\n",symtab, symo);
	if (p==-1)
	  goto skip_me;

	int offset = *tmp;
	long * myaddr = base + offset;
	
	if (type==1)
	  *myaddr=base+p; //check
	if (type==2)
	  *myaddr=p; //check
      skip_me:
	adr++;
      }
      addr=adr;
      norefc:
      if (*addr==0)
	addr+=2;
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

#endif
}
