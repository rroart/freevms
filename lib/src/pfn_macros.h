#ifndef pfn_macros_h
#define pfn_macros_h
#include <far_pointers.h>
#include <ints.h>
#include <mmg_routines.h>
#include <pfndef.h>
#include <pmmdef.h>
#include <ptedef.h>
#include <vms_macros.h>
#include <ssdef.h>
static inline void access_backpointer (PFN_PQ entry, PTE_PPQ va_pte, int map)
{
  extern PTE_PQ const
    mmg$gq_gpt_base,
    mmg$gq_pt_base,
    mmg$gq_s0s1base_pte_address;
  extern VOID_PQ const
    mmg$gq_window_va;
  extern INT_PQ const
    mmg$gq_window_pte_pfn;
  extern const unsigned long long
    mmg$gq_bwp_mask;
  unsigned long long
    pte_index_mask = 0x0000ffff;
  pte_index_mask <<= 48;

  if (entry->pfn$l_pt_pfn == 0)
  {
    *va_pte = (PTE_PQ) 0;
    return;
  }
  switch (entry->pfn$v_pagtyp)
  {
    case PFN$C_PROCESS :
    case PFN$C_PPGTBL  :

      if (entry->pfn$v_slot == 1)
      {
        *va_pte = &(mmg$gq_pt_base [(entry->pfn$q_pte_index & ~pte_index_mask)]);
        if (map == 1)
        {
          if (*va_pte >= mmg$gq_s0s1base_pte_address)
            *va_pte = svapte_to_va_pte ((PTE *) *va_pte);
        }
        return;
      }

      if (map == 0)
        *va_pte = &(mmg$gq_pt_base [(entry->pfn$q_pte_index & ~pte_index_mask)]);
      else
      {
        *mmg$gq_window_pte_pfn = entry->pfn$l_pt_pfn;
        tbi_data_64 (mmg$gq_window_va, THIS_CPU_ONLY, NO_PCB);
        *va_pte = (PTE_PQ) ((unsigned long long) mmg$gq_window_va +
                  (( (entry->pfn$q_pte_index & ~pte_index_mask ) << PTE$C_SHIFT_SIZE) &
                  mmg$gq_bwp_mask));
      }
      break;
    case PFN$C_GPGTBL  :
    case PFN$C_SYSTEM  :
      *va_pte = &(mmg$gq_pt_base [(entry->pfn$q_pte_index & ~pte_index_mask)]);
      if (map == 1)
      {
        if (*va_pte >= mmg$gq_s0s1base_pte_address)
          *va_pte = va_pte_to_svapte (*va_pte);
      }
      break;
    case PFN$C_GLOBAL  :
    case PFN$C_GBLWRT  :
      *va_pte = &(mmg$gq_gpt_base [(entry->pfn$q_pte_index & ~pte_index_mask)]);
      break;
    case PFN$C_RESERVED:
    case PFN$C_UNKNOWN:
      if (entry->pfn$l_pt_pfn != 0)
        *va_pte = &(mmg$gq_pt_base [(entry->pfn$q_pte_index & ~pte_index_mask)]);
  }
}

static inline int allocpfn (PFN_PPQ entry, VOID_PQ va, unsigned long long vpn)
{
    extern const unsigned long long
	mmg$gq_vpn_to_va;
    if (!va && !vpn)
	return (mmg_std$alloc_pfn_64 (entry));
    if (va && !vpn)
	return (mmg$alloc_pfn_algnd_64 (
	    ((unsigned long long) va >> mmg$gq_vpn_to_va),
	    entry));
    if (!va && vpn)
	return (mmg$alloc_pfn_algnd_64 (vpn, entry));
	bug_check (INCONMMGST, FATAL, COLD);
	return SS$_BADPARAM;
}

static inline int alloc_zero_pfn (PFN_PPQ entry, VOID_PQ va, unsigned long long vpn)
{
    extern const unsigned long long
	mmg$gq_vpn_to_va;
    if (!va && !vpn)
	return (mmg_std$alloc_zero_pfn_64 (entry));
    if (va && !vpn)
	return (mmg$alloc_zero_algnd_64 (
	    ((unsigned long long) va >> mmg$gq_vpn_to_va),
	    entry));
    if (!va && vpn)
	return (mmg$alloc_zero_algnd_64 (vpn, entry));
	bug_check (INCONMMGST, FATAL, COLD);
	return SS$_BADPARAM;
}
#define decref(entry)                                                                              \
{                                                                                                  \
  if (((entry)->pfn$w_refcnt--) < 0)                                                               \
    bug_check (REFCNTNEG, FATAL, COLD);                                                            \
}
#define decshr(entry)                                                                              \
{                                                                                                  \
  if (((entry)->pfn$l_shrcnt--) < 0)                                                               \
    bug_check (SHRCNTNEG, FATAL, COLD);                                                            \
}

static inline void establish_backpointer (PFN_PQ entry, PTE_PQ va_pte, int svapte, int gpt, int pt_pfn)
{
  extern PTE_PQ
    mmg$gq_max_gpte; 
  extern PTE_PQ const
    mmg$gq_gpt_base,
    mmg$gq_pt_base,
    mmg$gq_s0s1base_pte_address,
    mmg$gq_sptbase;
  extern const unsigned long long
    mmg$gq_level_width,
    mmg$gq_non_pt_mask;
  extern VOID_PQ const
    mmg$gq_system_virtual_base;
  PTE_PQ
    page_table_base,
    temp;
  unsigned long long
    index,
    pte_index_mask = 0x0000FFFF;
  pte_index_mask <<= 48;
  if (svapte == 1 && gpt == 1)
    bug_check (INCONMMGST, FATAL, COLD);

  if (va_pte == (PTE_PQ) 0)
  {
    entry->pfn$l_pt_pfn = 0;
    return;
  }

  if (gpt == 1)
  {

    if (va_pte < mmg$gq_gpt_base ||
        va_pte >= mmg$gq_max_gpte)
      bug_check (INCONMMGST, FATAL, COLD);

    index = ((unsigned long long) va_pte - (unsigned long long) mmg$gq_gpt_base) >> PTE$C_SHIFT_SIZE;
    entry->pfn$q_pte_index = (entry->pfn$q_pte_index & pte_index_mask) | index;
    temp = pte_va ((VOID_PQ) va_pte);
    entry->pfn$l_pt_pfn = temp->pte$v_pfn;
    return;
  }
  if (svapte == 1)
  {
    PTE_PQ
      converted_va_pte;

    if (va_pte < mmg$gq_sptbase)
      bug_check (INCONMMGST, FATAL, COLD);
    converted_va_pte = svapte_to_va_pte ((PTE *) va_pte);

    index = ((unsigned long long) converted_va_pte - (unsigned long long) mmg$gq_pt_base) >> PTE$C_SHIFT_SIZE;
    entry->pfn$q_pte_index = (entry->pfn$q_pte_index & pte_index_mask) | index;
    temp = pte_va ((VOID_PQ) converted_va_pte);
    entry->pfn$l_pt_pfn = temp->pte$v_pfn;
    return;
  }
  if (va_pte < mmg$gq_pt_base || va_pte >= (PTE_PQ) mmg$gq_system_virtual_base)
    bug_check (INCONMMGST, FATAL, COLD);
  index = ((unsigned long long) va_pte - (unsigned long long) mmg$gq_pt_base) >> PTE$C_SHIFT_SIZE;
  entry->pfn$q_pte_index = (entry->pfn$q_pte_index & pte_index_mask) | index;
  temp = pte_va (va_pte);
  if (pt_pfn == 0)
    entry->pfn$l_pt_pfn = temp->pte$v_pfn;
  else
    entry->pfn$l_pt_pfn = pt_pfn;
}
#define	incref(entry) ((entry)->pfn$w_refcnt++)

static inline PFN_PQ pfn_to_entry (uint32 pfn)
{
  extern PFN_PQ const
    pfn$pq_database;
  return (&(pfn$pq_database [(unsigned long long)pfn]));
}
#define test_backpointer(entry) ((entry)->pfn$l_pt_pfn != 0)
#ifdef NEW_STARLET

static inline int valid_pfn (unsigned int pfn)
{
    extern const unsigned int
	mmg$gl_maxpfn;
    extern int
	mmg$gl_pfn_memory_map [2],
	*mmg$gl_syi_pfn_memory_map;
    int
	i;
    PMM
	*memory_map;
    if (pfn > mmg$gl_maxpfn)
	return 0;
    if (mmg$gl_pfn_memory_map [0] == 0) return 1;
    memory_map = (PMM *)&mmg$gl_syi_pfn_memory_map[1];
    for (i = 0; i < mmg$gl_syi_pfn_memory_map [0]; ++i)
	if ((pfn >= memory_map[i].pmm$l_start_pfn) &&
	    (pfn < memory_map[i].pmm$l_start_pfn + 
					memory_map[i].pmm$l_pfn_count) &&
	    (memory_map[i].pmm$v_console | memory_map[i].pmm$v_openvms))
	    return 1;
    return 0;
}
#endif 
#endif 
