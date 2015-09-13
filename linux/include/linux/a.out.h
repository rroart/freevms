#ifndef __A_OUT_GNU_H__
#define __A_OUT_GNU_H__

#define __GNU_EXEC_MACROS__

#include <asm/a.out.h>

#if !defined (N_MAGIC)
#define N_MAGIC(exec) ((exec).a_info & 0xffff)
#endif

/* Code indicating object file or impure executable.  */
#define OMAGIC 0407
/* Code indicating pure executable.  */
#define NMAGIC 0410
/* Code indicating demand-paged executable.  */
#define ZMAGIC 0413
/* This indicates a demand-paged executable with the header in the text.
   The first page is unmapped to help trap NULL pointer references */
#define QMAGIC 0314

/* Code indicating core file.  */
#define CMAGIC 0421

#if !defined (N_BADMAG)
#define N_BADMAG(x)   (N_MAGIC(x) != OMAGIC     \
            && N_MAGIC(x) != NMAGIC     \
            && N_MAGIC(x) != ZMAGIC \
                && N_MAGIC(x) != QMAGIC)
#endif

#define _N_HDROFF(x) (1024 - sizeof (struct exec))

#if !defined (N_TXTOFF)
#define N_TXTOFF(x) \
 (N_MAGIC(x) == ZMAGIC ? _N_HDROFF((x)) + sizeof (struct exec) : \
  (N_MAGIC(x) == QMAGIC ? 0 : sizeof (struct exec)))
#endif

#if !defined (N_DATOFF)
#define N_DATOFF(x) (N_TXTOFF(x) + (x).a_text)
#endif

#if !defined (N_TRELOFF)
#define N_TRELOFF(x) (N_DATOFF(x) + (x).a_data)
#endif

#if !defined (N_DRELOFF)
#define N_DRELOFF(x) (N_TRELOFF(x) + N_TRSIZE(x))
#endif

#if !defined (N_SYMOFF)
#define N_SYMOFF(x) (N_DRELOFF(x) + N_DRSIZE(x))
#endif

#if !defined (N_STROFF)
#define N_STROFF(x) (N_SYMOFF(x) + N_SYMSIZE(x))
#endif

/* Address of text segment in memory after it is loaded.  */
#if !defined (N_TXTADDR)
#define N_TXTADDR(x) (N_MAGIC(x) == QMAGIC ? PAGE_SIZE : 0)
#endif

#include <asm/page.h>
#if defined(__i386__)
#define SEGMENT_SIZE    1024
#else
#ifndef SEGMENT_SIZE
#define SEGMENT_SIZE    PAGE_SIZE
#endif
#endif

#define _N_SEGMENT_ROUND(x) (((x) + SEGMENT_SIZE - 1) & ~(SEGMENT_SIZE - 1))

#define _N_TXTENDADDR(x) (N_TXTADDR(x)+(x).a_text)

#ifndef N_DATADDR
#define N_DATADDR(x) \
    (N_MAGIC(x)==OMAGIC? (_N_TXTENDADDR(x)) \
     : (_N_SEGMENT_ROUND (_N_TXTENDADDR(x))))
#endif

/* Address of bss segment in memory after it is loaded.  */
#if !defined (N_BSSADDR)
#define N_BSSADDR(x) (N_DATADDR(x) + (x).a_data)
#endif

#if !defined (N_NLIST_DECLARED)
struct nlist
{
    union
    {
        char *n_name;
        struct nlist *n_next;
        long n_strx;
    } n_un;
    unsigned char n_type;
    char n_other;
    short n_desc;
    unsigned long n_value;
};
#endif /* no N_NLIST_DECLARED.  */

#if !defined (N_UNDF)
#define N_UNDF 0
#endif
#if !defined (N_ABS)
#define N_ABS 2
#endif
#if !defined (N_TEXT)
#define N_TEXT 4
#endif
#if !defined (N_DATA)
#define N_DATA 6
#endif
#if !defined (N_BSS)
#define N_BSS 8
#endif
#if !defined (N_FN)
#define N_FN 15
#endif

#if !defined (N_EXT)
#define N_EXT 1
#endif
#if !defined (N_TYPE)
#define N_TYPE 036
#endif
#if !defined (N_STAB)
#define N_STAB 0340
#endif

/* The following type indicates the definition of a symbol as being
   an indirect reference to another symbol.  The other symbol
   appears as an undefined reference, immediately following this symbol.

   Indirection is asymmetrical.  The other symbol's value will be used
   to satisfy requests for the indirect symbol, but not vice versa.
   If the other symbol does not have a definition, libraries will
   be searched to find a definition.  */
#define N_INDR 0xa

/* The following symbols refer to set elements.
   All the N_SET[ATDB] symbols with the same name form one set.
   Space is allocated for the set in the text section, and each set
   element's value is stored into one word of the space.
   The first word of the space is the length of the set (number of elements).

   The address of the set is made into an N_SETV symbol
   whose name is the same as the name of the set.
   This symbol acts like a N_DATA global symbol
   in that it can satisfy undefined external references.  */

/* These appear as input to LD, in a .o file.  */
#define N_SETA  0x14        /* Absolute set element symbol */
#define N_SETT  0x16        /* Text set element symbol */
#define N_SETD  0x18        /* Data set element symbol */
#define N_SETB  0x1A        /* Bss set element symbol */

/* This is output from LD.  */
#define N_SETV  0x1C        /* Pointer to set vector in data area.  */

#endif /* __A_OUT_GNU_H__ */
