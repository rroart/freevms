#ifndef pte_functions_h
#define pte_functions_h

#include <far_pointers.h>
//#include <hwrpbdef.h>
//#include <ints.h>
#include <ptedef.h>
#include <ssdef.h>

static inline void $read_pte (PTE_PQ pte_address, PTE_PQ contents_p)
{
  extern PTE mmg$read_l1pte (PTE_PQ pte_address);
  extern PTE_PQ const mmg$gq_l1_base;
  extern unsigned long long const mmg$gq_page_size;
  extern unsigned long long const mmg$gq_bwp_width;
  extern unsigned long long const mmg$gq_bwp_mask;
  extern HWRPB * exe$gpq_hwrpb;
  if ((exe$gpq_hwrpb->hwrpb$v_virbnd == 0) ||
      ((unsigned long long)pte_address < (unsigned long long)mmg$gq_l1_base) ||	    
      ((unsigned long long)pte_address >= (unsigned long long)mmg$gq_l1_base+mmg$gq_page_size))
    {
      *contents_p = *pte_address;
      return;
    }
  *contents_p = mmg$read_l1pte(pte_address);
  return;
}

static inline void $write_pte (PTE_PQ pte_address, PTE contents)
{
  extern unsigned long long mmg$write_l1pte (PTE_PQ pte_address, PTE contents);
  extern PTE_PQ const mmg$gq_l1_base;
  extern unsigned long long const mmg$gq_page_size;
  extern unsigned long long const mmg$gq_bwp_width;
  extern unsigned long long const mmg$gq_bwp_mask;
  extern HWRPB * exe$gpq_hwrpb;
  if ((exe$gpq_hwrpb->hwrpb$v_virbnd == 0) ||
      ((unsigned long long)pte_address < (unsigned long long)mmg$gq_l1_base) ||	    
      ((unsigned long long)pte_address >= (unsigned long long)mmg$gq_l1_base+mmg$gq_page_size))
    {
      *pte_address = contents;
      return;
    }
  mmg$write_l1pte(pte_address,contents);	
  return;
}

#endif
